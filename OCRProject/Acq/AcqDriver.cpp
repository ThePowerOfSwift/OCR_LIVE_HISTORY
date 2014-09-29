#include "AcqDriver.h"
#include <QTimer>
#include "Include/Global.h"



AcqDriver::AcqDriver(QObject *parent)
: QObject(parent)
{
	hSDCaptureDevice = NULL;
	imageDataBufLeng = 0;
	imageDataBuf = NULL;

	cpVideoInfo = new VIDEO_SAMPLE_INFO;

	stopCaputureFlag = FALSE;

	testImgFileName =QString( "") ;

	pbData = new char[IMAGE_BUFF_LENGTH]();
	m_timerId = 0 ;//初始化
	timerCount = 0;

#ifdef OFFLINE_DEBUG
		
	count = 100  ;
#endif

	//打开配置文件
	liveTestConfigFile.setFileName("liveTest.Config");
	if (!liveTestConfigFile.open(QIODevice::ReadOnly | QIODevice::Text))
		qDebug("liveTestConfigFile open Failed \n");

	liveTestConfigTs.setDevice(&liveTestConfigFile);

	while (!liveTestConfigTs.atEnd())
	{
		liveTestConfigStr += liveTestConfigTs.readLine();

	}

}

 
#ifdef OFFLINE_DEBUG


void AcqDriver::createFalseData()
{
	if (count < 41000 )
		count += 1 ;
	else
		count = 0 ;


	fileName = QString(".bmp");

 
	fileName.prepend(QString::number(count, 10));
	//fileName.prepend(QString::number(4));
	
	fileName = (QString("3615.bmp") );
	//fileName.prepend(QString("G://BaiduYunDownload//liveImageData2//"));
	
	fileName.prepend(liveTestConfigStr);
	//fileName = (QString("G://BaiduYunDownload//12.bmp"));	
	
	localImage.load(fileName);
 
 	localImage = localImage.convertToFormat(QImage::Format_RGB888);
	pbData = (char *)localImage.bits();

	testImgFileName = fileName;
	QString logDataStr;
	logDataStr = QString("AcqDriver:第") + QString::number(count) + QString("帧模拟数据");

	//Global::AppendLogString( logDataStr,true);
	qDebug("AcqDriver 第%d 帧 图片.当前计时器运行次数为 %d \n ",count,timerCount);
	
	//写入系统日志
	//Global::systemLog->append(QString(tr("AcqDriver当前计时器运行次数为 ")), QString::number(count),
	//	SystemLog::INFO_TYPE);

	if (pbData == NULL)
	{
		logDataStr =  QString( " AcqDriver::createFalseData() :pbData is NULL err!");
		//Global::AppendLogString(logDataStr, true);
		 count += 1 ;
		return ;

	}
	Global::S_CCycleBuffer->write((char *)pbData, IMAGE_BUFF_LENGTH);

	cpVideoInfo->dwHeight = IMAGE_HEIGHT;
	cpVideoInfo->dwWidth = IMAGE_WIDTH;
	//delete[] pbData;

}

#endif


AcqDriver::~AcqDriver()
{
	hSDCaptureDevice = NULL;
	imageDataBufLeng = 0;
	imageDataBuf = NULL;

	dwDeviceNum = 0;
	dwDeviceIndex = 0;
	ZeroMemory(&cpVideoInfo, sizeof(VIDEO_SAMPLE_INFO));
	ZeroMemory(&VideoCaptureInfo, sizeof(VIDEO_CAPTURE_INFO));
	imageDataBufLeng = 0;

	if (cpVideoInfo != NULL)
	{
		delete cpVideoInfo;
		cpVideoInfo = NULL;
	}
	if (pbData != NULL)
	{
		delete pbData;
		pbData = NULL;
	}
 
#ifdef	OFFLINE_DEBUG 
	if (m_timerId != 0)
	{
		m_timerId = 0;
		killTimer(m_timerId);
	}
#endif

}

