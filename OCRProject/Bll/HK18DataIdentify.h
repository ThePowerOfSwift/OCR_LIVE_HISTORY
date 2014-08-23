#ifndef HK18DataIdentify_H

#define HK18DataIdentify_H

#include <QString>

//#include "svm.h"
#include "./Include/HorseDataStruct.h"
# include "./Bll/algorithmParam.h"
#include <QDir>
#include "Include/Global.h"

using namespace cv;

//#define  WRITE_SESSION_CLASSIFY_SAMPELS 
//#define  WRITE_MINUTE_CLASSIFY_SAMPELS
//#define  QDEBUG_OUTPUT
//#define		WRITE_ROI_SMAPLES_CLASS_INFO1
//#define		WRITE_ROI_SMAPLES_CLASS_INFO2 
//#define WRITE_ROI_SMAPLES_TEMP

class HK18DataIdentify
{
public:
	HK18DataIdentify();
	~HK18DataIdentify();

 
	//dataOutput���������mHorseInfo
	DataOutput dataOutput;
 
	bool isHistoryVideo;															// true: history video, false: live video 
	bool haveDataFlag;																// DataIdentify the frame has data or not
 
	bool read(Mat &srcMat, uchar* data, int length, int height = 480, int width = 640);		// return the error type, if all right, then return 0
	void haveData();
 
	void isReady();												//return the error type, if all right, then return 0

	void originPosition(/*void* imageData, int x, int y*/);							// return the position of the image origin
 
	// ===============================
	int identify();

 
private:

	Mat image;																		// current original image
	int dataLength;																	// the original data length	
	int imageHeight, imageWidth;
	int originX, originY;															// the position of the image origin;

	WINPLAPosStruct winPlaPosStruct;
	QINQPLPosStruct qinQPLPosStruct;
	CountDownMinutePosStruct countDownMinutePosStruct;
	//�����ֵ�����
	HorseNamePosStruct horseNamePosStruct;
	SessionPosStruct sessionPosStruct;
	CvRect * qinQplSubRect ;

	HOGDescriptor hog;
	Size winStride;
	Size padding;
	vector<float> descriptorVector;
	Mat hogMat;

	SVM numSVM;
	SVM LNSVM;
	SVM horseNameSVM;

	SVM raceTimeSvm;
	SVM sessionNumSvm;
	// true:QPL, false:QIN
	int algorithmState;
 
 
		 	 

	//��������λ�ã��������������rect 
	int setHorseNameRectPos();

	//����WIN PLA�����λ��
	int setWINPLARectPos();
	//����QIN QPL �����rect 
	int setQINQPLRectPos();
	//����ÿһ��QINQPL���ֵ�Rect
	int setEveryQINQPLPos(Mat &mat, int rectNum);
	//���õ���ʱλ��
	int setCountDownRectPos();
	//���ó��κ�λ��
	int setSessionRectPos();

	 //��ȡ����
	int getHorseNameIdentify();

	//����Ƿ����������仯�������жϳ��κű仯
	int isHorseNameChanged();

	//����ҶȺ�

	int calculateGraySum(Mat srcMa, int &length );
	// ��ȡ WIN PLA ʶ��

	int getWINPLAIdentify();
	// �ж��Ƿ��е�ɫ ��ɫ��ɫ flag =1 Ϊwin pla ��flag = 2 Ϊqpl qin

	bool  haveGroundColor(Mat srcMat,int flag );

	// ��ȡ QIN QPL ͬʱ��� QIN QPL ���
	int getQINQPLIdentify();

	//��� ���κ� 

	int getSessionIdentify();
	//��õ���ʱ 

	int getCountDownIdentify();

	//�ж� WIN PLA�Ƿ���С����
	bool judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge);							// DataIdentify dot for region 1 and region 2 (QPL), i = 1, region 1, i = 2, region 2(QPL)
	 // �ж� QIN QPL�Ƿ���С����
	int judgeQINQPLDot(Mat &roi, Mat &edge ,int *x);
	//�ж��Ƿ���QIN ����QPl��ǩ
	int judgeQINQPL();

	// ��Ҫ�ǽ�ͼ��ı�Ե���ಿ�ּ�������Ҫ����QIN QPL ���ݴ���
	int  trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize);

	//�Բ�ɫͼ�������ֵ 
	int colorThreshold(Mat &src, Mat &dst, int thereshold);
	// return 0-- two number with dot,  1-- two number without dot, 2-- three number	
	int calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum);
	int calculateXBewttenNumber(Mat &mat, int *x);
	int calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum);
	//���� 3λ��ÿ������X���꣬Ȼ���������ַָ� QIN QPL
	int calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum);


	int calculateGraySumXForSetQINQPLRect(Mat &mat, int *y, int roiNum);
	int calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum);
	
	//�����㷨״̬ 

	//�����ô���
	void createClassifySamples(float result, Mat &singleNum);
	void writeSamples(QString fileName, Mat &roi, QString path);
	
	int sampleCount[10];
 
 		 
  
};



