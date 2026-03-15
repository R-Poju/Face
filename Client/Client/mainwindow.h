#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QTimer>
#include <QTcpSocket>

#include <opencv.hpp>
#include <objdetect.hpp>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    //图像的采集与显示
    void updateCamera();

    //人脸数据识别
    void faceFrame(Mat& srcImage);

    //将数据发送到服务端
    void sendFaceData(const Mat& srcImage);

    //接收来自服务器的数据
    void recvFaceData();

private slots:
    void timer_connect();   //当计时器启动后，每隔五秒尝试连接一次
    void stop_connect();    //当连接成功后，停止计时器
    void start_connect();   //断开连接时，启动计时器


private:
    Ui::MainWindow *ui;

    //摄像头
    VideoCapture m_cap;
    //haar级联分类器
    cv::CascadeClassifier m_cascade;


    QTcpSocket m_socket;
    QTimer m_timer;


    //用于标识是否是同一个人脸进入到识别区域，避免资源浪费
    int m_faceCheckFlag = 0;
    //保存服务器回传的人脸图像
    cv::Mat m_faceMat;
};
#endif // MAINWINDOW_H

































