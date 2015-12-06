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

private:
    PavementEvaluation *pavement_evaluation;

    QLabel *statusLabel;
    QLabel *dataPointsLabel;
    QLabel *layerNumLabel;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();


    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *processingToolBar;

    QAction *openAction;
    QAction *exitAction;   
    QAction *reduceDCAction;
    QAction *detectionAction;
    QAction *reportAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    
    QString curFile;
};

#endif // MAINWINDOW_H
