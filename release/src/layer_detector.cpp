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
** 文件名： layer_detector.cpp
** 功能：层位检测的道相关方法和门限检测法的实现
**
****************************************************************************/

#include <QtGui>
#include "math.h"
#include "layer_detector.h"
#include "gpr_fileheader.h"
#include "parameterdialog.h"


LayerDetector::LayerDetector()
{

}

LayerDetector::~LayerDetector()
{

}


//-------------------------------------------------------------------------------------------------
//函数名称：TraceCorr_layer_detector()
//函数说明：相关法检测层位 - 直接用两个不等长序列相关
//参数说明：
//       input_data_matrix[col][row] - 待处理的数据矩阵
//       trace_num - 采样道数
//       first_search_point - 起始道指定的层位大致位置点
//       start_trace_num - 起始检测道
//       ref_window_length - 参考窗大小
//       search_window_length - 搜索窗大小
//输出：layer_position_array[] - 指定的存放层位点下标的数组
//-------------------------------------------------------------------------------------------------
void LayerDetector::trace_corr_detector(double **input_data_matrix, int trace_num, int first_search_point, int start_trace_num, int ref_window_length, int search_window_length, int *layer_position_array)
{
    //------------------------------变量声明-------------------------------------
    double *ref_signal = new double[ref_window_length]; //相关模板信号
    double *test_signal = new double[search_window_length];     //截取的搜索窗内信号
    double *ref_signal_padded_zero = new double[search_window_length];  //补零后的相关模板信号
    int current_search_center = first_search_point;     //起始道初始搜索中心点
    int current_layer_window_index; //层位点在窗口中的下标
    int max_corr_index; //互相关最大值的下标
    int layer_position_offset;  //本道的层位位置相对于前一道层位位置偏移量
    int trace_flag = 0;     //记录
    //-------------------------------------------------------------------------

    QProgressDialog progressingDialog;
    progressingDialog.setWindowTitle(QObject::tr("层位追踪"));
    progressingDialog.setLabelText(QObject::tr("层位追踪中，请稍等..."));
    progressingDialog.setRange(0, trace_num);
    progressingDialog.setModal(true);

    qDebug() << "开始道相关搜索...";

    //数组初始化
    memset(layer_position_array, 0, sizeof(int)*trace_num);
    memset(ref_signal, 0, sizeof(double)*ref_window_length);
    memset(test_signal, 0, sizeof(double)*search_window_length);

    //截取第一道的相关模板信号
    memcpy(ref_signal, &(input_data_matrix[start_trace_num][current_search_center - ref_window_length/2 - 1]), sizeof(double)*ref_window_length);

    //在模板窗内搜索极大值点下标，即为第1层层位在窗口内索引
    current_layer_window_index = max_abs(ref_signal, ref_window_length);

    //根据索引值确定第1层层位下标
    layer_position_array[start_trace_num] = current_search_center - ref_window_length/2 + current_layer_window_index;
    current_search_center = layer_position_array[start_trace_num];  //重新指定搜索中心点
    //重新确定相关模板
    memcpy(ref_signal, &(input_data_matrix[start_trace_num][current_search_center - ref_window_length/2]), sizeof(double)*ref_window_length);

    //逐道搜索
    for (int trace_iterator = start_trace_num+1; trace_iterator < trace_num; trace_iterator++){
        //更新进度条
        progressingDialog.setValue(trace_iterator);
        //截取搜索窗内的信号,搜索窗中心为前一道层位置
        memcpy(test_signal, &(input_data_matrix[trace_iterator][current_search_center - search_window_length/2]), sizeof(double)*search_window_length);
        //为了求互相关，需要先将ref_signal序列补零，使其与test_signal等长
        memset(ref_signal_padded_zero, 0, sizeof(double)*search_window_length);
        memcpy(ref_signal_padded_zero, ref_signal, sizeof(double)*ref_window_length);
        //用前一道层位窗口内数据与本道窗口内的信号求互相关
        int corr_length = search_window_length*2 - 1;     //相关长度为2N-1
        double *corr_value = new double[corr_length];    //互相关值
        xcorr(corr_value, ref_signal_padded_zero, test_signal, search_window_length);
        //找出互相关最大值的下标，即为本道的层位位置相对于前一道层位位置偏移量
        max_corr_index = max(corr_value, corr_length);
        layer_position_offset = -search_window_length + 1 + max_corr_index;
        //本层的层位位置为上一层层位位置+偏移量
        layer_position_array[trace_iterator] = current_search_center - search_window_length/2 + layer_position_offset + ref_window_length/2;
        //确定下一道的搜索中心
        current_search_center = layer_position_array[trace_iterator];

        //每隔500道重新定义一个相关模板
//        if (trace_flag >= 500) {
//            trace_flag = 0;
//            // 截取当前道层位为中心点窗口内的信号用于下一道互相关模板
//            memcpy(ref_signal, &(input_data_matrix[trace_iterator][current_search_center - ref_window_length/2 - 1]), sizeof(double)*ref_window_length);
//        }
//        trace_flag++;
    }
    QMessageBox::information(NULL, "层位追踪", "道相关法层位追踪完毕！", QMessageBox::Ok);
}





