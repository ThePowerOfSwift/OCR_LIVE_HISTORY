#include "DataIdentify.h"
#include <QDebug>
#include<fstream>


//#define  CALLBACK_MODE


//#define WRITE_ERROR 

DataIdentify::DataIdentify()
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
		//		dataOutput.mHorseInfo.horseName[i] = "";
		dataOutput.mHorseInfo.isSCR[i] = false;
	}

	// initialize the DataOutput
	dataOutput.session = 0;
	dataOutput.raceTime = 0;

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		dataOutput.svmResult[i] = -1;
		dataOutput.WIN[i].dataValue = 0.0;
		dataOutput.PLA[i].dataValue = 0.0;
		memset(dataOutput.mHorseInfo.horseName[i], 0, sizeof(wchar_t)* 4);
		dataOutput.mHorseInfo.horseID[i] = 0;
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

		memset(dataOutput.mHorseInfo.horseName[i], 0, sizeof(wchar_t)* 4);
		dataOutput.mHorseInfo.horseID[i] = 0;

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
	/*
	QString curPath = QDir::currentPath();
	qDebug("curPath = %s \n", qPrintable(curPath));
	*/
	// �����Ѿ�ѵ���õ����� 

	numSVM.load(".\\SVM\\LIVENumber.xml");
	LNSVM.load(".\\SVM\\LN_SVM_1.xml");

	horseNameSVM.load(".\\SVM\\horseName.xml");

	raceTimeSvm.load(".\\SVM\\raceTime.xml");
	sessionNumSvm.load(".\\SVM\\sessionNum.xml");


	// 
	dataOutput.horseNameChangedNum = 0;

}

DataIdentify::~DataIdentify()
{

}

// ===========================================================
// public function 
// ===========================================================
bool DataIdentify::read(Mat &srcMat, uchar* data, int length, int height, int width)
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
int DataIdentify::originPosition()
{
	originX = 0;
	originY = 0;
	int regionWidth = ORIGINPOSITION_REGIONWIDTH, regionHeight = ORIGINPOSITION_REGIONHEIGHT;
	// 
	// 	imshow("a",image);
	// 	waitKey();
	// regionWidth regionHeight û�����ϡ�
	Mat imageTemp;
	image.copyTo(imageTemp);
	Mat region(imageTemp, Rect(0, 0, regionWidth, regionHeight));
	Mat regionGray(regionHeight, regionWidth, CV_8UC1, Scalar::all(0));

	cvtColor(imageTemp, regionGray, CV_BGR2GRAY);

	for (int c = 0; c < regionGray.cols; c++)
	{
		for (int r = 0; r < regionGray.rows; r++)
		{
			if (regionGray.at<uchar>(r, c) < 190)
			{
				regionGray.at<uchar>(r, c) = 0;

			}			 

		}
	}

	 
	int* colSum = new int[regionHeight + 1];
	int* rowSum = new int[regionWidth + 1];

	// calculate the row sum
	for (int i = 0; i < regionHeight; i++)
	{
		rowSum[i] = 0;
		for (int j = 0; j < regionWidth; j++)
			rowSum[i] += regionGray.at<uchar>(i, j);

		rowSum[i] = rowSum[i] / regionWidth;

#ifdef QDEBUG_OUTPUT
		qDebug("rowSum[%d] = %d ",i,rowSum[i]);
#endif
	}


	// calculate the col sum
	for (int i = 0; i < regionWidth; i++)
	{
		colSum[i] = 0;
		for (int j = 0; j < regionHeight; j++)
			colSum[i] += regionGray.at<uchar>(j, i);

		colSum[i] = colSum[i] / regionHeight;
#ifdef QDEBUG_OUTPUT
		qDebug("rowSum[%d] = %d ", i, colSum[i]);
#endif
	}

	// DataIdentify the originX
	for (int i = 0; i < regionWidth; i++)
	{
		if (colSum[i]>10 )
		{
			originX = i - 1;
			break;
		}
	}
	// DataIdentify the originY
	for (int i = 0; i < regionHeight; i++)
	{
		if (rowSum[i]> 10 )
		{
			originY = i - 1;
			break;
		}
	}

#ifdef QDEBUG_OUTPUT
	qDebug("the originPosition Func : x =%d, y=%d", originX, originY);
#endif


	//���ƫ��Ԥ���ԭ��̫�࣬��ôֱ���˳�����
	if (abs(originX - ORIGIN_X_BASE_LIVE) > 10 | abs((originY-ORIGIN_X_BASE_LIVE) > 10 ) )
	{


		//д��ϵͳ��־
		Global::systemLog->append(QString(("Error :DataIdentify  ")), QString("Get OriginX Y Wrong��"),
			SystemLog::ERROR_TYPE);
	 
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
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

	CvRect newRect;
	newRect.x = originX;
	newRect.y = originY;

	newRect.width = imageTemp.cols - originX;
	newRect.height = imageTemp.rows - originY;

	Mat newMat;
	newMat = Mat(imageTemp, newRect);
	imwrite("originTrim.bmp", newMat);

	return EXEC_SUCCESS ;
}

int DataIdentify::identify()
{

	haveData();
	if (haveDataFlag == false )						// the frame has not any data, return 1
		return EXEC_SUCCESS ;
	if (algorithmState == EXIT_THIS_OCR )
	{
		dataOutput.haveDataFlag = false;
		haveDataFlag = false;
		return EXIT_THIS_OCR;
	}
	originPosition();

	if (algorithmState == EXIT_THIS_OCR)
	{
		dataOutput.haveDataFlag = false;
		haveDataFlag = false;
		//д��ϵͳ��־
 	//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("originPosition func EXIT this ocr"),
			SystemLog::ERROR_TYPE);
		return EXIT_THIS_OCR;
	}

	//��������λ��

	setHorseNameRectPos();

	if (algorithmState == EXIT_THIS_OCR)
	{
 
	//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("setHorseNameRectPos EXIT this ocr"),
			SystemLog::ERROR_TYPE);
		return EXIT_THIS_OCR;
	}
	// ���� WIN PLA λ��

	setWINPLARectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{
 
	//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("setWINPLARectPos EXIT this ocr"),
			SystemLog::ERROR_TYPE);
		return EXIT_THIS_OCR;
	}
	// ���� QIN QPL  ͬʱ����QIN QPL���λ�� 
	setQINQPLRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("setQINQPLRectPos EXIT this ocr"),
		SystemLog::ERROR_TYPE);

		return EXIT_THIS_OCR;
	}
	//  ���ó��κ�λ��

	setSessionRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��
		Global::systemLog->append(QString(("DataIdentify  ")), QString("setSessionRectPos EXIT this ocr"),
			SystemLog::ERROR_TYPE);

		return EXIT_THIS_OCR;
	}
	//���� ����ʱλ�� 
	setCountDownRectPos();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("setCountDownRectPos EXIT this ocr"),
			SystemLog::ERROR_TYPE);

		return EXIT_THIS_OCR;
	}
	//ʶ������ 
	getHorseNameIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("getHorseNameIdentify EXIT this ocr"),
			SystemLog::ERROR_TYPE);

		return EXIT_THIS_OCR;
	}
	// ʶ�� WIN PLA

	getWINPLAIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("getWINPLAIdentify EXIT this ocr"),
			SystemLog::INFO_TYPE);

		return EXIT_THIS_OCR;
	}
	//ʶ�� QIN QPL   ͬʱʶ��ѣɣΡ��ѣУ�
	getQINQPLIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("getQINQPLIdentify EXIT this ocr"),
			SystemLog::INFO_TYPE);

		return EXIT_THIS_OCR;
	}
	//ʶ�𳡴κ�

	getSessionIdentify();
	if (algorithmState == EXIT_THIS_OCR)
	{

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("getSessionIdentify EXIT this ocr"),
		SystemLog::INFO_TYPE);

		return EXIT_THIS_OCR;
	}
	//ʶ�𵹼�ʱ 

	getCountDownIdentify();

	if (algorithmState == EXIT_THIS_OCR)
	{

		Global::systemLog->append(QString(("DataIdentify")), QString("getCountDownIdentify EXIT this ocr"),
			SystemLog::INFO_TYPE);

		return EXIT_THIS_OCR;
	}
	return EXEC_SUCCESS;
}


