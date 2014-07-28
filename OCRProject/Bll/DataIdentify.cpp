#include "DataIdentify.h"
#include <QDebug>
#include<fstream>


//#define  CALLBACK_MODE
using namespace std;


DataIdentify::DataIdentify()
{
	dataLength = 0;
	imageHeight = 0;
	imageWidth = 0;

	memset(sampleCount, 0, sizeof(sampleCount));

	algorithmState = EXEC_SUCCESS;
	qinQplSubRect = new CvRect[19];
	{
		qinQplSubRect[0] = LB_REGION1_RECT;
		qinQplSubRect[1] = LB_REGION2_RECT;
		qinQplSubRect[2] = LB_REGION3_RECT;
		qinQplSubRect[3] = LB_REGION4_RECT;
		qinQplSubRect[4] = LB_REGION5_RECT;
		qinQplSubRect[5] = LB_REGION6_RECT;

		qinQplSubRect[6] = RU_REGION1_RECT;
		qinQplSubRect[7] = RU_REGION2_RECT;
		qinQplSubRect[8] = RU_REGION3_RECT;
		qinQplSubRect[9] = RU_REGION4_RECT;
		qinQplSubRect[10] = RU_REGION5_RECT;
		qinQplSubRect[11] = RU_REGION6_RECT;


		qinQplSubRect[12] = R_REGION1_RECT;
		qinQplSubRect[13] = R_REGION2_RECT;
		qinQplSubRect[14] = R_REGION3_RECT;
		qinQplSubRect[15] = R_REGION4_RECT;
		qinQplSubRect[16] = R_REGION5_RECT;
		qinQplSubRect[17] = R_REGION6_RECT;
		qinQplSubRect[18] = R_REGION7_RECT;


	}

	//initLengthHeightComp();

	// initialize the HorseInfo
	dataOutput.horseNum = HORSENUMBER;
 

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		dataOutput.mHorseInfo.horseID[i] = 0;
//		dataOutput.mHorseInfo.horseName[i] = "";
		dataOutput.mHorseInfo.isSCR[i] = false;
	}

	// initialize the DataOutput
	dataOutput.session = 0;
	dataOutput.raceTime = 0;

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		dataOutput.svmResult[i] = -1;
		dataOutput.WIN[i] = 0.0;
		dataOutput.PLA[i] = 0.0;
	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 12; j++)
			dataOutput.QPL_QIN[i][j] = 0;
	}


	for (int i = 0; i < dataOutput.horseNum  ; i++)
	{
		for (int j = 0; j < 3; j++)
			winPlaPosStruct.rect[i][j] = cvRect(0, 0, 0, 0);
	}


	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
		for (int j = 0; j < QIN_QPL_COL; j++)
			qinQPLPosStruct.rect[i][j] = cvRect(0, 0, 0, 0);
	}

	// 倒计时位置 初始化

	for (int i = 0; i < 2; i++)
		countDownMinutePosStruct.rect[i] = cvRect(0, 0, 0, 0);

	// HOG特征初始化
	hog.winSize = Size(10, 20);
	hog.blockSize = Size(10, 10);
	hog.blockStride = Size(5, 5);
	hog.cellSize = Size(5, 5);
	hog.nbins = 9;
	winStride = Size(1, 1);
	padding = Size(0, 0);
	float vectorSize = hog.getDescriptorSize();
	vector<float> descriptorVector(vectorSize);
	Mat HOG(1, 108, CV_32FC1, Scalar(0));
	this->descriptorVector = descriptorVector;
	this->hogMat = HOG;
	/*
	QString curPath = QDir::currentPath();
	qDebug("curPath = %s \n", qPrintable(curPath));
	*/
	// 加载已经训练好的样本 

	numSVM.load(".\\SVM\\numberSvm.xml");
	LNSVM.load(".\\SVM\\LN_SVM_1.xml");

	horseNameSVM.load(".\\SVM\\horseName.xml");

	raceTimeSvm.load(".\\SVM\\raceTime.xml");
	sessionNumSvm.load(".\\SVM\\sessionNum.xml");



}

DataIdentify::~DataIdentify()
{

}

// ===========================================================
// public function 
// ===========================================================
bool DataIdentify::read(Mat &srcMat ,uchar* data, int length, int height, int width)
{
	// 历史数据读入为Mat ，实时直播读入为 data 指针
	if (data != NULL)
	{
		Mat image_temp(height, width, CV_8UC3, data);
		bool emptyFlag = image_temp.empty();
		if (emptyFlag)
			return emptyFlag;
		else
		{
			image = image_temp;
			dataLength = length;
			imageHeight = height;
			imageWidth = width;


			return emptyFlag;
		}
	}
	else
	{

		cvtColor(srcMat, srcMat, CV_BGR2RGB);
		Mat image_temp = srcMat;
		bool emptyFlag = image_temp.empty();
		if (emptyFlag)
			return emptyFlag;
		else
		{
			image = image_temp;
			dataLength = length;
			imageHeight = height;
			imageWidth = width;


			return emptyFlag;
		}
	}

	

}

void  DataIdentify::isReady()
{
	;
}

 

// ===========================================================
// private function
// ===========================================================

// DataIdentify the current frame has or not has any data
void DataIdentify::haveData()
{
	Mat image_temp = image;
	//imshow("a", image_temp);
	//waitKey();

	cvtColor(image, image_temp, CV_RGB2BGR);
 

	// in region 1
	Vec3b bgr[5];
	uchar bluePixel[5];
	uchar greenPixel[5];
	uchar redPixel[5];
	uchar blueMean = 0, greenMean = 0, redMean = 0;
	uchar blueErr = 0, greenErr = 0, redErr = 0;

	for (int i = 0; i < HAVEDATA_PIXELNUMBER; i++)
	{
		bgr[i] = image_temp.at<Vec3b>(HAVEDATA_YINDEX * (i + 1), HAVEDATA_XINDEX);
		bluePixel[i] = bgr[i][0];
		greenPixel[i] = bgr[i][1];
		redPixel[i] = bgr[i][2];

		blueMean += bluePixel[i] / HAVEDATA_PIXELNUMBER;
		greenMean += greenPixel[i] / HAVEDATA_PIXELNUMBER;
		redMean += redPixel[i] / HAVEDATA_PIXELNUMBER;
	}

	if (blueMean > 50 && greenMean < 20 && redMean < 40)
	{
		haveDataFlag = true;
	}

	else
	{
		haveDataFlag = false;
	}

	dataOutput.haveDataFlag = haveDataFlag;

	// =====================================
	// region 2
	CvPoint point[6];
	Vec3b region2_bgr[6];
	int x1 = 40, x2 = 680;
	int y = 400, delta_y = 50;

	uchar region2_bluePixel[6];
	uchar region2_greenPixel[6];
	uchar region2_redPixel[6];
	uchar region2_blueMean = 0, region2_greenMean = 0, region2_redMean = 0;
	uchar region2_blueErr = 0, region2_greenErr = 0, region2_redErr = 0;

	for (int i = 0; i < 3; i++)
	{
		point[2 * i] = cvPoint(x1, y + i * delta_y);
		region2_bgr[2 * i] = image_temp.at<Vec3b>(point[2 * i].y, point[2 * i].x);

		point[2 * i + 1] = cvPoint(x2, y + i * delta_y);
		region2_bgr[2 * i + 1] = image_temp.at<Vec3b>(point[2 * i + 1].y, point[2 * i + 1].x);
	}

	for (int i = 0; i < 6; i++)
	{
		region2_bluePixel[i] = region2_bgr[i][0];
		region2_greenPixel[i] = region2_bgr[i][1];
		region2_redPixel[i] = region2_bgr[i][2];

		region2_blueMean += region2_bluePixel[i] / 6;
		region2_greenMean += region2_greenPixel[i] / 6;
		region2_redMean += region2_redPixel[i] / 6;
	}

	if (haveDataFlag == true)
	{
		if (region2_blueMean > 40 && region2_greenMean<30 && region2_redMean > 50)
		{
			haveDataFlag = true;
			cvtColor(image, image_temp, CV_RGB2BGR);
			//运行状态初始化为成功
			algorithmState = EXEC_SUCCESS;
		}
		else
		{
			algorithmState = EXIT_THIS_OCR;
			haveDataFlag = false;
		}
			
	}


	 
}