//-------------------------------------------------------------------------------------------------
//trace_corr_detector_by_single()
//函数说明：道相关追踪算法函数(一次读一道数据来检测)
//参数说明：
//       data_trace[col] - 输入的一道数据
//       first_search_point - 起始道指定的层位大致位置点
//       ref_window_length - 参考窗大小
//       search_window_length - 搜索窗大小
//-------------------------------------------------------------------------------------------------
void trace_corr_detector_by_single(double *data_trace, int first_search_point , int ref_window_length, int search_window_length, vector<int> &layer_position_array)
{
    static bool isFirstTrace = true;
    if (isFirstTrace) {
        trace_corr_detector_first_trace(data_trace, first_search_point, ref_window_length, search_window_length);
    }
    else {
        trace_corr_detector_remainder_trace(data_trace, ref_window_length, search_window_length);
    }

}




//-------------------------------------------------------------------------------------------------
//trace_corr_detector_first_trace()
//函数说明：道相关追踪算法函数(检测第一道)
//参数说明：
//       first_trace[col] - 输入的第一道数据
//       first_search_point - 起始道指定的层位大致位置点
//       ref_window_length - 参考窗大小
//       search_window_length - 搜索窗大小
//-------------------------------------------------------------------------------------------------
void trace_corr_detector_first_trace(double *first_trace, int first_search_point, int ref_window_length, int search_window_length)
{
    //------------------------------变量声明-------------------------------------
    double *ref_signal = new double[ref_window_length]; //相关模板信号
    double *test_signal = new double[search_window_length];     //截取的搜索窗内信号
    double *ref_signal_padded_zero = new double[search_window_length];  //补零后的相关模板信号
    int current_search_center = first_search_point;     //起始道初始搜索中心点
    int current_layer_window_index; //层位点在窗口中的下标
    int max_corr_index; //互相关最大值的下标
    int layer_position_offset;  //本道的层位位置相对于前一道层位位置偏移量
    int trace_flag = 0;     //记录
    //-------------------------------------------------------------------------


    //数组初始化
    memset(layer_position_array, 0, sizeof(int)*trace_num);
    memset(ref_signal, 0, sizeof(double)*ref_window_length);
    memset(test_signal, 0, sizeof(double)*search_window_length);

    //截取第一道的相关模板信号
    memcpy(ref_signal, &(input_data_matrix[start_trace_num][current_search_center - ref_window_length/2 - 1]), sizeof(double)*ref_window_length);

    //在模板窗内搜索极大值点下标，即为第1层层位在窗口内索引
    current_layer_window_index = max_abs(ref_signal, ref_window_length);

    //根据索引值确定第1层层位下标
    layer_position.push_back(current_search_center - ref_window_length/2 + current_layer_window_index);
    search_center_point = layer_position[1];  //重新指定搜索中心点
}



