#include "BllDataIdentify.h"
#include <QDebug>
#include "Include/HorseDataStruct.h"
#include <QDir>

 
BllDataIdentify::BllDataIdentify(QObject *parent)
	: QObject(parent)
{


	ReadHistoryVideo myReadVideo;

	double totalFrames;
	double fps;
	myReadVideo.open("E:\\BaiduYunDownload\\20140528202552.mpg", totalFrames, fps);

	Mat frameImage;

	// 写数据文件

	logFile.setFileName("RDBSData.txt");
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return;


	stopDataIdentifyTag = false;//初始化

	for (int i = 0; i < 30; i++)
		memset( myRaceTimeStruct, 0, sizeof(raceNumTimeStruct)*30);
	for (int i = 0; i < 20;i++)
		memset( myRaceNumberStruct, 0, sizeof(raceNumTimeStruct)*20);
	

	memset((void * )&priDataOutput, 0, sizeof(DataOutput));

 

	dataNewCount = 0 ;
#ifdef WRITE_IMAGES_BEFORE_DataIdentify
	bmpCount = 0;
#endif


	QINQPLTransformedCountDown = 0;
	sessionChangedCountDown = 0;
	QINQPLChangedCountDown = 0;

	raceSessionCount = 0;

	raceTimeCountDownNear9 = false;



	for (int i = 0; i < HORSENUMBER; i++)
	{
		priDataOutput.mHorseInfo.horseID[i] = 0;
	//	priDataOutput.mHorseInfo.horseName[i] = QString(" ");
		priDataOutput.mHorseInfo.isSCR[i] = false;
	}
}

BllDataIdentify::~BllDataIdentify()
{

}
/**
* @brief 初始化算法
*/
void BllDataIdentify::init()
{
 
}
/**
* @brief 场次号被用户校正改变 ，
*/
void BllDataIdentify::sessionNumTextChanged()
{
	qDebug("BllDataIdentify: sessionNumTextChanged \n");

}

/**
* @brief 倒计时 被用户校正改变 ，
*/
void BllDataIdentify::sessionCountDownTextChanged()
{
	qDebug("BllDataIdentify: sessionCountDownTextChanged \n");

}


