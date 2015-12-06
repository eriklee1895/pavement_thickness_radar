/****************************************************************************
**
** 探地雷达路面检测软件
** 中科院电子所十室
**
** Created by Erik Lee
** Time:2013/05
** Version: v0.1
** Platform: Window / Linux
** 开发工具: Qt 4.8.4 + Qt Creator
** Language: C++
**
** 文件名： pavementevaluation.cpp
** 功能：主窗口中央部件的实现
**
****************************************************************************/

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

#include "layer_detector.h"
#include "parameterdialog.h"
#include "pavementevaluation.h"
#include "bpfdialog.h"
#include "loadrefwavedialog.h"
#include "removecouplingpulsedialog.h"
#include "dsptools.h"
#include "thicknessdialog.h"



//-------------------------------------------------------------------------------------------------
//函数名称：PavementEvaluation()
//函数说明：PavementEvaluation类的构造函数，功能为创建主窗口界面
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
PavementEvaluation::PavementEvaluation(QWidget *parent)
    : QWidget(parent)
{
    //--------------------创建B-Scan与A-Scan--------------------------
    curve_viewer = new CurvePlotter;
    curve_viewer->setDisplayStyle(CurvePlotter::VerticalDisplay);
    curve_viewer->setFixedWidth(120);
    bscan_viewer = new Bscan_Viewer(curve_viewer, this);

    //---------------创建用来显示文件属性的TreeWidget-------------------------------
    fileInfoTreeWidget = new QTreeWidget();
    fileInfoTreeWidget->setHeaderLabels(
                QStringList() << tr("文件属性") << tr("属性值"));
    //在TreeWidget中显示文件信息
    updateFileInfoTreeWidget(true);

    QGroupBox *fileInfoGroupBox = new QGroupBox;
    fileInfoGroupBox->setTitle(tr("GPR文件信息"));
    QHBoxLayout *fileInfoLayout = new QHBoxLayout;
    fileInfoLayout->addWidget(fileInfoTreeWidget);
    fileInfoGroupBox->setLayout(fileInfoLayout);


    //----------创建LayerExplorerPannel-----------
    layerPanel = new LayerExplorerPanel;

    //---------------------信号与槽-------------------------------
    connect(bscan_viewer, SIGNAL(mouse_pos_changed()),
            this, SIGNAL(mouse_pos_on_profile_changed()));
    connect(bscan_viewer, SIGNAL(mouseClicked(QPoint)),
            this, SLOT(detectLayer(QPoint)));

    //---------------------布局-----------------------------
    //左侧布局
    QVBoxLayout *leftLayout = new QVBoxLayout;
    fileInfoTreeWidget->setFixedWidth(200);
    leftLayout->addWidget(fileInfoGroupBox);
    leftLayout->addStretch();
    leftLayout->addWidget(layerPanel, 10);


    //-----B-Scan/A-Scna布局--------
    QHBoxLayout *profileLayout = new QHBoxLayout;
    profileLayout->addWidget(bscan_viewer);
    profileLayout->addWidget(curve_viewer);

    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout,1);
    mainLayout->addLayout(profileLayout, 20);

    // 设置主布局为mainLayout
    setLayout(mainLayout);

    // -------------------默认参数----------------
    is_air_data_loaded = false;         // 默认没有载入对空数据
    is_metal_data_loaded = false;       // 默认没有载入金属板数据
    is_gpr_data_loaded = false;         // 默认没有加载GPR数据文件
}


/**
 * @brief PavementEvaluation::~PavementEvaluation
 */
PavementEvaluation::~PavementEvaluation()
{
    
}

/**
 * @brief 修改采样率和采样时窗
 * @param freq
 */
void PavementEvaluation::setSampingFrequency(float freq)
{
    // 修改采样频率和时窗
    fs = freq;
    timeWindow = 1 / fs * sampling_num;
    qDebug() << "new fs = " << fs << ", new time window = " << timeWindow << endl;
    // 更新信息列表
    updateFileInfoTreeWidget();
}


