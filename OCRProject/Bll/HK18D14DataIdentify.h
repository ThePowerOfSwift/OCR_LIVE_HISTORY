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
//#define		WRITE_ROI_SMAPLES_CLASS_INFO1
#define		WRITE_ROI_SMAPLES_CLASS_INFO2 



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

	int calculateGraySum(Mat srcMa);
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


//���κ�λ��
#define SESSION_POS_RECT cvRect(248,35,24,25)
//����λ��

// ��ʷ��Ƶ����Ҫʶ�𵹼�ʱ
// ʮλ�� 
#define   COUNTDOWNMINUTE_POS_RECT1 cvRect(495,32,10,15 )

//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(504,32,10,15)

// WIN PLA
#define  WIN_POS_RECT cvRect(125,55,31,364-58)
// PLA 
#define  PLA_POS_RECT cvRect(200,55,31,364-58)
// ����
#define HORSENAME_REGION_RECT cvRect(51,56,74,364-58)
//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT cvRect(87,314,126-87,332-314)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT  cvRect(100, 371, 16, 17)
//QIN QPL ���� ��������
#define		QINQPL_POS_RECT cvRect(66, 397, 664-66, 530-397)
 
#define  LB_REGION1_RECT cvRect(70, 410,  36 , 115)
#define  LB_REGION2_RECT cvRect(109, 430, 32, 95)
#define  LB_REGION3_RECT cvRect(146, 449, 32, 76)
#define  LB_REGION4_RECT cvRect(181, 467, 32, 58)

#define  LB_REGION5_RECT cvRect(216, 487, 32  , 38)
#define  LB_REGION6_RECT cvRect(253, 506, 32 , 19 )

#define RU_REGION1_RECT  cvRect(146, 397, 32, 17)
#define RU_REGION2_RECT  cvRect(182, 397, 31, 34)
#define RU_REGION3_RECT  cvRect(220, 397, 29, 54)
#define RU_REGION4_RECT  cvRect(256, 397, 29, 73)
#define RU_REGION5_RECT  cvRect(289, 397, 32, 92)
#define RU_REGION6_RECT  cvRect(324, 397, 32, 112)


#define R_REGION1_RECT cvRect(361, 397, 31, 128)
#define R_REGION2_RECT cvRect(398, 397, 32, 128)
#define R_REGION3_RECT cvRect(438, 397, 32, 128)
#define R_REGION4_RECT cvRect(472, 397, 31, 128)
#define R_REGION5_RECT cvRect(511, 397, 30 , 128)
#define R_REGION6_RECT cvRect(548, 397, 30 , 128)
#define R_REGION7_RECT cvRect(586, 397, 28 , 128)

#define  NUMBER_HEIGHT 19
#define  NUMBER_WIDTH 31 
#endif