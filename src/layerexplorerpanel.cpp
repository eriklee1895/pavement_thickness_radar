#include <QtGui>
#include "layerexplorerpanel.h"

ColorTag::ColorTag(QColor color, QWidget *parent)
    : QWidget(parent)
{
    setTagColor(color);
    setFixedSize(16, 16);
    setMouseTracking(true);
    isMouseOnTag = false;
    setToolTip(tr("修改该层颜色"));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}


void ColorTag::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(QBrush(tagColor, Qt::SolidPattern));
    painter.drawRoundRect(QRect(0, 0, width()-1, height()-1));
    if (isMouseOnTag) {
//        painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));
//        painter.drawRoundRect(QRect(0, 0, width()-1, height()-1));
    }
}

void ColorTag::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->x() > 0 && event->x() < (width()-1) && event->y() > 0 && event->y() < (height()-1) )
        isMouseOnTag = true;
    else
        isMouseOnTag = false;
    update();
}

void ColorTag::mousePressEvent(QMouseEvent *event)
{
    QColorDialog::setCustomColor(0,QRgb(0xFF0000));
    QColor color = QColorDialog::getColor();
    if(color.isValid())
        setTagColor(color);
    update();
}



//LayerVisibleCheckBox::LayerVisibleCheckBox(QWidget *parent)
//    : QCheckBox(parent)
//{

//}



PickLayerButton::PickLayerButton(QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(20, 20);
    setIcon(QIcon(":/images/finger.png"));
    setToolTip(tr("重新拾取该层"));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}



DeleteLayerButton::DeleteLayerButton(QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(20, 20);
    setIcon(QIcon(":/images/delete.png"));
    setToolTip(tr("删除该层"));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}



LayerExplorerPanel::LayerExplorerPanel(QWidget *parent)
    : QWidget(parent)
{
    layerNum = 5;
    layoutPanel();
    //--------------默认参数设置-----------------------

}


void LayerExplorerPanel::layoutPanel()
{
    const QColor colorIndex[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };

    for (int i = 0; i < layerNum; i++) {
        layerLabels.append(new QLabel(tr("第%1层:").arg(i+1)));
        colorTags.append(new ColorTag);
        colorTags[i]->setTagColor(colorIndex[i]);
        layerVisibleCheckBoxs.append(new QCheckBox);
        pickLayerButtons.append(new PickLayerButton);
        deleteLayerButtons.append(new DeleteLayerButton);
    }

    // ---------------------Layer Explorer面板---------------------
    m_LayerExplorerGroup = new QGroupBox;
    m_LayerExplorerGroup->setTitle(tr("反射层管理"));

    QPushButton *addLayerButton = new QPushButton(tr("添加层"));

    //----------------Layout---------------------
    // Layer Explorer面板Layout
    QGridLayout *layerExplorerGroupLayout = new QGridLayout;
    for (int i = 0; i < layerNum; i++) {
        layerExplorerGroupLayout->addWidget(layerLabels[i], i, 0);
        layerExplorerGroupLayout->addWidget(colorTags[i], i, 2);
        layerExplorerGroupLayout->addWidget(pickLayerButtons[i], i, 3);
        layerExplorerGroupLayout->addWidget(deleteLayerButtons[i], i, 4);
        layerExplorerGroupLayout->addWidget(layerVisibleCheckBoxs[i], i, 1);
        //layerExplorerGroupLayout->addWidget(new ColorTag(Qt::red), i, 5);
        layerExplorerGroupLayout->setColumnStretch(i, 5);
    }
    layerExplorerGroupLayout->addWidget(addLayerButton, layerNum, 0, layerNum, 2);
    layerExplorerGroupLayout->setAlignment(Qt::AlignLeft);
    m_LayerExplorerGroup->setLayout(layerExplorerGroupLayout);

    //layerExplorerGroupLayout->addWidget(addLayerButton, 2, 0);
//    layerExplorerGroupLayout->addWidget(a, 2, 1);
//    layerExplorerGroupLayout->addWidget(b, 2, 2);


    // 主面板Layout
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_LayerExplorerGroup);
    mainLayout->addStretch();
    mainLayout->setMargin(0);
    setLayout(mainLayout);

}


void LayerExplorerPanel::createColorComboBox(QComboBox *comboBox)
{
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.fillRect(0, 0, 16, 16, Qt::white);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::white);
    painter.fillRect(0, 0, 16, 16, Qt::black);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::black);
    painter.fillRect(0, 0, 16, 16, Qt::red);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::red);
    painter.fillRect(0, 0, 16, 16, Qt::green);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::green);
    painter.fillRect(0, 0, 16, 16, Qt::blue);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::blue);
    painter.fillRect(0, 0, 16, 16, Qt::yellow);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::yellow);
    painter.fillRect(0, 0, 16, 16, Qt::cyan);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::cyan);
    painter.fillRect(0, 0, 16, 16, Qt::magenta);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::magenta);
    painter.fillRect(0, 0, 16, 16, Qt::gray);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::gray);
    painter.fillRect(0, 0, 16, 16, Qt::darkRed);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkRed);
    painter.fillRect(0, 0, 16, 16, Qt::darkGreen);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkGreen);
    painter.fillRect(0, 0, 16, 16, Qt::darkBlue);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkBlue);
    painter.fillRect(0, 0, 16, 16, Qt::darkCyan);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkCyan);
    painter.fillRect(0, 0, 16, 16, Qt::darkMagenta);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkMagenta);
    painter.fillRect(0, 0, 16, 16, Qt::darkYellow);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkYellow);
    painter.fillRect(0, 0, 16, 16, Qt::darkGray);
    comboBox->addItem(QIcon(pixmap), tr(""), Qt::darkGray);
    comboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}