/*
���� ������rect����
*/
int DataIdentify::setHorseNameRectPos()
{
	Mat horseNameRegion(image, HORSENAME_REGION_RECT_LIVE);

	//ת�Ҷ�ͼ��
	cvtColor(horseNameRegion, horseNameRegion, CV_RGB2GRAY);
	Mat edge;
	Canny(horseNameRegion, edge, 450, 400, 3, true);
	int *y = new int[HORSENUMBER + 4];

	//ͬʱ��ȡ�����Ŀ
	calculateGraySumXForSetHorseNameRect(edge, y, dataOutput.horseNum);

	if (algorithmState == EXIT_THIS_OCR)
	{

		if (y != NULL)
		{
			delete[] y;
			y = NULL;
		}
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}

	for (int r = 0; r < dataOutput.horseNum; r++)
	{
		horseNamePosStruct.rect[r].x = HORSENAME_REGION_RECT_LIVE.x;
		horseNamePosStruct.rect[r].width = HORSENAME_REGION_RECT_LIVE.width;

		horseNamePosStruct.rect[r].y = HORSENAME_REGION_RECT_LIVE.y + y[r];
		if (r == dataOutput.horseNum - 1)
		{
			//��ֹ�߶�̫��
			horseNamePosStruct.rect[r].height = HORSENAME_REGION_RECT_LIVE.height - y[r];
			if (horseNamePosStruct.rect[r].height > 21)
				horseNamePosStruct.rect[r].height = 21;

		}
		else
			horseNamePosStruct.rect[r].height = y[r + 1] - y[r];
#ifdef WRITE_ROI_SMAPLES_TEMP
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
#endif
	}


	if (y != NULL)
	{
		delete[] y;
		y = NULL;
	}
	return EXEC_SUCCESS;
}