// get the origin position
// return the coodinate of the origin
void DataIdentify::originPosition()
{
	int regionWidth = ORIGINPOSITION_REGIONWIDTH, regionHeight = ORIGINPOSITION_REGIONHEIGHT;
	// 
	// 	imshow("a",image);
	// 	waitKey();
	// regionWidth regionHeight 没有用上。
	Mat region(image, Rect(0, 0, regionWidth, regionHeight));
	Mat regionGray(regionHeight, regionWidth, CV_8UC1, Scalar::all(0));

	cvtColor(image, regionGray, CV_BGR2GRAY);

//	imshow("a", regionGray);
//	waitKey();

	int* colSum = new int[regionHeight+1];
	int* rowSum = new int[regionWidth+1];

	// calculate the row sum
	for (int i = 0; i < regionHeight; i++)
	{
		rowSum[i] = 0;
		for (int j = 0; j < regionWidth; j++)
			rowSum[i] += regionGray.at<uchar>(i, j);

		rowSum[i] = rowSum[i] / regionWidth;
	}
	cout << endl;

	// calculate the col sum
	for (int i = 0; i < regionWidth; i++)
	{
		colSum[i] = 0;
		for (int j = 0; j < regionHeight; j++)
			colSum[i] += regionGray.at<uchar>(j, i);

		colSum[i] = colSum[i] / regionHeight;
	}

	// DataIdentify the originX
	for (int i = 0; i < regionWidth; i++)
	{
		if (colSum[i]>ORIGINPOSITION_COLSUM_THRESHOLD)
		{
			originX = i - 1;
			break;
		}
	}
	// DataIdentify the originY
	for (int i = 0; i < regionHeight; i++)
	{
		if (rowSum[i]>ORIGINPOSITION_ROWSUM_THRESHOLD)
		{
			originY = i - 1;
			break;
		}
	}

 
//	qDebug("the originPosition Func : x =%d, y=%d",originX,originY);
	if (colSum != NULL)
	{
		delete[] colSum;
		colSum = NULL;
	}
	if (rowSum != NULL)
	{
		delete[] rowSum;
		rowSum = NULL;
	}

 
	return;
}

int DataIdentify::identify()
{

	haveData();
	if (haveDataFlag == false || algorithmState == EXIT_THIS_OCR)						// the frame has not any data, return 1
		return EXIT_THIS_OCR;
	//设置马名位置

	setHorseNameRectPos();

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// 设置 WIN PLA 位置

	setWINPLARectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// 设置 QIN QPL  同时设置QIN QPL标记位置 
	setQINQPLRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//  设置场次号位置

	setSessionRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//设置 倒计时位置 
	setCountDownRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//识别马名 
	getHorseNameIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// 识别 WIN PLA

	getWINPLAIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//识别 QIN QPL   同时识别ＱＩＮ　ＱＰＬ
	getQINQPLIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//识别场次号

	getSessionIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//识别倒计时 

	getCountDownIdentify();

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	return EXEC_SUCCESS;
}


/*
设置 马名的rect区间
*/
int DataIdentify::setHorseNameRectPos()
{
	Mat horseNameRegion(image, HORSENAME_REGION_RECT);

	//转灰度图像
	cvtColor(horseNameRegion, horseNameRegion, CV_RGB2GRAY);
	Mat edge;
	Canny(horseNameRegion, edge, 450, 400, 3, true);
	int *y = new int[HORSENUMBER + 1];

	//同时获取马的数目
	calculateGraySumXForSetHorseNameRect(edge, y, dataOutput.horseNum);
 


	for (int r = 0; r < dataOutput.horseNum; r++)
	{
		horseNamePosStruct.rect[r].x = HORSENAME_REGION_RECT.x;
		horseNamePosStruct.rect[r].width = HORSENAME_REGION_RECT.width;

		horseNamePosStruct.rect[r].y = HORSENAME_REGION_RECT.y + y[r];
		if (r == dataOutput.horseNum  - 1)
		{
			//防止高度太高
			horseNamePosStruct.rect[r].height = HORSENAME_REGION_RECT.height - y[r];
			if (horseNamePosStruct.rect[r].height > 21)
				horseNamePosStruct.rect[r].height = 21;

		}
		else
			horseNamePosStruct.rect[r].height = y[r + 1] - y[r];

		QString fileName;
		fileName.prepend(QString(".bmp"));
		fileName.prepend(QString::number(r));
		fileName.prepend(QString("horInfo4Rect"));
		Mat horseNameRect;
		horseNameRect = Mat(image, horseNamePosStruct.rect[r]);
		cvtColor(horseNameRect, horseNameRect, CV_RGB2GRAY);
		//imshow("1", horseNameRect);
		imwrite(fileName.toStdString(), horseNameRect);
		//waitKey();
	}


	if (y != NULL)
	{
		delete[] y;
		y = NULL;
	}
	return EXEC_SUCCESS;
}

// 设置 WIN PLA 位置

int DataIdentify::setWINPLARectPos()
{
	 
	  
	// get the relative position of the three vertex in the first row, relative to the origin 
	CvPoint point[3];
	point[0].x = LIVE_IMAGEINFO1_DELTAX;
	point[0].y = LIVE_IMAGEINFO1_DELTAY;
	point[1].x = LIVE_IMAGEINFO1_DELTAX + LIVE_IMAGEINFO1_WIDTH1;
	point[1].y = LIVE_IMAGEINFO1_DELTAY;
	point[2].x = LIVE_IMAGEINFO1_DELTAX + LIVE_IMAGEINFO1_WIDTH1 + LIVE_IMAGEINFO1_WIDTH2 * 2 - 4;
	point[2].y = LIVE_IMAGEINFO1_DELTAY;

	// get each rect in the original image
	CvRect rect_temp = cvRect(0, 0, 0, 0);
	originX = 58;
	originY = 39;
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rect_temp.x = point[j].x + originX;
			if (i < 4)
				rect_temp.y = point[j].y + originY + i * (LIVE_IMAGEINFO1_HEIGHT + 4);

			if (i >= 4)
				rect_temp.y = point[j].y + originY + i * (LIVE_IMAGEINFO1_HEIGHT + 3) + 4;
 
			if (j == 0)
				rect_temp.width = LIVE_IMAGEINFO1_WIDTH1;
			else
				rect_temp.width = LIVE_IMAGEINFO1_WIDTH2;

			rect_temp.height = LIVE_IMAGEINFO1_HEIGHT;
			winPlaPosStruct.rect[i][j] = rect_temp;
		}
	}
	return EXEC_SUCCESS;

}


// 设置 QIN QPL  同时设置QIN QPL标记位置 

int DataIdentify::setQINQPLRectPos()
{

#ifdef QDEBUG_OUTPUT
	qDebug("setQINQPLRectPos func \n");
#endif
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	Mat image_temp;
	image.copyTo(image_temp);
	cvtColor(image_temp, image_temp, CV_RGB2GRAY);
	Mat imageInfo2Mat(image_temp, WHOLE_QINQPL_POS_RECT);
	Mat imageInfo2RegionSub1(imageInfo2Mat, QINQPL_POS_RECT);


	/*
	//Canny(roi, edge, 450, 400, 3, true);
	Mat lbRegionSub1(imageInfo2RegionSub1, cvRect(0, 17, 43, 114) );
	Mat lbRegionSub1_edge;
	Canny(lbRegionSub1, lbRegionSub1_edge, 450, 400, 3, true);
	CvRect numberRect[7];
	*/

	imwrite("imageInfo2RegionSub.bmp", imageInfo2RegionSub1);

	int deltaNum = HORSENUMBER - dataOutput.horseNum;
	for (int i = 0; i < 19 - deltaNum; i++)
	{
		QString fileName;
		Mat imageInfo2RoiEdge;
		Mat imageInfo2Roi(imageInfo2RegionSub1, qinQplSubRect[i]);
		Canny(imageInfo2Roi, imageInfo2RoiEdge, 500, 450, 3, true);
		fileName.prepend(".bmp");
		fileName.prepend(QString::number(i));
		imwrite(fileName.toStdString(), imageInfo2RoiEdge);

		if (setEveryQINQPLPos(imageInfo2RoiEdge, i) == EXIT_THIS_OCR)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("Error:setQINQPLRectPos function : setQINQPLRectPos return EXIT THIS OCR i = %d\n", i);
#endif // QDEBUG_OUTPUT

			return EXIT_THIS_OCR;

		}

	}
 
	return EXEC_SUCCESS;


}

