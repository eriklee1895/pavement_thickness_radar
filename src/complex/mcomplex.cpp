#include "mcomplex.h"
#include <math.h>
mcomplex::mcomplex(float dwreal,float dwimg)
{
	_data[MREAL]   =   dwreal;
	_data[MIMG]    =   dwimg;
}
mcomplex::mcomplex(const mcomplex &plex)
{
	_data[0]   =   plex._data[0];
	_data[1]   =   plex._data[1];
}
mcomplex::~mcomplex(void)
{
}
mcomplex &mcomplex::operator=(const mcomplex &plex)
{
	_data[0]   =   plex._data[0];
	_data[1]   =   plex._data[1];
	return *this;
}
bool mcomplex::operator==(const mcomplex &plex)
{
	if(fabs(_data[0]-plex._data[0])<COMPLEX_LZERO&&fabs(_data[1]-plex._data[1])<COMPLEX_LZERO)
		return true;
	return false;
}
mcomplex mcomplex::operator*(const float k)
{
	mcomplex reTemp;
	reTemp._data[MREAL]   =   _data[MREAL]*k;
	reTemp._data[MIMG]    =   _data[MIMG]*k;
	return reTemp;
}
mcomplex mcomplex::operator*(const mcomplex &inmcomplex)
{
	mcomplex reTemp;
	reTemp._data[MREAL]   =   _data[MREAL]*inmcomplex._data[MREAL]-_data[MIMG]*inmcomplex._data[MIMG];
	reTemp._data[MIMG]    =   _data[MIMG]*inmcomplex._data[MREAL]+_data[MREAL]*inmcomplex._data[MIMG];
	return reTemp;
}
mcomplex& mcomplex::operator=(const float inDw)
{
	_data[MREAL]   =   inDw;
	_data[MIMG]    =   0;
	return *this;
}
mcomplex mcomplex::operator+(const mcomplex &inmcomplex)
{
	mcomplex reTemp;
	reTemp._data[MREAL]   =   _data[MREAL]+inmcomplex._data[MREAL];
	reTemp._data[MIMG]    =   _data[MIMG]+inmcomplex._data[MIMG];
	return reTemp;
}
mcomplex mcomplex::operator-(const mcomplex &inmcomplex)
{
	mcomplex reTemp;
	reTemp._data[MREAL]   =   _data[MREAL]-inmcomplex._data[MREAL];
	reTemp._data[MIMG]    =   _data[MIMG]-inmcomplex._data[MIMG];
	return reTemp;
}
float mcomplex::mod()
{
	return sqrt(_data[MREAL]*_data[MREAL]+_data[MIMG]*_data[MIMG]);
}