//-------------------------------------------------------------------------------------------------
//函数名称：readGprData()
//函数说明：读取GPR文件，把文件头信息和数据分别存入相应变量
//参数说明：
//输出：gprFileName - 用户选择的GPR文件
//-------------------------------------------------------------------------------------------------
bool PavementEvaluation::readGprData(const QString &gprFileName)
{
    //保存文件头信息的结构体
    _GPR_HEAD gprFileHead;
    //读文件头
    QFile fileRead(gprFileName);
    if (!fileRead.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("GPR Viewer"),
                             tr("读取文件失败！"),
                             QMessageBox::Yes);
        return false;
    }
    fileRead.read((char *)&gprFileHead, sizeof(_GPR_HEAD));
    //从文件头中提取信息
    // 每道点数(包括90字节道头+数据点数)
    iSmpNum = gprFileHead.iSmpNum;
    //每道数据点数
    sampling_num = iSmpNum - 90/sizeof(ushort);
    // 采样频率
    fs = gprFileHead.dFreq;
    // 时窗
    timeWindow = gprFileHead.dTimeWnd;
    // 每道字节数
    totalByteEachTrace = iSmpNum *sizeof(ushort);
    //每道数据的字节数
    data_bytes_each_trace = totalByteEachTrace - 90;
    // 道数
    traceNum = (fileRead.size() - sizeof(_GPR_HEAD)) / totalByteEachTrace;
    // 初始检测到的层位为0
    layer_num = 0;

    //在fileInfoTreeWidget中更新显示文件信息
    updateFileInfoTreeWidget();

    //初始化进度条
    QProgressDialog *readingProgressDialog = new QProgressDialog;
    readingProgressDialog->setMinimumDuration(5);   //进度条至少显示5ms
    readingProgressDialog->setWindowTitle(tr("Please Wait"));
    readingProgressDialog->setLabelText(tr("读取GPR数据中..."));
    readingProgressDialog->setCancelButtonText(tr("取消"));
    readingProgressDialog->setRange(0,traceNum-1);
    //模态对话框，在其没有被关闭之前，用户不能与同一个应用程序的其他窗口进行交互
    readingProgressDialog->setModal(true);


    // 分配内存
    char *trace_head = new char[90];    //每道道头长度为90字节
    ushort *usData = new ushort[sampling_num];   //一次读取的每道数据
    ushort *usDataDeal = new ushort[sampling_num];

    bscan_data = new double *[traceNum];
    for(int i = 0; i < traceNum; i++){
        bscan_data[i] = new double[sampling_num];
    }

    // 定位文件指针
    int iStartPos = sizeof(_GPR_HEAD) + 6;
    fileRead.seek(iStartPos);
    //逐道读取数据
    for (int col = 0; col < traceNum; col++) {
        fileRead.read((char *)trace_head, 90);    //先读90字节道头
        //读本道数据
        if (fileRead.read((char *)usData, data_bytes_each_trace) == data_bytes_each_trace) {
            //进度条更新一次
            readingProgressDialog->setValue(col);
            if (readingProgressDialog->wasCanceled()){
                fileRead.remove();
                if (usData != NULL)
                {
                    delete[] usData;
                    usData = NULL;
                }
                if (usDataDeal != NULL)
                {
                    delete[] usDataDeal;
                    usDataDeal = NULL;
                }
                return false;
            }
            //把本道数据存入bscan_data[col][row]
            for (int i = 0; i < sampling_num; i++){
                bscan_data[col][i] = usData[i];
            }
        }
    }

    //释放资源
    fileRead.close();
    if (usData != NULL)
    {
        delete[] usData;
        usData = NULL;
    }
    if (usDataDeal != NULL)
    {
        delete[] usDataDeal;
        usDataDeal = NULL;
    }
