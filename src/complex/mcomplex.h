#ifndef _MCOMPLEX_H
#define _MCOMPLEX_H
#define MREAL 0
#define MIMG 1
#define COMPLEX_LZERO 0.00000001
class mcomplex
{
	public:
		mcomplex(float dwreal=0,float dwimg=0);
		mcomplex(const mcomplex &plex);
		~mcomplex(void);
	private:
		float _data[2];
	public:
		inline float &operator[](int i)
		{
			return _data[i];
		}
		mcomplex& operator=(const float inDw);
		mcomplex& operator=(const mcomplex &plex);
		bool operator==(const mcomplex &plex);
		mcomplex operator*(const mcomplex &inmcomplex);
		mcomplex operator*(const float k);
		mcomplex operator+(const mcomplex &inmcomplex);
		mcomplex operator-(const mcomplex &inmcomplex);
		float mod();
};
#endif
