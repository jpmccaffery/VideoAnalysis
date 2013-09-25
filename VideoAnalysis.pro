#-------------------------------------------------
#
# Project created by QtCreator 2012-06-02T13:09:56
#
#-------------------------------------------------

QT       += core gui phonon sql opencv

TARGET = VideoAnalysis
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += opencv

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE *= -O3

SOURCES += main.cpp\
        mainwindow.cpp \
    Z_Chain.cpp \
    VideoData.cpp \
    TopicModel.cpp \
    Sampler.cpp \
    LearningModel.cpp \
    HillClimber.cpp \
    DataManager.cpp \
    videowidget.cpp \
    videoloadwidget.cpp \
    videodialog.cpp \
    clickableimage.cpp \
    learningwindow.cpp \
    topicwindow.cpp \
    topicdialog.cpp \
    projectwidget.cpp \
    projectthread.cpp \
    datacollection.cpp \
    primaryfeature.cpp

HEADERS  += mainwindow.h \
    Z_Chain.h \
    VideoData.h \
    TopicModel.h \
    Sampler.h \
    LearningModel.h \
    HillClimber.h \
    DataManager.h \
    videowidget.h \
    videoloadwidget.h \
    videodialog.h \
    clickableimage.h \
    learningwindow.h \
    topicwindow.h \
    topicdialog.h \
    projectwidget.h \
    projectthread.h \
    datacollection.h \
    primaryfeature.h \
    GlobalConsts.h

FORMS    += mainwindow.ui \
    videowidget.ui \
    videoloadwidget.ui \
    videodialog.ui \
    clickableimage.ui \
    learningwindow.ui \
    topicwindow.ui \
    topicdialog.ui \
    projectwidget.ui




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib64/release/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib64/debug/ -lqwt
else:symbian: LIBS += -lqwt
else:unix: LIBS += -L$$PWD/../../../../lib64/ -lqwt

INCLUDEPATH += $$PWD/../../../../usr/include/qwt
DEPENDPATH += $$PWD/../../../../usr/include/qwt

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib64/release/ -lsvm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib64/debug/ -lsvm
else:symbian: LIBS += -lsvm
else:unix: LIBS += -L$$PWD/../../../../usr/lib64/ -lsvm

INCLUDEPATH += $$PWD/../../../../usr/lib64
DEPENDPATH += $$PWD/../../../../usr/lib64



#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/release/ -lopencv_legacy
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/debug/ -lopencv_legacy
#else:symbian: LIBS += -lopencv_legacy
#else:unix: LIBS += -L$$PWD/../../opencv/release/lib/ -lopencv_legacy

#INCLUDEPATH += $$PWD/../../opencv/release/include
#DEPENDPATH += $$PWD/../../opencv/release/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/release/ -lopencv_core
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/debug/ -lopencv_core
#else:symbian: LIBS += -lopencv_core
#else:unix: LIBS += -L$$PWD/../../opencv/release/lib/ -lopencv_core

#INCLUDEPATH += $$PWD/../../opencv/release/include
#DEPENDPATH += $$PWD/../../opencv/release/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/release/ -lopencv_highgui
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/debug/ -lopencv_highgui
#else:symbian: LIBS += -lopencv_highgui
#else:unix: LIBS += -L$$PWD/../../opencv/release/lib/ -lopencv_highgui

#INCLUDEPATH += $$PWD/../../opencv/release/include
#DEPENDPATH += $$PWD/../../opencv/release/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/release/ -lopencv_imgproc
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/release/lib/debug/ -lopencv_imgproc
#else:symbian: LIBS += -lopencv_imgproc
#else:unix: LIBS += -L$$PWD/../../opencv/release/lib/ -lopencv_imgproc

#INCLUDEPATH += $$PWD/../../opencv/release/include
#DEPENDPATH += $$PWD/../../opencv/release/include


#End of self installed opencv


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_core
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_core
#else:symbian: LIBS += -lopencv_core
#else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_core

#INCLUDEPATH += $$PWD/../../../../usr/local/include/
#DEPENDPATH += $$PWD/../../../../usr/local/lib/

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_highgui
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_highgui
#else:symbian: LIBS += -lopencv_highgui
#else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_highgui

#INCLUDEPATH += $$PWD/../../../../usr/local/include/
#DEPENDPATH += $$PWD/../../../../usr/local/lib/

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_imgproc
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_imgproc
#else:symbian: LIBS += -lopencv_imgproc
#else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_imgproc

#INCLUDEPATH += $$PWD/../../../../usr/local/include/
#DEPENDPATH += $$PWD/../../../../usr/local/lib/

