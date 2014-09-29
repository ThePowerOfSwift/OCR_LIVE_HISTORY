/**
*
* 项    目:  PET/CT软件
* 文件名称:  Global.h
* 文件路径： Include/Global.h
* 摘    要:  全局类、初始化数据库、系统日志线程、网络通信、全局静态变量
* 作    者： 刘兆邦
* 完成日期： 2012年09月05日
* Copyright (c) 2011-2012, 中科院苏州医工所医学影像室电子组.All rights reserved.
* Version v2.0
*/
//为了使QString中的'+'，被当做QStringBuilder的'%'执行，提高效率
#ifdef QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_CONCATENATION
#endif
#ifdef QT_USE_FAST_OPERATOR_PLUS
#define QT_USE_FAST_OPERATOR_PLUS
#endif
// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef moni_test 
//若Simulation_Test=1，表示模拟测试，Simulation_Test=0表示正式采集
#define moni_test 1
#endif
//下面Qt4.8之后的方法
//#ifdef QT_USE_QSTRINGBUILDER
//#define QT_USE_QSTRINGBUILDER
//#endif


#ifndef Global_H
#define Global_H
#include <QObject>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

#include "Net/Network.h"
#include "HorseDataStruct.h"
#include "CCycleBuffer.h"
#include "Acq/ThreadAcq.h"
#include "Acq/IAcq.h"


#include "Bll/SystemLog/SystemLogThread.h"
#include "Bll/SystemLog/SystemLog.h"

#define  IMAGE_WIDTH 720 
#define  IMAGE_HEIGHT 576 
#define BMP_HEADER 0 
#define	 IMAGE_BUFF_LENGTH (720*576*3 + BMP_HEADER )

//"Server", "58.67.161.109", 9068);
/* 
#define  Global::serverIpAddr QString("58.67.161.109")
#define  Global::serverPort 9068 
*/
//视频种类
 
enum videoKinds
{
	LIVE = 0 ,
	YAZHOUTAI = 1 ,
	HK18TAI = 2 ,
	HK18D14 = 3 

};

class Global : public QObject
{
	Q_OBJECT
public:
	Global(QObject *parent = 0);
	~Global();


	//PET/CT网络设备Id
	//控制台：0001H，主控制器： 0002H，CT影像工作站： 0003H，PET影像工作站0004H，控制盒0005H
	enum ComputerId {
		//Control System(控制台子系统)
		CS = 0x0001,
		//Main Control System(主控制器子系统)
		MCS = 0x0002,
		//CT-Acquisition System(CT采集重建子系统)
		CT_ARS = 0x0003,
		//PET-Acquisition System(PET采集重建子系统)
		PET_ARS = 0x0004,
		//Control Box(控制盒)
		CB = 0x0005
	};

	//**********控制台网络客户端接口*********//
	//控制台相对于主控制器、CT-ARS、PET-ARS都是客户端，主动发起连接者.
	//主控制器网络通讯接口类
	//static Network *mcsNetClient;
	//全局客户端状态
	//static ClientCmdStatus client_cmd_status;

	//**********控制台网络客户端接口*********//

	 
	//全局缓冲区 10 幅图像大小 12.4MB
	static CCycleBuffer* S_CCycleBuffer;

	//  发送数据缓存区 

	static CCycleBuffer * sendDataCCycleBuffer;


	//第二条线路

	static CCycleBuffer *sendDataCCycleBuffer1;

	//获取计算机设备名称，参数ComputerId枚举型
	static QString getComputerDeviceName(Global::ComputerId);
	//获取计算机设备名称，参数int
	static QString getComputerDeviceName(int computerId);
	

	//直播采集卡视频种类
	static QString liveCardVideoSource;


	static IAcq* myIAcq;//采集类
	static ThreadAcq* threadAcq;//采集线程
	static bool stopDataIdentifyTag;//模拟采集标志符


	static bool pauseDataIdentifyTag;//识别标识符
	// 服务器ip地址，端口号

	static QString serverIpAddr;
	static qint32  serverPort;

	//第二条线路
	static QString serverIpAddr1;
	static qint32 serverPort1;

	/***********比赛数据***********/
	static qint32 raceId;//比赛唯一识别ID，服务端获得
	static QList<TagHorseInfo> horseInfoList;//赛马信息：HorseID，HorseName，服务端获得
	static qint32 session;//场次号
	static qint32 raceTime;//比赛时间 倒计时 
	static qint32 timerCount; //计时
	static qint32 countRaceTime; //从当前场开始的时候计时 


	//标记 是否正在进行 历史视频 识别

	static bool isHistoryVideoIdentifyRuning;
	//是不是历史视频 标记 

	static bool isHistoryVideo;
	// 计算 一场比赛视频的帧数

	static qint32 historyFrameCount;

	//通过识别场次号，请求 服务器 全局场次号

	static qint32 requestRaceId; 
	

	// //比赛已经开始了 标志位
	static qint32 raceHasStarted; 
	
	
	//最长不超过14场的
	static bool isThisSessionRaceIDRequested[14];
	//保存请求到的场次号
	static bool requestedRaceID[14];
	//保存每一场的所有时间

	static int totalSessionTime[14] ;

	static bool isThisTotalSessionTimeSumbit[14];

	//保存总计时 

	static int oneSessionTotalRaceTime;
	// 是否 用户矫正了 场次号
	static bool isSessioncalibrated;
	
	// 是否场次号变化，如果变化则 请求新场次号，然后将其置为 FALSE

	static bool isSessionChanged;

	//是否时间相关数值被矫正，倒计时，顺计时

	static bool isCountTimeCalied;
	//历史视频比赛日期

	static QString historyVideoDate;

	//存取识别的历史数据
	static QFile historyIdentifyDataFile;

	static QDataStream historyIdentifyDataWS;

	//当前视频是否是63台需要识别的数据
	static bool is63TAIVideoData;

	//是否锁定了矫正

	static bool islockCali;
	/***********比赛数据***********/

	//历史视频 起始帧位置

	static qint32 videoStartPos;
	//识别视频类型

	static qint32 videoType;

	// 历史视频文件 记录 快进

	static qint32 frameAccValue;

	//标记暂停时候如果请求快进也是可以允许的

	static bool requestAdvanceDuringPause;
	///系统日志线程
	static SystemLogThread * systemLogThread;
	///系统日志接口类
	static SystemLog *systemLog;
	//全局初始化
	void init();
	/**
	* @brief qss初始化
	*/
	void Global::initQss();
	public slots:

signals:
	//初始化采集
	void initAcq();
	//开始采集
	void startAcq();

private:
};

enum QAbstractSocketState
{
	UnconnectedState = 0,
	HostLookupState = 1,
	ConnectingState = 3,
	BoundState = 4,
	ClosingState = 6,
	ListeningState = 5
};

#endif // Global_H