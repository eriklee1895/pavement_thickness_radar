#include <QtGui>
#include "thicknessdialog.h"

class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );

        QColor color( Qt::black );

        setTextPen( color );
#if 1
        setBorderPen( color );

        QColor c( Qt::gray );
        c.setAlpha( 200 );

        setBackgroundBrush( c );
#endif
    }
};


ThicknessDialog::ThicknessDialog(QWidget *parent) :
    QDialog(parent)
{
    QTabWidget *tab = new QTabWidget(this);

    // 显示厚度的QwtPlot
    thicknessPlot = new QwtPlot;
    // 画布
    QwtPlotCanvas *thicknessCanvas = new QwtPlotCanvas();
    thicknessCanvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    thicknessCanvas->setLineWidth( 1 );
    thicknessCanvas->setPalette( Qt::white );
    thicknessPlot->setCanvas( thicknessCanvas );
    alignScales(thicknessPlot);
    // Insert grid
    QwtPlotGrid *thicknessGrid = new QwtPlotGrid();
    thicknessGrid->setPen( Qt::gray, 0.0, Qt::DotLine );
    thicknessGrid->attach( thicknessPlot );
    // Insert curve
    thickness_curve = new QwtPlotCurve(tr("面层"));
    thickness_curve->setPen(Qt::darkBlue, 1);
    //thickness_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    thickness_curve->attach(thicknessPlot);
    // Axis
    QwtText xThicknessAxisText(tr("道数"));
    thicknessPlot->setAxisTitle(QwtPlot::xBottom, xThicknessAxisText);
    thicknessPlot->setAxisScale(QwtPlot::xBottom, 0, 10000, 1000);
    QwtText yThicknessAxisText(tr("厚度(cm)"));
    thicknessPlot->setAxisTitle(QwtPlot::yLeft, yThicknessAxisText );
    thicknessPlot->setAxisScale(QwtPlot::yLeft, 12, 24);
    thicknessPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // 显示介电常数的QwtPlot
    dielectricPlot = new QwtPlot;
    // 画布
    QwtPlotCanvas *dielectricCanvas = new QwtPlotCanvas();
    dielectricCanvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    dielectricCanvas->setLineWidth( 1 );
    dielectricCanvas->setPalette( Qt::white );
    dielectricPlot->setCanvas( dielectricCanvas );
    alignScales(dielectricPlot);
    // Insert grid
    QwtPlotGrid *dielectricGrid = new QwtPlotGrid();
    dielectricGrid->setPen( Qt::gray, 0.0, Qt::DotLine );
    dielectricGrid->attach( dielectricPlot );
    // Insert curve
    dielectric_curve = new QwtPlotCurve(tr("面层"));
    dielectric_curve->setPen(Qt::darkRed, 1);
    //dielectric_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    dielectric_curve->attach(dielectricPlot);
    // Axis
    QwtText xDielectricAxisText(tr("道数"));
    dielectricPlot->setAxisTitle(QwtPlot::xBottom, xDielectricAxisText);
    dielectricPlot->setAxisScale(QwtPlot::xBottom, 0, 10000, 1000);
    QwtText yDielectricAxisText(tr("介电常数"));
    dielectricPlot->setAxisTitle(QwtPlot::yLeft, yDielectricAxisText );
    dielectricPlot->setAxisScale(QwtPlot::yLeft, 3, 8);
    dielectricPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // Tab页
    QWidget *thicknessPage = new QWidget;
    QHBoxLayout *thicknessPageLayout = new QHBoxLayout;
    thicknessPageLayout->addWidget(thicknessPlot);
    thicknessPage->setLayout(thicknessPageLayout);

    QWidget *dielectricPage = new QWidget;
    QHBoxLayout *dielectricPageLayout = new QHBoxLayout;
    dielectricPageLayout->addWidget(dielectricPlot);
    dielectricPage->setLayout(dielectricPageLayout);

    tab->addTab(thicknessPage, tr("厚度检测结果"));
    tab->addTab(dielectricPage, tr("介电常数检测结果"));


    QPushButton *axisSetupButton = new QPushButton;
    axisSetupButton->setText(tr("图表设置"));
    axisSetupButton->setIcon(QIcon(":/images/setup2.png"));
    axisSetupButton->setIconSize(QSize(32,32));
    //axisSetupButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QPushButton *printButton = new QPushButton;
    printButton->setText(tr("打印"));
    printButton->setIcon(QIcon(":/images/print.png"));
    printButton->setIconSize(QSize(32,32));
    //printButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QPushButton *exportButton = new QPushButton;
    exportButton->setText(tr("输出报表"));
    exportButton->setIcon(QIcon(":/images/export.png"));
    exportButton->setIconSize(QSize(32,32));
    //exportButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);


    //------------------Layout-----------------------
    // Buttons Layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(axisSetupButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(printButton);
    buttonsLayout->addWidget(exportButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(tab);
    dialogLayout->addLayout(buttonsLayout);


    //-----------Configuration------------
    setLayout(dialogLayout);
    setWindowTitle(tr("厚度检测"));

    loadData();

}

//
//  Set a plain canvas frame and align the scales to it
//
void ThicknessDialog::alignScales(QwtPlot *plot)
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = plot->axisWidget( i );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );

        QwtScaleDraw *scaleDraw = plot->axisScaleDraw( i );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }
    plot->plotLayout()->setAlignCanvasToScales( true );
}