//输入canny 处理后图像
int  DataIdentify::setEveryQINQPLPos(Mat &mat, int rectNum)
{

#ifdef QDEBUG_OUTPUT
	qDebug("getImageInfo2Rect func \n");
#endif
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	int x0 = 0;
	int x1 = 0;
	//实际马匹数目与 14 个差 ，因为下面计算 每一个region的数字的个数时候，会用到

	if (dataOutput.horseNum > 14 | dataOutput.horseNum <= 0)
	{
		algorithmState = EXIT_THIS_OCR;

		return EXIT_THIS_OCR;

	}
	int delataNum = HORSENUMBER - dataOutput.horseNum;
	if (rectNum < 6 - delataNum) // 6-1
	{
		int * y = new int[6 - rectNum + 1]; // 6-1		 
		if (calculateGraySumXForSetQINQPLRect(mat, y, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
		{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
		}
		for (int i = 0; i < 6 - rectNum; i++)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("y_%d = %d \n", i, y[i]);
#endif
		}


		for (int i = rectNum + 1; i < 7 - delataNum; i++)
		{


			if (calculateGraySumYForTrim(mat, x0, x1, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			if (x1 <= x0)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
#endif
				return EXIT_THIS_OCR;
			}
			// 3个像素 空白
			qinQPLPosStruct.rect[i][rectNum].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum].width = x1 - x0 + 4;
			//向上增加一个像素
			qinQPLPosStruct.rect[i][rectNum].y = qinQplSubRect[rectNum].y + y[i - rectNum - 1] - 1;
			if (i == 6-delataNum)
			{
				qinQPLPosStruct.rect[i][rectNum].height = 19;
			}
			else
			{
			if (y[i - rectNum] - y[i - rectNum - 1] > 18)
				{
					y[i - rectNum] = y[i - rectNum - 1] + NUMBER_HEIGHT + 2;
				}
				qinQPLPosStruct.rect[i][rectNum].height = y[i - rectNum] - y[i - rectNum - 1] + 1;
			}
			 
		}
		 

		if (y != NULL)
		{
			delete[] y;
			y = NULL;
		}

	}
	else if (rectNum >= 6 & rectNum < 12)
	{

		int roiNum = rectNum - 5;
		int * y = new int[roiNum + 1]; // 6-1		 

		if (calculateGraySumXForSetQINQPLRect(mat, y, roiNum) == EXIT_THIS_OCR)
		{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
		}
		for (int i = 0; i < roiNum; i++)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("y_%d = %d \n", i, y[i]);
#endif
		}


		for (int i = 0; i <roiNum; i++)
		{


			if (calculateGraySumYForTrim(mat, x0, x1, roiNum) == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			if (x1 <= x0)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
#endif			
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			// 3个像素 空白
			qinQPLPosStruct.rect[i][rectNum - 4].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum - 4].width = x1 - x0 + 4;
			//向上增加一个像素
			qinQPLPosStruct.rect[i][rectNum - 4].y = qinQplSubRect[rectNum].y + y[i] - 1;
			if (i == roiNum - 1)
			{
				qinQPLPosStruct.rect[i][rectNum - 4].height = 19;
			}
			else
			{
				if (y[i + 1] - y[i ] > 18)
				{
					y[i +1] = y[i ] + NUMBER_HEIGHT + 2;
				}
				qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i];
			}
			 

		}

		if (y != NULL)
		{
			delete[] y;
			y = NULL;
		}
	}
	else if (rectNum >= 12)
	//rect num 12-18  cols 8-14
	{
		int roiNum = 7;
		int * y = new int[roiNum + 1]; // 7		 
		if (calculateGraySumXForSetQINQPLRect(mat, y, roiNum) == EXIT_THIS_OCR)
		{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
		}
		for (int i = 0; i < roiNum; i++)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("y_%d = %d \n", i, y[i]);
#endif
		}


		for (int i = 0; i < roiNum; i++)
		{

			if (i == 1 & rectNum - 4 == 10)
			{
#ifdef QDEBUG_OUTPUT
				qDebug(" MARK:getImageInfo2Rect function  ");
#endif
			}

			if (calculateGraySumYForTrim(mat, x0, x1, roiNum) == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}

			if (x1 <= x0)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
#endif
				algorithmState = EXIT_THIS_OCR;

				return EXIT_THIS_OCR;
			}
			// 3个像素 空白
			qinQPLPosStruct.rect[i][rectNum - 4].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum - 4].width = x1 - x0 + 4;
			//向上增加一个像素
			qinQPLPosStruct.rect[i][rectNum - 4].y = qinQplSubRect[rectNum].y + y[i] - 1;
			if (i == roiNum - 1)
			{
				qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT ;
			}
			else
			{
				if (y[i + 1] - y[i] > 18)
				{
					y[i + 1] = y[i] + NUMBER_HEIGHT + 2;
				}
				qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i];
			}
		 


		}

		if (y != NULL)
		{
			delete[] y;
			y = NULL;
		}
	}
	return EXEC_SUCCESS;

}
//  设置场次号位置

int DataIdentify::setSessionRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	sessionPosStruct.rect[0] = SESSION_POS_RECT;

}

//设置 倒计时位置 
int DataIdentify::setCountDownRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// 十位数
	countDownMinutePosStruct.rect[0] = COUNTDOWNMINUTE_POS_RECT1;
	//个位数
	countDownMinutePosStruct.rect[1] = COUNTDOWNMINUTE_POS_RECT2;


}
//识别马名 
int DataIdentify::getHorseNameIdentify()
{
	//暂时不识别
	int temp = 0;
	return EXIT_SUCCESS;
}
// 识别 WIN PLA

int DataIdentify::getWINPLAIdentify()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	CvRect rectDot[3];
	CvRect rectNoDot[3];

	rectDot[0].x = 3;	rectDot[1].x = 13;		rectDot[2].x = 27;
	rectNoDot[0].x = 3; rectNoDot[1].x = 16;	rectNoDot[2].x = 26;
	for (int i = 0; i < 3; i++)													// set the rect for single number in number region
	{
		rectDot[i].y = 0;
		rectDot[i].width = HISTORY_IMAGEINFO1_SINGLE_WIDTH;
		rectDot[i].height = HISTORY_IMAGEINFO1_HEIGHT;

		rectNoDot[i].y = 0;
		rectNoDot[i].width = HISTORY_IMAGEINFO1_SINGLE_WIDTH;
		rectNoDot[i].height = HISTORY_IMAGEINFO1_HEIGHT;
	}
	float factor[2][3] = { { 10, 1, 0.1 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

	// svm identify each number
	Mat edge;
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		for (int j = 1; j < 3; j++)
		{
			Mat roi(image, winPlaPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = winPlaPosStruct.rect[i][j].height;
			roiSize.width = winPlaPosStruct.rect[i][j].width;
			Mat roiThreshold(roiSize, CV_8UC3);
			bool emtyFlag = roi.empty();

			colorThreshold(roi, roiThreshold, 200);


			Mat roiThresholdEdge;
			cvtColor(roi, roi, CV_RGB2GRAY);
			//Canny(roi, edge, 150, 100, 3, true);

			//专门用于 小数点检测
			cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);
			//更改canny 阈值 降低阈值 原值 400 450 
			//Canny(roiThreshold, roiThresholdEdge, 150, 100, 3, true);
			//使用 阈值后的图像作为 输入
			//roi = roiThreshold;

			// 将阈值后的图像增强 roiThreshold 进行小数点判断
			for (int c = 0; c < roiThreshold.cols; c++)
			{
				for (int r = 0; r < roiThreshold.rows; r++)
				{
					if (roiThreshold.at<uchar>(r, c) > 10)
					{
						roiThreshold.at<uchar>(r, c) = 250;
					}
				}
			}

  

			if (i == 1 && j == 1)
			{

				int temp = 0;
#ifdef QDEBUG_OUTPUT
				qDebug("getImageInfo1_live:Mark");
#endif // QDEBUG_OUTPUT

			}
			bool dotFlag = judgeWINPLADot(1, roiThreshold, roiThresholdEdge);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
			writeSamples(i, j, 6, roi);
#endif
			/*
			//赛马退赛 标志 ，如果截取的数据区域小于30 说明此地方为 --- ，马匹已经退赛
			if (roiNew.cols < 20 )
			{
				dataOutput.mHorseInfo.isSCR[i] = true;
				dataOutput.WIN[i] = -1;
				dataOutput.PLA[i] = -1;
				continue ; 

			}
			*/

			float tempSum = 0.0;
			if (dotFlag)															// contain a dot
			{
				for (int k = 0; k < 3; k++)					// segment each single number and svm
				{
					if (k == 2 && rectDot[k].x + rectDot[k].width >= roi.cols)	// cross the boarder
						rectDot[k].width = roi.cols - rectDot[k].x;

					Mat singleNum(roi, rectDot[k]);									// the single number image

			 
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					writeSamples(i, j, k, singleNum);

#endif

					resize(singleNum, singleNum, cvSize(10, 20));
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					createClassifySamples(result, singleNum);

#endif
 
					tempSum += result * factor[0][k];
				}
			}
			else                                                                 // have no dot
			{
				for (int k = 0; k < 3; k++)
				{
					if (k == 2 && rectNoDot[k].x + rectNoDot[k].width >= roi.cols)
						rectNoDot[k].width = roi.cols - rectNoDot[k].x;
					Mat singleNum(roi, rectNoDot[k]);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					
					writeSamples(i, j, k, singleNum); 
					
#endif


					resize(singleNum, singleNum, cvSize(10, 20));
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					createClassifySamples(result, singleNum);
				 
#endif
					tempSum += result * factor[1][k];
				}
			}

			if (j == 1)
				dataOutput.WIN[i] = tempSum;
			if (j == 2)
				dataOutput.PLA[i] = tempSum;
		 
		}		 
	 
	}			 
	 
	return EXEC_SUCCESS;

}

