#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QObject>
#include<QProxyStyle>
//QMenu ->Action 图标自定义大小
class CustomStyle : public QProxyStyle
{
    Q_OBJECT

public:
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;
};

#endif // CUSTOMSTYLE_H
