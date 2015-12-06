
#if !defined(FILEHEAD_H)
#define FILEHEAD_H


//***************************** GPR *************************************//
#pragma pack(push)
#pragma pack(1)

struct GPOINT
{
	int x;
	int y;
};

struct _GPR_HEAD
{
	char chDescription[128]; 
	double dLenInt;          
	char chClrFileName[64];  
	GPOINT sHandGainPt[10];  
	int iGainPtNum;         
	
    unsigned char  btReserve1[128];

	char chFileFlag[4];	
	double dDielectric;	
    double dFreq;		
    double dTimeWnd;	// 时窗      
	int    iSmpNum;     // 采样点数
	int    iTrcNum;	    // 采样道数

    unsigned char btReserve2[124];

	int iSwMode;  
	int iTrcInt;  
	int iTrcDis;  
	int iMaxAmp;  
	bool bNormal; 

    unsigned char btReserve3[128];

	int iLfAxMode; 
	int iRtAxMode;
	int iTpAxMode; 
	int iBtAxMode; 
	
	int iLfMinSmp;     
	double dLfMinTime; 
	double dLfMinDepth;

	int iRtMinSmp;     
	double dRtMinTime; 
	double dRtMinDepth;

	int iTpMinTrc;   
	double dTpMinDis;
	
	int iBtMinTrc;   
	double dBtMinDis;
	
    bool bAutoAxis;

    bool bChsAxis;

	int iZeroPos; 

    unsigned char btReserve4[256];
};
#pragma pack(pop)

#endif
