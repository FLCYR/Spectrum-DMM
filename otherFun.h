#ifndef OTHERFUN_H
#define OTHERFUN_H
#include<visa.h>
#include<QDebug>
#endif // OTHERFUN_H



//返回unsigned char 字符串的字符个数
unsigned long strlen(const unsigned char*str)
{
    unsigned long length=0;
    while(*str++)
        length++;
    return length;
}

//实现query功能
ViStatus visaQuery(ViSession&vi,ViConstBuf order,ViPBuf buf,ViUInt32 bufSize,ViPUInt32 actualSize)
{
    ViStatus status;
    ViUInt32 retCnt;
    ViUInt32 actSize;

    if(!order||!buf)//如果缓冲区为空
        return -1;
    status=viWrite(vi,order,strlen(order),&retCnt);
    qDebug()<<"status1=="<<status;
    if(status!=VI_SUCCESS)
        return status;
    status=viRead(vi,buf,bufSize,&actSize);
      qDebug()<<"status2=="<<status;
    if(status!=VI_SUCCESS)
        return status;
    buf[actSize-1]='\0'; //
    return status;

}
