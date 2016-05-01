#ifndef DataIdentify_H

#define DataIdentify_H


//实时直播

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

	// 对外的接口参数

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
	//马名字的区域
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




	//设置马名位置，获得所有马名的rect 
	int setHorseNameRectPos();

	//设置WIN PLA，获得位置
	int setWINPLARectPos();
	//设置QIN QPL ，获得rect 
	int setQINQPLRectPos();
	//设置每一个QINQPL数字的Rect
	int setEveryQINQPLPos(Mat &mat, int rectNum);
	//设置倒计时位置
	int setCountDownRectPos();
	//设置场次号位置
	int setSessionRectPos();

	//获取马名
	int getHorseNameIdentify();
	//检测是否马名发生变化，用于判断场次号变化
	int isHorseNameChanged();

	//计算灰度和

	int calculateGraySum(Mat srcMa, int &length);
	// 获取 WIN PLA 识别

	int getWINPLAIdentify();
	// 检查是否包含底色
	bool haveGroundColor(Mat srcMat, int flag);
	// 获取 QIN QPL 同时获得 QIN QPL 标记
	int getQINQPLIdentify();

	//获得 场次号 

	int getSessionIdentify();
	//获得倒计时 

	int getCountDownIdentify();

	//判断 WIN PLA是否有小数点
	bool judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge);							// DataIdentify dot for region 1 and region 2 (QPL), i = 1, region 1, i = 2, region 2(QPL)
	// 判断 QIN QPL是否有小数点
	int judgeQINQPLDot(Mat &roi, Mat &edge, int *x);
	//判断是否是QIN 或者QPl标签
	int judgeQINQPL();

	// 主要是将图像的边缘空余部分减掉，主要用于QIN QPL 数据处理
	int  trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize);

	//对彩色图像进行阈值 
	int colorThreshold(Mat &src, Mat &dst, int thereshold);
	// return 0-- two number with dot,  1-- two number without dot, 2-- three number	
	int calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum);
	int calculateXBewttenNumber(Mat &mat, int *x);
	int calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum);
	//计算 3位数每个数的X坐标，然后用于数字分割 QIN QPL
	int calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum);


	int calculateGraySumXForSetQINQPLRect(Mat &mat, int *y, int roiNum);
	int calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum);

	//描述算法状态 

	//测试用代码
	void createClassifySamples(float result, Mat &singleNum);
	void writeSamples(QString fileName, Mat &roi, QString path);

	int sampleCount[10];



};



//定义原点位置为 

#define ORIGIN_X_BASE_LIVE 60

#define ORIGIN_Y_BASE_LIVE  37 
//场次号位置
#define SESSION_POS_RECT_LIVE cvRect(267+(originX-ORIGIN_X_BASE_LIVE),38+(originY-ORIGIN_Y_BASE_LIVE),285-267,60-38)
//分钟位置
// 十位数
#define  COUNTDOWNMINUTE_POS_RECT1_LIVE cvRect(560+(originX-ORIGIN_X_BASE_LIVE),41+(originY-ORIGIN_Y_BASE_LIVE),10,16)
//个位数
#define  COUNTDOWNMINUTE_POS_RECT2_LIVE cvRect(569+(originX-ORIGIN_X_BASE_LIVE),41+(originY-ORIGIN_Y_BASE_LIVE),10,16)

// WIN PLA
#define  WIN_POS_RECT_LIVE cvRect(136+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)
// PLA 
#define  PLA_POS_RECT_LIVE cvRect(216+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),38,363-65)

//QIN QPL标签位置 
#define  QINQPL_LABEL_POS_RECT_LIVE cvRect(91+(originX-ORIGIN_X_BASE_LIVE),377+(originY-ORIGIN_Y_BASE_LIVE),50,20 )

// QIN QPL 位置
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

// 马名位置 
#define HORSENAME_REGION_RECT_LIVE cvRect(69+(originX-ORIGIN_X_BASE_LIVE),65+(originY-ORIGIN_Y_BASE_LIVE),134-69,370-65)



#define  NUMBER_HEIGHT_LIVE 15 
#define  NUMBER_WIDTH_LIVE 36


#endif