//    QMessageBox::information(NULL, "读取GPR文件", "读取GPR文件完毕！", QMessageBox::Yes);


    //去直流
    for(int i = 0; i < traceNum; i++){
        dspTools::subtract_mean(bscan_data[i], sampling_num);
    }
//    QMessageBox::information(NULL, "去直流", "去直流完毕！", QMessageBox::Yes);

    return true;
}



//-------------------------------------------------------------------------------------------------
//函数名称：onReadButtonClicked()
//函数说明：读取GPR文件事件
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onReadButtonClicked()
{
    gprFileName = QFileDialog::getOpenFileName(
                this,
                tr("打开GPR文件"), ".",
                tr("GPR 文件 (*.gpr)"));
    if (!gprFileName.isEmpty()) {
        if (readGprData(gprFileName)) {
            // 画出Bscan剖面图
            bscan_viewer->setData(bscan_data, sampling_num, traceNum);
            bscan_viewer->clearAllLayerTracks();    // 清除所有层轨迹
            // 在顶部显示GPR文件名
            QStringList name = gprFileName.split("/");
            bscan_viewer->setGprFileName(name.last());
            // 更新GPR文件载入标志
            is_gpr_data_loaded = true;
            is_air_data_loaded = false;
            is_metal_data_loaded = false;
        }
    }

}



//-------------------------------------------------------------------------------------------------
//函数名称：onDetectButtonClicked()
//函数说明：进行倒相关层位检测，目前一次只检测单层
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onDetectButtonClicked()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }
    //------------------检测参数设置对话框声明-----------------------------
    ParameterDialog *setupDialog = new ParameterDialog(this);
    setupDialog->show();
    setupDialog->raise();
    setupDialog->activateWindow();
    setupDialog->setModal(true);
    setupDialog->firstSearchPointHintLabel->setText(tr("(1-%1)")
                                                    .arg(sampling_num));
    setupDialog->startHintLabel->setText(tr("(1-%1)")
                                          .arg(traceNum));

    //---------------------检测参数声明-------------------------
    int first_search_point;
    int start_trace;
    int ref_win;
    int search_win;

    LayerDetector detector;
    //保存层位点的数组
    int *layer_position_array = new int[traceNum];
    //初始化数组
    for (int i = 0; i < traceNum; i++) {
        layer_position_array[i] = 0;
    }

    // 当按下参数设置对话框的确定按钮时，传递参数并进行检测
    if (setupDialog->exec()){
        //--------------------把检测参数传递到参数变量中--------------------------
        first_search_point = setupDialog->firstSearchPointEdit->text().toInt() - 1;
        start_trace = setupDialog->startTraceEdit->text().toInt() - 1;
        ref_win = setupDialog->refWindEdit->text().toInt();
        search_win = setupDialog->searchWindEdit->text().toInt();

        //使用道相关检测器来检测，layer_position_array为保存层位结果的数组
        detector.trace_corr_detector(bscan_data, traceNum, first_search_point, start_trace, ref_win, search_win, layer_position_array);
        // 为了与单道检测一致，把layer_position_array内容复制到layer_position_list
        layer_position_list.clear();
        for (int i = 0; i < sampling_num; i++)
            layer_position_list << layer_position_array[i];

        //检测到的层位数+1
        layer_num++;

//        QMessageBox::information(NULL, "路面厚度检测软件", "反射层追踪完毕!", QMessageBox::Yes);

        // 把检测结果保存到Txt文件中
        //saveArrayAsTxt(layer_position_array, iTrcNum);

        emit layer_num_updated();

        // 在B-Scan上画出层
        QVector<QPointF> layer;
        for (int i = 0; i < traceNum; i++)
            layer.append(QPointF(i, layer_position_array[i]));
        bscan_viewer->setLayer(layer_num, layer);
    }
}

