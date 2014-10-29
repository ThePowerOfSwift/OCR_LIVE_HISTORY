#include "YZTDataIdentify.h"
#include <QDebug>
#include<fstream>


//#define  CALLBACK_MODE
using namespace std;


// 修改haveData函数，因为亚洲台的版式发生了变化，所以相应的函数也要变化

YZTDataIdentify::YZTDataIdentify()
{
	dataLength = 0;
	imageHeight = 0;
	imageWidth = 0;

	memset(sampleCount, 0, sizeof(sampleCount));

	algorithmState = EXEC_SUCCESS;
	qinQplSubRect = new CvRect[19];
	qinQplSubRect_12 = new CvRect[19];
	qinQplSubRect_14 = new CvRect[19];
	 
	qinQplSubRect_14[0] = LB_REGION1_RECT;
	qinQplSubRect_14[1] = LB_REGION2_RECT;
	qinQplSubRect_14[2] = LB_REGION3_RECT;
	qinQplSubRect_14[3] = LB_REGION4_RECT;
	qinQplSubRect_14[4] = LB_REGION5_RECT;
	qinQplSubRect_14[5] = LB_REGION6_RECT;

	qinQplSubRect_14[6] = RU_REGION1_RECT;
	qinQplSubRect_14[7] = RU_REGION2_RECT;
	qinQplSubRect_14[8] = RU_REGION3_RECT;
	qinQplSubRect_14[9] = RU_REGION4_RECT;
	qinQplSubRect_14[10] = RU_REGION5_RECT;
	qinQplSubRect_14[11] = RU_REGION6_RECT;


	qinQplSubRect_14[12] = R_REGION1_RECT;
	qinQplSubRect_14[13] = R_REGION2_RECT;
	qinQplSubRect_14[14] = R_REGION3_RECT;
	qinQplSubRect_14[15] = R_REGION4_RECT;
	qinQplSubRect_14[16] = R_REGION5_RECT;
	qinQplSubRect_14[17] = R_REGION6_RECT;
	qinQplSubRect_14[18] = R_REGION7_RECT;

	qinQplSubRect_14[0] = LB_REGION1_RECT;
	qinQplSubRect_14[1] = LB_REGION2_RECT;
	qinQplSubRect_14[2] = LB_REGION3_RECT;
	qinQplSubRect_14[3] = LB_REGION4_RECT;
	qinQplSubRect_14[4] = LB_REGION5_RECT;
	qinQplSubRect_14[5] = LB_REGION6_RECT;

	qinQplSubRect_14[6] = RU_REGION1_RECT;
	qinQplSubRect_14[7] = RU_REGION2_RECT;
	qinQplSubRect_14[8] = RU_REGION3_RECT;
	qinQplSubRect_14[9] = RU_REGION4_RECT;
	qinQplSubRect_14[10] = RU_REGION5_RECT;
	qinQplSubRect_14[11] = RU_REGION6_RECT;


	qinQplSubRect_14[12] = R_REGION1_RECT;
	qinQplSubRect_14[13] = R_REGION2_RECT;
	qinQplSubRect_14[14] = R_REGION3_RECT;
	qinQplSubRect_14[15] = R_REGION4_RECT;
	qinQplSubRect_14[16] = R_REGION5_RECT;
	qinQplSubRect_14[17] = R_REGION6_RECT;
	qinQplSubRect_14[18] = R_REGION7_RECT;


	// 12 
	qinQplSubRect_12[0] = LB_REGION1_RECT_12;
	qinQplSubRect_12[1] = LB_REGION2_RECT_12;
	qinQplSubRect_12[2] = LB_REGION3_RECT_12;
	qinQplSubRect_12[3] = LB_REGION4_RECT_12;
	qinQplSubRect_12[4] = LB_REGION5_RECT_12;
	qinQplSubRect_12[5] = LB_REGION6_RECT_12;

	qinQplSubRect_12[6] = RU_REGION1_RECT_12;
	qinQplSubRect_12[7] = RU_REGION2_RECT_12;
	qinQplSubRect_12[8] = RU_REGION3_RECT_12;
	qinQplSubRect_12[9] = RU_REGION4_RECT_12;
	qinQplSubRect_12[10] = RU_REGION5_RECT_12;
	qinQplSubRect_12[11] = RU_REGION6_RECT_12;


	qinQplSubRect_12[12] = R_REGION1_RECT_12;
	qinQplSubRect_12[13] = R_REGION2_RECT_12;
	qinQplSubRect_12[14] = R_REGION3_RECT_12;
	qinQplSubRect_12[15] = R_REGION4_RECT_12;
	qinQplSubRect_12[16] = R_REGION5_RECT_12;
	qinQplSubRect_12[17] = R_REGION6_RECT_12;
	qinQplSubRect_12[18] = R_REGION7_RECT_12;

	qinQplSubRect_12[0] = LB_REGION1_RECT_12;
	qinQplSubRect_12[1] = LB_REGION2_RECT_12;
	qinQplSubRect_12[2] = LB_REGION3_RECT_12;
	qinQplSubRect_12[3] = LB_REGION4_RECT_12;
	qinQplSubRect_12[4] = LB_REGION5_RECT_12;
	qinQplSubRect_12[5] = LB_REGION6_RECT_12;

	qinQplSubRect_12[6] = RU_REGION1_RECT_12;
	qinQplSubRect_12[7] = RU_REGION2_RECT_12;
	qinQplSubRect_12[8] = RU_REGION3_RECT_12;
	qinQplSubRect_12[9] = RU_REGION4_RECT_12;
	qinQplSubRect_12[10] = RU_REGION5_RECT_12;
	qinQplSubRect_12[11] = RU_REGION6_RECT_12;


	qinQplSubRect_12[12] = R_REGION1_RECT_12;
	qinQplSubRect_12[13] = R_REGION2_RECT_12;
	qinQplSubRect_12[14] = R_REGION3_RECT_12;
	qinQplSubRect_12[15] = R_REGION4_RECT_12;
	qinQplSubRect_12[16] = R_REGION5_RECT_12;
	qinQplSubRect_12[17] = R_REGION6_RECT_12;
	qinQplSubRect_12[18] = R_REGION7_RECT_12;


 
	//initLengthHeightComp();

	// initialize the HorseInfo
	dataOutput.horseNum = HORSENUMBER;
//	horseInfo.horseID = new int[horseInfo.horseNum];
//	horseInfo.horseName = new QString[horseInfo.horseNum];
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		horseInfo.horseID[i] = 0;
//		horseInfo.horseName[i] = "";
		horseInfo.isSCR[i] = false;
	}

	// initialize the DataOutput
	dataOutput.session = 0;
	dataOutput.raceTime = 0;

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		dataOutput.svmResult[i] = -1;
		dataOutput.WIN[i].dataValue = 0.0;
		dataOutput.PLA[i].dataValue = 0.0;
	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			dataOutput.QPL[i][j].dataValue = 0;
			dataOutput.QIN[i][j].dataValue = 0;

		}
	}

 
	for (int i = 0; i < dataOutput.horseNum; i++)
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
	hog.winSize = Size(8, 15);
	hog.blockSize = Size(4, 5);
	hog.blockStride = Size(2, 5);
	hog.cellSize = Size(2, 5);
	hog.nbins = 9;
	winStride = Size(1, 1);
	padding = Size(0, 0);
	float vectorSize = hog.getDescriptorSize();
	vector<float> descriptorVector(vectorSize);
	Mat HOG(1, 162, CV_32FC1, Scalar(0));
	this->descriptorVector = descriptorVector;
	this->hogMat = HOG;

	// 加载已经训练好的样本 

	numSVM.load(".\\SVM\\YZTnumberSvm.xml") ;
	LNSVM.load(".\\SVM\\YZTnumberSvm.xml") ;

	horseNameSVM.load(".\\SVM\\YZTnumberSvm.xml") ;

	raceTimeSvm.load(".\\SVM\\YZTnumberSvm.xml") ;
	sessionNumSvm.load(".\\SVM\\YZTnumberSvm.xml") ;

}

