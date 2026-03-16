#include "qface.h"
#include <QDebug>


QFace::QFace(QObject *parent) :
    QObject{parent}
{
    // 初始化成员
    m_logger = new Logger(this);

    seeta::ModelSetting FD_model("D:/FutureSrcInc/SeetaFace/SeetaFace2/bin/model/fd_2_00.dat", seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting PD_model("D:/FutureSrcInc/SeetaFace/SeetaFace2/bin/model/pd_2_00_pts5.dat", seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting FR_model("D:/FutureSrcInc/SeetaFace/SeetaFace2/bin/model/fr_2_10.dat", seeta::ModelSetting::GPU, 0);
    m_faceEngineptr = new seeta::FaceEngine(FD_model, PD_model, FR_model);

    // 导入已有的人脸数据库
    m_faceEngineptr->Load("./face.db");
}

QFace::~QFace()
{
    delete m_faceEngineptr;
}

int64_t QFace::faceRegister(cv::Mat &faceImage)
{
    // 将 opencv Mat数据转为 seetaImageData 类型
    SeetaImageData seetaImage;
    seetaImage.data = faceImage.data;
    seetaImage.width = faceImage.cols;
    seetaImage.height = faceImage.rows;
    seetaImage.channels = faceImage.channels();

    // 调用注册函数，返回一个人脸 id
    int64_t id = m_faceEngineptr->Register(seetaImage);

    if (id >= 0)
    {
        m_faceEngineptr->Save("./face.db");
    }

    return id;
}

int64_t QFace::faceQuery(cv::Mat &faceImage)
{
    qDebug() << "\n========== 查询人脸数据开始 ==========";
    m_logger->log("========== 查询人脸数据开始 ==========", Logger::Info);
    // 将 opencv Mat数据转为 seetaImageData 类型
    SeetaImageData seetaImage;
    seetaImage.data = faceImage.data;
    seetaImage.width = faceImage.cols;
    seetaImage.height = faceImage.rows;
    seetaImage.channels = faceImage.channels();

    float similarity = 0; // 查询结果的可信度
    int64_t id = m_faceEngineptr->Query(seetaImage, &similarity); // 运行时间较长

    if (similarity > 0.7)
    {
        // 将结果返回给主线程
        emit sendFaceID(id);
    }
    else
    {
        // id = -1;
        emit sendFaceID(id);
    }

    qDebug() << "可信度: " << similarity;
    qDebug() << "faceID: " << id;
    qDebug() << "========== 查询人脸数据结束 ==========\n";

    m_logger->log("可信度: " + QString::number(similarity), Logger::Info);
    m_logger->log("faceID: " + QString::number(id), Logger::Info);
    m_logger->log("========== 查询人脸数据结束 ==========", Logger::Info);

    return id;
}
