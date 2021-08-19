QT       += core gui

QT      +=printsupport



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customstyle.cpp \
    dialog.cpp \
    dmm6500.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    customstyle.h \
    dialog.h \
    dmm6500.h \
    light.h \
    mainwindow.h \
    otherFun.h \
    qcustomplot.h \
    variable.h

FORMS += \
    dialog.ui \
    mainwindow.ui
msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

#图标
RC_ICONS=./toolBar-ico/app.ico

#光谱仪文件
INCLUDEPATH+=d:\AVA_Specs
LIBS+=d:\AVA_Specs\avaspecx64.lib

#电流表
INCLUDEPATH+=D:\keddm\include
LIBS+=D:\keddm\lib\*.lib

INCLUDEPATH+=D:\VISA-LIB-INCLUDE\include
LIBS+=D:\VISA-LIB-INCLUDE\Lib_x64\msc\*.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc \
    qss.qrc

DISTFILES += \
    menu-ico/exit.png
