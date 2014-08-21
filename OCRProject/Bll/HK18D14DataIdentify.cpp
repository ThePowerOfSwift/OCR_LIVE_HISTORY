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

		memset(dataOutput.mHorseInfo.horseName[i], 0, sizeof(wchar_t)* 4);
		dataOutput.mHorseInfo.horseID[i] = 0;
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

	numSVM.load(".\\SVM\\HK18D14Number.xml") ;
	LNSVM.load(".\\SVM\\LN_SVM_1.xml") ;

	horseNameSVM.load(".\\SVM\\horseName.xml") ;

	raceTimeSvm.load(".\\SVM\\raceTime.xml") ;
	sessionNumSvm.load(".\\SVM\\sessionNum.xml") ;

	// 
	dataOutput.horseNameChangedNum = 0;



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
		bgr[i] = image_temp.at<Vec3b>(HAVEDATA_YINDEX * (i + 1), 42);
		bluePixel[i] = bgr[i][0];
		greenPixel[i] = bgr[i][1];
		redPixel[i] = bgr[i][2];

		blueMean += bluePixel[i] / HAVEDATA_PIXELNUMBER;
		greenMean += greenPixel[i] / HAVEDATA_PIXELNUMBER;
		redMean += redPixel[i] / HAVEDATA_PIXELNUMBER;
	}

	if (blueMean > 50 && greenMean < 60 && redMean < 60 )
	{
		haveDataFlag = true;
	}

	else
	{
		haveDataFlag = false;
	}

	

	// =====================================
	// QPl QIN ������ 
	CvPoint point[6];
	Vec3b region2_bgr[6];
	
	int x1 = 40, x2 = 670;
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
		if (region2_blueMean > 40 && region2_greenMean < 60 && region2_redMean > 50)
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
	dataOutput.haveDataFlag = haveDataFlag;


	 
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

	//	qDebug("rowSum[%d] = %d ", i, rowSum[i]);
	}


	// calculate the col sum
	for (int i = 0; i < regionWidth; i++)
	{
		colSum[i] = 0;
		for (int j = 0; j < regionHeight; j++)
			colSum[i] += regionGray.at<uchar>(j, i);

		colSum[i] = colSum[i] / regionHeight;

	//	qDebug("colSum[%d] = %d ",i,colSum[i]);
	}

	// DataIdentify the originX
	for (int i = 0; i < regionWidth; i++)
	{
		if (colSum[i] > 60 ) // 
		{
			originX = i - 1;
			break;
		}
	}
	// DataIdentify the originY
	for (int i = 0; i < regionHeight; i++)
	{
		if (rowSum[i] > 70 )
		{
			originY = i - 1;
			break;
		}
	}
 

	qDebug("the originPosition Func : x =%d, y=%d",originX,originY);
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

	originPosition();
	/*
	Mat test(image, cvRect(originX, originY,100,100) );
	
	imshow("a", test);
	waitKey();
	*/
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

	//���˵� �ͻҶ�ֵ����
	
	for (int c = 0; c < horseNameRegion.cols;c++)
	{
		for (int r = 0; r < horseNameRegion.rows;r++)
		{
			if (horseNameRegion.at<uchar>(r,c) < 100 )
			{
				horseNameRegion.at<uchar>(r, c) = 0;
			}
		}
	}

	
	Mat edge;
	//Canny(horseNameRegion, edge, 150, 100, 3, true);

	edge = horseNameRegion;


#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");
	writeSamples(QString("horseNameRegion.bmp"), edge, path);
#endif

	 
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
#ifdef WRITE_ROI_SMAPLES_TEMP
		QString path = QString(".//temp//");
		writeSamples(fileName , horseNameRect,path);
#endif
	 
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
	 
	//��ͼƬ��Ԥ���� �Ҷȣ�Ȼ��Ҷ���ֵ
	Mat imageGray;

	image.copyTo(imageGray);
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	for (int c = 0; c < imageGray.cols; c++)
	{
		for (int r = 0; r < imageGray.rows; r++)
		{
			if (imageGray.at<uchar>(r, c) < 90 )
			{
				imageGray.at<uchar>(r, c) = 0;
			}
		}
	}
	Mat winRegion(imageGray, WIN_POS_RECT);
	Mat plaRegion(imageGray, PLA_POS_RECT);

