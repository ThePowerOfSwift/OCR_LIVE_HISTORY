#include "HK18D14DataIdentify.h"
#include <QDebug>
#include<fstream>


//#define  CALLBACK_MODE
using namespace std;


HK18D14DataIdentify::HK18D14DataIdentify()
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
	//	dataOutput.mHorseInfo.horseName[i] = "";
		dataOutput.mHorseInfo.isSCR[i] = false;
	}

	// initialize the DataOutput
	dataOutput.session = 0;
	dataOutput.raceTime = 0;

 
	for (int i = 0; i < dataOutput.horseNum ; i++)
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

 
	for (int i = 0; i < dataOutput.horseNum ; i++)
	{
		for (int j = 0; j < 3; j++)
			winPlaPosStruct.rect[i][j] = cvRect(0, 0, 0, 0);
	}

 
	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
		for (int j = 0; j < QIN_QPL_COL; j++)
			qinQPLPosStruct.rect[i][j] = cvRect(0, 0, 0, 0);
	}

	// ����ʱλ�� ��ʼ��
 
	for (int i = 0; i < 2; i++)
		countDownMinutePosStruct.rect[i] = cvRect(0, 0, 0, 0);

	// HOG������ʼ��
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

	// �����Ѿ�ѵ���õ����� 

	numSVM.load(".\\SVM\\HK18numberSvm.xml") ;
	LNSVM.load(".\\SVM\\LN_SVM_1.xml") ;

	horseNameSVM.load(".\\SVM\\horseName.xml") ;

	raceTimeSvm.load(".\\SVM\\raceTime.xml") ;
	sessionNumSvm.load(".\\SVM\\sessionNum.xml") ;

}

HK18D14DataIdentify::~HK18D14DataIdentify()
{

}


