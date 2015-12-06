#ifndef THICKNESSDIALOG_H
#define THICKNESSDIALOG_H

#include <QDialog>
#include <qwt_plot.h>
#include <qglobal.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_legenditem.h>


class QPushButton;


class ThicknessDialog : public QDialog
{
    Q_OBJECT
public:
    ThicknessDialog(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    void alignScales(QwtPlot *plot); // 将坐标轴与图贴在一起
    void loadData();
    // -------------UI元素-------------



    //-----------Qwt插件------------
    QwtPlot *thicknessPlot;
    QwtPlot *dielectricPlot;
    QwtPlotCurve *thickness_curve;
    QwtPlotCurve *dielectric_curve;
    
};



#endif // THICKNESSDIALOG_H
