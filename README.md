# 人脸识别考勤系统

基于 **Qt5 + OpenCV + SeetaFace2** 实现的高效、稳定的人脸识别智能考勤系统，支持本地数据库阅览、人脸注册与识别等功能，适用于学校、公司等场景。



采用MVC模式分层开发：UI层、业务逻辑层、数据层，解耦模块提升可维护性 

基于OpenCV VideoCapture 实现摄像头视频流采集，级联分类器检测人脸区域，结合SeetaFace提取128维特征向量 

实现Logger日志系统，支持Debug/Info/Warning/Error 多级别记录，通过信号槽异步输出至UI与文件，追踪摄像头状 态、识别成功率、网络异常等关键事件 

设计多线程通信机制,通过注册cv::Mat实现摄像头采集、人脸处理与UI更新的异步协同，避免界面卡顿 

从人脸检测（准确率＞95%）、特征匹配到考勤记录入库，单帧处理耗时＜200ms，满足实时需求 

数据序列化，确保跨平台兼容性 



![如图所示](https://gitee.com/R-Poju/Images/raw/master/Face/01.png)





![](https://gitee.com/R-Poju/Images/raw/master/Face/02.png)



![](https://gitee.com/R-Poju/Images/raw/master/Face/03.png)