/**
 * @brief 自动检测模式，在B-Scans上点击鼠标，自动以该点为起点，
 *        弹出检测反射层对话框，检测反射层
 * @param startPoint - 检测起始点
 */
void PavementEvaluation::detectLayer(QPoint startPoint)
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    qDebug() << "接收鼠标按下事件，发送的点为(" << startPoint.x() << ","
             << startPoint.y() << ")" << endl;
    //------------------检测参数设置对话框声明-----------------------------
    ParameterDialog *setupDialog = new ParameterDialog(this);
    setupDialog->show();
    setupDialog->raise();
    setupDialog->activateWindow();
    setupDialog->setModal(true);
    setupDialog->firstSearchPointHintLabel->setText(tr("(1-%1)")
                                                    .arg(sampling_num));
    setupDialog->startHintLabel->setText(tr("(1-%1)")
                                          .arg(traceNum));
    setupDialog->firstSearchPointEdit->setText(tr("%1").arg(startPoint.y()));
    setupDialog->startTraceEdit->setText(tr("%1").arg(startPoint.x()));

    //---------------------检测参数声明-------------------------
    int first_search_point;
    int start_trace;
    int ref_win;
    int search_win;

    LayerDetector detector;
    //保存层位点的数组
    int *layer_position_array = new int[traceNum];
    //初始化数组
    for (int i = 0; i < traceNum; i++) {
        layer_position_array[i] = 0;
    }

    // 当按下参数设置对话框的确定按钮时，传递参数并进行检测
    if (setupDialog->exec()){
        //--------------------把检测参数传递到参数变量中--------------------------
        first_search_point = setupDialog->firstSearchPointEdit->text().toInt() - 1;
        start_trace = setupDialog->startTraceEdit->text().toInt() - 1;
        ref_win = setupDialog->refWindEdit->text().toInt();
        search_win = setupDialog->searchWindEdit->text().toInt();

        //---------------------测试参数是否正确传递------------------------------
        qDebug() << "first_search_point = " << first_search_point;
        qDebug() << "start_trace = " << start_trace;
        qDebug() << "ref_win = " << ref_win;
        qDebug() << "search_win = " << search_win;

        //使用道相关检测器来检测，layer_position_array为保存层位结果的数组
        detector.trace_corr_detector(bscan_data, traceNum, first_search_point, start_trace, ref_win, search_win, layer_position_array);

        //检测到的层位数+1
        layer_num++;

        QMessageBox::information(NULL, "路面检测软件", "反射层提取完毕!", QMessageBox::Yes);

        // 把检测结果保存到Txt文件中
        //saveArrayAsTxt(layer_position_array, iTrcNum);

        emit layer_num_updated();

        // 在bscan上画出层
        QVector<QPointF> layer;
        for (int i = 0; i < traceNum; i++)
            layer.append(QPointF(i, layer_position_array[i]));
        bscan_viewer->setLayer(layer_num, layer);
    }
}


//-------------------------------------------------------------------------------------------------
//函数名称：onSingleDetectButtonClicked()
//函数说明：测试单道检测功能
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onSingleDetectButtonClicked()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    //------------------检测参数设置对话框声明-----------------------------
    ParameterDialog *setupDialog = new ParameterDialog(this);
    setupDialog->show();
    setupDialog->raise();
    setupDialog->activateWindow();
    setupDialog->setModal(true);
    setupDialog->firstSearchPointHintLabel->setText(tr("(1-%1)")
                                                    .arg(sampling_num));
    setupDialog->startHintLabel->setText(tr("(1-%1)")
                                          .arg(traceNum));

    //---------------------检测参数声明-------------------------
    int first_search_point;
    int start_trace;
    int ref_win;
    int search_win;

    LayerDetector detector;
