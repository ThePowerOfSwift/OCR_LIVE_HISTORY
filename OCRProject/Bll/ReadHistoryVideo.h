#pragma once
#include <QString>
# include "opencv2/opencv.hpp"
# include "opencv/cv.h"
# include "opencv/cvaux.h"
# include "opencv/highgui.h"
# include "opencv/ml.h"

using namespace cv;
class ReadHistoryVideo
{
public:
	ReadHistoryVideo();
	~ReadHistoryVideo();
	IplImage *frameIplImage;
	int open(QString fileName, double &totalFrames, double &fps);
	int close();
	int read(int framePos, Mat &frame);

 
private:
	QString fileType;
	void initD14SDK();
	CvCapture* capture;


};

