#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QTimer>
#include <QWidget>
#include "opencv.hpp"

namespace Ui
{
    class RegisterWindow;
}

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

    void updateCamera();

private slots:
    void on_resetBtn_clicked();

    void on_addpicBtn_clicked();

    void on_registerBttn_clicked();

    void on_cameraBtn_clicked();

    void on_takephotoBtn_clicked();

private:
    Ui::RegisterWindow *ui;
    cv::VideoCapture m_cap;
    QTimer m_timer;
    bool m_timer_inited = false; // 标志定时器是否被启用过
    cv::Mat m_saveImage; // 用来存储拍照得到的照片
};

#endif // REGISTERWINDOW_HPP
