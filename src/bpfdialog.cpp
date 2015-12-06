#include <QtGui>
#include "bpfdialog.h"
#include "curveplotter.h"

/**
 * @brief BpfDialog::BpfDialog
 * @param parent
 */
BpfDialog::BpfDialog(PavementEvaluation *p, QWidget *parent) :
    QDialog(parent)
{
    m_pavementEvaluation = p;

    // 原始波形CurvePlotter Settings
    m_originalCurvePlotter = new CurvePlotter;
    m_originalCurvePlotter->setDisplayStyle(CurvePlotter::HorizontalDisplay);
    m_originalCurvePlotter->setCurveColor(Qt::blue);
    m_originalCurvePlotter->setFixedSize(300, 150);
    m_originalCurvePlotter->showLocationLine(false);
    // 滤波后的波形CurvePlotter Settings
    m_filteredCurvePlotter = new CurvePlotter;
    m_filteredCurvePlotter->setDisplayStyle(CurvePlotter::HorizontalDisplay);
    m_filteredCurvePlotter->setCurveColor(Qt::red);
    m_filteredCurvePlotter->setFixedSize(300, 150);
    m_filteredCurvePlotter->showLocationLine(false);
    // 频谱图
    m_spectrumViewer = new SpectrumViewer;
    m_spectrumViewer->setFixedSize(300, 200);
    m_dbRadioButton = new QRadioButton(tr("dB"));
    m_dbRadioButton->setChecked(true);
    m_nomalizedRadioButton = new QRadioButton(tr("归一化"));

    // 设置面板
    m_SetupGroup = new QGroupBox;
    m_SetupGroup->setTitle(tr("滤波器参数设置"));
    filterTypeComboBox = new QComboBox;
    filterTypeComboBox->addItem(tr("低通滤波器"));
    filterTypeComboBox->addItem(tr("高通滤波器"));
    filterTypeComboBox->addItem(tr("带通滤波器"));
    filterTypeComboBox->addItem(tr("带阻滤波器"));
    windowTypeComboBox = new QComboBox;
    windowTypeComboBox->addItem(tr("Hanning窗"));
    windowTypeComboBox->addItem(tr("Hamming窗"));
    windowTypeComboBox->addItem(tr("Kaiser窗"));
    windowTypeComboBox->addItem(tr("Blackman窗"));
    windowTypeComboBox->addItem(tr("Bartlett窗"));
    windowLengthEdit = new QLineEdit;
    windowLengthEdit->setText(tr("64"));    // 默认窗长度为64
    windowLengthEdit->setAlignment(Qt::AlignRight);
    m_freqGroup = new QGroupBox;
    m_freqGroup->setTitle(tr("截止频率"));
    fhLineEdit = new QLineEdit;
    fhLineEdit->setText(tr("600"));     // 默认下限截止频率为600MHz
    fhLineEdit->setAlignment(Qt::AlignRight);
    flLineEdit = new QLineEdit;
    flLineEdit->setText(tr("1000"));    // 默认上限截止频率为1GHz
    flLineEdit->setAlignment(Qt::AlignRight);
    traceIndexSpinBox = new QSpinBox;
    traceIndexSpinBox->setRange(1, 10000);
    filterButton = new QPushButton(tr("滤波"));
    cancelButton = new QPushButton(tr("取消"));

    //---------------Signals & Slots-----------------
    connect(traceIndexSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(traceIndexChanged(int)));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(close()));

    //------------------Layout-----------------------
    QHBoxLayout *upLeftLayout = new QHBoxLayout;
    upLeftLayout->addWidget(new QLabel(tr("原始信号频谱")));
    upLeftLayout->addStretch();
    upLeftLayout->addWidget(new QLabel(tr("幅值显示方式：")));
    upLeftLayout->addWidget(m_dbRadioButton);
    upLeftLayout->addWidget(m_nomalizedRadioButton);

    //截止频率面板
    QGridLayout *freqPannelLayout = new QGridLayout;
    freqPannelLayout->addWidget(new QLabel(tr("低频截止频率：")), 0, 0);
    freqPannelLayout->addWidget(flLineEdit, 0, 1);
    freqPannelLayout->addWidget(new QLabel(tr("(MHz)")), 0, 2);
    freqPannelLayout->addWidget(new QLabel(tr("高频截止频率：")), 1, 0);
    freqPannelLayout->addWidget(fhLineEdit, 1, 1);
    freqPannelLayout->addWidget(new QLabel(tr("(MHz)")), 1, 2);
    freqPannelLayout->setAlignment(Qt::AlignLeft);
    m_freqGroup->setLayout(freqPannelLayout);

    // 滤波器设置面板Layout
    QGridLayout *setupPannelLayout = new QGridLayout;
    setupPannelLayout->addWidget(new QLabel(tr("滤波器类型：")), 0, 0);
    setupPannelLayout->addWidget(filterTypeComboBox, 0, 1, 1, 2);
    setupPannelLayout->addWidget(new QLabel(tr("窗函数类型：")), 1, 0);
    setupPannelLayout->addWidget(windowTypeComboBox, 1, 1, 1, 2);
    setupPannelLayout->addWidget(new QLabel(tr("滤波器长度：")), 2, 0);
    setupPannelLayout->addWidget(windowLengthEdit, 2, 1);
    setupPannelLayout->addWidget(new QLabel(tr("(点)")), 2, 2);
    setupPannelLayout->addWidget(m_freqGroup, 3, 0, 5, 3);
    setupPannelLayout->addWidget(new QLabel(tr("选择预览道数：")), 8, 0);
    setupPannelLayout->addWidget(traceIndexSpinBox, 8, 1);
    setupPannelLayout->addWidget(new QLabel(tr("(1- %1)").arg(m_pavementEvaluation->traceNum)), 8, 2);
    setupPannelLayout->setAlignment(Qt::AlignLeft);
    m_SetupGroup->setLayout(setupPannelLayout);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(filterButton);
    buttonLayout->addWidget(cancelButton);


    //对话框Layout
    QGridLayout *dialogLayout = new QGridLayout;
    dialogLayout->addLayout(upLeftLayout, 0, 0);
    dialogLayout->addWidget(m_spectrumViewer, 1, 0);
    dialogLayout->addWidget(m_SetupGroup, 0, 1, 2, 1);
    dialogLayout->addWidget(new QLabel(tr("原始信号波形:")), 2, 0);
    dialogLayout->addWidget(new QLabel(tr("滤波后的信号波形:")), 2, 1);
    dialogLayout->addWidget(m_originalCurvePlotter, 3, 0);
    dialogLayout->addWidget(m_filteredCurvePlotter, 3, 1);
    dialogLayout->addLayout(buttonLayout, 4, 1);
    dialogLayout->setAlignment(Qt::AlignCenter);

    //-----------Configuration------------
    setLayout(dialogLayout);
    //setFixedSize(this->sizeHint());
    setWindowTitle(tr("信号滤波设置"));
    //setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/**
 * @brief BpfDialog::loadOriginalCurveData
 * @param data
 */
