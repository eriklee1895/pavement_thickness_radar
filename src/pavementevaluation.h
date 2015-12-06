#ifndef PAVEMENTEVALUATION_H
#define PAVEMENTEVALUATION_H


#include <QApplication>
#include <QWidget>
#include <QList>
#include <QVector>
#include <QPointF>
#include <QPoint>
#include <QString>
#include "gpr_fileheader.h"
#include "curveplotter.h"
#include "bscan_viewer.h"
#include "layerexplorerpanel.h"


class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QRadioButton;
class QStackedLayout;
class QPushButton;

class QTreeWidget;
class QTreeWidgetItem;
class QwtPlot;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtSymbol;

class CurvePlotter;
class Bscan_Viewer;
class LoadRefWaveDialog;
class RemoveCouplingPulseDialog;
class BpfDialog;

class PavementEvaluation : public QWidget
{
    Q_OBJECT
    friend class LoadRefWaveDialog;
    friend class RemoveCouplingPulseDialog;
    friend class BpfDialog;
public:
    PavementEvaluation(QWidget *parent = 0);
    ~PavementEvaluation();
    int get_layer_num() const { return layer_num; }
    // 返回B-Scan中鼠标当前的位置
    const QString& current_pos_on_profile() const { return bscan_viewer->currentMousePosInfo(); }
    void setSampingFrequency(float freq);   // 设置采样频率域采样时窗，并更新信息列表

    // GPR文件、金属板文件、对空文件是否导入的标志
    bool isGprFileLoaded() { return is_gpr_data_loaded; }
    bool isMetalFileLoaded() { return is_metal_data_loaded; }
    bool isAirFileLoaded() { return is_air_data_loaded; }

signals:
    void layer_num_updated();                   // 检测到一层，更新层数
    void mouse_pos_on_profile_changed();        // 鼠标在B-Scan上移动的signal

public slots:
    void onReadButtonClicked(void);             // 读GPR文件
    void onDetectButtonClicked(void);           // 道相关检测
    // 当鼠标在B-Scan上点击时，进行检测的响应函数，功能其实同onDetectButtonClicked()
    void detectLayer(QPoint startPoint);
    void onSingleDetectButtonClicked(void);     // 逐道检测
    void onBpfAction(void);                     // BPF滤波
    bool outputReport(void);                    // 输出报表
    void removeDC();                            // 去除直流
    void reversePhase();                        // 数据反相
    void removeBackground();                    // 去除背景
    void gain();                                // 深部增益处理
    void onSetupRefWavesButtonClicked();        // 打开设置定标波形对话框
    void onRemoveCouplingPulseButtonClicked();        // 打开去除直耦波对话框

private:
    //---------------------GUI元素------------------------
    QTreeWidget *fileInfoTreeWidget;
    QLabel *dataPointsLabel;

    // GPR波形显示GUI
    CurvePlotter *curve_viewer;     // 显示A-Scan的控件
    Bscan_Viewer *bscan_viewer;     // 显示B-Scan的控件

    LayerExplorerPanel *layerPanel; // 反射层管理器面板

    //-----------------GPR文件头中的信息----------------
    QString gprFileName;            // 当前打开的GPR文件名
    int iSmpNum;                    // 每道点数(包括90字节道头+数据点数)
    int sampling_num;               // 每道点数 = iSmpNum - 45
    double fs;                      // 采样频率
    double timeWindow;              // 时窗
    int totalByteEachTrace;         // 每道字节数(90字节道头+道数据)
    int data_bytes_each_trace;      // 每道数据的字节数 = totalByteEachTrace - 90
    int traceNum;                   // 道数
    int layer_num;                  // 检测到的层位数
    QString fileName;               // 当前打开的GPR文件名
    //------------------保存GPR数据的数组----------------
    double **bscan_data;                // 保存GPR数据的数组
    QList<int> layer_position_list;     // 保存层位点的数组
    QVector<double> air_data;           // 对空数据
    QVector<double> metal_data;         // 金属板数据
    QString air_data_file_name;         // 载入的对空波形文件的路径
    QString metal_data_file_name;       // 载入的金属板波形文件的路径
    bool is_gpr_data_loaded;            // 标记GPR文件是否已经载入
    bool is_air_data_loaded;            // 标记对空采集数据是否已经加载
    bool is_metal_data_loaded;          // 标记金属板数据是否已经加载

    //------------------私有函数------------------------
    bool saveArrayAsTxt(double *array, int n);    //把数组保存到txt文件中
    bool readGprData(const QString &gprFileName);   //读取GPR格式文件
    void updateFileInfoTreeWidget(bool isInit = false);    // 更新fileInfoTreeWidget显示的内容
};

#endif // PAVEMENTEVALUATION_H