LONG AcqDriver::init()
{
	//返回值
	LONG rtValue = 0 ;
#ifdef  QDEBUGPRINT
	qDebug("Enter init Func AcqDriver Class ");

#endif
	if (rtValue = AVerGetDeviceNum(&dwDeviceNum) != CAP_EC_SUCCESS)
	{
		//Global::AppendLogString( QString("Can't get the number of devices."),true);
#ifdef  QDEBUGPRINT
		qDebug("Can't get the number of devices.");

#endif

		//写入系统日志
		Global::systemLog->append(QString(tr("Can't get the number of devices ")),QString("AcqDriver"),
			SystemLog::ERROR_TYPE);

		return 2;
	}
	else
	{
		 

#ifdef  QDEBUGPRINT
		qDebug(" get the number of devices.%d ", dwDeviceNum);
#endif
	}
	if (dwDeviceNum != 1)
		return CAP_EC_ERROR_STATE;
	for (DWORD dwDeviceIndex = 0; dwDeviceIndex < dwDeviceNum; dwDeviceIndex++)
	{
		QString myString;

		if (rtValue = AVerGetDeviceName(dwDeviceIndex, wcDeviceName) != CAP_EC_SUCCESS)
		{
			//Global::AppendLogString(QString("can not get the name of device."), 1);
#ifdef  QDEBUGPRINT
			qDebug("can not get the name of device.");
#endif
			//写入系统日志
			Global::systemLog->append(QString(tr("Can't get the name of devices ")), QString("AcqDriver"),
				SystemLog::ERROR_TYPE);
			return 2;
		}
		else
		{
			myString = myString.fromWCharArray(wcDeviceName);
			QString logStr;
			logStr = QString("Get the name of device") + myString;
			//Global::AppendLogString(logStr, 1);
#ifdef  QDEBUGPRINT
			qDebug(" get the name of device. %s", qPrintable(myString));
#endif
		}

	}
	//创建 设备
	rtValue |= AVerCreateCaptureObjectEx(dwDeviceNum-1, DEVICETYPE_SD, NULL, &hSDCaptureDevice);
	// 设置电视制式 PAL or NSTC
	rtValue |= AVerSetVideoFormat(hSDCaptureDevice, VIDEOFORMAT_PAL);
	// 设置分辨率
	rtValue |= AVerSetVideoResolution(hSDCaptureDevice, VIDEORESOLUTION_720X576);
	//设置预览
	rtValue |= AVerSetVideoPreviewEnabled(hSDCaptureDevice, TRUE);




	//AVerSetVideoWindowPosition(hSDCaptureDevice, rectClient);
	// 设置视频来源
	if (Global::liveCardVideoSource == QString("AV"))
	{
		rtValue |= AVerSetVideoSource(hSDCaptureDevice, VIDEOSOURCE_VGA);
	}
	else if (Global::liveCardVideoSource == QString("SVIDEO"))
	{
		rtValue |= AVerSetVideoSource(hSDCaptureDevice, VIDEOSOURCE_SVIDEO);
	}
	else if (Global::liveCardVideoSource == QString("YB"))
	{
		rtValue |= AVerSetVideoSource(hSDCaptureDevice, VIDEOSOURCE_COMPOSITE);
	}
	else if (Global::liveCardVideoSource == QString("COMPONET"))
	{
		rtValue |= AVerSetVideoSource(hSDCaptureDevice, VIDEOSOURCE_COMPONENT);

	}
	else 
		rtValue |= AVerSetVideoSource(hSDCaptureDevice, VIDEOSOURCE_VGA);

	 

	if (rtValue == CAP_EC_SUCCESS)
		return CAP_EC_SUCCESS;
	else
	{
		return CAP_EC_ERROR_STATE ;
	}




}


// 打开设备

LONG AcqDriver::open()
{
#ifdef  QDEBUGPRINT
	qDebug("Enter open Func AcqDriver Class ");

#endif
#ifdef OFFLINE_DEBUG
	m_timerId = startTimer(TIME_CYCLE);
#endif // OFFLINE_DEBUG

	// 开始视频流	
	LONG rtValue = 0; 
	rtValue |= AVerStartStreaming(hSDCaptureDevice);
#ifdef CALLBACK_MODE
  
		//设置采集信息
		ZeroMemory(&VideoCaptureInfo, sizeof(VIDEO_CAPTURE_INFO));
		VideoCaptureInfo.bOverlayMix = FALSE;
		VideoCaptureInfo.dwDurationMode = DURATION_TIME; //
		VideoCaptureInfo.dwDuration = 20;
		VideoCaptureInfo.dwCaptureType = CT_SEQUENCE_FRAME;// CT_SEQUENCE_FRAME;
		VideoCaptureInfo.dwSaveType = ST_CALLBACK_RGB24;
		//设置回调函数
		VideoCaptureInfo.lpCallback = readBmpData;
		VideoCaptureInfo.lCallbackUserData = (LONG)this;


		//开始采集
		rtValue |= AVerCaptureVideoSequenceStart(hSDCaptureDevice, VideoCaptureInfo);

		if (rtValue == CAP_EC_SUCCESS)
			return CAP_EC_SUCCESS;
		else
		{
			return CAP_EC_ERROR_STATE;
		}
 
#endif
#ifndef CALLBACK_MODE
	
	 
		//申请 地址 
		rtValue |= AVerCaptureSingleImageToBuffer(hSDCaptureDevice,
			NULL, &imageDataBufLeng, NULL, 50);
		imageDataBuf = new BYTE[imageDataBufLeng];
#ifndef OFFLINE_DEBUG
		// 500ms 启动定时器
		m_timerId = startTimer(1000);
#endif
		if (rtValue == CAP_EC_SUCCESS)
			return CAP_EC_SUCCESS;
		else
		{
			return CAP_EC_ERROR_STATE;
		}
 
#endif // !CALLBACK_MODE

 
}