/**
* @brief 从多次识别出的倒计时、场次号中选出正确的
*/
LONG BllDataIdentify::chooseRightRaceTimeRaceSession(DataOutput &outputStruct)
{
	 
	if (outputStruct.session == -1 || outputStruct.raceTime == -1)
	{
		//return -1;
	}
	

	//判定场次号 以及倒计时  场次号发生变化 
	if (sessionChanged == true & sessionChangedDly1 != true )
	{
		if (outputStruct.session - Global::session == 1)
		{
			Global::session = outputStruct.session;
		}
		//已经检测到场次号发生变化了 + 1 
		else
		{
			//定时器清零 。新的场次号。
			Global::timerCount = 0;
			Global::session += 1;
			Global::raceTime = 0;

			raceSessionCount++;

			raceTimeCountDownNear9 = false;
		}
		
	 
		
	}// 场次号未发生变化 
	else
	{
		// 利用15次用于快速更新   替换原有的30
		if (dataNewCount <= 15 )
		{
			if (dataNewCount == 0)
			{
				for (int i = 0; i < 20; i++)
				{
					myRaceNumberStruct[i].content = 0;
					myRaceNumberStruct[i].contentCount = 0;
				}

				for (int i = 0; i < 30; i++)
				{
					myRaceTimeStruct[i].content = 0;
					myRaceTimeStruct[i].contentCount = 0;
				}
			}
			else
			{

				myRaceNumberStruct[outputStruct.session].content = outputStruct.session;
				myRaceNumberStruct[outputStruct.session].contentCount++;

				myRaceTimeStruct[outputStruct.raceTime].content = outputStruct.raceTime;
				myRaceTimeStruct[outputStruct.raceTime].contentCount++;

			}
			dataNewCount++;
		}
		else
		{
			//最大内容出现计数次数
			int maxContentCount;
			int maxContent;
			maxContentCount = myRaceNumberStruct[0].contentCount;
			maxContent = myRaceNumberStruct[0].content;
			//判决输出
			for (int i = 0; i < 20; i++)
			{

				if (maxContentCount < myRaceNumberStruct[i].contentCount)
				{
					maxContentCount = myRaceNumberStruct[i].contentCount;
					maxContent = myRaceNumberStruct[i].content;
				}


			}

			//Global::session = maxContent;
			if (maxContent - Global::session == 1)
			{
				Global::session = maxContent;
				raceSessionCount++;
				Global::timerCount = 0;
			}
			//如果某个时候，场次号被检测出的次数超过一定限值，可以认为，这个时候这个值为正确的

			if (maxContentCount > 8 )
			{
				Global::session = maxContent;
				raceSessionCount++;
				Global::timerCount = 0;
			}
			
			//挑出来 应该输出的  倒计时时间 
			maxContentCount = myRaceTimeStruct[0].contentCount;
			maxContent = myRaceTimeStruct[0].content;
			for (int i = 0; i < 30; i++)
			{

				if (maxContentCount < myRaceTimeStruct[i].contentCount)
				{
					maxContentCount = myRaceTimeStruct[i].contentCount;
					maxContent = myRaceTimeStruct[i].content;
				}


			}
			dataNewCount = 0;

			//第一次
			if (Global::raceTime == 0)
			{
				Global::raceTime = outputStruct.raceTime;
			} 
			else //非第一次
			{
				//取个位数
				if (raceTimeCountDownNear9 == true)
				{
					if (Global::raceTime - maxContent % 10 <= 3 & Global::raceTime - maxContent % 10 >= 0)
					{
						Global::raceTime = maxContent % 10;
					}

				}
				else
				{
					if (Global::raceTime - maxContent   <= 3 & Global::raceTime - maxContent  >= 0)
					{
						Global::raceTime = maxContent ;
					}
				}
			}
			
				
			

			 //检测到 比赛时间 到10min 了，这时 
			if (maxContent == 10 )
			{
				raceTimeCountDownNear9 = true;
			}
		

			memset(myRaceNumberStruct, 0, sizeof(raceNumTimeStruct));
			memset(myRaceTimeStruct, 0, sizeof(raceNumTimeStruct));



		}
	}
	//排除掉 0 的情况  raceSessionCount 当天比赛场次计数 ，这种情况发生在刚开始检测到了比赛数据。
	if (Global::session == 0 & raceSessionCount == 0 )
	{
		Global::session = 1;
		Global::timerCount = 0;
	}

	//此时没有检测到倒计时 输出 0 
	if (Global::raceTime == -1 )
	{
		Global::raceTime = 0;
	}
	// 
	sessionChangedDly1 = sessionChanged;

	 
	return 1;

}

/**
* @brief 判断数据是否为新数据
*/
LONG BllDataIdentify::winPlaDivTen(DataOutput &outputStruct)
{
	//无数据区域设置为0 
	for (int i = outputStruct.horseNum; i < 14;i ++ )
	{
		outputStruct.WIN[i] = 0;
		outputStruct.PLA[i] = 0;
	}
	for (int i = outputStruct.horseNum - 7; i < 7;i++)
	{
		for (int j = 0; j < i; j++)
			outputStruct.QPL_QIN[i][j] = 0;

	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = outputStruct.horseNum+1; j < 15; j++)
			outputStruct.QPL_QIN[i][j] = 0;
	}

	//设置 退赛马匹


	
	// 所有的win pla 设置为 ÷ 10
	int dataBigger10 = 0;
	float dataSmaller10 = 0;
	float dataTemp = 0;
	for (int i = 0; i < 14; i++)
	{
		dataTemp = outputStruct.WIN[i] / 10 ;

		if (dataTemp >= 10)
		{
			outputStruct.WIN[i] = (int)dataTemp;
		}
		else
		{
			dataBigger10 = (int)outputStruct.WIN[i];
			outputStruct.WIN[i] = (float)(dataBigger10 /10.0 );
		}

		dataTemp = outputStruct.PLA[i] / 10;

		if (dataTemp >= 10)
		{
			outputStruct.PLA[i] = (int)dataTemp;
		}
		else
		{
			dataBigger10 = (int)outputStruct.PLA[i];
			outputStruct.PLA[i] = (float)(dataBigger10 / 10.0);
		}
		 
	}
	

	return 1 ;
}