YZTDataIdentify::~YZTDataIdentify()
{

}


/*
	读入一幅图像数据，Mat 或者为指针
*/
bool YZTDataIdentify::read(Mat &srcMat, uchar* data, int length, int height, int width)
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
 

void  YZTDataIdentify::isReady()
{
	isProcessed = true;
}


// ===========================================================
// private function
// ===========================================================

// DataIdentify the current frame has or not has any data
void YZTDataIdentify::haveData()
{
	Mat image_temp = image;
	//imshow("a", image_temp);
	//waitKey();

	cvtColor(image, image_temp, CV_RGB2BGR);
	 
	// in region 1
	Vec3b bgr[HORSENUMBER];
	uchar bluePixel[HORSENUMBER];
	uchar greenPixel[HORSENUMBER];
	uchar redPixel[HORSENUMBER];
	uchar blueMean = 0, greenMean = 0, redMean = 0;
	uchar blueErr = 0, greenErr = 0, redErr = 0;

	for (int i = 0; i < HORSENUMBER ; i++)
	{
		bgr[i] = image_temp.at<Vec3b>(NUMBER_HEIGHT * (i), ADCHECK_WIN_ORIGIN_X);
		bluePixel[i] = bgr[i][0];
		greenPixel[i] = bgr[i][1];
		redPixel[i] = bgr[i][2];

		blueMean += bluePixel[i] / HORSENUMBER;
		greenMean += greenPixel[i] / HORSENUMBER;
		redMean += redPixel[i] / HORSENUMBER;
	}

	if (blueMean < 20 && greenMean < 20 && redMean < 20)
	{
		haveDataFlag = true;
	}

	else
	{
		haveDataFlag = false;
		return;
	}

	dataOutput.haveDataFlag = haveDataFlag;
 
	// =====================================
	memset(bgr, 0, HORSENUMBER);
	memset(bluePixel, 0, HORSENUMBER);
	memset(greenPixel, 0, HORSENUMBER);
	memset(redPixel, 0, HORSENUMBER);
	blueMean = 0;
	greenMean = 0;
	redMean = 0;
	// QPl QIN 区域检测 
	for (int i = 0; i < 7 ; i++)
	{
		bgr[i] = image_temp.at<Vec3b>(NUMBER_HEIGHT * (i )+ADCHECK_QIN_ORIGIN_Y, ADCHECK_QIN_ORIGIN_X);
		bluePixel[i] = bgr[i][0];
		greenPixel[i] = bgr[i][1];
		redPixel[i] = bgr[i][2];

		blueMean += bluePixel[i] / 7;
		greenMean += greenPixel[i] / 7;
		redMean += redPixel[i] / 7;
	}
	
	if (blueMean < 10 & greenMean  < 10 & redMean > 30 )
	{
		dataOutput.haveDataFlag = true;
	}
	else
	{
		dataOutput.haveDataFlag = false;
		return;
	}

	 
}


// get the origin position
// return the coodinate of the origin
void YZTDataIdentify::originPosition()
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
 
 

int YZTDataIdentify::identify()
{

	haveData();
	if (haveDataFlag == false || algorithmState == EXIT_THIS_OCR)						// the frame has not any data, return 1
		return EXIT_THIS_OCR;
	//设置马名位置

	setHorseNameRectPos();

	// 设置 WIN PLA 位置

	setWINPLARectPos();

	// 设置 QIN QPL  同时设置QIN QPL标记位置 
	setQINQPLRectPos();

	//  设置场次号位置

	setSessionRectPos();

	//设置 倒计时位置 
	setCountDownRectPos();

	//识别马名 
	getHorseNameIdentify();

	// 识别 WIN PLA

	getWINPLAIdentify();

	//识别 QIN QPL   同时识别ＱＩＮ　ＱＰＬ
	getQINQPLIdentify();

	//识别场次号

	getSessionIdentify();

	//识别倒计时 

	getCountDownIdentify();

 
	return EXEC_SUCCESS;
}