//识别 QIN QPL   同时识别ＱＩＮ　ＱＰＬ
int DataIdentify::getQINQPLIdentify()
{

#ifdef QDEBUG_OUTPUT
	qDebug("getImageInfo2_live func \n");
#endif
	judgeQINQPL();

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	CvRect rect[3][3];


	float factor[3][3] = { { 1, 0.1, 0 }, { 10, 1, 0 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

	// svm identify each number

	int dotFlag;
	Mat imageInfo2Mat(image, WHOLE_QINQPL_POS_RECT);
	Mat imageInfo2RegionSub1(imageInfo2Mat, QINQPL_POS_RECT);

	//
			rect[0][0].x = 0;	rect[0][1].x = 9;		rect[0][2].x = 23;			// for two number with dot	
			rect[1][0].x = 0;	rect[1][1].x = 9;		rect[1][2].x = 19;			// for two number without dot
			//rect[2][0].x = 0;	rect[2][1].x = 11;		rect[2][2].x = 22;			// for three number without dot
			rect[2][0].x = 0;	rect[2][1].x = 9;		rect[2][2].x = 19;			// for three number without dot
	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
		for (int j = 0; j < QIN_QPL_COL; j++)
		{

			if (i == j | j == i + 1)
				continue;

			if (qinQPLPosStruct.rect[i][j].width < 5)
			{
				continue;
			}
				//过滤掉退赛的马匹数据 ，设置结果为-1 
			if (j> i+1)
			{
				if ( dataOutput.mHorseInfo.isSCR[i] == true )
				{
					dataOutput.QPL_QIN[i][j] = -1;
					continue; 
				}
				if (dataOutput.mHorseInfo.isSCR[j-1] == true)
				{
					dataOutput.QPL_QIN[i][j] = -1;
					continue;
				}
			}
			if (j < i )
			{
				
				if (dataOutput.mHorseInfo.isSCR[j+7] == true)
				{
					dataOutput.QPL_QIN[i][j] = -1;
					continue;
				}
			}
		
		
			if (imageInfo2RegionSub1.cols < (qinQPLPosStruct.rect[i][j].x + qinQPLPosStruct.rect[i][j].width))
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			if (imageInfo2RegionSub1.rows < (qinQPLPosStruct.rect[i][j].y + qinQPLPosStruct.rect[i][j].height))
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			Mat roi(imageInfo2RegionSub1, qinQPLPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = qinQPLPosStruct.rect[i][j].height;
			roiSize.width = qinQPLPosStruct.rect[i][j].width;
			Mat roiThreshold(roiSize, CV_8UC3);



			if (i == 1 & j == 0)
			{
				qDebug("Mark \n");
#ifdef  QDEBUG_OUTPUT
				qDebug("Mark \n");
#endif //  QDEBUG_OUTPUT


			}

			colorThreshold(roi, roiThreshold, 200);
			//roi = roiThreshold;
			if (i == 1 & j == 6)
			{
				//imshow("roiThreshold", roi);
				//	waitKey();
			}


			//	Canny(roi, edge, 450, 400, 3, true);

			CvRect roiNewSize;
			//	dotFlag = identifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;

			trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);

			cvtColor(roiForDotJudge, roiForDotJudge, CV_RGB2GRAY);

			cvtColor(roi, roi, CV_RGB2GRAY);

			roiNew = Mat(roi, roiNewSize);

			//将图像 增强 然后进行小数点判断
			for (int c = 0; c < roiForDotJudge.cols; c++)
			{
				for (int r = 0; r < roiForDotJudge.rows; r++)
				{
					if (roiForDotJudge.at<uchar>(r, c) > 10)
					{
						roiForDotJudge.at<uchar>(r, c) = 250;
					}
				}
			}




			int *x = new int[3 + 1];
			memset(x, 0, 4);



			/*		imwrite("error.bmp", roi);
			imshow("roi", roi);
			waitKey(1000);*/
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
			writeSamples(i, j, 6, roiForDotJudge);
#endif // WRITE_ROI_SMAPLES_CLASS

			Mat edge;
			dotFlag = judgeQINQPLDot(roiForDotJudge, edge, x);
			if (i == 0 & j == 2)
			{

				QString fileNameTemp;
				fileNameTemp.prepend(QString(".bmp"));
				fileNameTemp.prepend(QString::number(i));
				fileNameTemp.prepend(QString("i_j"));
				fileNameTemp.prepend(QString::number(j));
				imwrite(fileNameTemp.toStdString(), edge);
				//imshow("roi", roi);
				//waitKey();
			}

			if (dotFlag == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;

			}
			roiNewSize.y = 0;
			roiNewSize.height = roi.rows;
			if (roiNewSize.x + roiNewSize.width >= roi.cols)
			{
				roiNewSize.width = roi.cols - roiNewSize.x;
			}

		
			for (int i = 0; i < 3; i++)													// set the rect for single number in number region
			{
				for (int j = 0; j < 3; j++)
				{
					rect[i][j].y = 0;
					if (j < 2)
					{
						rect[i][j].width = rect[i][j + 1].x - rect[i][j].x;
					}
					else
					{
						rect[i][j].width = roiNewSize.width - rect[i][j].x;

					}
					rect[i][j].height = qinQPLPosStruct.rect[i][j].height;
				}
			}
 


			if (dotFlag == -1)
				qDebug("identifyImageInfor2_Dot_live_yp is wrong ! \n");
			//			imshow("edge", edge);
			//			waitKey();

			float tempSum = 0.0;
			if (dotFlag == 0)				 		// two number with dot
			{
				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{
					// x[k]此时为 两个数字中间
					if (k == 1)
					{

						rect[0][k].width = roiNew.cols - x[k];
						rect[0][k].x = x[k];
					}

					else
					{
						rect[0][k].x = 0;
						rect[0][k].width = x[k] + 1;
					}
					rect[0][k].height = roiNew.rows;

					Mat singleNum(roiNew, rect[0][k]);								// the single number image
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					writeSamples(i, j, k, singleNum);
#endif

					resize(singleNum, singleNum, cvSize(10, 20));
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
				 
					createClassifySamples(result, singleNum);
#endif // WRITE_ROI_SMAPLES_CLASS


					tempSum += result * factor[0][k];
				}
			}

			if (dotFlag == 1)										// two number without dot
			{
				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{

					if (k == 1 && rect[1][k].x + rect[1][k].width >= roiNew.cols)	// cross the boarder
						rect[1][k].width = roiNew.cols - rect[1][k].x;

					rect[1][k].height = roiNew.rows;
					Mat singleNum(roiNew, rect[1][k]);								// the single number image
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					writeSamples(i, j, k, singleNum);
#endif

					resize(singleNum, singleNum, cvSize(10, 20));
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					 
					createClassifySamples(result, singleNum);
#endif	

					tempSum += result * factor[1][k];
				}
			}

			if (dotFlag == 2)										// three number without dot
			{
				for (int k = 0; k < 3; k++)										// segment each single number and svm
				{
					if (k == 2)
					{

						rect[2][k].width = roiNew.cols - x[k];
						rect[2][k].x = x[k];
					}

					else
					{
						rect[2][k].x = x[k];
						rect[2][k].width = x[k + 1] - x[k];
					}

					rect[2][k].height = roiNew.rows;
					Mat singleNum(roiNew, rect[2][k]);								// the single number image

#ifdef  WRITE_ROI_SMAPLES_CLASS_INFO2
					writeSamples(i, j, k, singleNum);
#endif
					resize(singleNum, singleNum, cvSize(10, 20));
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef  WRITE_ROI_SMAPLES_CLASS_INFO2
					createClassifySamples(result, singleNum);
#endif //  WRITE_ROI_SMAPLES_CLASS_INFO2

					tempSum += result * factor[2][k];
				}
			}
			dataOutput.QPL_QIN[i][j] = tempSum;

			if (x != NULL)
			{
				delete[] x;
				x = NULL;
			}
			//delete[] x;
		}			// end j		
	} 	// end i	



	return EXEC_SUCCESS;
 
}
//识别场次号

int DataIdentify::getSessionIdentify()
{

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	Mat roi(image, sessionPosStruct.rect[0]);
	//Mat grayROI;

	//imwrite("session.bmp", roi);
	//cvtColor(roi, grayROI, CV_RGB2GRAY);
	cvtColor(roi, roi, CV_RGB2GRAY);
	resize(roi, roi, cvSize(10, 20));
	hog.compute(roi, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result = numSVM.predict(hogMat);



	dataOutput.session = int(result);

#ifdef WRITE_SESSION_CLASSIFY_SAMPELS 
	createClassifySamples(result, roi);

#endif

	return EXIT_SUCCESS;
}


//识别倒计时 
int DataIdentify::getCountDownIdentify()
{

	//countDownMinutePosStruct.rect[1] = cvRect(512 + originX, 4 + originY, 11, 17);
	// 十位数
	Mat minuteRoi1(image, countDownMinutePosStruct.rect[0]);
	//个位数
	Mat minuteRoi2(image, countDownMinutePosStruct.rect[1]);


	//	imwrite("miniteRoi1.bmp", minuteRoi1);
	//	imwrite("minuteRoi2.bmp", minuteRoi2);
	cvtColor(minuteRoi1, minuteRoi1, CV_RGB2GRAY);
	resize(minuteRoi1, minuteRoi1, cvSize(10, 20));
	hog.compute(minuteRoi1, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result1 = raceTimeSvm.predict(hogMat);

	cvtColor(minuteRoi2, minuteRoi2, CV_RGB2GRAY);
	resize(minuteRoi2, minuteRoi2, cvSize(10, 20));
	hog.compute(minuteRoi2, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result2 = raceTimeSvm.predict(hogMat);
#ifdef WRITE_MINUTE_CLASSIFY_SAMPELS 
	if (result1 >= 0 & result1 < 10)
	{
		createClassifySamples(result1, minuteRoi1);
	}
	if (result2 >= 0 & result2 < 10)
	{
		createClassifySamples(result2, minuteRoi2);
	}

#endif
	//	dataOutput.raceTime = (int)(result2*10); //倒计时为分钟数


	if (result1 == -1)
	{
		if (result2 == -1)
		{

			dataOutput.raceTime = -1;
			return EXEC_WARNING;							 // 未检测到分钟时间
		}
		else
		{
			dataOutput.raceTime = (int)result2;  //倒计时为分钟数
		}
	}
	else
	{
		if (result2 == -1)
		{
			dataOutput.raceTime = -1;
			return EXEC_WARNING;
		}
		else
		{
			result1 = result1 * 10.0 + result2;
			// 分钟数不可能大于30 
			if (result1 > 30)
			{
				return EXEC_WARNING;
			}
			dataOutput.raceTime = (int)(result1);  //倒计时为分钟数
		}
	}


	//	result1 += result1 * 10 + result2;
	//	dataOutput.raceTime = (int)(result1); //倒计时为分钟数

}


// 通过计算 灰度在 X方向上的投影 之间的间隔Y坐标，得到 WIN PLA 各个数字区域
// 输入图像为灰度 canny变换后图像

int DataIdentify::calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum)
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetHorseNameRect :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影
	int dimension = 0;

	dimension = mat.rows;

	int * graySumX = new int[dimension + 1];
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelY = 0; pixelY < dimension; pixelY++)
	{
		for (int pixelX = 0; pixelX < mat.cols; pixelX++)
		{

			graySumX[pixelY] += mat.at<uchar>(pixelY, pixelX);
		}
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetHorseNameRect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD = 300;
	for (int i = 0; i < dimension; i++)
	{

		//设置为 300 过滤掉 部分数字的底色
		if ((graySumX[i] < 300 && graySumX[i + 1] > 300
			&& graySumX[i + 2] > 300))
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetHorseNameRect : The  y %d is  %d", j, i);
#endif
			j++;

		}
	}

	if (graySumX != NULL)
	{
		delete[] graySumX;
		graySumX = NULL;
	}


	if (j > HORSENUMBER)
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :Func error  j != roiNum"), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetHorseNameRect :Func error  j != roiNum \n ");
#endif

		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


	return EXEC_SUCCESS;

}
// 通过计算 灰度在 X方向上的投影 之间的间隔Y坐标，得到 马名字的区间 
// 输入图像为灰度 canny变换后图像

