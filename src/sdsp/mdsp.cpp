#include "mdsp.h"
#include <math.h>
#include <string.h>
//
#include <iostream>
using namespace std;
/*
 * 希尔伯特变换辅助函数
 */
static void fht(int n,float x[]);
bool mfft::fft(const mcomplex* pTDin,mcomplex* pFDout,int nFr)
{
	const int  count  = 1<<nFr;
	const int  hcount = count/2;
	int        i        ,j,k;
	int        ntemp;
	int        bfsize   ,p;
	float      angle;
	//
	mcomplex  *pTD    = new mcomplex[count];
	mcomplex  *pFD    = new mcomplex[count];
	mcomplex  *W      = new mcomplex[count/2];
	mcomplex  *pTemp  = 0;
	memcpy(pTD,pTDin,sizeof(mcomplex)*count);

	for(i=0; i<hcount; i++)
	{
		angle   =   -float(i)*2.0*PI/count;
		W[i]    =   mcomplex(cos(angle),sin(angle));
	}
	//
	for(k=0; k<nFr; k++)
	{
		ntemp=1<<k;
		for(j=0; j<ntemp; j++)
		{
			bfsize=1<<(nFr-k);
			for(i=0; i<bfsize/2; i++)
			{
				p = j*bfsize;
				pFD[i+p] = pTD[i+p]+pTD[i+p+bfsize/2];
				pFD[i+p+bfsize/2] = (pTD[i+p]-pTD[i+p+bfsize/2])*W[i*(1<<k)];
			}
		}
		pTemp = pTD;
		pTD = pFD;
		pFD = pTemp;

	}
	for(j=0; j<count; j++)
	{
		p=0;
		for(i=0; i<nFr; i++)
		{
			if(j&(1<<i))
			{
				p += 1<<(nFr-i-1);
			}
		}
		pFDout[j] = pTD[p];
	}

	delete []W;
	delete []pTD;
	delete []pFD;
	return true;
}
bool mfft::ifft(mcomplex* pTD,mcomplex* pFD,int nFr)
{
	int        i     = 0;
	const int  count = 1<<nFr;
	mcomplex  *pFDin = new mcomplex[count];
	for(i = 0; i<count; i++)
	{
		pFDin[i] = mcomplex(pFD[i][MREAL],-pFD[i][MIMG]);
	}
	
	mfft::fft(pFDin,pTD,nFr);
	
	for(i=0;i<count;i++)
		pTD[i]=mcomplex(pTD[i][MREAL]/count,-pTD[i][MIMG]/count);
	return true;
}
bool mfft::fft(const float* ptd,mcomplex* &pFD,int nsize)
{
	int        nfr     = 0;
	int        i       = 0;
	const int  newsize = mdsp::to2n(nsize, nfr);
	mcomplex  *pTD     = new mcomplex[newsize];
	if(0 == pTD)
	{
		return false;
	}
	//
	if(pFD==0)
	{
		pFD=new mcomplex[newsize];
	}
	//
	for(i = 0;i < nsize; ++i)
	{
		pTD[i] = ptd[i];
	}
	for(i = 0; i < newsize; ++i)
		pTD[i] = 0;
	fft(pTD, pFD, nfr);
	if(pTD)
	{
		delete[]pTD;
		pTD = 0;
	}
	return true;
}
bool mfft::ifft(float* &ptd,mcomplex* pFD,int nsize)
{
	int       nFr = 0;
	int       i   = 0;
	mcomplex *pTD = new mcomplex[nsize];
	//
	if(0 == pTD)
	{
		return false;
	}
	mdsp::to2n(nsize,nFr);
	if(0 == ptd)
	{
		ptd=new float[nsize];
	}
	ifft(pTD, pFD, nFr);
	for(i=0; i<nsize; ++i)
	{
		ptd[i]=pTD[i][MREAL];
	}
	if(pTD)
	{
		delete[]pTD;
		pTD=0;
	}
	return true;
}
bool mdsp::econv(const float* s,const float* h,float* &g,int m,int n,bool keeplen,int type)
{
	const int sigLen = m;
	const int nleng  = m+n-1;
	int       nindex = 0;
	int       i      = 0 ,j=0;
	if(g!=NULL)
	{
		delete[]g;
		g=NULL;
	}
	g = new float[nleng];
	//
	switch(type)
	{
		case CONV_HALF_SYMMETRY:
			for(i=0;i<nleng;++i)
			{
				g[i]=0;
				for(j=0;j<n;++j)
				{
					nindex=i-j;
					//延拓方式
					//对称延拓
					if(nindex<0)
					{
						nindex=-nindex-1;
					}
					else if(nindex>=m)
					{
						nindex=2*m-nindex-1;
					}
					//
					g[i]+=h[j]*s[nindex];
				}
			}
			break;
		case CONV_SYMMETRY:
			for(i=0;i<nleng;++i)
			{
				g[i]=0;
				for(j=0;j<n;++j)
				{
					nindex=i-j;
					//延拓方式
					//对称延拓
					if(nindex<0)
					{
						nindex=-nindex;
					}
					else if(nindex>=m)
					{
						nindex=2*m-nindex-2;
					}
					//
					g[i]+=h[j]*s[nindex];
				}
			}
			break;
		case CONV_CONST:
			for(i=0;i<nleng;++i)
			{
				g[i]=0;
				for(j=0;j<n;++j)
				{
					nindex=i-j;
					//延拓方式
					//周期延拓
					if(nindex<0)
					{
						nindex=0;
					}
					else if(nindex>=m)
					{
						nindex=m-1;
					}
					//
					g[i]+=h[j]*s[nindex];
				}
			}
			break;
		case CONV_CYCLE:
			for(i=0;i<nleng;++i)
			{
				g[i]=0;
				for(j=0;j<n;++j)
				{
					nindex=i-j;
					//延拓方式
					//周期延拓
					if(nindex<0)
					{
						nindex=m+nindex;
					}
					else if(nindex>=m)
					{
						nindex=nindex-m;
					}
					//
					g[i]+=h[j]*s[nindex];
				}
			}
			break;
		case CONV_ZERO:
			for(i=0;i<nleng;++i)
			{
				g[i]=0;
				for(j=0;j<n;++j)
				{
					nindex=i-j;
					//延拓方式
					//0延拓
					//
					if(nindex>=0&&nindex<m)
						g[i]+=h[j]*s[nindex];
				}
			}
			break;
	}
	//
	if(keeplen)
	{
		wkeep(g,sigLen,nleng);
	}
	return true;
}
bool mdsp::conv(const float* ss,const float* hh,float* &g,int m,int n,bool keeplen)
{
	const int  sigLen = m;
	float     *s      = 0;
	float     *h      = 0;
	const int  nleng  = m+n-1;
	int        i      = 0,j=0;
	int        nloop  = 1;
	//确保m>n,leng(s)>leng(h)
	if(n>m)
	{
		s=(float*)hh;
		h=(float*)ss;
		m ^=n ^=m ^=n;
	}
	else
	{
		s=(float*)ss;
		h=(float*)hh;
	}

	//const int nband=(nleng+1)/2;
	if(g==NULL)
		g=new float[nleng];
	//
	for(i=0;i<n-1;++i)
	{
		g[i]=0;
		for(j=0;j<=i;++j)
		{
			g[i]+=h[j]*s[i-j];
		}
	}
	//
	for(i=n-1;i<m;++i)
	{
		g[i]=0;
		for(j=0;j<n;++j)
		{
			g[i]+=h[j]*s[i-j];
		}

	}
	//
	for(i=m;i<nleng;++i)
	{
		g[i]=0;
		for(j=nloop;j<n;++j)
		{
			g[i]+=h[j]*s[i-j];
		}
		++nloop;
	}
	//
	if(keeplen)
	{
		wkeep(g,sigLen,nleng);
	}
	return true;
}
int mfft::expansion(float* &s,int oldsize,int newsize,int nFlag)
{
	int nfr = 0;
	if(newsize == 0)
	{
		nfr=log(oldsize)/log(2);
		if(1<<nfr<oldsize)
		{
			++nfr;
			newsize=1<<nfr;
		}
		else
		{
			return nfr;
		}
	}
	else
	{
		nfr=log(newsize)/log(2);
	}
	int i          =   0;
	float *ptemp   =   new float[newsize];
	memcpy(ptemp,s,sizeof(float)*oldsize);
	//
	delete[]s;
	s=ptemp;
	//
	//补边
	switch(nFlag)
	{
		case EXPANSION_ZERO:
		for(i=oldsize;i<newsize;++i)
		{
			s[i]=0;
		}
		break;
		default:
		for(i=oldsize;i<newsize;++i)
		{
			s[i]=s[oldsize-1]*cos((i-oldsize+1)*PI/(2*(newsize-oldsize)));
		}
		break;
	}
	return nfr;
}
bool mdsp::wkeep(float* s,const int nleng,const int oldleng)
{
	if(nleng>=oldleng||nleng<=0)
	{
		return false;
	}
	const int delta = (oldleng-nleng)/2;
	int       i     = 0;
	for(i=0;i<nleng;++i)
	{
		s[i]=s[i+delta];
	}
	return true;
}
bool mdsp::dotMulti(mcomplex* s,float* g,int m,int n,int offset)
{
	if(offset<0)
		return false;
	if(n==0)
		n=m;
	//
	if(offset>=m)
	{
		memset(s,0,sizeof(float)*m);
		return true;
	}
	const int nsize=m<n+offset?m:n+offset;
	for(int i=offset;i<nsize;++i)
	{
		s[i]=s[i]*g[i+offset];
	}
	return true;
}
bool mdsp::dotMulti(float* s,float* g,int m,int n,int offset)
{
	if(offset<0)
		return false;
	if(n==0)
		n=m;
	//
	if(offset>=m)
	{
		memset(s,0,sizeof(float)*m);
		return true;
	}
	const int nsize = m<n+offset?m:n+offset;
	int       i     = 0;
	for(i=offset;i<nsize;++i)
	{
		s[i]*=g[i];
	}
	return true;
}
bool mdsp::createFilter(const float* argument,float*& filter,const int nleng,int nedgeLen,const bool bauto,const int type)
{
	int i     = 0,j=0;
	int index = 0;
	nedgeLen=argument[4]-argument[2];
	if(nedgeLen<=0)
	{
		for(i=1;i<argument[0]-1;++i)
		{
			if(argument[i*3+4]-argument[i*3+2]<nedgeLen)
				nedgeLen=argument[i*3+4]-argument[i*3+2];
		}
		if(nedgeLen>20)
			nedgeLen=20;
	}
	if(filter!=0)
	{
		delete[]filter;
		filter=0;
	}
	filter=new float[nleng];
	memset(filter,0,sizeof(float)*nleng);

	for(i=0;i<argument[0];++i)
	{
		index=i*3+1;	
		for(j=argument[index];j<argument[index+1];++j)
			filter[j]=argument[index+2];
	}
	//
	if(bauto)
	{
		memcpy(filter+nleng/2,filter,sizeof(float)*nleng/2);
		for(i=0;i<nleng/2;++i)
		{
			filter[i]=filter[nleng-i-1];
		}
	}
	//
	float       htemp[nedgeLen];
	const float a               = PI/(2 *nedgeLen),b=PI/nedgeLen;
	switch(type)
	{
		case MDSP_CREATEFILTER_COS:
			for(i=0;i<nedgeLen;++i)
			{
				htemp[i]=a*cos(b*(i-nedgeLen/2));
			}
			break;
		case MDSP_CREATEFILTER_TRIANGLE:
			for(i=0;i<nedgeLen;++i)
			{
				htemp[i]=2.0/nedgeLen-fabs(float(i)-1.0*nedgeLen/2.0)*4/(nedgeLen*nedgeLen);
			}
			break;
	}
	if(type)
	{
		float *g=0;
		conv(filter,htemp,g,nleng,nedgeLen);
		memcpy(filter,g,sizeof(float)*nleng);
		delete[]g;
	}
	if(bauto)
	{
		memcpy(filter,filter+nleng/2,sizeof(float)*nleng/2);
		for(i=nleng/2;i<nleng;++i)
		{
			filter[i]=filter[i-nleng/2];
		}
	}
	return true;
}
void mdsp::abs(mcomplex* sig,float*& pout,int nleng)
{
	if(pout==0)
	{
		pout=new float[nleng];
	}
	for(int i=0;i<nleng;++i)
		pout[i]=sig[i].mod();
}
int mdsp::to2n(int num)
{
	int nfr=log(num)/log(2);
	if(1<<nfr<num)
	{
		++nfr;
		return 1<<nfr;
	}
	else
	{
		return num;
	}
}
int mdsp::to2n(int num,int& nfr)
{
	nfr=log(num)/log(2);
	if(1<<nfr<num)
	{
		++nfr;
		return 1<<nfr;
	}
	else
	{
		return num;
	}
}
bool mdsp::r(float* px,float* py,float*& pout,int xlen,int ylen)
{
	if(xlen==0)
		return false;
	if(ylen<=0)
		ylen=xlen;
	if(pout==0)
	{
		pout=new float[ylen];
	}
	for(int i=0,j;i<ylen;++i)
	{
		pout[i]=0;
		for(j=i;j<ylen;++j)
		{
			pout[i]+=py[j]*px[j-i];
		}
	}
	return true;
}
bool mdsp::hilbert(const float* s,float*& pout,const int sleng)
{
	mcomplex *pfd    = 0;
	int       nfr    = 0;
	int       ncount = to2n(sleng,nfr);
	mcomplex *ptd    = new mcomplex[ncount];
	int       i      = 0;
	if(pout==0)
	{
		pout=new float[sleng];
	}
	mfft::fft(s,pfd,sleng);
	for(i=1;i<ncount/2;++i)
		pfd[i]=pfd[i]*2.0;
	for(i=ncount/2+1;i<ncount;++i)
		pfd[i]=0;
	mfft::ifft(ptd,pfd,nfr);
	for(i=0;i<sleng;++i)
		pout[i]=ptd[i][MIMG];
	delete[] ptd;
	delete[] pfd;
	return true;
}
void fht(int n,float x[])
{
	int   i  ,j,k,m,l1,l2,l3,l4,n1,n2,n4;
	float a  ,e,c,s,t,t1,t2;
	for (j=1,i=1;i<16;i++)
	{
		m=i;
		j=2*j;
		if(j==n) break;
	}
	n1=n-1;
	for (j=0,i=0;i<n1;i++)
	{
		if (i<j)
		{
			t=x[j];
			x[j]=x[i];
			x[i]=t;
		}
		k=n/2;
		while (k<(j+1))
		{
			j=j-k;
			k=k/2;                       
		}
		j=j+k;
	}
	for (i=0;i<n;i+=2)
	{
		t=x[i];
		x[i]=t+x[i+1];
		x[i+1]=t-x[i+1];
	}
	n2=1;
	for (k=2;k<=m;k++)
	{
		n4=n2;
		n2=n4+n4;
		n1=n2+n2;
		e=6.28318530719586/n1;
		for(j=0;j<n;j+=n1)
		{
			l2=j+n2;
			l3=j+n4;
			l4=l2+n4;
			t=x[j];
			x[j]=t+x[l2];
			x[l2]=t-x[l2];
			t=x[l3];
			x[l3]=t+x[l4];
			x[l4]=t-x[l4];
			a=e;
			for (i=1;i<n4;i++)
			{
				l1=j+i;
				l2=j-i+n2;
				l3=l1+n2;
				l4=l2+n2;
				c=cos(a);
				s=sin(a);
				t1=x[l3]*c+x[l4]*s;
				t2=x[l3]*s-x[l4]*c;
				a=(i+1)*e;
				t=x[l1];
				x[l1]=t+t1;
				x[l3]=t-t1;
				t=x[l2];
				x[l2]=t+t2;
				x[l4]=t-t2;
			}
		}

	}
}
void mdsp::hilbert(int n,float* x)
{
	int newsize=to2n(n);
	if(newsize==n)
	{
		n2_hilbert(n,x);
	}
	else
	{
		float *newx=new float[newsize];
		memcpy(newx,x,sizeof(float)*n);
		for(int i=n;i<newsize;++i)
			newx[i]=x[n-1];
		n2_hilbert(newsize,newx);
		memcpy(x,newx,sizeof(float)*n);
	}
}
void mdsp::n2_hilbert(int n,float x[])
{
	int   i  ,n1,n2;
	float t;
	n1=n/2;
	n2=n1+1;
	fht(n,x);
	for (i=1;i<n1;i++)
	{
		t=x[i];
		x[i]=x[n-i];
		x[n-i]=t;
	}
	for(i=n2;i<n;i++)
		x[i]=-x[i];
	x[0]=0.0;
	x[n1]=0.0;
	fht(n,x);
	t=1.0/n;
	for(i=0;i<n;i++)
		x[i]*=t;
}
