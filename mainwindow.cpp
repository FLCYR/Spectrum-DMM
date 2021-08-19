#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isReady=false;
    range=1e-4;
    samplePoints=0;
    lightNoise=nullptr;
    style=new CustomStyle();
    rangeDialog.hide();
    timeUpdate=new QTimer(this);
    connect(timeUpdate,&QTimer::timeout,this,&MainWindow::setTime);
    timeUpdate->start(1000);
    ui->frame_3->setObjectName("plot");
    this->setWindowTitle("LiSpecView-VCS");

    createToolBars();
    setMenuBarInfo();
    setStatusBarInfo();
    setPlotInfo();
    loadStyleQss();



    //设置型号和序列号
    ui->lineEdit_type->setText(light.type);
    ui->lineEdit_serialNum->setText(light.serialNumber);

    connect(&light,&Light::paintPlot,this,&MainWindow::drawPlot);//光谱绘制
    connect(&dmm,&DMM6500::sendCurrent,this,&MainWindow::getCurrent); //绘制功率
    connect(&rangeDialog,&Dialog::rangeChange,this,&MainWindow::setRange); //范围设置槽函数

    if(dmm.getDmmStatus())
        successConnDmm();
    if(light.getSpectrumStatus())
        successConnLight();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *e)
{
    light.exitMeansure();
}
void MainWindow::connectDevic()
{
    if(!dmm.getDmmStatus())
    {
        dmm.dmm6500Init();
    }
    if(!light.getSpectrumStatus())
    {
        light.spectrumInit();
    }

    if(dmm.getDmmStatus())
    {
       successConnDmm();
    }
    if(light.getSpectrumStatus())
    {
        successConnLight();
    }

}
//退出程序
void MainWindow::exitProgramm()
{

}
//坐标轴信息
void MainWindow::setPlotInfo()
{



//    ui->plot_power->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
//                              QCP::iSelectLegend | QCP::iSelectPlottables);



    ui->plot_light->plotLayout()->insertRow(0);
    ui->plot_power->plotLayout()->insertRow(0);
    ui->plot_light->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_light,"光谱",12));
    ui->plot_power->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_power,"功率",12));

    //坐标轴名字
    ui->plot_power->xAxis->setLabel("时间(ms)");
    ui->plot_power->yAxis->setLabel("功率(mw)");

    ui->plot_light->xAxis->setLabel("波长(nm)");
    ui->plot_light->yAxis->setLabel("ADC Counts");

    //坐标轴名称颜色和字体大小
    ui->plot_power->xAxis->setLabelColor(QColor(0,0,255));
    ui->plot_power->xAxis->setLabelFont(QFont("宋体",13));
    ui->plot_power->yAxis->setLabelColor(QColor(0,0,255));
    ui->plot_power->yAxis->setLabelFont(QFont("宋体",13));

    ui->plot_light->xAxis->setLabelColor(QColor(0,0,255));
    ui->plot_light->xAxis->setLabelFont(QFont("宋体",13));
    ui->plot_light->yAxis->setLabelColor(QColor(0,0,255));
    ui->plot_light->yAxis->setLabelFont(QFont("宋体",13));


    //设置坐标背景颜色
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(240, 240, 240));//渐变
    plotGradient.setColorAt(1, QColor(210, 210, 210));
    ui->plot_light->setBackground(plotGradient);
    ui->plot_power->setBackground(plotGradient);

    ui->plot_power->addGraph();

    ui->plot_light->addGraph();//原曲线
    ui->plot_light->graph(0)->setPen(QPen(Qt::blue));
    ui->plot_light->addGraph(); //噪声
    ui->plot_light->graph(1)->setPen(QPen(Qt::yellow));
    ui->plot_light->addGraph(); //原曲线-噪声
    ui->plot_light->graph(2)->setPen(QPen(Qt::red));

    ui->plot_light->graph(0)->rescaleAxes(); //设置 可显示
    ui->plot_light->graph(1)->rescaleAxes(true);
    ui->plot_light->graph(2)->rescaleAxes(true);

    ui->plot_light->graph(1)->setVisible(false);
    ui->plot_light->graph(2)->setVisible(false); //设置不可见

    ui->plot_light->xAxis->setRange(100,1200);
    ui->plot_light->yAxis->setRange(0,65000);
    ui->plot_power->xAxis->setRange(0,300);
}
//加载qss文件
void MainWindow::loadStyleQss()
{
    QFile file(":/css/style.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream text(&file);
    QString styleSheet=text.readAll();
    this->setStyleSheet(styleSheet);
    file.close();
}
//设置时间
void MainWindow::setTime()
{
    QDateTime currentTime=QDateTime::currentDateTime();
    QString timeStr=currentTime.toString("yyyy-MM-dd hh:mm:ss");
    timeLabel->setText(timeStr);
}
//状态栏设置
void MainWindow::setStatusBarInfo()
{
    ui->statusbar->setLayoutDirection(Qt::RightToLeft);//从右到左
    timeLabel=new QLabel();
    dmmStatusLabel=new QLabel("电流表连接失败");
    spectrumStatusLabel=new QLabel("光谱仪连接失败");
    dmmStatusLabel->setStyleSheet("color:red");
    spectrumStatusLabel->setStyleSheet("color:red");
    ui->statusbar->addWidget(timeLabel);
    ui->statusbar->addWidget(spectrumStatusLabel);
    ui->statusbar->addWidget(dmmStatusLabel);
    setTime();
}
//菜单栏属性设置
void MainWindow::setMenuBarInfo()
{
    ui->menu_file->setStyle(style);
    ui->menu_help->setStyle(style);
    ui->menu_soft->setStyle(style);
    ui->menu_view->setStyle(style);
    ui->menu_meansure->setStyle(style);
}

//创建工具栏
void MainWindow::createToolBars()
{

    QIcon ico(":/icos/toolBar-ico/file.png");
    itemFile=new QAction(ico,"系数文件",this);
    itemConn=new QAction(QIcon(":/icos/toolBar-ico/conn.png"),"连接设备");

    itemStart=new QAction(QIcon(":/icos/toolBar-ico/start.png"),"连续测量");
    itemStop=new QAction(QIcon(":/icos/toolBar-ico/stop.png"),"停止测试");
    itemSaveData=new QAction(QIcon(":/icos/toolBar-ico/save-data.png"),"保存数据");
    itemSavePowerPng=new QAction(QIcon(":/icos/toolBar-ico/save-power.png"),"保存功率图");
    itemNoise=new QAction(QIcon(":/icos/toolBar-ico/noise.png"),"噪声采集");
    itemSingle=new QAction(QIcon(":/icos/toolBar-ico/single.png"),"单次测量");
    itemSaveLightPng=new QAction(QIcon(":/icos/toolBar-ico/save-light.png"),"保存光谱图");
    QAction*itemLogo=new QAction(QIcon(":/icos/toolBar-ico/company-logo.png"),"");



    connect(itemStart,&QAction::triggered,this,&MainWindow::startSeriesMeansure);//开始测试槽函数
    connect(itemStop,&QAction::triggered,this,&MainWindow::stopMeansure);
    connect(itemNoise,&QAction::triggered,this,&MainWindow::collectNoise); //噪声采集
    connect(itemSingle,&QAction::triggered,this,&MainWindow::startSingleMeansure);//单次测量
    connect(itemFile,&QAction::triggered,this,&MainWindow::openCaliFile);
    connect(itemSaveData,&QAction::triggered,this,&MainWindow::saveAllData);//保存所有数据

    connect(itemConn,&QAction::triggered,this,&MainWindow::connectDevic);
    connect(itemSavePowerPng,&QAction::triggered,this,&MainWindow::savePower);//保存谱图
    connect(itemSaveLightPng,&QAction::triggered,this,&MainWindow::saveLight);

    ui->toolBar->addAction(itemFile);
    ui->toolBar->addAction(itemConn);
    ui->toolBar->addSeparator();

    ui->toolBar->addAction(itemStart);
    ui->toolBar->addAction(itemSingle);
    ui->toolBar->addAction(itemStop);
    ui->toolBar->addSeparator();

    ui->toolBar->addAction(itemNoise);
    ui->toolBar->addSeparator();

    ui->toolBar->addAction(itemSaveData);
    ui->toolBar->addAction(itemSavePowerPng);
    ui->toolBar->addAction(itemSaveLightPng);
    itemLogo->setCheckable(false);
    ui->toolBar_2->addAction(itemLogo);
    itemStop->setEnabled(false);
}

//计算FWHM
double MainWindow::calcFWHM(int maxIndex,double maxValue,const QVector<double>&x,const QVector<double>&y)
{
    double mean=maxValue/2;

    int first=0,sec=0;
    for(int i=maxIndex;i>=0;i--)
    {
        if(y[i]<=mean)
        {
             first=i;
             break;
        }
    }
    for(int i=maxIndex;i<y.size();i++)
    {
        if(y[i]<=mean)
        {
             sec=i;
             break;
        }
    }
   return x[sec]-x[first];
}
//设置光谱图表数据
void MainWindow::drawPlot(double*x,double*y,unsigned short count)
{
    QVector<double>xVector(x,x+count);
    QVector<double>yVector(y,y+count);
    if(lightNoise)
    {
        QVector<double>noise;
        for(int i=0;i<count;i++)
        {
            noise.append(y[i]-lightNoise[i]);
        }
        ui->plot_light->graph(2)->setData(xVector,noise);
    }

    ui->plot_light->graph(0)->setData(xVector,yVector);
    ui->plot_light->replot();

    auto iter=std::max_element(yVector.begin(),yVector.end());
    int pos=std::distance(yVector.begin(),iter);
    double maxValue=xVector[pos];
    double fwhm=calcFWHM(pos,*iter,xVector,yVector);

    ui->lineEdit_top_wave->setText(QString("%1").arg(maxValue));
    ui->lineEdit_fwhm->setText(QString("%1").arg(fwhm));
}
//获取电流和处理
void MainWindow::getCurrent(int type,double current)
{
    QVector<double>x=dmm.getXVector();
    QVector<double>y=dmm.getYVector();
    if(range<=1e-4)
        current*=1e6;
    else if(range>=1e-3&&range<=1e-1)
        current*=1e3;
    ui->lineEdit_current->setText(QString("%1").arg(current)); //显示当前电流
    double maxPower=*std::max_element(y.begin(),y.end());
    double sum=std::accumulate(y.begin(),y.end(),0.0);
    double avg=sum/y.size();
    ui->lineEdit_max->setText(QString("%1").arg(maxPower));
    ui->lineEdit_avg->setText(QString("%1").arg(avg));
    //连续模式
    if(type==1)
    {
        ui->plot_power->graph(0)->setData(x,y);
        ui->plot_power->replot();
    }
    //脉冲模式
    else if(type==2)
    {
        ui->plot_power->graph(0)->setData(x,y);
        ui->plot_power->replot();
        //设置频率，占空比
        calcDmmData(x,y);
    }
}

//计算最大和平均功率 频率
void MainWindow::calcDmmData(const QVector<double>&x,const QVector<double>&y)
{
    int firstPos=0,secPos=0,thirdPos=0;
    bool flag=false;
    bool sFlag=false;
    double freq=0,duty=0;

    if(y[0]<=0)
    {
        for(int i=0;i<y.size();i++)
        {
            if(!flag&&y[i]>0)
            {
                firstPos=i;
                flag=true;
            }
            if(!sFlag&&flag&&y[i]<=0)
            {
                secPos=i;
                sFlag=true;
            }
            if(sFlag&&y[i]>0)
            {
                thirdPos=i;
                break;
            }
        }
        duty=(x[secPos]-x[firstPos])/(x[thirdPos]-x[firstPos])*100;
    }
    else
    {
        for(int i=0;i<y.size();i++)
        {
            if(!flag&&y[i]<=0)
            {
                firstPos=i;
                flag=true;
            }
            if(!sFlag&&flag&&y[i]>0)
            {
                secPos=i;
                sFlag=true;
            }
            if(sFlag&&y[i]<=0)
            {
                thirdPos=i;
                break;
            }
        }
        duty=100*(1.0-(x[secPos]-x[firstPos])/(x[thirdPos]-x[firstPos]));
    }
    qDebug()<<firstPos<<" "<<secPos<<" "<<thirdPos;
    if(firstPos&&secPos&&thirdPos)
    {
        freq=1/(x[thirdPos]-x[firstPos])*1000;
        ui->lineEdit_freq->setText(QString("%1").arg(freq));
        ui->lineEdit_duty->setText(QString("%1").arg(duty));
    }
}

//单次测量
void MainWindow::startSingleMeansure()
{
    if(!isReady)
    {
        QMessageBox::warning(this,"ERROR","没有打开系数文件");
        return;
    }
    setToolsActEnabled(false);
    ratio=ui->lineEdit_ratio->text().toDouble();
    dmm.setCalibration(ratio);
    //光谱仪单次测量
    if(light.getSpectrumStatus())
    {
        int integral=ui->spinBox_integral->value();
        light.setSpectrumParameter(integral,0);
        light.doMeansure();
    }
    //连续模式
    if(dmm.getDmmStatus()&&ui->radioBtb_contin->isChecked())
    {
        dmm.setMeanFun();
        dmm.singleMeansureCurrent();
    }
    //脉冲模式
    else if(dmm.getDmmStatus()&&ui->radioBtn_pluse->isChecked())
    {
        samplePoints=ui->spinBox_points->value();
        if(preSamplePoints<samplePoints) //如果前一次点数小于当前点数 重新分配空间
            dmm.freeReadBuffer();
        preSamplePoints=samplePoints;
        if(samplePoints<100)
            samplePoints=100;
        double freq=ui->spinBox_frequency->value();
        if(freq<1000)
            freq=1000;
        if(freq>1000000)
            freq=1000000;
        dmm.setPointsAndInterval(samplePoints,1/freq*1000);
        dmm.pulseMeansureInit(samplePoints,freq,range);
        dmm.pulseMeansure();//开始脉冲测量
    }
    setToolsActEnabled(true);
}
//连续测量
void MainWindow::startSeriesMeansure()
{
    if(!isReady)
    {
        QMessageBox::warning(this,"ERROR","没有打开系数文件");
        return;
    }
    ratio=ui->lineEdit_ratio->text().toDouble();
    dmm.setCalibration(ratio);

    //光谱仪
    if(light.getSpectrumStatus())
    {
        setToolsActEnabled(false);
        int integral=ui->spinBox_integral->value();
        light.startMeansure(500,integral,0); //100ms
    }

    //脉冲模式下
    if(dmm.getDmmStatus()&&ui->radioBtn_pluse->isChecked())
    {
        setToolsActEnabled(false);
        samplePoints=ui->spinBox_points->value();
        if(samplePoints<100)
            samplePoints=100;

        if(preSamplePoints<samplePoints) //如果前一次点数小于当前点数 重新分配空间
            dmm.freeReadBuffer();
        double freq=ui->spinBox_frequency->value();
        if(freq<1000)
            freq=1000;
        if(freq>1000000)
            freq=1000000;

        dmm.pulseMeansureInit(samplePoints,freq,range); //测量初始化
        qDebug()<<"freq=="<<freq;
        dmm.setPointsAndInterval(samplePoints,1/freq*1000);//设置采样间隔和采样点数
        dmm.startPulseMeansure(500);
    }
    //连续模式
    else if(dmm.getDmmStatus()&&ui->radioBtb_contin->isChecked())
    {

        setToolsActEnabled(false);
        dmm.setMeanFun();
        dmm.startTimerMeansure(500);// 500ms
    }

}


//停止测试
void MainWindow::stopMeansure()
{
    setToolsActEnabled(true);
    dmm.stopTimerMeansure();
    light.stopMeansure();

}

//光谱仪连接成功
void MainWindow::successConnLight()
{
    spectrumStatusLabel->setText("光谱仪连接成功");
    spectrumStatusLabel->setStyleSheet("color:#D16AF5");

}
void MainWindow::successConnDmm()
{
    dmmStatusLabel->setText("电流表连接成功");
    dmmStatusLabel->setStyleSheet("color:#D16AF5");

}
//下拉框 量程设置
void MainWindow::on_comboBox_range_currentIndexChanged(int index)
{

    switch(index)
    {
    case 0:
        range=1e-4;//100uA
        ui->label_current->setText("电流[uA]");
        break;
    case 1:
        range=1e-3;//1mA
        ui->label_current->setText("电流[mA]");
        break;
    case 2:
        range=1e-2;//10mA
        ui->label_current->setText("电流[mA]");
        break;
    case 3:
        range=1e-1;//100mA
        ui->label_current->setText("电流[mA]");
        break;
    case 4:
        range=1.0;//1A
        ui->label_current->setText("电流[A]");
        break;
    case 5:
        range=3.0;//3A
        ui->label_current->setText("电流[A]");
        break;
    default:
        range=1e-4;
        ui->label_current->setText("电流[uA]");
        break;
    }
    qDebug()<<"range: "<<range;
}
//打开系数文件
void MainWindow::openCaliFile()
{
    QString fileName;
    QVector<int> temp;
    QRegExp reg("\\d+,\\d+\\.?\\d+");
    fileName=QFileDialog::getOpenFileName(this,"系数文件","./","csvFile(*.csv *.txt)");
    qDebug()<<fileName;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if(file.isOpen())
    {
        file.readLine(); //文件头
        while(!file.atEnd())
        {
            QString line=file.readLine();
            line=line.trimmed();

            if(reg.exactMatch(line))
            {
                QStringList list=line.split(',');
                if(list.size()==2)
                {
                    calibration.insert(list[0].toInt(),list[1]);
                    temp.append(list[0].toInt());
                }
            }
            else
            {
               QMessageBox::warning(this,"ERROR","系数文件格式不对");
               isReady=false;
               return;
            }
        }
    }
    int minValue=*std::min_element(temp.begin(),temp.end());
    ui->spinBox_wave->setRange(minValue,*std::max_element(temp.begin(),temp.end()));
    ui->lineEdit_ratio->setText(calibration[minValue]);
    isReady=true;
}
//连续RadioButton 被点击
void MainWindow::on_radioBtb_contin_clicked()
{
    if(ui->radioBtb_contin->isChecked())
    {
        ui->spinBox_duty->setEnabled(false);
        ui->spinBox_points->setEnabled(false);
        ui->spinBox_frequency->setEnabled(false);
        ui->spinBox_plus_freq->setEnabled(false);
        ui->plot_power->xAxis->setLabel("点数");
        ui->plot_power->replot();
    }

}
//脉冲 RadioButton点击
void MainWindow::on_radioBtn_pluse_clicked()
{
    if(ui->radioBtn_pluse->isChecked())
    {
        ui->spinBox_duty->setEnabled(true);
        ui->spinBox_points->setEnabled(true);
        ui->spinBox_frequency->setEnabled(true);
        ui->spinBox_plus_freq->setEnabled(true);
        ui->plot_power->xAxis->setLabel("时间(ms)");
        ui->plot_power->replot();
    }


}
//是否选择触发
void MainWindow::triggerSelect()
{

    if(ui->checkBox_light->isChecked())
    {
        isLightTrigger=true;
    }
    if(ui->checkBox_dmm->isChecked())
    {
        isDmmTrigger=true;
    }
}
//保存光谱图
void MainWindow::saveLight()
{
    QString fileName=QFileDialog::getSaveFileName(this,"保存截图","./","pic(*.png)");
    QScreen*screen=QGuiApplication::primaryScreen();
    QPixmap pixPower=screen->grabWindow(ui->plot_light->winId());//功率图
    bool flag=pixPower.save(fileName);
    if(flag)
    {
        QMessageBox::information(this,"SAVE","保存成功");
    }
}
//保存功率图
void MainWindow::savePower()
{
    QString fileName=QFileDialog::getSaveFileName(this,"保存截图","./","pic(*.png)");
    QScreen*screen=QGuiApplication::primaryScreen();
    QPixmap pixPower=screen->grabWindow(ui->plot_power->winId());//功率图

    bool flag=pixPower.save(fileName);
    if(flag)
    {
        QMessageBox::information(this,"SAVE","保存成功");
    }
}

//保存光谱仪数据
void MainWindow::saveAllData()
{
    dmm.saveData();
    light.saveData();
}

//采集噪声并绘图
void MainWindow::collectNoise()
{
    if(!light.getSpectrumStatus())
    {
        QMessageBox::warning(this,"ERROR","光谱仪没有连接");
        return;
    }
    double *wave=NULL;
    int pixCount=0;
    QMessageBox::information(this,"Information","确保在无光条件下采集");
    if(light.getWaveLength(wave,pixCount))
    {
         if(!lightNoise)
         {
             lightNoise=new double[pixCount];
         }
         if(lightNoise)
         {
             light.setSpectrumParameter(10,0);
             light.collectNoise(lightNoise);
             QVector<double> x(wave,wave+pixCount);
             QVector<double> y(lightNoise,lightNoise+pixCount);
             qDebug()<<"x.size,y.size="<<x.size()<<""<<y.size();
             ui->plot_light->graph(1)->setData(x,y);
             if(ui->checkBox_noise->isChecked())
                 ui->plot_light->replot();
        }
    }

}

//设置工具栏action是否可用

void MainWindow::setToolsActEnabled(bool flag)
{
    if(flag)
    {
        itemStart->setEnabled(flag);
        itemFile->setEnabled(flag);
        itemConn->setEnabled(flag);
        itemSaveData->setEnabled(flag);
        itemSavePowerPng->setEnabled(flag);
        itemSaveLightPng->setEnabled(flag);
        itemNoise->setEnabled(flag);
        itemNoise->setEnabled(flag);
        itemSingle->setEnabled(flag);
        itemStop->setEnabled(!flag);
    }
    else
    {
        itemStart->setEnabled(flag);
        itemFile->setEnabled(flag);
        itemConn->setEnabled(flag);
        itemSaveData->setEnabled(flag);
        itemSavePowerPng->setEnabled(flag);
        itemSaveLightPng->setEnabled(flag);
        itemNoise->setEnabled(flag);
        itemNoise->setEnabled(flag);
        itemSingle->setEnabled(flag);
        itemStop->setEnabled(flag);
    }
}

//功率显示范围设置
void MainWindow::on_btn_powerRange_clicked()
{
    rangeDialog.type=1;
    rangeDialog.setPowerLabel();
    rangeDialog.setPowerDefaultValue();
    rangeDialog.show();
}
//波长范围设置
void MainWindow::on_btn_lightRange_clicked()
{
    rangeDialog.type=2;
    rangeDialog.setWaveDefaultValue();
    rangeDialog.setWaveLabel();
    rangeDialog.show();
}
//范围设置
void MainWindow::setRange(double xMin,double xMax,double yMin,double yMax)
{

    if(xMax>xMin)
    {
        if(rangeDialog.type==1)
        {
            ui->plot_power->xAxis->setRange(xMin,xMax);
            ui->plot_power->replot();
        }
        else
        {
            ui->plot_light->xAxis->setRange(xMin,xMax);
            ui->plot_light->replot();
        }

    }
    if(yMax>yMin)
    {
        if(rangeDialog.type==1)
        {
            ui->plot_power->yAxis->setRange(yMin,yMax);
            ui->plot_power->replot();
        }
        else
        {
            ui->plot_light->yAxis->setRange(yMin,yMax);
            ui->plot_light->replot();
        }
    }
}




//原曲线复选框状态改变
void MainWindow::on_checkBox_origin_stateChanged(int arg1)
{
    if(ui->checkBox_origin->isChecked())
    {
        ui->plot_light->graph(0)->setVisible(true);
    }
    else
        ui->plot_light->graph(0)->setVisible(false);
      ui->plot_light->replot();
}
//噪声曲线是否显示
void MainWindow::on_checkBox_noise_stateChanged(int arg1)
{
    if(ui->checkBox_noise->isChecked())
    {
        ui->plot_light->graph(1)->setVisible(true);
    }
    else
        ui->plot_light->graph(1)->setVisible(false);
      ui->plot_light->replot();
}
//原曲线-噪声曲线
void MainWindow::on_checkBox_orig_noise_stateChanged(int arg1)
{
    if(ui->checkBox_orig_noise->isChecked())
    {
        ui->plot_light->graph(2)->setVisible(true);
    }
    else
    {
         ui->plot_light->graph(2)->setVisible(false);

    }
      ui->plot_light->replot();

}

//菜单栏槽函数
void MainWindow::on_action_file_triggered()
{
    openCaliFile();
}
//保存所有数据
void MainWindow::on_action_saveData_triggered()
{
    saveAllData();
}
//菜单栏 保存功率图谱
void MainWindow::on_action_savePowerPng_triggered()
{
    savePower();
}
//菜单栏保存光谱图
void MainWindow::on_action_saveLightPng_triggered()
{
    saveLight();
}

//功率自适应
void MainWindow::on_actions_powerAuto_triggered()
{
    //自适应
}
//光谱范围自适应
void MainWindow::on_action_lightAuto_triggered()
{
    //
}
//单次测量
void MainWindow::on_action_signle_triggered()
{
    startSingleMeansure();
}
//连续测量
void MainWindow::on_action_contin_triggered()
{
    startSeriesMeansure();
}
//停止测量
void MainWindow::on_action_stop_triggered()
{
    stopMeansure();
}
//设置
void MainWindow::on_action_set_triggered()
{
    QMessageBox::information(this,"set","None");
}
//帮助
void MainWindow::on_action_help_triggered()
{
    QMessageBox::information(this,"Help","Help");
}
//关于
void MainWindow::on_action_about_triggered()
{
    QMessageBox::information(this,"About","About");
}
//根据波长 设置标定系数
void MainWindow::on_spinBox_wave_valueChanged(int arg1)
{

    ui->lineEdit_ratio->setText(calibration[arg1]);
}