int DataIdentify::calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum)
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetHorseNameRect :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影
	int dimension = 0;


	dimension = mat.rows;

	int * graySumX = new int[dimension + 1];
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelY = 0; pixelY < dimension; pixelY++)
	{
		for (int pixelX = 0; pixelX < mat.cols; pixelX++)
		{

			graySumX[pixelY] += mat.at<uchar>(pixelY, pixelX);
		}
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetHorseNameRect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD = 300;
	for (int i = 0; i < dimension; i++)
	{

		//设置为 300 过滤掉 部分数字的底色
		if ((graySumX[i] < 300 && graySumX[i + 1] > 300
			&& graySumX[i + 2] > 300))
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetHorseNameRect : The  y %d is  %d", j, i);
#endif
			j++;

		}
	}
	//获取马的数目
	horseNum = j;

	if (graySumX != NULL)
	{
		delete[] graySumX;
		graySumX = NULL;
	}


	if (j  > HORSENUMBER)
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :Func error  j != roiNum"), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetHorseNameRect :Func error  j != roiNum \n ");
#endif

		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


	return EXEC_SUCCESS;

}

/*

通过检测 数字图片上半部分 空白区域的大小以及 位置来决定是否包含小数点
*/

bool DataIdentify::judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge)
{


	int len = 0;
	if (i == 1)									// processing the region 1
		len = LIVE_IMAGEINFO1_DOT_LEN;

	// 	if (i == 2)									// processing the region 2(QPL)		
	// 		len = LIVE_IMAGEINFO2_DOT_LEN;
	/*
	int* graySum = new int[edge->cols];
	for (int c = 0; c < edge->cols; c++)
	{
	graySum[c] = 0;
	for (int r = 0; r < edge->rows; r++)
	graySum[c] += edge->at<uchar>(r, c);
	graySum[c] = graySum[c] / edge->cols;
	}
	*/



	Mat halfEdge;
	halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows / 2));
	/*
	Mat halfEdgeBelow;
	halfEdgeBelow = Mat(edge, cvRect(0, edge.rows / 2, edge.cols, edge.rows / 2));
	int* graySumBelow = new int[halfEdgeBelow.cols];
	*/
	//去除底色边缘条纹
	int* graySum = new int[halfEdge.cols + 1];


	//imshow("halfEdge", halfEdgeBelow);
	//waitKey();
	memset(graySum, 0, sizeof(int)*halfEdge.cols);


	for (int c = 0; c < halfEdge.cols; c++)
	{
		for (int r = 0; r < halfEdge.rows; r++)
		{
			graySum[c] += halfEdge.at<uchar>(r, c);
		}

#ifdef QDEBUG_OUTPUT
		qDebug("identifyImageInfo1_Dot_live: graySum[%d] = %d \n", c, graySum[c]);
#endif
	}

	graySum[0] = 0;  graySum[1] = 0;  graySum[2] = 0;
	graySum[halfEdge.cols] = 0;
	graySum[halfEdge.cols - 1] = 0;
	graySum[halfEdge.cols - 2] = 0;
	graySum[halfEdge.cols - 3] = 0;


	//检测最大间距
	int  maxLengthPos = 0;
	int maxLengthAdd = 0; //右移
	int maxLengthMinus = 0; // 左移
	int maxLength = 0;
	/*
	//检测下半区 ，因为部分三位数字，中间的间距较大被误识别了。
	int  maxLengthPosBelow = 0;
	int maxLengthAddBelow = 0; //右移
	int maxLengthMinusBelow = 0; // 左移
	int maxLengthBelow = 0;

	*/
	for (int c = 0; c < halfEdge.cols; c++)
	{
		if (graySum[c] <= 150)
		{
			for (maxLengthAdd = 0; c + maxLengthAdd < halfEdge.cols; maxLengthAdd++)
			{
				if (graySum[c + maxLengthAdd] > 150)
				{
					break;
				}
				if (c + maxLengthAdd == halfEdge.cols - 1)
				{
					maxLengthAdd = 0;
					maxLengthMinus = 0;
					break;
				}

			}
			for (maxLengthMinus = 0; c - maxLengthMinus >= 0; maxLengthMinus++)
			{
				if (graySum[c - maxLengthMinus] > 150)
				{
					break;
				}
				if (c - maxLengthMinus == 0)
				{
					maxLengthMinus = 0;
					maxLengthAdd = 0;
					break;
				}
			}
		}
		//修正，因为上面的超出正确值1 。
		if (maxLengthAdd > 0)
		{
			maxLengthAdd--;
		}
		if (maxLengthMinus > 0)
		{
			maxLengthMinus--;

		}
		if (maxLength < maxLengthAdd + maxLengthMinus)
		{
			maxLengthPos = c;
			maxLength = maxLengthAdd + maxLengthMinus + 1; // +1 将本身所在的位置加上

#ifdef QDEBUG_OUTPUT
			qDebug("identifyImageInfo1_Dot_live: maxLengthPos%d,maxLength :%d \n", maxLengthPos, maxLength);
#endif
		}


	}

	if (graySum != NULL)
	{
		delete[] graySum;
		graySum = NULL;
	}

	if (maxLength >= 6 & maxLengthPos >19 & maxLengthPos < 31)
	{
		return true;
	}
	else
	{
		return false;
	}

 


}

