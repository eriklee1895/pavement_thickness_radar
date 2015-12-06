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
** 文件名： mainwindow.h
** 功能：主窗口菜单、工具栏、状态栏，以及相应行为的函数声明
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "pavementevaluation.h"

class QAction;
class QLabel;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    

    
public slots:
    void updateStatusBar();
    void about();

private slots:
    void onSetFsAction();           // 设置采样率


private:
    //-----------Private Methods---------------
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    //-----------Members--------------------
    PavementEvaluation *pavement_evaluation;    // 中央窗口Widget
    QString curFile;
    //-----------GUI Members---------------
    QLabel *layerNumLabel;      // 层数，显示在状态栏
    QLabel *mousePosLabel;      // 坐标信息，显示在状态栏
    //-----------Menu----------------
    QMenu *fileMenu;            // 文件菜单
    QMenu *editMenu;            // 数据处理菜单
    QMenu *toolsMenu;           // 工具菜单
    QMenu *setupMenu;           // 设置菜单
    QMenu *helpMenu;            // 帮助菜单
    //-----------ToolBar----------------
    QToolBar *quickOperationToolBar;    // 工具栏
    //-----------Actions----------------
    // 文件菜单
    QAction *openAction;                // 打开GPR文件
    QAction *exitAction;                // 退出
    //------------------------------------------
    // 数据处理菜单
    QAction *setFsAction;               // 设置采样频率与采样时窗
    QAction *reduceDCAction;            // 去直流
    QAction *removeCouplingPulseAction; // 去除直耦波
    QAction *reversePhaseAction;        // 数据反相
    QAction *removeBackgroundAction;    // 去除背景
    QAction *gainAction;                // 时变增益处理
    QAction *equalizationAction;        // 时域均衡处理
    QAction *bpfFilterAction;           // BPF滤波
    //------------------------------------------
    // 工具菜单
    QAction *setupRefWavesAction;       // 设置定标波形
    QAction *detectAction;              // 道相关检测
    QAction *singleDetectAction;        // 逐道检测
    QAction *reportAction;              // 输出报表
    QAction *layerManagerAction;        // 层管理器
    //------------------------------------------
    // 设置菜单
    QAction *bsacnRulerSwichAction;           // B-Scan标尺开关
    QAction *ascanGridSwichAction;            // A-Scan网格开关
    QAction *preferenceAction;          // 偏好设置
    //------------------------------------------
    // 关于菜单
    QAction *aboutAction;               // 关于对话框
    QAction *aboutQtAction;             // 关于Qt对话框，^_^
    

};

#endif // MAINWINDOW_H
