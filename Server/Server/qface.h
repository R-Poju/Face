#ifndef QFACE_HPP
#define QFACE_HPP

#include <QObject>
#include <FaceEngine.h>
#include <opencv.hpp>
#include "logger.h"

using namespace seeta;

// 负责人脸数据存储，人脸检测，人脸识别
class QFace : public QObject
{
    Q_OBJECT
public:
    explicit QFace(QObject *parent = nullptr);
    ~QFace();

public slots:
    // 注册接口
    int64_t faceRegister(cv::Mat &faceImage);
    // 查询接口，return: 人脸数据在数据库中对应的编号
    int64_t faceQuery(cv::Mat &faceImage);

signals:
    void sendFaceID(int64_t faceID); // 接收子线程的运行结果

private:
    seeta::FaceEngine *m_faceEngineptr;
    Logger *m_logger;
};

#endif // QFACE_HPP
