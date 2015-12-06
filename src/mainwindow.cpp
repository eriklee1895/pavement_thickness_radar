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
** 文件名： mainwindow.cpp
** 功能：主窗口程序，用于完成菜单、工具栏、状态栏，以及相应行为的实现
**
****************************************************************************/

#include <QtGui>
#include "mainwindow.h"
#include "bscan_viewer.h"


/**
 * @brief 构造函数
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    pavement_evaluation = new PavementEvaluation;
    setCentralWidget(pavement_evaluation);
    createActions();    //创建Actions
    createMenus();      //创建菜单
    createToolBars();   //创建工具栏
    createStatusBar();  //创建状态栏
    setWindowIcon(QPixmap(":/images/layer.png"));
    setWindowTitle(tr("探地雷达路面检测软件 - 中科院电子所"));
}



/**
 * @brief 创建状态栏
 */
void MainWindow::createStatusBar()
{
    layerNumLabel = new QLabel(tr("当前已检测到层数=%1")
                               .arg(pavement_evaluation->get_layer_num()));
    layerNumLabel->setIndent(3);

    mousePosLabel = new QLabel(pavement_evaluation->current_pos_on_profile());
    mousePosLabel->setIndent(3);

    statusBar()->addWidget(layerNumLabel);
    statusBar()->addWidget(mousePosLabel);

    connect(pavement_evaluation, SIGNAL(layer_num_updated()),
            this, SLOT(updateStatusBar()));
    connect(pavement_evaluation, SIGNAL(mouse_pos_on_profile_changed()),
            this, SLOT(updateStatusBar()));


}

/**
 * @brief 更新状态栏
 */
void MainWindow::updateStatusBar()
{
    layerNumLabel->setText(tr("当前已检测到层数=%1")
                               .arg(pavement_evaluation->get_layer_num()));

    mousePosLabel->setText(pavement_evaluation->current_pos_on_profile());
}



/**
 * @brief 创建菜单和工具栏的Action
 */
