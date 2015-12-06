#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T16:12:22
#
#-------------------------------------------------

QT       += core gui


DEFINES    += QT_DLL QWT_DLL

CONFIG(debug,debug|release):LIBS += -L"D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/lib" -lqwtd

          else:LIBS += -L"D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/lib" -lqwt

INCLUDEPATH += E:/My_Projects/My_Qt_Projects/PavementEvaluation/splab/include
INCLUDEPATH += D:/ProgramFiles/ProgrammingTools/Qt/4.8.4/include/qwt
INCLUDEPATH += E:/My_Projects/My_Qt_Projects/PavementEvaluation/fftw3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PavementEvaluation
TEMPLATE = app


SOURCES += main.cpp\
        pavementevaluation.cpp \
    layer_detector.cpp \
    parameterdialog.cpp \
    mainwindow.cpp \
    layerinfo.cpp \
    curveplotter.cpp \
    bscan_viewer.cpp \
    thicknessestimator.cpp \
    dsptools.cpp \
    bpfdialog.cpp \
    removecouplingpulsedialog.cpp \
    loadrefwavedialog.cpp \
    arthurstyle/hoverpoints.cpp \
    arthurstyle/arthurwidgets.cpp \
    arthurstyle/arthurstyle.cpp \
    layerexplorerpanel.cpp \
    spectrumviewer.cpp \
    thicknessdialog.cpp \
    fft_operation.cpp \
    sdsp/mdsp.cpp \
    complex/mcomplex.cpp

HEADERS  += pavementevaluation.h \
    layer_detector.h \
    gpr_fileheader.h \
    parameterdialog.h \
    mainwindow.h \
    layerinfo.h \
    curveplotter.h \
    bscan_viewer.h \
    thicknessestimator.h \
    dsptools.h \
    bpfdialog.h \
    removecouplingpulsedialog.h \
    loadrefwavedialog.h \
    arthurstyle/hoverpoints.h \
    arthurstyle/arthurwidgets.h \
    arthurstyle/arthurstyle.h \
    layerexplorerpanel.h \
    spectrumviewer.h \
    thicknessdialog.h \
    segy_filehead.h \
    fft_operation.h \
    sdsp/mdsp.h \
    complex/mcomplex.h

RESOURCES += \
    pavementevaluation.qrc \
    arthurstyle/shared.qrc

RC_FILE += apprc.rc

FORMS +=

OTHER_FILES += \
    readme.txt