/*
	����һ��ͼ�����ݣ�Mat ����Ϊָ��
*/
bool HK18D14DataIdentify::read(Mat &srcMat, uchar* data, int length, int height, int width)
{
	// ��ʷ���ݶ���ΪMat ��ʵʱֱ������Ϊ data ָ��
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
		imwrite("srcMat.bmp", srcMat);
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
 

void  HK18D14DataIdentify::isReady()
{
	;
}


// ===========================================================
// private function
// ===========================================================

// DataIdentify the current frame has or not has any data
void HK18D14DataIdentify::haveData()
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

	if (blueMean > 50 && greenMean < 45 && redMean < 45)
	{
		haveDataFlag = true;
	}

	else
	{
		haveDataFlag = false;
	}

	dataOutput.haveDataFlag = haveDataFlag;

	// =====================================
	// QPl QIN ������ 
	CvPoint point[6];
	Vec3b region2_bgr[6];
	int x1 = QINQPL_POS_RECT.x - 30;
	int	x2 = QINQPL_POS_RECT.x + QINQPL_POS_RECT.width + 30;

	int y = QINQPL_POS_RECT.y, delta_y = 40; // 335 480 

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
		if (region2_blueMean > 40 && region2_greenMean < 50 && region2_redMean > 50)
		{
			haveDataFlag = true;
			cvtColor(image, image_temp, CV_RGB2BGR);
			//����״̬��ʼ��Ϊ�ɹ�
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
void HK18D14DataIdentify::originPosition()
{
	int regionWidth = ORIGINPOSITION_REGIONWIDTH, regionHeight = ORIGINPOSITION_REGIONHEIGHT;
	// 
	// 	imshow("a",image);
	// 	waitKey();
	// regionWidth regionHeight û�����ϡ�
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
 
 

int HK18D14DataIdentify::identify()
{

	haveData();
	if (haveDataFlag == false || algorithmState == EXIT_THIS_OCR)						// the frame has not any data, return 1
		return EXIT_THIS_OCR;
	//��������λ��

	setHorseNameRectPos();

	if (algorithmState == EXIT_THIS_OCR )
	{
		return EXIT_THIS_OCR;
	}
	// ���� WIN PLA λ��

	setWINPLARectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// ���� QIN QPL  ͬʱ����QIN QPL���λ�� 
	setQINQPLRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
 		return EXIT_THIS_OCR;
	}
	//  ���ó��κ�λ��

	setSessionRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//���� ����ʱλ�� 
	setCountDownRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//ʶ������ 
	getHorseNameIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// ʶ�� WIN PLA

	getWINPLAIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//ʶ�� QIN QPL   ͬʱʶ��ѣɣΡ��ѣУ�
	getQINQPLIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//ʶ�𳡴κ�

	getSessionIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//ʶ�𵹼�ʱ 

	getCountDownIdentify();

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	return EXEC_SUCCESS;
}


/*
	���� ������rect����
*/
int HK18D14DataIdentify::setHorseNameRectPos()
{
	Mat horseNameRegion(image, HORSENAME_REGION_RECT);

	//ת�Ҷ�ͼ��
	cvtColor(horseNameRegion, horseNameRegion, CV_RGB2GRAY);
	Mat edge;
	Canny(horseNameRegion, edge, 450, 400, 3, true);
	int *y = new int[HORSENUMBER + 1];
 
	//ͬʱ��ȡ�����Ŀ
	calculateGraySumXForSetHorseNameRect(edge, y, dataOutput.horseNum);
  
	for (int r = 0; r < dataOutput.horseNum; r++)
	{
		horseNamePosStruct.rect[r].x = HORSENAME_REGION_RECT.x;
		horseNamePosStruct.rect[r].width = HORSENAME_REGION_RECT.width;

		horseNamePosStruct.rect[r].y = HORSENAME_REGION_RECT.y + y[r];
		if (r == dataOutput.horseNum - 1)
		{
			//��ֹ�߶�̫��
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
 
// ���� WIN PLA λ��

int HK18D14DataIdentify::setWINPLARectPos()
{
	 
	Mat winRegion(image, WIN_POS_RECT);
	Mat plaRegion(image, PLA_POS_RECT);

	Mat plaThreshold(plaRegion.rows, plaRegion.cols,CV_8UC3);
	Mat winThreshold(winRegion.rows, winRegion.cols, CV_8UC3);

	//��ɫ��ֵ
	colorThreshold(winRegion, winThreshold, 80);
	colorThreshold(plaRegion, plaThreshold, 80);
	//ת�Ҷ�ͼ��
	cvtColor(winThreshold, winThreshold, CV_RGB2GRAY);
	cvtColor(plaThreshold, plaThreshold, CV_RGB2GRAY);

	//��ǿ�Ҷ�ͼ��

	for (int r = 0; r < plaThreshold.rows;r++)
	{
		for (int c = 0; c < plaThreshold.cols;c++)
		{
			if (plaThreshold.at<uchar>(r, c) < 20 )
			{
				plaThreshold.at<uchar>(r, c) = 0;
			}
			else
				plaThreshold.at<uchar>(r, c) = 250;
		}
	}

	for (int r = 0; r < winThreshold.rows; r++)
	{
		for (int c = 0; c < winThreshold.cols; c++)
		{
			if (winThreshold.at<uchar>(r, c) < 20 )
			{
				winThreshold.at<uchar>(r, c) = 0;
			}
			else
				winThreshold.at<uchar>(r, c) = 250;
		}
	}

//	Canny(winRegion, edgeWin, 150, 100, 3, true);
//	Canny(plaRegion, edgePla, 150, 100, 3, true);


	int *yWin = new int[HORSENUMBER + 1];
	int *yPla = new int[HORSENUMBER + 1];
	
	int x0, x1;

	calculateGraySumXForSetWINPLARect(winThreshold, yWin, dataOutput.horseNum);
	calculateGraySumXForSetWINPLARect(plaThreshold, yPla, dataOutput.horseNum);
	// get the relative position of the three vertex in the first row, relative to the origin 
	//
	imwrite("WIN_Region.bmp", winThreshold);
	imwrite("PLA_Region.bmp", plaThreshold);

	//���� ʶ�����������Xֵ
	if (calculateGraySumYForTrim(winThreshold, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
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
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		// 3������ �հ�	 		
		winPlaPosStruct.rect[i][0].x = WIN_POS_RECT.x + x0 - 2;
		winPlaPosStruct.rect[i][0].width = x1 - x0 + 4;

		
		winPlaPosStruct.rect[i][0].y = WIN_POS_RECT.y + yWin[i];

		if (i == dataOutput.horseNum - 1)
		{
			
			winPlaPosStruct.rect[i][0].height = winPlaPosStruct.rect[i - 1][0].height;
		}
		else
		{
			//����߶ȹ��ߣ���ô˵�������� ��������ƥ����ôǿ�����ø߶�Ϊ�̶�ֵ��
			if (yWin[i + 1] - yWin[i] > 18)
			{
				yWin[i + 1] = yWin[i] + NUMBER_HEIGHT + 2;
			}

			winPlaPosStruct.rect[i][0].height = yWin[i + 1] - yWin[i];
		}

		if (winPlaPosStruct.rect[i][0].width <= 20)
		{
			winPlaPosStruct.rect[i][0].width = NUMBER_WIDTH;
		}

		if (winPlaPosStruct.rect[i][0].height <= 10)
		{
			winPlaPosStruct.rect[i][0].height = NUMBER_HEIGHT;
		}

	}

	x0 = 0;
	x1 = 0;
	// PLA part 
	//���� ʶ�����������Xֵ
	if (calculateGraySumYForTrim(plaThreshold, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
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

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		// 3������ �հ�	 		
		winPlaPosStruct.rect[i][1].x = PLA_POS_RECT.x + x0 - 2;
		winPlaPosStruct.rect[i][1].width = x1 - x0 + 4;

		winPlaPosStruct.rect[i][1].y = PLA_POS_RECT.y + yWin[i];


		if (i == dataOutput.horseNum - 1)
		{
			winPlaPosStruct.rect[i][1].height = winPlaPosStruct.rect[i - 1][1].height;
		}
		else
		{

			winPlaPosStruct.rect[i][1].height = yWin[i + 1] - yWin[i];
		}

		if (winPlaPosStruct.rect[i][1].width <= 20)
		{
			winPlaPosStruct.rect[i][1].width = NUMBER_WIDTH;
		}

		if (winPlaPosStruct.rect[i][1].height <= 10)
		{
			winPlaPosStruct.rect[i][1].height = NUMBER_HEIGHT;
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
 

// ���� QIN QPL  ͬʱ����QIN QPL���λ�� 
 
int HK18D14DataIdentify::setQINQPLRectPos()
{
#ifdef QDEBUG_OUTPUT
	qDebug("setQINQPLRectLive");
#endif
 
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	

	Mat image_temp;
	image.copyTo(image_temp);

	//Mat qinQPLPosStructMat(image_temp, qinQPLPosStructMatRECT);

	Mat qinQPLPosStructRegionSub1(image_temp, QINQPL_POS_RECT );

	/*
	//Canny(roi, edge, 450, 400, 3, true);
	Mat lbRegionSub1(qinQPLPosStructRegionSub1, cvRect(0, 17, 43, 114) );
	Mat lbRegionSub1_edge;
	Canny(lbRegionSub1, lbRegionSub1_edge, 450, 400, 3, true);
	CvRect numberRect[7];
	*/
	colorThreshold(image_temp, image_temp, 120);
	cvtColor(image_temp, image_temp, CV_RGB2GRAY);

	 
	//��ͼ�� ��ǿ Ȼ�����С�����ж�
	for (int c = 0; c < image_temp.cols; c++)
	{
		for (int r = 0; r < image_temp.rows; r++)
		{
			if (image_temp.at<uchar>(r, c)  <  50)
			{
				image_temp.at<uchar>(r, c) = 0;
			}
			else
			{
				image_temp.at<uchar>(r, c) = 250;
			}
		}
	}
	 
	imwrite("ori.bmp", image_temp);

	int deltaNum = HORSENUMBER - dataOutput.horseNum;
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
			
#ifdef QDEBUG_OUTPUT
			qDebug("Error:setQINQPLRectLive_yp function : getqinQPLPosStructRect return EXIT THIS OCR i = %d\n", i);
#endif // QDEBUG_OUTPUT
			return EXIT_THIS_OCR;
		}

	}



	return EXEC_SUCCESS;

	 
}

//����canny �����ͼ��
int  HK18D14DataIdentify::setEveryQINQPLPos(Mat &mat, int rectNum)
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
	//ʵ����ƥ��Ŀ�� 14 ���� ����Ϊ������� ÿһ��region�����ֵĸ���ʱ�򣬻��õ�

	if (dataOutput.horseNum >14 | dataOutput.horseNum <= 0)
	{
		algorithmState = EXIT_THIS_OCR;

		return EXIT_THIS_OCR;

	}
	int delataNum = HORSENUMBER - dataOutput.horseNum;
	if (rectNum < 6 - delataNum) // 6-1
	{
		int * y = new int[6 - rectNum + 1]; // 6-1		
		//���� ʶ������ÿ�� ����֮���Yֵ
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
		// ʶ�����������Xֵ������ �ų�����ƥ��������14�����
		for (int i = rectNum + 1; i < 7 - delataNum; i++)
		{
			//���� ʶ�����������Xֵ
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
			// 3������ �հ�
			qinQPLPosStruct.rect[i][rectNum].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum].width = x1 - x0 + 4;
			//��������һ������
			qinQPLPosStruct.rect[i][rectNum].y = qinQplSubRect[rectNum].y + y[i - rectNum - 1] - 1;
			if (i == 6 - delataNum)
			{
				 

				//���һ����������ĸ߶ȿ���ʹ�õ�һ�У���0�еĸ߶�
				qinQPLPosStruct.rect[i][rectNum].height = qinQPLPosStruct.rect[1][0].height;
			}
			else
			{
				if (y[i - rectNum] - y[i - rectNum - 1] > 18)
				{
					y[i - rectNum] = y[i - rectNum - 1] + NUMBER_HEIGHT + 2;
				}

				// ��������y ����
				qinQPLPosStruct.rect[i][rectNum].height = y[i - rectNum] - y[i - rectNum - 1] + 1;
			}
		 
		}
		//qinQPLPosStruct.rect[][]

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
				qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
#endif			
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			// 3������ �հ�
			qinQPLPosStruct.rect[i][rectNum - 4].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum - 4].width = x1 - x0 + 4;
			//��������һ������
			qinQPLPosStruct.rect[i][rectNum - 4].y = qinQplSubRect[rectNum].y + y[i] - 1;
			if (i == roiNum - 1)
			{
				//�����ʱ �޷�ͨ������y ���õ��߶ȣ���ô����ǰ���Ѿ�������ĸ߶ȣ���Ϊ�߶�ֵ��������һ��
				qinQPLPosStruct.rect[i][rectNum - 4].height = qinQPLPosStruct.rect[1][0].height;
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
			// 3������ �հ�
			qinQPLPosStruct.rect[i][rectNum - 4].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum - 4].width = x1 - x0 + 4;
			//��������һ������
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
//  ���ó��κ�λ��

int HK18D14DataIdentify::setSessionRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	sessionPosStruct.rect[0] = SESSION_POS_RECT;
 
}

//���� ����ʱλ�� 
int HK18D14DataIdentify::setCountDownRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// ʮλ��
	countDownMinutePosStruct.rect[0] = COUNTDOWNMINUTE_POS_RECT1;
	//��λ��
	countDownMinutePosStruct.rect[1] = COUNTDOWNMINUTE_POS_RECT2;

	
}
//ʶ������ 
int HK18D14DataIdentify::getHorseNameIdentify()
{
	//��ʱ��ʶ��
	int temp = 0 ;
	return EXIT_SUCCESS;
}
// ʶ�� WIN PLA

int HK18D14DataIdentify::getWINPLAIdentify()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR ;
	}


	CvRect rectDot[3];
	CvRect rectNoDot[3];

	rectDot[0].x = 0;	rectDot[1].x = 9;		rectDot[2].x = 23 ;
	rectNoDot[0].x = 0; rectNoDot[1].x = 9;	rectNoDot[2].x = 18;
	
	float factor[2][3] = { { 10, 1, 0.1 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

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

			for (int i = 0; i < 3; i++)													// set the rect for single number in number region
			{
				rectDot[i].y = 0;
				rectDot[i].width = 8;
				rectDot[i].height = roiSize.height ;

				rectNoDot[i].y = 0;
				rectNoDot[i].width = 8;
				rectNoDot[i].height = roiSize.height ;
			}


			Mat roiThreshold(roiSize, CV_8UC3);
			bool emtyFlag = roi.empty();

			colorThreshold(roi, roiThreshold, 150);


			Mat roiThresholdEdge;
			cvtColor(roi, roi, CV_RGB2GRAY);
			//Canny(roi, edge, 150, 100, 3, true);

			//ר������ С������
			cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);

			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;

			trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);
			roiNew = Mat(roi, roiNewSize);
			/*

			colorThreshold(roi, roiThreshold, 160);


			cvtColor(roi, roi, CV_RGB2GRAY);
			cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);
			//	Canny(roi, edge, 450, 400, 3, true);

		
			*/
			 

			// ����ֵ���ͼ����ǿ roiThreshold ����С�����ж�
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
 
			if (i == 1 && j == 0 )
			{

				int temp = 0;
#ifdef QDEBUG_OUTPUT
				qDebug("getwinPlaPosStruct_live:Mark");
#endif // QDEBUG_OUTPUT

			}
			bool dotFlag = judgeWINPLADot(1, roiThreshold, roiThresholdEdge);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
			writeSamples(i, j, 6, roiNew);
#endif
 
			//�������� ��־ �������ȡ����������С��30 ˵���˵ط�Ϊ --- ����ƥ�Ѿ�����
			if (roiNew.cols < 20 )
			{
				dataOutput.mHorseInfo.isSCR[i] = true;
				dataOutput.WIN[i] = -1;
				dataOutput.PLA[i] = -1;
				continue ; 

			}

			float tempSum = 0.0;
			if (dotFlag)															// contain a dot
			{
				for (int k = 0; k < 3; k++)					// segment each single number and svm
				{
					if (k == 2 && rectDot[k].x + rectDot[k].width >= roiNew.cols)	// cross the boarder
						rectDot[k].width = roiNew.cols - rectDot[k].x;

					Mat singleNum(roiNew, rectDot[k]);									// the single number image

			 
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
					if (k == 2 && rectNoDot[k].x + rectNoDot[k].width >= roiNew.cols)
						rectNoDot[k].width = roiNew.cols - rectNoDot[k].x;
					 
					Mat singleNum(roiNew, rectNoDot[k]);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					
					writeSamples(i, j, k, singleNum); 
					
#endif

					resize(singleNum, singleNum, cvSize(10, 20));

					hog.compute(singleNum, descriptorVector, winStride, padding);
					for (int m = 0; m < HOGFEATURENUMBER; m++)
						hogMat.at<float>(0, m) = descriptorVector[m];

					float result = numSVM.predict(hogMat);

					createClassifySamples(result, singleNum);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
					createClassifySamples(result, singleNum);
					
#endif
					tempSum += result * factor[1][k];
				}
			}

			if (j == 0)
				dataOutput.WIN[i] = tempSum;
			if (j == 1)
				dataOutput.PLA[i] = tempSum;


		}			// end j

	}				// end i

	return EXEC_SUCCESS;
}

//ʶ�� QIN QPL   ͬʱʶ��ѣɣΡ��ѣУ�
int HK18D14DataIdentify::getQINQPLIdentify()
{

#ifdef QDEBUG_OUTPUT
	qDebug("getqinQPLPosStruct_live func \n");
#endif


	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}


	//�ж�ʶ���ǩ�Ƿ�ΪQIN or QPL
	judgeQINQPL();

	CvRect rect[3][3];


	float factor[3][3] = { { 1, 0.1, 0 }, { 10, 1, 0 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

	// svm DataIdentify each number

	int dotFlag;
	Mat qinQPLPosStructMat(image, WHOLE_QINQPL_POS_RECT);
	Mat qinQPLPosStructRegionSub1(image, QINQPL_POS_RECT);

	//imwrite("image.bmp", image);
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

			//���̫С�����˵�
			if (qinQPLPosStruct.rect[i][j].width < 5 )
			{
				continue; 
			}

			//���˵���������ƥ���� �����ý��Ϊ-1 
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
		

			qinQPLPosStruct.rect[i][j].width = qinQPLPosStruct.rect[i][j].width; //����2�����ء�
	
			Mat roi(image, qinQPLPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = qinQPLPosStruct.rect[i][j].height;
			roiSize.width = qinQPLPosStruct.rect[i][j].width;
			Mat roiThreshold(roiSize, CV_8UC3);



			if (i == 0 & j == 10 )
			{
				qDebug("Mark \n");
#ifdef  QDEBUG_OUTPUT
				qDebug("Mark \n");
#endif //  QDEBUG_OUTPUT


			}

		 

			colorThreshold(roi, roiThreshold, 160);


			cvtColor(roi, roi, CV_RGB2GRAY);
			cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);
			//	Canny(roi, edge, 450, 400, 3, true);

			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;
			
			trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);

		 
			// ����ֵ���ͼ����ǿ roiThreshold ����С�����ж�
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

			

			roiNew = Mat(roi, roiNewSize);



			int *x = new int[3 + 1];
			memset(x, 0, 4);



			/*		imwrite("error.bmp", roi);
			imshow("roi", roi);
			waitKey(1000);*/
			//Mat roiNew(roi, roiNewSize);
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
			writeSamples(i, j, 6, roiForDotJudge);
#endif // WRITE_ROI_SMAPLES_CLASS

			Mat edge;
			dotFlag = judgeQINQPLDot(roiForDotJudge, edge, x);
			if (i == 0 & j == 5)
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
				qDebug("DataIdentifyImageInfor2_Dot_live_yp is wrong ! \n");
			//			imshow("edge", edge);
			//			waitKey();

			float tempSum = 0.0;
			if (dotFlag == 0)				 		// two number with dot
			{
				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{
					// x[k]��ʱΪ ���������м�
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

				//	if (k == 1 && rect[1][k].x + rect[1][k].width >= roiNew.cols)	// cross the boarder
				//		rect[1][k].width = roiNew.cols - rect[1][k].x;

					// x[k]��ʱΪ ���������м�
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
//ʶ�𳡴κ�

int HK18D14DataIdentify::getSessionIdentify()
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

	return EXIT_SUCCESS ;
}
 

//ʶ�𵹼�ʱ 
int HK18D14DataIdentify::getCountDownIdentify()
{

	//countDownMinutePosStruct.rect[1] = cvRect(512 + originX, 4 + originY, 11, 17);
	// ʮλ��
	Mat minuteRoi1(image, countDownMinutePosStruct.rect[0]);
	//��λ��
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
	//	dataOutput.raceTime = (int)(result2*10); //����ʱΪ������


	if (result1 == -1)
	{
		if (result2 == -1)
		{

			dataOutput.raceTime = -1;
			return EXEC_WARNING ;							 // δ��⵽����ʱ��
		}
		else
		{
			dataOutput.raceTime = (int)result2;  //����ʱΪ������
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
			// �����������ܴ���30 
			if (result1 > 30)
			{
				return EXEC_WARNING ;
			}
			dataOutput.raceTime = (int)(result1);  //����ʱΪ������
		}
	}


	//	result1 += result1 * 10 + result2;
	//	dataOutput.raceTime = (int)(result1); //����ʱΪ������

}
 

// ͨ������ �Ҷ��� X�����ϵ�ͶӰ ֮��ļ��Y���꣬�õ� WIN PLA ������������
// ����ͼ��Ϊ�Ҷ� canny�任��ͼ��

int HK18D14DataIdentify::calculateGraySumXForSetWINPLARect(Mat &mat, int *y, int &horseNum)
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetHorseNameRect :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	����ͶӰ
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

		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
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
// ͨ������ �Ҷ��� X�����ϵ�ͶӰ ֮��ļ��Y���꣬�õ� �����ֵ����� 
// ����ͼ��Ϊ�Ҷ� canny�任��ͼ��

int HK18D14DataIdentify::calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum)
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetHorseNameRect :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	����ͶӰ
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

		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
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
	//��ȡ�����Ŀ
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

ͨ����� ����ͼƬ�ϰ벿�� �հ�����Ĵ�С�Լ� λ���������Ƿ����С����
*/

bool HK18D14DataIdentify::judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge)
{
 
	Mat halfEdge;
	halfEdge = Mat(edge, cvRect(0,0,edge.cols,edge.rows/2)) ;
	/*
	Mat halfEdgeBelow;
	halfEdgeBelow = Mat(edge, cvRect(0, edge.rows / 2, edge.cols, edge.rows / 2));
	int* graySumBelow = new int[halfEdgeBelow.cols];
	*/
	//ȥ����ɫ��Ե����
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

	graySum[0] = 0;  graySum[1] = 0;  graySum[2] = 0;
	graySum[halfEdge.cols] = 0;
	graySum[halfEdge.cols - 1] = 0;
	graySum[halfEdge.cols - 2] = 0;
	graySum[halfEdge.cols - 3] = 0;


	//��������
	int  maxLengthPos = 0;
	int maxLengthAdd = 0; //����
	int maxLengthMinus = 0; // ����
	int maxLength = 0;
	/*
	//����°��� ����Ϊ������λ���֣��м�ļ��ϴ���ʶ���ˡ�
	int  maxLengthPosBelow = 0;
	int maxLengthAddBelow = 0; //����
	int maxLengthMinusBelow = 0; // ����
	int maxLengthBelow = 0;

	*/
	for (int c = 0; c < halfEdge.cols;c++)
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
		//��������Ϊ����ĳ�����ȷֵ1 ��
		if (maxLengthAdd > 0 )
		{
			maxLengthAdd--;
		}
		if (maxLengthMinus > 0 )
		{
			maxLengthMinus--;

		}
		if (maxLength < maxLengthAdd + maxLengthMinus )
		{
			maxLengthPos = c;
			maxLength	 = maxLengthAdd + maxLengthMinus + 1 ; // +1 ���������ڵ�λ�ü���
			 
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

	if (maxLength >= 5 & maxLengthPos >15 & maxLengthPos < 25 )
	{
		return true;
	}
	else
	{
		return false;
	}
	
	 
}

int  HK18D14DataIdentify::judgeQINQPLDot(Mat &roi, Mat &edge, int *x)
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
	else if (delta > 24 ) // three number . 20
	{
		//��ȡ3�����ֵ� ����ֵ x
		if (calculateGraySumYForQINDotJudge(edge, x, 3) != EXEC_SUCCESS)
		{
			x[0] = 0 ;
			x[1] = 8 ;
			x[2] = 18 ;
			algorithmState = EXEC_SUCCESS;

		}
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		return 2;
	}
	else if (delta <= 24 & delta >= 20  ) // tow number with dot .
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
			//����Y��ͶӰͨ��reverseY����Ϊ true
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

			return 2; //��λ��		 
		}			
		else
		{
			 //��λС��
			if (calculateXBewttenNumber(halfEdge, x) != EXEC_SUCCESS)
			{
				x[0] = 6;
				x[1] = 10 ;

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
	else if (delta < 20 & delta >= 5) // two number 
	{
		//��λ������ֹ ��λС�� ���� ���м�⣬ ͨ������Ѿ��޷����� ��λС������λ�� ��1.1 ��99
		Mat halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows / 2));
		if (calculateXBewttenNumber(halfEdge, x) != EXEC_SUCCESS)
		{
			x[0] = 5;
			x[1] = 6;
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}
			return 1;

		}
		else
		{
			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}

			//��϶��������Ϊ С����
			if ((x[1] - x[0]) > 4 )
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
		return EXIT_THIS_OCR ;
	}
 
	
}

 

