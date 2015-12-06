#ifndef PAVEMENTEVALUATION_H
#define PAVEMENTEVALUATION_H


#include <QApplication>
#include <QWidget>
#include <QList>
#include <QPointF>
#include <QPoint>
#include "gpr_fileheader.h"


#include "plot.h"


class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QRadioButton;
class QStackedLayout;
class QPushButton;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QTextEdit;
class QwtPlot;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtSymbol;

class PavementEvaluation : public QWidget
{
    Q_OBJECT
    
public:
    PavementEvaluation(QWidget *parent = 0);
    ~PavementEvaluation();
    int get_layer_num() const { return layer_num; }

signals:
    void layer_num_updated();

public slots:
    void onReadButtonClicked(void);
    void onDetectButtonClicked(void);
    bool outputReport(void);
    void plotSingleTrace(void);
    void plotLayer(void);


private:
    //---------------------GUI元素------------------------
    QListWidget *listWidget;
    QTreeWidget *treeWidget;
    QPushButton *readButton;
    QPushButton *quitButton;
    QPushButton *detectButton;
    QPushButton *reportButton;
    QPushButton *showSingleTraceButton;
    QLabel *dataPointsLabel;
    QTextEdit *textEditer;
    QTableWidget *tableWidget;

    QwtPlot *ascanPlotter;
    QwtPlot *layerPlotter;
    QwtPlotCurve *single_trace_curve;
    QwtPlotCurve *layer_curve;
    Plot *plot;


    //-----------------GPR文件头中的信息----------------
    int iSmpNum;          // 每道点数(包括90字节道头+数据点数)
    int sampling_num;      //每道点数 = iSmpNum - 45
    double dTimeWnd;      // 时窗
    int uiTrcByteNum;    // 每道字节数
    int data_bytes_each_trace;    //每道数据的字节数 = uiTrcByteNum - 90
    int iTrcNum;          // 道数
    int layer_num;   //检测到的层位数
    QString fileName;   //当前打开的GPR文件名
    //------------------保存GPR数据的数组----------------
    double **bscan_data;        //保存GPR数据的数组

    //------------------私有函数------------------------
    template <class T> bool saveArrayAsTxt(T *array, int n);    //把数组保存到txt文件中
    bool readGprData(const QString &gprFileName);   //读取GPR格式文件
    void createAscanPlot();
    void createLayerPlot();

};

#endif // PAVEMENTEVALUATION_H
