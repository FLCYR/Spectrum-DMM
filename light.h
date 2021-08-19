#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include"avaspec.h"


class Light:public QObject
{
    Q_OBJECT
public:
    Light();
    ~Light();

    QString type;
    QString serialNumber;

    void spectrumInit();

    void setSpectrumParameter(int integrationTime,int triggerMode);
    static void callBackMeansureFun(AvsHandle*_handle,int*result);
    bool getSpectrumStatus();//返回光谱仪状态
    void startMeansure(int interval,int integrationTime,int triggerMode);//开启定时器
    void stopMeansure();//停止采集
    void exitMeansure();//退出测量

    void saveData(); //保存数据
    void collectNoise(double * arr); //噪声采集
    bool getWaveLength(double*&waveArr,int &pixCount);//获取波长数组

signals:
    void readInfo(int status);
    void paintPlot(double*x,double*y,unsigned short count);//发送绘图表的信号

public slots:
    void readData(int status);
    void doMeansure();


private:
   static Light*lightPtr;
   AvsHandle handle;//设备句柄
   MeasConfigType devicConfig;
   unsigned short pixelCount;//像素个数
   double * waveLengthArr;//存放波长数组
   double *lightAdcArr;//光强

   QTimer * meansureTimer;
   bool isConnOk; //连接状态
};

#endif // LIGHT_H
