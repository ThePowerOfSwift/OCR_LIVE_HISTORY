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

 
	//dataOutput里面包含了mHorseInfo
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
	//马名字的区域
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

	int calculateGraySum(Mat srcMa, int &length );
	// 获取 WIN PLA 识别

	int getWINPLAIdentify();

	// 判断是否有底色 绿色底色 flag =1 为win pla ，flag = 2 为qpl qin

	bool  haveGroundColor(Mat srcMat,int flag );

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
	int calculateXBewttenNumber(Mat &mat, int *x);
	int calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum);
	//计算 3位数每个数的X坐标，然后用于数字分割 QIN QPL
	int calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum);


	int calculateGraySumXForSetQINQPLRect(Mat &mat, int *y, int roiNum);
	int calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum);
	
	//描述算法状态 

	//测试用代码
	void createClassifySamples(float result, Mat &singleNum);
	void writeSamples(QString fileName,Mat &roi,QString path );
	
	int sampleCount[10];
 
 		 
  
};

// 香港63的历史数据，其中画面大小 640*480 
 
// 修改各个区域位置 为WMV文件 。



/*
	由于下列位置坐标均使用的是绝对坐标，所以 需要每次计算远点坐标，然后进行校正
	因为同一个版式的画面，但是画面会有X 向 与Y向上的偏移。


*/

//定义原点位置为  以2010年 10.1 号视频作为基准 ，画面大小 704*576

#define ORIGIN_X_BASE_HK63D14 27
#define ORIGIN_Y_BASE_HK63D14 34


//场次号位置
//#define SESSION_POS_RECT_HK63D14 cvRect(240+(originX-ORIGIN_X_BASE_HK63D14),37+(originY-ORIGIN_Y_BASE_HK63D14),259-240,61-37)
//分钟位置

//场次号位置
#define SESSION_POS_RECT_HK63D14 cvRect(82+(originX-ORIGIN_X_BASE_HK63D14),27+(originY-ORIGIN_Y_BASE_HK63D14),27,29)
//分钟位置

// 历史视频不需要识别倒计时
// 十位数 
#define   COUNTDOWNMINUTE_POS_RECT1_HK63D14 cvRect(495+(originX-ORIGIN_X_BASE_HK63D14),32+(originY-ORIGIN_Y_BASE_HK63D14),10,15 )

//个位数
#define  COUNTDOWNMINUTE_POS_RECT2_HK63D14 cvRect(504+(originX-ORIGIN_X_BASE_HK63D14),32+(originY-ORIGIN_Y_BASE_HK63D14),10,15)

// WIN PLA
#define  WIN_POS_RECT_HK63D14 cvRect(57+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),40,371-69)
// PLA 
#define  PLA_POS_RECT_HK63D14 cvRect(121+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),35,371-69)
// 马名
#define HORSENAME_REGION_RECT_HK63D14 cvRect(20+(originX-ORIGIN_X_BASE_HK63D14),69+(originY-ORIGIN_Y_BASE_HK63D14),35,371-69)
//QIN QPL标签位置
#define  QINQPL_LABEL_POS_RECT_HK63D14 cvRect(59+(originX-ORIGIN_X_BASE_HK63D14),377+(originY-ORIGIN_Y_BASE_HK63D14),19,19)

//QIN QPL
#define		WHOLE_QINQPL_POS_RECT_HK63D14_HK63D14  cvRect(100+(originX-ORIGIN_X_BASE_HK63D14), 371+(originY-ORIGIN_Y_BASE_HK63D14), 16, 17)
//QIN QPL 方框 绝对坐标
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