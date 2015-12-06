#include <QtGui>
#include <cmath>
#include "curveplotter.h"
#include <dsptools.h>
/**
 * @brief 构造函数，设置默认参数
 * @param parent
 */
CurvePlotter::CurvePlotter(QWidget *parent)
    : QWidget(parent)
{
    // ------------- 默认参数设置 ----------------
    isAntialiased = false;              // 默认不开启抗锯齿
    isGridOn = true;                    // 默认绘制网格
    isLocationLineOn = true;            // 默认开启定位光标(随鼠标移动的黑线)
    mousePosInfo = 0;                   // TODO
    gridNum = 6;                        // 默认在较窄方向(默认横向)显示6个网格
    displayOrientation = VerticalDisplay;     // 默认纵向显示
    setCurveColor(Qt::red);
    setCurveWidth(1);

    // 默认坐标轴参数
    adjustAxisX(0.0, 32768.0);
    adjustAxisY(0.0, 1100);
    qDebug() << "minX = " << minX << "maxX = " << maxX
             <<  "minY = " << minY << "maxY = " << maxY
              <<  "spanX = " << spanX() << "spanY = " << spanY()
             << "numXTick = " << numYTicks << "numYTicks" << numYTicks << endl;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    if (displayStyle = VerticalDisplay)
//        setFixedWidth(150);
//    else    // displayStyle = HorizontalDisplay
//        setFixedHeight(150);
    setMouseTracking(true);
}


/**
 * @brief 装入一条曲线数据
 * @param id
 * @param data
 */
void CurvePlotter::setCurveData(int id, const QVector<QPointF> &data, bool isAutoScale)
{
    // 如果isAutoScale = true，则自动计算坐标轴幅度，如果isAutoScale默认值为true
    // 否则需要用户在调用setCurveData前调用setAxisX(),setAxisY()手动设定
    if (isAutoScale) {
        double minVal = data[0].y();
        double maxVal = data[0].y();
        double axisScale = 0;
        for (int i = 0; i < data.count(); i ++) {
            if (data[i].y() < minVal)
                minVal = data[i].y();
            if (data[i].y() > maxVal)
                maxVal = data[i].y();
        }
        // 为了让波形中心在零点，幅值取maxVal和minVal的较大值的1.05倍，以便不会显得饱和
        axisScale = qMax(abs(minVal), abs(maxVal)) * 1.05;
        if (displayOrientation == HorizontalDisplay) {
            setAxisX(0, data.count()-1);
            setAxisY(-axisScale, axisScale);
        } else {    // displayOrientation == HorizontalDisplay
            setAxisX(-axisScale, axisScale);
            setAxisY(0, data.count()-1);
        }
    }
    // 载入数据
    curveMap[id] = data;
    update();
}



/**
 * @brief 装入一条曲线数据
 * @param id
 * @param data
 */
void CurvePlotter::setCurveData(int id, const QVector<double> &data, bool isAutoScale)
{
    QVector<QPointF> points;
    for (int i = 0; i < data.count(); ++i)
        points.append(QPointF(i, data[i]));
    setCurveData(id, points, isAutoScale);
}


/**
 * @brief 清除一条指定曲线
 * @param id
 */
void CurvePlotter::clearCurve(int id)
{
    curveMap.remove(id);
    update();
}


/**
 * @brief 绘图事件
 * @param event
 */
void CurvePlotter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, isAntialiased);

    // 绘制边框
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    //painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));  // 背景颜色
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    // 绘制网格
    if (isGridOn)
        drawGrid(&painter);

    // 绘制零轴
    if (displayOrientation == VerticalDisplay) {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(width()/2, 0, width()/2, height()-1);
    } else {    // displayStyle == HorizontalDisplay
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, height()/2, width()-1, height()/2);
    }

    // 绘制曲线
    drawCurves(&painter);

    // 绘制定位光标(随鼠标移动的黑线)
    if (isLocationLineOn) {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
        if (displayOrientation == VerticalDisplay)
            painter.drawLine(0, mousePos.y(), width(), mousePos.y());
        else    // displayStyle == HorizontalDisplay
            painter.drawLine(mousePos.x(), 0, mousePos.x(), height());
    }


}


/**
 * @brief 鼠标移动过程中，更新记录鼠标坐标的变量mousePos
 * @param event
 */
void CurvePlotter::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    //qDebug() << "curve mouse Event sample = " << event->pos().y() * curveMap[0].count() / height() << endl;
    update();
}

/**
 * @brief 更新鼠标坐标
 * @param new_pos
 */
void CurvePlotter::refershMousePos(QPoint new_pos)
{
    mousePos = new_pos;
    update();
}

/**
 * @brief 清除所有曲线
 */
void CurvePlotter::clearAllCurves()
{
    QMapIterator<int, QVector<QPointF> > i(curveMap);
    while (i.hasNext()) {
        i.next();

        int id = i.key();
        clearCurve(id);
    }
}


/**
 * 最小尺寸提示
 * @return
 */
QSize CurvePlotter::minimumSizeHint() const
{
    return QSize(100, 100);
}

/**
 * 最佳尺寸提示
 * @return
 */