/*
	设置 马名的rect区间
*/
int YZTDataIdentify::setHorseNameRectPos()
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
		if (r == dataOutput.horseNum - 1)
		{
			//防止高度太高
			horseNamePosStruct.rect[r].height = HORSENAME_REGION_RECT.height - y[r];
			if (horseNamePosStruct.rect[r].height > 21)
				horseNamePosStruct.rect[r].height = 21;

		}
		else
			horseNamePosStruct.rect[r].height = y[r + 1] - y[r];

#ifdef WRITE_HORSENAME_FILE
		QString fileName;
		fileName.prepend(QString(".bmp"));
		fileName.prepend(QString::number(r));
		fileName.prepend(QString("horInfo4Rect"));
		Mat horseNameRect;
		horseNameRect = Mat(image, horseNamePosStruct.rect[r]);
		cvtColor(horseNameRect, horseNameRect, CV_RGB2GRAY);
		imwrite(fileName.toStdString(), horseNameRect);
#endif
		//imshow("1", horseNameRect);
		//imwrite(fileName.toStdString(), horseNameRect);
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

int YZTDataIdentify::setWINPLARectPos()
{
	isProcessed = false;											// start data process


	Mat winRegion(image, WIN_POS_RECT);
	Mat plaRegion(image, PLA_POS_RECT);

	//转灰度图像
	cvtColor(winRegion, winRegion, CV_RGB2GRAY);
	cvtColor(plaRegion, plaRegion, CV_RGB2GRAY);

	Mat edgePla;
	Mat edgeWin;

	Canny(winRegion, edgeWin, 450, 400, 3, true);
	Canny(plaRegion, edgePla, 450, 400, 3, true);
	int *yWin = new int[HORSENUMBER + 1];
	int *yPla = new int[HORSENUMBER + 1];
	
	int x0, x1;

	imwrite("edgeWin.bmp", edgeWin);
	imwrite("edgePla.bmp", edgePla);

	calculateGraySumXForSetWINPLARect(edgeWin, yWin, horseNum);
	calculateGraySumXForSetWINPLARect(edgePla, yPla, horseNum);
	// get the relative position of the three vertex in the first row, relative to the origin 
	//


	//计算 识别区域的两侧X值
	if (calculateGraySumYForTrim(edgeWin, x0, x1, horseNum) == EXIT_THIS_OCR)
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	if (x1 <= x0)
	{
#ifdef QDEBUG_OUTPUT
		qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
#endif
		return EXIT_THIS_OCR;
	}
	for (int i = 0; i < horseNum; i++)
	{
		// 3个像素 空白	 		
		winPlaPosStruct.rect[i][0].x = WIN_POS_RECT.x + x0 - 2;
		winPlaPosStruct.rect[i][0].width = x1 - x0 + 4;

		winPlaPosStruct.rect[i][0].y = WIN_POS_RECT.y + yWin[i];

		if (i == horseNum - 1)
		{
			winPlaPosStruct.rect[i][0].height = winPlaPosStruct.rect[i - 1][0].height;
		}
		else
		{
			winPlaPosStruct.rect[i][0].height = yWin[i + 1] - yWin[i];
		}
	}

	x0 = 0;
	x1 = 0;
	// PLA part 
	//计算 识别区域的两侧X值
	if (calculateGraySumYForTrim(edgePla, x0, x1, horseNum) == EXIT_THIS_OCR)
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	if (x1 <= x0)
	{
#ifdef QDEBUG_OUTPUT
		qDebug("ERROR:setWINPLARect_live function : x1 < x0 ERROR");
#endif
		return EXIT_THIS_OCR;
	}

	for (int i = 0; i < horseNum; i++)
	{
		// 3个像素 空白	 		
		winPlaPosStruct.rect[i][1].x = PLA_POS_RECT.x + x0 - 2;
		winPlaPosStruct.rect[i][1].width = x1 - x0 + 4;

		winPlaPosStruct.rect[i][1].y = PLA_POS_RECT.y + yWin[i];


		if (i == horseNum - 1)
		{
			winPlaPosStruct.rect[i][1].height = winPlaPosStruct.rect[i - 1][1].height;
		}
		else
		{
			winPlaPosStruct.rect[i][1].height = yWin[i + 1] - yWin[i];
		}

	}

	if (yPla != NULL)
	{
		delete[] yPla;
		yPla = NULL;
	}
	if (yWin != NULL)
	{
		delete[] yWin;
		yWin = NULL;
	}

	return EXEC_SUCCESS;

}
 

// 设置 QIN QPL  同时设置QIN QPL标记位置 
 
int YZTDataIdentify::setQINQPLRectPos()
{
#ifdef QDEBUG_OUTPUT
	qDebug("setQINQPLRectLive");
#endif
 
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	
	if (horseNum <= 12)
	{
		qinQplSubRect = qinQplSubRect_12;
	}
	else
		qinQplSubRect = qinQplSubRect_14;

	Mat image_temp;
	image.copyTo(image_temp);

	//Mat qinQPLPosStructMat(image_temp, qinQPLPosStructMatRECT);

	Mat qinQPLPosStructRegionSub1(image_temp, QINQPL_POS_RECT );

	Vec3b pixel;
	for (int c = 0; c < image_temp.cols; c++)
	{
		for (int r = 0; r < image_temp.rows; r++)
		{
			pixel = image_temp.at<Vec3b>(r, c);		// row,col index (NOT x,y)
			for (int p = 0; p < 3; p++)
			{
				if (p == 2 )
				{
					pixel[p] = 0;
				}
				
			}
		
			image_temp.at<Vec3b>(r, c) = pixel;

		}
	}

	
 //	colorThreshold(image_temp, image_temp, 150);
	cvtColor(image_temp, image_temp, CV_RGB2GRAY);
	 
	imwrite("ori.bmp", image_temp);
	/*
	//将图像 增强 然后进行小数点判断
	for (int c = 0; c < image_temp.cols; c++)
	{
		for (int r = 0; r < image_temp.rows; r++)
		{
			if (image_temp.at<uchar>(r, c) > 10)
			{
				image_temp.at<uchar>(r, c) = 250;
			}
		}
	}

	*/



	int deltaNum = HORSENUMBER - horseNum;
	for (int i = 0; i < 19 - deltaNum; i++)
	{
		QString fileName;
		Mat qinQPLPosStructRoiEdge;

		Mat qinQPLPosStructRoi(image_temp, qinQplSubRect[i]);
		//Canny(qinQPLPosStructRoi, qinQPLPosStructRoiEdge, 200, 150, 3, true);

		fileName.prepend(".bmp");
		fileName.prepend(QString::number(i));
		imwrite(fileName.toStdString(), qinQPLPosStructRoi);

		if (setEveryQINQPLPos(qinQPLPosStructRoi, i) == EXIT_THIS_OCR)
		{
			
			qDebug("Error:setQINQPLRectLive_yp function : getqinQPLPosStructRect return EXIT THIS OCR i = %d\n", i);
#ifdef QDEBUG_OUTPUT
			qDebug("Error:setQINQPLRectLive_yp function : getqinQPLPosStructRect return EXIT THIS OCR i = %d\n", i);
#endif // QDEBUG_OUTPUT
			return EXIT_THIS_OCR;
		}

	}



	return EXEC_SUCCESS;

	 
}

