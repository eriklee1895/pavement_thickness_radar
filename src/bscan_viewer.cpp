#include <QtGui>
#include <QVector>
#include <cmath>
#include "bscan_viewer.h"
#include "curveplotter.h"

/**
 * @brief Bscan_Viewer::Bscan_Viewer - 构造函数
 * @param plotter
 * @param parent
 */
Bscan_Viewer::Bscan_Viewer(CurvePlotter *plotter, QWidget *parent) :
    QWidget(parent)
{
    m_curvePlotter = plotter;

    setMouseTracking(true);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200, 200);

    // 初始状态
    isDataLoaded = false;   // 初始未载入数据
    isRulerVisible = true;    // 默认显示标尺

    //-------标尺尺寸参数-------
    if (isRulerVisible)
        rulerSpan = 40;             // 标尺占高度空间
    else
        rulerSpan = 0;              // 不显示标尺

    rulerSpace = 2;             // 标尺与剖面图空隙
    rulerScaleLength = 8;       // 标尺短刻度刻度长度

    gprFileName = tr("空文件");
    mousePosInfoString = tr("(0道，0点)");
}


/**
 * @brief Bscan_Viewer::drawTriangle - 画顶部和底部三角形
 * @param xpos
 * @param painter
 */
void Bscan_Viewer::drawTriangle(QPoint mousePos, QPainter* painter)
{
    // 画标记道数的红色三角
    QPolygon pointsTopTriangle;
    pointsTopTriangle << QPoint(mousePos.x(), rulerSpan)
                      << QPoint(mousePos.x()-6, rulerSpan-8)
                      << QPoint(mousePos.x()+6, rulerSpan-8);

    QPolygon pointsBottomTriangle;
    pointsBottomTriangle << QPoint(mousePos.x(), height()-2-rulerSpan+rulerSpace)
                         << QPoint(mousePos.x()-6, height()-2-rulerSpan+rulerSpace+8)
                         << QPoint(mousePos.x()+6, height()-2-rulerSpan+rulerSpace+8);

    painter->setBrush(QColor(255,0,0));
    painter->drawPolygon(pointsTopTriangle);
    painter->drawPolygon(pointsBottomTriangle);

    // 画标点数与时延的红色三角
    if (isRulerVisible) {
        QPolygon pointsLeftTriangle;
        pointsLeftTriangle << QPoint(rulerSpan-rulerSpace, mousePos.y())
                          << QPoint(rulerSpan-rulerSpace-8, mousePos.y()-6)
                          << QPoint(rulerSpan-rulerSpace-8, mousePos.y()+6);

        QPolygon pointsRightTriangle;
        pointsRightTriangle << QPoint(width()-1-rulerSpan+rulerSpace, mousePos.y())
                             << QPoint(width()-1-rulerSpan+rulerSpace+8, mousePos.y()-6)
                             << QPoint(width()-1-rulerSpan+rulerSpace+8, mousePos.y()+6);

        painter->setBrush(QColor(255,0,0));
        painter->drawPolygon(pointsLeftTriangle);
        painter->drawPolygon(pointsRightTriangle);
    }


}


/**
 * @brief Bscan_Viewer::paintEvent
 * @param event
 */
void Bscan_Viewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);


    // -------------画像素映射Bar-----------------
    painter.setPen(Qt::black);
    QPixmap data_pixmap(200,100);
    data_pixmap.fill(QColor(255,255,255));

    // ------------自己定义的灰度Bar----------------
