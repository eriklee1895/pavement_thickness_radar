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


//-------------------------------------------------------------------------------------------------
//函数名称：PavementEvaluation()
//函数说明：PavementEvaluation类的构造函数，功能为创建主窗口界面
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
PavementEvaluation::PavementEvaluation(QWidget *parent)
    : QWidget(parent)
{
    //---------------创建TreeWidget用来显示文件属性-------------------------------
    treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels(
                QStringList() << tr("文件属性") << tr("属性值"));
    //在TreeWidget中显示文件信息
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setText(0, tr("文件名"));
    treeWidget->addTopLevelItem(treeItem);

    QTreeWidgetItem *treeItem1 = new QTreeWidgetItem(treeWidget);
    treeItem1->setText(0, tr("采样点数"));
    treeWidget->addTopLevelItem(treeItem1);

    QTreeWidgetItem *treeItem2 = new QTreeWidgetItem;
    treeItem2->setText(0, tr("采样时窗"));
    treeWidget->addTopLevelItem(treeItem2);

    QTreeWidgetItem *treeItem3 = new QTreeWidgetItem;
    treeItem3->setText(0, tr("每道字节数"));
    treeWidget->addTopLevelItem(treeItem3);

    QTreeWidgetItem *treeItem4 = new QTreeWidgetItem;
    treeItem4->setText(0, tr("采样道数"));
    treeWidget->addTopLevelItem(treeItem4);

    QTreeWidgetItem *treeItem5 = new QTreeWidgetItem;
    treeItem5->setText(0, tr("文件头字节数"));
    treeWidget->addTopLevelItem(treeItem5);

    //---------------创建TableWidget-------------------------------
    tableWidget = new QTableWidget(10, 10);
    //不允许编辑Table项
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //-------------------创建QListWidget----------------------------
    listWidget = new QListWidget;
    //-------------------创建QTextEdit----------------------------
    textEditer = new QTextEdit;
    textEditer->setFontPointSize(10);

    //--------------------创建QwtPlot用来显示A-Scan------------------
    createAscanPlot();
    createLayerPlot();
    single_trace_curve = new QwtPlotCurve();
    //--------------------创建Bscan_Viewer--------------------------
    plot = new Plot;

    //--------------------创建按钮---------------------------------
    readButton = new QPushButton(tr("读取GPR文件"));
    detectButton = new QPushButton(tr("检测层位"));
    reportButton = new QPushButton(tr("输出检测报告"));
    showSingleTraceButton = new QPushButton(tr("显示单道波形"));
    quitButton = new QPushButton(tr("退出"));

    //连接信号与槽
    connect(readButton, SIGNAL(clicked()),
            this, SLOT(onReadButtonClicked()));
    connect(detectButton, SIGNAL(clicked()),
            this, SLOT(onDetectButtonClicked()));
    connect(reportButton, SIGNAL(clicked()),
            this, SLOT(outputReport()));
    connect(showSingleTraceButton, SIGNAL(clicked()),
            this, SLOT(plotSingleTrace()));
    connect(quitButton, SIGNAL(clicked()),
            qApp, SLOT(quit()));

    //---------------------布局-----------------------------
    //右侧按钮面板布局
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(readButton);
    rightLayout->addWidget(detectButton);
    rightLayout->addWidget(reportButton);
    rightLayout->addWidget(showSingleTraceButton);
    rightLayout->addStretch();
    rightLayout->addWidget(quitButton);
    //Plot布局
    QVBoxLayout *plotLayout = new QVBoxLayout;
    plotLayout->addWidget(ascanPlotter);
    plotLayout->addWidget(layerPlotter);
    //上部控件布局
    QHBoxLayout *upLayout = new QHBoxLayout;
    upLayout->addWidget(treeWidget,0);
    //upLayout->addWidget(listWidget);
    //upLayout->addWidget(textEditer);
    //upLayout->addWidget(ascanPlotter, 2);
    //upLayout->addWidget(layerPlotter, 2);
    upLayout->addLayout(plotLayout, 2);
    upLayout->addLayout(rightLayout,0);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(upLayout, 1);
    //mainLayout->addWidget(tableWidget, 10);
    //mainLayout->addWidget(plot);
    setLayout(mainLayout);

}