//输入canny 处理后图像
int  YZTDataIdentify::setEveryQINQPLPos(Mat &mat, int rectNum)
{
#ifdef QDEBUG_OUTPUT
	qDebug("getqinQPLPosStructRect func \n");
#endif
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	int x0 = 0;
	int x1 = 0;
	//实际马匹数目与 14 个差 ，因为下面计算 每一个region的数字的个数时候，会用到

	if (horseNum >14 | horseNum <= 0)
	{
		algorithmState = EXIT_THIS_OCR;

		return EXIT_THIS_OCR;

	}
	int delataNum = 0;
	int is12 = 0;
	if (horseNum <= 12 )
	{
		is12 = 1;
		delataNum = HORSENUMBER - horseNum - 1 ;
	}
	else
	{
		is12 = 0;
		delataNum = HORSENUMBER - horseNum  ;
	}
	 
	if (rectNum < 6 - delataNum) // 6-1
	{
		int * y = new int[6 - rectNum + 1]; // 6-1		
		//计算 识别区域每个 数字之间的Y值
		if (calculateGraySumXForSetQINQPLRect(mat, y, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
		{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
		}
#ifdef QDEBUG_OUTPUT
		for (int i = 0; i < 6 - rectNum; i++)
		{

			qDebug("y_%d = %d \n", i, y[i]);

		}
#endif
		// 识别区域的两侧X值，其中 排除掉马匹数量低于14的情况
		for (int i = rectNum + 1; i < 7 - delataNum; i++)
		{
			//计算 识别区域的两侧X值
			if (calculateGraySumYForTrim(mat, x0, x1, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			if (x1 <= x0)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
#endif
				return EXIT_THIS_OCR;
			}
			// 3个像素 空白
			qinQPLPosStruct.rect[i][rectNum].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum].width = x1 - x0 + 4;
			//向上增加一个像素
			qinQPLPosStruct.rect[i][rectNum].y = qinQplSubRect[rectNum].y + y[i - rectNum - 1] - 1;
			if (i == 6 - delataNum)
			{//最后一个数字区域的高度可以使用第一行，第0列的高度
				qinQPLPosStruct.rect[i][rectNum].height = 15 ;
			}
			else
			{// 两个数字y 做减
				qinQPLPosStruct.rect[i][rectNum].height = y[i - rectNum] - y[i - rectNum - 1] + 1;
			}
		 
		}
	 

		if (y != NULL)
		{
			delete[] y;
			y = NULL;
		}

	}
	else if (rectNum >= 6 & rectNum < (12 - is12 ) )
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
				qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
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
				//如果此时 无法通过计算y 来得到高度，那么采用前面已经计算过的高度，因为高度值基本保持一致
				qinQPLPosStruct.rect[i][rectNum - 4].height = 15 ;
			}
			else
			{
				qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i];
			}
			/*
			//检查越界
			if ((qinQPLPosStruct.rect[i][rectNum-4].x + qinQPLPosStruct.rect[i][rectNum].width)
			> (QINQPLRoiRegion[rectNum].width + QINQPLRoiRegion[rectNum].x)

			)
			{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
			}
			if ((qinQPLPosStruct.rect[i][rectNum - 4].y + qinQPLPosStruct.rect[i][rectNum].height) >
			(QINQPLRoiRegion[rectNum].height + QINQPLRoiRegion[rectNum].y))
			{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
			}
			*/

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
				qDebug(" MARK:getqinQPLPosStructRect function  ");
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
				qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
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
				qinQPLPosStruct.rect[i][rectNum - 4].height = 15;
			}
			else
			{
				qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i];
			}
			/*
			//检查越界
			if ((qinQPLPosStruct.rect[i][rectNum - 4].x + qinQPLPosStruct.rect[i][rectNum].width)
			> (QINQPLRoiRegion[rectNum].width + QINQPLRoiRegion[rectNum].x + 2 )

			)
			{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
			}
			if ((qinQPLPosStruct.rect[i][rectNum - 4].y + qinQPLPosStruct.rect[i][rectNum].height) >
			(QINQPLRoiRegion[rectNum ].height + QINQPLRoiRegion[rectNum].y + 1 ))
			{
			algorithmState = EXIT_THIS_OCR;
			return EXIT_THIS_OCR;
			}
			*/


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

int YZTDataIdentify::setSessionRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	sessionPosStruct.rect[0] = SESSION_POS_RECT;
 
	return EXEC_SUCCESS;
}

//设置 倒计时位置 
int YZTDataIdentify::setCountDownRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// 十位数
	countDownMinutePosStruct.rect[0] = COUNTDOWNMINUTE_POS_RECT1;
	//个位数
	countDownMinutePosStruct.rect[1] = COUNTDOWNMINUTE_POS_RECT2;
	return EXEC_SUCCESS;
	
}
//识别马名 
int YZTDataIdentify::getHorseNameIdentify()
{
	//暂时不识别
	int temp = 0 ;
	return EXIT_SUCCESS;
}
// 识别 WIN PLA