QSize CurvePlotter::sizeHint() const
{
    if (displayOrientation == VerticalDisplay)
        return QSize(150, 320);
    else
        return QSize(320, 150);
}

/**
 * @brief 设置x轴坐标坐标范围
 * @param min
 * @param max
 */
void CurvePlotter::adjustAxisX(double min, double max)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numXTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numXTicks < MinTicks)
        numXTicks = MinTicks;
    minX = std::floor(min / step) * step;
    maxX = std::ceil(max / step) * step;
}

/**
 * @brief 设置x轴坐标坐标范围
 * @param min
 * @param max
 */
void CurvePlotter::adjustAxisY(double min, double max)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numYTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numYTicks < MinTicks)
        numYTicks = MinTicks;
    minY = std::floor(min / step) * step;
    maxY = std::ceil(max / step) * step;
}

/**
 * @brief 设置网格颜色
 * @param color
 */
void CurvePlotter::setGridColor(const QColor &color)
{
    gridPen.setColor(color);
    update();
}

/**
 * @brief 设置网格粗细
 * @param penWidth
 */
void CurvePlotter::setGridWidth(int penWidth)
{
    gridPen.setWidth(penWidth);
    update();
}

/**
 * @brief 是否开启抗锯齿
 * @param antialiased
 */
void CurvePlotter::setAntialiased(bool antialiased)
{
    isAntialiased = antialiased;
    update();
}

/**
 * @brief 显示/隐藏网格
 * @param isGridLineOn
 */
void CurvePlotter::showGrid(bool isGridLineOn)
{
    this->isGridOn = isGridLineOn;
    update();
}



/**
 * @brief 显示/隐藏定位光标(随鼠标移动的黑线)
 * @param isLocationLineOn
 */
void CurvePlotter::showLocationLine(bool isLocationLineOn)
{
    this->isLocationLineOn = isLocationLineOn;
    update();
}


/**
 * @brief 绘制网格
 * @param painter
 */
void CurvePlotter::drawGrid(QPainter *painter)
{
    // 设置网格线的画笔
    //painter->setPen(QPen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap));
    painter->setPen(QPen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap));
    if (displayOrientation == VerticalDisplay) {  // 纵向显示情况
        gridSpace = width() / gridNum;  // 网格间距
        int gridNumX = gridNum;
        int gridNumY = height() / gridSpace;
        // 竖线
        for (int i = 0; i <= gridNumX; ++i)
            painter->drawLine(gridSpace * i, 0, gridSpace * i, height());
        // 横线
        for (int j = 0; j <= gridNumY; ++j)
            painter->drawLine(0, gridSpace * j, width(), gridSpace * j);
    } else {    // 横向向显示情况
        gridSpace = height() / gridNum;  // 网格间距
        int gridNumX = width() / gridSpace;
        int gridNumY = gridNum;
        // 横线
        for (int i = 0; i <= gridNumY; ++i)
            painter->drawLine(0, gridSpace * i, width(), gridSpace * i);
        // 竖线
        for (int j = 0; j <= gridNumX; ++j)
            painter->drawLine(gridSpace * j, 0, gridSpace * j, height());
    }

}


/**
 * @brief 绘制曲线
 * @param painter
 */
void CurvePlotter::drawCurves(QPainter *painter)
{
//    // 暂时可以使用6种颜色来描述id=1-6的曲线
//    static const QColor colorForIds[6] = {
//        Qt::blue, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
//    };
    // 遍历所有曲线，并全部画出来
    QMapIterator<int, QVector<QPointF> > iter(curveMap);
    while (iter.hasNext()) {
        iter.next();
        int id = iter.key();    // 取得当前曲线的key，用来确定该曲线颜色
        QVector<QPointF> data = iter.value();   // 取得当前曲线的原始坐标数据
        QPolygonF polyline(data.count());       // 要画在屏幕上的曲线
        // 遍历曲线的每一个点，将原始坐标映射到Widget坐标系
        for (int i = 0; i < data.count(); i++) {
            double dx;  // x方向偏移量
            double dy;  // y方向偏移量
            double x;   // 映射到Widget上的坐标值
            double y;   // 映射到Widget上的坐标值
            if (displayOrientation == VerticalDisplay) {  // 纵向显示情况
                dx = data[i].y() - minX; // x方向代表振幅
                dy = data[i].x() - minY; // y方向代表点数
                // 坐标映射
                x = 0 + (dx * (width() - 1)/spanX());
                y = 0 + (dy * (height() - 1)/spanY());
            } else { // displayStyle == HorizontalDisplay, 横向显示情况
                dx = data[i].x() - minX; // x方向代表点数
                dy = data[i].y() - minY; // y方向代表振幅
                // 坐标映射
                x = 0 + (dx * (width() - 1)/spanX());
                y = height() - (dy * (height() - 1)/spanY());
            }
            // 将坐标映射后的点存入待画在屏幕上的曲线
            polyline[i] = QPointF(x, y);
        }
        // 绘制坐标映射后的曲线
        //painter->setPen(QPen(colorForIds[uint(id) % 6], 1, Qt::SolidLine, Qt::RoundCap));
        painter->setPen(QPen(curveColor, curveWidth, Qt::SolidLine, Qt::RoundCap));
        painter->drawPolyline(polyline);
    }
}