//    //保存层位点的数组
//    int *layer_position_array = new int[iTrcNum];
//    //初始化数组
//    for (int i = 0; i < iTrcNum; i++) {
//        layer_position_array[i] = 0;
//    }

    // 当按下参数设置对话框的确定按钮时，传递参数并进行检测
    if (setupDialog->exec()){
        //--------------------把检测参数传递到参数变量中--------------------------
        first_search_point = setupDialog->firstSearchPointEdit->text().toInt() - 1;
        start_trace = setupDialog->startTraceEdit->text().toInt() - 1;
        ref_win = setupDialog->refWindEdit->text().toInt();
        search_win = setupDialog->searchWindEdit->text().toInt();

        //---------------------测试参数是否正确传递------------------------------
        qDebug() << "first_search_point = " << first_search_point;
        qDebug() << "start_trace = " << start_trace;
        qDebug() << "ref_win = " << ref_win;
        qDebug() << "search_win = " << search_win;


        //初始化进度条
        QProgressDialog progressingDialog;
        progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
        progressingDialog.setWindowTitle(QObject::tr("层位追踪"));
        progressingDialog.setLabelText(QObject::tr("层位追踪中，请稍等..."));
        progressingDialog.setRange(0, traceNum-1);
        progressingDialog.setModal(true);

        //测试单道的道相关检测器来检测
        // layer_position_array为保存层位结果的数组
        layer_position_list.clear();
        for(int i = 0; i < traceNum; i++) {
            progressingDialog.setValue(i);
            layer_position_list << detector.trace_corr_detector_by_single(bscan_data[i], first_search_point, ref_win, search_win);
        }


        //检测到的层位数+1
        layer_num++;

        QMessageBox::information(NULL, "路面检测软件", "道相关追踪完毕!", QMessageBox::Yes);

        // 把检测结果保存到Txt文件中
        //saveArrayAsTxt(layer_position_array, iTrcNum);

        emit layer_num_updated();
    }


}


//-------------------------------------------------------------------------------------------------
//函数名称：saveArrayAsTxt()
//函数说明：把指定数组保存为Txt格式
//参数说明： array  -  待保存为Txt的数组
//           n    -  数组大小
//输出：
//-------------------------------------------------------------------------------------------------
bool PavementEvaluation::saveArrayAsTxt(double *array, int n)
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("把Table选中列保存为Txt文件"), ".",
                                                    tr("Txt文件 (*.txt)"));
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    //写入文件失败
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("路面厚度检测软件"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    //在写文件的过程中，把光标设为等待光标
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //把Table当前列保存到Txt文件中
    for (int i = 0; i < n; i++){
        out << array[i] << endl;
    }
    //文件写完后把光标恢复
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, "写入Txt文件", "写入Txt文件完毕！", QMessageBox::Yes);
    return true;
}

