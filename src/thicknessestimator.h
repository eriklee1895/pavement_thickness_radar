#ifndef THICKNESSESTIMATOR_H
#define THICKNESSESTIMATOR_H

/*
 * 这个类用来计算层的厚度
 */

#include <QVector>
#include <QMap>


class ThicknessEstimator
{
public:
    ThicknessEstimator();
    void addLayer(int index, const QVector<double>& newLayer);     // 添加一层
    void removeLayer(int index);    // 移除一层

    void EstimateDielectrics();     // 估计所有层的介电常数
    void EstimateLayerThickness();  // 估计所有层的厚度
private:
    QMap<int, QVector<double> > layer_positions;  // 保存所有层信息
    QMap<int, QVector<double> > dielectric_values;  // 记录每一个介质层介电常数的数组
    QMap<int, QVector<double> > layer_thickness_value;    //记录每一个介质层厚度的数组，考虑用比数组好的数据结构

    double fs;                  // 采样频率
    int sampling_num;           // 采样点数
    int trace_num;              // 采样道数
};

#endif // THICKNESSESTIMATOR_H
