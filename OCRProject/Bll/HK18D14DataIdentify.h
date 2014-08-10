#ifndef HK18D14DataIdentify_H

#define HK18D14DataIdentify_H

#include <QString>

//#include "svm.h"
#include "./Include/HorseDataStruct.h"
# include "./Bll/algorithmParam.h"
#include <QDir>
#include "Include/Global.h"

using namespace cv;

//#define  WRITE_SESSION_CLASSIFY_SAMPELS 
//#define  WRITE_MINUTE_CLASSIFY_SAMPELS
#define  QDEBUG_OUTPUT
#define		WRITE_ROI_SMAPLES_CLASS_INFO1
//#define		WRITE_ROI_SMAPLES_CLASS_INFO2 



class HK18D14DataIdentify
{
public:
	HK18D14DataIdentify();
	~HK18D14DataIdentify();

 
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
	void writeSamples(int i,int j, int k,Mat &roi );
	
	int sampleCount[10];
 
 		 
  
};

// ���18̨����ʷ���ݣ����л����С 640*480 
 
// �޸ĸ�������λ�� ΪWMV�ļ� ��



/*
	��������λ�������ʹ�õ��Ǿ������꣬���� ��Ҫÿ�μ���Զ�����꣬Ȼ�����У��
	��Ϊͬһ����ʽ�Ļ��棬���ǻ������X �� ��Y���ϵ�ƫ�ơ�


*/

//����ԭ��λ��Ϊ 

#define ORIGIN_X_BASE 43
#define ORIGIN_Y_BASE 26 


//���κ�λ��
//#define SESSION_POS_RECT cvRect(240+(originX-ORIGIN_X_BASE),37+(originY-ORIGIN_Y_BASE),259-240,61-37)
//����λ��

//���κ�λ��
#define SESSION_POS_RECT cvRect(248+(originX-ORIGIN_X_BASE),35+(originY-ORIGIN_Y_BASE),24,25)
//����λ��

// ��ʷ��Ƶ����Ҫʶ�𵹼�ʱ
// ʮλ�� 
#define   COUNTDOWNMINUTE_POS_RECT1 cvRect(495+(originX-ORIGIN_X_BASE),32+(originY-ORIGIN_Y_BASE),10,15 )

//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(504+(originX-ORIGIN_X_BASE),32+(originY-ORIGIN_Y_BASE),10,15)

// WIN PLA
#define  WIN_POS_RECT cvRect(126+(originX-ORIGIN_X_BASE),55+(originY-ORIGIN_Y_BASE),32,364-58)
// PLA 
#define  PLA_POS_RECT cvRect(200+(originX-ORIGIN_X_BASE),55+(originY-ORIGIN_Y_BASE),32,364-58)
// ����
#define HORSENAME_REGION_RECT cvRect(51+(originX-ORIGIN_X_BASE),56+(originY-ORIGIN_Y_BASE),74,364-58)
//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT cvRect(87+(originX-ORIGIN_X_BASE),314+(originY-ORIGIN_Y_BASE),126-87,332-314)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT  cvRect(100+(originX-ORIGIN_X_BASE), 371+(originY-ORIGIN_Y_BASE), 16, 17)
//QIN QPL ���� ��������
#define		QINQPL_POS_RECT cvRect(66+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 664-66, 530-397)
 
#define  LB_REGION1_RECT cvRect(70+(originX-ORIGIN_X_BASE), 410+(originY-ORIGIN_Y_BASE),  36 , 115)
#define  LB_REGION2_RECT cvRect(109+(originX-ORIGIN_X_BASE), 430+(originY-ORIGIN_Y_BASE), 32, 95)
#define  LB_REGION3_RECT cvRect(146+(originX-ORIGIN_X_BASE), 449+(originY-ORIGIN_Y_BASE), 32, 76)
#define  LB_REGION4_RECT cvRect(181+(originX-ORIGIN_X_BASE), 467+(originY-ORIGIN_Y_BASE), 32, 58)

#define  LB_REGION5_RECT cvRect(216+(originX-ORIGIN_X_BASE), 487+(originY-ORIGIN_Y_BASE), 32  , 38)
#define  LB_REGION6_RECT cvRect(253+(originX-ORIGIN_X_BASE), 506+(originY-ORIGIN_Y_BASE), 32 , 19 )

#define RU_REGION1_RECT  cvRect(146+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 32, 17)
#define RU_REGION2_RECT  cvRect(182+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 31, 34)
#define RU_REGION3_RECT  cvRect(220+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 29, 54)
#define RU_REGION4_RECT  cvRect(256+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 29, 73)
#define RU_REGION5_RECT  cvRect(289+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 32, 92)
#define RU_REGION6_RECT  cvRect(324+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 32, 112)


#define R_REGION1_RECT cvRect(361+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 31, 128)
#define R_REGION2_RECT cvRect(398+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 32, 128)
#define R_REGION3_RECT cvRect(438+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 32, 128)
#define R_REGION4_RECT cvRect(472+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 31, 128)
#define R_REGION5_RECT cvRect(511+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 30 , 128)
#define R_REGION6_RECT cvRect(548+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 30 , 128)
#define R_REGION7_RECT cvRect(586+(originX-ORIGIN_X_BASE), 397+(originY-ORIGIN_Y_BASE), 28 , 128)

#define  NUMBER_HEIGHT 19
#define  NUMBER_WIDTH 31 
#endif