/**
* @brief 判断数据是否为新数据 同时判断数据是否过大 如果变化范围超过此数值的原值的2/1
 则认为是错误，丢弃。采用原有 识别结果。
*/
LONG BllDataIdentify::isDataOutputNew(DataOutput &outputStruct)
{

	// 数据除以10
	if (Global::videoType != YAZHOUTAI )
	{
		winPlaDivTen(outputStruct);
	}
	

	bool dataOutOfRange = false ;
	int  dataOutOfRangeCount = 0 ;
	int  WINChangedNum = 0;
	int  PLAChangedNum = 0;
	int  QINQPLCHangedNum = 0;
	// 判断WIN数据是否变化
	outputStruct.changeStatus = 0 ;



	//WIN改变数目过多 等待3个周期在发送新数据，并且有2个以上数据 超出了改变那范围 ，此时场次号发生了改变
	if (WINChangedNum >= HORSENUMBER / 2 & PLAChangedNum >= HORSENUMBER / 2 & (dataOutOfRangeCount > 2))
	{
		sessionChangedCountDown = 3;
		sessionChanged = true;

	}
	//当QIN QPL发生切换时候 ，等待3个定时器周期，在发送新的数据，保持数据稳定。
	if (priDataOutput.isQPL != outputStruct.isQPL)
	{
		if (sessionChanged == false)
		{
			QINQPLTransformedCountDown = 3;
			QINQPLTransformed = true;
		}

	}

	if (sessionChangedCountDown == 0 )
	{
		for (int i = 0; i < outputStruct.horseNum; i++)
		{
			if (abs(outputStruct.WIN[i] - priDataOutput.WIN[i]) > 0.05)
			{
				//qDebug("WIN : i=%d , new is %f pri is %f ", i,  outputStruct.WIN[i], priDataOutput.WIN[i]);
				//outputStruct.changeStatus = WIN_CHANGED;
				WINChangedNum++;

			}
			if (abs((outputStruct.WIN[i] - priDataOutput.WIN[i])) / outputStruct.WIN[i] > 0.5)
			{
				dataOutOfRange = true;
				dataOutOfRangeCount++;
			}

		}


		for (int i = 0; i < outputStruct.horseNum; i++)
		{
			if (outputStruct.PLA[i] == 0.0)
			{
				continue;
			}
			if (abs(outputStruct.PLA[i] - priDataOutput.PLA[i]) >  0.05)
			{
				//qDebug("PLA:i=%d ,new is %f pri is %f ", i,  outputStruct.PLA[i], priDataOutput.PLA[i]);
				//outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
				PLAChangedNum++;
			}
			if (abs((outputStruct.PLA[i] - priDataOutput.PLA[i])) / outputStruct.PLA[i] > 0.5)
			{
				dataOutOfRange = true;
				dataOutOfRangeCount++;
			}


		}
	}
	

	if (QINQPLTransformedCountDown == 0)
	{
		for (int i = 0; i < 7; i++)
		{

			for (int j = 0; j < outputStruct.horseNum; j++)
			{
				if (i == j || i == (j + 1))
					continue;
				//排除掉 退赛的马匹
				if (j> i + 1)
				{
					if (outputStruct.mHorseInfo.isSCR[i] == true)
					{
						 
						continue;
					}
					if (outputStruct.mHorseInfo.isSCR[j - 1] == true)
					{
						 
						continue;
					}
				}
				if (j < i)
				{

					if (outputStruct.mHorseInfo.isSCR[j + 7] == true)
					{ 
						continue;
					}
				}

				if (abs(outputStruct.QPL_QIN[i][j] - priDataOutput.QPL_QIN[i][j]) > 0.05)
				{
					//qDebug("QIN_QPL:i=%d ,j=%d new is %f pri is %f ", i, j, outputStruct.QPL_QIN[i][j], priDataOutput.QPL_QIN[i][j]);
					//outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
					QINQPLCHangedNum++;
				}
				if (priDataOutput.isQPL == outputStruct.isQPL)
				{
					if (outputStruct.QPL_QIN[i][j] == 0.0)
					{
						continue; 
					}
					if (abs((outputStruct.QPL_QIN[i][j] - priDataOutput.QPL_QIN[i][j])) / outputStruct.QPL_QIN[i][j] > 0.5)
					{

						dataOutOfRange = true;
#ifdef QDEBUG_OUTPUT
						qDebug("i=%d,j=%d,cur = %f , pri =%f", i, j, outputStruct.QPL_QIN[i][j], priDataOutput.QPL_QIN[i][j]);
#endif
						dataOutOfRangeCount++;
					}

				}

			}
		}
	}
	
	//数据超出本应有的范围，那么就不会发送此时的数据， 此时应该避免掉 比赛刚开始，有数据的时候，此时应该发送数据。
	//添加 Global::raceHasStarted 来控制 第一次，即 比赛未开始，那么可以发送的。
	if (dataOutOfRange == false | Global::raceHasStarted == 0 )
	{
		Global::raceHasStarted = 1;
		if (sessionChangedCountDown == 0 )
		{
			sessionChanged = false;
		}
		if (QINQPLTransformedCountDown == 0 )
		{
			QINQPLTransformed = false;
		}
		if (sessionChangedCountDown > 0)
		{
			sessionChangedCountDown--;
		}
		if (QINQPLTransformedCountDown > 0)
		{
			QINQPLTransformedCountDown--;
		}

		if (WINChangedNum > 0)
		{
			if (sessionChangedCountDown == 0)
				outputStruct.changeStatus = WIN_CHANGED;
		}
		else if (PLAChangedNum > 0)
		{
			if (sessionChangedCountDown == 0)
				outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
		}
		else if (QINQPLCHangedNum > 0)
		{
			if (QINQPLTransformedCountDown == 0)
				outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
		}
	}
	else
	{
		outputStruct.changeStatus = 0;
	}
	
	if (sessionChangedCountDown == 0 )
	{
		priDataOutput = outputStruct;

	}
	
	// 选择正确场次号，倒计时
	chooseRightRaceTimeRaceSession(outputStruct);

	if (outputStruct.changeStatus > 0 )
	{
		writeDataFile(outputStruct);
	}
		
	return 1;
}