//    QLinearGradient gradient(20, 20, 20, 220);
//    gradient.setColorAt(0.0, Qt::white);
//    gradient.setColorAt(1.0, Qt::black);
//    painter.setBrush(QBrush(gradient));
//    painter.setPen(Qt::black);
//    painter.drawRect(20,20, 30, 200);

    // 如果数据已经载入，则绘制曲线及定位光标
    if(isDataLoaded) {
        // --------绘制标尺-----------
        drawRulers(&painter);

        // --------绘制BScan剖面图-----------
        painter.drawPixmap(rulerSpan, rulerSpan, bufferPixmap);

        // --------绘制层位-----------
        drawLayers(&painter);

        // 绘制参考竖线
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(mousePos.x(), rulerSpan, mousePos.x(), height()-2-rulerSpan);
        // 绘制红色定位三角
        drawTriangle(mousePos, &painter);

        // 绘制定位光标
        painter.setPen(Qt::yellow);
        painter.drawLine(mousePos.x() - 20, mousePos.y(), mousePos.x() + 20, mousePos.y());
        painter.drawLine(mousePos.x(), mousePos.y() - 20, mousePos.x(), mousePos.y()+ 20);
    }

    // -----绘制中央背景-------
    if (!isDataLoaded) {
        QFont textFont;
        textFont.setPixelSize(30);
        painter.setPen(QPen(Qt::black));
        painter.setFont(textFont);
        painter.drawText(QRect(0, 0, width(), height() - 50), Qt::AlignCenter, tr("中科院电子所\n超宽带雷达实验室"));
        QPixmap pixmapPulse(":/images/pulse.png");
        painter.drawPixmap(width()/2 - 50, height()/2 + 10, 100, 100, pixmapPulse);
    }

    // --------绘制边框-----------
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width()-1, height() -1));

}

/**
 * @brief 鼠标在B-SCAN上移动时，更新A-SCAN单道波形
 * @param event
 */
void Bscan_Viewer::mouseMoveEvent(QMouseEvent *event)
{
    // 如果已经载入GPR数据，则鼠标在Profile上移动时，
    // 令m_curvePlotter实时更新单道波形
    if (isDataLoaded) {
        // 记录输入的道数
        double dx = (double)m_traceNum / (bufferPixmap.width()-1);
        double dy = (double)m_samplingNum / (bufferPixmap.height()-1);
        //QPoint originPoint(rulerSpan, rulerSpan);   // B-Scan的原点
        int currentX = (int)((event->pos().x()-rulerSpan) * dx);   // B-Scan图中的X坐标(道数)
        int currentY = (int)((event->pos().y()-rulerSpan) * dy);  // B-Scan图中的Y坐标(点数)
        // 当鼠标进入B-Scan图区时，将当前坐标显示在状态栏，并将该道数据显示在A-Scan上
        if ((event->pos().x() >= rulerSpan) && (event->pos().y() >= rulerSpan) &&
                (event->pos().x() <= width()-2-rulerSpan) && (event->pos().y() <= height()-2-rulerSpan)) {
            // 把该道数据送入buffer
            QVector<QPointF> points;
            QVector<double> trace_values;
            for (int i = 0; i < m_samplingNum; i++){
                points.append(QPointF(i, bscan_data[currentX][i]));
                trace_values.append(bscan_data[currentX][i]);
            }
            // 显示A-SCAN，坐标幅值范围默认自动确定
            m_curvePlotter->setCurveData(0, points);

            //-----鼠标移动时更新鼠标所指处的道数和点数，以便状态栏显示------
            mousePosInfoString = tr("(%1道，%2点, %3)")
                    .arg(currentX+1)
                    .arg(currentY+1)
                    .arg(bscan_data[currentX][currentY]);
            mousePos = event->pos();
            emit mouse_pos_changed();   // 发射signal，以便CurvePlotter和MainWindow的状态栏更新显示
        }
        // A-Scan图与B-Scan图同高
        m_curvePlotter->setFixedHeight(bufferPixmap.height());
    }

    // 更新m_curvePlotter的鼠标坐标，以便实时刷新横线
    m_curvePlotter->refershMousePos(event->pos());
    // 刷新显示
    update();

}

/**
 * @brief B-Scan鼠标按下事件
 *        响应的动作包括：获取鼠标坐标对应的B-Scan对应的道数点数，更新A-Scan波形
 * @param event
 */