//-------------------------------------------------------------------------------------------------
//函数名称：onBpfButtonClicked()
//函数说明：对数据进行带通滤波，BPF为Hamming窗生成的FIR BPF，使用的预先设计好的滤波器系数
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onBpfAction(void)
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    BpfDialog *bpfDialog = new BpfDialog(this); // 建立滤波器对话框
    bpfDialog->setOriginalPlotterAxis(0, sampling_num, -32768, 32768);      // 原始波形Plotter坐标设置
    bpfDialog->setFilteredPlotterAxis(0, sampling_num, -32768, 32768);      // 滤波后波形Plotter窗口内的坐标设置
    double bpfResult[1024];                     // 保存结果
    double a[1] = { 1 };                        // FIR系统的分母系数
    // ---------------- 载入原始数据 ------------------
    QVector<QPointF> originalPoints;
    for (int i = 0; i < 1024; i++)
        originalPoints.append(QPointF(i, bscan_data[1293][i]));
    bpfDialog->loadOriginalCurveData(originalPoints);
    // 600MHz - 2GHz Hamming BPF FIR, Fs = 40GHz
    //int bpf_length = 65;    // 滤波器长度
    double bpf_coeff1[65] = {   // 滤波器系数
      -0.0006356060389334,-0.0004984463938134,-0.000326249581182,-0.0001081382758523,
      0.0001552814424625,0.0004379567460369, 0.000677455435689,0.0007702865478317,
      0.0005759050511386,-6.949082800545e-005,-0.001329930207681,-0.003339730386179,
      -0.006170537629441,-0.009800335196914, -0.01408981421165, -0.01877109661968,
       -0.02345257193798,  -0.0276416808267, -0.03078508387551, -0.03232311396142,
       -0.03175308730984, -0.02869429379135, -0.02294658993474, -0.01453465943464,
      -0.003731227317539, 0.008945293573041,  0.02276033129049,  0.03681819109012,
        0.05013771824321,  0.06174098284662,  0.07074552294356,  0.07644995425929,
        0.07840327863968,  0.07644995425929,  0.07074552294356,  0.06174098284662,
        0.05013771824321,  0.03681819109012,  0.02276033129049, 0.008945293573041,
      -0.003731227317539, -0.01453465943464, -0.02294658993474, -0.02869429379135,
       -0.03175308730984, -0.03232311396142, -0.03078508387551,  -0.0276416808267,
       -0.02345257193798, -0.01877109661968, -0.01408981421165,-0.009800335196914,
      -0.006170537629441,-0.003339730386179,-0.001329930207681,-6.949082800545e-005,
      0.0005759050511386,0.0007702865478317, 0.000677455435689,0.0004379567460369,
      0.0001552814424625,-0.0001081382758523,-0.000326249581182,-0.0004984463938134,
      -0.0006356060389334
    };
//    // 滤波
//    dspTools::filter(bpfResult, bpf_coeff1, 65, a, 1, bscan_data[1000], 1024);
//    // 把滤波结果显示出来
//    QVector<QPointF> filteredPoints;
//    for (int i = 0; i < 1024; i++)
//        filteredPoints.append(QPointF(i, bpfResult[i]));
//    bpfDialog->loadFilteredCurveData(filteredPoints);
    // ------------ 确定新的坐标轴-----------------------------
//    QVector<double> values;
//    for (int i = 0; i < filteredPoints.count(); i++)
//        values.append(filteredPoints[i].y());
//    qSort(values);
//    qDebug() << "count: " << filteredPoints.count() << ", min value = " << values[0] << ", max value = " << values[filteredPoints.count()-1] << endl;
//    bpfDialog->setFilteredPlotterAxis(0, sampling_num, values[0], values[filteredPoints.count()-1]);
//    bpfDialog->loadFilteredCurveData(filteredPoints);
    // ------------------------------------------------------

    // ------------ 对B-Scan滤波 -----------------------------
    qDebug() << "Enter B-Scan Filter";

    QProgressDialog progressingDialog(this);
    progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
    progressingDialog.setWindowTitle(QObject::tr("BPF滤波"));
    progressingDialog.setLabelText(QObject::tr("滤波中，请稍等..."));
    progressingDialog.setRange(0, traceNum-1);
    progressingDialog.setModal(true);
    // 逐道滤波
    for (int i = 0; i != traceNum; i++) {
        progressingDialog.setValue(i);
        dspTools::filter(bpfResult, bpf_coeff1, 65, a, 1, bscan_data[i], 1024);
        memcpy(bscan_data[i], bpfResult, sizeof(double)*sampling_num);
    }
    qDebug() << "Filter successful";
    bscan_viewer->setData(bscan_data, sampling_num, traceNum);
    QMessageBox::information(NULL, "BPF滤波", "完成滤波！", QMessageBox::Yes);
    qDebug() << "Exit B-Scan Filter";

    // 显示滤波预览
    dspTools::filter(bpfResult, bpf_coeff1, 65, a, 1, bscan_data[1293], 1024);
    QVector<QPointF> filteredPoints;
    for (int i = 0; i < 1024; i++)
        filteredPoints.append(QPointF(i, bpfResult[i]));
    bpfDialog->loadFilteredCurveData(filteredPoints);
    bpfDialog->show();
