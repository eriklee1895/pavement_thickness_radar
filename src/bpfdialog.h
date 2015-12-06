#ifndef BPFDIALOG_H
#define BPFDIALOG_H

#include <QDialog>
#include "spectrumviewer.h"
#include "pavementevaluation.h"


class QLabel;
class QSpinBox;
class QPushButton;
class CurvePlotter;
class QComboBox;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QListWidget;

class BpfDialog : public QDialog
{
    Q_OBJECT
public:
    BpfDialog(PavementEvaluation *p, QWidget *parent = 0);
    void loadOriginalCurveData(const QVector<QPointF> &data);
    void loadFilteredCurveData(const QVector<QPointF> &data);
    int getTraceIndex() {return traceIndex;}
    void setTraceIndex(int index) {traceIndex = index;}
    void setOriginalPlotterAxis(int minX, int maxX, int minY, int maxY);
    void setFilteredPlotterAxis(int minX, int maxX, int minY, int maxY);

signals:

public slots: 
    void traceIndexChanged(int new_index);
private:

    // -------------UI元素-------------
    SpectrumViewer *m_spectrumViewer;   // 显示频谱图
    CurvePlotter *m_originalCurvePlotter;   // 显示原始波形
    CurvePlotter *m_filteredCurvePlotter;   // 显示滤波后的波形
    QSpinBox *traceIndexSpinBox;            // 预览道数
    QPushButton *filterButton;              // 滤波按钮
    QPushButton *cancelButton;              // 取消按钮
    QGroupBox *m_SetupGroup;                // 滤波器参数设置面板
    QGroupBox *m_freqGroup;                 // 截止频率设置面板
    QRadioButton *m_dbRadioButton;          // 频谱幅度为dB
    QRadioButton *m_nomalizedRadioButton;   // 频谱幅度为归一化
    QComboBox *filterTypeComboBox;          // 滤波器类型选择框
    QComboBox *windowTypeComboBox;          // 窗函数类型选择框
    QLineEdit *windowLengthEdit;            // 窗函数长度选择文本框
    QLineEdit *fhLineEdit;                  // 上限截止频率选择文本框
    QLineEdit *flLineEdit;                  // 下限截止频率选择文本框



    // -------------数据成员-------------
    PavementEvaluation *m_pavementEvaluation;
    int traceIndex;     // 要显示波形的道数
    // 显示原始波形的CurvePlotter的坐标轴参数
    int originalMinX;
    int originalMaxX;
    int originalMinY;
    int originalMaxY;
    // 显示滤波后波形的CurvePlotter的坐标轴参数
    int filteredMinX;
    int filteredMaxX;
    int filteredMinY;
    int filteredMaxY;

};

#endif // BPFDIALOG_H