void BpfDialog::loadOriginalCurveData(const QVector<QPointF> &data)
{
    m_originalCurvePlotter->setAxisX(originalMinX, originalMaxX);
    m_originalCurvePlotter->setAxisY(originalMinY, originalMaxY);
    m_originalCurvePlotter->setCurveData(0, data);

    // 显示频谱
    m_spectrumViewer->loadSignal(data, 40e9);

    update();
}

/**
 * @brief BpfDialog::loadFilteredCurveData
 * @param data
 */
void BpfDialog::loadFilteredCurveData(const QVector<QPointF> &data)
{
    m_filteredCurvePlotter->setAxisX(filteredMinX, filteredMaxX);
    m_filteredCurvePlotter->setAxisY(filteredMinY, filteredMaxY);
    m_filteredCurvePlotter->setCurveData(0, data);
    update();
}


/**
 * @brief 设置原始波形的Plotter的坐标轴
 * @param minX
 */
void BpfDialog::setOriginalPlotterAxis(int minX, int maxX, int minY, int maxY)
{
    this->originalMinX = minX;
    this->originalMaxX = maxX;
    this->originalMinY = minY;
    this->originalMaxY = maxY;
}

/**
 * @brief 设置滤波后的Plotter的坐标轴
 * @param minX
 */
void BpfDialog::setFilteredPlotterAxis(int minX, int maxX, int minY, int maxY)
{
    this->filteredMinX = minX;
    this->filteredMaxX = maxX;
    this->filteredMinY = minY;
    this->filteredMaxY = maxY;
}


/**
 * @brief 预览道数指示条变化的响应事件:更新对话框中原始波形及频谱和滤波后波形
 */
void BpfDialog::traceIndexChanged(int new_index)
{
    // 更新保存道数的变量
    setTraceIndex(new_index);
    qDebug() << "改变了预览道数: " << new_index;
    QVector<QPointF> points;
    for (int i = 0; i < m_pavementEvaluation->sampling_num; i++)
        points.append(QPointF(i, m_pavementEvaluation->bscan_data[new_index][i]));
    loadOriginalCurveData(points);      // 载入原始波形

}








