#ifndef YZTDataIdentify_H

#define YZTDataIdentify_H

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

//#define  WRITE_HORSENAME_FILE

class YZTDataIdentify
{
public:
	YZTDataIdentify();
	~YZTDataIdentify();

	// ����Ľӿڲ���
	HorseInfo horseInfo;
	DataOutput dataOutput;

	int isProcessed;																// -1 processed,but no data, 0 processing, 1 processed
	bool isHistoryVideo;															// true: history video, false: live video 
	bool haveDataFlag;																// DataIdentify the frame has data or not
	bool isQPL;
	int horseNum;																	// the horse total number from video


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
	CvRect * qinQplSubRect_14 ;
	CvRect * qinQplSubRect_12 ;
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
	// ��ȡ WIN PLA ʶ��

	int getWINPLAIdentify();

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
	//������
	int calculateXBewttenNumber(Mat &mat, int *x);
	//�����ж�С���� ͨ������ ����ĳ����ֵ�� �հ������С
	int calculateXBewttenNumberForJudgeDot(Mat &mat, int  *x);
	int calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum);
	//���� 3λ��ÿ������X���꣬Ȼ���������ַָ� QIN QPL
	int calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum);


	int calculateGraySumXForSetQINQPLRect(Mat &mat, int *y, int roiNum);
	int calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum);
	
	//�����㷨״̬ 

	//�����ô���
	void createClassifySamples(float result, Mat &singleNum);
	void writeSamples(int i,int j, int k,Mat &roi );
	
	int sampleCount[10];
 
   
  
};

 

// ����̨ ��ʽ����
//���κ�λ��
#define SESSION_POS_RECT cvRect(50,22,21,19)
//����λ��

// ʮλ�� 
#define   COUNTDOWNMINUTE_POS_RECT1 cvRect(163,22,12,18 )

//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(174,22,12,18)

// WIN PLA
#define  WIN_POS_RECT cvRect(141,78,41,312-78)
// PLA 
#define  PLA_POS_RECT cvRect(181,78,40,312-78 )
// ����
#define HORSENAME_REGION_RECT cvRect(76,78,128-76,312-76)
//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT cvRect(225,327,247-225,340-327)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT  cvRect(12, 314, 624-12, 476-314)
//QIN QPL ���� ��������
#define		QINQPL_POS_RECT cvRect(256, 345, 601-256, 445-328)
// 14 ƥ��

#define  LB_REGION1_RECT cvRect(254, 344,274-254, 445-344 )
#define  LB_REGION2_RECT cvRect(278, 362, 22, 445-362)
#define  LB_REGION3_RECT cvRect(300, 378, 26, 445-378)
#define  LB_REGION4_RECT cvRect(326, 395, 26, 445-395)

#define  LB_REGION5_RECT cvRect(352, 415,26  , 30)
#define  LB_REGION6_RECT cvRect(382, 432, 26 , 13 )

#define RU_REGION1_RECT  cvRect(257, 328, 25, 13)
#define RU_REGION2_RECT  cvRect(282, 328, 25, 30)
#define RU_REGION3_RECT  cvRect(308, 328, 25, 47)
#define RU_REGION4_RECT  cvRect(333, 328, 29, 65)
#define RU_REGION5_RECT  cvRect(362, 328, 26, 83)
#define RU_REGION6_RECT  cvRect(388, 328, 27, 100)

#define R_REGION1_RECT cvRect(415, 328, 27, 117)
#define R_REGION2_RECT cvRect(442, 328, 27, 117)
#define R_REGION3_RECT cvRect(469, 328, 26, 117)
#define R_REGION4_RECT cvRect(495, 328, 27, 117)
#define R_REGION5_RECT cvRect(522, 328, 26 , 117)
#define R_REGION6_RECT cvRect(548, 328, 30 , 117)
#define R_REGION7_RECT cvRect(574, 328, 26 , 117)

// 12 ƥ��

#define  LB_REGION1_RECT_12 cvRect(256, 348,27, 446-348 )
#define  LB_REGION2_RECT_12 cvRect(283, 369, 31, 445-369)
#define  LB_REGION3_RECT_12 cvRect(314, 389, 34, 445-389)
#define  LB_REGION4_RECT_12 cvRect(348, 411, 30, 445-411)

#define  LB_REGION5_RECT_12 cvRect(378, 431,31  , 14)


#define  LB_REGION6_RECT_12 cvRect(0, 0, 0 , 0 )

#define RU_REGION1_RECT_12  cvRect(259, 328, 27, 14)
#define RU_REGION2_RECT_12  cvRect(286, 328, 30, 35)
#define RU_REGION3_RECT_12  cvRect(317, 328, 31, 56)
#define RU_REGION4_RECT_12  cvRect(350, 328, 33, 65)
#define RU_REGION5_RECT_12  cvRect(381, 328, 31, 78)

#define RU_REGION6_RECT_12  cvRect(0, 0, 0, 0)

#define R_REGION1_RECT_12  cvRect(413, 328, 31, 98)
#define R_REGION2_RECT_12 cvRect(444, 328, 31, 98)
#define R_REGION3_RECT_12 cvRect(477, 328, 33, 98)
#define R_REGION4_RECT_12 cvRect(511, 328, 34, 98)
#define R_REGION5_RECT_12 cvRect(542, 328, 31 , 98)
#define R_REGION6_RECT_12 cvRect(574, 328, 32 , 98)
#define R_REGION7_RECT_12 cvRect(0, 0, 0 , 0)




//����Ƿ�Ϊ������ �궨�� 

#define   NUMBER_HEIGHT 13 
#define   ADCHECK_WIN_ORIGIN_X  16
#define   ADCHECK_WIN_ORIGIN_Y  80

#define  ADCHECK_QIN_ORIGIN_X 618
#define  ADCHECK_QIN_ORIGIN_Y 330


#endif