#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");

	writeSamples(QString("WIN_Region.bmp"), winRegion, path);
	writeSamples(QString("PLA_Region.bmp"), plaRegion, path);

#endif

 
	int *yWin = new int[HORSENUMBER + 1];
	int *yPla = new int[HORSENUMBER + 1];
	
	int x0, x1;

	if (calculateGraySumXForSetWINPLARect(winRegion, yWin, dataOutput.horseNum) == EXIT_THIS_OCR )
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	
	if (calculateGraySumXForSetWINPLARect(plaRegion, yPla, dataOutput.horseNum)  == EXIT_THIS_OCR)
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	// get the relative position of the three vertex in the first row, relative to the origin 
	//
	
	//���� ʶ�����������Xֵ
	if (calculateGraySumYForTrim(winRegion, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
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
		//		yWin[i + 1] = yWin[i] + NUMBER_HEIGHT + 2;
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
	if (calculateGraySumYForTrim(plaRegion, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
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

		winPlaPosStruct.rect[i][1].y = PLA_POS_RECT.y + yPla[i];


		if (i == dataOutput.horseNum - 1)
		{
			winPlaPosStruct.rect[i][1].height = winPlaPosStruct.rect[i - 1][1].height;
		}
		else
		{

			winPlaPosStruct.rect[i][1].height = yPla[i + 1] - yPla[i];
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
	
//��ȡһ��QIN QPL λ��
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

	Mat image_temp;
	image.copyTo(image_temp);

	//Mat qinQPLPosStructMat(image_temp, qinQPLPosStructMatRECT);

	Mat qinQPLPosStructRegionSub1(image_temp, QINQPL_POS_RECT );

	//����ɫ��ֵ
	Vec3b pixel; // B G R
	int sum = 0 ;
	for (int c = 0; c < image_temp.cols; c++)
	{
		for (int r = 0; r < image_temp.rows; r++)
		{
			pixel = image_temp.at<Vec3b>(r, c); //  

			sum = (pixel[0] - 170) * (pixel[0] - 170);

			sum += (pixel[1] - 160) * (pixel[1] - 160);

			sum += (pixel[2] - 168) * (pixel[2] - 168);

			if (sum >= 10000 )
			{
				pixel[0] = 0;
				pixel[1] = 0;
				pixel[2] = 0;
			}
			image_temp.at<Vec3b>(r, c) = pixel;
			sum = 0;
		}
	}

#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");

	writeSamples(QString("ori1.bmp"), image_temp, path);
	 

#endif

	 
	//colorThreshold(image_temp, image_temp, 120);
	cvtColor(image_temp, image_temp, CV_RGB2GRAY);
	 
#ifdef WRITE_ROI_SMAPLES_TEMP
	 

	writeSamples(QString("ori2.bmp"), image_temp, path);


#endif
 

	int deltaNum = HORSENUMBER - dataOutput.horseNum;
	for (int i = 0; i < 19 - deltaNum; i++)
	{
		//�����ƥ��������8 ����ô ǰ6�����򶼲���ʶ����
		if (deltaNum >= 6 )
		{
			if (i < 6)
			{
				continue; 
			}
		}
		QString fileName;
		Mat qinQPLPosStructRoiEdge;

		Mat qinQPLPosStructRoi(image_temp, qinQplSubRect[i]);
		//Canny(qinQPLPosStructRoi, qinQPLPosStructRoiEdge, 200, 150, 3, true);

		fileName.prepend(".bmp");
		fileName.prepend(QString::number(i));

#ifdef WRITE_ROI_SMAPLES_TEMP
		QString path = QString(".//temp//");

		writeSamples(fileName, qinQPLPosStructRoi, path);


#endif
		 


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
				qinQPLPosStruct.rect[i][rectNum].height = NUMBER_HEIGHT ;
			}
			else
			{
				if (y[i - rectNum] - y[i - rectNum - 1] > 18)
				{
					y[i - rectNum] = y[i - rectNum - 1] + NUMBER_HEIGHT  ;
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
				qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT ;
			}
			else
			{
				if (y[i + 1] - y[i ] > 18)
				{
					y[i +1] = y[i ] + NUMBER_HEIGHT  ;
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
				qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT;
			}
			else
			{
				//Ϊ���ų���   ������ƥ �����ָ߶�̫�ߡ�
				if (y[i + 1] - y[i] > 18)
				{
					y[i + 1] = y[i] + NUMBER_HEIGHT ;
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
	//ͨ������Ҷ�ֵ�ͣ���ȷ���Ƿ����������ֵı仯�������ַ����˱仯��˵��
	// ���κŷ����˱仯��

	isHorseNameChanged();
	//��ʱ��ʶ��
	int temp = 0;
	return EXIT_SUCCESS;
 
}

//ͨ������Ҷ�ֵ�ͣ���ȷ���Ƿ����������ֵı仯�������ַ����˱仯��˵��
int HK18D14DataIdentify::isHorseNameChanged()
{

	//�����һ�ε� 
	 

	//�洢�����ĻҶȺ�
	int *graySum ;
	graySum = new int  [dataOutput.horseNum + 1];
	int  ChangedNum = 0;
	int graySumThreshold = 3000 ;
	int *length;

	length = new int[dataOutput.horseNum + 1];
	
	memset(graySum, 0, sizeof(int)* dataOutput.horseNum);
	memset(length, 0, sizeof(int)* dataOutput.horseNum);

 	for (int h = 0; h < dataOutput.horseNum;h ++)
	{
		
		Mat horseNameRegion(image, horseNamePosStruct.rect[h]);
		cvtColor(horseNameRegion, horseNameRegion, CV_RGB2GRAY);
		Mat horseNameRegionEdge;
		//Canny(horseNameRegion, horseNameRegionEdge, 450, 400, 3, true);
		 
		graySum[h] = calculateGraySum(horseNameRegion,length[h]);

		
		if ( abs(graySum[h] - dataOutput.mHorseInfo.graySum[h]) > graySumThreshold 
			& abs(length[h]-dataOutput.mHorseInfo.length[h]) > 4 ) 
		{
			ChangedNum++;
#ifdef QDEBUG_OUTPUT
			qDebug("  graySum[%d] = %d ,pri = %d \n",h,
				graySum[h], dataOutput.mHorseInfo.graySum[h]);
#endif
			ChangedNum++;

		}
	
		dataOutput.mHorseInfo.graySum[h] = graySum[h];
		dataOutput.mHorseInfo.length[h] = length[h];
	}
	 
	if (ChangedNum > 1 )
	{
#ifdef QDEBUG_OUTPUT
		qDebug("  horseNameChangedNum = %d \n",
			dataOutput.horseNameChangedNum);
#endif
		Global::session++;

		//��ʱ������ ���µĳ��κš� �������ʷ��Ƶͨ�� ����֡����ʵ��
		Global::timerCount = 0;
		Global::historyFrameCount = 0;
		Global::countRaceTime = 0;

		Global::isSessionChanged = true;
		//dataOutput.horseNameChangedNum++;
	}
	//����˹�У���˳��κţ���ô��ʱ ���¸�ֵ���κš�
	if (Global::isSessioncalibrated)
	{
		dataOutput.horseNameChangedNum = Global::session;

		//��ʱ������ ���µĳ��κš� �������ʷ��Ƶͨ�� ����֡����ʵ��
		Global::timerCount = 0;
		Global::historyFrameCount = 0;

		Global::isSessionChanged = true;
		//˳��ʱ
		Global::countRaceTime = 0;

	}
	delete[] graySum;


	return EXEC_SUCCESS;


}
//����Ҷ�ֵ�ͣ�����ֵ��Ϊ�Ҷ�ֵ�� ͬʱ����ֵ
int HK18D14DataIdentify::calculateGraySum(Mat srcMat,int &length)
{
	int graySum = 0 ;

	for (int c = 0; c < srcMat.cols;c++)
	{
		for (int r = 0; r < srcMat.rows;r++)
		{
			if (srcMat.at<uchar>(r,c) < 100 )
			{
				srcMat.at<uchar>(r, c) = 0 ;
			}
			graySum += srcMat.at<uchar>(r, c);
		}
	}

	int *sum;
	sum = new int[srcMat.cols + 1];
	memset(sum, 0, sizeof(int)*srcMat.cols);

	for (int c = 0; c < srcMat.cols; c++)
	{
		for (int r = 0; r < srcMat.rows; r++)
		{
			sum[c] += srcMat.at<uchar>(r, c);
		}
	}

	for (int c = srcMat.cols-1 ; c >= 0 ; c-- )
	{
		if (sum[c] > 200 )
		{
			length = c;
			break;
		}
	}
	delete[] sum;
	return graySum;


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

	rectDot[0].x = 0;	rectDot[1].x = 11;		rectDot[2].x = 22;
	rectNoDot[0].x = 0; rectNoDot[1].x = 13;	rectNoDot[2].x =22 ;
	
	float factor[2][3] = { { 10, 1, 0.1 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat
	
	//��ͼƬ��Ԥ���� �Ҷȣ�Ȼ��Ҷ���ֵ
	Mat imageGray;
	
	image.copyTo(imageGray);
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	Mat imageGrayThreshold;
	imageGray.copyTo(imageGrayThreshold);
	for (int c = 0; c < imageGrayThreshold.cols; c++)
	{
		for (int r = 0; r < imageGrayThreshold.rows; r++)
		{
			if (imageGrayThreshold.at<uchar>(r,c) < 85 )
			{
				imageGrayThreshold.at<uchar>(r, c) = 0;
			}
		}
	}
	QString path = QString(".//temp//");
#ifdef WRITE_ROI_SMAPLES_TEMP

	
	writeSamples(QString("WINPLA_ROI.bmp"), imageGrayThreshold, path);


#endif
 
	 
	QString fileNameStr;
	// svm DataIdentify each number
 
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Mat roi(imageGray, winPlaPosStruct.rect[i][j]);


			Mat roiColor(image, winPlaPosStruct.rect[i][j]);

			dataOutput.isWinPlaHasGroundColor[i][j] = haveGroundColor(roiColor,2 );

			CvSize roiSize;
			roiSize.height = winPlaPosStruct.rect[i][j].height;
			roiSize.width = winPlaPosStruct.rect[i][j].width;

			Mat roiTrim;
			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge(imageGrayThreshold, winPlaPosStruct.rect[i][j]);;

			trimRoiBlankPart(roiForDotJudge, roiTrim, roiNewSize);

			roiNew = Mat(roiForDotJudge, roiNewSize);

			for (int i = 0; i < 3; i++)			// set the rect for single number in number region
			{
				if (i ==2 )
				{
					rectNoDot[i].width = roi.cols - rectNoDot[i].x;
					rectDot[i].width = roi.cols - rectDot[i].x;
				}
				else if ( i == 0 )
				{
					rectNoDot[i].width = 13;
					rectDot[i].width = 11;
				}
				else
				{
					rectNoDot[i].width = 9 ;
					rectDot[i].width = 9 ;
				}
				
				rectDot[i].y = roiNewSize.y;
			 
				rectDot[i].height = roiNew.rows;

				rectNoDot[i].y = roiNewSize.y;

				rectNoDot[i].height = roiNew.rows;
			}

		
			 
 
			if (i == 3 && j == 1 )
			{

				int temp = 0;
#ifdef QDEBUG_OUTPUT
				qDebug("getwinPlaPosStruct_live:Mark");
#endif // QDEBUG_OUTPUT

			}
			
			bool dotFlag;
			dotFlag = judgeWINPLADot(1, roiNew, roiForDotJudge);
			
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
			
			
			fileNameStr = QString("");
			fileNameStr.append(QString("i_"));
			fileNameStr.append(QString::number((int)i, 10));
			fileNameStr.append(QString("j_"));
			fileNameStr.append(QString::number((int)j, 10));
			fileNameStr.append(QString("k_"));
			fileNameStr.append(QString::number((int)6, 10));
			fileNameStr.append(QString(".bmp"));

			writeSamples(fileNameStr, roiNew, path);
#endif
 
			//�������� ��־ �������ȡ����������С��30 ˵���˵ط�Ϊ --- ����ƥ�Ѿ�����
			if (roiNew.cols < 20 )
			{
				dataOutput.mHorseInfo.isSCR[i] = true;
				dataOutput.WIN[i] = -1;
				dataOutput.PLA[i] = -1;
				continue ; 

			}
			else
			{
				dataOutput.mHorseInfo.isSCR[i] = false;
			}

			float tempSum = 0.0;
			if (dotFlag)															// contain a dot
			{
				for (int k = 0; k < 3; k++)					// segment each single number and svm
				{
					  
					Mat singleNum(roi, rectDot[k]);									// the single number image

			 

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1


					fileNameStr = QString("");
					fileNameStr.append(QString("i_"));
					fileNameStr.append(QString::number((int)i, 10));
					fileNameStr.append(QString("j_"));
					fileNameStr.append(QString::number((int)j, 10));
					fileNameStr.append(QString("k_"));
					fileNameStr.append(QString::number((int)k, 10));
					fileNameStr.append(QString(".bmp"));

					writeSamples(fileNameStr, singleNum, path);
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
					 
					 
					Mat singleNum(roi, rectNoDot[k]);


#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1


					fileNameStr = QString("");
					fileNameStr.append(QString("i_"));
					fileNameStr.append(QString::number((int)i, 10));
					fileNameStr.append(QString("j_"));
					fileNameStr.append(QString::number((int)j, 10));
					fileNameStr.append(QString("k_"));
					fileNameStr.append(QString::number((int)k, 10));
					fileNameStr.append(QString(".bmp"));

					writeSamples(fileNameStr, singleNum, path);
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

			if (j == 0)
				dataOutput.WIN[i] = tempSum;
			if (j == 1)
				dataOutput.PLA[i] = tempSum;


		}			// end j

	}				// end i

	return EXEC_SUCCESS;
}

//������ֵ����Ƿ��� ��ɫ��ɫ
// srcMat��ɫԭͼ
bool HK18D14DataIdentify::haveGroundColor(Mat srcMat ,int flag )
{
	//תΪ�Ҷ�ͼ��
	cvtColor(srcMat, srcMat, CV_RGB2GRAY);

	//�Ҷ���ֵ
	int graySum = 0;
	for (int c = 0; c < srcMat.cols;c++)
	{
		for (int r = 0; r < srcMat.rows;r++)
		{
			if (srcMat.at<uchar>(r,c) < 85 )
			{
				srcMat.at<uchar>(r, c) = 0;
			}
			graySum += srcMat.at<uchar>(r, c);
		}

		
	}

#ifdef QDEBUG_OUTPUT
	qDebug("graySum = %d " ,graySum );
#endif // DEBUG
	 
	int threshold = 60000 ;
	if (graySum >= threshold)
	{
		return true;
	}
	else
		return false;
	 

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

	Mat imageTemp;
	image.copyTo(imageTemp);

	
	//����ɫ��ֵ
	Vec3b pixel; // B G R
	int sum = 0;
	for (int c = 0; c < imageTemp.cols; c++)
	{
		for (int r = 0; r < imageTemp.rows; r++)
		{
			pixel = imageTemp.at<Vec3b>(r, c); //  

			sum = (pixel[0] - 170) * (pixel[0] - 170);

			sum += (pixel[1] - 160) * (pixel[1] - 160);

			sum += (pixel[2] - 168) * (pixel[2] - 168);

			if (sum >= 7000)
			{
				pixel[0] = 0;
				pixel[1] = 0;
				pixel[2] = 0;
			}
			imageTemp.at<Vec3b>(r, c) = pixel;
			sum = 0;
		}
	}
	 
	 
	cvtColor(imageTemp, imageTemp, CV_RGB2GRAY);

	Mat imageGray;

	cvtColor(image, imageGray, CV_RGB2GRAY);

	for (int c = 0; c < imageTemp.cols;c++)
	{
		for (int r = 0; r < imageTemp.rows;r++)
		{
			if (imageTemp.at<uchar>(r,c) < 100 )
			{
				imageTemp.at<uchar>(r, c) = 0;
			}
		}
	}

#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");

	writeSamples(QString("GetQinQpl.bmp"), imageTemp, path);


#endif

 

	float factor[3][3] = { { 1, 0.1, 0 }, { 10, 1, 0 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

	// svm DataIdentify each number

	int dotFlag;
	Mat qinQPLPosStructMat(imageTemp, WHOLE_QINQPL_POS_RECT);
	Mat qinQPLPosStructRegionSub1(imageTemp, QINQPL_POS_RECT);

	 
	rect[0][0].x = 0;	rect[0][1].x = 9;		rect[0][2].x = 11;			// for two number with dot	
	rect[1][0].x = 0;	rect[1][1].x = 9;		rect[1][2].x = 0;			// for two number without dot	 
	rect[2][0].x = 0;	rect[2][1].x = 9;		rect[2][2].x = 19;			// for three number without dot
 
	

	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
		for (int j = 0; j < QIN_QPL_COL; j++)
		{

			if (i == j | j == i + 1)
				continue;

			//���̫С�����˵�
			if (qinQPLPosStruct.rect[i][j].width < 5 | qinQPLPosStruct.rect[i][j].height < 5  )
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

				if (dataOutput.mHorseInfo.isSCR[i + 7] == true)
				{
					dataOutput.QPL_QIN[i][j] = -1;
					continue;
				}


			}
		
 
			// imageGray Ϊ�Ҷ�ͼ��
			Mat roi(imageGray, qinQPLPosStruct.rect[i][j]);

			Mat roiForDotJudge(imageTemp, qinQPLPosStruct.rect[i][j]);


			Mat roiColor(image, qinQPLPosStruct.rect[i][j]);

			dataOutput.isQplQinHasGroundColor[i][j] = haveGroundColor(roiColor, 1);


			//CvSize roiSize;
			//roiSize.height = qinQPLPosStruct.rect[i][j].height;
			//roiSize.width = qinQPLPosStruct.rect[i][j].width;
		 

			if (i == 5 & j == 10 )
			{

				qDebug("Mark \n");
#ifdef  QDEBUG_OUTPUT
				qDebug("Mark \n");
#endif //  QDEBUG_OUTPUT


			}

	  
			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
		 
			
			if (trimRoiBlankPart(roiForDotJudge, roiForDotJudge, roiNewSize) == EXIT_THIS_OCR)
			{
				return EXIT_THIS_OCR;
			}
			
 
			roiNew = Mat(roi, roiNewSize);
 

			int *x = new int[3 + 1];
			memset(x, 0, 4);


 
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
			writeSamples(i, j, 6, roiForDotJudge);
#endif // WRITE_ROI_SMAPLES_CLASS

			Mat edge;
			dotFlag = judgeQINQPLDot(roiForDotJudge, edge, x);
			if (i == 0 & j == 9)
			{

				QString fileNameTemp;
				fileNameTemp.prepend(QString(".bmp"));
				fileNameTemp.prepend(QString::number(i));
				fileNameTemp.prepend(QString("i_j"));
				fileNameTemp.prepend(QString::number(j));

#ifdef WRITE_ROI_SMAPLES_TEMP
			//	QString path = QString(".//temp//");

				writeSamples(fileNameTemp, edge, path);


#endif

		 
			}

			if (dotFlag == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;

			}
			roiNewSize.y = 0;
			//roiNewSize.height = roi.rows;
			if (roiNewSize.x + roiNewSize.width >= roi.cols)
			{
				roiNewSize.width = roi.cols - roiNewSize.x;
			}

		


		
		
 
			if (dotFlag == -1)
				qDebug("DataIdentifyImageInfor2_Dot_live_yp is wrong ! \n");
			 

			float tempSum = 0.0;
			if (dotFlag == 0)				 	 // 2λ������С����
			{
				int r = 0;
				for (int c = 0; c < 2; c++)
				{
					rect[r][c].y = 0;
					if (c < 1)
					{
						rect[r][c].width = rect[r][c + 1].x - rect[r][c].x;
					}
					else
					{
						rect[r][c].width = roiNewSize.width - rect[r][c + 1].x;

					}
					rect[r][c].height = roiNewSize.height;
					rect[r][c].y = 0;
				}
			 
				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{ 

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

			if (dotFlag == 1)	 //��λ��
			{

				int r = 1;
				for (int c = 0; c < 2; c++)
				{
					rect[r][c].y = 0;
					if (c < 1)
					{
						rect[r][c].width = rect[r][c + 1].x - rect[r][c].x;
					}
					else
					{
						rect[r][c].width = roiNewSize.width - rect[r][c].x;

					}
					rect[r][c].height = roiNewSize.height;
					rect[r][c].y = 0;
				}
 

				for (int k = 0; k < 2; k++)										// segment each single number and svm
				{

			 
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

			if (dotFlag == 2)			//3λ��
			{
				int r = 2 ;
				for (int c = 0; c < 3 ; c++)
				{
					rect[r][c].y = 0;
					if (c < 2 )
					{
						rect[r][c].width = rect[r][c + 1].x - rect[r][c].x;
					}
					else
					{
						rect[r][c].width = roiNewSize.width - rect[r][c].x;

					}
					rect[r][c].height = roiNewSize.height;
					rect[r][c].y = 0;
				}
				for (int k = 0; k < 3; k++)			// segment each single number and svm
				{
				 
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
		qDebug("ERROR : calculateGraySumXForSetWINPLARect :mat is empty \n");
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
		qDebug("calculateGraySumXForSetWINPLARect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD = 80;
	for (int i = 0; i < dimension; i++)
	{
		if (i == 0 & graySumX[i ] > THEREHOLD)
		{
			y[j] = i;
			j++;
			i += 5;
		}
		else
		{
			//����Ϊ 300 ���˵� �������ֵĵ�ɫ
			if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD))
				//�������������2��������ֵ���ᱻʡ�Ե�
				//		&& graySumX[i + 2] > THEREHOLD && graySumX[i + 3] > THEREHOLD
				//		&& graySumX[i + 4] > THEREHOLD ))
			{
				y[j] = i;
#ifdef QDEBUG_OUTPUT
				qDebug(" calculateGraySumXForSetWINPLARect : The  y %d is  %d", j, i);
#endif
				j++;
				i += 5;

			}
		}

		//δ��ʶ��
		if (j >= 2 )
		{
			if (y[j-1] - y[j-2] >= 34 )
			{
				return EXIT_THIS_OCR;
			}
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
		if (i == 0 & graySumX[0] > THEREHOLD)
		{
			y[j] = i;
			j++;

			i += 10 ;
		}
		else
		{

			//����Ϊ 300 ���˵� �������ֵĵ�ɫ
			if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
				&& graySumX[i + 2] > THEREHOLD))
			{
				y[j] = i;
#ifdef QDEBUG_OUTPUT
				qDebug(" calculateGraySumXForSetHorseNameRect : The  y %d is  %d", j, i);
#endif
				j++;

				i += 10;

			}
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
	 

	//ȥ����ɫ��Ե����
	int* graySum = new int[halfEdge.cols+1];
	
 
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

 

	//�������� ����ü����WIN PLA���ݣ�graySum[16-20]��0�ĸ����� 
	int  maxLength = 0;
	if (edge.cols >= 20 )
	{
		for (int i = 15; i < 21;i++)
		{
			if (graySum[i] < 20 )
			{
				maxLength++;
			}
		}
		 
		 
	}

	if (graySum != NULL)
	{
		delete[] graySum;
		graySum = NULL;
	}

	//����28 ��һ�����Ǵ���С���㣬С��28Ҳ��һ������ ������С����
	if (edge.cols >  28) // 29 ����Ϊ��С�����
	{
		return   true;
	}
	else if (edge.cols <= 28 ) //�ж��Ƿ��д���С����Ľ���
	{
		//���������ϵĿո� ������Ϊ��С����
		if (maxLength > 2 )
		{
			return true;
		}

		return  false;
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
		for (int r = 0; r < edge.rows / 2 ; r++)
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
	//int delta = backIndex - frontIndex;
	
	int delta = edge.cols ;

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
	else if (delta >= 25 ) // three number . 27
	{
		//��ȡ3�����ֵ� ����ֵ x
		
		{
			x[0] = 0 ;
			x[1] = 9 ;
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
	else if (delta <= 24 & delta > 20  ) // tow number with dot .
	{

		if (graySum[edge.cols / 2] != 0 & graySum[edge.cols / 2 - 1] != 0
			& graySum[edge.cols / 2 + 1] != 0)
		{
			{
				x[0] = 0;
				x[1] = 9;
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
		else
		{
			x[0] = 9;
			x[1] = 11;
			x[2] = 0;

			if (graySum != NULL)
			{
				delete[] graySum;
				graySum = NULL;
			}
			return 0;

		}
	
 
	}
	else if (delta <= 20 ) // two number 
	{
		  
		x[0] = 9;
		x[1] = 10;
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

	int y0, y1;
	int  *graySum = new int[oriMat.rows +1 ];

	memset(graySum, 0, (oriMat.rows + 1)*sizeof(int));
	for (int r = 0; r< oriMat.rows;r++)
	{
		for (int c = 0; c < oriMat.cols; c++)
		{
			graySum[r] += oriMat.at<uchar>(r, c);
		}
		 
	}

	for (int r = 0; r< oriMat.rows; r++)
	{
		if (graySum[r] > 0)
		{
			y0 = r;
			break;
		}
	}

	for (int r = 0; r< oriMat.rows; r++)
	{
		if (graySum[oriMat.rows-r] > 0)
		{
			y1 = oriMat.rows - r;
			break;
		}
	}

	delete[] graySum;
	roiNewSize.x = x0;

	if ((x1 + 1) > oriMat.cols)
		roiNewSize.width = oriMat.cols - x0;
	else
		roiNewSize.width = x1 - x0 + 1;

	roiNewSize.y = y0;
	 
	roiNewSize.height = y1-y0+1;

	newRoiMat = Mat(oriMat, roiNewSize);

	if (roiNewSize.width < 10 )
	{
		return EXIT_THIS_OCR;
	}
	else 
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
			//if (x + 2 >= mat.cols)
			//	x1 = mat.cols - 1;
		//	else 
				x1 = x ; // add one pixel
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
		qDebug("ERROR : calculateGraySumXForSetQINQPLRect :mat is empty \n");
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
		qDebug("calculateGraySumXForSetQINQPLRect %d = %d \n", pixelY, graySumX[pixelY]);
#endif
	}
	int j = 0;
	int THEREHOLD =  200 ;
	for (int i = 0; i < dimension; i++)
	{
		if (i == 0 & graySumX[0] > THEREHOLD)
		{
			y[j] = i;
			j++ ;

			i += 13 ;
		}
		else
		{
			//����Ϊ 300 ���˵� �������ֵĵ�ɫ ���ر�Ե ������С��300 ��2������300 
			if ((graySumX[i - 1] <THEREHOLD & graySumX[i] < THEREHOLD && graySumX[i + 1] >= THEREHOLD
				&& graySumX[i + 2] >= THEREHOLD))
			{
				y[j] = i;
#ifdef QDEBUG_OUTPUT
				qDebug(" calculateGraySumXForSetQINQPLRect : The  y %d is  %d", j, i);
#endif
				j++;

				i += 13 ;

				if (j > roiNum)
				{
					//Global::AppendLogString(QString("Error:calculateGraySumXForSetRect2 :Func error j>roiNum "), true);
#ifdef QDEBUG_OUTPUT
					qDebug("calculateGraySumXForSetQINQPLRect :Func error j =%d >roiNum=%d \n ",j,roiNum );
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
		qDebug("calculateGraySumXForSetQINQPLRect :Func error  j %d != roiNum  %d \n ",j,roiNum);
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


void HK18D14DataIdentify::writeSamples(QString fileName, Mat &roi,QString path)
{
 
	QString fileNameStr = path;


	//QString curPath = QDir::currentPath();
	QDir::setCurrent(fileNameStr);
	QString curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));
	//curPath = QDir::currentPath();
	
	/*
	fileNameStr = QString("");
	fileNameStr.append(QString("i_"));
	fileNameStr.append(QString::number((int)i, 10));
	fileNameStr.append(QString("j_"));
	fileNameStr.append(QString::number((int)j, 10));
	fileNameStr.append(QString("k_"));
	fileNameStr.append(QString::number((int)k, 10));
	fileNameStr.append(QString(".bmp"));
	*/
	imwrite(fileName.toStdString(), roi);

	//�˵���һ��Ŀ¼
	QDir::setCurrent("../");


	//curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));

 
}

