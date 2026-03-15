#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "mainwindow.h"
#include "Logger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 将需要使用信号传输的自定义数据格式进行注册
    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");

    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // 设置数据库名
    db.setDatabaseName("server.db");
    // 打开数据库
    if (!db.open())
    {
        qDebug() << db.lastError().text();
        return -1;
    }
    // 创建 员工信息数据表
    QString createTable = "create table if not exists employee(ID integer primary key autoincrement, name varchar(256), sex varchar(32),"
                          "birthday text, address text, phone text, faceID integer unique, headFile text)";
    QSqlQuery query;
    if (!query.exec(createTable))
    {
        qDebug() << query.lastError().text();
        return -1;
    }
    // 创建 考勤数据表
    createTable = "create table if not exists attendance(attendanceID integer primary key autoincrement, employeeID integer,"
                          "attendanceTime TimeStamp NOT NULL DEFAULT(datetime('now', 'localtime')))";
    if (!query.exec(createTable))
    {
        qDebug() << query.lastError().text();
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
