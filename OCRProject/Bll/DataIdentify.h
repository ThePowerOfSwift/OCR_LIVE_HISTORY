#ifndef DataIdentify_H

#define DataIdentify_H


//ʵʱֱ��

#include <QString>

//#include "svm.h"
#include "./Include/HorseDataStruct.h"
# include "./Bll/algorithmParam.h"
#include <QDir>
#include "Include/Global.h"

using namespace std;
 
using namespace cv;

//#define  WRITE_SESSION_CLASSIFY_SAMPELS 


#define  WRITE_MINUTE_CLASSIFY_SAMPELS


#define		QDEBUG_OUTPUT

#define		WRITE_ROI_SMAPLES_TEMP
#define		WRITE_ROI_SMAPLES_CLASS_INFO1
#define		WRITE_ROI_SMAPLES_CLASS_INFO2 





class DataIdentify
{
public:
	DataIdentify();
	~DataIdentify();

	// ����Ľӿڲ���

	DataOutput dataOutput;

	// -1 processed,but no data, 0 processing, 1 processed
	bool isHistoryVideo;															// true: history video, false: live video 
	bool haveDataFlag;																// DataIdentify the frame has data or not


	bool read(Mat &srcMat, uchar* data, int length, int height = 480, int width = 640);		// return the error type, if all right, then return 0
	void haveData();

	void isReady();												//return the error type, if all right, then return 0

	int originPosition(/*void* imageData, int x, int y*/);							// return the position of the image origin

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
	CvRect * qinQplSubRect;

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

	int calculateGraySum(Mat srcMa, int &length);
	// ��ȡ WIN PLA ʶ��

	int getWINPLAIdentify();
	// ����Ƿ������ɫ
	bool haveGroundColor(Mat srcMat, int flag);
	// ��ȡ QIN QPL ͬʱ��� QIN QPL ���
	int getQINQPLIdentify();

	//��� ���κ� 

	int getSessionIdentify();
	//��õ���ʱ 

	int getCountDownIdentify();

	//�ж� WIN PLA�Ƿ���С����
	bool judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge);							// DataIdentify dot for region 1 and region 2 (QPL), i = 1, region 1, i = 2, region 2(QPL)
	// �ж� QIN QPL�Ƿ���С����
	int judgeQINQPLDot(Mat &roi, Mat &edge, int *x);
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



//����ԭ��λ��Ϊ 

#define ORIGIN_X_BASE_LIVE 60

#define ORIGIN_Y_BASE_LIVE  37 
//���κ�λ��
#define SESSION_POS_RECT_LIVE cvRect(267+(originX-ORIGIN_X_BASE_LIVE),38+(originY-ORIGIN_Y_BASE_LIVE),285-267,60-38)
//����λ��
// ʮλ��
#define  COUNTDOWNMINUTE_POS_RECT1_LIVE cvRect(560+(originX-ORIGIN_X_BASE_LIVE),41+(originY-ORIGIN_Y_BASE_LIVE),10,16)
//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2_LIVE cvRect(569+(originX-ORIGIN_X_BASE_LIVE),41+(originY-ORIGIN_Y_BASE_LIVE),10,16)

// WIN PLA
#define  WIN_POS_RECT_LIVE cvRect(136+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)
// PLA 
#define  PLA_POS_RECT_LIVE cvRect(216+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)

//QIN QPL��ǩλ�� 
#define  QINQPL_LABEL_POS_RECT_LIVE cvRect(91+(originX-ORIGIN_X_BASE_LIVE),377+(originY-ORIGIN_Y_BASE_LIVE),50,20 )

// QIN QPL λ��
#define		WHOLE_QINQPL_POS_RECT_LIVE  cvRect(9+(originX-ORIGIN_X_BASE_LIVE), 378+(originY-ORIGIN_Y_BASE_LIVE), 697, 170)
#define		QINQPL_POS_RECT_LIVE cvRect(69, 22, 563, 137)

#define  RE_BASE_X 78+(originX-ORIGIN_X_BASE_LIVE)
#define  RE_BASE_Y 400+(originY-ORIGIN_Y_BASE_LIVE)

#define  LB_REGION1_RECT_LIVE cvRect(4+RE_BASE_X, 19+RE_BASE_Y, 35, 116)
#define  LB_REGION2_RECT_LIVE cvRect(42+RE_BASE_X, 38+RE_BASE_Y, 36, 99)
#define  LB_REGION3_RECT_LIVE cvRect(77+RE_BASE_X, 56+RE_BASE_Y, 35, 79)
#define  LB_REGION4_RECT_LIVE cvRect(114+RE_BASE_X, 74+RE_BASE_Y, 35, 62)
#define  LB_REGION5_RECT_LIVE cvRect(149+RE_BASE_X, 95+RE_BASE_Y,35 , 40)
#define  LB_REGION6_RECT_LIVE cvRect(188+RE_BASE_X, 111+RE_BASE_Y, 35 , 24)

#define RU_REGION1_RECT_LIVE  cvRect(81+RE_BASE_X, 0+RE_BASE_Y, 36, 21)
#define RU_REGION2_RECT_LIVE  cvRect(117+RE_BASE_X, 0+RE_BASE_Y, 35 , 40)
#define RU_REGION3_RECT_LIVE  cvRect(154+RE_BASE_X, 0+RE_BASE_Y, 35 , 60)
#define RU_REGION4_RECT_LIVE  cvRect(190+RE_BASE_X, 0+RE_BASE_Y, 35 , 78)
#define RU_REGION5_RECT_LIVE  cvRect(227+RE_BASE_X, 0+RE_BASE_Y, 35 , 96)
#define RU_REGION6_RECT_LIVE  cvRect(263+RE_BASE_X, 0+RE_BASE_Y, 35 , 115)

#define R_REGION1_RECT_LIVE  cvRect(300+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)
#define R_REGION2_RECT_LIVE cvRect(336+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)
#define R_REGION3_RECT_LIVE cvRect(373+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)
#define R_REGION4_RECT_LIVE cvRect(409+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)
#define R_REGION5_RECT_LIVE cvRect(447+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)
#define R_REGION6_RECT_LIVE cvRect(481+RE_BASE_X, 0+RE_BASE_Y, 36 , 136)
#define R_REGION7_RECT_LIVE cvRect(517+RE_BASE_X, 0+RE_BASE_Y, 35 , 136)

// ����λ�� 
#define HORSENAME_REGION_RECT_LIVE cvRect(69+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),134-69,370-65)



#define  NUMBER_HEIGHT_LIVE 15 
#define  NUMBER_WIDTH_LIVE 36


#endif