void MainWindow::createActions()
{
    //----------------File Menu Actions---------------------
    openAction = new QAction(tr("打开GPR文件..."), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("打开GPR文件"));
    connect(openAction, SIGNAL(triggered()), pavement_evaluation, SLOT(onReadButtonClicked()));

    exitAction = new QAction(tr("退出"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("退出"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //----------------Edit Menu Actions---------------------
    setFsAction = new QAction(tr("设置采样频率"), this);
    setFsAction->setStatusTip(tr("设置采样频率，采样时窗也同时改变"));
    connect(setFsAction, SIGNAL(triggered()),
            this, SLOT(onSetFsAction()));

    reduceDCAction = new QAction(tr("去除直流"), this);
    reduceDCAction->setIcon(QIcon(":/images/cut.png"));
    reduceDCAction->setShortcut(QKeySequence::Cut);
    reduceDCAction->setStatusTip(tr("去除直流分量"));
    connect(reduceDCAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(removeDC()));

    removeCouplingPulseAction = new QAction(tr("去除直耦波"), this);
    removeCouplingPulseAction->setIcon(QIcon(":/images/cut.png"));
    removeCouplingPulseAction->setStatusTip(tr("去除直耦波"));
    connect(removeCouplingPulseAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onRemoveCouplingPulseButtonClicked()));

    reversePhaseAction = new QAction(tr("数据反相"), this);
    reversePhaseAction->setStatusTip(tr("数据反相"));
    connect(reversePhaseAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(reversePhase()));

    removeBackgroundAction = new QAction(tr("去除背景"), this);
    removeBackgroundAction->setStatusTip(tr("去除背景"));
    connect(removeBackgroundAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(removeBackground()));

    gainAction = new QAction(tr("深部信号增强"), this);
    gainAction->setStatusTip(tr("深部信号增强"));
    connect(gainAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(gain()));

    equalizationAction = new QAction(tr("时域均衡（*）"), this);
    equalizationAction->setStatusTip(tr("时域均衡（*）"));


    bpfFilterAction = new QAction(tr("600MHz-2GHz带通滤波"), this);
    bpfFilterAction->setIcon(QIcon(":/images/bpf.png"));
    bpfFilterAction->setStatusTip(tr("600MHz-2GHz带通滤波"));
    connect(bpfFilterAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onBpfAction()));

   //----------------Tools Menu Actions---------------------
    setupRefWavesAction = new QAction(tr("设置定标波形"), this);
    setupRefWavesAction->setIcon(QIcon(":/images/pulse_ref.png"));
    setupRefWavesAction->setStatusTip(tr("设置定标波形"));
    connect(setupRefWavesAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onSetupRefWavesButtonClicked()));

    detectAction = new QAction(tr("提取层位"), this);
    detectAction->setIcon(QIcon(":/images/layer.png"));
    detectAction->setShortcut(tr("F9"));
    detectAction->setStatusTip(tr("道相关层位提取"));
    connect(detectAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onDetectButtonClicked()));

    singleDetectAction = new QAction(tr("测试逐道检测层位"), this);
    singleDetectAction->setIcon(QIcon(":/images/layer.png"));
    singleDetectAction->setShortcut(tr("F9"));
    singleDetectAction->setStatusTip(tr("逐道道相关层位提取"));
    connect(singleDetectAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onSingleDetectButtonClicked()));

    reportAction = new QAction(tr("厚度检测"), this);
    reportAction->setIcon(QIcon(":/images/report.png"));
    reportAction->setStatusTip(tr("厚度检测"));
    connect(reportAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(outputReport()));

    layerManagerAction = new QAction(tr("层管理器"), this);
    layerManagerAction->setIcon(QIcon(":/images/layer.png"));
    layerManagerAction->setShortcut(tr("F10"));
    layerManagerAction->setStatusTip(tr("层管理器"));
    connect(layerManagerAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onSingleDetectButtonClicked()));

    //----------------Setup Menu Action---------------------
    bsacnRulerSwichAction = new QAction(tr("B-Scan标尺开关"), this);
    bsacnRulerSwichAction->setStatusTip(tr("B-Scan标尺开关"));
    bsacnRulerSwichAction->setCheckable(true);
    bsacnRulerSwichAction->setChecked(true);

    ascanGridSwichAction = new QAction(tr("A-Scan网格开关"), this);
    ascanGridSwichAction->setStatusTip(tr("A-Scan网格开关"));
    ascanGridSwichAction->setCheckable(true);
    ascanGridSwichAction->setChecked(true);

    preferenceAction = new QAction(tr("设置软件外观等参数（*）"), this);
    preferenceAction->setStatusTip(tr("设置软件外观等参数（*）"));

    //----------------About Menu Action---------------------
    aboutAction = new QAction(tr("关于"), this);
    aboutAction->setStatusTip(tr("显示软件信息"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("关于&Qt"), this);
    aboutQtAction->setStatusTip(tr("显示Qt库版本信息"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

/**
 * @brief 创建菜单
 */
void MainWindow::createMenus()
{
    //--------------------File Menu--------------------
    fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    //--------------------Edit Menu--------------------
    editMenu = menuBar()->addMenu(tr("数据处理"));
    editMenu->addAction(setFsAction);
    editMenu->addAction(reduceDCAction);
    editMenu->addAction(removeCouplingPulseAction);
    editMenu->addAction(reversePhaseAction);
    editMenu->addAction(removeBackgroundAction);
    editMenu->addAction(gainAction);
    editMenu->addAction(bpfFilterAction);

    //--------------------Tools Menu--------------------
    toolsMenu = menuBar()->addMenu(tr("工具"));
    toolsMenu->addAction(setupRefWavesAction);
    toolsMenu->addAction(detectAction);
    toolsMenu->addAction(reportAction);
    toolsMenu->addAction(layerManagerAction);

    //--------------------Setup Menu--------------------
    toolsMenu = menuBar()->addMenu(tr("设置"));
    toolsMenu->addAction(bsacnRulerSwichAction);
    toolsMenu->addAction(ascanGridSwichAction);
    toolsMenu->addAction(preferenceAction);

    //--------------------Help Menu--------------------
    helpMenu = menuBar()->addMenu(tr("帮助"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

/**
 * @brief 创建工具栏
 */
void MainWindow::createToolBars()
{
    quickOperationToolBar = addToolBar((tr("&QuickOperation")));
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(openAction);
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(removeCouplingPulseAction);
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(detectAction);
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(setupRefWavesAction);
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(layerManagerAction);
    quickOperationToolBar->addSeparator();
    quickOperationToolBar->addAction(reportAction);
    quickOperationToolBar->addSeparator();

    // 设置工具栏显示风格为文字在图标之下
    quickOperationToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //quickOperationToolBar->setIconSize(QSize(32, 32));


//    QToolButton *btnExport = new QToolButton( quickOperationToolBar );
//    btnExport->setText( tr("为每一层都设置上") );
//    btnExport->setIcon( QPixmap( ":/images/layer.png" ) );
//    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
//    quickOperationToolBar->addWidget( btnExport );
//    //connect( btnExport, SIGNAL( clicked() ), SLOT( exportDocument() ) );
//    quickOperationToolBar->addSeparator();
}




/**
 * @brief 关于对话框
 */
void MainWindow::about()
{
    QMessageBox::about(this, tr("关于 探地雷达路面检测软件"),
            tr("<h2>探地雷达路面检测软件 V0.1</h2>"
               "<p>Copyright &copy; 2013 IECAS."
               "<p>作者：Erik Lee(李玉恒)"
               "<p>现在实现了道相关检测 "
               "厚度检测和介电常数计算进行中... "));
}

/**
 * @brief 设置采样频率的对话框
 */
void MainWindow::onSetFsAction()
{
    if (!pavement_evaluation->isGprFileLoaded()) {
        QMessageBox::information(NULL, "路面厚度检测软件", "尚未载入GPR文件!",
                                 QMessageBox::Yes);
        return;
    }

    bool ok;
    double fs = QInputDialog::getDouble(this, tr("手动设置采样频率"),
                                       tr("采样频率(GHz)"), 40, 0, 1000, 2, &ok);
    if (ok)
        pavement_evaluation->setSampingFrequency(fs);
}















