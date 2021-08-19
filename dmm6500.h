#ifndef DMM6500_H
#define DMM6500_H

#include <QObject>
#include<QDebug>
#include<QTimer>
#include<visa.h>

#define MAX_READING 256
#define MAX_SIZE_CURR 64
class DMM6500:public QObject
{
    Q_OBJECT
public:
    DMM6500();
    ~DMM6500();
    void dmm6500Init();
    void singleMeansureCurrent();//读取一次电流
    void startTimerMeansure(int interval);//连续模式测试
    void stopTimerMeansure();//暂停定时测试
    void setRange(double range); //设置范围
    void pulseMeansure();//脉冲模式测量
    void pulseMeansureInit(int points,double freq,double range);//脉冲模式初始化
    void startPulseMeansure(int interval); //开始脉冲测量
    void stopPulseMeansure();//停止脉冲测量
    bool getDmmStatus();
    void saveData();// 保存数据

    void timeOutFun();//定时器函数
    void setCalibration(double calibration);//设置标定系数
    void setPointsAndInterval(int points,double interval);//设置采样点数和采样间隔

    void freeReadBuffer();//释放readbuffer内存
    QVector<double> getXVector();
    QVector<double> getYVector();

    void setMeanFun();
private:
    QVector<double>xVector;//存放电流x轴数据 时间间隔
    QVector<double>yVector;//存放电流y轴数据
    double calibration; //标定系数
    ViSession session;
    ViSession defaultRM;
    int type; //测量模式
    int points;//点数
    int samplePoints;//采样点数
    double sampleInterval;//采样间隔
    unsigned char reading[MAX_READING];
    QTimer*readTimer;
    QTimer*pulseTimer;// 脉冲模式 定时读取
    bool isconnOk;//是否连接成功
    uchar* readBuf;
signals:
    void sendCurrent(int type,double current);

};

#endif // DMM6500_H