void Bscan_Viewer::mousePressEvent(QMouseEvent *event)
{
    // 只有当载入数据
    if (isDataLoaded) {
        if(event->button() == Qt::LeftButton) {
            // 当鼠标进入B-Scan图区时
            if ((event->pos().x() >= rulerSpan) && (event->pos().y() >= rulerSpan) &&
                    (event->pos().x() <= width()-2-rulerSpan) && (event->pos().y() <= height()-2-rulerSpan)) {
                // 屏幕坐标
                int x = event->x() - rulerSpan;
                int y = event->y() - rulerSpan;
                QPoint startPoint;
                // 把屏幕坐标转换为B-Scan数据中的道数与点数
                double startX = static_cast<double>(x) / (bufferPixmap.width()-1) * m_traceNum;
                double startY = static_cast<double>(y) / (bufferPixmap.height()-1) * m_samplingNum;
                startPoint.setX(static_cast<int>(startX));
                startPoint.setY(static_cast<int>(startY));
                // 如果点在靠左的位置则变成0
//                if (startPoint.x() < m_traceNum*0.016)
//                    startPoint.setX(static_cast<int>(0));
                emit mouseClicked(startPoint);  // 告知MainWindow状态栏与A-Scan Plotter更新信息
            }
        }
    }
}


/**
 * @brief Bscan_Viewer::resizeEvent
 * @param event
 */
void Bscan_Viewer::resizeEvent(QResizeEvent *event)
{
    refreshBufferPixmap();
}





/**
 * @brief Bscan_Viewer::setData
 * @param data  - 二维剖面数据data[][]
 * @param samplingNum   - 每道采样点数
 * @param traceNum  - 道数
 */
void Bscan_Viewer::setData(double **data, int samplingNum, int traceNum)
{
    bscan_data = data;
    m_samplingNum = samplingNum;
    m_traceNum = traceNum;

    // 采样率和时窗手动设置
    fs = 40;
    time_window = 1/fs * m_samplingNum;

    isDataLoaded = true;
    refreshBufferPixmap();
}

/**
 * @brief 画GPR BSCAN剖面图
 * @param painter
 */
void Bscan_Viewer::drawBscanProfile(QPainter *painter)
{
    qDebug() << "bufferPixmap尺寸：" << bufferPixmap.size() << endl;
    qDebug() << "bufferPixmap尺寸：" << width() << endl;
    int w = bufferPixmap.size().width();
    qDebug() << "w:" << bufferPixmap.width() << endl;
    // 一开始dx与dy用int型，出现BSCAN绘制错位，一直找不到原因，原来应该用double型！
    double dx = (double)(m_traceNum) / (bufferPixmap.width()-1);          // x方向步长
    double dy = (double)m_samplingNum / (bufferPixmap.height()-1);      // y方向步长
//    if(m_traceNum <= width()) dx = 1.0;
//    if(m_samplingNum <= height()) dy = 1.0;

/*
    //旧代码，是按照0~65535来伸缩灰度的，这样当信号滤波后信号幅值变低，会使对比度变差
    int gray_index = 0;     // 灰度值值
    for (int i = 0; i < width()- 1; i++) {       // 遍历每一道
        for (int j = 0; j < height()- 1; j++) {  // 遍历1道中的每一点
            // 按信号的幅度计算灰度值
            gray_index = (int)((bscan_data[(int)(i*dx)][(int)(j*dy)] + 32768.0) / 65536.0 * 255.0);
            // 设置画笔
            painter->setPen(QColor(gray_index, gray_index, gray_index));
            // 画出该点
            painter->drawPoint(i, j);
        }
    }
*/
    // 先计算GPR数据中的最大与最小幅值
    double minValue = 0;    // GPR数据中的最小幅值
    double maxValue = 0;    // GPR数据中的最大幅值
    double span = 0;        // 幅值的跨度
    for (int i = 0; i < m_traceNum; i++) {       // 遍历每一道
        for (int j = 0; j < m_samplingNum; j++) {  // 遍历1道中的每一点
            if (bscan_data[i][j] > maxValue)
                maxValue = bscan_data[i][j];
            if (bscan_data[i][j] < minValue)
                minValue = bscan_data[i][j];
        }
    }
    span = maxValue - minValue;

    int gray_index = 0;     // 灰度值值
    for (int i = 0; i < bufferPixmap.width()- 1; i++) {       // 遍历每一道
        for (int j = 0; j < bufferPixmap.height()- 1; j++) {  // 遍历1道中的每一点
            // 按信号的幅度计算灰度值
            gray_index = (int)((bscan_data[(int)(i*dx)][(int)(j*dy)] - minValue) / span * 255.0);
            // 设置画笔
            painter->setPen(QColor(gray_index, gray_index, gray_index));
            // 画出该点
            painter->drawPoint(i, j);
        }
    }
}



