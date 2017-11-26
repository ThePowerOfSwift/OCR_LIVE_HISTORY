// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef ACQDRIVER_H
#define ACQDRIVER_H
#pragma once
#include <QtWidgets/QMainWindow>
#include <qt_windows.h>
#include "AVSDK/AVerCapAPI.h"
#include "Include/CCycleBuffer.h"
#include <QTimerEvent>
#include <QFile>
#include <QTextStream>
#include "bll/ReadHistoryVideo.h"

/*
版本说明。
2017.10.8 
1. 将720x456 采集卡分辨率 提高至1280x720 。
2. 使用sdk的hdmi接口

*/
  #define	OFFLINE_DEBUG 
//#define  ONLY_SAVE_IMAGES

#ifdef OFFLINE_DEBUG
#define  VERSION_NUM "版本:1.0 2017/11//06. 离线模式 yp " // 


#else
#define  VERSION_NUM "版本:1.0  2017/11//06.  直播模式 yp " //
// 0906 更改了部分板式，主要是win pla 变窄了
/*
修改了 原点 阈值 ，由25 降低到 8 ，为了防止第一行有马匹 退赛。
*/
#define WRITE_IMAGES_BEFORE_DataIdentify
#endif

//#define CALLBACK_MODE 
#define TIME_CYCLE 1000  // 2s 

class AcqDriver :public QObject
{
	Q_OBJECT
private:
	char * pbData;
	QImage localImage;
	// 设备句柄
	HANDLE hSDCaptureDevice;
	WCHAR wcDeviceName[MAX_PATH];
	DWORD dwDeviceNum;
	DWORD dwDeviceIndex;
	VIDEO_SAMPLE_INFO * cpVideoInfo;
	BYTE * imageDataBuf;
	uchar * bmpImageBits;
 
	LONG imageDataBufLeng;

	VIDEO_CAPTURE_INFO VideoCaptureInfo;

	void  captureSingleImage() ;

#ifdef OFFLINE_DEBUG
	void createFalseData();
	//真正的定时器
	
	int count;
	QString fileName;
#endif
	//计时器
	int timerCount;
public:
	AcqDriver(QObject *parent = 0);
	~AcqDriver();
	// 非callback模式下，停止单幅图片采集 标志位，停止while循环
	BOOL stopCaputureFlag ;

	// 初始化驱动
	LONG init();

	// 打开设备
	LONG open();

	// 读取图像数据 RGB888
	LONG read(char * buffer, LONG * size);

	//关闭设备 ，释放内存
	LONG close();
	// 环形缓冲区相关

	LONG getCcbUsedSize();

	LONG ccbRead(char * buffer, int size);
	//call back 函数
	void writeData(VIDEO_SAMPLE_INFO VideoInfo, BYTE *pbData, LONG lLength, LONG lUserData);
	
	static BOOL WINAPI readBmpData(VIDEO_SAMPLE_INFO VideoInfo, BYTE *pbData,
		LONG lLength, __int64 tRefTime, LONG lUserData);

 
	QString testImgFileName ;

	QFile liveTestConfigFile ;

	QString liveTestConfigStr;
	QTextStream liveTestConfigTs;
protected:
	//定时器
	int m_timerId;
	virtual void timerEvent(QTimerEvent *event);




};

#endif