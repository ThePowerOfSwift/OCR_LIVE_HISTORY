#include "readHistoryVideo.h"


ReadHistoryVideo::ReadHistoryVideo()
{


}

int ReadHistoryVideo::close()
{
	cvReleaseCapture(&capture);
	return 1;
}
ReadHistoryVideo::~ReadHistoryVideo()
{

	cvReleaseCapture(&capture);
	 
}
int ReadHistoryVideo::open(QString fileName,double &totalFrames,double &fps )
{
	 
	char*  fileNameCharPtr;

	QByteArray ba = fileName.toLatin1();

	fileNameCharPtr = ba.data();

	capture = cvCreateFileCapture(fileNameCharPtr);  //读取视频
	fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);										 // 读取视频的帧率
	totalFrames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	return 1;
}
int ReadHistoryVideo::read(int framePos,Mat &frame )
{

//	for (int f = 0; f < 1000;f++)
	{
		// 读取视频中有总帧数
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, framePos*25);

		frameIplImage = cvQueryFrame(capture);	 		 //抓取帧

		Mat frameMat(frameIplImage);

		frame = frameMat;

	//	imshow("image", frameMat);
	//	waitKey();
	}
 
	return 1;
 
}