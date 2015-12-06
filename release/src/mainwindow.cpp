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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    pavement_evaluation = new PavementEvaluation;
    setCentralWidget(pavement_evaluation);

    createActions();    //创建Actions
    createMenus();      //创建菜单
    createToolBars();   //创建工具栏
    createStatusBar();  //创建状态栏

    setWindowTitle(tr("探地雷达路面检测软件 - 中科院电子所十室"));

}




void MainWindow::createStatusBar()
{
    statusLabel = new QLabel(tr("读入完毕"));
    statusLabel->setAlignment(Qt::AlignHCenter);
    statusLabel->setMinimumSize(statusLabel->sizeHint());

    dataPointsLabel = new QLabel(tr("读入点数=0"));
    dataPointsLabel->setIndent(3);

    layerNumLabel = new QLabel(tr("当前已检测到层数=%1")
                               .arg(pavement_evaluation->get_layer_num()));
    dataPointsLabel->setIndent(3);

    statusBar()->addWidget(statusLabel);
    statusBar()->addWidget(dataPointsLabel);
    statusBar()->addWidget(layerNumLabel);

    connect(pavement_evaluation, SIGNAL(layer_num_updated()),
            this, SLOT(updateStatusBar()));


}

void MainWindow::updateStatusBar()
{
    layerNumLabel->setText(tr("当前已检测到层数=   %1")
                               .arg(pavement_evaluation->get_layer_num()));
}




//创建菜单和工具栏的Action
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
    reduceDCAction = new QAction(tr("去除直流"), this);
    reduceDCAction->setIcon(QIcon(":/images/cut.png"));
    reduceDCAction->setShortcut(QKeySequence::Cut);
    reduceDCAction->setStatusTip(tr("去除直流分量"));


   //----------------Tools Menu Actions---------------------
    detectionAction = new QAction(tr("层位追踪"), this);
    detectionAction->setIcon(QIcon(":/images/layer.png"));
    detectionAction->setShortcut(tr("F9"));
    detectionAction->setStatusTip(tr("道相关层位追踪"));
    connect(detectionAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(onDetectButtonClicked()));


    reportAction = new QAction(tr("输出层位检测报表"), this);
    reportAction->setIcon(QIcon(":/images/report.png"));
    reportAction->setStatusTip(tr("输出层位检测报表"));
    connect(reportAction, SIGNAL(triggered()),
            pavement_evaluation, SLOT(outputReport()));


    //----------------About Menu Action---------------------
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    //--------------------File Menu--------------------
    fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    //--------------------Edit Menu--------------------
    editMenu = menuBar()->addMenu(tr("数据处理"));
    editMenu->addAction(reduceDCAction);


    //--------------------Tools Menu--------------------
    toolsMenu = menuBar()->addMenu(tr("工具"));
    toolsMenu->addAction(detectionAction);
    toolsMenu->addAction(reportAction);

    menuBar()->addSeparator();

    //--------------------Help Menu--------------------
    helpMenu = menuBar()->addMenu(tr("帮助"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}


void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(openAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(reduceDCAction);

    processingToolBar = addToolBar(tr("&Processing"));
    processingToolBar->addAction(detectionAction);
    processingToolBar->addAction(reportAction);
}





void MainWindow::about()
{
    QMessageBox::about(this, tr("About 路面检测"),
            tr("<h2>路面检测软件 V0.1</h2>"
               "<p>Copyright &copy; 2013 IECAS."
               "<p>现在实现了道相关检测 "
               "厚度检测和介电常数计算进行中... "));
}

