int  DataIdentify::judgeQINQPLDot(Mat &roi, Mat &edge, int *x)
{

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	//Canny(roi, edge, 150, 100, 3, true);
	edge = roi;
	int graySumPeakNum = 0;
	int peakDistance = 0;

	int columns = edge.cols;

	int* graySum = new int[edge.cols + 1];
	memset(graySum, 0, edge.cols*sizeof(int));

	int sum = 0;
	for (int c = 0; c < edge.cols; c++)
	{
		sum = 0;
		for (int r = 0; r < edge.rows; r++)
		{
			sum += edge.at<uchar>(r, c);
			//	qDebug("roi.at[%d][%d] = %d \n",r,c,roi.at<uchar>(r,c));
		}
		graySum[c] = sum;
#ifdef QDEBUG_OUTPUT
		qDebug("graySum[%d] = %d", c, graySum[c]);
#endif
	}

	int frontIndex = 0;
	int backIndex = 0;
	for (int count = 0; count <= edge.cols; count++)
	{
		if (graySum[count] >= 300)
		{
			frontIndex = count;
			break;
		}


	}

	for (int count = edge.cols - 1; count >= 0; count--)
	{
		if (graySum[count] >= 300)
		{
			backIndex = count;
			break;
		}


	}
	int delta = backIndex - frontIndex;


	//int delta = roiNewSize.width;
	if (delta < 0)

	{
		qDebug("Dot check is wrong \n");

		algorithmState = EXIT_THIS_OCR;
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		return EXIT_THIS_OCR;
	}
	else if (delta > 24) // three number . 20
	{
		//获取3个数字的 坐标值 x
		if (calculateGraySumYForQINDotJudge(edge, x, 3) != EXEC_SUCCESS)
		{
			x[0] = 0;
			x[1] = 8;
			x[2] = 18;
			algorithmState = EXEC_SUCCESS;

		}
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		return 2;
	}
	else if (delta <= 24 & delta >= 21) // tow number with dot .
	{
		Mat halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows / 2));
		memset(graySum, 0, edge.cols*sizeof(int));
		sum = 0;
		for (int c = 0; c < halfEdge.cols; c++)
		{
			sum = 0;
			for (int r = 0; r < halfEdge.rows; r++)
			{
				sum += halfEdge.at<uchar>(r, c);
				//	qDebug("roi.at[%d][%d] = %d \n",r,c,roi.at<uchar>(r,c));
			}
			graySum[c] = sum;
#ifdef QDEBUG_OUTPUT
			qDebug("sizeof(int) = %d,graySum[%d] = %d", sizeof(int), c, graySum[c]);
#endif // QDEBUG_OUTPUT


		}
		int position;
		for (position = halfEdge.cols - 1; position > 0; position--)
		{
			if (graySum[position] != 0)
				break;
		}

		if (graySum[position / 2] >= 200 || graySum[position / 2 + 1] >= 200
			|| graySum[position / 2 - 1] >= 200)
		{
			//计算Y向投影通过reverseY设置为 true
			//calculateGraySumX(halfEdge, y, 3,true);
			if (calculateGraySumYForQINDotJudge(edge, x, 3) != EXEC_SUCCESS)
			{
				x[0] = 0;
				x[1] = 8;
				x[2] = 18;
				algorithmState = EXEC_SUCCESS;

			}
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}

			return 2; //三位数		 
		}
		else
		{
			//两位小数
			if (calculateXBewttenNumber(halfEdge, x) != EXEC_SUCCESS)
			{
				x[0] = 7;
				x[1] = 14;

			}
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}
			return 0;
		}

		//checkIfIsThreeNumber(edge,);

	}
	else if (delta < 21 & delta >= 5) // two number 
	{
		//两位数，防止 两位小数 进入 进行检测， 通过宽度已经无法检测出 两位小数与两位数 如1.1 ，99
		Mat halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows / 2));
		if (calculateXBewttenNumber(halfEdge, x) != EXEC_SUCCESS)
		{
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;

		}
		else
		{
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}

			//间隙过大，数据为 小数。
			if ((x[1] - x[0]) > 6)
				return 0;
			else
				return 1;

		}


	}
	else
	{
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


}



int DataIdentify::judgeQINQPL()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	Mat roi(image, QINQPL_LABEL_POS_RECT);
	// 	imshow("a", roi);
	// 	waitKey();

	 
	cvtColor(roi, roi, CV_RGB2GRAY);

	resize(roi, roi, cvSize(10, 20), 0, 0, INTER_CUBIC);
	hog.compute(roi, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result = LNSVM.predict(hogMat);
	if (int(result) == 0)
		dataOutput.isQPL = true;
	else
		dataOutput.isQPL = false;

	return EXEC_SUCCESS;
}