PavementEvaluation::~PavementEvaluation()
{
    
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
    // 时窗
    dTimeWnd = gprFileHead.dTimeWnd;
    // 每道字节数
    uiTrcByteNum = iSmpNum *sizeof(ushort);
    //每道数据的字节数
    data_bytes_each_trace = uiTrcByteNum - 90;
    // 道数
    iTrcNum = (fileRead.size() - sizeof(_GPR_HEAD)) / uiTrcByteNum;
    // 初始检测到的层位为0
    layer_num = 0;

    //在TreeWidget中显示文件信息
    treeWidget->clear();

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setText(0, tr("文件名"));
    treeItem->setText(1, gprFileName);
    treeWidget->addTopLevelItem(treeItem);

    QTreeWidgetItem *treeItem1 = new QTreeWidgetItem(treeWidget);
    treeItem1->setText(0, tr("采样点数"));
    treeItem1->setText(1, QString::number(sampling_num) + "点");
    treeWidget->addTopLevelItem(treeItem1);

    QTreeWidgetItem *treeItem2 = new QTreeWidgetItem;
    treeItem2->setText(0, tr("采样时窗"));
    treeItem2->setText(1, QString::number(dTimeWnd) + "ns");
    treeWidget->addTopLevelItem(treeItem2);

    QTreeWidgetItem *treeItem3 = new QTreeWidgetItem;
    treeItem3->setText(0, tr("每道字节数"));
    treeItem3->setText(1, QString::number(uiTrcByteNum) + "字节");
    treeWidget->addTopLevelItem(treeItem3);

    QTreeWidgetItem *treeItem4 = new QTreeWidgetItem;
    treeItem4->setText(0, tr("采样道数"));
    treeItem4->setText(1, QString::number(iTrcNum));
    treeWidget->addTopLevelItem(treeItem4);

    QTreeWidgetItem *treeItem5 = new QTreeWidgetItem;
    treeItem5->setText(0, tr("文件头字节数"));
    treeItem5->setText(1, QString::number(sizeof(_GPR_HEAD)) + "字节");
    treeWidget->addTopLevelItem(treeItem5);


    //初始化进度条
    QProgressDialog *readingProgressDialog = new QProgressDialog;
    readingProgressDialog->setMinimumDuration(5);   //进度条至少显示5ms
    readingProgressDialog->setWindowTitle(tr("Please Wait"));
    readingProgressDialog->setLabelText(tr("读取GPR数据中..."));
    readingProgressDialog->setCancelButtonText(tr("取消"));
    readingProgressDialog->setRange(0,iTrcNum-1);
    //模态对话框，在其没有被关闭之前，用户不能与同一个应用程序的其他窗口进行交互
    readingProgressDialog->setModal(true);


    // 分配内存
    char *trace_head = new char[90];    //每道道头长度为90字节
    ushort *usData = new ushort[sampling_num];   //一次读取的每道数据
    ushort *usDataDeal = new ushort[sampling_num];

    bscan_data = new double *[iTrcNum];
    for(int i = 0; i < iTrcNum; i++){
        bscan_data[i] = new double[sampling_num];
    }

    // 定位文件指针
    int iStartPos = sizeof(_GPR_HEAD) + 6;
    fileRead.seek(iStartPos);
    //逐道读取数据
    for (int col = 0; col < iTrcNum; col++) {
        fileRead.read((char *)trace_head, 90);    //先读90字节道头
        //读本道数据
        if (fileRead.read((char *)usData, data_bytes_each_trace) == data_bytes_each_trace) {
            //进度条更新一次
            readingProgressDialog->setValue(col);
            if (readingProgressDialog->wasCanceled()){
                fileRead.remove();
                tableWidget->clear();
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
    QMessageBox::information(NULL, "读取GPR文件", "读取GPR文件完毕！", QMessageBox::Yes);


    //去直流
    LayerDetector detector;
    for(int i = 0; i < iTrcNum; i++){
        detector.subtract_mean(bscan_data[i], sampling_num);
    }
    QMessageBox::information(NULL, "去直流", "去直流完毕！", QMessageBox::Yes);

    tableWidget->setRowCount(10);
    tableWidget->setColumnCount(iTrcNum);

    //-------------------------------把读取的数据显示在Table中----------------------------------
//    tableWidget->clear();
//    tableWidget->setRowCount(sampling_num);
//    tableWidget->setColumnCount(iTrcNum);

//    //初始化进度条
//    QProgressDialog showingProgressDialog;
//    showingProgressDialog.setMinimumDuration(5);   //进度条至少显示5ms
//    showingProgressDialog.setLabelText(tr("显示B-Scan数据中..."));
//    showingProgressDialog.setRange(0,iTrcNum-1);
//    showingProgressDialog.setModal(true);

//    //把数组里的数据再显示出来
//    for(int i = 0; i < iTrcNum; i++){
//        showingProgressDialog.setValue(i);
//        for (int j = 0; j < sampling_num; j++){
//            QTableWidgetItem *item = new QTableWidgetItem;
//            item->setText(QString::number(bscan_data[i][j]));
//            tableWidget->setItem(j, i, item);
//        }
//    }
//    QMessageBox::information(this, "显示B-Scan数据", "显示B-Scan数据完毕！", QMessageBox::Yes);

    return true;
}



//-------------------------------------------------------------------------------------------------
//函数名称：onReadButtonClicked()
//函数说明：读取GPR文件的SLOT
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onReadButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("打开GPR文件"), ".",
                tr("GPR 文件 (*.gpr)"));
    if (!fileName.isEmpty())
        readGprData(fileName);
}



//-------------------------------------------------------------------------------------------------
//函数名称：onDetectButtonClicked()
//函数说明：进行倒相关层位检测，目前一次只检测单层
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
void PavementEvaluation::onDetectButtonClicked()
{
    //------------------检测参数设置对话框声明-----------------------------
    ParameterDialog *setupDialog = new ParameterDialog(this);
    setupDialog->show();
    setupDialog->raise();
    setupDialog->activateWindow();
    setupDialog->setModal(true);
    setupDialog->firstSearchPointHintLabel->setText(tr("(1-%1)")
                                                    .arg(sampling_num));
    setupDialog->startHintLabel->setText(tr("(1-%1)")
                                          .arg(iTrcNum));

    //---------------------检测参数声明-------------------------
    int first_search_point;
    int start_trace;
    int ref_win;
    int search_win;

    LayerDetector *detector = new LayerDetector;
    //保存层位点的数组
    int *layer_position_array = new int[iTrcNum];
    //初始化数组
    for (int i = 0; i < iTrcNum; i++) {
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
        detector->trace_corr_detector(bscan_data, iTrcNum, first_search_point, start_trace, ref_win, search_win, layer_position_array);

        //检测到的层位数+1
        layer_num++;

        QMessageBox::information(NULL, "路面检测软件", "道相关追踪完毕!", QMessageBox::Yes);

        // 把检测结果保存到Txt文件中
        //saveArrayAsTxt(layer_position_array, iTrcNum);

        // 把检测结果显示在List和Table中
        listWidget->clear();
        for (int i = 0; i < iTrcNum; i++){
            QListWidgetItem *item_list = new QListWidgetItem;
            QTableWidgetItem *item_table = new QTableWidgetItem;
            item_list->setText(QString::number(layer_position_array[i]));
            item_table->setText(QString::number(layer_position_array[i]));
            item_table->setTextAlignment(Qt::AlignCenter);
            listWidget->addItem(item_list);
            tableWidget->setItem(layer_num - 1, i, item_table);
        }

        textEditer->clear();
        textEditer->append(tr("检测到层位数：%1")
                           .arg(layer_num));


        plotLayer();
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
template <class T>
bool PavementEvaluation::saveArrayAsTxt(T *array, int n)
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
//函数名称：outputReport()
//函数说明：输出Txt格式的路面检测报表
//参数说明：
//输出：
//-------------------------------------------------------------------------------------------------
bool PavementEvaluation::outputReport()
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

    //---------------把检测结果写入Txt文件-------------------
    //头信息：层数
    out << "[LayerNum]: " << layer_num << "\r\n";
    out << "\r\n";

    //单层信息
    //格式为:
    //    [LayerPtNum]:4
    //    1,5,0.2
    //    2,4.5,0.15
    //    3,4.5,0.15
    //    4,4.5,0.15
    //    [LayerEnd]
    for (int i = 0; i < layer_num; i++) {
        out << "[LayerPtNum]: " << i+1 << "\r\n";
        for (int j = 0; j < iTrcNum; j++){
            out << tableWidget->item(i, j)->text();
            //两个数据间用逗号隔开，最后一个数据后面不加逗号
            if (j < iTrcNum - 1) {
                out << ", ";
            }
        }
        out << "\r\n";
        out << "[LayerEnd]" << "\r\n";
        out << "\r\n";
    }

    //文件写完后把光标恢复
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, "写入Txt文件", "写入Txt文件完毕！", QMessageBox::Yes);
    return true;
}



/**
* @brief 在QwtPlot中显示单道波形
* @return
*/
void PavementEvaluation::plotSingleTrace()
{
    // 显示输入对话框，用于输入要显示A-Scan波形的道数
    QInputDialog *inputTraceNumDialog = new QInputDialog(this);
    inputTraceNumDialog->setLabelText(tr("输入道数："));
    inputTraceNumDialog->setWindowTitle(tr("显示单道波形"));
    inputTraceNumDialog->setOkButtonText(tr("确定"));
    inputTraceNumDialog->setCancelButtonText(tr("取消"));
    //inputTraceNumDialog->setInputMode(QInputDialog::IntInput);
    inputTraceNumDialog->show();
    inputTraceNumDialog->raise();
    inputTraceNumDialog->activateWindow();
    inputTraceNumDialog->setModal(true);

    if (inputTraceNumDialog->exec()) {
        // 记录输入的道数
        int traceToShowNum = inputTraceNumDialog->textValue().toInt();
        // 把该道数据送入buffer
        QVector<QPointF> points;
        for (int i = 0; i < sampling_num; i++){
            points.append(QPointF(i, bscan_data[traceToShowNum][i]));
        }
        // 对曲线做设置
        single_trace_curve->setTitle( "ascan_curve" );
        single_trace_curve->setPen( Qt::red, 1 );
        single_trace_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
//        curve->setStyle( QwtPlotCurve::Lines );
//        curve->setCurveAttribute( QwtPlotCurve::Fitted ); //使用插值，使曲线更光滑。事实表明对显示波形没用，不需要！
        single_trace_curve->setSamples( points );
        single_trace_curve->attach( ascanPlotter );
        //ascanPlotter->setAxisScale(QwtPlot::xBottom, 0, sampling_num);    //设置坐标轴
        //重绘，更新界面
        ascanPlotter->replot();
    }

}



/**
* @brief 创建QwtPlot显示单道波形
* @return
*/
void PavementEvaluation::createAscanPlot()
{
    ascanPlotter = new QwtPlot;
    ascanPlotter->setTitle( QObject::tr("单道波形") );
    ascanPlotter->setCanvasBackground( Qt::white );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( ascanPlotter );


//    QPolygonF points;
//    for (int i = 0; i < 1000; i++){
//        points += QPointF(i/10.0, 3*sin(i/10.0));
//    }

//    QwtPlotCurve *curve = new QwtPlotCurve();
//    curve->setTitle( "Some Points" );
//    curve->setPen( Qt::blue, 1 );
//    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
//    curve->setSamples( points );
//    curve->attach( ascanPlotter );

}


/**
* @brief 创建QwtPlot显示检测到的层位
* @return
*/
void PavementEvaluation::createLayerPlot()
{
    layerPlotter = new QwtPlot;
    layerPlotter->setTitle( QObject::tr("检测介质层") );
    layerPlotter->setCanvasBackground( Qt::white );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( layerPlotter );
}




/**
* @brief 画出检测到的层
* @return
*/
void PavementEvaluation::plotLayer()
{
    int max_val, min_val;
    // 把该道数据送入buffer
    QVector<QPointF> points;
    for (int i = 0; i < iTrcNum; i++){
        points.append(QPointF(i, tableWidget->item(layer_num-1, i)->text().toInt()));
    }
    // 对曲线做设置
    single_trace_curve->setTitle( "ascan_curve" );
    single_trace_curve->setPen( Qt::blue, 1 );
    single_trace_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    single_trace_curve->setSamples( points );
    layerPlotter->setAxisScale(QwtPlot::yLeft, 100, 800);    //设置坐标轴
    single_trace_curve->attach( layerPlotter );
    //重绘，更新界面
    layerPlotter->replot();
}