BOOL WINAPI AcqDriver::readBmpData(VIDEO_SAMPLE_INFO VideoInfo, BYTE *pbData, LONG lLength, __int64 tRefTime, LONG lUserData)
{
	//qDebug("Enter readBmpData func ");
	AcqDriver *pThis = (AcqDriver*)lUserData;
	pThis->writeData(VideoInfo, pbData, lLength, lUserData);
	pThis->cpVideoInfo = &VideoInfo;
	return TRUE;
}
// 读取数据 
void AcqDriver::writeData(VIDEO_SAMPLE_INFO VideoInfo, BYTE *pbData, LONG lLength, LONG lUserData)
{

	AcqDriver *pThis = (AcqDriver*)lUserData;
 
	// 内存拷贝,添加至环形缓冲区
	if (pbData == NULL || lLength > Global::S_CCycleBuffer->getBufSize() )
	{
		//Global::AppendLogString(QString("AcqDriver: writeData wrong ! "),true) ;
#ifdef QDEBUGPRINT
		qDebug("AcqDriver: writeData wrong ! \n");
		//写入系统日志
	
#endif
		Global::systemLog->append(QString(tr("AcqDriver: writeData wrong ")), QString("AcqDriver"),
			SystemLog::ERROR_TYPE);
	}
	if (Global::S_CCycleBuffer->getFreeSize() >= lLength )
	{
		Global::S_CCycleBuffer->write((char *)pbData, lLength);

 
	}


}

 
//关闭设备 ，释放内存
LONG AcqDriver::close()
{

 
	//关闭定时器
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
	if (imageDataBuf != NULL )
	{
		delete[] imageDataBuf;
		imageDataBuf = NULL;
	}
	/*
	if (bmpImageBits !=NULL)
	{
		delete[] bmpImageBits;
		bmpImageBits = NULL;
	}
	*/
  
#ifdef  QDEBUGPRINT
	qDebug("Enter close Func AcqDriver Class ");

#endif
	// 停止采集
	LONG rtValue = AVerCaptureVideoSequenceStop(hSDCaptureDevice);
	// 关闭
	rtValue |= AVerStopStreaming(hSDCaptureDevice);

	if (rtValue == CAP_EC_SUCCESS)
		return CAP_EC_SUCCESS;
	else
	{
		return CAP_EC_ERROR_STATE ;
	}
}

LONG AcqDriver::getCcbUsedSize()
{

	return Global::S_CCycleBuffer->getUsedSize();

}
//读取 
LONG AcqDriver::ccbRead(char * buffer, int size)
{
	return 	Global::S_CCycleBuffer->read(buffer, size);
}

//定时器周期响应函数
void AcqDriver::timerEvent(QTimerEvent *event)
{
	if (Global::isCountTimeCalied )
	{
		Global::timerCount = Global::countRaceTime * 60;

		Global::isCountTimeCalied = false;
	}
	
	//计时分钟数 
	Global::countRaceTime = Global::timerCount / 60;


	// 每个定时器周期更新一次
	if (event->timerId() == m_timerId)
	{
		Global::timerCount++;
		
#ifdef  OFFLINE_DEBUG
		createFalseData();
#endif //  OFFLINE_DEBUG
	captureSingleImage();

	}
}
// 利用定时器周期抓取单幅图片
void AcqDriver::captureSingleImage()
{
#ifndef	OFFLINE_DEBUG 
 
	 
	LONG rtValue = AVerCaptureSingleImageToBuffer(hSDCaptureDevice,
		imageDataBuf, &imageDataBufLeng, NULL, 50);
 
	localImage = QImage::fromData((uchar *)imageDataBuf, (int)(imageDataBufLeng));

	localImage = localImage.convertToFormat(QImage::Format_RGB888);
	int imageWidth = localImage.width();
	int imageHeight = localImage.height();
	int imageLength = imageWidth * imageHeight * 3 ;
	localImage.save("acq.bmp");
	//睡眠调节 采集图片时间 1秒两次 
	 
	// 内存拷贝,添加至环形缓冲区
	if (localImage.bits() == NULL || imageLength > Global::S_CCycleBuffer->getBufSize())
	{
		qDebug("AcqDriver: captureSingleImage func wrong ! \n");
	}
	if (Global::S_CCycleBuffer->getFreeSize() >= imageLength)
	{
		Global::S_CCycleBuffer->write((char *)localImage.bits(), imageLength);


	}
 
#endif 
 
}