/**
 * @brief Bscan_Viewer::refreshBufferPixmap
 */
void Bscan_Viewer::refreshBufferPixmap()
{
    // 画布尺寸应该给标尺留出余量，余量为2*rulerHight
    bufferPixmap = QPixmap(size().width() - 2*rulerSpan, size().height() - 2*rulerSpan);
    bufferPixmap.fill(this, rulerSpan, rulerSpan);
    QPainter painter(&bufferPixmap);
    painter.initFrom(this);
    // 画BSCAN剖面图
    if(isDataLoaded)
        drawBscanProfile(&painter);
    update();
}



/**
 * @brief 画介质层
 * @param painter
 * @param layer
 */
void Bscan_Viewer::drawLayers(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    QMapIterator<int, QVector<QPointF> > iter(layerTracksMap);
    while(iter.hasNext()) {     // 遍历所有层
        iter.next();
        int id = iter.key();
        QVector<QPointF> data = iter.value();
        QPolygonF layer(data.count());
        for (int i = 0; i < layer.count(); i++) {
            double dx = data[i].x();
            double dy = data[i].y();
            double x = rulerSpan + dx/m_traceNum * (bufferPixmap.width() - 1);
            double y = rulerSpan + dy/m_samplingNum * (bufferPixmap.height() -1);
            layer[i] = QPointF(x,y);
        }
        // 画出该层曲线
        painter->setPen(QPen(colorForIds[uint(id) % 6], 3, Qt::SolidLine, Qt::RoundCap));
        painter->drawPoints(layer);
       // painter->drawPolyline(layer);
    }
}

/**
 * @brief setLayer
 * @param layerPoints
 */
void Bscan_Viewer::setLayer(int id, const QVector<QPointF> &layerPoints)
{
    layerTracksMap[id] = layerPoints;
    update();
}

/**
 * @brief 清除1条指定层曲线
 * @param id
 */
void Bscan_Viewer::clearLayerTrack(int id)
{
    layerTracksMap.remove(id);
    update();
}

/**
 * @brief 清除所有层曲线
 */
void Bscan_Viewer::clearAllLayerTracks()
{
    QMapIterator<int, QVector<QPointF> > iter(layerTracksMap);
    while (iter.hasNext()) {
        iter.next();

        int id = iter.key();
        clearLayerTrack(id);
    }
}

/**
 * @brief 绘制3个标尺
 * @param painter
 */
