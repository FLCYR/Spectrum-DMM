#include "light.h"
#include <QFileDialog>
#include<QMessageBox>
#include<QTextStream>
Light* Light::lightPtr=nullptr;
Light::Light()
{
    lightPtr=this;
    waveLengthArr=nullptr;
    lightAdcArr=nullptr;
    isConnOk=false;
    meansureTimer=new QTimer(this);
    connect(this,&Light::readInfo,this,&Light::readData);
    connect(meansureTimer,&QTimer::timeout,this,&Light::doMeansure);
    spectrumInit();
}

Light::~Light()
{

        //test
//    if(waveLengthArr!=nullptr)
//        delete  []waveLengthArr;
//    if(lightAdcArr!=nullptr)
//        delete  [] lightPtr;
    if(handle!=-1)
    {
         AVS_StopMeasure(handle);
         AVS_Done();
         meansureTimer->stop();
    }

}
//退出测试
void Light::exitMeansure()
{

    if(handle!=-1)
    {
        AVS_StopMeasure(handle);
        AVS_Deactivate(handle);
        AVS_Done();
    }
}
//光谱仪初始化
void Light::spectrumInit()
{
    //连接光谱仪
    isConnOk=false;
    int result=AVS_Init(0); //0 usb口
    if(result<1)
    {
        handle=-1;
        qDebug()<<"not found light";
        return;
    }
    AVS_ResetParameter(handle); //reset
    AvsIdentityType devicInfo;
    unsigned reqSize;
    AVS_GetList(sizeof(devicInfo),&reqSize,&devicInfo);

    qDebug()<<devicInfo.SerialNumber<<" "<<devicInfo.UserFriendlyName;
    type=devicInfo.UserFriendlyName;
    serialNumber=devicInfo.SerialNumber;
    handle=AVS_Activate(&devicInfo);
    AVS_GetNumPixels(handle,&pixelCount); //获取像素值个数

    waveLengthArr=new double[pixelCount];  //分配 波长数组空间

    lightAdcArr=new double[pixelCount];      //分配 存放 光强数据 内存


    //获取波长
    if(ERR_SUCCESS==AVS_GetLambda(handle,waveLengthArr))
    {
        qDebug()<<"pixel number=="<<pixelCount;
        qDebug()<<"success read wave length";
    }

    int res=AVS_UseHighResAdc(handle,true); //设置16位模式 最大值65535
    if(res==ERR_SUCCESS)
    {
        qDebug()<<"success set 16 bit model";
    }

    isConnOk=true;

}


//设置光谱仪参数
void Light::setSpectrumParameter(int integrationTime,int triggerMode)
{

    if(handle==-1)
    {
        qDebug()<<"spectrum failed";
        return;

    }
    devicConfig.m_StartPixel=0;//开始的像素值
    devicConfig.m_StopPixel=pixelCount-1;//结束的像素值
    devicConfig.m_IntegrationTime=integrationTime; //积分时间
    devicConfig.m_IntegrationDelay=0;//积分延迟时间
    devicConfig.m_NrAverages=1;// 取多少次数据的平均值作为该点数值

    devicConfig.m_CorDynDark.m_Enable=0;
    devicConfig.m_CorDynDark.m_ForgetPercentage=100; //recommend 100
    devicConfig.m_Smoothing.m_SmoothPix=0;
    devicConfig.m_Smoothing.m_SmoothModel=0;
    devicConfig.m_SaturationDetection=0;

    devicConfig.m_Trigger.m_Mode=triggerMode;
    devicConfig.m_Trigger.m_Source=0;
    devicConfig.m_Trigger.m_SourceType=0;

    devicConfig.m_Control.m_StrobeControl=0;
    devicConfig.m_Control.m_StrobeControl=0;
    devicConfig.m_Control.m_LaserDelay=0;
    devicConfig.m_Control.m_LaserWidth=0; //激光宽度
    devicConfig.m_Control.m_LaserWaveLength=0;
    devicConfig.m_Control.m_StoreToRam=0;

    //设置光谱仪参数
    AVS_PrepareMeasure(handle,&devicConfig);
}
//定时器 调用函数
void Light::doMeansure()
{
  AVS_MeasureCallback(handle,callBackMeansureFun,1);
}
//测量的回调函数
void Light::callBackMeansureFun(AvsHandle *_handle, int *result)
{

    qDebug()<<"result="<<*result;
    emit lightPtr->readInfo(*result);

}
//读取光强
void Light::readData(int status)
{
    unsigned timeLabel;
    if(status==0)
    {
        qDebug()<<"read-data..................";
        AVS_GetScopeData(handle,&timeLabel,lightAdcArr);
        emit paintPlot(waveLengthArr,lightAdcArr,pixelCount);
    }
}
//开始定时器
void Light::startMeansure(int interval,int integrationTime,int triggerMode)
{
    if(!meansureTimer->isActive())
    {
        //设置光谱仪测试参数
        qDebug()<<"start light test";
        setSpectrumParameter(integrationTime,triggerMode);
        meansureTimer->start(interval);
    }
}
//停止定时器
void Light::stopMeansure()
{
    meansureTimer->stop();
}
//返回是否连成功
bool Light::getSpectrumStatus()
{
    return isConnOk;
}

//获取波长和像素个数
bool Light::getWaveLength(double *&waveArr,int & pixCount)
{
    if(waveLengthArr&&pixelCount)
    {
        waveArr=waveLengthArr;
        pixCount=pixelCount;
        return true;
    }
    return false;
}
//噪声采集
void Light::collectNoise(double * arr)
{
    unsigned int timeLabel;
    AVS_Measure(handle,NULL,1);
    while(!AVS_PollScan(handle))
        ;
    AVS_GetScopeData(handle,&timeLabel,arr);
}


//保存数据
void Light::saveData()
{
    QString fileName=QFileDialog::getSaveFileName(nullptr,"保存光谱数据","./","text(*.txt)");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(NULL,"ERROR","保存失败");
        return;
    }

    QTextStream out(&file);

    if(waveLengthArr)
    {
        for(int i=0;i<pixelCount;i++)
        {
            out<<waveLengthArr[i]<<" "<<lightAdcArr[i]<<"\n";
        }
    }
    file.close();

}















