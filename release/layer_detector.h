/****************************************************************************
**
** 探地雷达路面检测软件
** 中科院电子所十室
**
** Created by Erik Lee
** Time:2013/05
** Version: v0.1
** Platform: Window / Linux
** 开发工具: Qt 4.8.4 + Qt Creator
** Language: C++
**
** 文件名： layer_detector.h
** 功能：层位检测的道相关方法和门限检测法的实现
**
*****************************************************************************/

#ifndef _LAYER_DETECTOR_H
#define _LAYER_DETECTOR_H

#include <vector>
using std::vector;

class LayerDetector
{
public:
    LayerDetector();    //构造函数
    virtual ~LayerDetector();

    //----------------------- LayerDetector类用户接口------------------------------------
    //道相关追踪算法函数(一次检测全部数据)
    void trace_corr_detector(double **input_data_matrix, int trace_num, int first_search_point, int start_trace_num, int ref_window_length, int search_window_length, int *layer_position_array);

    //道相关追踪算法函数(一次检测检测一道数据)
    void trace_corr_detector_by_single(double *data_trace, int first_search_point, int start_trace_num, int ref_window_length, int search_window_length, vector<int> &layer_position_array);
    //道相关追踪算法函数(检测第一道)
    void trace_corr_detector_first_trace(double *first_trace, int first_search_point, int ref_window_length, int search_window_length);
    //道相关追踪算法函数(检测除第一道以外的剩余道)
    void trace_corr_detector_remainder_trace(double *trace, int ref_window_length, int search_window_length);

    //-----------------尚未实现的接口函数--------------------
    //道相关单道检测函数
    //门限检测算法函数
    void threshold_detector();  //TODO
    //介电常数计算函数
    void dielectric_cal();      //TODO
    //厚度计算函数
    void thicness_cal();      //TODO
    //------------------------------------------------------------------------------------


private:
    int sampling_num;           //采样点数
    int trace_num;              //采样道数
    int time_win;               //采样时窗
    float fs;                   //采样频率
    int search_center_point;    //第一道的搜索中心点
    vector<int> layer_position;
    vector<float> dielectric_value;    //记录每一个介质层介电常数的数组
    vector<float> thickness_value;    //记录每一个介质层厚度的数组，考虑用比数组好的数据结构


    //运算中要用到的辅助函数
    int max(const double array[], int n);     //返回数组最大元素的下标
    int max_abs(const double array[], int n);     //返回数组绝对值最大的元素的下标
    void xcorr(double *corr_value, const double *x, const double *y, int N);     //求序列x和y的互相关，要求x与y等长
    void subtract_mean(double *array, int n);     //去数组直流分量
    double sum(const double *array, int n);     //求一维数组的和
    double mean(const double *array, int n);     //求一维数组的均值
};

#endif // _LAYER_DETECTOR_H
