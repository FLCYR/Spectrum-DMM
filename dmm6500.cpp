#include "dmm6500.h"
#include"otherFun.h"
#include"variable.h"
#include<QMessageBox>
#include<QFile>
#include<QFileDialog>
#include<QTextStream>
DMM6500::DMM6500()
{
    readTimer=new QTimer(this);
    pulseTimer=new QTimer(this);
    isconnOk=false;
    calibration=0;
    session=0;
    type=1;
    points=0;
    readBuf=NULL;
    connect(readTimer,&QTimer::timeout,this,&DMM6500::timeOutFun);
    connect(pulseTimer,&QTimer::timeout,this,&DMM6500::pulseMeansure);
    dmm6500Init();
}
DMM6500::~DMM6500()
{
   if(session!=0)
   {
       viClose(session);
       viClose(defaultRM);
   }
   readTimer->stop();
}

void DMM6500::setCalibration(double calibration)
{
    this->calibration=calibration;
}
//6500初始化
void DMM6500::dmm6500Init()
{

    ViStatus status;
    ViUInt32 act;
    unsigned char buf[256];
    char resName[]="USB0::0x05e6::0x6500::04502527::INSTR";
    status=viOpenDefaultRM(&defaultRM);
    if(status<VI_SUCCESS)
    {
        qDebug()<<"VISA INIT FALIED";
        return;
    }

    status=viOpen(defaultRM,resName,VI_NULL,VI_NULL,&session);
    if(status<VI_SUCCESS)
    {
        qDebug()<<"open Devic Failed";
        return;
    }
    ViPBuf orderC=(ViPBuf)"*CLS\n";
    ViPBuf orderS=(ViPBuf)"*RST\n";
    viWrite(session,orderC,strlen(orderC),NULL);
    viWrite(session,orderS,strlen(orderS),NULL);
    //设置操作超时时间
    status=viSetAttribute(session,VI_ATTR_TMO_VALUE,2000);
    //读取设备信息
    status=visaQuery(session,(unsigned char*)"*IDN?\n",buf,256,&act);

    if(!status)
    {
        qDebug()<<"devic info:"<<QString((char*)buf);
    }


    ViPBuf order2=(ViPBuf)("SENS:CURR:RANG 1e-6\n");

    //设置量程
    viWrite(session,order2,strlen(order2),NULL);
    isconnOk=true;
}
void DMM6500::setMeanFun()
{
    //测试电流 100uA量程 0.001分辨率 viWrite(session,)
    ViPBuf order1=(ViPBuf)("FUNC \"CURR\"\n");
    //设置测量电流模式
    viWrite(session,order1,strlen(order1),NULL);
}
//连续模式 单次读取电流
void DMM6500::singleMeansureCurrent()
{
    xVector.clear();
    yVector.clear();
    type=1;
    ViStatus status;
    status=visaQuery(session, (uchar*)"READ?\n",reading,MAX_READING,NULL);
    QString str((char*)reading);
    double value=str.toDouble();
    if(value>1e3)
    {
        qDebug()<<"error value: "<<str;
        return;
    }
    xVector.append(1);
    yVector.append(value*1000*calibration);
    emit sendCurrent(type,value);
}
//定时器回调函数
void DMM6500::timeOutFun()
{
     ViStatus status;
     status=visaQuery(session, (uchar*)"READ?\n",reading,MAX_READING,NULL);
     QString str((char*)reading);
     points++;
     xVector.append(points);
     double value=str.toDouble();
     if(value>1e3)
     {
         qDebug()<<"error value: "<<str;
         return;
     }

     yVector.append(1000*value*calibration);
     emit sendCurrent(type,value);
}
//开启连续读取
void DMM6500::startTimerMeansure(int interval)
{
    type=1;
    readTimer->start(interval);
}
//暂停定时器
void DMM6500::stopTimerMeansure()
{

    if(readTimer->isActive())
    {
        readTimer->stop();
    }
    if(pulseTimer->isActive())
    {
        pulseTimer->stop();
    }
}
//设置量程
void DMM6500::setRange(double range)
{
    char order[32];
    sprintf(order,"SENS:CURR:RANG %lf\n",range);
    qDebug()<<"order: "<<order;
    //设置量程
    viWrite(session,(uchar*)order,strlen((uchar*)order),NULL);
    //kedmm6500_SetAttributeViReal64(session, VI_NULL, KEDMM6500_ATTR_RANGE, range);

}
//获取电流表状态
bool DMM6500::getDmmStatus()
{
    return isconnOk;
}
//脉冲模式
void DMM6500::pulseMeansure()
{
    type=2;
    xVector.clear();
    yVector.clear();
    ViUInt32 actSize=0;
    viWrite(session,orderCls,strlen(orderCls),NULL);//清空缓冲区
    viWrite(session,orderInit,strlen(orderInit),NULL); //开始
    viWrite(session,orderWait,strlen(orderWait),NULL); //等待采集完成
    //读取数据
    if(!readBuf)
    {
        readBuf=new uchar[MAX_SIZE_CURR*samplePoints+1];
        if(readBuf==NULL)
        {
            QMessageBox::warning(NULL,"ERROR","内存不足,重新");
            return;
        }
    }
    double value=0;

    ViStatus status=visaQuery(session,(uchar*)orderRead,readBuf,MAX_SIZE_CURR*samplePoints,&actSize);
    if(!status)
    {

         QString temp((char*)readBuf);
         QStringList list=temp.split(',');
         for(int i=0;i<list.size();i++)
         {
             value=list[i].toDouble();
             qDebug()<<"value:"<<value;
             if(value<=0||value<1e-6) //算法 有待改进
                 value=0;
             if(value>1e4)  //异常处理
             {
                  qDebug()<<"error value:"<<list[i];
                  return;
             }
             xVector.append(list[++i].toDouble()*1000);
             yVector.append(1000*value*calibration);
         }
         qDebug()<<"dmm meansure";
         emit sendCurrent(2,value);
    }

}

