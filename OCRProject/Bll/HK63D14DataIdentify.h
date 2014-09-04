#ifndef HK63D14DataIdentify_H

#define HK63D14DataIdentify_H

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


class HK63D14DataIdentify
{
public:
	HK63D14DataIdentify();
	~HK63D14DataIdentify();

 
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
	void writeSamples(QString fileName,Mat &roi,QString path );
	
	int sampleCount[10];
 
 		 
  
};

// ���63����ʷ���ݣ����л����С 640*480 
 
// �޸ĸ�������λ�� ΪWMV�ļ� ��



/*
	��������λ�������ʹ�õ��Ǿ������꣬���� ��Ҫÿ�μ���Զ�����꣬Ȼ�����У��
	��Ϊͬһ����ʽ�Ļ��棬���ǻ������X �� ��Y���ϵ�ƫ�ơ�


*/

//����ԭ��λ��Ϊ  ��2010�� 10.1 ����Ƶ��Ϊ��׼ �������С 704*576

#define ORIGIN_X_BASE_HK63D14 27
#define ORIGIN_Y_BASE_HK63D14 34


//���κ�λ��
//#define SESSION_POS_RECT_HK63D14 cvRect(240+(originX-ORIGIN_X_BASE_HK63D14),37+(originY-ORIGIN_Y_BASE_HK63D14),259-240,61-37)
//����λ��

//���κ�λ��
#define SESSION_POS_RECT_HK63D14 cvRect(82+(originX-ORIGIN_X_BASE_HK63D14),27+(originY-ORIGIN_Y_BASE_HK63D14),27,29)
//����λ��

// ��ʷ��Ƶ����Ҫʶ�𵹼�ʱ
// ʮλ�� 
#define   COUNTDOWNMINUTE_POS_RECT1_HK63D14 cvRect(495+(originX-ORIGIN_X_BASE_HK63D14),32+(originY-ORIGIN_Y_BASE_HK63D14),10,15 )

//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2_HK63D14 cvRect(504+(originX-ORIGIN_X_BASE_HK63D14),32+(originY-ORIGIN_Y_BASE_HK63D14),10,15)

// WIN PLA
#define  WIN_POS_RECT_HK63D14 cvRect(57+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),40,371-69)
// PLA 
#define  PLA_POS_RECT_HK63D14 cvRect(121+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),35,371-69)
// ����
#define HORSENAME_REGION_RECT_HK63D14 cvRect(20+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),35,371-69)
//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT_HK63D14 cvRect(59+(originX-ORIGIN_X_BASE_HK63D14),377+(originY-ORIGIN_Y_BASE_HK63D14),19,19)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT_HK63D14_HK63D14  cvRect(100+(originX-ORIGIN_X_BASE_HK63D14), 371+(originY-ORIGIN_Y_BASE_HK63D14), 16, 17)
//QIN QPL ���� ��������
#define		QINQPL_POS_RECT_HK63D14 cvRect(45+(originX-ORIGIN_X_BASE_HK63D14), 394+(originY-ORIGIN_Y_BASE_HK63D14), 637-45, 524-394)
 
#define  LB_REGION1_RECT_HK63D14 cvRect(49+(originX-ORIGIN_X_BASE_HK63D14), 412+(originY-ORIGIN_Y_BASE_HK63D14),  36 ,117)
#define  LB_REGION2_RECT_HK63D14 cvRect(93+(originX-ORIGIN_X_BASE_HK63D14), 432+(originY-ORIGIN_Y_BASE_HK63D14), 39, 97)
#define  LB_REGION3_RECT_HK63D14 cvRect(138+(originX-ORIGIN_X_BASE_HK63D14), 449+(originY-ORIGIN_Y_BASE_HK63D14), 40, 78)
#define  LB_REGION4_RECT_HK63D14 cvRect(179+(originX-ORIGIN_X_BASE_HK63D14), 472+(originY-ORIGIN_Y_BASE_HK63D14), 40, 58)

#define  LB_REGION5_RECT_HK63D14 cvRect(222+(originX-ORIGIN_X_BASE_HK63D14), 492+(originY-ORIGIN_Y_BASE_HK63D14), 40  , 38)
#define  LB_REGION6_RECT_HK63D4 cvRect(264+(originX-ORIGIN_X_BASE_HK63D14), 511+(originY-ORIGIN_Y_BASE_HK63D14), 40 , 19 )

#define RU_REGION1_RECT_HK63D14  cvRect(83+(originX-ORIGIN_X_BASE_HK63D14),  391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 16)
#define RU_REGION2_RECT_HK63D14  cvRect(119+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 38)
#define RU_REGION3_RECT_HK63D4  cvRect(160+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 57)
#define RU_REGION4_RECT_HK63D14  cvRect(200+(originX-ORIGIN_X_BASE_HK63D14), 390+(originY-ORIGIN_Y_BASE_HK63D14), 40, 78)
#define RU_REGION5_RECT_HK63D14  cvRect(240+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 92)
#define RU_REGION6_RECT_HK63D14  cvRect(280+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 117)


#define R_REGION1_RECT_HK63D14 cvRect(320+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 132)
#define R_REGION2_RECT_HK63D14 cvRect(360+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 132)
#define R_REGION3_RECT_HK63D14 cvRect(400+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 132)
#define R_REGION4_RECT_HK63D14 cvRect(449+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40, 132)
#define R_REGION5_RECT_HK63D14 cvRect(500+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40 , 132)
#define R_REGION6_RECT_HK63D14 cvRect(545+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40 , 132)
#define R_REGION7_RECT_HK63D14 cvRect(599+(originX-ORIGIN_X_BASE_HK63D14), 391+(originY-ORIGIN_Y_BASE_HK63D14), 40 , 132)

#define  NUMBER_HEIGHT_HK63D14 15
#define  NUMBER_WIDTH_HK63D14 31 
#endif