/**
* @brief 开始识别
*/
int BllDataIdentify::saveToLocalFile(char *data, QString path)
{
 
#ifdef WRITE_IMAGES_BEFORE_DataIdentify
		Mat mat_temp(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, data);

		bmpCount++;
		QPixmap pixmap;
		QImage myImage;
		myImage = QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);


		QString bmpFileName;
		bmpFileName = QString(".bmp");
		bmpFileName.prepend(QString::number(bmpCount % 100));
  
		// 转换RGB888 到QIMAGE
		for (int h = 0; h < IMAGE_HEIGHT; h++) {
			// scanLine returns a ptr to the start of the data for that row 
			memcpy(myImage.scanLine(h), (data + IMAGE_WIDTH * 3 * h),
				IMAGE_WIDTH * 3);
		}
 
		//QString curPath = QDir::currentPath();
		QDir::setCurrent(path);
#ifndef OFFLINE_DEBUG

		myImage.save(bmpFileName);

#endif // !OFFLINE_DEBUG
#ifdef OFFLINE_DEBUG 
	/*	myImage.save("acq.bmp");
		if (bmpCount == 14)
		{
			qDebug("Debug MARK");
		}*/
		qDebug("读入第 %d 幅图像 \n", bmpCount);
#endif
		//退到上一层目录
		QDir::setCurrent("../");
		QString curPath = QDir::currentPath();
 
#endif

		return 1;
}

/*
开始历史视频识别
*/
int BllDataIdentify::startHistoryDataIdentify(QString fileName, int videoType)
{
	//注册该对象为元对象可识别系统，元对象将会将该参数保存起来，供队列连接调用。特别针对多线程
	qRegisterMetaType<DataOutput>("DataOutput");

	double totalFrames;
	double videoFps;
	myReadHistoryVideo.open(fileName, totalFrames, videoFps);
	Mat frameMat;

	int progressPercent;
	if (Global::videoStartPos < 0  | Global::videoStartPos > totalFrames / videoFps )
	{
		Global::videoStartPos = 0;
	}
	for (int f = Global::videoStartPos; f < totalFrames/videoFps & !Global::acqStop ;f++)
	{
		myReadHistoryVideo.read(f, frameMat);

		qDebug("frame count = %d",f);
		progressPercent = 100* f * videoFps / totalFrames;

		algorithmExecHistory(videoType, NULL, frameMat, progressPercent);

		Sleep(800);


	}
	
  
	return 1;

}
/*
算法执行 识别

*/

