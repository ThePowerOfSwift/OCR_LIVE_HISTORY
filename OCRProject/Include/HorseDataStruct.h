#ifndef HORSEDATASTRUCT_H
#define HORSEDATASTRUCT_H
#define HORSENUMBER_1	 14
#define REALHORSENUM    12  // 郁朋 添加
#define QIN_QPL_ROW_1	 7
#define QIN_QPL_COL_1	15
//#include <tchar.h>
#include "./Bll/algorithmParam.h"
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "opencv/ml.h"




enum  DATATYPE
{
	WINTYPE = 1,
	PLATYPE = 2,
	QINTYPE = 3,
	QPLTYPE = 4

};


typedef struct TagProtocolMsg
{
	int	MsgID;			//消息类型
	int nDataSize;		//后面发送的数据大小，后面无数据
	char Param[51];		//消息参数，多参数以\n分隔
	char Check[5];
}PROTOCOLMSG, *LPPROTOCOLMSG;
//马信息内存结构体描述
typedef struct TagHorseInfo
{
	int	HorseID;	 //马的唯一编号
	char HorseName[50]; 	//马名
}HORSEINFO, *LPHORSEINFO;

//赛程信息内存结构体描述
typedef struct TagPlanInfo
{
	char RaceDate[12];  //比赛日期
	int	RaceCount; //当日场数
}PLANINFO, *LPPLANINFO;

//赛事信息内存结构体描述
typedef struct TagRaceInfo
{
	int 	RaceID;	 //赛事ID
	int	RaceNO;	 //当日场次
	char RaceDate[12];	//比赛日期
	char RaceTime[10]; //比赛开始时间
	int 	CountTime; 	//比赛进行多少分钟
	int 	HorseCount; 	//参赛马数
}RACEINFO, *LPRACEINFO;

//WIN和PLA数据结构体描述
typedef struct TagWPDataInfo
{
	int  RaceID; //所属赛事ID
	int  HorseID; //马的唯一编号可关联马信息表
	int  HorseNO; //本场比赛中马的序号，比如第3号马
	float  WinValue; //数据值
	int AtTime; //所在时刻，正式开始比赛的时间,分钟单位
}WPDATAINFO, *LPWPDATAINFO;

//QIN和QPL数据结构体描述
typedef struct TagQDataInfo
{
	int  RaceID; //所属赛事ID
	int  HorseID; //马的唯一编号可关联马信息表
	int  HorseNO; //本场比赛中马的序号，比如第3号马
	float  QinValue; //数据值，由XNO与YNO组合得出
	int  YNO; //在Y轴上的第几号，跟它组合得出的数据
	int  AtTime; //所在时刻，正式开始比赛的时间,分钟单位
}QDATAINFO, *LPQDATAINFO;



//客户端命令状态
enum ClientCmdStatus {
	no_status = 1,
	//连接状态
	connect_status = 2,
	//连接状态
	disconnect_status = 3,
	//登陆状态
	login_status = 4,
	//请求马信息表状态
	request_horse_status = 5,
	//请求race_id状态
	request_race_id_status = 6,
	//提交赛长状态
	submit_race_time_status = 7,
	//提交实时数据状态
	submit_real_data_status = 8
};


//是否图片上数据发生了变化
enum	dataChangeEnum {
	WIN_CHANGED = 0x001,
	PLA_CHANGED = 0x002,
	QIN_QPL_CHANGED = 0x004,

	WIN_PLA_CHANGED = WIN_CHANGED | PLA_CHANGED,
	WIN_QIN_QPL_CHANGED = WIN_CHANGED | QIN_QPL_CHANGED,
	PLA_QIN_QPL_CHANGED = PLA_CHANGED | QIN_QPL_CHANGED,
	WIN_PLA_QIN_QPL_CHANGED = WIN_PLA_CHANGED | QIN_QPL_CHANGED

};

//场次号 以及倒计时 结构体
struct  raceNumTimeStruct 
{
	int content ;
	int contentCount ;
};



struct HorseInfo
{
	 
	int  horseID[HORSENUMBER];				// the horse ID
	bool isSCR[HORSENUMBER];				//退赛 标志位
	//马名的灰度和
	int  graySum[HORSENUMBER];				//灰度值和
	//名字长度
	int length[HORSENUMBER];

