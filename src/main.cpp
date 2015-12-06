#include <QApplication>
#include <QtGui>

#include "mainwindow.h"
#include "dsptools.h"




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //---------------让Qt支持中文-----------------------------
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    //------------------------------------------------------
    qDebug() << "GPR文件头大小:" << sizeof(_GPR_HEAD) << endl;
    //filter_test();
    //splib_test();
    //shift_right_test();
    //shift_left_test();
    //splib_fft_test();
    MainWindow w;
    w.show();
    return app.exec();
}



