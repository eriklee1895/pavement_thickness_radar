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
#include "thicknessestimator.h"
using std::vector;

class LayerDetector
{
    friend class ThicknessEstimator;
public:
    LayerDetector();    //构造函数
    virtual ~LayerDetector();

    //----------------------- LayerDetector类用户接口------------------------------------
    // 道相关追踪算法函数(一次检测全部数据)
    void trace_corr_detector(double **input_data_matrix, int trace_num, int first_search_point, int start_trace_num, int ref_window_length, int search_window_length, int *layer_position_array);

    //道相关追踪算法函数(检测第一道)
    int trace_corr_detector_first_trace(double *first_trace, int first_search_point, int ref_window_length, int search_window_length);
    //道相关追踪算法函数(检测除第一道以外的剩余道)
    int trace_corr_detector_remainder_trace(double *data_trace, int ref_window_length, int search_window_length);
    //道相关追踪算法函数(一次检测检测一道数据)
    int trace_corr_detector_by_single(double *data_trace, int first_search_point , int ref_window_length = 60, int search_window_length = 80);

    //------------------------------尚未实现的接口函数---------------------------------------
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
    vector<int> layer_position;
    vector<float> dielectric_value;    //记录每一个介质层介电常数的数组
    vector<float> thickness_value;    //记录每一个介质层厚度的数组，考虑用比数组好的数据结构

    // isFirstTrace变量用于在逐道检测中判断当前处理的是第一道还是剩余道，因为第一道与剩余道处理不太一样
    bool isFirstTrace;

    //道相关检测用到的变量
    int current_search_center; //层位点在窗口中的下标
    int layer_position_offset;  //本道的层位位置相对于前一道层位位置偏移量
    double *ref_signal;     // 用于道By道检测
    
    
};

#endif // _LAYER_DETECTOR_H
