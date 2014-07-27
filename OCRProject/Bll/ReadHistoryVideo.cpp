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

	capture = cvCreateFileCapture(fileNameCharPtr);  //��ȡ��Ƶ
	fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);										 // ��ȡ��Ƶ��֡��
	totalFrames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	return 1;
}
int ReadHistoryVideo::read(int framePos,Mat &frame )
{

//	for (int f = 0; f < 1000;f++)
	{
		// ��ȡ��Ƶ������֡��
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, framePos*25);

		frameIplImage = cvQueryFrame(capture);	 		 //ץȡ֡

		Mat frameMat(frameIplImage);

		frame = frameMat;

	//	imshow("image", frameMat);
	//	waitKey();
	}
 
	return 1;
 
}