#ifndef CURVEPLOTTER_H
#define CURVEPLOTTER_H

#include <QMap>
#include <QVector>
#include <QColor>
#include <QImage>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QString>



class CurvePlotter : public QWidget
{
    Q_OBJECT
public:
    explicit CurvePlotter(QWidget *parent = 0);
    //-------------显示风格设置------------------
    // 定义显示方式的枚举：横向显示/纵向
    enum DisplayOrientation {HorizontalDisplay, VerticalDisplay};
    void setDisplayStyle(DisplayOrientation style) {displayOrientation = style;}
    DisplayOrientation getDisplayStyle() {return displayOrientation;}
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    //-------------坐标轴参数设置------------------
    void adjustAxisX(double min, double max);
    void adjustAxisY(double min, double max);
    void setAxisX(double min, double max) {minX = min; maxX = max;}
    void setAxisY(double min, double max) {minY = min; maxY = max;}
    QString getAxisInfo() const {
        return QString(tr("info: ***minX = %1, maxX = %2, minY = %3, maxY = %4")
                       .arg(minX)
                       .arg(maxX)
                       .arg(minY)
                       .arg(maxY));
    }

    // -------------设置网格参数--------------
    void setGridColor(const QColor &color);
    void setGridWidth(int penWidth);
    void setGridPen(const QPen &pen) { gridPen = pen; update(); }
    void setGridNum(int num) { gridNum = num; update(); }

    // ---------------设置曲线参数------------------
    void setCurveColor(const QColor &color) { curveColor = color; update();}
    void setCurveWidth(int penWidth) {curveWidth = penWidth; update();}
    void setCurvePen(const QPen &pen) {curvePen = pen; update();}
    // 载入待画的曲线数据，默认使用自动计算坐标轴幅度
    void setCurveData(int id, const QVector<QPointF> &data, bool isAutoScale = true);
    void setCurveData(int id, const QVector<double> &data, bool isAutoScale = true);
    void clearCurve(int id);    //清除屏幕上的曲线数据

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
signals:
    
public slots:
    // 清除屏幕上的所有曲线数据
    void clearAllCurves();
    // 设置是否开启抗锯齿
    void setAntialiased(bool antialiased);
    // 刷新定位光标
    void refreshCursor(QPoint pos) {mousePos = pos; update();}
    // 设置是否显示网格
    void showGrid(bool isGridOn);
    // 是否显示定位光标(随鼠标移动的黑线)
    void showLocationLine(bool isLocationLineOn);
    // 刷新鼠标位置
    void refershMousePos(QPoint new_pos);

private:
    //----------------------显示风格属性-----------------------
    DisplayOrientation    displayOrientation;              // 显示方向：横向或纵向显示波形
    bool            isAntialiased;             // 是否开启抗锯齿
    bool            isGridOn;                  // 是否显示网格线
    bool            isLocationLineOn;          // 是否显示定位光标(随鼠标移动的黑线)
    //------------------------网格属性-------------------------
    QColor          gridColor;                 // 网格颜色
    int             gridWidth;                 // 设置网格的线宽
    QPen            gridPen;                   // 网格画笔，TODO
    //------------------------曲线属性-------------------------
    QColor          curveColor;                // 曲线颜色
    int             curveWidth;                // 设置曲线的线宽
    QPen            curvePen;                  // 曲线画笔，TODO
    //------------------------网格参数-------------------------
    int             gridSpace;                 // 网格间距
    int             gridNum;                   // 网格数目(横向)
    //------------------------坐标轴属性-------------------------
    //坐标轴属性，注意：由于坐标系统从(0,0)开始，numXTicks和numYTicks属性值要比实际-1
    double          minX;
    double          maxX;
    int             numXTicks;
    double          minY;
    double          maxY;
    int             numYTicks;
    double spanX() const { return maxX - minX; }    // X轴的范围
    double spanY() const { return maxY - minY; }    // Y轴的范围

    //-----------------------其他参数数据成员-----------------------------
    int mousePosInfo;       // 返回鼠标位置所对应的BSCAN位置，这个功能暂时没搞定
    QPoint mousePos;        // 同上
    QMap<int, QVector<QPointF> > curveMap;  // 保存要绘制的曲线(曲线Id，曲线数据)

    //------------------------private方法-------------------------
    void drawGrid(QPainter *painter);               // 绘制网格
    void drawCurves(QPainter *painter);             // 绘制曲线

};



#endif // CURVEPLOTTER_H