/**
* @brief
//// ���� WIN PLA λ��

*/
int DataIdentify::setWINPLARectPos()
{

	Mat winRegion(image, WIN_POS_RECT_LIVE);
	Mat plaRegion(image, PLA_POS_RECT_LIVE);

	//ת�Ҷ�ͼ��
	cvtColor(winRegion, winRegion, CV_RGB2GRAY);
	cvtColor(plaRegion, plaRegion, CV_RGB2GRAY);

	Mat edgePla;
	Mat edgeWin;


	//Canny(winRegion, edgeWin, 450, 400, 3, true);
	//Canny(plaRegion, edgePla, 450, 400, 3, true);



	for (int c = 0; c < winRegion.cols; c++)
	{
		for (int r = 0; r < winRegion.rows; r++)
		{
			if (winRegion.at<uchar>(r, c) < 203)
			{
				winRegion.at<uchar>(r, c) = 0;
			}
		}
	}

	edgeWin = winRegion;

	

	for (int c = 0; c < plaRegion.cols; c++)
	{
		for (int r = 0; r < plaRegion.rows; r++)
		{
			if (plaRegion.at<uchar>(r, c) < 203)
			{
				plaRegion.at<uchar>(r, c) = 0;
			}
		}
	}

	edgePla = plaRegion;

	//ȥ������

	int *yWin = new int[HORSENUMBER + 4];
	int *yPla = new int[HORSENUMBER + 4];

	int x0, x1;


	calculateGraySumXForSetWINPLARect(edgeWin, yWin, dataOutput.horseNum);
	calculateGraySumXForSetWINPLARect(edgePla, yPla, dataOutput.horseNum);

	if (algorithmState == EXIT_THIS_OCR)
	{
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

		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}

	// get the relative position of the three vertex in the first row, relative to the origin 
	//
//		imwrite("WIN_Region.bmp", edgeWin);
//		imwrite("PLA_Region.bmp", edgePla);


	//���� ʶ�����������Xֵ
	if (calculateGraySumYForTrim(edgeWin, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
	{
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

		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	if (x1 <= x0)
	{
#ifdef QDEBUG_OUTPUT
		qDebug("ERROR:getqinQPLPosStructRect function : x1 < x0 ERROR");
#endif
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

		return EXIT_THIS_OCR;
	}
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		// 3������ �հ�	 		
		winPlaPosStruct.rect[i][0].x = WIN_POS_RECT_LIVE.x + x0 - 1;
		winPlaPosStruct.rect[i][0].width = x1 - x0 + 1;


		winPlaPosStruct.rect[i][0].y = WIN_POS_RECT_LIVE.y + yWin[i];

		if (i == dataOutput.horseNum - 1)
		{

			winPlaPosStruct.rect[i][0].height = winPlaPosStruct.rect[i - 1][0].height;
		}
		else
		{
			//����߶ȹ��ߣ���ô˵�������� ��������ƥ����ôǿ�����ø߶�Ϊ�̶�ֵ��
			if (yWin[i + 1] - yWin[i] > 23)
			{
			//	yWin[i + 1] = yWin[i] + NUMBER_HEIGHT_LIVE + 2;
				winPlaPosStruct.rect[i][0].height = NUMBER_HEIGHT_LIVE + 2;

			}
			else
			{
				winPlaPosStruct.rect[i][0].height = yWin[i + 1] - yWin[i];

			}

		}
		//��� �Ƿ��г����
		if (winPlaPosStruct.rect[i][0].width <= 20)
		{
			winPlaPosStruct.rect[i][0].width = NUMBER_WIDTH_LIVE;
		}

		if (winPlaPosStruct.rect[i][0].height <= 10)
		{
			winPlaPosStruct.rect[i][0].height = NUMBER_HEIGHT_LIVE;
		}

	}

	x0 = 0;
	x1 = 0;
	// PLA part 
	//���� ʶ�����������Xֵ
	if (calculateGraySumYForTrim(edgePla, x0, x1, dataOutput.horseNum) == EXIT_THIS_OCR)
	{

		Global::systemLog->append(QString(("HK18D14DataIdentify  ")),
			QString("calculateGraySumYForTrim2 EXIT THIS OCR2 "),
			SystemLog::INFO_TYPE);

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
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}
	if (x1 <= x0)
	{
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
#ifdef QDEBUG_OUTPUT
		qDebug("ERROR:setWINPLARect_live function : x1 < x0 ERROR");
#endif
		return EXIT_THIS_OCR;
	}

	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		// 3������ �հ�	 		
		winPlaPosStruct.rect[i][1].x = PLA_POS_RECT_LIVE.x + x0 - 1;
		winPlaPosStruct.rect[i][1].width = x1 - x0 + 1;

		winPlaPosStruct.rect[i][1].y = PLA_POS_RECT_LIVE.y + yWin[i];


		if (i == dataOutput.horseNum - 1)
		{
			winPlaPosStruct.rect[i][1].height = winPlaPosStruct.rect[i - 1][1].height;
		}
		else
		{

			//����߶ȹ��ߣ���ô˵�������� ��������ƥ����ôǿ�����ø߶�Ϊ�̶�ֵ��
			if (yWin[i + 1] - yWin[i] > 23)
			{
				//	yWin[i + 1] = yWin[i] + NUMBER_HEIGHT_LIVE + 2;
				winPlaPosStruct.rect[i][1].height = NUMBER_HEIGHT_LIVE + 2;

			}
			else
			{
				winPlaPosStruct.rect[i][1].height = yWin[i + 1] - yWin[i];

			}
		}

		if (winPlaPosStruct.rect[i][1].width <= 20)
		{
			winPlaPosStruct.rect[i][1].width = NUMBER_WIDTH_LIVE;
		}

		if (winPlaPosStruct.rect[i][1].height <= 10)
		{
			winPlaPosStruct.rect[i][1].height = NUMBER_HEIGHT_LIVE;
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
//	Mat imageInfo2Mat(image_temp, WHOLE_QINQPL_POS_RECT_LIVE);
//	Mat imageInfo2RegionSub1(imageInfo2Mat, QINQPL_POS_RECT_LIVE);

	qinQplSubRect = new CvRect[19];
	{
		qinQplSubRect[0] = LB_REGION1_RECT_LIVE;
		qinQplSubRect[1] = LB_REGION2_RECT_LIVE;
		qinQplSubRect[2] = LB_REGION3_RECT_LIVE;
		qinQplSubRect[3] = LB_REGION4_RECT_LIVE;
		qinQplSubRect[4] = LB_REGION5_RECT_LIVE;
		qinQplSubRect[5] = LB_REGION6_RECT_LIVE;

		qinQplSubRect[6] = RU_REGION1_RECT_LIVE;
		qinQplSubRect[7] = RU_REGION2_RECT_LIVE;
		qinQplSubRect[8] = RU_REGION3_RECT_LIVE;
		qinQplSubRect[9] = RU_REGION4_RECT_LIVE;
		qinQplSubRect[10] = RU_REGION5_RECT_LIVE;
		qinQplSubRect[11] = RU_REGION6_RECT_LIVE;


		qinQplSubRect[12] = R_REGION1_RECT_LIVE;
		qinQplSubRect[13] = R_REGION2_RECT_LIVE;
		qinQplSubRect[14] = R_REGION3_RECT_LIVE;
		qinQplSubRect[15] = R_REGION4_RECT_LIVE;
		qinQplSubRect[16] = R_REGION5_RECT_LIVE;
		qinQplSubRect[17] = R_REGION6_RECT_LIVE;
		qinQplSubRect[18] = R_REGION7_RECT_LIVE;


	}



#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");

	writeSamples(QString("ori1.bmp"), image_temp, path);


#endif




	int deltaNum = HORSENUMBER - dataOutput.horseNum;
	for (int i = 0; i < 19 - deltaNum; i++)
	{
		//�����ƥ��������8 ����ô ǰ6�����򶼲���ʶ����
		if (deltaNum >= 6)
		{
			if (i < 6)
			{
				continue;
			}
		}
		Mat imageInfo2RoiEdge;
		Mat imageInfo2Roi(image_temp, qinQplSubRect[i]);
		
		
		//Canny(imageInfo2Roi, imageInfo2RoiEdge, 500, 450, 3, true);

		for (int c = 0; c < imageInfo2Roi.cols; c++ )
		{
			for (int r = 0; r < imageInfo2Roi.rows; r++)
			{
				if (imageInfo2Roi.at<uchar>(r,c) < 203 )
				{
					imageInfo2Roi.at<uchar>(r, c) = 0;
				}
			}
		}

		imageInfo2RoiEdge = imageInfo2Roi;
		//ȥ������


#ifdef WRITE_ROI_SMAPLES_TEMP
		QString fileName;
		fileName.prepend(".bmp");
		fileName.prepend(QString::number(i));
		QString path = QString(".//temp//");

		writeSamples(fileName, imageInfo2RoiEdge, path);

#endif
		if (setEveryQINQPLPos(imageInfo2RoiEdge, i) == EXIT_THIS_OCR)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("Error:setQINQPLRectPos function : setQINQPLRectPos return EXIT THIS OCR i = %d\n", i);
#endif // QDEBUG_OUTPUT

			//д��ϵͳ��־
			Global::systemLog->append(QString(("DataIdentify  ")), QString("setEveryQINQPLPos EXIT this ocr"),
				SystemLog::INFO_TYPE);

			return EXIT_THIS_OCR;

		}

	}



	return EXEC_SUCCESS;


}

/**
* @brief
//����canny �����ͼ�� ����ÿһ��qin qpl����λ��

*/
int  DataIdentify::setEveryQINQPLPos(Mat &mat, int rectNum)
{

#ifdef QDEBUG_OUTPUT
	qDebug("setEveryQINQPLPos func \n");
#endif
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	int x0 = 0;
	int x1 = 0;
	//ʵ����ƥ��Ŀ�� 14 ���� ����Ϊ������� ÿһ��region�����ֵĸ���ʱ�򣬻��õ�

	if (dataOutput.horseNum > 14 | dataOutput.horseNum <= 0)
	{
		algorithmState = EXIT_THIS_OCR;

		//д��ϵͳ��־
		Global::systemLog->append(QString(("DataIdentify  ")), QString("horseNum > 14 EXIT this ocr"),
			SystemLog::INFO_TYPE);


		return EXIT_THIS_OCR;



	}
	int delataNum = HORSENUMBER - dataOutput.horseNum;
	if (rectNum < 6 - delataNum) // 6-1
	{
		int * y = new int[6 - rectNum + 1]; // 6-1		 
		if (calculateGraySumXForSetQINQPLRect(mat, y, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
		{
			algorithmState = EXIT_THIS_OCR;

			//д��ϵͳ��־
			Global::systemLog->append(QString(("DataIdentify")),
				QString("calculateGraySumXForSetQINQPLRect EXIT this ocr"),
				SystemLog::INFO_TYPE);
			if (y != NULL)
			{
				delete[] y;
				y = NULL;
			}

			return EXIT_THIS_OCR;


		}
		/*
		for (int i = 0; i < 6 - rectNum; i++)
		{
#ifdef QDEBUG_OUTPUT
			qDebug("y_%d = %d \n", i, y[i]);
#endif
		}

		*/
		for (int i = rectNum + 1; i < 7 - delataNum; i++)
		{


			if (calculateGraySumYForTrim(mat, x0, x1, 6 - rectNum - delataNum) == EXIT_THIS_OCR)
			{
				algorithmState = EXIT_THIS_OCR;
				if (y != NULL)
				{
					delete[] y;
					y = NULL;
				}
 
				//д��ϵͳ��־
				Global::systemLog->append(QString(("DataIdentify  ")), QString("setEveryQINQPLPos EXIT this ocr"),
					SystemLog::INFO_TYPE);
 

				return EXIT_THIS_OCR;
			}
			if (x1 <= x0 | (x1 - x0) < 12)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
#endif

				if (y != NULL)
				{
					delete[] y;
					y = NULL;
				}
				return EXIT_THIS_OCR;
			}
			// 3������ �հ�
			qinQPLPosStruct.rect[i][rectNum].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum].width = x1 - x0 + 4;

			 
			//��������һ������
			if (y[i - rectNum - 1] - 1 >= 0)
			{
				qinQPLPosStruct.rect[i][rectNum].y = qinQplSubRect[rectNum].y + y[i - rectNum - 1] - 1;

			}
			else
				qinQPLPosStruct.rect[i][rectNum].y = qinQplSubRect[rectNum].y + y[i - rectNum - 1];
			
			//���һ������ ���ø߶�Ϊ19 
			if (i == 6 - delataNum)
			{
				qinQPLPosStruct.rect[i][rectNum].height = 19;
			}
			 // 
			else
			{
				if (y[i - rectNum] - y[i - rectNum - 1] > 24)
				{
					//y[i - rectNum] = y[i - rectNum - 1] + NUMBER_HEIGHT_LIVE + 2;
					qinQPLPosStruct.rect[i][rectNum].height = NUMBER_HEIGHT_LIVE + 2;
				}
				else
				{
					qinQPLPosStruct.rect[i][rectNum].height = y[i - rectNum] - y[i - rectNum - 1] - 1;

				}
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
			if (y != NULL)
			{
				delete[] y;
				y = NULL;
			}
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
			if (x1 <= x0 | (x1-x0) < 12 ) 
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
#endif			
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}
			// 3������ �հ�
			qinQPLPosStruct.rect[i][rectNum - 4].x = qinQplSubRect[rectNum].x + x0 - 2;
			qinQPLPosStruct.rect[i][rectNum - 4].width = x1 - x0 + 4;
			//��������һ������
			qinQPLPosStruct.rect[i][rectNum - 4].y = qinQplSubRect[rectNum].y + y[i] -1;
			if (i == roiNum - 1)
			{
				qinQPLPosStruct.rect[i][rectNum - 4].height = 19;
			}
			else
			{
				if (y[i + 1] - y[i] > 24 )
				{
					//y[i + 1] = y[i] + NUMBER_HEIGHT_LIVE + 2;

					qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT_LIVE + 2;
				}
				else
				{
					qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i] - 1;

				}
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
			if (y != NULL)
			{
				delete[] y;
				y = NULL;
			}

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
				if (y != NULL)
				{
					delete[] y;
					y = NULL;
				}

				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}

			if (x1 <= x0 | (x1 - x0) < 12)
			{
#ifdef QDEBUG_OUTPUT
				qDebug("ERROR:getImageInfo2Rect function : x1 < x0 ERROR");
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
				qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT_LIVE;
			}
			else
			{
				if (y[i + 1] - y[i] > 24 )
				{
					//y[i + 1] = y[i] + NUMBER_HEIGHT_LIVE + 2;

					qinQPLPosStruct.rect[i][rectNum - 4].height = NUMBER_HEIGHT_LIVE + 2;
				}
				else
				{
					qinQPLPosStruct.rect[i][rectNum - 4].height = y[i + 1] - y[i] - 1;

				}
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

int DataIdentify::setSessionRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	sessionPosStruct.rect[0] = SESSION_POS_RECT_LIVE;

	return EXEC_SUCCESS;
}

//���� ����ʱλ�� 
int DataIdentify::setCountDownRectPos()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	// ʮλ��
	countDownMinutePosStruct.rect[0] = COUNTDOWNMINUTE_POS_RECT1_LIVE;
	//��λ��
	countDownMinutePosStruct.rect[1] = COUNTDOWNMINUTE_POS_RECT2_LIVE;

	return EXEC_SUCCESS;

}

//ʶ������ 
int DataIdentify::getHorseNameIdentify()
{
	//ͨ������Ҷ�ֵ�ͣ���ȷ���Ƿ����������ֵı仯�������ַ����˱仯��˵��
	// ���κŷ����˱仯��
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	isHorseNameChanged();
	//��ʱ��ʶ��
	int temp = 0;
	return EXIT_SUCCESS;
}

//ͨ������Ҷ�ֵ�ͣ���ȷ���Ƿ����������ֵı仯�������ַ����˱仯��˵��
int DataIdentify::isHorseNameChanged()
{

	//�����һ�ε� 

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//�洢�����ĻҶȺ�
	int *graySum;
	graySum = new int[dataOutput.horseNum + 1];
	int  ChangedNum = 0;
	int graySumThreshold = 3000;
	int *length;

	length = new int[dataOutput.horseNum + 1];

	memset(graySum, 0, sizeof(int)* dataOutput.horseNum);
	memset(length, 0, sizeof(int)* dataOutput.horseNum);

	for (int h = 0; h < dataOutput.horseNum; h++)
	{

		Mat horseNameRegion(image, horseNamePosStruct.rect[h]);
		cvtColor(horseNameRegion, horseNameRegion, CV_RGB2GRAY);
		Mat horseNameRegionEdge;
		//Canny(horseNameRegion, horseNameRegionEdge, 450, 400, 3, true);

		graySum[h] = calculateGraySum(horseNameRegion, length[h]);


		if (abs(graySum[h] - dataOutput.mHorseInfo.graySum[h]) > graySumThreshold
			& abs(length[h] - dataOutput.mHorseInfo.length[h]) > 4)
		{
			
#ifdef QDEBUG_OUTPUT
			qDebug("  graySum[%d] = %d ,pri = %d \n", h,
				graySum[h], dataOutput.mHorseInfo.graySum[h]);
#endif
			 
			ChangedNum++;

		}

		dataOutput.mHorseInfo.graySum[h] = graySum[h];
		dataOutput.mHorseInfo.length[h] = length[h];
	}
	//���change
	if (ChangedNum > 1)
	{
		Global::sessionChangedNum0 = true;
	}
	if (Global::sessionChangedNum0 & Global::sessionChangedNum1 )
	{
#ifdef QDEBUG_OUTPUT
		qDebug("  horseNameChangedNum = %d \n",
			dataOutput.horseNameChangedNum);
#endif
		if (Global::session == 0 )
		{
			Global::session++;
			//��ֵ���κ�
			//��ʱ������ ���µĳ��κš� �������ʷ��Ƶͨ�� ����֡����ʵ��
			Global::timerCount = 0;

			Global::countRaceTime = 0;

			Global::isSessionChanged = true;

			Global::isCountTimeCalied = false;

		}
		//���� ˳��ʱ���� 15 ���ӣ��ſ��Ըı䳡�κ�
		else if (Global::countRaceTime  > 15 | Global::session == 1)
		{
			Global::session++;
			//��ֵ���κ�
			//��ʱ������ ���µĳ��κš� �������ʷ��Ƶͨ�� ����֡����ʵ��
			Global::timerCount = 0;

			Global::countRaceTime = 0;

			Global::isSessionChanged = true;
			Global::isCountTimeCalied = false;

		}
	
	

	}
	dataOutput.session = Global::session;

	//����˹�У���˳��κţ���ô��ʱ ���¸�ֵ���κš�
	if (Global::isSessioncalibrated)
	{
		dataOutput.horseNameChangedNum = Global::session;
		
 

	}

	if (graySum != NULL )
	{
		delete[] graySum;
		graySum = NULL;
	}

	Global::sessionChangedNum2 = Global::sessionChangedNum1;

	Global::sessionChangedNum1 = Global::sessionChangedNum0;

	return EXEC_SUCCESS;
}
/**
* @brief
//����Ҷ�ֵ�ͣ�����ֵ��Ϊ�Ҷ�ֵ�� ͬʱ����ֵ

*/
int DataIdentify::calculateGraySum(Mat srcMat, int &length)
{
	int graySum = 0;

	for (int c = 0; c < srcMat.cols; c++)
	{
		for (int r = 0; r < srcMat.rows; r++)
		{
			if (srcMat.at<uchar>(r, c) < 100)
			{
				srcMat.at<uchar>(r, c) = 0;
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

	for (int c = srcMat.cols - 1; c >= 0; c--)
	{
		if (sum[c] > 200)
		{
			length = c;
			break;
		}
	}
	delete[] sum;
	return graySum;

}
/**
* @brief
// ʶ�� WIN PLA

*/
int DataIdentify::getWINPLAIdentify()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}


	CvRect rectDot[3];
	CvRect rectNoDot[3];

	rectDot[0].x = 0;	rectDot[1].x = 10;		rectDot[2].x = 25;
	rectNoDot[0].x = 0; rectNoDot[1].x = 10;	rectNoDot[2].x = 19;

	float factor[2][3] = { { 10, 1, 0.1 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

#ifdef WRITE_ROI_SMAPLES_TEMP

	QString path = QString(".//temp//");
	writeSamples(QString("WINPLA_ROI.bmp"), image, path);


#endif
	// svm DataIdentify each number
	Mat edge;
	for (int i = 0; i < dataOutput.horseNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Mat roi(image, winPlaPosStruct.rect[i][j]) ;
			Mat roiThreshold;
			Mat roiThresholdEdge;

			cvtColor(roi, roi, CV_RGB2GRAY);

			roi.copyTo(roiThreshold);

			//ʹ�ûҶ���ֵ

			for (int r = 0; r < roiThreshold.rows; r++)
			{
				for (int c = 0; c < roiThreshold.cols; c++)
				{
					if (roiThreshold.at<uchar>(r, c) < 200)
					{
						roiThreshold.at<uchar>(r, c) = 0;
					}
				}
			}

			if (i == 0 & j == 0 )
			{
				int temp = 0;
			}
			//ר������ С������
			//cvtColor(roiThreshold, roiThreshold, CV_RGB2GRAY);

			CvRect roiNewSize;
			//	dotFlag = DataIdentifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;


			trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);

			

			if (algorithmState == EXIT_THIS_OCR)
			{
				return EXIT_THIS_OCR;
			}
			//����Ƿ�Խ��
			if (roi.cols < roiNewSize.x + roiNewSize.width |
				roi.rows < roiNewSize.y + roiNewSize.height)
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}



			CvSize roiSize;
			roiSize.height = winPlaPosStruct.rect[i][j].height;
			roiSize.width = winPlaPosStruct.rect[i][j].width;

			for (int t = 0; t < 3; t++)													// set the rect for single number in number region
			{
				if (t == 2)
				{
					rectNoDot[t].width = roi.cols - rectNoDot[t].x;
					rectDot[t].width = roi.cols - rectDot[t].x;
				}

				rectNoDot[t].width = 9;
				rectDot[t].width = 9;

				rectDot[t].y = 0;

				rectDot[t].height = roiNewSize.height;

				rectNoDot[t].y = 0;

				rectNoDot[t].height = roiNewSize.height;
			}



			roiNew = Mat(roi, roiNewSize);


			if (i == 2 && j == 0)
			{

				int temp = 0;
#ifdef QDEBUG_OUTPUT
				qDebug("getwinPlaPosStruct_live:Mark");
#endif // QDEBUG_OUTPUT

			}
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1
			QString path = QString(".//temp//");
			QString fileNameStr;

			fileNameStr = QString("");
			fileNameStr.append(QString("WINPLA_i_"));
			fileNameStr.append(QString::number((int)i, 10));
			fileNameStr.append(QString("j_"));
			fileNameStr.append(QString::number((int)j, 10));
			fileNameStr.append(QString("k_"));
			fileNameStr.append(QString::number((int)6, 10));
			fileNameStr.append(QString(".bmp"));

			writeSamples(fileNameStr, roiNew, path);
#endif

			bool dotFlag = false;
			// ͨ���ж� ������ж����� �Ƿ����С���� 3
			if (roiNew.cols >= 32)
			{
				dotFlag = true;
			}
			else
				dotFlag = false;
			//	dotFlag = judgeWINPLADot(1, roiThreshold, roiThresholdEdge);


			//�������� ��־ �������ȡ����������С��30 ˵���˵ط�Ϊ --- ����ƥ�Ѿ�����
			if (roiNew.cols < 20)
			{
				dataOutput.mHorseInfo.isSCR[i] = true;
				dataOutput.WIN[i].dataValue = -1;
				dataOutput.PLA[i].dataValue = -1;
				continue;

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
					if (k == 2 && rectDot[k].x + rectDot[k].width >= roiNew.cols)	// cross the boarder
						rectDot[k].width = roiNew.cols - rectDot[k].x;


					//����Ƿ�Խ��
					if (roiNew.cols < rectDot[k].x + rectDot[k].width |
						roiNew.rows < rectDot[k].y + rectDot[k].height)
					{

						if (j ==0 )
						{
							dataOutput.WIN[i].byPass = true;
						}
						if (j == 1 )
						{
							dataOutput.PLA[i].byPass = true;
						}
						continue ; 
					//	algorithmState = EXIT_THIS_OCR;
						//return EXIT_THIS_OCR;
					}
					else
					{
						dataOutput.WIN[i].byPass = false ;
						dataOutput.PLA[i].byPass = false ;
					}
					Mat singleNum(roiNew, rectDot[k]);									// the single number image


#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1


					fileNameStr = QString("");
					fileNameStr.append(QString("WINPLA_i_"));
					fileNameStr.append(QString::number((int)i, 10));
					fileNameStr.append(QString("j_"));
					fileNameStr.append(QString::number((int)j, 10));
					fileNameStr.append(QString("k_"));
					fileNameStr.append(QString::number((int)k, 10));
					fileNameStr.append(QString(".bmp"));

					writeSamples(fileNameStr, singleNum, path);
#endif


					resize(singleNum, singleNum, hog.winSize);
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


					//����Ƿ�Խ��
					if (roiNew.cols < rectNoDot[k].x + rectNoDot[k].width |
						roiNew.rows < rectNoDot[k].y + rectNoDot[k].height)
					{
						//���Ե�ǰ���WIN PLA���ݣ�������һ��ʶ��
						if (j == 0)
						{
							dataOutput.WIN[i].byPass = true;
						}
						if (j == 1)
						{
							dataOutput.PLA[i].byPass = true;
						}
						continue;

						algorithmState = EXIT_THIS_OCR;
						return EXIT_THIS_OCR;
					} 
					else
					{ 
						dataOutput.WIN[i].byPass = false ;
						dataOutput.PLA[i].byPass = false ;

					}

					Mat singleNum(roiNew, rectNoDot[k]);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO1


					fileNameStr = QString("");
					fileNameStr.append(QString("WINPLA_i_"));
					fileNameStr.append(QString::number((int)i, 10));
					fileNameStr.append(QString("j_"));
					fileNameStr.append(QString::number((int)j, 10));
					fileNameStr.append(QString("k_"));
					fileNameStr.append(QString::number((int)k, 10));
					fileNameStr.append(QString(".bmp"));

					writeSamples(fileNameStr, singleNum, path);
#endif


					resize(singleNum, singleNum, hog.winSize);

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
				dataOutput.WIN[i].dataValue = tempSum;
			if (j == 1)
				dataOutput.PLA[i].dataValue = tempSum;


		}			// end j

	}				// end i

	return EXEC_SUCCESS;
}


/**
* @brief

//������ֵ����Ƿ��� ��ɫ��ɫ
// srcMat��ɫԭͼ
*/
bool DataIdentify::haveGroundColor(Mat srcMat, int flag)
{
	//תΪ�Ҷ�ͼ��
	cvtColor(srcMat, srcMat, CV_RGB2GRAY);

	//�Ҷ���ֵ
	int graySum = 0;
	for (int c = 0; c < srcMat.cols; c++)
	{
		for (int r = 0; r < srcMat.rows; r++)
		{
			if (srcMat.at<uchar>(r, c) < 85)
			{
				srcMat.at<uchar>(r, c) = 0;
			}
			graySum += srcMat.at<uchar>(r, c);
		}


	}


#ifdef QDEBUG_OUTPUT
	qDebug("graySum = %d ", graySum);
#endif // DEBUG

	int threshold = 55000;
	if (graySum >= threshold)
	{
		return true;
	}
	else
		return false;


}


/**
* @brief


//ʶ�� QIN QPL   ͬʱʶ��ѣɣΡ��ѣУ�

*/
int DataIdentify::getQINQPLIdentify()
{

#ifdef QDEBUG_OUTPUT
	qDebug("getQINQPLIdentify func \n");
#endif
	judgeQINQPL();

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	CvRect rect[3][3];


#ifdef WRITE_ROI_SMAPLES_TEMP
	QString path = QString(".//temp//");

	writeSamples(QString("GetQinQpl.bmp"), image, path);


#endif
	float factor[3][3] = { { 1, 0.1, 0 }, { 10, 1, 0 }, { 100, 10, 1 } };							// the first line for dot, the second line for no dat

	// svm identify each number

	int dotFlag;
	Mat imageInfo2Mat(image, WHOLE_QINQPL_POS_RECT_LIVE);
	Mat imageInfo2RegionSub1(imageInfo2Mat, QINQPL_POS_RECT_LIVE);

	Mat imageTemp ;

	image.copyTo(imageTemp) ;

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
			//�ų����°� ����
			/*
			if (dataOutput.horseNum < 8)
			{
				if (j < 6)
				{
					continue;
				}
			}
			*/
			if (j >= QIN_QPL_COL + dataOutput.horseNum - 14)
			{
				continue;
			}

			if (j < i)
			{
				//�ų���ǰ��� �������������� 8 
				if (j <= 5 & j >= dataOutput.horseNum - 8)
				{
					continue;
				}
				if (i >(6 + dataOutput.horseNum) - 14)
				{
					continue;
				}
			}



			if (qinQPLPosStruct.rect[i][j].width < 5 | qinQPLPosStruct.rect[i][j].height < 5)
			{
				continue;
			}
			//���˵���������ƥ���� �����ý��Ϊ-1 
			if (j> i + 1)
			{
				if (dataOutput.mHorseInfo.isSCR[i] == true)
				{
					if (dataOutput.isQPL)
					{
						dataOutput.QPL[i][j].dataValue = -1;
					}
					else
					{
						dataOutput.QIN[i][j].dataValue = -1;

					}
					continue;
				}
				if (dataOutput.mHorseInfo.isSCR[j - 1] == true)
				{
					if (dataOutput.isQPL)
					{
						dataOutput.QPL[i][j].dataValue = -1;
					}
					else
					{
						dataOutput.QIN[i][j].dataValue = -1;

					}
					continue;
				}
			}
			if (j < i)
			{


				if (dataOutput.mHorseInfo.isSCR[j + 7] == true)
				{
					if (dataOutput.isQPL)
					{
						dataOutput.QPL[i][j].dataValue = -1;
					}
					else
					{
						dataOutput.QIN[i][j].dataValue = -1;

					}
					continue;
				}

				if (dataOutput.mHorseInfo.isSCR[i + 7] == true)
				{
					if (dataOutput.isQPL)
					{
						dataOutput.QPL[i][j].dataValue = -1;
					}
					else
					{
						dataOutput.QIN[i][j].dataValue = -1;

					}
					continue;
				}

			}
 
			Mat roi(imageTemp, qinQPLPosStruct.rect[i][j]);

			CvSize roiSize;
			roiSize.height = qinQPLPosStruct.rect[i][j].height;
			roiSize.width = qinQPLPosStruct.rect[i][j].width;

			 

			//	colorThreshold(roi, roiThreshold, 200);
			Mat roiThreshold;
			Mat roiThresholdEdge;
			cvtColor(roi, roi, CV_RGB2GRAY);

			roiThreshold = roi.clone();

			//ʹ�ûҶ���ֵ roiThreshold����ȥ�� �������ҿհ�
			int *graySum = new int [roiThreshold.cols + 1];
			memset(graySum, 0, (roiThreshold.cols + 1)*sizeof(int));

			int sum = 0 ;
			for (int c = 0; c < roiThreshold.cols; c++)
			{
				for (int r = 0; r < roiThreshold.rows; r++)
				{
					 
					if (roiThreshold.at<uchar>(r, c) > 190)
					{
						roiThreshold.at<uchar>(r, c) = 0;
						
					}
					 
					sum += roiThreshold.at<uchar>(r, c);

				}
			}
			bool hasGroudColor;
			if (sum > 30000)
			{
				hasGroudColor = true;
			}
			else
				hasGroudColor = false;

			//qDebug(" sum ALL = %d i = %d j = %d ",sum ,i ,j );
			//�����Ƿ��е�ɫ��ʹ�ò�ͬ��ֵ
			roiThreshold = roi.clone();
			for (int c = 0; c < roiThreshold.cols; c++)
			{
				for (int r = 0; r < roiThreshold.rows; r++)
				{
					if (hasGroudColor )
					{
						if (roiThreshold.at<uchar>(r, c) < 190)
						{
							roiThreshold.at<uchar>(r, c) = 0;

						}
					}
					else
					{
						if (roiThreshold.at<uchar>(r, c) < 140)
						{
							roiThreshold.at<uchar>(r, c) = 0;

						}
					}
					
					graySum[c] += roiThreshold.at<uchar>(r, c);
					 

				}
			}

			CvRect roiNewSize;
			//	dotFlag = identifyImageInfor2_Dot_live(&edge);
			Mat roiNew;
			Mat roiForDotJudge;

			/*trimRoiBlankPart(roiThreshold, roiForDotJudge, roiNewSize);

			if (algorithmState == EXIT_THIS_OCR)
			{
				return EXIT_THIS_OCR;
			}
			*/
			//�ü� roiThreshold
			int x0 = 0;
			int x1 = 0 ;
 
			for (int c = 0; c < roiThreshold.cols; c ++)
			{
				if (graySum[c] >= 180 )
				{
					x0 = c;
					break;
				}
			}
			for (int c = roiThreshold.cols -1 ; c >=0 ; c--)
			{
				if (graySum[c] >= 180)
				{
					x1 = c ;
					break;
				}
			}

			if (graySum != NULL )
			{
				delete[] graySum;
				graySum = NULL;
			}
			roiNewSize.x = x0;

			if (x1 < x0 )
			{
				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}


			// 
			int y1 = 0;
			int y0 = 0;

			int *graySumX = new int[roiThreshold.rows + 1];
			memset(graySumX, 0, (roiThreshold.rows + 1)*sizeof(int));

			for (int r = 0; r < roiThreshold.rows; r++)
			{
				for (int c = 0; c < roiThreshold.cols; c++)
				{
					 
					graySumX[r] += roiThreshold.at<uchar>(r, c);

				}
			}
			 

			for (int r = 0; r < roiThreshold.rows;r++)
			{
				if (graySumX[r] >= 180)
				{
					y0 = r ;
					break ;
				}
			}
			for (int r = roiThreshold.rows - 1; r >= 0; r--)
			{
				if (graySumX[r] >= 180)
				{
					y1 = r ;
					break ;
				}
			}


			roiNewSize.width = x1 - x0 + 1;
			 
			roiNewSize.y = y0 ;
			roiNewSize.height = y1-y0 + 1 ;

			if (graySumX != NULL)
			{
				delete[] graySumX;
				graySumX = NULL;
			}
 
			Mat roiClone;
			roiClone = roi.clone();

			roiNew = Mat(roi, roiNewSize);

			roiForDotJudge = Mat(roiClone, roiNewSize);

			/*
			//��ͼ�� ��ǿ Ȼ�����С�����ж�
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
			*/
			 
			for (int c = 0; c < roiForDotJudge.cols; c++)
			{
				for (int r = 0; r < roiForDotJudge.rows; r++)
				{
					if (roiForDotJudge.at<uchar>(r, c) < 140)
					{
						roiForDotJudge.at<uchar>(r, c) = 0;
					}
				}
			}

			int *x = new int[3 + 1];
			memset(x, 0, 4);
 
			if (i == 0 & j == 5 )
			{
				int temp = 0;

			}
			// 0 ��λ������С����
			// 1 ��λ��
			// 2 ��λ��
			Mat edge;
			dotFlag = judgeQINQPLDot(roiThreshold, edge, x);

#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2

			QString fileNameTemp;
			fileNameTemp.prepend(QString(".bmp"));
			fileNameTemp.prepend(QString("_6"));
			fileNameTemp.prepend(QString::number(j));
			fileNameTemp.prepend(QString("j_"));
			fileNameTemp.prepend(QString::number(i));
			fileNameTemp.prepend(QString("i_"));

			//	QString path = QString(".//temp//");

			writeSamples(fileNameTemp, roiThreshold, path);


#endif
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


			for (int r = 0; r < 3; r++)													// set the rect for single number in number region
			{
				for (int c = 0; c < 3; c++)
				{
					rect[r][c].y = 0;
					if (c < 2)
					{
						rect[r][c].width = rect[r][c + 1].x - rect[r][c].x;
					}
					else
					{
						rect[i][c].width = roiNewSize.width - rect[r][c].x;

					}
					rect[r][c].height = qinQPLPosStruct.rect[r][c].height;
				}
			}




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

					//����Ƿ�Խ��
					if (roiNew.cols < rect[0][k].x + rect[0][k].width |
						roiNew.rows < rect[0][k].y + rect[0][k].height)
					{
						if (x != NULL)
						{
							delete x;
							x = NULL;
						}
						algorithmState = EXIT_THIS_OCR;
						return EXIT_THIS_OCR;
					}
					Mat singleNum(roiNew, rect[0][k]);								// the single number image
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					QString fileNameTemp;
					fileNameTemp.prepend(QString(".bmp"));
					fileNameTemp.prepend(QString::number(k));
					fileNameTemp.prepend(QString("k_"));
					fileNameTemp.prepend(QString::number(j));
					fileNameTemp.prepend(QString("j_"));
					fileNameTemp.prepend(QString::number(i));
					fileNameTemp.prepend(QString("i_"));
					writeSamples(fileNameTemp, singleNum, path);
#endif

					resize(singleNum, singleNum, hog.winSize);
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

					//����Ƿ�Խ��
					if (roiNew.cols < rect[1][k].x + rect[1][k].width |
						roiNew.rows < rect[1][k].y + rect[1][k].height)
					{

						if (x != NULL)
						{
							delete x;
							x = NULL;
						}
						algorithmState = EXIT_THIS_OCR;
						return EXIT_THIS_OCR;
					}
					Mat singleNum(roiNew, rect[1][k]);								// the single number image
#ifdef WRITE_ROI_SMAPLES_CLASS_INFO2
					QString fileNameTemp;
					fileNameTemp.prepend(QString(".bmp"));
					fileNameTemp.prepend(QString::number(k));
					fileNameTemp.prepend(QString("k_"));
					fileNameTemp.prepend(QString::number(j));
					fileNameTemp.prepend(QString("j_"));
					fileNameTemp.prepend(QString::number(i));
					fileNameTemp.prepend(QString("i_"));
					writeSamples(fileNameTemp, singleNum, path);
#endif

					resize(singleNum, singleNum, hog.winSize);
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
						 
						rect[2][k].x	 = x[k]+1;
						rect[2][k].width = roiNew.cols - rect[2][k].x;
					}

					else
					{
						if (k == 1 )
						{
							rect[2][k].x = x[k] + 1;
						}
						else 
							rect[2][k].x = x[k] ;
						
						rect[2][k].width = x[k + 1] - rect[2][k].x ;
					}

					rect[2][k].height = roiNew.rows;
					//����Ƿ�Խ��
					if (roiNew.cols < rect[2][k].x + rect[2][k].width |
						roiNew.rows < rect[2][k].y + rect[2][k].height)
					{

						if (x != NULL)
						{
							delete x;
							x = NULL;
						}
						algorithmState = EXIT_THIS_OCR;
						return EXIT_THIS_OCR;
					}
					Mat singleNum(roiNew, rect[2][k]);								// the single number image

#ifdef  WRITE_ROI_SMAPLES_CLASS_INFO2
					QString fileNameTemp;
					fileNameTemp.prepend(QString(".bmp"));
					fileNameTemp.prepend(QString::number(k));
					fileNameTemp.prepend(QString("k_"));
					fileNameTemp.prepend(QString::number(j));
					fileNameTemp.prepend(QString("j_"));
					fileNameTemp.prepend(QString::number(i));
					fileNameTemp.prepend(QString("i_"));
					writeSamples(fileNameTemp, singleNum, path);
#endif
					resize(singleNum, singleNum, hog.winSize);
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
//ʶ�𳡴κ�

int DataIdentify::getSessionIdentify()
{

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	Mat roi(image, sessionPosStruct.rect[0]);


	cvtColor(roi, roi, CV_RGB2GRAY);
	resize(roi, roi, hog.winSize);
	hog.compute(roi, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result = numSVM.predict(hogMat);



	//dataOutput.session = int(result);

#ifdef WRITE_SESSION_CLASSIFY_SAMPELS 
	createClassifySamples(result, roi);

#endif

	return EXIT_SUCCESS;
}


/**
* @brief

//ʶ�𵹼�ʱ

*/
int DataIdentify::getCountDownIdentify()
{

	//countDownMinutePosStruct.rect[1] = cvRect(512 + originX, 4 + originY, 11, 17);
	// ʮλ��
	Mat minuteRoi1(image, countDownMinutePosStruct.rect[0]);
	//��λ��
	Mat minuteRoi2(image, countDownMinutePosStruct.rect[1]);

	cvtColor(minuteRoi1, minuteRoi1, CV_RGB2GRAY);
	cvtColor(minuteRoi2, minuteRoi2, CV_RGB2GRAY);

	//�Ҷ���ֵ

	for (int r = 0; r < minuteRoi1.rows;r++)
	{
		for (int c = 0; c < minuteRoi1.cols;c++)
		{
			if (minuteRoi1.at<uchar>(r,c) < 100 )
			{
				minuteRoi1.at<uchar>(r, c) = 0;
			}
		}
	}


	for (int r = 0; r < minuteRoi2.rows; r++)
	{
		for (int c = 0; c < minuteRoi2.cols; c++)
		{
			if (minuteRoi2.at<uchar>(r, c) < 100)
			{
				minuteRoi2.at<uchar>(r, c) = 0;
			}
		}
	}

	imwrite("minute1.bmp", minuteRoi1);
	imwrite("minute2.bmp", minuteRoi2);
	
	resize(minuteRoi1, minuteRoi1, hog.winSize);
	hog.compute(minuteRoi1, descriptorVector, winStride, padding);
	for (int m = 0; m < HOGFEATURENUMBER; m++)
		hogMat.at<float>(0, m) = descriptorVector[m];

	float result1 = raceTimeSvm.predict(hogMat);

	
	resize(minuteRoi2, minuteRoi2, hog.winSize);
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
			return EXEC_WARNING;							 // δ��⵽����ʱ��
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
			return EXEC_WARNING;
		}
		else
		{
			result1 = result1 * 10.0 + result2;
			// �����������ܴ���30 
			if (result1 > 30)
			{
				return EXEC_WARNING;
			}
			dataOutput.raceTime = (int)(result1);  //����ʱΪ������
		}
	}


	return EXEC_SUCCESS;

}

/**
* @brief

// ͨ������ �Ҷ��� X�����ϵ�ͶӰ ֮��ļ��Y���꣬�õ� WIN PLA ������������
// ����ͼ��Ϊ�Ҷ� canny�任��ͼ��

*/
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
		//д��ϵͳ��־
	//	Global::systemLog->append(QString(("DataIdentify  ")),
	//		QString("calculateGraySumXForSetWINPLARect func pixel y ") +
	//		QString::number(pixelY) + QString("graySUm") +
	//		QString::number(graySumX[pixelY]),
	//		SystemLog::INFO_TYPE);
	}
	int j = 0;
	int THEREHOLD = 300;
	for (int i = 0; i < dimension - 2; i++)
	{
		if (graySumX[0] > 300 & i == 0 )
		{
			y[j] = i;
			j++;
		}
		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
		//else if ((graySumX[i] < 300 && graySumX[i + 1] > 300
		//&& graySumX[i + 2] > 300)) ��ʱ�򣬻���� ��������ƥ �߶�Ϊ1 
		else if (graySumX[i] < 300 && graySumX[i + 1] > 300 )
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetHorseNameRect : The  y %d is  %d", j, i);
#endif
			j++;

			if (j > HORSENUMBER)
			{
				//Global::AppendLogString(QString("Error:calculateGraySumX :Func error  j != roiNum"), true);
#ifdef QDEBUG_OUTPUT
				qDebug("calculateGraySumXForSetHorseNameRect :Func error  j %d > HORSENUMBER %d\n ", j, HORSENUMBER);
#endif
				if (graySumX != NULL)
				{
					delete[] graySumX;
					graySumX = NULL;
				}

				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
			}



		}
	}

	if (graySumX != NULL)
	{
		delete[] graySumX;
		graySumX = NULL;
	}


	return EXEC_SUCCESS;

}
// ͨ������ �Ҷ��� X�����ϵ�ͶӰ ֮��ļ��Y���꣬�õ� �����ֵ����� 
// ����ͼ��Ϊ�Ҷ� canny�任��ͼ��

