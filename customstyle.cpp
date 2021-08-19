#include "customstyle.h"

int CustomStyle ::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (metric == QStyle::PM_SmallIconSize) {
        return 32;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}
