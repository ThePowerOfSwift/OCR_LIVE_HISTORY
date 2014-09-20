#ifndef BllDataIdentify_H
#define BllDataIdentify_H

#include <QObject>
#include "Include/Global.h"

#include <QFile>
#include <QTimerEvent>
#include "readHistoryVideo.h"

#include "DataIdentify.h"
#include "Bll/HK18DataIdentify.h"
#include "Bll/YZTDataIdentify.h"

#include "Bll/HK18D14DataIdentify.h"
#include "Bll/HK63D14DataIdentify.h"

//#define  CALLBACK_MODE
#define WRITE_IMAGES_BEFORE_DataIdentify



/*
算法色彩说明 
进入DataIdentify read函数的时候，色彩为反色。
进入have data 函数为了识别广告，先将色彩 cvtColor反一下
然后识别完广告，再反回来。
yp 20140701
*/ 
class BllDataIdentify : public QObject
{
	Q_OBJECT

public:
	BllDataIdentify(QObject *parent = 0 );
	~BllDataIdentify();
	
	// 
	QFile logFile;


	void writeDataFile(DataOutput &dataOutput);
public slots:
	//初始化算法
	void init();

	//开始识别
	void start(QString fileName, int videoType );

 
	//停止识别
	void stop();

 

signals:
	void readyRead(DataOutput output, QByteArray byteArray,int imageWidth ,int imageHeight);
	//
	void readyReadBmp(DataOutput output, QByteArray byteArray,int imageWidth, int imageHeight);
	//本次读取的历史文件已经处理完毕，读取下一个文件
	void readNextFile();
	// 场次号发生了变化，请求服务器 本场次号对应的raceID
	void requestRaceIdSig(int session);


	// 提交比赛时长 
	void submitRaceTimeSig(qint32 raceTime);
	 
	// 发送 缓存区数据 信号量
	void sendBufferDataSig();

private:
	//DataIdentify DataIdentify;//识别算法
	/**
	* @brief 检测 是否图片数据发生变化
	*/
	LONG  isDataOutputNew(DataOutput &outputStruct);
	/**
	* @brief WIN PLA数据除以10 
	*/
	LONG  winPlaDivTen(DataOutput &outputStruct);
	/**
	* @brief 从多次识别出的倒计时、场次号中选出正确的
	*/
	LONG chooseRightRaceTimeRaceSession(DataOutput &outputStruct);

	int saveToLocalFile(char *data, QString path);


	int startHistoryDataIdentify(QString fileName, int videoType );
	int startLiveDataIdentify(int videoType);


	int algorithmExecHistory(int videoType,uchar * imageBuf, Mat &srcMat,int progressPercent);

	int algorithmExecLive( int videoType, uchar * imageBuf, Mat &srcMat);


	/*
	通过文件名，以及场次号 获取马名 
	*/

	void getHorseNameFromDataFileLive(QString fileName, DataOutput &outputStruct);

	void getHorseNameFromDataFileHistory(QString fileName, DataOutput &outputStruct);


	/*
		写历史文件
	*/
	void writeHistoryData(DataOutput &dataOutput);
	/*
		当历史视频的日期发生了变化，那么所有的相关全局变量都要初始化
	*/

	void initGlobal();
	/*
	将要发送的数据写入环形buffer

	*/
	void writeOutputDataIntoBuffer(DataOutput &dataOutput);


	DataOutput priDataOutput;

	//live  香港18台 直播 720*576
	DataIdentify  myDataIdentify;
	// 香港18台 历史数据 640*480
	HK18DataIdentify  mHK18DataIdentify;
	// 亚洲台 历史数据 640*480 
	YZTDataIdentify  mYZTDataIdentify;
	DataIdentify    dataIdentifyClass;

	HK18D14DataIdentify mHK18D14DataIdentify;

	HK63D14DataIdentify mHK63D14DataIdentify;

	bool pauseDataIdentifyTag;//标识符


	ReadHistoryVideo myReadHistoryVideo;


	// 储存比赛结果信息  最大支持场次号20场  倒计时 30 min
	struct raceNumTimeStruct  myRaceNumberStruct[20];
	struct raceNumTimeStruct  myRaceTimeStruct[30];
	// 数据更新次数统计
	int		dataNewCount; 

	//用于排除掉 QPL QIN切换时候 ，数据没有完全切换的数据发送
	//通过检测WIN PLA数据大量变化，一半以上发生变化，此时 sessionChanged持续3个定时器周期为true 
	bool sessionChanged;
	bool sessionChangedDly1;
	//保持3个定时器周期计数器
	int sessionChangedCountDown; 

	//保持3个定时器周期的 用于QIN QPL 改变的定时器

	int QINQPLChangedCountDown;
	//通过检测QIN QPL数据大量变化，一半以上发生变化，要么是场次号变化了，要么是QIN QPL发生了切换
	bool QINQPLTransformed;
	//保持3个定时器周期
	int QINQPLTransformedCountDown;

	//比赛已经开始了 true 为开始了，即比赛已经开始
	bool raceHasBegan;
	//所有场次号计数
	int raceSessionCount;
	//比赛倒计时接近9 了，当前为10 
	bool raceTimeCountDownNear9;
	//是否第一次检测到了 场次号 标志

	bool isRaceSessionDetected;

	//第一次检测到的场次号

	int firstRaceSessionDetected;


	//每一场次号，都需要检测一下 ，第一次检测到的倒计时是否大于10分钟，
	//如果小于10min，则是错误的。因为每次都是大于10 min的。

	bool isRightRaceTimeCountDownDetected;
	
#ifdef WRITE_IMAGES_BEFORE_DataIdentify
	int  bmpCount;
#endif

	// 历史数据增量

	int frameAccValue;

 
	QString videoFileName;
	//文件 
	 
	QFile horseNameHistoryDataFile;
	QFile horseNameIdDataFile;

	QTextStream horseNameHistoryData;
	QTextStream horseNameIdData;

	QString horseNameHistoryStr;
	QString horseNameIdStr;

	QFile liveHorseNameFile;

	QTextStream liveHorseNameTs;
	QString liveHorseNameStr;


	//保存马名

	QStringList horseNameList;

	QList <int> horseIdList;

	QString videoFileDate;
	

	//标记 QIN QPL 发生改变。
	bool isQINQPLTransformed;
	 
	// 倒计时  
	int  raceCountDownTime;

	//
	QByteArray notRequestRaceIdSession;

	bool bmpCountIsSet;
	

};

#endif // BllDataIdentify_H