//    bpfDialog->raise();
//    bpfDialog->activateWindow();
//    bpfDialog->setModal(true);

}


//-------------------------------------------------------------------------------------------------
//函数名称：outputReport()
//函数说明：输出Txt格式的路面检测报表
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
bool PavementEvaluation::outputReport()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return false;
    }

    //--------显示厚度检测结果对话框-------------
    ThicknessDialog thicknessDialog(this);
    if (thicknessDialog.exec()) {

    }

    return true;
}



/**
 * @brief 数据去直流
 */
void PavementEvaluation::removeDC()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    dspTools::removeDC(bscan_data, traceNum, sampling_num);
    // 更新B-Scan显示
    bscan_viewer->setData(bscan_data, sampling_num, traceNum);
}


/**
 * @brief 数据反相
 */
void PavementEvaluation::reversePhase()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    dspTools::reversePhase(bscan_data, traceNum, sampling_num);
    // 更新B-Scan显示
    bscan_viewer->setData(bscan_data, sampling_num, traceNum);
}


/**
 * @brief 去除背景
 */
void PavementEvaluation::removeBackground()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    dspTools::removeBackground(bscan_data, traceNum, sampling_num);
    // 更新B-Scan显示
    bscan_viewer->setData(bscan_data, sampling_num, traceNum);
}


/**
 * @brief 深部增益处理
 */
void PavementEvaluation::gain()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    dspTools::gainFunc(bscan_data, traceNum, sampling_num);
    // 更新B-Scan显示
    bscan_viewer->setData(bscan_data, sampling_num, traceNum);
}



/**
 * @brief // 更新fileInfoTreeWidget显示的内容
 * @param isInit - true:只在程序启动时初始化使用。此时只显示出参数名称，参数值不列出。
 *                 false：参数名称与参数值同时更新
 */
void PavementEvaluation::updateFileInfoTreeWidget(bool isInit)
{
    fileInfoTreeWidget->clear();

    QTreeWidgetItem *treeItem1 = new QTreeWidgetItem(fileInfoTreeWidget);
    treeItem1->setText(0, tr("文件名"));
    if (!isInit) treeItem1->setText(1, gprFileName);
    fileInfoTreeWidget->addTopLevelItem(treeItem1);

    QTreeWidgetItem *treeItem2 = new QTreeWidgetItem(fileInfoTreeWidget);
    treeItem2->setText(0, tr("采样点数"));
    if (!isInit) treeItem2->setText(1, QString::number(sampling_num) + "点");
    fileInfoTreeWidget->addTopLevelItem(treeItem2);

    QTreeWidgetItem *treeItem3 = new QTreeWidgetItem;
    treeItem3->setText(0, tr("采样频率"));
    if (!isInit) treeItem3->setText(1, QString::number(fs) + "GHz");
    fileInfoTreeWidget->addTopLevelItem(treeItem3);

    QTreeWidgetItem *treeItem4 = new QTreeWidgetItem;
    treeItem4->setText(0, tr("采样时窗"));
    if (!isInit) treeItem4->setText(1, QString::number(timeWindow) + "ns");
    fileInfoTreeWidget->addTopLevelItem(treeItem4);

    QTreeWidgetItem *treeItem5 = new QTreeWidgetItem;
    treeItem5->setText(0, tr("每道字节数"));
    if (!isInit) treeItem5->setText(1, QString::number(totalByteEachTrace) + "字节");
    fileInfoTreeWidget->addTopLevelItem(treeItem5);

    QTreeWidgetItem *treeItem6 = new QTreeWidgetItem;
    treeItem6->setText(0, tr("采样道数"));
    if (!isInit) treeItem6->setText(1, QString::number(traceNum));
    fileInfoTreeWidget->addTopLevelItem(treeItem6);

    QTreeWidgetItem *treeItem7 = new QTreeWidgetItem;
    treeItem7->setText(0, tr("文件头字节数"));
    if (!isInit) treeItem7->setText(1, QString::number(sizeof(_GPR_HEAD)) + "字节");
    fileInfoTreeWidget->addTopLevelItem(treeItem7);
}

