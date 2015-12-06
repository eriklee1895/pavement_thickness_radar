#ifndef M_MDSP_H
#define M_MDSP_H
#define MDSP_FFTW__
//
#ifndef PI
#define PI 3.141592653589793
#endif
#include "../complex/mcomplex.h"
/*
 * FFT变换时的补边方式
 */
#define            EXPANSION_COS                    1
#define            EXPANSION_ZERO                   2
/*
 * 生成波波器的習积因子
 */
#define            MDSP_CREATEFILTER_NONE           0x000
#define            MDSP_CREATEFILTER_COS            0x001
#define            MDSP_CREATEFILTER_TRIANGLE       0x002
/*
 * 卷积的延拓方式
 */
#define            CONV_ZERO                        0x001
#define            CONV_CYCLE                       0x002
#define            CONV_CONST                       0x003
#define            CONV_SYMMETRY                    0x004
#define            CONV_HALF_SYMMETRY               0x005//滤波器为偶数个时用
namespace mfft
{
	/*
	 * 指针都有2^nFr个内存空间
	 */
	bool fft(const      mcomplex *pTD,mcomplex *pFD,int nFr);
	bool ifft(mcomplex  *pTD,mcomplex *pFD,int nFr);
	/*
	 * 将s扩展为newsize并补cos边
	 */
	/*
	 * 当oldsize=newsize时，s不会重新申请内存
	 */
	int expansion(float* &s,int oldsize,int newsize=0,int nFlag=EXPANSION_COS);
	/*
	 * pFD必须为2^n大小的内存空间,ptd与nsize不一定[自动扩展]
	 * 如果pFD=0,则pFD会在函数中申请相应的内存,否则必须保证pFD有足够(2^n)的内存空间
	 */
	bool fft(const float* ptd,mcomplex* &pFD,int nsize);
	/*
	 * 都满足2^n
	 * 如果pTD=0,则pTD会在函数中申请相应的内存
	 */
	bool ifft(float * &pTD,mcomplex* pFD,int nsize);

};
/*
 * 函数中申请内存前会释放原来的内存空间
 */
namespace mdsp
{
	/*
	 * 希尔伯特变换
	 * x既输入又为输出
	 * n必须为2的方次
	 */
	void n2_hilbert(int  n,float x[]);
	void hilbert(int     n,float *x);
	/*
	 * 希尔伯特变换
	 */
	bool hilbert(const float* sIn,float*& pout,const int sleng);
	/*
	 * [计算習积]
	 * s:信号,h:滤波器,g输出信号,m：s的长度,n:h的长度,s为信号，h为波波器
	 * g:函数中重新申请内存,当g!=0时会使用delete[] g;
	 */
	bool conv(const float* s,const float * h,float*& g,int m,int n,bool keeplen=true);
	/*
	 * 延拓習积
	 */
	bool econv(const float* s,const float * h,float*& g,int m,int n,bool keeplen=false,int type=CONV_SYMMETRY);
	/*
	 * [保持信号的长度] 信号保留的为中间部分
	 * 将信号的长度由oldleng变为nleng，s不会重新申请内存
	 */
	bool wkeep(float* s,const int nleng,const int oldleng);
	/*
	 * [对应点相乘]
	 * 将信号s与偏移为offset的g的对应点相乘
	 * m为s的长度，n为g的长度,当n=0时表明n=m
	 * s为输入也为输出 
	 */
	bool dotMulti(float     *s,float *g,int m,int n=0,int offset=0);
	bool dotMulti(mcomplex  *s,float *g,int m,int n=0,int offset=0);
	/*
	 * [生成一个频域的滤波器]
	 * argument:滤波器参数，arg[0]为数据组数,arg[3*n+1],arg[3*n+2],为频域的下上限，arg[3*n+3]为权数
	 * filter:在函数中申请内存的滤波器
	 * nleng:滤波器长度
	 * nedgeLen:为皱积因子(通过習积来平滑边缘)的长度(2*delta),或为过渡区域长度的两倍
	 * bauto:是否平移等自动处理[考虑到fft实信号变换的对称性]
	 * type:边缘平滑类型,默认为cos習积平滑
	 */
	bool createFilter(const float* argument,float* &filter,const int nleng,int nedgeLen=-1,const bool bauto=true,const int type=MDSP_CREATEFILTER_COS
			);
	/*
	 * 求复数数组的模
	 */
	void abs(mcomplex* sig,float*& pout,int nleng);
	/*
	 * 将一个数扩展为2^n，如num=7返回8,9->16
	 */
	int to2n(int num);
	/*
	 * nFr为返回参数之一
	 */
	int to2n(int num,int &nFr);
	/*
	 * 求相关
	 * r(x,y)
	 */
	bool r(float* px,float* py,float*& pout,int xlen,int ylen=-1);
};
#endif
