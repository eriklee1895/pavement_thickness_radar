#ifndef BSCAN_VIEWER_H
#define BSCAN_VIEWER_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QMap>

#define       COLOR_MATRIX_DATALENGTH       750
#define       COLOR_MATRIX_DATALNUM         250
#define MDATA_ZERO 0.0000001

class QPixmap;
class CurvePlotter;
class QPoint;
class QPointF;
class QPolygonF;
class QPolygon;

class Bscan_Viewer : public QWidget
{
    Q_OBJECT
public:
    explicit Bscan_Viewer(CurvePlotter *plotter, QWidget *parent = 0);
    void setData(double **data, int samplingNum, int traceNum);
    const QString& currentMousePosInfo() const { return mousePosInfoString; }
    void setLayer(int id, const QVector<QPointF> &layerPoints);
    void clearLayerTrack(int id);
    void clearAllLayerTracks();
    void setGprFileName(QString name) { gprFileName = name; }

    enum colorMode
    {
        COLOR_TYPE_GRAY      ,
        COLOR_TYPE_RWB       ,
        COLOR_TYPE_FOURCOLOR ,
        COLOR_TYPE_GRAY_INV
    };

signals:
    void mouse_pos_changed();
    void mouseClicked(QPoint startPoint);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    void showRuler(bool isRulerOn); // 显示/关闭标尺

private:

    void drawTriangle(QPoint mousePos, QPainter* painter);  // 在顶部和底部各绘制一个红色三角形
    void drawBscanProfile(QPainter *painter);        // 核心：在QPixmap上画剖面图
    void drawLayers(QPainter *painter);
    void refreshBufferPixmap();                      // 更新缓冲区
    void drawRulers(QPainter *painter);              // 绘制标尺

    float d_colorMatrix[COLOR_MATRIX_DATALENGTH];
    CurvePlotter *m_curvePlotter;
    double **bscan_data;        // 指向GPR数据
    bool isDataLoaded;          // 是否已经载入数据
    bool isRulerVisible;          // 是否显示标尺
    int m_samplingNum;          // 采样点数
    int m_traceNum;             // 道数
    double fs;                  // 采样率
    double time_window;         // 时窗
    int rulerSpan;              // 标尺的高度空间
    int rulerSpace;             // 标尺与剖面图空隙
    int rulerScaleLength;       // 标尺短刻度刻度长度
    QString mousePosInfoString;
    QString gprFileName;        // GPR文件名
    QPoint mousePos;
    QPixmap bufferPixmap;
    QMap<int, QVector<QPointF> > layerTracksMap;  // 保存要绘制的层(曲线Id，曲线数据)
};


#endif // BSCAN_VIEWER_H