int YZTDataIdentify::getWINPLAIdentify()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR ;
	}

	CvRect rectDot[3];
	CvRect rectNoDot[3];

	rectDot[0].x = 1;	rectDot[1].x = 11;		rectDot[2].x = 16;
	rectNoDot[0].x = 1; rectNoDot[1].x = 11;	rectNoDot[2].x = 18;
	for (int i = 0; i < 3; i++)													// set the rect for single number in number region
	{
		rectDot[i].y = 0;
		rectDot[i].width = 8 ;
		rectDot[i].height = 13 ;

		rectNoDot[i].y = 0;
		rectNoDot[i].width = 9 ;
		rectNoDot[i].height = 13 ;
	}
	float factor[2][2] = { {  1, 0.1 }, {  10, 1 } };							// the first line for dot, the second line for no dat

	// svm DataIdentify each number
	Mat edge;
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Mat roi(image, winPlaPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = winPlaPosStruct.rect[i][j].height;
			roiSize.width = winPlaPosStruct.rect[i][j].width;
			Mat roiThreshold(roiSize, CV_8UC3);
			bool emtyFlag = roi.empty();

		//	colorThreshold(roi, roiThreshold, 200);


			Mat roiThresholdEdge;
			cvtColor(roi, roi, CV_RGB2GRAY);
			//Canny(roi, edge, 150, 100, 3, true);
			roiThreshold = roi;
			//专门用于 小数点检测
			//cvtColor(roi, roiThreshold, CV_RGB2GRAY);
			//更改canny 阈值 降低阈值 原值 400 450 
			//Canny(roiThreshold, roiThresholdEdge, 150, 100, 3, true);
			//使用 阈值后的图像作为 输入
			//roi = roiThreshold;
			
			// 将阈值后的图像增强 roiThreshold 进行小数点判断
			for (int c = 0; c < roiThreshold.cols; c++)
			{
				for (int r = 0; r < roiThreshold.rows; r++)
				{
					if (roiThreshold.at<uchar>(r, c) < 50)
					{
						roiThreshold.at<uchar>(r, c) = 0;
					}
				}
			}
 
			if (i == 6 && j == 0 )
			{

				int temp = 0;
#ifdef QDEBUG_OUTPUT
				qDebug("getwinPlaPosStruct_live:Mark");
#endif // QDEBUG_OUTPUT

			}
			imwrite("roiThreshold.bmp", roiThreshold);
			bool dotFlag = judgeWINPLADot(1, roiThreshold, roiThresholdEdge);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
			writeSamples(i, j, 6, roi);
#endif


			float tempSum = 0.0;
			if (dotFlag)															// contain a dot
			{
				for (int k = 0; k <2 ; k++)					// segment each single number and svm
				{
					if (k == 1 && rectDot[k].x + rectDot[k].width >= roi.cols)	// cross the boarder
						rectDot[k].width = roi.cols - rectDot[k].x;

					Mat singleNum(roi, rectDot[k]);									// the single number image

					bool emtyFlag = roi.empty();
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					writeSamples(i, j, k, singleNum);

#endif

					resize(singleNum, singleNum, hog.winSize);
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < descriptorVector.size(); m++)
					{
						hogMat.at<float>(0, m) = descriptorVector[m];
					 
					}
					

					float result = numSVM.predict(hogMat);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					createClassifySamples(result, singleNum);

#endif


					tempSum += result * factor[0][k];
				}
			}
			else                // have no dot
			{
				for (int k = 0; k < 2 ; k++)
				{
					if (k == 1 && rectNoDot[k].x + rectNoDot[k].width >= roi.cols)
						rectNoDot[k].width = roi.cols - rectNoDot[k].x;
					Mat singleNum(roi, rectNoDot[k]);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					writeSamples(i, j, k+10, singleNum);

#endif

					resize(singleNum, singleNum, hog.winSize);
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < descriptorVector.size(); m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					createClassifySamples(result, singleNum);
#endif
					tempSum += result * factor[1][k];
				}
			}

			if (j == 0)
				dataOutput.WIN[i].dataValue = tempSum;
			if (j == 1)
				dataOutput.PLA[i].dataValue = tempSum;


		}			// end j

	}				// end i

	return EXEC_SUCCESS;
}