int HK18D14DataIdentify::judgeQINQPL()
{
	if (algorithmState == EXIT_THIS_OCR )
	{
		return EXIT_THIS_OCR ;
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
�ü�ROI�������Ŀհ�����
*/
int  HK18D14DataIdentify::trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize)
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	int x0, x1;


	Mat edge;
	edge = oriMat;
//	Canny(oriMat, edge, 150, 100, 3, true);

	//������С�ߴ�

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



//�Բ�ɫͼ�� 3��RGB��ɫͨ��������ֵ���� ���� thereshold��ֵ �� 0 
// Ϊ�˸��õؽ�����ֵ����������ƽ��ֵ��Ȼ������ƽ��ֵ������ֵ
int HK18D14DataIdentify::colorThreshold(Mat &src,Mat &dst, int thereshold)
{
	//�Բ�ɫͼ������ֵ ȡ��

	int rgbSum[3] = { 0 };

	Vec3b pixel; // B G R
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
	
		 
	
	for (int c = 0; c < src.cols; c++)
	{
		for (int r = 0; r < src.rows; r++)
		{
			pixel = src.at<Vec3b>(r, c);		// row,col index (NOT x,y)
			for (int p = 0; p < 3; p++)
			{
				if (p == 1 )
				{
					if (pixel[p] <= thereshold + 50 ) // adaptiveThreshold[p])
						pixel[p] = 0;
				}
				else
				{
					if (pixel[p] <= thereshold) // adaptiveThreshold[p])
						pixel[p] = 0;

				}
				

			}
			
			dst.at<Vec3b>(r, c) = pixel;

		}
	}
	return EXEC_SUCCESS;
}

 


//����ͶӰcalculateGraySumX(Mat &mat, int *y, int roiNum);
//����ͼ��Ϊcannyͼ��
// reverseYΪͳ�� Y��ͶӰ ��ȡ y ֵ ��
int  HK18D14DataIdentify::calculateXBewttenNumber(Mat &mat, int  *x )
{
	if (mat.empty())
	{
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumX :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	����ͶӰ
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
	int THEREHOLD =  200 ; // change to 200 from 300  to test 4.1 with background color . 20140701-2/1188.bmp
	for (int i = dimension / 3 - 1; i < dimension; i++)
	{
		if (graySumX[i] <= THEREHOLD)
		{
			x[0] = i;
			break;
		}

	}

	for (int i = dimension *2 / 3 -1 ; i > 0 ; i--)
	{
		if (graySumX[i] <= THEREHOLD)
		{
			x[1] = i;
			break;
		}

	}

	for (int i = x[0]; i< x[1]; i ++)
	{
		if (graySumX[i] > THEREHOLD )
		{
#ifdef QDEBUG_OUTPUT
			qDebug("calculateXBewttenNumber: it is a tow number without dot .");
#endif
			return EXIT_THIS_OCR;
		}
	}

	if ((x[1] - x[0]) > 12 | (x[1] - x[0]) < 0 )
	{
	
#ifdef QDEBUG_OUTPUT
		qDebug("Error :calculateXBewttenNumber: x[1] %d x[0] %d distance is too large or zero .",x[1],x[0]);
#endif
		 
		if (graySumX != NULL)
		{
			delete[] graySumX;
			graySumX = NULL;
		}
		return EXIT_THIS_OCR;
	}
	else
	{
#ifdef  QDEBUG_OUTPUT
		qDebug("calculateXBewttenNumber: x0 = %d x1 =%d \n",x[0],x[1] );
#endif //  QDEBUG_OUTPUT

		if (graySumX != NULL)
		{
			delete[] graySumX;
			graySumX = NULL;
		}
		return EXEC_SUCCESS;
	}
}

/*
//����ͶӰcalculateGraySumX(Mat &mat, int *y, int roiNum);
//����ͼ��Ϊcannyͼ��
 
int  HK18D14DataIdentify::calculateGraySumX(Mat &mat, int  *y, int roiNum  )
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("ERROR : calculateGraySumX :mat is empty \n"),true) ;
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumX :mat is empty \n");
#endif //  QDEBUG_OUTPUT

		
		return EXIT_THIS_OCR ;

	}
	 
	//	����ͶӰ
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
		//��ֹ��һ�����ؾ��� ���ֲ���
		if (i == 0 && (graySumX[0] >= THEREHOLD || graySumX[0] < THEREHOLD &&
			graySumX[i + 1] > THEREHOLD && graySumX[i + 2] > THEREHOLD))
		{	
			y[j] = 0;
			j++;
			//����1����ֹ������С�� 300�ġ� 
			i += 2;
		
		}
		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
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

//����ͶӰcalculateGraySumX(Mat &mat, int *y, int roiNum);
//����ͼ��Ϊcannyͼ��
 
int  HK18D14DataIdentify::calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum )
{
	if (mat.empty())
	{
		//Global::AppendLogString(QString("ERROR : calculateGraySumX :mat is empty \n"),true) ;
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumYForQINDotJudge :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	����ͶӰ
	
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
		//��ֹ��һ�����ؾ��� ���ֲ���
		if (i == 0 && (graySumX[0] >= THEREHOLD || graySumX[0] < THEREHOLD &&
			graySumX[i + 1] > THEREHOLD && graySumX[i + 2] > THEREHOLD))
		{
			x[j] = 0;
			j++;
			//����1����ֹ������С�� 300�ġ� 
			//i += 2;

		}
		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
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


//��������ͶӰ
int  HK18D14DataIdentify::calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum)
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
	//����  ����ͶӰ
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

	//��ȡ ����y����
	for (int x = 0; x <= mat.cols; x++)
	{
		//�ų��� �������� ����3������ ��200 ����Ϊ����������
		// �޸� ���������������� ��ʱ��Ὣ 1 ���˵��ˣ�����ֻ��1����ͨ�������������Ľ� 

		if (graySumY[x] > THERSHOLD)
		{
			x0 = x;
			break;
		}
	}
	for (int x = mat.cols - 1; x >= 0; x--)
	{//�ų��� ��������  
		if (graySumY[x] > THERSHOLD  )
		{ 
			if (x + 2 >= mat.cols)
				x1 = mat.cols - 1;
			else 
				x1 = x + 2 ; // add one pixel
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


//����ͶӰcalculateGraySumXForSetRect2(Mat &mat, int *y, int roiNum);
//����ͼ��Ϊcannyͼ��

int  HK18D14DataIdentify::calculateGraySumXForSetQINQPLRect(Mat &mat, int  *y, int roiNum )
{

	if (mat.empty())
	{
		//Global::AppendLogString(QString("Error:calculateGraySumX :mat is empty "), true);
#ifdef  QDEBUG_OUTPUT
		qDebug("ERROR : calculateGraySumXForSetRect2 :mat is empty \n");
#endif //  QDEBUG_OUTPUT


		return EXIT_THIS_OCR;

	}

	//	����ͶӰ
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
		qDebug("calculateGraySumXForSetRect2 %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD =  300;
	for (int i = 0; i < dimension; i++)
	{
		if (i==0 &graySumX[0] > 300)
		{
			y[j] = i;
			j++;
		}
		else
		{
			//����Ϊ 300 ���˵� �������ֵĵ�ɫ
			if ((graySumX[i] < 300 && graySumX[i + 1] > 300
				&& graySumX[i + 2] > 300))
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
					qDebug("calculateGraySumXForSetRect2 :Func error j =%d >roiNum=%d \n ",j,roiNum );
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
	//��ȡ ����y����
}



/*
��ʶ��Ľ������д�� ��ӦĿ¼ ��ҪΪ����
*/
void HK18D14DataIdentify::createClassifySamples(float result, Mat &singleNum)
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

	//�˵���һ��Ŀ¼
	//	QDir::setCurrent("../");
	//	QDir::setCurrent("../");
	QDir::setCurrent("E://mvs//OCR_git//OCRProject");
	//curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));





}


void HK18D14DataIdentify::writeSamples(int i, int j, int k, Mat &roi)
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

	//�˵���һ��Ŀ¼
	QDir::setCurrent("../");


	//curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));

 
}

