#ifndef DSPTOOLS_H
#define DSPTOOLS_H

#include <iostream>
#include <math.h>
#include <QVector>
#include <vector.h>

using namespace splab;

namespace dspTools {
//---------------------600MHz-2GHz BPF FIR-----------------------------------
//// 使用长度为65的Hamming窗函数法生成的滤波器
//int bpf_length = 65;    // 滤波器长度
//double bpf_coeff[65] = {    // 滤波器系数
//  -0.0006356060389334,-0.0004984463938134,-0.000326249581182,-0.0001081382758523,
//  0.0001552814424625,0.0004379567460369, 0.000677455435689,0.0007702865478317,
//  0.0005759050511386,-6.949082800545e-005,-0.001329930207681,-0.003339730386179,
//  -0.006170537629441,-0.009800335196914, -0.01408981421165, -0.01877109661968,
//   -0.02345257193798,  -0.0276416808267, -0.03078508387551, -0.03232311396142,
//   -0.03175308730984, -0.02869429379135, -0.02294658993474, -0.01453465943464,
//  -0.003731227317539, 0.008945293573041,  0.02276033129049,  0.03681819109012,
//    0.05013771824321,  0.06174098284662,  0.07074552294356,  0.07644995425929,
//    0.07840327863968,  0.07644995425929,  0.07074552294356,  0.06174098284662,
//    0.05013771824321,  0.03681819109012,  0.02276033129049, 0.008945293573041,
//  -0.003731227317539, -0.01453465943464, -0.02294658993474, -0.02869429379135,
//   -0.03175308730984, -0.03232311396142, -0.03078508387551,  -0.0276416808267,
//   -0.02345257193798, -0.01877109661968, -0.01408981421165,-0.009800335196914,
//  -0.006170537629441,-0.003339730386179,-0.001329930207681,-6.949082800545e-005,
//  0.0005759050511386,0.0007702865478317, 0.000677455435689,0.0004379567460369,
//  0.0001552814424625,-0.0001081382758523,-0.000326249581182,-0.0004984463938134,
//  -0.0006356060389334
//};

// 实现Matlab中的filter函数
void filter(double y[], double b[], int lenB, double a[], int lenA,
            const double x[], int lenX, double zf[] = NULL, const double zi[] = NULL);



//--------------------运算中要用到的辅助函数---------------------------------
int max_in_array(const double array[], int n);     //返回数组最大元素的下标
int max_in_QVector(const QVector<double> &v);       //返回QVector最大元素的下标
int max_in_QVector(const QVector<double> &v, int n);       //返回QVector最大元素的下标
int max_in_Vector(const Vector<double> &v);       //返回Vector最大元素的下标
int max_in_array_abs(const double array[], int n);     //返回数组绝对值最大的元素的下标
void xcorr(double *corr_value, const double *x, const double *y, int N);     //求序列x和y的互相关，要求x与y等长
void subtract_mean(double *array, int n);     //去数组直流分量
void subtract_mean(QVector<double> &array);     //去数组直流分量
double sum(const double *array, int n);     //求一维数组的和
double sum(const QVector<double> &array);     //求vector的和
double mean(const double *array, int n);     //求一维数组的均值
double mean(const QVector<double> &array);     //求vector的均值
double shift_vector_right(QVector<double> &array, int n);    // 向量右移n位，左边补零
double shift_vector_left(QVector<double> &array, int n);    // 向量左移n位，右边补零
double shift_vector(QVector<double> &array, int n);    // 向量移位，n>0右移，n<0左移
//-----------------------------------------------------------------------



//-------------------------GPR处理函数-------------------------------------
void removeDC(double **bscan_array, int trace_num, int sampling_points); // 去除直流
void reversePhase(double **bscan_array, int trace_num, int sampling_points); // 数据反相
void removeBackground(double **bscan_array, int trace_num, int sampling_points);    //去除背景
void gainFunc(double **bscan_array, int trace_num, int sampling_points);   // 增益函数
// 去除数据中的直耦波，需要对空数据作为参考。该操作不仅可以去除耦合波，还可以显现被系统固有噪声掩盖的信号
void removeCouplingPulse(double **bscan_array, double *air_data, int trace_num, int sampling_points);
//------------------------------------------------------------------------
}

#endif // DSPTOOLS_H
