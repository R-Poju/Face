#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QFileDialog>
#include "qface.h"
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>
#include <QMessageBox>

RegisterWindow::RegisterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    setWindowTitle("信息采集");

    ui->birthday->setDate(QDate::currentDate());
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::updateCamera()
{
    // 1. 获取摄像头数据并显示
    cv::Mat srcImage;
    if (m_cap.isOpened())
    {
        m_cap >> srcImage;
        m_cap >> m_saveImage;
        if (srcImage.empty()) return;
    }

    // 2. 水平翻转图像
    cv::flip(srcImage, srcImage, 1);
    cv::flip(m_saveImage, m_saveImage, 1);

    // 3. 转换格式并显示 Mat --> QImage
    cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, srcImage.step, QImage::Format_RGB888);

    ui->avatar->setPixmap(QPixmap::fromImage(image).scaled(ui->avatar->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void RegisterWindow::on_resetBtn_clicked()
{
    // 清空数据
    ui->nameEdit->clear();
    ui->birthday->setDate(QDate::currentDate());
    ui->addressEdit->clear();
    ui->phoneEdit->clear();
}


void RegisterWindow::on_addpicBtn_clicked()
{
    // 通过文件对话框获得图片路径
    QString filename = QFileDialog::getOpenFileName(this);
    if (filename == "")
    {
        return;
    }

    // 显示路径
    ui->picfileEdit->setText(filename);

    // 显示图片
    QPixmap pix(filename);
    pix = pix.scaledToWidth(ui->avatar->width());
    ui->avatar->setPixmap(pix);

}


void RegisterWindow::on_registerBttn_clicked()
{
    // 0. 首先判断信息是否填写完整了
    if (ui->nameEdit->text().isEmpty() || ui->addressEdit->text().isEmpty() || ui->phoneEdit->text().isEmpty()
        || (!ui->man->isChecked() && !ui->woman->isChecked()))
    {
        QMessageBox::warning(this, "注册", "请先填写个人信息再点击注册按钮");
        return;
    }

    // 1. 通过照片，结合 Face 模块得到 faceID
    if (ui->picfileEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "注册", "请先拍照或者选择一张照片");
        return;
    }

    QFace faceObj;
    cv::Mat image = cv::imread(ui->picfileEdit->text().toUtf8().data()); // todo 用异步或者多线程解决加载图片时间过长的问题
    if (image.empty())
    {
        QMessageBox::warning(this, "注册", "请先拍照或者选择一张照片");
        return;
    }

    int faceID = faceObj.faceRegister(image);
    qDebug() << "faceID: " << faceID;

    // 把头像保存到本地
    QString headFile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    cv::imwrite(headFile.toUtf8().data(), image);

    // 2. 把个人信息存储到 empleyee 数据表
    QSqlTableModel model;
    model.setTable("employee");
    QSqlRecord record = model.record();
    // 设置数据
    record.setValue("name", ui->nameEdit->text());
    record.setValue("sex", ui->man->isChecked() ? "man" : "woman");
    record.setValue("birthday", ui->birthday->text());
    record.setValue("address", ui->addressEdit->text());
    record.setValue("phone", ui->phoneEdit->text());
    record.setValue("faceID", faceID);
    record.setValue("headFile", headFile);


    // 3. 提示注册成功
    bool ret = model.insertRecord(-1, record);
    if (ret)
    {
        // 启动定时器，以待下次拍照
        if (m_timer_inited)
        {
            m_timer.start(100);
        }

        qDebug() << "注册成功";
        QMessageBox::information(this, "注册提示", "注册成功");
        model.submitAll();
    }
    else
    {
        qDebug() << "注册失败";
        QMessageBox::information(this, "注册提示", "注册失败");
    }

}


void RegisterWindow::on_cameraBtn_clicked()
{
    if (ui->cameraBtn->text() == "打开摄像头")
    {
        if (m_cap.open(0))
        {
            ui->cameraBtn->setText("关闭摄像头");

            connect(&m_timer, &QTimer::timeout, this, &RegisterWindow::updateCamera);
            m_timer.start(100);
            m_timer_inited = true;
        }
        else
        {
            qDebug() << "摄像头打开失败";
            return;
        }
    }
    else
    {
        ui->cameraBtn->setText("打开摄像头");

        disconnect(&m_timer, &QTimer::timeout, this, &RegisterWindow::updateCamera);
        m_timer.stop();

        // 关闭摄像头
        m_cap.release();
    }
}


void RegisterWindow::on_takephotoBtn_clicked() // todo 处理点击拍照时 按钮的显示
{
    if (!m_saveImage.empty())
    {
        // 把拍照照片保存到本地
            QString headFile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64())); // 生成文件要存储的路径
        ui->picfileEdit->setText(headFile); // 把路径显示到标签
        cv::imwrite(headFile.toUtf8().data(), m_saveImage);
        // 关闭定时器，停止刷新
        m_timer.stop();
    }
    else
    {
        QMessageBox::warning(this, "拍照", "请先打开摄像头");
        return;
    }
}

