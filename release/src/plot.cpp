#include "plot.h"
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_layout.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>

class RasterData: public QwtMatrixRasterData
{
public:
    RasterData()
    {
        QVector<double> values;
        for ( uint i = 0; i < 1024 * 9000; i++ ){
            values += static_cast<int>((qrand()/(static_cast<double>(RAND_MAX)+1))*255);

        }

        const int numColumns = 9000;
        setValueMatrix( values, numColumns );

        setInterval( Qt::XAxis,
            QwtInterval( 1, 9000, QwtInterval::IncludeBorders ) );
        setInterval( Qt::YAxis,
            QwtInterval( 1, 1024, QwtInterval::IncludeBorders ) );
        setInterval( Qt::ZAxis, QwtInterval( 0, 255 ) );
    }
};

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::black, Qt::white )
    {

    }
};

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );
    setCanvas( canvas );

#if 0
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::DotLine );
    grid->attach( this );
#endif

    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap() );

    d_spectrogram->setData( new RasterData() );
    d_spectrogram->attach( this );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth( 40 );
    rightAxis->setColorMap( zInterval, new ColorMap() );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );

    plotLayout()->setAlignCanvasToScales( true );

    setAxisScale( QwtPlot::xBottom, 0.0, 3.0 );
    setAxisMaxMinor( QwtPlot::xBottom, 0 );
    setAxisScale( QwtPlot::yLeft, 0.0, 3.0 );
    setAxisMaxMinor( QwtPlot::yLeft, 0 );

    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier( canvas );
    magnifier->setAxisEnabled( QwtPlot::yRight, false );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas );
    panner->setAxisEnabled( QwtPlot::yRight, false );
}

void Plot::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "rasterview.pdf" );
}

void Plot::setResampleMode( int mode )
{
    RasterData *data = static_cast<RasterData *>( d_spectrogram->data() );
    data->setResampleMode(
        static_cast<QwtMatrixRasterData::ResampleMode>( mode ) );

    replot();
}