	wchar_t horseName[HORSENUMBER][5]; //名字 。

 //	QString horseName[HORSENUMBER];
};

// WIN PLA
struct WINPLAPosStruct
{
	int deltaX, deltaY;					// the x-shift and y-shift of image region 1 relative to the image origin
	int shiftX, shiftY;
	int height, width[3];
	CvRect rect[HORSENUMBER][3];						// the characters regions
};
//QIN QPL 
struct QINQPLPosStruct
{
	int deltaX, deltaY;					// the x-shift and y-shift of image region 2 relative to the image origin
	CvRect rect[QIN_QPL_ROW][QIN_QPL_COL];
	CvRect QPL_QINRect;
};
// 分钟
// 个 位 ，十 位 
struct CountDownMinutePosStruct
{
	int deltaX, deltaY;
	CvRect rect[2];
};
//  场次号 预留两个Rect 
struct SessionPosStruct
{
	int deltaX, deltaY;
	CvRect rect[2];
};

// 开辟14个Rect 马名 
struct HorseNamePosStruct
{
	int deltaX, deltaY;
	CvRect rect[HORSENUMBER];
};
enum ALGO_STATE {
	EXIT_THIS_OCR = -1, // 退出此次识别 
	EXEC_SUCCESS = 1  ,	//执行成功，运行正常
	EXEC_FAILED = -1 ,
	EXEC_WARNING = -2 
};

struct HorseDataStruct
{
	float dataValue; // 识别数据
	bool  byPass; //是否不识别,用于rect 设置有问题，所以忽略

	CvRect mRect;
};

struct dataSendStruct {

	bool winDataSendSuccess;
	bool plaDataSendSuccess;
	bool qinQPLDataSendSucess;
};


struct DataOutput
{
	//存储马名信息，马的id，序号，以及是否退赛等信息
	HorseInfo mHorseInfo;
	// image region 1
	int horseNum;									// the total horse number in the current video
	int svmResult[HORSENUMBER];						// the index of the house name in the current sample
	int nameType[HORSENUMBER];						// 2: the horse name contains 2 char, 3: the name contains 3 char, 4: 4char

	//通过检测马名字灰度值变化来决定是否发生了场次变化
	int horseNameChangedNum;

	//float WIN[HORSENUMBER];
	//float PLA[HORSENUMBER];
	HorseDataStruct WIN[HORSENUMBER];
	HorseDataStruct PLA[HORSENUMBER];

	//标记改变
	bool winChangedFlag[HORSENUMBER];
	bool plaChangedFlag[HORSENUMBER];

	//标记 是否用户矫正
	bool winCaliFlag[HORSENUMBER];
	bool plaCaliFlag[HORSENUMBER];

	//用于标记是否该位置的WIN  PLA 有底色。
	bool  isWinPlaHasGroundColor[HORSENUMBER][2];
 
	// image region 2
	bool isQPL;		 	// true: QPL, false: QIN

	//float QPL[QIN_QPL_ROW][QIN_QPL_COL];
	//float QIN[QIN_QPL_ROW][QIN_QPL_COL];

	HorseDataStruct QPL[QIN_QPL_ROW][QIN_QPL_COL];
	HorseDataStruct QIN[QIN_QPL_ROW][QIN_QPL_COL];

	//标记改变
	bool qplQinChangedFlag[QIN_QPL_ROW][QIN_QPL_COL];

	//标记是否矫正

	bool qplQinCaliFlag[QIN_QPL_ROW][QIN_QPL_COL];
	//用于标记是否该位置的qpl qin 有底色。
	bool  isQplQinHasGroundColor[QIN_QPL_ROW][QIN_QPL_COL];
	// image region 3
	int session;
	//倒计时时间
	int raceTime;

	// 检查是否有数据

	bool haveDataFlag;
	//检测结果改变情况
	int changeStatus;
	// 发送状态
	int sendStatus;
	//比赛时长  一个场次的
	int countRaceTime;
	// onceRaceTIme+倒计时 
	int totoalRaceTime;

	//历史视频处理 百分比 

	int videoProgressPercent;


	//标记 场次号发生变化 ，只显示一次

	bool sessionChangedFlag;


	dataSendStruct mDataSendFlag ;
};


struct LoginStruct {
	QString serverAddr;
	int ipPort ;
};

#endif // HORSEDATASTRUCT_H
