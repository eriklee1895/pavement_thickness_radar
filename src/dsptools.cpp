#define BOUNDS_CHECK

#include "dsptools.h"
#include <stdio.h>
#include <memory.h>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDebug>
#include <window.h>


using namespace splab;

namespace dspTools {


/*
$	Reimplementation of my MATLAB function : [y,zf] = filter(b,a,X,zi)

	filters the data in vector X with the filter described by numerator coefficient vector b and denominator coefficient vector a.
	a(1) might be 1, anyway a(1) cannot be 0.

	if zi isn't NULL and zf isn't NULL, then the function accepts initial conditions, zi, and returns the final conditions, zf, of the filter delays

$	Inputs,
		a, b, x, zi,
		lenA is the length of array a, lenB is the length of array b, both can be 1.
	Outputs,
		y, zf;

$	WatchOut,
		1. The length of y[] must agree with the length of x[] !
		2. a[0] cannot be zero !
		3. Length of zf must be max(length(a),length(b))-1 or ZERO !
		4. Length of zi must be the same as zf or ZERO !
		5. Function won't allocate space for outputs y & zf !

$	Function is written by Yu XianGuo, NUDT ( Aug31, 2012 )
*/

void filter(double y[], double b[], int lenB, double a[], int lenA,
            const double x[], int lenX, double zf[], const double zi[])
{
    int i, j;

    if( a[0]==0 )
        throw   ("a[0] cannot be zero!");
	if( a[0]!=1 ){
        for( i=1; i!=lenA; i++ )
            a[i] /= a[0];
        for( i=1; i!=lenB; i++ )
            b[i] /= a[0];
        a[0] = 1;
    }

    int na = lenA - 1, nb = lenB - 1;
    int len = na>nb ? na : nb;

    bool zi_null = zi==NULL;
    if( zf!=NULL ){
        memset( zf, 0, len*sizeof(double) );
        if( zi==NULL )
        zi = new double [len] ();
    }

    bool zf_null = zf==NULL;
    if( zi!=NULL && zf==NULL )
        zf = new double [len] ();

    // 1.
    if( zf==NULL && zi==NULL ){
        y[0] = b[0] * x[0];
        for( i=1; i<lenX; i++ ){
        y[i] = 0;
        for( j=0; j<=nb; j++ ){
            if( i-j<0 )	break;
            y[i] += b[j] * x[i-j];
        }
        for( j=1; j<=na; j++ ){
            if( i-j<0 )	break;
            y[i] -= a[j] * x[i-j];
        }
        }

    }
    // 2.
    else{
        double *A = new double [len+1] ();
        memcpy( A, a, lenA*sizeof(double) );
        double *B = new double [len+1] ();
        memcpy( B, b, lenB*sizeof(double) );
        double *zf_last = new double [len];
        memcpy( zf_last, zi, len*sizeof(double) );

        for( i=0; i!=lenX; i++ ){
            y[i] = b[0] * x[i] + zf_last[0];
            zf[len-1] = B[len] * x[i] - A[len] * y[i];
            for( j=len-2; j>=0; j-- )
                zf[j] = B[j+1]*x[i] + zf_last[j+1] - A[j+1]*y[i];
            memcpy( zf_last, zf, len*sizeof(double) );
        }

        delete [] A;
        delete [] B;
        delete [] zf_last;
        if(zi_null) delete [] zi;
        if(zf_null) delete [] zf;
    }

    return;
}

/*	验证...................
1. MATLAB

b=[0.8147,0.9058,0.1270];
x=[0.9134,0.6324,0.0975,0.2785,0.5469,0.9575,0.9649,0.1576,0.9706,0.9572];
[y,zf] = filter(b,1,x);

2. C++

double	b[] = { 0.8147, 0.9058, 0.1270 };
double	a[1] = { 1 };
double	x[] = { 0.9134, 0.6324, 0.0975, 0.2785, 0.5469, 0.9575, 0.9649, 0.1576, 0.9706, 0.9572 };

double	zf[2];
double	y[10];
filter( y, zf, b, 3, a, 1, x, 10, NULL );

for( int i=0; i!=10; i++ )
printf("%f ",y[i]);
printf("\n");
for( int i=0; i!=2; i++ )
printf("%f ",zf[i]);
*/

/* 本程序对应的MATLAB代码......

function   [y,zf] = yuFilter(b,a,x,zi)
% This function reimplement the MATLAB inner function filter().
% All the parameters accords with the MATLAB inner function filter().
%
% Function is written by Yu XianGuo, NUDT ( August 31, 2012 )

if a(1)==0
return;
end
if a(1)~=1
for i=2:length(a)
a(i) = a(i)/a(1);
end
for i=2:length(b)
b(i) = b(i)/a(1);
end
a(1) = 1;
end
na = length(a)-1;
nb = length(b) - 1;
len = max(na,nb); % length of zf

if nargout==1
zf = [];
else
if nargin<4
zi = zeros(1,len);
elseif length(zi)~=len
return;
end
zf = zeros(size(zi));
zf_last = zi;
end

y = zeros(size(x));

if isempty(zf)

y(1) = b(1)*x(1);
for i=2:length(x)
for j=1:nb+1
if(i+1-j<1) break; end
y(i) = y(i) + b(j)*x(i+1-j);
end
for j=2:na+1
if(i+1-j<1) break; end
y(i) = y(i) - a(j)*x(i+1-j);
end
end

end

if ~isempty(zf)

a = [a zeros(1,nb-na)];
len = length(zf);
for i=1:length(x)
y(i) = b(1)*x(i) + zf_last(1);
zf(len) = b(len+1)*x(i) - a(len+1)*y(i);
for j=len-1:-1:1
zf(j) = b(j+1)*x(i) + zf_last(j+1) - a(j+1)*y(i);
end
zf_last = zf;
end

end


*/



//-------------------------------------------------------------------------------------------------
//函数名称：max_in_array()
//函数说明：返回数组中最大元素的下标
//参数说明：
//       arry[] - 待处理的数据数组
//       n - 数组大小
//输出：数组中最大元素的下标
//-------------------------------------------------------------------------------------------------
int max_in_array(const double array[],int n)
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
//函数名称：max_in_QVector()
//函数说明：返回QVector最大元素的下标
//参数说明：
//       v - 待处理的数据vector
//输出：数组中最大元素的下标
//-------------------------------------------------------------------------------------------------
int max_in_QVector(const QVector<double> &v)
{
    int index_max = 0;
    double maxVal = v[0];
    for (int i = 0; i < v.count(); ++i)
        if (v[i] > maxVal) {
            index_max = i;
            maxVal = v[i];
        }
    return index_max;
}


//-------------------------------------------------------------------------------------------------
//函数名称：max_in_QVector()
//函数说明：返回QVector最大元素的下标
//参数说明：
//       v - 待处理的数据vector
//输出：数组中最大元素的下标
//-------------------------------------------------------------------------------------------------
int max_in_QVector(const QVector<double> &v, int n)
{
    int index_max = 0;
    double maxVal = v[0];
    for (int i = 0; i < n; ++i)
        if (v[i] > maxVal) {
            index_max = i;
            maxVal = v[i];
        }
    return index_max;
}



//-------------------------------------------------------------------------------------------------
//函数名称：max_in_array()
//函数说明：返回Vector最大元素的下标
//参数说明：
//       v - 待处理的数据vector
//输出：数组中最大元素的下标
//-------------------------------------------------------------------------------------------------
int max_in_Vector(const Vector<double> &v)
{
    int index_max = 0;
    double maxVal = v[0];
    for (int i = 0; i < v.size(); ++i)
        if (v[i] > maxVal) {
            index_max = i;
            maxVal = v[i];
        }
    return index_max;
}


//-------------------------------------------------------------------------------------------------
//函数名称: max_in_array_abs()
//函数说明：返回数组绝对值最大的元素的下标
//参数说明：
//       arry[] - 待处理的数据矩阵
//       n - 数组大小
//输出：数组中绝对值最大元素的下标
//-------------------------------------------------------------------------------------------------
int max_in_array_abs(const double array[], int n)
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
//输出：corr_value[] - 相关结果
//-------------------------------------------------------------------------------------------------
void xcorr(double *corr_value, const double *x, const double *y, int N)
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
void subtract_mean(double *array, int n)
{
    double mean_value = mean(array, n);
    for (int i = 0; i < n; i++){
        array[i] = array[i] - mean_value;
    }
}


//-------------------------------------------------------------------------------------------------
//函数名称: subtract_mean()
//函数说明：去数组直流分量
//参数说明：
//       arry[] - 待处理的数据vector
//输出：直接对原数组操作
//-------------------------------------------------------------------------------------------------
void subtract_mean(QVector<double> &array)
{
    double n = array.count();
    double mean_value = mean(array);
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
double sum(const QVector<double> &array)
{
    double s = 0;
    int n = array.count();
    for (int i = 0; i < n; i++){
        s += array[i];
    }
    return s;
}


//-------------------------------------------------------------------------------------------------
//函数名称: sum()
//函数说明：数组求和
//参数说明：
//       arry[] - 待处理的数据vector
//输出：数组的和
//-------------------------------------------------------------------------------------------------
double sum(const double *array, int n)
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
double mean(const double *array, int n)
{
    return sum(array, n)/n;
}



//-------------------------------------------------------------------------------------------------
//函数名称: mean()
//函数说明：求一维数组的均值
//参数说明：
//       arry[] - 待处理的数据vector
//输出：数组均值
//-------------------------------------------------------------------------------------------------
double mean(const QVector<double> &array)
{
    int n = array.count();
    return sum(array)/n;
}



//-------------------------------------------------------------------------------------------------
//函数名称: shift_vector_right()
//函数说明：向量右移n位，左边补零
//参数说明：
//       arry[] - 待处理的数据vector
//       n      - 移位数
//输出：数组均值
//-------------------------------------------------------------------------------------------------
double shift_vector_right(QVector<double> &array, int n)
{
    n = abs(n);
    if (n  > 0) {
        // 右移n位
        for (int i = array.count()-1; i >= n; i--)
            array[i] = array[i - n];
        // 左端补零
        for (int i = 0; i < n; i++)
            array[i] = 0;
    }
}




//-------------------------------------------------------------------------------------------------
//函数名称: shift_vector_left()
//函数说明：向量左移n位，右边补零
//参数说明：
//       arry[] - 待处理的数据vector
//       n      - 移位数
//输出：数组均值
//-------------------------------------------------------------------------------------------------
double shift_vector_left(QVector<double> &array, int n)
{
    n = abs(n);
    if (n > 0) {
        // 左移n位
        for (int i = 0; i <= array.count()-1-n; i++)
            array[i] = array[i + n];
        // 右端补零
        for (int i = array.count()-1; i >= array.count()-n; i--)
            array[i] = 0;
    }
}




//-------------------------------------------------------------------------------------------------
//函数名称: shift_vector()
//函数说明：向量移位，n>0右移，n<0左移，多余的补零
//参数说明：
//       arry[] - 待处理的数据vector
//       n      - 移位数
//输出：数组均值
//-------------------------------------------------------------------------------------------------
double shift_vector(QVector<double> &array, int n)
{
    // 右移n位
    if (n>=0)
        shift_vector_right(array, n);
    else
        shift_vector_left(array, abs(n));
}





//-------------------------------------------------------------------------------------------------
//函数名称: removeDC()
//函数说明：去除数据中的直流分量
//参数说明：
//       arry[][] - 待处理的数据矩阵
//       trace_num - 道数
//       sampling_points - 每道点数
//输出：去除直流后的GPR数据数组
//-------------------------------------------------------------------------------------------------
void removeDC(double **bscan_array, int trace_num, int sampling_points)
{
    QProgressDialog progressingDialog;
    progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
    progressingDialog.setWindowTitle(QObject::tr("去除直流"));
    progressingDialog.setLabelText(QObject::tr("去除直流，请稍等..."));
    progressingDialog.setRange(0, trace_num);
    progressingDialog.setModal(true);

    for(int i = 0; i < trace_num; i++){
        //更新进度条
        progressingDialog.setValue(i);
        // 去除每道的直流
        subtract_mean(bscan_array[i], sampling_points);
    }
    QMessageBox::information(NULL, "去直流", "去直流完毕！", QMessageBox::Yes);
}


//-------------------------------------------------------------------------------------------------
//函数名称: reversePhase()
//函数说明：对数据做反相处理
//参数说明：
//       bscan_array[][] - 待处理的数据矩阵
//       trace_num - 道数
//       sampling_points - 每道点数
//输出：反相后的GPR数据数组
//-------------------------------------------------------------------------------------------------
void reversePhase(double **bscan_array, int trace_num, int sampling_points)
{
    QProgressDialog progressingDialog;
    progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
    progressingDialog.setWindowTitle(QObject::tr("数据反相"));
    progressingDialog.setLabelText(QObject::tr("数据反相，请稍等..."));
    progressingDialog.setRange(0, trace_num - 1);
    progressingDialog.setModal(true);

    for (int i = 0; i != trace_num; i++) {
        //更新进度条
        progressingDialog.setValue(i);

        for (int j = 0; j != sampling_points; j++) {
            // 逐道反相
            bscan_array[i][j] = -bscan_array[i][j];     // 感觉取反就行了吧？
        }
    }
    QMessageBox::information(NULL, "数据反相", "数据反相处理完毕！", QMessageBox::Yes);
}



//-------------------------------------------------------------------------------------------------
//函数名称: removeBackground()
//函数说明：去除背景
//参数说明：
//       bscan_array[][] - 待处理的数据矩阵
//       trace_num - 道数
//       sampling_points - 每道点数
//       startTrace - 起始道
//输出：去除背景后的GPR数据数组
//-------------------------------------------------------------------------------------------------
void removeBackground(double **bscan_array, int trace_num, int sampling_points)
{
    // 目前该算法还存在问题，因为10000*30000太大，应该改进计算方法才行！
    QProgressDialog progressingDialog;
    progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
    progressingDialog.setWindowTitle(QObject::tr("去除背景"));
    progressingDialog.setLabelText(QObject::tr("去除背景，请稍等..."));
    progressingDialog.setRange(0, sampling_points - 1);
    progressingDialog.setModal(true);

    double sum_horizontal = 0.0;
    double average_horizontal = 0.0;
    for (int sampling_index = 0; sampling_index != sampling_points; sampling_index++) {
        //更新进度条
        progressingDialog.setValue(sampling_index);
        //  求水平均值
        for (int trace_index = 0; trace_index != trace_num; trace_index++)
            sum_horizontal += bscan_array[trace_index][sampling_index];
        average_horizontal = sum_horizontal / trace_num;    // 水平方向均值
        qDebug() << "average_horizontal = " << average_horizontal;
        // 水平方向减去均值
        for (int trace_index = 0; trace_index != trace_num; trace_index++)
            bscan_array[trace_index][sampling_index] -= average_horizontal;     // 水平方向减均值
    }
    QMessageBox::information(NULL, "去除背景", "去除背景完毕！", QMessageBox::Yes);
}

/**
 * @brief 增益函数，目前只是一个简单雏形，在350~680点简单的Hamming窗平滑增益6倍
 *        上升段与下降段取得是128点Hamming窗前后半段。中间增益为6
 * @param bscan_array
 * @param trace_num
 * @param sampling_points
 */
void gainFunc(double **bscan_array, int trace_num, int sampling_points)
{
    // 生成增益函数
    /* 增益函数形状：
     *         .......
     *       .        .
     *      .          .
     *......            ..........
     */
    qDebug() << "----------Enter the gain func!------------";
    qDebug() << "trace_num = " << trace_num << ", sampling_points = " << sampling_points;
    Vector<double> hamming_win(hamming(128, 7.0));  // 中间为Hamming窗
    Vector<double> gain_func(1026, 1);  // 增益函数
    // 上升段
    for(int i = 300, j = 0; i < 364, j < 64; i++, j++)
        gain_func[i] = gain_func[i] + hamming_win[j];
    // 中间段
    for(int i = 364; i < 650; i++)
        gain_func[i] = gain_func[i] + hamming_win[64];
    // 下降段
    for(int i = 650, j = 64; i < 650+64, j < 128; i++, j++)
        gain_func[i] = gain_func[i] + hamming_win[j];


    for (int i = 0; i < 128; i++)
        qDebug() << i << ". " << hamming_win[i];

    qDebug() << "----------gain func generated!------------";
    QProgressDialog progressingDialog;
    progressingDialog.setMinimumDuration(5);   //进度条至少显示5ms
    progressingDialog.setWindowTitle(QObject::tr("数据增强"));
    progressingDialog.setLabelText(QObject::tr("数据增强，请稍等..."));
    progressingDialog.setRange(0, trace_num - 1);
    progressingDialog.setModal(true);
    qDebug() << "----------begin gain !------------";
    for (int i = 0; i < trace_num; i++) {
        //更新进度条
        progressingDialog.setValue(i);
        for (int j = 0; j < sampling_points; j++) {
            // 逐道反相
            bscan_array[i][j] *= gain_func[j];     // 感觉取反就行了吧？
        }
    }
    QMessageBox::information(NULL, "数据增强", "数据增强完毕！", QMessageBox::Yes);
}


/**
 * @brief 去除数据中的直耦波，需要对空数据作为参考。该操作不仅可以去除耦合波，还可以显现被系统固有噪声掩盖的信号
 *        需要注意的是，操作前需要把对空数据与实测数据前段对齐才能进行相减。
 * @param bscan_array   GPR B-SCAN数据（二维）
 * @param air_data      对空数据（一维）
 * @param trace_num     道数
 * @param sampling_points   采样点数
 */
void removeCouplingPulse(double **bscan_array, double *air_data, int trace_num, int sampling_points)
{

}



}   // end of namespace dspTools