/**
 * @brief 载入参考波形
 */
void PavementEvaluation::onSetupRefWavesButtonClicked()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    //------------------设置参考波形对话框声明-----------------------------
    LoadRefWaveDialog *setupRefWavesDialog = new LoadRefWaveDialog(this, this);
    setupRefWavesDialog->show();
    setupRefWavesDialog->raise();
    setupRefWavesDialog->activateWindow();
    setupRefWavesDialog->setModal(true);

    //---------------------定标参数声明-------------------------
    QString airDataFileName;
    QString metalDataFileName;

    // 当按下定标波形参数设置对话框的确定按钮时
    if (setupRefWavesDialog->exec()){

    }
}



/**
 * @brief 去除直耦波对话框
 */
void PavementEvaluation::onRemoveCouplingPulseButtonClicked()
{
    if (!is_gpr_data_loaded) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }


    //------------------去直耦波对话框声明-----------------------------
    RemoveCouplingPulseDialog *removeCouplingPulseDialog = new RemoveCouplingPulseDialog(this);
    removeCouplingPulseDialog->show();
    removeCouplingPulseDialog->raise();
    removeCouplingPulseDialog->activateWindow();
    removeCouplingPulseDialog->setModal(true);

    //---------------------定标参数声明-------------------------

    // 当按下定标波形参数设置对话框的确定按钮时
    if (removeCouplingPulseDialog->exec()){
    }

    // 释放资源
    if (removeCouplingPulseDialog)
        delete removeCouplingPulseDialog;


    // 如果已经载入了对空波形，则
    if (is_air_data_loaded) {
        // 去耦合波进度条
        QProgressDialog progressingDialog(this);
        progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
        progressingDialog.setWindowTitle(QObject::tr("去除直耦波"));
        progressingDialog.setLabelText(QObject::tr("去除直耦波中，请稍等..."));
        progressingDialog.setRange(0, traceNum-1);
        progressingDialog.setModal(true);

        // 逐道去直耦波，目前放弃相关对齐方法，感觉直接找前256点波形最值也可以实现对齐
        for (int i = 0; i < traceNum; i++){
            // 更新进度条
            progressingDialog.setValue(i);

            QVector<double> temp_air(air_data); // 每道暂存的对空波形，因为每道都要移位
            // 分别找出对空波形和当前道数据波形的前256点峰值最大值的下表
            int index_air = dspTools::max_in_QVector(air_data, 160);
            int index_signal = dspTools::max_in_array(bscan_data[i], 160);
            // 对空信号相对本道GPR信号的偏移量
            int offset = index_air - index_signal;
            // 将信号幅值拉伸至与对空信号一致
            int ratio = air_data[index_air]/bscan_data[i][index_signal];
            qDebug() << "offset = " << offset << ", ratio = " << ratio;
            // 对空数据移位以便与实采数据对齐
            if (offset > 0)
                dspTools::shift_vector_left(temp_air, offset);
            if (offset < 0)
                dspTools::shift_vector_right(temp_air, offset);

            index_air = dspTools::max_in_QVector(temp_air, 256);
            index_signal = dspTools::max_in_array(bscan_data[i], 256);


            // 信号 - 对空波形 = 去直耦波
            for (int j = 0; j < sampling_num; j++) {
                bscan_data[i][j] = ratio*bscan_data[i][j] - temp_air[j];
            }
        }
        // 重绘B-Scan剖面图
        bscan_viewer->setData(bscan_data, sampling_num, traceNum);
    }
}






