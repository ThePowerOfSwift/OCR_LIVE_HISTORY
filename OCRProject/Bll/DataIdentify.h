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
 
 

class DataIdentify
{
public:
	DataIdentify();
	~DataIdentify();

	// 对外的接口参数
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
	// 获取 WIN PLA 识别

	int getWINPLAIdentify();

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
	void writeSamples(int i, int j, int k, Mat &roi);

	int sampleCount[10];



};

 

//场次号位置
#define SESSION_POS_RECT cvRect(267,38,285-267,60-38)
//分钟位置
// 十位数
#define  COUNTDOWNMINUTE_POS_RECT1 cvRect(567,37,575-567,54-37)
//个位数
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(576,37,587-576,54-37)

// WIN PLA
#define  WIN_POS_RECT cvRect(135,65,175-135,363-65)
// PLA 
#define  PLA_POS_RECT cvRect(216,65,256-216,363-65)

//QIN QPL标签位置
#define  QINQPL_LABEL_POS_RECT cvRect(99,378,139-99,400-378)

// QIN QPL 位置
#define		WHOLE_QINQPL_POS_RECT  cvRect(9, 376, 697, 197)
#define		QINQPL_POS_RECT cvRect(69, 22, 563, 137)
 
#define  LB_REGION1_RECT cvRect(0, 19, 43, 116)
#define  LB_REGION2_RECT cvRect(42, 38, 36, 99)
#define  LB_REGION3_RECT cvRect(77, 56, 38, 79)
#define  LB_REGION4_RECT cvRect(114, 74, 38, 62)
#define  LB_REGION5_RECT cvRect(149, 95, 186 - 149 + 1, 40)
#define  LB_REGION6_RECT cvRect(188, 111, 225 - 188 + 1, 24)

#define RU_REGION1_RECT  cvRect(81, 0, 36, 21)
#define RU_REGION2_RECT  cvRect(117, 0, 38, 40)
#define RU_REGION3_RECT  cvRect(154, 0, 190 - 154 + 1, 60)
#define RU_REGION4_RECT  cvRect(190, 0, 227 - 190 + 1, 78)
#define RU_REGION5_RECT  cvRect(227, 0, 263 - 227 + 1, 96)
#define RU_REGION6_RECT  cvRect(263, 0, 302 - 263 + 1, 115)

#define R_REGION1_RECT  cvRect(300, 0, 36 + 1, 136)
#define R_REGION2_RECT cvRect(336, 0, 36, 136)
#define R_REGION3_RECT cvRect(373, 0, 37, 136)
#define R_REGION4_RECT cvRect(409, 0, 447 - 409 + 1, 136)
#define R_REGION5_RECT cvRect(447, 0, 481 - 447 , 136)
#define R_REGION6_RECT cvRect(481, 0, 36 , 136)
#define R_REGION7_RECT cvRect(517, 0, 557 - 519 , 136)

// 马名位置 
#define HORSENAME_REGION_RECT cvRect(69,65,134-69,370-65)

 