/*
裁剪ROI区域多余的空白像素
*/
int  DataIdentify::trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize)
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	int x0, x1;


	Mat edge;

	Canny(oriMat, edge, 150, 100, 3, true);

	//计算缩小尺寸

	if (calculateGraySumYForTrim(edge, x0, x1, 1) == EXIT_THIS_OCR)
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


	roiNewSize.x = x0;

	if ((x1 + 2) > oriMat.cols)
		roiNewSize.width = oriMat.cols - x0;
	else
		roiNewSize.width = x1 - x0 + 2;

	roiNewSize.y = 0;
	roiNewSize.height = edge.rows;

	newRoiMat = Mat(oriMat, roiNewSize);

	return EXEC_SUCCESS;
}



//对彩色图像 3个RGB颜色通道进行阈值操作 低于 thereshold的值 置 0 
// 为了更好地进行阈值操作，计算平均值，然后利用平均值进行阈值
int DataIdentify::colorThreshold(Mat &src, Mat &dst, int thereshold)
{
	//对彩色图像做阈值 取舍

	int rgbSum[3] = { 0 };

	Vec3b pixel;
	for (int c = 0; c < src.cols; c++)
	{
		for (int r = 0; r < src.rows; r++)
		{
			pixel = src.at<Vec3b>(r, c);		// row,col index (NOT x,y)
			for (int p = 0; p < 3; p++)
			{
				rgbSum[p] += pixel[p];
			}
		}
	}

	int adaptiveThreshold[3] = { 0 };

	for (int i = 0; i < 3; i++)
	{
		if (i != 2)
		{
			adaptiveThreshold[i] = rgbSum[i] * 4 / (2 * src.cols*src.rows);

		}
		else
		{
			adaptiveThreshold[i] = rgbSum[i] * 5 / (2 * src.cols*src.rows);
		}
		if (adaptiveThreshold[i] > 255)
		{
			adaptiveThreshold[i] = 210;
		}
		//	qDebug("adaptiveThreshold %d %d rgbSum %d\n",i,adaptiveThreshold[i],rgbSum[i]) ;
	}

	for (int c = 0; c < src.cols; c++)
	{
		for (int r = 0; r < src.rows; r++)
		{
			pixel = src.at<Vec3b>(r, c);		// row,col index (NOT x,y)
			for (int p = 0; p < 3; p++)
			if (pixel[p] <= thereshold) // adaptiveThreshold[p])
				pixel[p] = 0;
			dst.at<Vec3b>(r, c) = pixel;

		}
	}
	return EXEC_SUCCESS;
}




//横向投影calculateGraySumX(Mat &mat, int *y, int roiNum);
//输入图像为canny图像
// reverseY为统计 Y向投影 获取 y 值 。
int  DataIdentify::calculateXBewttenNumber(Mat &mat, int  *x)
{
	if (mat.empty())
	{
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumX :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影
	int dimension = mat.cols;
	int * graySumX = new int[dimension + 1];

	memset(graySumX, 0, sizeof(int)*dimension);
	for (int pixelX = 0; pixelX < dimension; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{

			graySumX[pixelX] += mat.at<uchar>(pixelY, pixelX);
		}
#ifdef QDEBUG_OUTPUT
		qDebug("graySumX %d = %d \n", pixelX, graySumX[pixelX]);
#endif
	}
	int j = 0;
	int THEREHOLD = 200; // change to 200 from 300  to test 4.1 with background color . 20140701-2/1188.bmp
	for (int i = dimension / 2; i < dimension; i++)
	{
		if (graySumX[i] >= THEREHOLD)
		{
			x[1] = i;
			break;
		}

	}

	for (int i = dimension / 2; i > 0; i--)
	{
		if (graySumX[i] >= THEREHOLD)
		{
			x[0] = i;
			break;
		}

	}

	if ((x[1] - x[0]) > 12 | (x[1] - x[0]) < 0)
	{

#ifdef QDEBUG_OUTPUT
		qDebug("Error :calculateXBewttenNumber: x[1] x[0] distance is too large or zero .");
#endif

		if (graySumX != NULL)
		{
			delete[] graySumX;
			graySumX = NULL;
		}
	}
	else
	{
		if (graySumX != NULL)
		{
			delete[] graySumX;
			graySumX = NULL;
		}
		return EXEC_SUCCESS;
	}
}

/*
//横向投影calculateGraySumX(Mat &mat, int *y, int roiNum);
//输入图像为canny图像

int  DataIdentify::calculateGraySumX(Mat &mat, int  *y, int roiNum  )
{
if (mat.empty())
{
//Global::AppendLogString(QString("ERROR : calculateGraySumX :mat is empty \n"),true) ;
#ifdef  QDEBUG_OUTPUT
qDebug("ERROR : calculateGraySumX :mat is empty \n");
#endif //  QDEBUG_OUTPUT


return EXIT_THIS_OCR ;

}

//	横向投影
int dimension = 0;

dimension = mat.rows;


//	float * m = new float[5];
int * graySumX = 0;
graySumX = new int[dimension];
memset(graySumX, 0, sizeof(int)*dimension);

for (int pixelY = 0; pixelY < dimension ; pixelY++)
{
for (int pixelX = 0; pixelX <  mat.cols; pixelX++)
{

graySumX[pixelY] +=  mat.at<uchar>(pixelY, pixelX);
}

#ifdef QDEBUG_OUTPUT
qDebug("graySumX %d = %d \n", pixelY, graySumX[pixelY]);
#endif
}
int j = 0;
int THEREHOLD =  300;
for (int i = 0; (i + 2) < dimension; i++)
{
//防止第一个像素就是 数字部分
if (i == 0 && (graySumX[0] >= THEREHOLD || graySumX[0] < THEREHOLD &&
graySumX[i + 1] > THEREHOLD && graySumX[i + 2] > THEREHOLD))
{
y[j] = 0;
j++;
//跳过1，防止附近有小于 300的。
i += 2;

}
//设置为 300 过滤掉 部分数字的底色
else if (i != 0 && (graySumX[i] < 300 && graySumX[i + 1] > 300
&& graySumX[i + 2] > 300 ))
{
y[j] = i;
#ifdef QDEBUG_OUTPUT
qDebug(" graySumX : The  y %d is  %d", j, i);
#endif
j++;


if (j>  roiNum)
{
//Global::AppendLogString(QString("Error:calculategraySumX :Func error j>roiNum \n "),true) ;
#ifdef QDEBUG_OUTPUT
qDebug("calculategraySumX :Func error j>roiNum \n ");
#endif
algorithmState = EXIT_THIS_OCR;
if (graySumX != NULL)
{

//	delete[] graySumX;
//	graySumX = NULL;

}
return EXIT_THIS_OCR ;
}
}
}
if (j !=roiNum)
{
//Global::AppendLogString(QString("Error:calculategraySumX :Func error  j != roiNum\n "), true);
#ifdef QDEBUG_OUTPUT
qDebug("calculategraySumX :Func error  j != roiNum \n ");
#endif
algorithmState = EXIT_THIS_OCR;
if (graySumX != NULL )
{

//delete[] graySumX;
//graySumX = NULL;

}

return EXIT_THIS_OCR ;
}

if (graySumX != NULL)
{

//delete[] graySumX;
//graySumX = NULL;

}

return EXEC_SUCCESS;


}
*/

//横向投影calculateGraySumX(Mat &mat, int *y, int roiNum);
//输入图像为canny图像

int  DataIdentify::calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum)
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("ERROR : calculateGraySumX :mat is empty \n"),true) ;
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumYForQINDotJudge :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影

	int dimension = 0;


	dimension = mat.cols;


	//	float * m = new float[5];
	int * graySumX = 0;
	graySumX = new int[dimension + 1];
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelX = 0; pixelX < dimension; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{

			graySumX[pixelX] += mat.at<uchar>(pixelY, pixelX);
		}

#ifdef QDEBUG_OUTPUT
		qDebug("graySumX %d = %d \n", pixelX, graySumX[pixelX]);