//识别 QIN QPL   同时识别ＱＩＮ　ＱＰＬ
int YZTDataIdentify::getQINQPLIdentify()
{

#ifdef QDEBUG_OUTPUT
	qDebug("getqinQPLPosStruct_live func \n");
#endif


	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}


	//判断识别标签是否为QIN or QPL
	judgeQINQPL();

	CvRect rect[3][3];

	// the first line for dot, the second line for no dot ，第三行为3位数
	float factor[3][3] = { { 1, 0.1, 0 }, { 10, 1, 0 }, { 100, 10, 1 } };			

	// svm DataIdentify each number

	int dotFlag;
	Mat qinQPLPosStructMat(image, WHOLE_QINQPL_POS_RECT);
	Mat qinQPLPosStructRegionSub1(image, QINQPL_POS_RECT);

	imwrite("image.bmp", image);

	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
		for (int j = 0; j < QIN_QPL_COL; j++)
		{

			if (i == j | j == i + 1)
				continue;

			if (qinQPLPosStruct.rect[i][j].width < 5 )
			{
				continue; 
			}
			 
	
			Mat roi(image, qinQPLPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = qinQPLPosStruct.rect[i][j].height;
			roiSize.width = qinQPLPosStruct.rect[i][j].width;
			Mat roiThreshold(roiSize, CV_8UC3);



			if (i == 5& j == 3 )
			{
				qDebug("Mark \n");
#ifdef  QDEBUG_OUTPUT
				qDebug("Mark \n");
#endif //  QDEBUG_OUTPUT


			}

			//colorThreshold(roi, roiThreshold, 150);

			roiThreshold = roi;
			cvtColor(roi, roi, CV_RGB2GRAY);

			cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);
			//	Canny(roi, edge, 450, 400, 3, true);

			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;
			
			 

			// 将阈值后的图像增强 roiThreshold 进行小数点判断
			for (int c = 0; c < roiThreshold.cols; c++)
			{
				for (int r = 0; r < roiThreshold.rows; r++)
				{
					if (roiThreshold.at<uchar>(r, c) < 100)
					{
						roiThreshold.at<uchar>(r, c) = 0;
					}
				}
			}

			trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);

			imwrite("QINQPL_roiThreshold.bmp", roiForDotJudge);

			roiNew = Mat(roi, roiNewSize);



			int *x = new int[3 + 1];
			memset(x, 0, 4);



			/*		imwrite("error.bmp", roi);
			imshow("roi", roi);
			waitKey(1000);*/

			Mat edge;
			dotFlag = judgeQINQPLDot(roiForDotJudge, edge, x);
			if (i == 4 & j ==2 )
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
			//
			rect[0][0].x = 0;	rect[0][1].x =7;		rect[0][2].x = 14;			// for two number with dot	
			rect[1][0].x = 0;	rect[1][1].x = 7;		rect[1][2].x = 14;			// for two number without dot
			//rect[2][0].x = 0;	rect[2][1].x = 11;		rect[2][2].x = 22;			// for three number without dot
			rect[2][0].x = 0;	rect[2][1].x =7;		rect[2][2].x = 14;			// for three number without dot

			for (int r = 0; r < 3; r++)													// set the rect for single number in number region
			{
				for (int c= 0; c < 3; c++)
				{
					rect[r][c].y = 0;
					if (j < 2)
					{
						rect[r][c].width = rect[r][c + 1].x - rect[r][c].x;
					}
					else
					{
						rect[r][c].width = roiNewSize.width - rect[r][c].x;

					}
					rect[r][c].height = qinQPLPosStruct.rect[r][c].height;
				}
			}

			 
			//Mat roiNew(roi, roiNewSize);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
			writeSamples(i, j, 6, roiForDotJudge);
#endif // WRITE_ROI_SMAPLES_CLASS



			if (dotFlag == -1)
				qDebug("YZTDataIdenfify GetQINQPLIDentify Dot judgment is wrong ! \n");
			//			imshow("edge", edge);
			//			waitKey();

			float tempSum = 0.0;
			if (dotFlag == 0)		//两个数字 有小数点
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
					rect[0][k].y = 0;
					rect[0][k].height = roiNew.rows;

					Mat singleNum(roiNew, rect[0][k]);								// the single number image
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					writeSamples(i, j, k, singleNum);
#endif
					resize(singleNum, singleNum, hog.winSize);
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < descriptorVector.size(); m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2					 
					createClassifySamples(result, singleNum);
#endif // WRITE_ROI_SMAPLES_CLASS


					tempSum += result * factor[0][k];
				}
			}

			if (dotFlag == 1)				//两个数字无小数点
			{
				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{
 
					// x[k]此时为 两个数字中间
					if (k == 1)
					{

						rect[1][k].width = roiNew.cols - x[k];
						rect[1][k].x = x[k];
					}

					else
					{
						rect[1][k].x = 0;
						rect[1][k].width = x[k] + 1;
					}
					rect[1][k].y = 0;
					rect[1][k].height = roiNew.rows;
					 
					Mat singleNum(roiNew, rect[1][k]);								// the single number image

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					writeSamples(i, j, k, singleNum);
#endif
					resize(singleNum, singleNum, hog.winSize);
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < descriptorVector.size(); m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					 
					createClassifySamples(result, singleNum);
#endif	

					tempSum += result * factor[1][k];
				}
			}

			if (dotFlag == 2)	// 3位数
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
					resize(singleNum, singleNum, hog.winSize);
					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < descriptorVector.size(); m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);
#ifdef  WRITE_ROI_SMAPLES_CLASS_INFO2
					createClassifySamples(result, singleNum);
#endif //  WRITE_ROI_SMAPLES_CLASS_INFO2

					tempSum += result * factor[2][k];
				}
			}
			if (dataOutput.isQPL)

			{
				dataOutput.QPL[i][j].dataValue = tempSum;

			}
			else
			{
				dataOutput.QIN[i][j].dataValue = tempSum;

			}

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

int YZTDataIdentify::getSessionIdentify()
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
	resize(roi, roi, hog.winSize);
	hog.compute(roi, descriptorVector, winStride, padding);
	for (int m = 0; m < descriptorVector.size(); m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result = numSVM.predict(hogMat);



	dataOutput.session = int(result);

#ifdef WRITE_SESSION_CLASSIFY_SAMPELS 
	createClassifySamples(result, roi);

#endif

	return EXIT_SUCCESS ;
}
 

//识别倒计时 
int YZTDataIdentify::getCountDownIdentify()
{

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//countDownMinutePosStruct.rect[1] = cvRect(512 + originX, 4 + originY, 11, 17);
	// 十位数
	Mat minuteRoi1(image, countDownMinutePosStruct.rect[0]);
	//个位数
	Mat minuteRoi2(image, countDownMinutePosStruct.rect[1]);


	//	imwrite("miniteRoi1.bmp", minuteRoi1);
	//	imwrite("minuteRoi2.bmp", minuteRoi2);
	cvtColor(minuteRoi1, minuteRoi1, CV_RGB2GRAY);
	resize(minuteRoi1, minuteRoi1, hog.winSize);
	hog.compute(minuteRoi1, descriptorVector, winStride, padding);
	for (int m = 0; m < descriptorVector.size(); m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result1 = raceTimeSvm.predict(hogMat);

	cvtColor(minuteRoi2, minuteRoi2, CV_RGB2GRAY);
	resize(minuteRoi2, minuteRoi2, hog.winSize);
	hog.compute(minuteRoi2, descriptorVector, winStride, padding);
	for (int m = 0; m < descriptorVector.size(); m++)
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
			return EXEC_WARNING ;							 // 未检测到分钟时间
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
			return EXEC_WARNING   ;
		}
		else
		{
			result1 = result1 * 10.0 + result2;
			// 分钟数不可能大于30 
			if (result1 > 30)
			{
				return EXEC_WARNING ;
			}
			dataOutput.raceTime = (int)(result1);  //倒计时为分钟数
		}
	}


	//	result1 += result1 * 10 + result2;
	//	dataOutput.raceTime = (int)(result1); //倒计时为分钟数
	return EXEC_SUCCESS;
}
 

