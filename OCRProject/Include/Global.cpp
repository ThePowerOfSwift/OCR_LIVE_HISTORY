#include "Global.h"

#include "Model/SystemLog/MoSystemLog.h"
Global::Global(QObject *parent)
	: QObject(parent)
{
      
}

Global::~Global()
{
	if (myIAcq != 0)
	{
		//myIAcq->unLoadDevice();
		delete myIAcq;
		myIAcq = 0;
	}
	if (mcsNetClient!=0)
	{
		delete mcsNetClient;
		mcsNetClient = 0;
	}

	//if (S_CCycleBuffer != 0)
	//{
	//	delete S_CCycleBuffer;
	//	S_CCycleBuffer = 0;
	//}
}

//主控制器网络通讯接口类
Network *Global::mcsNetClient;
//全局客户端状态
ClientCmdStatus Global::client_cmd_status;
//全局缓冲区
CCycleBuffer* Global::S_CCycleBuffer = 0;

//发送数据缓存区
CCycleBuffer * Global::sendDataCCycleBuffer = 0;

//初始化采集
IAcq* Global::myIAcq = 0;
ThreadAcq* Global::threadAcq = 0;
bool Global::stopDataIdentifyTag = false;//模拟采集标志符,开始模拟:false,停止模拟true


bool Global::pauseDataIdentifyTag = false;//识别标识符


// 标记，是否为 实时直播

bool Global::isHistoryVideo = false;

// 历史视频正在进行识别标志

bool Global::isHistoryVideoIdentifyRuning = false;
//历史视频起始帧位置
qint32 Global::videoStartPos = 0;
//识别视频类型

qint32 Global::videoType = 0;

//历史视频文件，快进累加
qint32 Global::frameAccValue = 1 ;

//标记暂停时候的快进请求

bool Global::requestAdvanceDuringPause= false ;
 /***********比赛数据***********/
qint32 Global::raceId;//比赛唯一识别ID，服务端获得
QList<TagHorseInfo> Global::horseInfoList;//赛马信息：HorseID，HorseName，服务端获得
qint32 Global::session = 0;//比赛场次号

qint32 Global::raceTime;//比赛时间

// 请求到的全局场次号id
qint32 Global::requestRaceId = 0;

//提交失败 标志位

bool Global::serverSubmitFailed = false;

//标记状态

bool Global::serverNotConnected = false;
//本场场次号的全局id 已经请求 标志位

bool Global::isSessionRaceIdRequested = false;

bool Global:: requestedRaceID[14];

//比赛已经开始标志
qint32 Global::raceHasStarted = 0 ;
//比赛当前场次计时 
qint32 Global::countRaceTime = 0 ;

qint32 Global::timerCount = 0 ;


qint32 Global::historyFrameCount = 0;
// 是否用户校正了场次号
bool Global::isSessioncalibrated = false ;
//是否 场次号发生了变化

bool Global::isSessionChanged = false;
 // 保存请求到的场次号 
bool Global::isThisSessionRaceIDRequested[14];

// 保存比赛时长

int Global::totalSessionTime[14];
bool Global::isThisTotalSessionTimeSumbit[14];

//历史视频比赛日期

QString Global::historyVideoDate = "";

//是否锁定了矫正

bool Global::islockCali= false ;

//历史数据文件

QFile Global::historyIdentifyDataFile;


QDataStream Global::historyIdentifyDataWS;


bool Global::is63TAIVideoData = false ;

/***********比赛数据***********/



//系统日志线程
SystemLogThread * Global::systemLogThread = new SystemLogThread();
//系统日志
SystemLog * Global::systemLog = new SystemLog();
/**
 *刘兆邦-添加
 *函数介绍: 全局初始化
 *输入参数: 无
 *返回值: 无
 */
void Global::init()
{
	//系统日志线程启动
	QObject::connect(systemLog, SIGNAL(dataEntered(MoSystemLog)), systemLogThread, SLOT(saveSystemLog(MoSystemLog)));
	systemLogThread->start();

	//初始化样式表
	initQss();

	//服务器网络通讯接口
	mcsNetClient = new Network("Server", SERVER_IP_ADDRESS, SERVER_PORT );
	S_CCycleBuffer = new CCycleBuffer();
	sendDataCCycleBuffer = new CCycleBuffer();
	//写入系统日志
	systemLog->append(QString(tr("服务器网络通讯接口初始完毕")), tr("连接打开成功."), SystemLog::INFO_TYPE);

	//初始化采集
	myIAcq = new IAcq();
	//threadAcq = new ThreadAcq();
	//myIAcq->moveToThread(threadAcq);
	

	// 场次号 是否请求到了
	for (int i = 0; i < 14; i++)
	{
		Global::isThisSessionRaceIDRequested[i] = false;

		Global::requestedRaceID[i] = 0;

		Global::totalSessionTime[i] = 0;

		Global::isThisTotalSessionTimeSumbit[i] = false;
	}



}

/**
* @brief qss初始化
*/
void Global::initQss()
{
	//全局qss先清空
	//QString ss = qApp->styleSheet();
	//qApp->setStyleSheet("");
	QDir dir;
	QString current = dir.currentPath();
	QFile file("Resources/StyleSheet.qss");
	if (!file.open(QFile::ReadOnly))
	{

		QMessageBox msgBox;
		msgBox.setText("程序出现错误.");
		msgBox.setInformativeText("系统初始化错误，qss文件无法打开，请确认Resources/StyleSheet.qss文件是否存在。\
								  								  								   出错程序:/Include/Global.cpp的initQss()函数");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
	}
	QString styleSheet = file.readAll().data();
	//设定全局qss风格
	qApp->setStyleSheet(styleSheet);
}
/**
 *刘兆邦-添加
 *函数介绍: 通过计算机Id获取计算机设备名称，一般用于系统日志，界面书写
 *输入参数: 枚举类型Global::ComputerId
 *返回值: 无
 */
QString Global::getComputerDeviceName(Global::ComputerId computerId)
{
	QString deviceName;
	switch (computerId)
	{
	case 0x0001:
		deviceName = tr("控制台");
		break;
	case 0x0002:
		deviceName = tr("主控制器");
		break;
	case 0x0003:
		deviceName = tr("CT-ARS");
		break;
	case 0x0004:
		deviceName = tr("PET-ARS");
		break;
	case 0x0005:
		deviceName = tr("控制盒");
		break;
	default:
		deviceName = tr("未明设备");
	}
	return deviceName;
}
/**
 *刘兆邦-添加
 *函数介绍: 通过计算机Id获取计算机设备名称，一般用于系统日志，界面书写
 *输入参数: int,computerId
 *返回值: 无
 */
QString Global::getComputerDeviceName(int computerId)
{
	QString deviceName;
	switch (computerId)
	{
	case 0x0001:
		deviceName = tr("控制台");
		break;
	case 0x0002:
		deviceName = tr("主控制器");
		break;
	case 0x0003:
		deviceName = tr("CT-ARS");
		break;
	case 0x0004:
		deviceName = tr("PET-ARS");
		break;
	case 0x0005:
		deviceName = tr("控制盒");
		break;
	default:
		deviceName = tr("未明设备");
	}
	return deviceName;
}