int DataIdentify::calculateGraySumXForSetHorseNameRect(Mat &mat, int *y, int &horseNum)
{

	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
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
	for (int i = 0; i < dimension - 2; i++)
	{
		if (graySumX[0] >300 & i == 0 )
		{
			y[0] = i;
			j++;
		}
		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
		else if ((graySumX[i] < 300 && graySumX[i + 1] > 300
			&& graySumX[i + 2] > 300))
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetHorseNameRect : The  y %d is  %d", j, i);
#endif
			j++;



			if (j > HORSENUMBER)
			{
				if (graySumX != NULL)
				{
					delete[] graySumX;
					graySumX = NULL;
				}

				//Global::AppendLogString(QString("Error:calculateGraySumX :Func error  j != roiNum"), true);
#ifdef QDEBUG_OUTPUT
				qDebug("calculateGraySumXForSetHorseNameRect :Func error  j%d != roiNum %d \n ",j,HORSENUMBER);
#endif

				algorithmState = EXIT_THIS_OCR;
				return EXIT_THIS_OCR;
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



	return EXEC_SUCCESS;

}

/**
* @brief
ͨ����� ����ͼƬ�ϰ벿�� �հ�����Ĵ�С�Լ� λ���������Ƿ����С����
*/


bool DataIdentify::judgeWINPLADot(int i, Mat &edge, Mat &roiThresholdEdge)
{



	Mat halfEdge;
	halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows / 2));
	/*
	Mat halfEdgeBelow;
	halfEdgeBelow = Mat(edge, cvRect(0, edge.rows / 2, edge.cols, edge.rows / 2));
	int* graySumBelow = new int[halfEdgeBelow.cols];
	*/
	//ȥ����ɫ��Ե����
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


	//��������
	int  maxLengthPos = 0;
	int maxLengthAdd = 0; //����
	int maxLengthMinus = 0; // ����
	int maxLength = 0;


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
		//��������Ϊ����ĳ�����ȷֵ1 ��
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
			maxLength = maxLengthAdd + maxLengthMinus + 1; // +1 ���������ڵ�λ�ü���

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


/**
* @brief
�ж� QIN QPL ���� �Ƿ���С����
*/
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
	int delta = backIndex - frontIndex + 1 ;

	if (delta > 0 )
	{
		CvRect newRect;
		newRect.x = frontIndex;
		newRect.y = 0 ;
		newRect.height = edge.rows;
		newRect.width = backIndex - frontIndex + 1 ;
		edge = Mat(edge, newRect);
	}
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
	else if (delta >= 25) // three number . 20
	{
		//��ȡ3�����ֵ� ����ֵ x
		if (calculateGraySumYForQINDotJudge(edge, x, 3) != EXEC_SUCCESS)
		{
			x[0] = 0;
			x[1] = edge.cols / 3;
			x[2] = edge.cols * 2 / 3;
			algorithmState = EXEC_SUCCESS;

		}
		if (graySum != NULL)
		{
			delete[] graySum;
			graySum = NULL;
		}
		return 2;
	}
	else if (delta <= 24 & delta > 19) // tow number with dot .
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
			if (calculateGraySumYForQINDotJudge(halfEdge, x, 3) != EXEC_SUCCESS)
			{
				x[0] = 0;
				x[1] = halfEdge.cols / 3;
				x[2] = halfEdge.cols * 2 / 3;
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
	else if (delta <= 19 & delta >= 5) // two number 
	{
		//��λ������ֹ ��λС�� ���� ���м�⣬ ͨ������Ѿ��޷����� ��λС������λ�� ��1.1 ��99
		//��ҪС����λ�ò�Ҫ���� ����1/4λ��
		Mat halfEdge = Mat(edge, cvRect(0, 0, edge.cols, edge.rows * 3 / 4));
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

			int sum = 0 ;
			for (int r = 0; r<edge.rows; r++)
			{
				for (int c = x[0] +2 ; c < x[1]-1; c++ )
				{
					sum += edge.at<uchar>(r, c);
				}
			}

			//��϶��������Ϊ С����
			if ((x[1] - x[0]) > 6)
			{
				if (sum >= 200 )
				{
					return 0;
				}
				else 
					return 1;
			}
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



/**
* @brief

ͨ��ʶ��qin qpl ��ǩ���жϴ�ʱΪ ������������
*/


int DataIdentify::judgeQINQPL()
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}

	//Mat roi(image, QINQPL_LABEL_POS_RECT_LIVE);

	Mat qinQPL(image,QINQPL_LABEL_POS_RECT_LIVE) ;



	cvtColor(qinQPL, qinQPL, CV_RGB2GRAY);

	Mat qinQPLOri;
	qinQPL.copyTo(qinQPLOri);

	int frontIndex;
	int backIndex;
	int *graySum = new int[qinQPL.cols+1];

	memset(graySum, 0, (qinQPL.cols + 1 )* sizeof(int));

	for (int c = 0; c < qinQPL.cols; c++)
	{
		for (int r = 0; r < qinQPL.rows; r++)
		{
			if (qinQPL.at<uchar>(r,c) < 150 )
			{
				qinQPL.at<uchar>(r, c) = 0;
			}
			graySum[c] += qinQPL.at<uchar>(r, c);

		}

		
	}

	imwrite("qinQPL.bmp", qinQPL);
	
	for (int i = 0; i < qinQPL.cols; i ++ )
	{
		if (graySum[i] > 300 )
		{
			frontIndex = i; 
			break;
		}
	}

	for (int i = qinQPL.cols ; i> 0 ; i -- )
	{
		if (graySum[i] > 300)
		{
			backIndex = i;
			break;
		}
	}


	if (graySum != NULL )
	{
		delete[] graySum;
	}

	CvRect newSize;
	newSize.x = frontIndex;
	newSize.width = backIndex - frontIndex + 1;

	if (newSize.width < 0 )
	{
		return EXIT_THIS_OCR ;
	}
	newSize.y = 0;
	newSize.height = qinQPL.rows;


	Mat roiWhole(qinQPLOri, newSize);
 
	CvRect newSize1;
	newSize1.x = roiWhole.cols - 10;
	newSize1.width = 10;
	newSize1.y = 0;
	newSize1.height = roiWhole.rows;

	Mat roi(roiWhole, newSize1);
	imwrite("qinQPLroi.bmp", roi);
	 
	resize(roi, roi, hog.winSize, 0, 0, INTER_CUBIC);
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
int  DataIdentify::trimRoiBlankPart(Mat &oriMat, Mat &newRoiMat, CvRect &roiNewSize)
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	
	/*
	int x0, x1;


	Mat edge;

	Canny(oriMat, edge, 150, 100, 3, true);

	//������С�ߴ�

	if (calculateGraySumYForTrim(edge, x0, x1, 1) == EXIT_THIS_OCR)
	{
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}


	roiNewSize.x = x0 ;

	if ((x1 + 2) > oriMat.cols)
		roiNewSize.width = oriMat.cols - x0;
	else
		roiNewSize.width = x1 - x0 + 2  ;

	roiNewSize.y = 0;
	roiNewSize.height = edge.rows;

	newRoiMat = Mat(oriMat, roiNewSize);


	if (roiNewSize.width < 15)
	{
		return EXIT_THIS_OCR;
	}
	else
		return EXEC_SUCCESS;
		*/

	int *sumX = new int[oriMat.rows];
	memset(sumX, 0, (oriMat.rows)*sizeof(int));

	int y0 = 0;
	int y1 = 0;

	//���� X����ͶӰ 
	for (int r = 0; r < oriMat.rows; r++)
	{
		for (int c = 0; c < oriMat.cols; c++)
		{
			sumX[r] += oriMat.at<uchar>(r, c);
		}

	}

	for (int r = 0; r < oriMat.rows; r++)
	{
		if (sumX[r] > 20)
		{
			y0 = r;
			break;

		}
	}

	for (int r = oriMat.rows - 1; r >= 0; r--)
	{
		if (sumX[r] > 20)
		{
			y1 = r;
			break;
		}
	}


	int *sumY = new int[oriMat.cols];
	memset(sumY, 0, (oriMat.cols)*sizeof(int));

	int x0 = 0;
	int x1 = 0;

	//���� X����ͶӰ 
	for (int c = 0; c < oriMat.cols; c++)
	{
		for (int r = 0; r < oriMat.rows; r++)
		{
			sumY[c] += oriMat.at<uchar>(r, c);
		}

	}


	for (int c = 0; c < oriMat.cols; c++)
	{
		if (sumY[c] > 20)
		{
			if (c > 0)
			{
				x0 = c - 1;
				break;
			}
			else
			{
				x0 = 0;
				break;
			}

		}
	}

	for (int c = oriMat.cols - 1; c >= 0; c--)
	{
		if (sumY[c] > 20)
		{
			x1 = c + 1;
			break;

		}
	}

	if (x0 > 0)
	{
		roiNewSize.x = x0 ;
		roiNewSize.width = x1 - x0  ;
	}
	else
	{
		roiNewSize.x = x0;
		roiNewSize.width = x1 - x0 ;
	}


	if (y0 > 0)
	{
		roiNewSize.y = y0;
		roiNewSize.height = y1 - y0 ;
	}
	else
	{
		roiNewSize.y = y0;
		roiNewSize.height = y1 - y0 ;

	}


	newRoiMat = Mat(oriMat, roiNewSize);
	// ����Y����ͶӰ


	if (sumX != NULL)
	{
		delete[] sumX;
		sumX = NULL;
	}

	if (sumY != NULL)
	{
		delete[] sumY;
		sumY = NULL;
	}


	if (roiNewSize.width < 15)
	{
		return EXIT_THIS_OCR;
	}
	else
		return EXEC_SUCCESS;
}




//�Բ�ɫͼ�� 3��RGB��ɫͨ��������ֵ���� ���� thereshold��ֵ �� 0 
// Ϊ�˸��õؽ�����ֵ����������ƽ��ֵ��Ȼ������ƽ��ֵ������ֵ
int DataIdentify::colorThreshold(Mat &src, Mat &dst, int thereshold)
{
	if (algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}
	//�Բ�ɫͼ������ֵ ȡ��

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



	for (int c = 0; c < src.cols; c++)
	{
		for (int r = 0; r < src.rows; r++)
		{
			pixel = src.at<Vec3b>(r, c);		// row,col index (NOT x,y)
			for (int p = 0; p < 3; p++)
			{
				if (p == 1)
				{
					if (pixel[p] <= thereshold + 50) // adaptiveThreshold[p])
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
int  DataIdentify::calculateXBewttenNumber(Mat &mat, int  *x)
{
	if (mat.empty() | algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}


	//	����ͶӰ
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
		return EXIT_THIS_OCR;
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

/**
* @brief ����ͶӰcalculateGraySumX(Mat &mat, int *y, int roiNum);
����ͼ��Ϊcannyͼ��
*/


int  DataIdentify::calculateGraySumYForQINDotJudge(Mat &mat, int  *x, int roiNum)
{
	if (mat.empty() | algorithmState == EXIT_THIS_OCR)
	{
		return EXIT_THIS_OCR;
	}


	//	����ͶӰ

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
				if (graySumX != NULL)
				{
					delete[] graySumX;
					graySumX = NULL;
				}

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
		qDebug("calculateGraySumYForQINDotJudge :Func error  j %d != roiNum %d \n ",j,roiNum);
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
int  DataIdentify::calculateGraySumYForTrim(Mat &mat, int &x0, int &x1, int roiNum)
{
	if (mat.empty() | algorithmState == EXIT_THIS_OCR)
	{
		//Global::AppendLogString(QString("Error:calculateGraySumY:mat is empty "), true);
#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim:mat is empty \n");
#endif

		return  EXIT_THIS_OCR;
	}
	//����  ����ͶӰ
	int * graySumY = new int[mat.cols + 1];

	memset(graySumY, 0, sizeof(int)*mat.cols);
	for (int pixelX = 0; pixelX < mat.cols; pixelX++)
	{
		for (int pixelY = 0; pixelY < mat.rows; pixelY++)
		{
			graySumY[pixelX] += mat.at<uchar>(pixelY, pixelX);
		}
		//graySumY[pixelX] /= roiNum;

#ifdef QDEBUG_OUTPUT
		qDebug("calculateGraySumYForTrim %d = %d \n", pixelX, graySumY[pixelX]);
#endif
	}
	int thereshold = 200;
	//��ȡ ����y����
	for (int x = 0; x <= mat.cols; x++)
	{
		//�ų��� �������� ����3������ ��200 ����Ϊ����������
		// �޸� ���������������� ��ʱ��Ὣ 1 ���˵��ˣ�����ֻ��1����ͨ�������������Ľ� 

		if (graySumY[x] > thereshold)
		{
			x0 = x;
			break;
		}
	}
	for (int x = mat.cols - 1; x >= 0; x--)
	{//�ų��� �������� ����3������ �� 600 Ϊ�� ȥ�� ��ɫ�������µ�С�� ����ʶ�� 300Ϊ�˶�1����ʶ��
		if (graySumY[x] > thereshold /* & graySumY[x - 2] > thereshold */)
		{
			x1 = x;
			break;
		}

	}
#ifdef QDEBUG_OUTPUT
	qDebug("x0 = %d x=%d", x0, x1);
#endif
	if (x1 <= x0 | x1 < 0 | x0 < 0)
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

int  DataIdentify::calculateGraySumXForSetQINQPLRect(Mat &mat, int  *y, int roiNum)
{
	if (mat.empty() | algorithmState == EXIT_THIS_OCR)
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
	int THEREHOLD = 200;
	for (int i = 0; i < dimension - 2; i++)
	{
		if (graySumX[0] > THEREHOLD & i ==0 )
		{
			y[j] = i;
			j++;
		}
		//����Ϊ 300 ���˵� �������ֵĵ�ɫ
		//else if ((graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
		 //&& graySumX[i + 2] > THEREHOLD))
		// �������ƥ��������ô�߶ȿ���Ϊ1 
		else if (graySumX[i] < THEREHOLD && graySumX[i + 1] > THEREHOLD
			)
		{
			y[j] = i;
#ifdef QDEBUG_OUTPUT
			qDebug(" calculateGraySumXForSetRect2 : The  y %d is  %d", j, i);


#endif
		 

			j++ ;

			i += 2;

			if (j > roiNum)
			{
				//Global::AppendLogString(QString("Error:calculateGraySumXForSetRect2 :Func error j>roiNum "), true);
#ifdef QDEBUG_OUTPUT
				qDebug("calculateGraySumXForSetRect2 :Func error j %d >roiNum %d \n ",j,roiNum);
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
		qDebug("calculateGraySumXForSetRect2 :Func error  j %d != roiNum %d \n ",j,roiNum);
#endif
 
		algorithmState = EXIT_THIS_OCR;
		return EXIT_THIS_OCR;
	}



	return EXEC_SUCCESS;
	//��ȡ ����y����
}



/*
��ʶ��Ľ������д�� ��ӦĿ¼ ��ҪΪ����
*/
void DataIdentify::createClassifySamples(float result, Mat &singleNum)
{


	//
	QString runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);
	//�˵���һ��Ŀ¼
	QDir::setCurrent("../");

	QDir::setCurrent("../");


	QDir::setCurrent(".//OCRProject//");


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
	//�˻ص�exe·��
	runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);




}


void DataIdentify::writeSamples(QString fileName, Mat &roi, QString path)
{

	//���»�ȡĿ¼
	QString runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);
	//�˵���һ��Ŀ¼
	QDir::setCurrent("../");

	QDir::setCurrent("../");


	QDir::setCurrent(".//OCRProject//");


	QString fileNameStr = path;


	//QString curPath = QDir::currentPath();
	QDir::setCurrent(fileNameStr);
	QString curPath = QDir::currentPath();
	//qDebug("curPath = %s \n", qPrintable(curPath));
	//curPath = QDir::currentPath();


	imwrite(fileName.toStdString(), roi);

	//�˻ص�exe·��
	runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);



}

