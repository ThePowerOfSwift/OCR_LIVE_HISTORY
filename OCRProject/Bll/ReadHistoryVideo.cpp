#include "readHistoryVideo.h"
#include "JPPlayApi.h"
#include "JPPlayError.h"

ReadHistoryVideo::ReadHistoryVideo()
{

//	initD14SDK();

}

void ReadHistoryVideo::initD14SDK()
{
	BOOL result = Player_InitialDirectDraw();
	if (result == FALSE )
	{
		qDebug("ERROR: ReadHistoryVideo initD14SDK func InitialDirectDraw faild \n");
	}
}
int ReadHistoryVideo::close()
{
	DWORD errorCode;
//	return 1;
	if (fileType == "d14" | fileType == "h64" | fileType == "D14" | fileType == "H64")
	{
		bool rtValue = false;
		//rtValue = Player_Pause(1);

		//rtValue = Player_Stop(1);
		if (rtValue == false )
		{
			qDebug("ReadHistoryVideo:colse Player_Stop Function exec error !");
		}
		errorCode = Player_GetLastError(1);
		rtValue = Player_CloseFile(1);
		if (rtValue == false)
		{
			qDebug("ReadHistoryVideo:colse Player_CloseFile Function exec error !");
		}
		/*rtValue = Player_CloseStream(1);
		if (rtValue == false)
		{
			qDebug("ReadHistoryVideo:colse Player_CloseStream Function exec error !");
		}*/
		rtValue = Player_ReleaseDirectDraw( );
		if (rtValue == false)
		{
			qDebug("ReadHistoryVideo:colse Player_ReleaseDirectDraw Function exec error !");
		}
		errorCode = Player_GetLastError(1);

	}
	else
	{
		cvReleaseCapture(&capture);
	}

	return 1;
}
ReadHistoryVideo::~ReadHistoryVideo()
{

	
	if (fileType == "d14" | fileType == "h64" | fileType == "D14" | fileType == "H64")
	{
	//	Player_Stop(1);
	//	Player_CloseFile(1);
	//	Player_CloseStream(1);
		Player_ReleaseDirectDraw();
		DWORD errorCode = Player_GetLastError(1);
	}
	else
	{
		cvReleaseCapture(&capture);
	}
	 
}
int ReadHistoryVideo::open(QString fileName,double &totalFrames,double &fps )
{
	 
	initD14SDK();
	char*  fileNameCharPtr;

	QByteArray ba = fileName.toLatin1();

	fileNameCharPtr = ba.data();

	fileType = fileName.mid(fileName.size() - 3, 3);

	if (fileType == "d14" | fileType == "h64" | fileType == "D14" | fileType == "H64")
	{
		fps = 25;
		bool rtValue = Player_OpenFile(1, fileNameCharPtr);
		HWND hwnd = NULL;
		UINT nMsg = 0;
		 
		rtValue = Player_SetPlayEndMsg(1, hwnd, nMsg);

		totalFrames = Player_GetFileFrames(1);

		if (Player_Play(1, NULL) != TRUE)
		{
			qDebug("SDK PlayerPlay Error! \n");

		}

	}
	else  
	 
	{
	
		capture = cvCreateFileCapture(fileNameCharPtr);  //读取视频
		fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);										 // 读取视频的帧率
		totalFrames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	}
	
	DWORD errorCode = Player_GetLastError(1);
	return 1;
}
int ReadHistoryVideo::read(int framePos,Mat &frame )
{
	if (fileType == "d14" | fileType == "h64" | fileType == "D14" | fileType == "H64")
	{
		QString fileName = QString("readD14.bmp") ;

		QByteArray byteArray = fileName.toLatin1();

		bool rtValue = Player_SetPlayedFrames(1, framePos * 25 + 1);

		rtValue = Player_SetPicQuality(1, true);
		//Player_GetPictureSize(1, &dwWidth, &dwHeight);
		rtValue = Player_SnapPicture(1, byteArray.data());
		if (rtValue)
		{
		
			frame = imread(byteArray.data());
		}


	}
	else
	{
		// 读取视频中有总帧数
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, framePos * 25);

		frameIplImage = cvQueryFrame(capture);	 		 //抓取帧

		Mat frameMat(frameIplImage);

		frame = frameMat;

		//	imshow("image", frameMat);
		//	waitKey();

	}
	
	return 1;
 
}
 