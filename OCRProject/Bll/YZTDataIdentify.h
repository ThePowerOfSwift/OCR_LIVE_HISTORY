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
	int judgeQINQPLDot(Mat &roi, Mat &edge ,int *x);
	//判断是否是QIN 或者QPl标签
	int judgeQINQPL();

	// 主要是将图像的边缘空余部分减掉，主要用于QIN QPL 数据处理
	int  trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize);

	//对彩色图像进行阈值 
	int colorThreshold(Mat &src, Mat &dst, int thereshold);
	// return 0-- two number with dot,  1-- two number without dot, 2-- three number	
	int calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum);
	//计算间隔
	int calculateXBewttenNumber(Mat &mat, int *x);
	//用于判断小数点 通过计算 低于某个阈值的 空白区间大小
	int calculateXBewttenNumberForJudgeDot(Mat &mat, int  *x);
	int calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum);
	//计算 3位数每个数的X坐标，然后用于数字分割 QIN QPL
	int calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum);


	int calculateGraySumXForSetQINQPLRect(Mat &mat, int *y, int roiNum);
	int calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum);
	
	//描述算法状态 

	//测试用代码
	void createClassifySamples(float result, Mat &singleNum);
	void writeSamples(int i,int j, int k,Mat &roi );
	
	int sampleCount[10];
 
   
  
};

 

// 亚洲台 版式数据
//场次号位置
#define SESSION_POS_RECT cvRect(50,22,21,19)
//分钟位置

// 十位数 
#define   COUNTDOWNMINUTE_POS_RECT1 cvRect(163,22,12,18 )

//个位数
#define  COUNTDOWNMINUTE_POS_RECT2 cvRect(174,22,12,18)

// WIN PLA
#define  WIN_POS_RECT cvRect(141,78,41,312-78)
// PLA 
#define  PLA_POS_RECT cvRect(181,78,40,312-78 )
// 马名
#define HORSENAME_REGION_RECT cvRect(76,78,128-76,312-76)
//QIN QPL标签位置
#define  QINQPL_LABEL_POS_RECT cvRect(225,327,247-225,340-327)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT  cvRect(12, 314, 624-12, 476-314)
//QIN QPL 方框 绝对坐标
#define		QINQPL_POS_RECT cvRect(256, 345, 601-256, 445-328)
// 14 匹马

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

// 12 匹马

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




//检测是否为广告相关 宏定义 

#define   NUMBER_HEIGHT 13 
#define   ADCHECK_WIN_ORIGIN_X  16
#define   ADCHECK_WIN_ORIGIN_Y  80

#define  ADCHECK_QIN_ORIGIN_X 618
#define  ADCHECK_QIN_ORIGIN_Y 330


#endif