void DMM6500::startPulseMeansure(int interval)
{
    pulseTimer->start(interval);

}
//脉冲模式下 电流表进行初始化
void DMM6500::pulseMeansureInit(int points,double freq,double range)
{
    uchar* orderCls=(uchar*)"TRACE:CLE \"defbuffer1\"\n";
    uchar* orderDig=(uchar*)"DIG:FUNC \"CURR\"\n";
    uchar* orderCnt=(uchar*)"DIG:COUNt 1\n";
    uchar* orderModel=(uchar*)"TRIG:LOAD \"EMPTY\"\n";
    uchar* orderWaitBloc=(uchar*)"TRIG:BLOC:WAIT 1, EXTernal,NEVer,OR,EXTernal,EXTernal\n";
    uchar* orderInput=(uchar*)"TRIG:EXT:IN:EDGE FALL\n";


    char orderPoint[32];
    sprintf(orderPoint,"TRAC:POIN %d,\"defbuffer1\"\n",points);
    char orderFreq[32];
    sprintf(orderFreq,"DIG:CURR:SRATE %lf\n",freq);
    char orderGet[64];
    sprintf(orderGet,"TRIG:BLOC:MDIG 1,\"defbuffer1\",%d\n",points);
    char orderRange[32];
    sprintf(orderRange,"SENS:DIG:CURR:RANG %lf\n",range);

    //初始化
    viWrite(session,(uchar*)"SYST:CLE\n",9,NULL); //清除系统信息
    viWrite(session,(uchar*)"STAT:CLE\n",9,NULL);

    viWrite(session,orderCls,strlen(orderCls),NULL);//清除缓冲区
    viWrite(session,(uchar*)orderPoint,strlen(orderPoint),NULL);//设置缓冲区点数

    viWrite(session,orderDig,strlen(orderDig),NULL); //设置电流模式
    viWrite(session,orderCnt,strlen(orderCnt),NULL);  //本次采集测量次数

    viWrite(session,(uchar*)orderRange,strlen((uchar*)orderRange),NULL); //设置量程
    viWrite(session,(uchar*)orderFreq,strlen((uchar*)orderFreq),NULL);//设置采集频率
    viWrite(session,orderModel,strlen(orderModel),NULL);//加载空模型

    viWrite(session,orderWaitBloc,strlen(orderWaitBloc),NULL);//设置等待模块
    viWrite(session,(uchar*)orderGet,strlen((uchar*)orderGet),NULL);//设置采集模块
    viWrite(session,orderInput,strlen(orderInput),NULL);//设置输入触发模式

    sprintf(orderRead,"TRAC:DATA? 1,%d,\"defbuffer1\",READ,REL\n",points);

}

// 保存数据
void DMM6500::saveData()
{


    QString fileName=QFileDialog::getSaveFileName(nullptr,"保存功率数据","./","text(*.txt)");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(NULL,"ERROR","保存失败");
        return;
    }
    QTextStream out(&file);

    for(int i=0;i<xVector.size();i++)
    {
        out<<xVector[i]<<" "<<yVector[i]<<"\n";
    }
    file.close();
}
QVector<double>DMM6500::getXVector()
{

    return xVector;
}

QVector<double>DMM6500::getYVector()
{
    return yVector;
}


void DMM6500::setPointsAndInterval(int points, double interval)
{
    samplePoints=points;
    sampleInterval=interval;
}



void DMM6500::freeReadBuffer()
{
    delete []readBuf;
    readBuf=nullptr;
}