void ThicknessDialog::loadData()
{
    // 读厚度
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("打开厚度文件"), ".",
                tr("厚度文件 (*.txt)"));

    QVector<QPointF> thicknessVal;

    QFile thicknessFile(fileName);
    if (thicknessFile.open(QIODevice::ReadOnly)) {
        //qDebug() << "读取厚度文件成功！"<< endl;
        QTextStream in(&thicknessFile);
        double x;
        for (int i = 0; i < 10000; i++) {
            in >> x;
            thicknessVal.append(QPointF(i, x));
            //qDebug() << "读入厚度" << i << ":" << thicknessVal[i] << endl;
        }
        thicknessFile.close();
    }
    thickness_curve->setSamples(thicknessVal);
    LegendItem *thicknessLegendItem = new LegendItem();
    thicknessLegendItem->setBackgroundMode(QwtPlotLegendItem::LegendBackground);
    thicknessLegendItem->setBorderPen(QPen(Qt::black));
    thicknessLegendItem->setTextPen(QPen(Qt::black));
    thicknessLegendItem->setBackgroundBrush(QBrush(Qt::white));
    thicknessLegendItem->setAlignment(Qt::AlignTop | Qt::AlignRight);
    thicknessLegendItem->setBorderRadius( 0 );
    thicknessLegendItem->setMargin( 4 );
    thicknessLegendItem->setSpacing( 2 );
    thicknessLegendItem->setItemMargin( 0 );
    thicknessLegendItem->attach(thicknessPlot);



    // 读介电常数
    fileName = QFileDialog::getOpenFileName(
                this,
                tr("打开介电常数文件"), ".",
                tr("介电常数文件 (*.txt)"));

    QVector<QPointF> dielectricVal;

    QFile dielectricFile(fileName);
    if (dielectricFile.open(QIODevice::ReadOnly)) {
        //qDebug() << "读取介电常数文件成功！"<< endl;
        QTextStream in(&dielectricFile);
        double x;
        for (int i = 0; i < 10000; i++) {
            in >> x;
            dielectricVal.append(QPointF(i, x));
            //qDebug() << "读入介电常数" << i << ":" << dielectricVal[i] << endl;
        }
        dielectricFile.close();
    }
    dielectric_curve->setSamples(dielectricVal);
    LegendItem *dielectricLegendItem = new LegendItem();
    //dielectricLegendItem->setBackgroundMode(QwtPlotLegendItem::LegendBackground);
    dielectricLegendItem->setBorderPen(QPen(Qt::black, 1));
    dielectricLegendItem->setTextPen(QPen(Qt::black));
    dielectricLegendItem->setBackgroundBrush(QBrush(Qt::white));
    dielectricLegendItem->setAlignment(Qt::AlignTop | Qt::AlignRight);
    dielectricLegendItem->setBorderRadius( 0 );
    dielectricLegendItem->setMargin( 4 );
    thicknessLegendItem->setSpacing( 2 );
    dielectricLegendItem->setItemMargin( 0 );
    dielectricLegendItem->attach(dielectricPlot);

}



