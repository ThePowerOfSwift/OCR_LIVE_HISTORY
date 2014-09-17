#ifndef DataIdentify_H

#define DataIdentify_H


//ʵʱֱ��

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
#define  WRITE_ROI_SMAPLES_TEMP
//#define		WRITE_ROI_SMAPLES_CLASS_INFO1
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

#define ORIGIN_X_BASE_LIVE 58
#define ORIGIN_Y_BASE_LIVE 39 
//���κ�λ��
#define SESSION_POS_RECT_LIVE cvRect(267+(originX-ORIGIN_X_BASE_LIVE),38+(originY-ORIGIN_Y_BASE_LIVE),285-267,60-38)
//����λ��
// ʮλ��
#define  COUNTDOWNMINUTE_POS_RECT1_LIVE cvRect(567+(originX-ORIGIN_X_BASE_LIVE),37+(originY-ORIGIN_Y_BASE_LIVE),575-567,54-37)
//��λ��
#define  COUNTDOWNMINUTE_POS_RECT2_LIVE cvRect(576+(originX-ORIGIN_X_BASE_LIVE),37+(originY-ORIGIN_Y_BASE_LIVE),587-576,54-37)

// WIN PLA
#define  WIN_POS_RECT_LIVE cvRect(136+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)
// PLA 
#define  PLA_POS_RECT_LIVE cvRect(216+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)

//QIN QPL��ǩλ��
#define  QINQPL_LABEL_POS_RECT_LIVE cvRect(126+(originX-ORIGIN_X_BASE_LIVE),381+(originY-ORIGIN_Y_BASE_LIVE),12,17)

// QIN QPL λ��
#define		WHOLE_QINQPL_POS_RECT_LIVE  cvRect(9+(originX-ORIGIN_X_BASE_LIVE), 378+(originY-ORIGIN_Y_BASE_LIVE), 697, 197)
#define		QINQPL_POS_RECT_LIVE cvRect(69, 22, 563, 137)

#define  LB_REGION1_RECT_LIVE cvRect(4, 19, 35, 116)
#define  LB_REGION2_RECT_LIVE cvRect(42, 38, 36, 99)
#define  LB_REGION3_RECT_LIVE cvRect(77, 56, 35, 79)
#define  LB_REGION4_RECT_LIVE cvRect(114, 74, 35, 62)
#define  LB_REGION5_RECT_LIVE cvRect(149, 95,35 , 40)
#define  LB_REGION6_RECT_LIVE cvRect(188, 111, 35 , 24)

#define RU_REGION1_RECT_LIVE  cvRect(81, 0, 36, 21)
#define RU_REGION2_RECT_LIVE  cvRect(117, 0, 35 , 40)
#define RU_REGION3_RECT_LIVE  cvRect(154, 0, 35 , 60)
#define RU_REGION4_RECT_LIVE  cvRect(190, 0, 35 , 78)
#define RU_REGION5_RECT_LIVE  cvRect(227, 0, 35 , 96)
#define RU_REGION6_RECT_LIVE  cvRect(263, 0, 35 , 115)

#define R_REGION1_RECT_LIVE  cvRect(300, 0, 35 , 136)
#define R_REGION2_RECT_LIVE cvRect(336, 0, 35 , 136)
#define R_REGION3_RECT_LIVE cvRect(373, 0, 35 , 136)
#define R_REGION4_RECT_LIVE cvRect(409, 0, 35 , 136)
#define R_REGION5_RECT_LIVE cvRect(447, 0, 35 , 136)
#define R_REGION6_RECT_LIVE cvRect(481, 0, 36 , 136)
#define R_REGION7_RECT_LIVE cvRect(517, 0, 35 , 136)

// ����λ�� 
#define HORSENAME_REGION_RECT_LIVE cvRect(69+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),134-69,370-65)



#define  NUMBER_HEIGHT_LIVE 18 
#define  NUMBER_WIDTH_LIVE 36


#endif