#endif
	}
	int j = 0;
	int THEREHOLD = 200;
	for (int i = 0; (i + 2) < dimension; i++)
	{
		//防止第一个像素就是 数字部分
		if (i == 0 && (graySumX[0] >= THEREHOLD || graySumX[0] < THEREHOLD &&
			graySumX[i + 1] > THEREHOLD && graySumX[i + 2] > THEREHOLD))
		{
			x[j] = 0;
			j++;
			//跳过1，防止附近有小于 300的。 
			//i += 2;

		}
		//设置为 300 过滤掉 部分数字的底色
		else if (i != 0 && (graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
			&& graySumX[i + 2] > THEREHOLD))
		{
			x[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumYForQINDotJudge : The  y %d is  %d", j, i);
#endif
			j++;


			if (j>  roiNum)
			{
				//Global::AppendLogString(QString("Error:calculategraySumX :Func error j>roiNum \n "),true) ;
#ifdef QDEBUG_OUTPUT
				qDebug("calculateGraySumYForQINDotJudge :Func error j>roiNum \n ");
#endif		
				algorithmState = EXIT_THIS_OCR;
				if (graySumX != NULL)
				{
					/*
					delete[] graySumX;
					graySumX = NULL;
					*/
				}
				return EXIT_THIS_OCR;
			}
		}
	}
	if (j != roiNum)
	{
		//Global::AppendLogString(QString("Error:calculategraySumX :Func error  j != roiNum\n "), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForQINDotJudge :Func error  j != roiNum \n ");
#endif
		algorithmState = EXIT_THIS_OCR;
		if (graySumX != NULL)
		{
			if (graySumX != NULL)
			{
				delete[] graySumX;
				graySumX = NULL;
			}
		}

		return EXIT_THIS_OCR;
	}

	if (graySumX != NULL)
	{
		delete[] graySumX;
		graySumX = NULL;
	}

	return EXEC_SUCCESS;


}


//计算纵向投影
int  DataIdentify::calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum)
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumY:mat is empty "), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim:mat is empty \n");
#endif

		return  EXIT_THIS_OCR;
	}
	//计算  纵向投影
	int * graySumY = new int[mat.cols + 1];

	memset(graySumY, 0, sizeof(int)*mat.cols);
	for (int pixelX = 0; pixelX < mat.cols; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{
			graySumY[pixelX] += mat.at<uchar>(pixelY, pixelX);
		}
		graySumY[pixelX] /= roiNum;
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim %d = %d \n", pixelX, graySumY[pixelX]);
#endif
	}

	//提取 两边y坐标
	for (int x = 0; x <= mat.cols; x++)
	{
		//排除掉 单条条纹 连续3个像素 》200 可认为发现了数字
		// 修改 由于连续三个像素 有时候会将 1 过滤掉了，所以只用1个，通过其他方法，改进 

		if (graySumY[x] > 500)
		{
			x0 = x;
			break;
		}
	}
	for (int x = mat.cols - 1; x >= 0; x--)
	{//排除掉 单条条纹 连续3个像素 》 600 为了 去除 底色线条导致的小点 被误识别 300为了对1进行识别。
		if (graySumY[x] > 500 & graySumY[x - 2] > 150)
		{
			if (x + 2 >= mat.cols)
				x1 = mat.cols - 1;
			else
				x1 = x + 2; // add one pixel
			break;
		}

	}
#ifdef QDEBUG_OUTPUT
	qDebug("x0 = %d x=%d", x0, x1);
#endif
	if (x1 < x0)
	{
		//Global::AppendLogString(QString("Error:calculateGraySumY :func error happens "), true);
#ifdef QDEBUG_OUTPUT
		qDebug("ERROR :calculateGraySumYForTrim :func error happens \n");
#endif
		if (graySumY != NULL)
		{
			delete[] graySumY;
			graySumY = NULL;
		}
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;

	}
	if (graySumY != NULL)
	{
		delete[] graySumY;
		graySumY = NULL;
	}

	//delete[] graySumY;
	//graySumY = NULL;
	return EXEC_SUCCESS;
}


//横向投影calculateGraySumXForSetRect2(Mat &mat, int *y, int roiNum);
//输入图像为canny图像

int  DataIdentify::calculateGraySumXForSetQINQPLRect(Mat &mat, int  *y, int roiNum)
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetRect2 :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影
	int dimension = 0;


	dimension = mat.rows;

	int * graySumX = new int[dimension + 1];
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelY = 0; pixelY < dimension; pixelY++)
	{
		for (int pixelX = 0; pixelX < mat.cols; pixelX++)
		{

			graySumX[pixelY] += mat.at<uchar>(pixelY, pixelX);
		}
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetRect2 %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD = 300;
	for (int i = 0; i < dimension; i++)
	{

		//设置为 300 过滤掉 部分数字的底色
		if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
			&& graySumX[i + 2] > THEREHOLD))
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetRect2 : The  y %d is  %d", j, i);
#endif
			j++;

			i += 2;

			if (j > roiNum)
			{
				//Global::AppendLogString(QString("Error:calculateGraySumXForSetRect2 :Func error j>roiNum "), true);
#ifdef QDEBUG_OUTPUT
				qDebug("calculateGraySumXForSetRect2 :Func error j>roiNum \n ");
#endif		
				if (graySumX != NULL)
				{
					delete[] graySumX;
					graySumX = NULL;
				}


				algorithmState = EXIT_THIS_OCR;
				//delete[] graySumX;
				//graySumX = NULL;
				return EXIT_THIS_OCR;
			}
		}
	}

	if (graySumX != NULL)
	{
		delete[] graySumX;
		graySumX = NULL;
	}


	if (j != roiNum)
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :Func error  j != roiNum"), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetRect2 :Func error  j != roiNum \n ");
#endif
		//delete[] graySumX;
		//graySumX = NULL;
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}

	//delete[] graySumX;
	//graySumX = NULL;

	return EXEC_SUCCESS;
	//提取 两边y坐标
}



/*
将识别的结果按类写入 相应目录 主要为数字
*/
void DataIdentify::createClassifySamples(float result, Mat &singleNum)
{

	//QString curPath = QDir::currentPath();

	/*char fileName[200];

	memset(fileName, 0, sizeof(fileName));
	int j = 0;

	j = sprintf_s(fileName, 200, "%s", ".//acqSamples//");
	j += sprintf_s(fileName + j, 200 - j, "%d", (int)result);
	j += sprintf_s(fileName + j, 200 - j, "%s", "//");
	j += sprintf_s(fileName + j, 200 - j, "%d", sampleCount[(int) result]);*/

	QString fileNameStr = QString(".//acqSamples//");


	fileNameStr.append(QString::number((int)result, 10));
	fileNameStr.append(QString("//"));

	sampleCount[(int)result]++;

	//QString curPath = QDir::currentPath();
	QDir::setCurrent(fileNameStr);
	QString curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));

	//curPath = QDir::currentPath();
	fileNameStr = QString("");
	fileNameStr.append(QString::number(sampleCount[(int)result] % 1000));
	fileNameStr.append(QString(".bmp"));
	imwrite(fileNameStr.toStdString(), singleNum);

	//退到上一层目录
	//	QDir::setCurrent("../");
	//	QDir::setCurrent("../");
	QDir::setCurrent("E://mvs//OCR_git//OCRProject");
	//curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));





}


void DataIdentify::writeSamples(int i, int j, int k, Mat &roi)
{

	//QString curPath = QDir::currentPath();

	/*char fileName[200];

	memset(fileName, 0, sizeof(fileName));
	int j = 0;

	j = sprintf_s(fileName, 200, "%s", ".//acqSamples//");
	j += sprintf_s(fileName + j, 200 - j, "%d", (int)result);
	j += sprintf_s(fileName + j, 200 - j, "%s", "//");
	j += sprintf_s(fileName + j, 200 - j, "%d", sampleCount[(int) result]);*/

	QString fileNameStr = QString(".//roiSamples//");


	//QString curPath = QDir::currentPath();
	QDir::setCurrent(fileNameStr);
	QString curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));
	//curPath = QDir::currentPath();
	fileNameStr = QString("");
	fileNameStr.append(QString("i_"));
	fileNameStr.append(QString::number((int)i, 10));
	fileNameStr.append(QString("j_"));
	fileNameStr.append(QString::number((int)j, 10));
	fileNameStr.append(QString("k_"));
	fileNameStr.append(QString::number((int)k, 10));
	fileNameStr.append(QString(".bmp"));
	imwrite(fileNameStr.toStdString(), roi);

	//退到上一层目录
	QDir::setCurrent("../");


	//curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));


}