// 通过计算 灰度在 X方向上的投影 之间的间隔Y坐标，得到 WIN PLA 各个数字区域
// 输入图像为灰度 canny变换后图像

int YZTDataIdentify::calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum)
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetWINPLARect :mat is empty \n");
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
		qDebug("calculateGraySumXForSetWINPLARect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD = 200;
	for (int i = 0; i < dimension; i++)
	{

		//设置为 300 过滤掉 部分数字的底色
		if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
			&& graySumX[i + 2] > THEREHOLD))
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetWINPLARect : The  y %d is  %d", j, i);
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
		qDebug("calculateGraySumXForSetWINPLARect :Func error  j != roiNum \n ");
#endif

		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


	return EXEC_SUCCESS;

}
// 通过计算 灰度在 X方向上的投影 之间的间隔Y坐标，得到 马名字的区间 
// 输入图像为灰度 canny变换后图像

int YZTDataIdentify::calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum)
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

	int * graySumX = new int[dimension+1];
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


	if (j  > HORSENUMBER )
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

bool YZTDataIdentify::judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge)
{
 
	Mat halfEdge;
	halfEdge = Mat(edge, cvRect(0,2,edge.cols,edge.rows/2-4)) ;
	/*
	Mat halfEdgeBelow;
	halfEdgeBelow = Mat(edge, cvRect(0, edge.rows / 2, edge.cols, edge.rows / 2));
	int* graySumBelow = new int[halfEdgeBelow.cols];
	*/
	//去除底色边缘条纹
	int* graySum = new int[halfEdge.cols+1];
	

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
		qDebug("DataIdentifywinPlaPosStruct_Dot_live: graySum[%d] = %d \n",c,graySum[c]);
#endif
	}

	graySum[0] = 0;  graySum[1] = 0;  
	graySum[halfEdge.cols] = 0;
	graySum[halfEdge.cols - 1] = 0;
	graySum[halfEdge.cols - 2] = 0;
 


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
	int nearBegin = 0;
	int nearEnd = 0;


	for (int i = 0; i < halfEdge.cols; i++)
	{
		if (graySum[i] != 0 )
		{
			nearBegin = i;
			break;
		}
	}

	for (int i = halfEdge.cols; i > 0 ; i--)
	{
		if (graySum[i] != 0)
		{
			nearEnd = i;
			break;
		}
	}

	for (int c = nearBegin ; c < nearEnd;c++)
	{
		if (graySum[c] <= 150 )
		{
			for (maxLengthAdd = 0; c + maxLengthAdd < halfEdge.cols; maxLengthAdd++)
			{
				if (graySum[c + maxLengthAdd] > 150)
				{
					break ;
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
					break ;
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
		if (maxLengthAdd > 0 )
		{
			maxLengthAdd--;
		}
		if (maxLengthMinus > 0 )
		{
			maxLengthMinus--;

		}
		if (maxLength <= maxLengthAdd + maxLengthMinus+1 )
		{
			maxLengthPos = c;
			maxLength	 = maxLengthAdd + maxLengthMinus + 1 ; // +1 将本身所在的位置加上
			 
#ifdef QDEBUG_OUTPUT
			qDebug("judgeWINPLADot: maxLengthPos%d,maxLength :%d \n", maxLengthPos, maxLength );
#endif
		}

		
	}

	if (graySum != NULL )
	{
		delete[] graySum;
		graySum = NULL;
	}

	if (maxLength >= 4 & maxLengthPos >7 & maxLengthPos < 14 )
	{
		return true;
	}
	else
	{
		return false;
	}
	
	 
}

int  YZTDataIdentify::judgeQINQPLDot(Mat &roi, Mat &edge, int *x)
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

	int* graySum = new int[edge.cols+1];
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
		if (graySum[count] >= 300 )
		{
			frontIndex = count;
			break;
		}


	}

	for (int count = edge.cols - 1; count >= 0; count--)
	{
		if (graySum[count] >= 300  )
		{
			backIndex = count;
			break;
		}


	}
	int delta = backIndex - frontIndex;
	

	delta = roi.cols;

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
		return EXIT_THIS_OCR  ;
	}
	else if (delta > 15 ) // three number . 17
	{
		//获取3个数字的 坐标值 x
		if (calculateGraySumYForQINDotJudge(edge, x, 3) != EXEC_SUCCESS)
		{
			x[0] = 0 ;
			x[1] = 6 ;
			x[2] = 12;
			algorithmState = EXEC_SUCCESS;

		}
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		return 2;
	}
	else if (delta <= 15 & delta >= 14  ) // tow number with dot .
	{
		 
		x[0] = 5;
		x[1] = 8;
		x[2] = 10;
		 
		return 0 ; //带小数点 2位小数	
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
  
	}
	else if (delta < 14 & delta >= 5) // two number 
	{

		x[0] = 4;
		x[1] = 5;
		 
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}

		return 1 ;
  

	}
	else
	{
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		algorithmState = EXIT_THIS_OCR;
#ifdef QDEBUG_OUTPUT
		qDebug("judgeQINQPLDot Function : algorithmSate = -1 \n");
#endif
		return EXIT_THIS_OCR ;
	}
 
	
}

 

