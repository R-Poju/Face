#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    /* ==================== 1. UI 界面初始化 ==================== */
    ui->setupUi(this);
    setWindowTitle("人脸识别智能考勤系统 - 服务端");
    ui->logs->setReadOnly(true);
    ui->logs->setWordWrapMode(QTextOption::NoWrap); // 禁用自动换行

    /* ==================== 2. 成员变量初始化 ==================== */
    m_byteSize = 0;
    m_logger = new Logger(this);

    /* ==================== 3. 开始监听连接 ==================== */
    // 启动服务器，监听新连接
    m_server.listen(QHostAddress::Any, 9999);
    connect(&m_server, &QTcpServer::newConnection, this, &MainWindow::acceptClient);
    qDebug() << "***** 服务器已启动，等待连接... *****\n";
    m_logger->log("***** 服务器已启动，等待连接... *****\n", Logger::Info);

    /* ==================== 4. 为数据库绑定表格 ==================== */
    m_model.setTable("employee");

    /* ==================== 5. 将 faceID 查询工作移交到子线程 ==================== */
    QThread *thread = new QThread(this);  // 创建线程
    // 把 QFace 对象移动到 thread 线程执行
    m_faceObj.moveToThread(thread);
    // 启动线程
    thread->start();
    // 连接槽函数，收到信号后，会在子线程中执行
    connect(this, &MainWindow::query, &m_faceObj, &QFace::faceQuery);
    // 子线程处理完毕后，发送 sendFaceID 信号，主线程接收
    connect(&m_faceObj, &QFace::sendFaceID, this, &MainWindow::recvFaceID);

    /* ==================== 6. 处理其他信号连接 ==================== */
    // 连接日志信号到UI更新槽函数
    connect(m_logger, &Logger::logPosted, this, &MainWindow::appendLogToUI, Qt::QueuedConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::acceptClient()
{
    // 获取与客户端通信的套接字
    m_socket = m_server.nextPendingConnection();

    // 当客户端有数据发送过来时，会发送 readyRead 信号
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readData);

}

void MainWindow::readData()
{
    /* ==================== 1. 接收客户端传来的人脸数据 ==================== */

    // 1. 把套接字绑定到数据流
    QDataStream stream(m_socket);
    stream.setVersion(QDataStream::Qt_5_14);

    // 2. 从流中读取数据
    if(m_byteSize == 0)
    {
        if (m_socket->bytesAvailable() < (qint64)sizeof(m_byteSize)) return;

        // 采集数据的长度
        stream >> m_byteSize;
    }

    if (m_socket->bytesAvailable() < (qint64)m_byteSize) // 说明数据还没有发送完成，继续等待
    {
        return;
    }

    QByteArray data;
    stream >> data;

    if(data.size() == 0) // 没有读到数据
    {
        return;
    }
    // 接收成功则重置，以备下一次接收
    m_byteSize = 0;

    qDebug() << "***** 数据接收成功 *****";
    m_logger->log("***** 数据接收成功 *****", Logger::Info);

    // 图片处理，水平翻转并显示
    QPixmap pix;
    pix.loadFromData(data, "jpg");
    if (!pix.isNull()) {
        // 将 QPixmap 转换为 QImage
        QImage img = pix.toImage();
        // 水平翻转图片
        img = img.mirrored(true, false);  // 第一个参数为 true 表示水平翻转，第二个参数为 false 表示不进行垂直翻转
        // 将翻转后的 QImage 转换回 QPixmap
        QPixmap flippedPix = QPixmap::fromImage(img);
        // 显示翻转后的图片
        if (ui->picLb)
        {
            ui->picLb->setPixmap(flippedPix.scaled(ui->picLb->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        }
    }

    /* ==================== 2. 识别人脸数据，返回 faceID ==================== */
    // 1. 将字节流数据转为 Mat 矩阵
    cv::Mat faceImage;
    std::vector<uchar> decode;
    decode.resize(data.size());
    memcpy(decode.data(), data.data(), data.size());
    faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);

    // 2. 调用人脸数据查询接口拿到 faceID
    emit query(faceImage);
}

void MainWindow::recvFaceID(int64_t faceID)
{
    QJsonObject msg;

    // 1. 如果 <0 则此次查询的结果不可信，返回空数据
    if (faceID < 0)
    {
        msg["employeeID"] = QJsonValue();
        msg["name"] = QJsonValue();
        msg["department"] = QJsonValue();
        msg["date"] = QJsonValue();
    }
    else
    {
       // 2. 根据 faceID 从数据中查询个人信息
        m_model.setFilter(QString("faceID=%1").arg(faceID)); // 为模型设置过滤器
        m_model.select(); // 查询
        if (m_model.rowCount() == 1) // 判断有没有查询到数据
        {
            // 工号，姓名，部门，时间
            QSqlRecord record = m_model.record(0);
            msg["employeeID"] = record.value("ID").toString();
            msg["name"] = record.value("name").toString();
            msg["department"] = "软件开发";
            msg["date"] = QDateTime::currentDateTime().toString("yy-MM-dd HH:mm:ss");

            // 3. 把数据写入考勤表
            QString insertSql = QString("INSERT INTO attendance(employeeID) VALUES('%1')").arg(record.value("ID").toString());
            QSqlQuery query;
            if (!query.exec(insertSql)) // 考勤数据插入失败，更改 json
            {
                msg["employeeID"] = QJsonValue();
                msg["name"] = QJsonValue();
                msg["department"] = QJsonValue();
                msg["date"] = QJsonValue();

                qDebug() << query.lastError().text(); // 打印错误信息
                m_logger->log(query.lastError().text(), Logger::Error);
            }
            // 考勤数据插入成功，无需更改 json (发送的时 faceID 查询成功时格式化好的 json)
        }
    }

    // 把结果发送给客户端
    // toJson() 方法 将 QJsonDocument 转换为 JSON 格式的 QByteArray（可以直接写入 socket）
    // QJsonDocument::Compact 参数表示 输出紧凑格式的 JSON（不包含换行和额外空格）
    m_socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
}

void MainWindow::appendLogToUI(const QString &msg, Logger::LogLevel level)
{
    if (!ui->logs) return;  // 确保日志窗口已初始化

    QTextCharFormat fmt;

    switch (level) {
    case Logger::Debug:
        fmt.setForeground(Qt::darkBlue);
        break;
    case Logger::Info:
        fmt.setForeground(Qt::darkGreen);
        break;
    case Logger::Warning:
        fmt.setForeground(Qt::darkYellow);
        break;
    case Logger::Error:
        fmt.setForeground(Qt::red);
        fmt.setFontWeight(QFont::Bold);
        break;
    default:
        break;
    }

    ui->logs->mergeCurrentCharFormat(fmt);
    ui->logs->appendPlainText(msg);

           // 自动滚动到底部
    QScrollBar *scrollBar = ui->logs->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