int BllDataIdentify::algorithmExecHistory(int videoType, uchar * imageBuf, Mat &srcMat, int progressPercent)
{
	//算法 
	Mat imageTemp;
	srcMat.copyTo(imageTemp);

	if (videoType == YAZHOUTAI )
	{
	//	mYZTDataIdentify = new YZTDataIdentify;
	  
		bool emptyData = (mYZTDataIdentify).read(imageTemp, imageBuf, (IMAGE_BUFF_LENGTH - BMP_HEADER),
			IMAGE_HEIGHT, IMAGE_WIDTH);

		if (emptyData == true)
		{
			qDebug() << "empty image data" << endl;

		}

		int imageWidth;
		int imageHeight;

		QByteArray byteArray;

		bool isHistoryVideo;

		//	imshow("q",srcMat);
		//	waitKey();
		int nChannel = srcMat.channels();
		//	buf = new uchar[srcMat.cols*srcMat.rows*nChannel];
		QImage img;
		if (nChannel == 3)
		{

			cv::cvtColor(srcMat, srcMat, CV_BGR2RGB);
			img = QImage((const unsigned char*)srcMat.data, srcMat.cols, srcMat.rows, QImage::Format_RGB888);

		}

		imageWidth = srcMat.cols;
		imageHeight = srcMat.rows;
		byteArray.append((char *)img.bits(), srcMat.cols * srcMat.rows*nChannel);

		isHistoryVideo = true;


		int rtValue = (mYZTDataIdentify).identify();

		(mYZTDataIdentify).dataOutput.videoProgressPercent = progressPercent;

		if ((mYZTDataIdentify).haveDataFlag == false) //广告
		{
			//显示图片，但是不输出结果
			emit readyReadBmp((mYZTDataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

		}
		else if ((mYZTDataIdentify).haveDataFlag == true)
		{
			//获取算法数据
			DataOutput outputStruct = (mYZTDataIdentify).dataOutput;
			//数据处理，屏蔽无效数据为-1

			isDataOutputNew(outputStruct);

			//数据有改变才会发送信号
			if (outputStruct.changeStatus >= 0)
			{

				emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
			}

			qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
		}


	}
	else if (videoType == HK18TAI)
	{
		  
	//	mHK18DataIdentify = new HK18DataIdentify;
	  

		bool emptyData = (mHK18DataIdentify).read(imageTemp, imageBuf, (IMAGE_BUFF_LENGTH - BMP_HEADER),
			IMAGE_HEIGHT, IMAGE_WIDTH);

		if (emptyData == true)
		{
			qDebug() << "empty image data" << endl;

		}

		int imageWidth;
		int imageHeight;

		QByteArray byteArray;

		bool isHistoryVideo;

		//	imshow("q",srcMat);
		//	waitKey();
		int nChannel = srcMat.channels();
		//	buf = new uchar[srcMat.cols*srcMat.rows*nChannel];
		QImage img;
		if (nChannel == 3)
		{

			cv::cvtColor(srcMat, srcMat, CV_BGR2RGB);
			img = QImage((const unsigned char*)srcMat.data, srcMat.cols, srcMat.rows, QImage::Format_RGB888);

		}

		imageWidth = srcMat.cols;
		imageHeight = srcMat.rows;
		byteArray.append((char *)img.bits(), srcMat.cols * srcMat.rows*nChannel);

		isHistoryVideo = true;


		int rtValue = (mHK18DataIdentify).identify();

		(mHK18DataIdentify).dataOutput.videoProgressPercent = progressPercent;


		if ((mHK18DataIdentify).haveDataFlag == false) //广告
		{
			//显示图片，但是不输出结果
			emit readyReadBmp((mHK18DataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

		}
		else if ((mHK18DataIdentify).haveDataFlag == true)
		{
			//获取算法数据
			DataOutput outputStruct = (mHK18DataIdentify).dataOutput;
			//数据处理，屏蔽无效数据为-1

			isDataOutputNew(outputStruct);

			//数据有改变才会发送信号
			if (outputStruct.changeStatus >= 0)
			{

				emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
			}

			qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
		}


	}

	
	
	 
  
	return 1;
}

/**/
int BllDataIdentify::algorithmExecLive(int videoType, uchar * imageBuf, Mat &srcMat)
{ 
	//算法 
 
	if (videoType != LIVE )
	{
		return EXIT_THIS_OCR ;
	}
	Mat imageTemp;
	srcMat.copyTo(imageTemp);

	
	bool emptyData = dataIdentifyClass.read(imageTemp, imageBuf, (IMAGE_BUFF_LENGTH - BMP_HEADER),
		IMAGE_HEIGHT, IMAGE_WIDTH);

	if (emptyData == true)
	{
		qDebug() << "empty image data" << endl;

	}

	int imageWidth;
	int imageHeight;

	QByteArray byteArray;

	bool isHistoryVideo;
	 
	imageWidth = IMAGE_WIDTH;
	imageHeight = IMAGE_HEIGHT;

	byteArray.append((char *)imageBuf, imageWidth * imageHeight * 3);
	 


	int rtValue = dataIdentifyClass.identify();

	if (dataIdentifyClass.haveDataFlag == false) //广告
	{
		//显示图片，但是不输出结果
		emit readyReadBmp(dataIdentifyClass.dataOutput, byteArray, imageWidth, imageHeight);

	}
	else if (dataIdentifyClass.haveDataFlag == true)
	{
		//获取算法数据
		DataOutput outputStruct = dataIdentifyClass.dataOutput;
		//数据处理，屏蔽无效数据为-1

		isDataOutputNew(outputStruct);

		//数据有改变才会发送信号
		if (outputStruct.changeStatus >= 0)
		{

			emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
		}

		qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
	}

	return 1;
}
/*
开始实时直播 识别 
*/
int  BllDataIdentify::startLiveDataIdentify(int videoType)
{
	Global::stopDataIdentifyTag = false;//开始跑
	int width = IMAGE_WIDTH;
	int height = IMAGE_HEIGHT;

	uchar* data = new uchar[IMAGE_BUFF_LENGTH];
	//注册该对象为元对象可识别系统，元对象将会将该参数保存起来，供队列连接调用。特别针对多线程
	qRegisterMetaType <DataOutput> ("DataOutput");

	//算法
	while ( !Global::stopDataIdentifyTag )
	{

		if (Global::S_CCycleBuffer->getUsedSize() >= IMAGE_BUFF_LENGTH)
		{
			Global::S_CCycleBuffer->read((char*)data, IMAGE_BUFF_LENGTH);

			QString path = "./acqImages/";
			//saveToLocalFile((char*)data, path);

			//算法
			algorithmExecLive(videoType, data, Mat());
		}
 
	}

	delete[] data;
	qDebug() << "【BllDataIdentify】退出识别";
	return 1;
}
/**
* @brief 开始识别
*/
void BllDataIdentify::start(QString fileName, int videoType)
{

	if (videoType != LIVE )
	{
		startHistoryDataIdentify(fileName, videoType);
	}
	else
	{ // LIVE 
		startLiveDataIdentify(videoType );
	}
	

}


void BllDataIdentify::writeDataFile(DataOutput &dataOutput)
{
	 
 
	QString logStr;

	QTextStream logContentOut(&logFile);

	logStr = QString("第") + QString::number(bmpCount) + QString("幅图像");
	logContentOut << logStr << "\n";
	logContentOut << " WIN			PLA \n";
	// 写WIN PLA
	for (int i = 0; i < 14; i++)
	{
		 
		logContentOut << QString::number(dataOutput.WIN[i]) << "	";

		logContentOut << QString::number(dataOutput.WIN[i] - priDataOutput.WIN[i]) << "		";

		logContentOut << QString::number(dataOutput.PLA[i]) << "\n" ;
		logContentOut << QString::number(dataOutput.PLA[i] - priDataOutput.PLA[i]) << "		";
 
	}

	logContentOut << "QIN or QPL ";
	for (int j = 0; j < 15; j++)
	{
		logContentOut << "	";
		logContentOut << QString::number(j) ;
	}
	logContentOut << " \n ";
	for (int i = 0; i < 7; i++)
	{
		logContentOut <<QString::number(i) ;
		logContentOut << "		";
		for (int j = 0; j < 15; j++)
		{
			logContentOut << dataOutput.QPL_QIN[i][j];
			logContentOut << " ";
			logContentOut << (dataOutput.QPL_QIN[i][j] - priDataOutput.QPL_QIN[i][j]);

			logContentOut << "	";
		}
		logContentOut << "\n";
	}
	

}
/**
* @brief 停止识别
*/
void BllDataIdentify::stop()
{
	Global::stopDataIdentifyTag = true;
}

 