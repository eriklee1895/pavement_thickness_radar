#include <QtGui>
#include "spectrumviewer.h"
#include "dsptools.h"
//#include "sdsp/mdsp.h"

SpectrumViewer::SpectrumViewer( QWidget *parent ):
    QwtPlot( parent )
{
    fs = 40;    // 默认采样率为40GHz

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setLineWidth( 1 );
    canvas->setPalette( Qt::white );
    setCanvas( canvas );
    alignScales();

    // Insert grid
    grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->attach( this );

    // Insert curve
    spectrum_curve = new QwtPlotCurve();
    spectrum_curve->setPen(Qt::darkBlue, 1);
    spectrum_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    spectrum_curve->attach( this );

    // Axis
    QwtText xAxisText(tr("频率(GHz)"));
    setAxisTitle(QwtPlot::xBottom, xAxisText);
    setAxisScale(QwtPlot::xBottom, 0, 6);

    QwtText yAxisText(tr("幅值(dB)"));
    setAxisTitle(QwtPlot::yLeft, yAxisText );
    setAxisScale(QwtPlot::yLeft, -40, 0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

//
//  Set a plain canvas frame and align the scales to it
//
void SpectrumViewer::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = axisWidget( i );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );

        QwtScaleDraw *scaleDraw = axisScaleDraw( i );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }
    plotLayout()->setAlignCanvasToScales( true );
}



void SpectrumViewer::loadSignal(const QVector<QPointF> &data, double fs)
{
    std::complex<double> *pSignalData = new std::complex<double>[data.count()];
    std::complex<double> *pFftResult = new std::complex<double>[data.count()];

    // 载入信号
    for (int i = 0; i < data.count(); ++i)
        pSignalData[i] = std::complex<double>(data[i].y());


    // 对载入信号做FFT,结果保存在fftResult中
    fft(pSignalData , data.count(), pFftResult);

    // 求幅度谱
    QVector<double> spectrumValues(data.count());
    for(int i = 0; i < data.count(); i++)
        spectrumValues[i] = sqrt(pFftResult[i].real() * pFftResult[i].real()
                + pFftResult[i].imag() * pFftResult[i].imag());
    // 找出频谱值中的最大值
    double max_value_of_spectrum = spectrumValues[dspTools::max_in_QVector(spectrumValues)];
    // 频谱归一化处理
    for(int i = 0; i < data.count(); i++)
        spectrumValues[i] = spectrumValues[i] / max_value_of_spectrum;


    double freq_delta = (fs/2 * 1e-9) / (data.count()/2);  // 频率步进值


    QVector<QPointF> spectrumPoints;
    for (int i = 0; i < data.count(); i++){
        spectrumPoints.append(QPointF(freq_delta*i, 20*log10(spectrumValues[i])));
    }
    spectrum_curve->setSamples( spectrumPoints );
    qDebug() << "采样率:" << fs << "，数据点数为：" << data.count();

}



int SpectrumViewer::fft(std::complex<double> * x, int n, std::complex<double> * y)
{
    int k = n >> 1;
    int i, j; i = j = 0;
    std::complex<double> w(1.0, 0.0);
    std::complex<double> wn(cos(2*M_PI/n), sin(-2*M_PI/n));
    std::complex<double> t(0.0, 0.0);
    std::complex<double> * x0 = new std::complex<double>[k];
    std::complex<double> * x1 = new std::complex<double>[k];
    std::complex<double> * y0 = new std::complex<double>[k];
    std::complex<double> * y1 = new std::complex<double>[k];
    if ((NULL == x0) || (NULL == x1) || (NULL == y0) || (NULL == y1)) {
        return 0;
    }
    if (1 == n) {
        y[0] = x[0];
        return 1;
    }
    for (i = 0; i < k ; i++) {
        int j = 2 * i;
        x0[i] = x[j];
        x1[i] = x[j+1];
    }
    if (fft(x0, k, y0) && fft(x1, k, y1)) {
         for (i = 0; i < k; i++) {
            t = y1[i] * w;
            y[i] = y0[i] + t;
            y[i+k] = y0[i] - t; w = w * wn;
         }
     }
    return 1;

}




