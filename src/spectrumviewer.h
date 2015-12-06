#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include <qwt_plot.h>
#include <qglobal.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>

#include <iostream>
#include <cmath>
#include <complex>

//typedef std::complex<double> complex;


class QwtPlotGrid;
class QwtPlotCurve;

class SpectrumViewer : public QwtPlot
{
    Q_OBJECT

public:
    SpectrumViewer( QWidget* = NULL );
    void loadSignal(const QVector<QPointF> &data, double fs);

public Q_SLOTS:


protected:

private:
    // -----方法成员------
    void alignScales(); // 将坐标轴与图贴在一起
    int fft(std::complex<double> * x, int n, std::complex<double> * y);
    // -----数据成员------
    QwtPlotGrid *grid;                      // 网格
    QwtPlotCurve *spectrum_curve;           // 频谱曲线
    QVector<QPointF> spectrumData;          // 计算得到的频谱数据

    int signalLength;       // 信号点数
    double fs;              // 采样率
};

#endif // SPECTRUMVIEWER_H