// ���18̨����ʷ���ݣ����л����С 640*480 
 
// �޸ĸ�������λ�� ΪWMV�ļ� ��

/*
	��������λ�������ʹ�õ��Ǿ������꣬���� ��Ҫÿ�μ���Զ�����꣬Ȼ�����У��
	��Ϊͬһ����ʽ�Ļ��棬���ǻ������X �� ��Y���ϵ�ƫ�ơ�


*/

//����ԭ��λ��Ϊ 

#define ORIGIN_X_BASE 53 
#define ORIGIN_Y_BASE 31 


//���κ�λ��
#define SESSION_POS_RECT cvRect(240+(originX-ORIGIN_X_BASE),37+(originY-ORIGIN_Y_BASE),259-240,61-37)
//����λ��

// ʮλ�� 
#define   COUNTDOWNMINUTE_POS_RECT1 cvRect(495+(originX-ORIGIN_X_BASE),32+(originY-ORIGIN_Y_BASE),10,15 )

//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(504+(originX-ORIGIN_X_BASE),32+(originY-ORIGIN_Y_BASE),10,15)

// WIN PLA
#define  WIN_POS_RECT cvRect(121+(originX-ORIGIN_X_BASE),52+(originY-ORIGIN_Y_BASE),35,308-53)
// PLA 
#define  PLA_POS_RECT cvRect(192+(originX-ORIGIN_X_BASE),52+(originY-ORIGIN_Y_BASE),35,306-53)
// ����
#define HORSENAME_REGION_RECT cvRect(63+(originX-ORIGIN_X_BASE),52+(originY-ORIGIN_Y_BASE),121-63,308-53)
//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT cvRect(114+(originX-ORIGIN_X_BASE),316+(originY-ORIGIN_Y_BASE),12,15)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT  cvRect(12+(originX-ORIGIN_X_BASE), 314+(originY-ORIGIN_Y_BASE), 624-12, 476-314)
//QIN QPL ���� ��������
#define		QINQPL_POS_RECT cvRect(76+(originX-ORIGIN_X_BASE), 335+(originY-ORIGIN_Y_BASE), 570-76, 446-335)
 
#define  LB_REGION1_RECT cvRect(80+(originX-ORIGIN_X_BASE), 348+(originY-ORIGIN_Y_BASE),  28 , 445-348)
#define  LB_REGION2_RECT cvRect(109+(originX-ORIGIN_X_BASE), 365+(originY-ORIGIN_Y_BASE), 28, 445-365)
#define  LB_REGION3_RECT cvRect(143+(originX-ORIGIN_X_BASE), 380+(originY-ORIGIN_Y_BASE), 28, 445-380)
#define  LB_REGION4_RECT cvRect(175+(originX-ORIGIN_X_BASE), 395+(originY-ORIGIN_Y_BASE), 28, 445-395)

#define  LB_REGION5_RECT cvRect(206+(originX-ORIGIN_X_BASE), 415+(originY-ORIGIN_Y_BASE), 28  , 30)
#define  LB_REGION6_RECT cvRect(237+(originX-ORIGIN_X_BASE), 429+(originY-ORIGIN_Y_BASE), 28 , 16 )

#define RU_REGION1_RECT  cvRect(142+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28, 15)
#define RU_REGION2_RECT  cvRect(176+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28, 30)
#define RU_REGION3_RECT  cvRect(210+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28, 47)
#define RU_REGION4_RECT  cvRect(241+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28, 64)
#define RU_REGION5_RECT  cvRect(274+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29, 80)
#define RU_REGION6_RECT  cvRect(307+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28, 96)

#define R_REGION1_RECT cvRect(338+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29, 111)
#define R_REGION2_RECT cvRect(370+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29, 111)
#define R_REGION3_RECT cvRect(403+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29, 111)
#define R_REGION4_RECT cvRect(435+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29, 111)
#define R_REGION5_RECT cvRect(469+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 29 , 111)
#define R_REGION6_RECT cvRect(502+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28 , 111)
#define R_REGION7_RECT cvRect(533+(originX-ORIGIN_X_BASE), 334+(originY-ORIGIN_Y_BASE), 28 , 111)

#define  NUMBER_HEIGHT 16 
#define  NUMBER_WIDTH 28 
#endif