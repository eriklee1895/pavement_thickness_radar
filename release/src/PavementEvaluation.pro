#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T16:12:22
#
#-------------------------------------------------

QT       += core gui


DEFINES    += QT_DLL QWT_DLL

CONFIG(debug,debug|release):LIBS += -L"D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/lib" -lqwtd

          else:LIBS += -L"D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/lib" -lqwt

INCLUDEPATH += D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/include/qwt


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PavementEvaluation
TEMPLATE = app


SOURCES += main.cpp\
        pavementevaluation.cpp \
    layer_detector.cpp \
    parameterdialog.cpp \
    mainwindow.cpp \
    plot.cpp

HEADERS  += pavementevaluation.h \
    layer_detector.h \
    gpr_fileheader.h \
    parameterdialog.h \
    mainwindow.h \
    plot.h

RESOURCES += \
    pavementevaluation.qrc