//-------------------------------------------------------------------------------------------------
//trace_corr_detector_remainder_trace()
//函数说明：道相关追踪算法函数(检测除第一道以外的剩余道)
//参数说明：
//       trace[col] - 从第二道开始输入的单道数据
//       trace_num - 采样道数
//       first_search_point - 起始道指定的层位大致位置点
//       start_trace_num - 起始检测道
//       ref_window_length - 参考窗大小
//       search_window_length - 搜索窗大小
//输出：layer_position_array[] - 指定的存放层位点下标的数组
//-------------------------------------------------------------------------------------------------
void trace_corr_detector_remainder_trace(double *trace, int ref_window_length, int search_window_length)
{
    //------------------------------变量声明-------------------------------------
    double *ref_signal = new double[ref_window_length]; //相关模板信号
    double *test_signal = new double[search_window_length];     //截取的搜索窗内信号
    double *ref_signal_padded_zero = new double[search_window_length];  //补零后的相关模板信号
    int current_search_center = first_search_point;     //起始道初始搜索中心点
    int current_layer_window_index; //层位点在窗口中的下标
    int max_corr_index; //互相关最大值的下标
    int layer_position_offset;  //本道的层位位置相对于前一道层位位置偏移量
    int trace_flag = 0;     //记录
    //-------------------------------------------------------------------------



    //重新确定相关模板
    memcpy(ref_signal, &(input_data_matrix[start_trace_num][current_search_center - ref_window_length/2]), sizeof(double)*ref_window_length);

    //逐道搜索
    for (int trace_iterator = ; trace_iterator < trace_num; trace_iterator++){
        //更新进度条
        progressingDialog.setValue(trace_iterator);
        //截取搜索窗内的信号,搜索窗中心为前一道层位置
        memcpy(test_signal, &(input_data_matrix[trace_iterator][current_search_center - search_window_length/2]), sizeof(double)*search_window_length);
        //为了求互相关，需要先将ref_signal序列补零，使其与test_signal等长
        memset(ref_signal_padded_zero, 0, sizeof(double)*search_window_length);
        memcpy(ref_signal_padded_zero, ref_signal, sizeof(double)*ref_window_length);
        //用前一道层位窗口内数据与本道窗口内的信号求互相关
        int corr_length = search_window_length*2 - 1;     //相关长度为2N-1
        double *corr_value = new double[corr_length];    //互相关值
        xcorr(corr_value, ref_signal_padded_zero, test_signal, search_window_length);
        //找出互相关最大值的下标，即为本道的层位位置相对于前一道层位位置偏移量
        max_corr_index = max(corr_value, corr_length);
        layer_position_offset = -search_window_length + 1 + max_corr_index;
        //本层的层位位置为上一层层位位置+偏移量
        layer_position_array[trace_iterator] = current_search_center - search_window_length/2 + layer_position_offset + ref_window_length/2;
        //确定下一道的搜索中心
        current_search_center = layer_position_array[trace_iterator];

        //每隔500道重新定义一个相关模板
//        if (trace_flag >= 500) {
//            trace_flag = 0;
//            // 截取当前道层位为中心点窗口内的信号用于下一道互相关模板
//            memcpy(ref_signal, &(input_data_matrix[trace_iterator][current_search_center - ref_window_length/2 - 1]), sizeof(double)*ref_window_length);
//        }
//        trace_flag++;
    }
}








//-------------------------------------------------------------------------------------------------
//函数名称：max()
//函数说明：返回数组中最大元素的下标
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：数组中最大元素的下标
//-------------------------------------------------------------------------------------------------
int LayerDetector::max(const double array[],int n)
{
    int t = 0;
    double maxx;
    maxx = array[0];
    for(int i = 0;i < n; i++)
        if (array[i] > maxx){
            t = i;
            maxx = array[i];
        }
    return t;
}



//-------------------------------------------------------------------------------------------------
//函数名称: max_abs()
//函数说明：返回数组绝对值最大的元素的下标
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：数组中绝对值最大元素的下标
//-------------------------------------------------------------------------------------------------
int LayerDetector::max_abs(const double array[], int n)
{
    int t = 0;
    double maxValueOfAbs;
    maxValueOfAbs = fabs(array[0]);
    for(int i = 0;i < n; i++){
        if (fabs(array[i]) > maxValueOfAbs){
            t = i;
            maxValueOfAbs = fabs(array[i]);
        }
    }
    return t;
}




//-------------------------------------------------------------------------------------------------
//函数名称：xcorr()
//函数说明：求两个序列的互相关有偏估计, 计算的是Rxy，即y在x中出现的位置，
//        注意：x,y的位置Matlab中xcorr函数相反。
//        下标为[-N+1,N-1]
//参数说明：
//       x[],y[] - 待求序列
//       N - 序列长度
//输出：r[] - 相关结果
//-------------------------------------------------------------------------------------------------
void LayerDetector::xcorr(double *corr_value, const double *x, const double *y, int N)
{
    double sxy;
    int    delay,i,j;

    for(delay = -N + 1; delay < N; delay++){
        //Calculate the numerator
        sxy = 0;
        for(i=0; i<N; i++){
            j = i + delay;
            if((j < 0) || (j >= N))  //The series are no wrapped,so the value is ignored
                continue;
            else
                sxy += (x[i] * y[j]);
        }

        //Calculate the correlation series at "delay"
        corr_value[delay + N - 1] = sxy;
    }

}

//-------------------------------------------------------------------------------------------------
//函数名称: subtract_mean()
//函数说明：去数组直流分量
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：直接对原数组操作
//-------------------------------------------------------------------------------------------------
void LayerDetector::subtract_mean(double *array, int n)
{
    double mean_value = mean(array, n);
    for (int i = 0; i < n; i++){
        array[i] = array[i] - mean_value;
    }
}


//-------------------------------------------------------------------------------------------------
//函数名称: sum()
//函数说明：数组求和
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：数组的和
//-------------------------------------------------------------------------------------------------
double LayerDetector::sum(const double *array, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++){
        s += array[i];
    }
    return s;
}


//-------------------------------------------------------------------------------------------------
//函数名称: mean()
//函数说明：求一维数组的均值
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：数组均值
//-------------------------------------------------------------------------------------------------
double LayerDetector::mean(const double *array, int n)
{
    return sum(array, n)/n;
}

