int YZTDataIdentify::judgeQINQPL()
{
	if (algorithmState == EXIT_THIS_OCR )
	{
		return EXIT_THIS_OCR ;
	}
 
	Mat roi(image, QINQPL_LABEL_POS_RECT);
	// 	imshow("a", roi);
	// 	waitKey();

	cvtColor(roi, roi, CV_RGB2GRAY);

	resize(roi, roi, hog.winSize, 0, 0, INTER_CUBIC);
	hog.compute(roi, descriptorVector, winStride, padding);
	for (int m = 0; m < descriptorVector.size(); m++)
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
int  YZTDataIdentify::trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize)
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	int x0, x1;


	Mat edge;
	edge = oriMat;
//	Canny(oriMat, edge, 150, 100, 3, true);

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
int YZTDataIdentify::colorThreshold(Mat &src,Mat &dst, int thereshold)
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
	
	for (int i = 0; i < 3;i++)
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
			if (pixel[p] <= thereshold ) // adaptiveThreshold[p])
				pixel[p] = 0;
			dst.at<Vec3b>(r, c) = pixel;

		}
	}
	return EXEC_SUCCESS;
}

 


//横向投影calculateGraySumX(Mat &mat, int *y, int roiNum);
//输入图像为canny图像
// reverseY为统计 Y向投影 获取 y 值 。
int  YZTDataIdentify::calculateXBewttenNumber(Mat &mat, int  *x )
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
	int * graySumX = new int[dimension+1];

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
	int THEREHOLD =  100 ; // change to 200 from 300  to test 4.1 with background color . 20140701-2/1188.bmp
	for (int i = 0; i + dimension /2 < dimension;i++)
	{
		if (graySumX[dimension/2+i] < 100)
		{
			x[0] = i + dimension / 2;
			x[1] = x[0];
			break; 
		}
		if (graySumX[dimension/2-i] <100)
		{
			x[0] =  dimension / 2 - i;
			x[1] = x[0];
			break;
		}
	}
	
#ifdef QDEBUG_OUTPUT
	qDebug("calculateXBewttenNumber Func ： x0 = %d x1 =%d \n", x[0], x[1]);
#endif

	if ((x[1] - x[0]) > 12 | (x[1] - x[0]) < 0 )
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
	return EXEC_SUCCESS;
}

/*
//横向投影calculateGraySumX(Mat &mat, int *y, int roiNum);
//输入图像为canny图像
 
int  YZTDataIdentify::calculateGraySumX(Mat &mat, int  *y, int roiNum  )
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
 
int  YZTDataIdentify::calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum )
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
	graySumX = new int[dimension+1];
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelX = 0; pixelX < dimension; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{

			graySumX[pixelX] +=  mat.at<uchar>(pixelY, pixelX) ;
		}

#ifdef QDEBUG_OUTPUT
		qDebug("graySumX %d = %d \n", pixelX, graySumX[pixelX]);
#endif
	}
	int j = 0;
	int THEREHOLD = 200 ;
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
int  YZTDataIdentify::calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum)
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumY:mat is empty "), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim:mat is empty \n");
#endif
		 
		return  EXIT_THIS_OCR ;
	}

	int THERSHOLD = 100;
	//计算  纵向投影
	int * graySumY = new int[mat.cols+1];

	memset(graySumY, 0, sizeof(int)*mat.cols);
	for (int pixelX = 0; pixelX < mat.cols; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{
			graySumY[pixelX] += mat.at<uchar>(pixelY, pixelX);
		}
		graySumY[pixelX] /= roiNum ;
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim %d = %d \n", pixelX, graySumY[pixelX]);
#endif
	}

	//提取 两边y坐标
	for (int x = 0; x <= mat.cols; x++)
	{
		//排除掉 单条条纹 连续3个像素 》200 可认为发现了数字
		// 修改 由于连续三个像素 有时候会将 1 过滤掉了，所以只用1个，通过其他方法，改进 

		if (graySumY[x] > THERSHOLD)
		{
			x0 = x;
			break;
		}
	}
	for (int x = mat.cols - 1; x >= 0; x--)
	{//排除掉 单条条纹 连续3个像素 》 600 为了 去除 底色线条导致的小点 被误识别 300为了对1进行识别。
		if (graySumY[x] > THERSHOLD  & graySumY[x - 1] > THERSHOLD)
		{ 
			if (x + 2 >= mat.cols)
				x1 = mat.cols - 1;
			else 
				x1 = x  ; // add one pixel
			break;
		}

	}
#ifdef QDEBUG_OUTPUT
	qDebug("x0 = %d x1=%d", x0, x1);
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

int  YZTDataIdentify::calculateGraySumXForSetQINQPLRect(Mat &mat, int  *y, int roiNum )
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetQINQPLRect :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	横向投影
	int dimension = 0;

	
	dimension = mat.rows;
	
	int * graySumX = new int[dimension+1];	
	memset(graySumX, 0, sizeof(int)*dimension);

	for (int pixelY = 0; pixelY < dimension; pixelY++)
	{
		for (int pixelX = 0; pixelX < mat.cols; pixelX++)
		{

			graySumX[pixelY] += mat.at<uchar>(pixelY, pixelX);
		}
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumXForSetQINQPLRect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD =  100;
	for (int i = 0; i < dimension; i++)
	{
		if (i == 0 & graySumX[0] > THEREHOLD)
		{
			y[j] = i;
			j++;
		}
		else
		{
			//设置为 300 过滤掉 部分数字的底色
			if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
				&& graySumX[i + 2] > THEREHOLD))
			{
				y[j] = i;
#ifdef QDEBUG_OUTPUT
				qDebug(" calculateGraySumXForSetQINQPLRect : The  y %d is  %d", j, i);
#endif
				j++;

				i += 2;

				if (j > roiNum)
				{
					//Global::AppendLogString(QString("Error:calculateGraySumXForSetRect2 :Func error j>roiNum "), true);
#ifdef QDEBUG_OUTPUT
					qDebug("calculateGraySumXForSetQINQPLRect :Func error j>roiNum \n ");
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
		qDebug("calculateGraySumXForSetQINQPLRect :Func error  j != roiNum \n ");
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
void YZTDataIdentify::createClassifySamples(float result, Mat &singleNum)
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


void YZTDataIdentify::writeSamples(int i, int j, int k, Mat &roi)
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