void Bscan_Viewer::drawRulers(QPainter *painter)
{
    // GPR数据抽样步长
    double dx = (double)m_traceNum / (bufferPixmap.width()-1);          // x方向步长
    double dy = (double)m_samplingNum / (bufferPixmap.height()-1);      // y方向步长

    // 设置每个标尺的起点与重点
    // 顶部标尺起点
    QPoint topRulerStartPoint(rulerSpan, rulerSpan-rulerSpace);
    QPoint topRulerEndPoint(width()-2-rulerSpan, rulerSpan-rulerSpace);
    // 底部标尺起点
    QPoint bottomRulerStartPoint(rulerSpan, height()-2-rulerSpan + rulerSpace);
    QPoint bottomRulerEndPoint(width()-2-rulerSpan, height()-2-rulerSpan + rulerSpace);
    // 左端标尺起点
    QPoint leftRulerStartPoint(rulerSpan-rulerSpace, rulerSpan);
    QPoint leftRulerEndPoint(rulerSpan-rulerSpace, height()-2-rulerSpan);
    // 右端标尺起点
    QPoint rightRulerStartPoint(topRulerEndPoint.x()+rulerSpace, leftRulerStartPoint.y());
    QPoint rightRulerEndPoint(topRulerEndPoint.x()+rulerSpace, leftRulerEndPoint.y());

    // ---------------------画顶部道数标尺(道数)-------------------------
//    painter->setBrush(QColor(0,0,0));
//    painter->drawLine(topRulerStartPoint, topRulerEndPoint);    // 标尺底轴
//    // 数字刻度的间距取值策略
//    int topNumberTick;
//    if (m_traceNum < 1000 && m_traceNum >100)
//        topNumberTick = (m_traceNum/10/10) *10;
//    else if (m_traceNum <= 100)
//        topNumberTick = m_traceNum/10;
//    else
//        topNumberTick = (m_traceNum/10/10/10) * 10 * 10;
//    // 将数字刻度变化为坐标步进值
//    double topNumberTickStep = (double)topNumberTick / dx;
//    // 先画带数字刻度的最大坐标
//    for(int i = 0; i < (double)bufferPixmap.width()/topNumberTickStep; i++) {
//        // 最大刻度的起点和终点
//        QPoint numberTickStartPoint(topRulerStartPoint.x()+topNumberTickStep*i,topRulerStartPoint.y());
//        QPoint numberTickEndPoint(topRulerStartPoint.x()+topNumberTickStep*i,topRulerStartPoint.y()-rulerScaleLength);
//        // 画数字
//        QString numberString = QString::number(topNumberTick*i);   // 把数字刻度转换成字符串
//        QSize numberStringSize = fontMetrics().size(0, numberString);   // 数字刻度数字的Size，下面绘制定坐标用
//        painter->drawText(numberTickEndPoint - QPoint(numberStringSize.width()/2, 2), numberString);
//        // 画道数Label
//        painter->drawText(rulerSpan + 0.5*bufferPixmap.width() - fontMetrics().width("道数")*0.5,
//                          fontMetrics().size(0, "道数").height()+2, tr("道数"));
//        // 画最长的有数字的刻度
//        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
//    }
//    // 画中等长度的刻度
//    for(int i = 0; i < (double)bufferPixmap.width()/topNumberTickStep*2; i++) {
//        // 中等长度刻度的起点和终点
//        QPoint numberTickStartPoint(topRulerStartPoint.x()+topNumberTickStep*i/2,topRulerStartPoint.y());
//        QPoint numberTickendPoint(topRulerStartPoint.x()+topNumberTickStep*i/2,
//                                  topRulerStartPoint.y()-rulerScaleLength*0.8);
//        // 画中等长度的刻度
//        painter->drawLine(numberTickStartPoint, numberTickendPoint);
//    }
//    // 画最小长度的刻度
//    for(int i = 0; i < (double)bufferPixmap.width()/topNumberTickStep*10; i++) {
//        // 中等长度刻度的起点和终点
//        QPoint numberTickStartPoint(topRulerStartPoint.x()+topNumberTickStep*i/10,topRulerStartPoint.y());
//        QPoint numberTickEndPoint(topRulerStartPoint.x()+topNumberTickStep*i/10,
//                                  topRulerStartPoint.y()-rulerScaleLength*0.4);
//        // 画最短的刻度
//        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
//    }


    // ---------------------画底部道数标尺(道数)-------------------------
    painter->setBrush(QColor(0,0,0));
    painter->drawLine(bottomRulerStartPoint, bottomRulerEndPoint);    // 标尺底轴
    // 数字刻度的间距取值策略
    int bottomNumberTick;
    if (m_traceNum < 1000 && m_traceNum >100)
        bottomNumberTick = (m_traceNum/10/10) *10;
    else if (m_traceNum <= 100)
        bottomNumberTick = m_traceNum/10;
    else
        bottomNumberTick = (m_traceNum/10/10/10) * 10 * 10;
    // 将数字刻度变化为坐标步进值
    double bottomNumberTickStep = (double)bottomNumberTick / dx;
    // 先画带数字刻度的最大坐标
    for(int i = 0; i < (double)bufferPixmap.width()/bottomNumberTickStep; i++) {
        // 最长刻度的起点和终点
        QPoint numberTickStartPoint(bottomRulerStartPoint.x()+bottomNumberTickStep*i,
                                    bottomRulerStartPoint.y());
        QPoint numberTickEndPoint(bottomRulerStartPoint.x() + bottomNumberTickStep*i,
                                  bottomRulerStartPoint.y() + rulerScaleLength);
        // 画数字
        QString numberString = QString::number(bottomNumberTick*i);   // 把数字刻度转换成字符串
        QSize numberStringSize = fontMetrics().size(0, numberString);   // 数字刻度数字的Size，下面绘制定坐标用
        painter->drawText(numberTickEndPoint - QPoint(numberStringSize.width()/2, -numberStringSize.height()+2), numberString);
        // 画道数Label
        painter->drawText(rulerSpan + 0.5*bufferPixmap.width() - fontMetrics().width("道数")*0.5,
                          height()-fontMetrics().size(0, "道数").height()+6, tr("道数"));
        // 画最长的有数字的刻度
        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
    }
    // 画中等长度的刻度
    for(int i = 0; i < (double)bufferPixmap.width()/bottomNumberTickStep*2; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(bottomRulerStartPoint.x()+(bottomNumberTickStep/2)*i,bottomRulerStartPoint.y());
        QPoint numberTickendPoint(bottomRulerStartPoint.x()+(bottomNumberTickStep/2)*i,
                                  bottomRulerStartPoint.y()+rulerScaleLength*0.8);
        // 画中等长度的刻度
        painter->drawLine(numberTickStartPoint, numberTickendPoint);
    }
    // 画最小长度的刻度
    for(int i = 0; i < (double)bufferPixmap.width()/bottomNumberTickStep*10; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(bottomRulerStartPoint.x()+bottomNumberTickStep*i/10,bottomRulerStartPoint.y());
        QPoint numberTickendPoint(bottomRulerStartPoint.x()+bottomNumberTickStep*i/10,
                                  bottomRulerStartPoint.y()+rulerScaleLength*0.4);
        // 画最短的刻度
        painter->drawLine(numberTickStartPoint, numberTickendPoint);
    }



    // ---------------------画左端点数标尺标尺(点数)-------------------------
    painter->setBrush(QColor(0,0,0));
    painter->drawLine(leftRulerStartPoint, leftRulerEndPoint);  // 标尺底轴
    // 数字刻度的间距取值策略
    int leftNumberTick;
    leftNumberTick = (m_samplingNum/10/10)*10;
    // 将数字刻度变化为坐标步进值
    double leftNumberTickStep = (double)leftNumberTick / dy;
    // 先画带数字刻度的最大坐标
    for(int i = 0; i < (double)bufferPixmap.height()/leftNumberTickStep; i++) {
        // 最大刻度的起点和终点
        QPoint numberTickStartPoint(leftRulerStartPoint.x(), leftRulerStartPoint.y()+leftNumberTickStep*i);
        QPoint numberTickendPoint(leftRulerStartPoint.x()-rulerScaleLength, leftRulerStartPoint.y()+leftNumberTickStep*i);
        // --------画数字(旋转了-90°)-----
        QString numberString = QString::number(leftNumberTick*i);   // 把数字刻度转换成字符串
        QSize numberStringSize = fontMetrics().size(0, numberString);   // 数字刻度数字的Size，下面绘制定坐标用
        painter->translate(numberTickendPoint.x(), numberTickendPoint.y());     //变换旋转中心到文字坐标，这一步重要
        painter->rotate(-90);   // 坐标系旋转-90°
        painter->drawText(-numberStringSize.width()/2, -2, numberString);
        painter->resetMatrix();     // 很重要：文字绘制完毕后把坐标系恢复，要不然后面画图是乱的！！！
        // 画点数Label
        painter->translate(0, rulerSpan + bufferPixmap.height()/2);
        painter->rotate(-90);
        painter->drawText(-fontMetrics().width("点数")*0.5, fontMetrics().size(0, "点数").height()+2, tr("点数"));
        painter->resetMatrix();
        // 画最长的有数字的刻度
        painter->drawLine(numberTickStartPoint, numberTickendPoint);
    }
    // 画中等长度的刻度
    for(int i = 0; i < (double)bufferPixmap.height()/leftNumberTickStep*2; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(leftRulerStartPoint.x(), leftRulerStartPoint.y()+leftNumberTickStep*i/2);
        QPoint numberTickendPoint(leftRulerStartPoint.x()-rulerScaleLength*0.8,
                                  leftRulerStartPoint.y()+leftNumberTickStep*i/2);
        // 画中等长度的刻度
        painter->drawLine(numberTickStartPoint, numberTickendPoint);
    }
    // 画最小长度的刻度
    for(int i = 0; i < (double)bufferPixmap.height()/leftNumberTickStep*10; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(leftRulerStartPoint.x(), leftRulerStartPoint.y()+leftNumberTickStep*i/10);
        QPoint numberTickendPoint(leftRulerStartPoint.x()-rulerScaleLength*0.4,
                                  leftRulerStartPoint.y()+leftNumberTickStep*i/10);
        // 画最短的刻度
        painter->drawLine(numberTickStartPoint, numberTickendPoint);
    }


    // ---------------------画右端时延标尺标尺(时延)-------------------------
    painter->setBrush(QColor(0,0,0));
    painter->drawLine(rightRulerStartPoint, rightRulerEndPoint);  // 标尺底轴
    // 数字刻度的间距取值策略
    int rightNumberTick = (int)(time_window/10);
    double time_delta = time_window / (bufferPixmap.height()-1);
    // 将数字刻度变化为坐标步进值
    double rightNumberTickStep = (double)rightNumberTick / time_delta;

    // 先画带数字刻度的最大坐标
    for(int i = 0; i < (double)bufferPixmap.height()/rightNumberTickStep; i++) {
        // 最大刻度的起点和终点
        QPoint numberTickStartPoint(rightRulerStartPoint.x(),
                                    rightRulerStartPoint.y()+rightNumberTickStep*i);
        QPoint numberTickEndPoint(rightRulerStartPoint.x()+rulerScaleLength,
                                  leftRulerStartPoint.y()+rightNumberTickStep*i);
        // --------画数字(旋转了-90°)-----
        QString numberString = QString::number(rightNumberTick*i);   // 把数字刻度转换成字符串
        QSize numberStringSize = fontMetrics().size(0, numberString);   // 数字刻度数字的Size，下面绘制定坐标用
        painter->translate(numberTickEndPoint.x(), numberTickEndPoint.y());     //变换旋转中心到文字坐标，这一步重要
        painter->rotate(90);   // 坐标系旋转90°
        painter->drawText(-numberStringSize.width()/2, -2, numberString);
        painter->resetMatrix();     // 很重要：文字绘制完毕后把坐标系恢复，要不然后面画图是乱的！！！
        // 画点数Label
        painter->translate(width()-1, rulerSpan + bufferPixmap.height()/2);
        painter->rotate(90);
        painter->drawText(-fontMetrics().width("时延(ns)")/2, fontMetrics().size(0, "时延").height()+2,
                          tr("时延(ns)"));
        painter->resetMatrix();
        // 画最长的有数字的刻度
        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
    }
    // 画中等长度的刻度
    for(int i = 0; i < (double)bufferPixmap.height()/rightNumberTickStep*2; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(rightRulerStartPoint.x(), rightRulerStartPoint.y()+rightNumberTickStep*i/2);
        QPoint numberTickEndPoint(rightRulerStartPoint.x()+rulerScaleLength*0.8,
                                  rightRulerStartPoint.y()+rightNumberTickStep*i/2);
        // 画中等长度的刻度
        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
    }
    // 画最小长度的刻度
    for(int i = 0; i < (double)bufferPixmap.height()/rightNumberTickStep*10; i++) {
        // 中等长度刻度的起点和终点
        QPoint numberTickStartPoint(rightRulerStartPoint.x(), rightRulerStartPoint.y()+rightNumberTickStep*i/10);
        QPoint numberTickEndPoint(rightRulerStartPoint.x()+rulerScaleLength*0.4,
                                  rightRulerStartPoint.y()+rightNumberTickStep*i/10);
        // 画最短的刻度
        painter->drawLine(numberTickStartPoint, numberTickEndPoint);
    }


    // 在顶部画文件名称
    QFont titleFont;
    titleFont.setPointSize(11);
    painter->setFont(titleFont);
    painter->drawText(width()/2 - fontMetrics().width(gprFileName)*0.5,
                      rulerSpan/2 + 6, gprFileName);

}

/**
 * @brief 显示/隐藏标尺
 * @param isGridLineOn
 */
void Bscan_Viewer::showRuler(bool isRulerOn)
{
    this->isRulerVisible = isRulerOn;
    update();
}



