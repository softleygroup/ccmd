#-------------------------------------------------
#
# Project created by QtCreator 2012-04-13T10:51:13
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = CCMD
TEMPLATE = app

unix:!macx:!symbian: INCLUDEPATH += ~/boost_1_49_0 

SOURCES += main.cpp\
        mainwindow.cpp \
        glwidget.cpp \

HEADERS  += mainwindow.h \
    glwidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    CCMD_res.qrc

macx: LIBS += -L$$PWD/../../Code/Develop/ccmd/bin/macosx/Release/ -lccmd
//unix:!macx:!symbian: LIBS += -L$$PWD/../bin/linux/ -lccmd

unix:!macx:!symbian: LIBS += -L$$PWD/../bin/linux/ -lccmd
unix:!macx:!symbian: LIBS += -L/usr/local/cuda/lib -lcuda -lcudart

INCLUDEPATH += $$PWD/../src/include
DEPENDPATH += $$PWD/../src/include

unix:!macx:!symbian: INCLUDEPATH += /export/home/tps24/mtb/NVIDIA_GPU_Computing_SDK/C/common/inc 
unix:!macx:!symbian: DEPENDPATH += /export/home/tps24/mtb/NVIDIA_GPU_Computing_SDK/C/common/inc 

unix:!macx:!symbian: INCLUDEPATH += /usr/local/cuda/include 
unix:!macx:!symbian: DEPENDPATH += /usr/local/cuda/include  

macx: PRE_TARGETDEPS += $$PWD/../../Code/Develop/ccmd/bin/macosx/Release/libccmd.a
unix:!macx:!symbian: PRE_TARGETDEPS += $$PWD/../bin/linux/libccmd.a

