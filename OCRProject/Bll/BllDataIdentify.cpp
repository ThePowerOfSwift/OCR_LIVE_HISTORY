#include "BllDataIdentify.h"
#include <QDebug>
#include "Include/HorseDataStruct.h"
#include <QDir>

 
BllDataIdentify::BllDataIdentify(QObject *parent)
	: QObject(parent)
{
	 
	// 写数据文件

	logFile.setFileName("RDBSData.txt");
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
		 qDebug("logFile open Failed \n");


	pauseDataIdentifyTag = false;//初始化

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

	isRaceSessionDetected = false;

	isRightRaceTimeCountDownDetected = false;

	frameAccValue = 0;

	Global::frameAccValue = 1;


	//打开文件
	horseNameIdDataFile.setFileName(".//dat//horseNameIdDataFile.txt");

	if (!horseNameIdDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	horseNameIdData.setDevice(&horseNameIdDataFile);

	while (!horseNameIdData.atEnd())
	{
		horseNameIdStr += horseNameIdData.readLine();

	}

	//刨掉前5000匹马

	horseNameIdStr = horseNameIdStr.mid(20000, horseNameIdStr.size() - 20000);
	horseNameIdDataFile.close();

	horseNameHistoryDataFile.setFileName(".//dat//horseNameHistoryDataFile.txt");
	if (!horseNameHistoryDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	horseNameHistoryData.setDevice(&horseNameHistoryDataFile);

	while (!horseNameHistoryData.atEnd())
	{
		horseNameHistoryStr += horseNameHistoryData.readLine();

	}

	horseNameIdDataFile.close();

	videoFileDate = QString("");


	raceCountDownTime = 0;

	QINQPLTransformed = false;

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
	 // 场次号改变标记为 false
   
	//利用马的名字灰度变化检测 场次号变化，从而计算场次号。 
	// 第一次检测到的场次号必须是正确的，否则所有场次号都是错误的。
	// 如果每次都从1开始，那么可以保证 所有场次号正确。
 
	//每次都执行
	if (Global::session >= 1 )
	{
		
		 if (Global::isSessionChanged)
		 {
			//清空
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
			
		 
			 isRightRaceTimeCountDownDetected = false;
			 raceTimeCountDownNear9 = false;

			 Global::raceTime = 0;
			 Global::countRaceTime = 0;

			 raceCountDownTime = 0;
		 	
		 }
		 
		//获取马名 由于outputStruct 每次都会清空，所以需要每次都要获取
		getHorseNameFromDataFile(videoFileName, outputStruct);

		// 日期发生了变化，那么清空一下 
		if (videoFileDate != Global::historyVideoDate)
		{
			//第一次不要初始化，以后文件名发生变化的时候在进行初始化
			if (videoFileDate.size() > 2 )
			{
				initGlobal();
			}
		
		}
		//日期赋值
		videoFileDate = Global::historyVideoDate;
		 
	}
	//如果场次号被校正了
	if (Global::isSessioncalibrated)
	{
		//获取马名
		getHorseNameFromDataFile(videoFileName, outputStruct);

		// 标志位清零
		Global::isSessioncalibrated = false;
		
		isRightRaceTimeCountDownDetected = false;
	 
		raceTimeCountDownNear9 = false;

	}
  
	
	// 统计15次， 然后寻找出现次数最多的

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
		 
		//如果某个时候，场次号被检测出的次数超过一定限值，可以认为，这个时候这个值为正确的

		if (maxContentCount > 8 & isRaceSessionDetected == false )
		{
			 
			//获取第一次检测到了场次号。
			firstRaceSessionDetected = maxContent;
			//当前场次正确的倒计时没有被检测到。
			isRightRaceTimeCountDownDetected = false;
			isRaceSessionDetected = true;
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
 
		{
			
			if (isRightRaceTimeCountDownDetected == true)
			{
				//取个位数 倒计时
				if (raceTimeCountDownNear9 == true)
				{
					if (Global::raceTime - maxContent % 10 <= 3 & Global::raceTime - maxContent % 10 >= 0)
					{
						Global::raceTime = maxContent % 10;

						//总时长
						emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);
					}
					//如果出现次数很多，强行赋值
					else if (maxContentCount >= 10 )
					{
						 
						Global::raceTime = maxContent % 10;
							 
						//总时长
						emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);
					}

				}
				else
				{
					if (Global::raceTime - maxContent <= 3 & Global::raceTime - maxContent >= 0)
					{
						Global::raceTime = maxContent;
						//总时长
						emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);

					}
						//如果出现次数很多，强行赋值
					else if (maxContentCount >= 10)
					{
							 
						Global::raceTime = maxContent;
						//总时长
						emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);
						 
					}
				}
			}

			//第一次一定要检测到倒计时 发生了递减1 才能认为识别正确了倒计时
			if (maxContent >= 10 & isRightRaceTimeCountDownDetected == false)
			{
				//第一次一定要检测到倒计时 发生了递减1 才能认为识别正确了倒计时
				if (raceCountDownTime - maxContent == 1)
				{
					isRightRaceTimeCountDownDetected = true ;
					Global::raceTime = maxContent ;
					//总时长
					emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);
				}
				raceCountDownTime = maxContent ;

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
	

	//当QIN QPL发生切换时候 ，等待3个定时器周期，在发送新的数据，保持数据稳定。
	//等待5幅图片的时间
	//if ((outputStruct.horseNameChangedNum - Global::session != 1)
	//	& (priDataOutput.isQPL != outputStruct.isQPL) )
	if (priDataOutput.isQPL != outputStruct.isQPL & QINQPLTransformed == false)
	{
		//if (sessionChanged == false)
		{
			QINQPLTransformedCountDown = 5 ;
			QINQPLTransformed = true ;
		}

		//标记QIN QPL 发生了转变
		 

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

			for (int j = 0; j <= outputStruct.horseNum; j++)
			{
				if (i == j || j == (i + 1))
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
	
	//WIN改变数目过多 等待3个周期在发送新数据，并且有2个以上数据 超出了改变那范围 ，此时场次号发生了改变
	//如果场次号发生了变化，那么立刻发送

	if (Global::isSessionChanged)
	{
		sessionChangedCountDown = 4 ;
		sessionChanged = true;

	}
	

	//数据超出本应有的范围，那么就不会发送此时的数据， 此时应该避免掉 比赛刚开始，有数据的时候，此时应该发送数据。
	//添加 Global::raceHasStarted 来控制 第一次，即 比赛未开始，那么可以发送的。
	// 如果场次号发生了变化
	if (dataOutOfRange == false | Global::raceHasStarted == 0 | QINQPLTransformed |
		Global::isSessionChanged )
	{
		Global::raceHasStarted = 1;
		if (sessionChangedCountDown == 1 )
		{
			sessionChanged = false;
		}
		if (QINQPLTransformedCountDown == 1)
		{
			QINQPLTransformed = false;
		}
		

		if (WINChangedNum > 0)
		{
		//	if (sessionChangedCountDown == 0)
 				outputStruct.changeStatus = WIN_CHANGED;
		}
		if (PLAChangedNum > 0)
		{
		//	if (sessionChangedCountDown == 0)
				outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
		}
		if (QINQPLCHangedNum > 0)
		{
			if (QINQPLTransformedCountDown == 0)
				outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
						
		}
		//如果此时刚发生QIN QPL转变，那么发送数据
		if (QINQPLTransformedCountDown == 1)
		{
			outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
		}

		//如果场次号发生了变化，那么所有数据都要发送
		if (sessionChangedCountDown == 1 )
		{
			outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
			outputStruct.changeStatus = outputStruct.changeStatus | WIN_CHANGED;
			outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
		}
	}
	else
	{
		outputStruct.changeStatus = 0;
 

	}

	//用于控制等待的计数器，开始自减
	if (sessionChangedCountDown > 0)
	{
		sessionChangedCountDown--;
	}
	if (QINQPLTransformedCountDown > 0)
	{
		QINQPLTransformedCountDown--;
	}

	// 选择正确场次号，倒计时
	chooseRightRaceTimeRaceSession(outputStruct);

	

	 //如果场次号发生了变化，那么要对priDataOutput赋值，如果qinqpl发生了变化，
	// 同样也要赋值的

	if (outputStruct.changeStatus > 0 | Global::isSessionChanged | QINQPLTransformed )
	{
		// 只有 数据发生了变化，才将数据送入 priDataOutput
		if (sessionChangedCountDown == 0)
		{
			priDataOutput = outputStruct;

		}
		//如果是历史视频，那么才写入数据到文件里面 否则 
		if (Global::isHistoryVideo)
		{
			writeDataFile(outputStruct);
		}
	
	}

	//发现是否场次号发生变化，如果变化了，标志位
	if (Global::isSessionChanged)
	{
		Global::isSessionChanged = false;

		Global::isSessionRaceIdRequested = false;
		//请求新场次号
		emit requestRaceIdSig();

	}

		
	return 1;
}

/**
* @brief 将数据写入文件
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
	//myReadHistoryVideo = new ReadHistoryVideo;
	myReadHistoryVideo.open(fileName, totalFrames, videoFps);
	Mat frameMat;

	int progressPercent;
	if (Global::videoStartPos < 0  | Global::videoStartPos > totalFrames / videoFps )
	{
		Global::videoStartPos = 0;
	}
	for (int f = Global::videoStartPos; f <= ((totalFrames / videoFps)  )& !Global::stopDataIdentifyTag;)
	{
		Global::isHistoryVideoIdentifyRuning = true;
		if (f >= ((totalFrames / videoFps) ) )
		{
			break; 
		}
		myReadHistoryVideo.read(f, frameMat);

		qDebug("frame count = %d",f);
		progressPercent = 100 * f / (totalFrames/ videoFps);

		imwrite("ReadframeMat.bmp", frameMat);
	 
		algorithmExecHistory(videoType, NULL, frameMat, progressPercent);


		Sleep(800);
		 

		f += Global::frameAccValue;

		// 统计一个场次的帧数
		Global::historyFrameCount += Global::frameAccValue;
		// 分钟数
		Global::countRaceTime = Global::historyFrameCount / 60;

		Global::frameAccValue = 1;

		//暂停即进入睡眠，停止暂停，退出 一旦此时有快进请求那么，进行快进或者倒退显示

		Global::requestAdvanceDuringPause = false;
		while (Global::pauseDataIdentifyTag & !Global::requestAdvanceDuringPause )
		{
			Sleep(1);
		}
	
	}
	
	myReadHistoryVideo.close();
	//处理完本文件 激发一个信号，读取文件列表下一个文件
	
	/*
	if (myReadHistoryVideo != NULL)
	{
		delete myReadHistoryVideo;
		myReadHistoryVideo = NULL;
	}
	*/

	emit readNextFile();

	Global::isHistoryVideoIdentifyRuning = false ;
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
			else //如果没有改变，那么此时可以发送缓存未发送的数据
			{
				if (Global::sendDataCCycleBuffer->getBufSize() > 0)
				{
					emit sendBufferDataSig();
				}
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
			else
			{
				if (Global::sendDataCCycleBuffer->getBufSize() > 0)
				{
					emit sendBufferDataSig();
				}
			}

			qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
		}


	}
	else if (videoType == HK18D14)
	{

		//	mHK18DataIdentify = new HK18DataIdentify;
		if (Global::is63TAIVideoData)
		{
			bool emptyData = (mHK63D14DataIdentify).read(imageTemp, imageBuf, (IMAGE_BUFF_LENGTH - BMP_HEADER),
				IMAGE_HEIGHT, IMAGE_WIDTH);

			if (emptyData == true)
			{
				return -1;
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


			int rtValue = (mHK63D14DataIdentify).identify();

			(mHK63D14DataIdentify).dataOutput.videoProgressPercent = progressPercent;


			if ((mHK63D14DataIdentify).haveDataFlag == false) //广告
			{
				//显示图片，但是不输出结果
				emit readyReadBmp((mHK63D14DataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

			}
			else if ((mHK63D14DataIdentify).haveDataFlag == true)
			{
				//获取算法数据
				DataOutput outputStruct = (mHK63D14DataIdentify).dataOutput;
				//数据处理，屏蔽无效数据为-1

				isDataOutputNew(outputStruct);

				//数据有改变才会发送信号
				if (outputStruct.changeStatus >= 0)
				{

					emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
				}
				else
				{
					if (Global::sendDataCCycleBuffer->getBufSize() > 0)
					{
						emit sendBufferDataSig();
					}
				}

				qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
			}


			return 1;
		}
		else
		{
			bool emptyData = (mHK18D14DataIdentify).read(imageTemp, imageBuf, (IMAGE_BUFF_LENGTH - BMP_HEADER),
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
				img = QImage((const unsigned char*)srcMat.data, srcMat.cols, srcMat.rows,
					QImage::Format_RGB888);

			}

			imageWidth = srcMat.cols;
			imageHeight = srcMat.rows;
			byteArray.append((char *)img.bits(), srcMat.cols * srcMat.rows*nChannel);

			isHistoryVideo = true;


			int rtValue = (mHK18D14DataIdentify).identify();

			(mHK18D14DataIdentify).dataOutput.videoProgressPercent = progressPercent;


			if ((mHK18D14DataIdentify).haveDataFlag == false) //广告
			{
				//显示图片，但是不输出结果
				emit readyReadBmp((mHK18D14DataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

			}
			else if ((mHK18D14DataIdentify).haveDataFlag == true)
			{
				//获取算法数据
				DataOutput outputStruct = (mHK18D14DataIdentify).dataOutput;
				//数据处理，屏蔽无效数据为-1

				isDataOutputNew(outputStruct);

				//数据有改变才会发送信号
				if (outputStruct.changeStatus >= 0)
				{

					emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
				}
				else
				{
					if (Global::sendDataCCycleBuffer->getBufSize() > 0)
					{
						emit sendBufferDataSig();
					}
				}

				qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
			}
			 
			return 1;

		}
	}
		
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
			//为了与raceID保持同步，必须在获取raceID后才能写入buffer
			// isSessionRaceIdRequested 在场次号发生变化时候被 置false
			// 在获取到的时候，被置为true 
			if (Global::isSessionRaceIdRequested)
			{
				writeOutputDataIntoBuffer(outputStruct);

			}
			else
			{
				qDebug("not requested ");
			}
			
			emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);


		}
		else //如果没有改变，那么此时可以发送缓存未发送的数据
		{
			if (Global::sendDataCCycleBuffer->getBufSize() > 2  )
			{

				emit sendBufferDataSig();
			}
		}
		qDebug() << "【BllDataIdentify】一帧图像识别时间：" << endl;
	}

	return 1;
}
/*
将要发送的数据写入环形buffer

*/

void BllDataIdentify::writeOutputDataIntoBuffer(DataOutput &dataOutput)
{
	 
	QByteArray sendBlock;
	int dataType = 0;
	//提交实时WIN数据
	if (dataOutput.changeStatus == WIN_CHANGED | dataOutput.changeStatus == WIN_PLA_CHANGED 
		| dataOutput.changeStatus == WIN_QIN_QPL_CHANGED| dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED )
	{
		//写入WIN数据
		dataType = WINTYPE;

		for (int i = 1; i <= dataOutput.horseNum; i++)
		{
			//封装一个WIN
			TagWPDataInfo WPData;
			WPData.HorseID = i;
			WPData.HorseNO = i;

			WPData.WinValue = dataOutput.WIN[i - 1];
		 
			WPData.RaceID = Global::requestRaceId;

			WPData.AtTime = Global::countRaceTime;

			int m = sizeof(TagWPDataInfo);
			sendBlock.append((char*)&WPData, sizeof(TagWPDataInfo));

			
		}

		//写入缓存
		if (Global::sendDataCCycleBuffer->getFreeSize() > sendBlock.size())
		{
			Global::sendDataCCycleBuffer->write((char*)&dataType, sizeof(dataType));
			int sendBlockSize = sendBlock.size();
			Global::sendDataCCycleBuffer->write((char*)&sendBlockSize, sizeof(sendBlockSize));
			Global::sendDataCCycleBuffer->write(sendBlock.data(), sendBlock.size());
		}
		//数组清空
		sendBlock.clear();

	}
	if (dataOutput.changeStatus == PLA_CHANGED | dataOutput.changeStatus == WIN_PLA_CHANGED
		| dataOutput.changeStatus == PLA_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED)
	{
		 //写入pla 数据
		dataType = PLATYPE;
 

		for (int i = 1; i <= dataOutput.horseNum; i++)
		{
			//封装一个WIN
			TagWPDataInfo WPData;
			WPData.HorseID = i;
			WPData.HorseNO = i;

			WPData.WinValue = dataOutput.PLA[i - 1];

			WPData.RaceID = Global::requestRaceId;

			WPData.AtTime = Global::countRaceTime;

			sendBlock.append((char*)&WPData, sizeof(TagWPDataInfo));
		}


		//写入缓存
		if (Global::sendDataCCycleBuffer->getFreeSize() > sendBlock.size())
		{
			Global::sendDataCCycleBuffer->write((char*)&dataType, sizeof(dataType));
			int sendBlockSize = sendBlock.size();
			Global::sendDataCCycleBuffer->write((char*)&sendBlockSize, sizeof(sendBlockSize));
			Global::sendDataCCycleBuffer->write(sendBlock.data(), sendBlock.size());
		}
		//数组清空
		sendBlock.clear();

	}
	if (dataOutput.changeStatus == QIN_QPL_CHANGED  | dataOutput.changeStatus == PLA_QIN_QPL_CHANGED
		| dataOutput.changeStatus == WIN_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED )
	{
	
		 //写入QIN QPL 数据
		if (dataOutput.isQPL)
		{
			dataType = QPLTYPE;
		}
		else
		{
			dataType = QINTYPE;
		}
		  

		//每一列

		for (int i = 1; i <= dataOutput.horseNum; i++)
		{
			for (int j = 1; j < i; j++)
			{
				//封装一个WIN
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = Global::requestRaceId;//所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (j <= 7) // 正表
				{
					 QDataInfo.QinValue = dataOutput.QPL_QIN[j - 1][i];
				}
				else //补充图表
				{
					QDataInfo.QinValue = dataOutput.QPL_QIN[i - 8][j - 8];
				}


				//发送
				sendBlock.append((char*)&QDataInfo, sizeof(TagQDataInfo));

			}

			for (int j = i + 1; j <= dataOutput.horseNum; /* HORSENUMBER_1; */ j++)
			{

				//封装一个WIN
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = Global::requestRaceId;//所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (i <= 7) // 正表
				{ 
					QDataInfo.QinValue = dataOutput.QPL_QIN[i - 1][j];
				}
				else //补充图表
				{
					QDataInfo.QinValue = dataOutput.QPL_QIN[j - 8][i - 8];
				}

				//发送
				sendBlock.append((char*)&QDataInfo, sizeof(TagQDataInfo));
			}


		}

		//写入缓存
		if (Global::sendDataCCycleBuffer->getFreeSize() > sendBlock.size())
		{
			Global::sendDataCCycleBuffer->write((char*)&dataType, sizeof(dataType));
			int sendBlockSize = sendBlock.size();
			Global::sendDataCCycleBuffer->write((char*)&sendBlockSize, sizeof(sendBlockSize));
			Global::sendDataCCycleBuffer->write(sendBlock.data(), sendBlock.size());
		}

		//数组清空
		sendBlock.clear();


	}
 

 

}
/*
开始实时直播 识别 
*/
int  BllDataIdentify::startLiveDataIdentify(int videoType)
{
	Global::pauseDataIdentifyTag = false;//开始跑
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

		while (Global::pauseDataIdentifyTag)
		{
			Sleep(1);
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
		 
		videoFileName = fileName;
		
		startHistoryDataIdentify(fileName, videoType);
	}
	else
	{ // LIVE 
		startLiveDataIdentify(videoType );
	}
	

}


/*
获取马名
Race Date 	20060101
Start one Session 	 02
Toal horse number 12
*/
void BllDataIdentify::getHorseNameFromDataFile(QString fileName,DataOutput &outputStruct)
{

	//清空list

	horseNameList.clear();
	horseIdList.clear();

	int labelPos = 0 ;
	
	QString fileAppend = ".wmv";
	labelPos = videoFileName.indexOf(fileAppend);

	//获取 日期
	if (labelPos > 0)
	{
		int  startPos = 0;

		for (int i = labelPos; i > 0; i--)
		{
			QString oneChar;
			oneChar = videoFileName.mid(i,1);

			if (oneChar == QString("/"))
			{
				startPos = i;
				break;
			}
		}
		Global::historyVideoDate = fileName.mid(startPos+1, 8);
		/*
		Global::historyVideoDate = fileName.mid(startPos + 1, 4);
		Global::historyVideoDate.append(QString("-"));
		Global::historyVideoDate += fileName.mid(startPos + 5, 2);
		Global::historyVideoDate.append(QString("-"));
		Global::historyVideoDate += fileName.mid(startPos + 7, 2);

		*/
	}
	else
	{
		labelPos = fileName.indexOf("01N");

		Global::historyVideoDate = fileName.mid(labelPos + 3, 8);
		/*
		Global::historyVideoDate = fileName.mid(labelPos + 3, 4);
		Global::historyVideoDate.append(QString("-"));
		Global::historyVideoDate += fileName.mid(labelPos + 7, 2);
		Global::historyVideoDate.append(QString("-"));
		Global::historyVideoDate += fileName.mid(labelPos + 9, 2);
		*/
	}
	//如果直播，那么日期值小于 4 
	if (Global::historyVideoDate.size() < 4 )
	{
		return;
	}

	QString searchLabel;
	searchLabel = Global::historyVideoDate + QString("Start one Session \t ");
	if (Global::session < 10)
	{
		searchLabel += QString("0") + QString::number(Global::session);
	}
	else
		searchLabel += QString::number(Global::session);

	int oneSessionStrPos;
	oneSessionStrPos = horseNameHistoryStr.indexOf(searchLabel);


	if (oneSessionStrPos >  1 )
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("信息")), QString(tr("当前识别历史文件日期为")) 
			+ Global::historyVideoDate,SystemLog::INFO_TYPE);
	}
	else
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("日期错误"))
			+ Global::historyVideoDate, SystemLog::INFO_TYPE);
	}

	QString oneSessionStr;
	oneSessionStr = horseNameHistoryStr.mid(oneSessionStrPos, 140);

	int horseNum = 0;
	QString horseNumsStr = oneSessionStr.mid(48, 1);
	horseNum = horseNumsStr.toInt();

	QString horseNamePartStr;
	horseNamePartStr = oneSessionStr.mid(49, 96);

	QString oneHorseName;
	QString cha;

	int horseCount = 0;
	for (int i = 0; i < horseNamePartStr.size(); i++)
	{

		cha = horseNamePartStr.mid(i, 1);
		if (cha > "z")
		{
			oneHorseName += cha;
		}


		if ((cha >= QString("1") & cha <= QString("9")) | cha == QString("E"))
		{
			if (oneHorseName.size() >= 2)
			{
				if (oneHorseName.size() != 2 & oneHorseName.size() != 3
					& oneHorseName.size() != 4 )
				{
					//写入系统日志
					Global::systemLog->append(QString(tr("ERROR")), QString(tr("马名 超出有效值范围." ))+ oneHorseName,
						SystemLog::ERROR_TYPE);

				}

				horseNameList.append(oneHorseName);

				wchar_t * encodedName;
				oneHorseName.toWCharArray(outputStruct.mHorseInfo.horseName[horseCount]);

				int pos = horseNameIdStr.indexOf(oneHorseName);

				QString idStr;

				idStr = horseNameIdStr.mid(pos - 7, 7);
				int horseId;
				QString oneNum;
				for (int index = 0; index < idStr.size(); index++)
				{
					QString bitOne;
					bitOne = idStr.mid(index, 1);
					if (bitOne >= QString("0") &
						bitOne <= QString("9"))
					{
						oneNum += idStr.mid(index, 1);

					}

					if (idStr.mid(index, 1) == QString("\t"))
					{
						
						horseId = oneNum.toInt();

						if (horseId > 10558 | horseId < 0)
						{
						//写入系统日志
							Global::systemLog->append(QString(tr("ERROR")), tr("Horse Id 超出有效值范围."), SystemLog::ERROR_TYPE);
							 
						}
						 
						outputStruct.mHorseInfo.horseID[horseCount] = horseId;
						//	horseIdList.append(horseId);
						break;
					}
				}
				oneHorseName = QString("");
 
				horseCount++;

			}

		}


	}



}



void BllDataIdentify::writeDataFile(DataOutput &dataOutput)
{
	 
 
	QString logStr;

	//QTextStream logContentOut(&logFile);
	/*
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
	
 */
	writeHistoryData( dataOutput);
}


/*
	写入历史数据文件
*/

void BllDataIdentify::writeHistoryData(DataOutput &dataOutput)
{
 
 //

	QString logStr;

	QTextStream logContentOut(&logFile);
	//
	int dataType = 0  ;
	QString raceIdStr;
	if (Global::session < 10)
	{
		raceIdStr = Global::historyVideoDate +QString("0")+ QString::number(Global::session);
	}
	else
	{
		raceIdStr = Global::historyVideoDate + QString::number(Global::session);
	}
	
	if (Global::historyVideoDate.size() < 6 )
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("ERROR")), QString(tr("writeHistoryData func : historyVideoDate Wrong.")
			+Global::historyVideoDate),	SystemLog::ERROR_TYPE);
	}

	Global::raceId = raceIdStr.toInt();

	//写入win
	if (dataOutput.changeStatus == WIN_CHANGED | dataOutput.changeStatus == WIN_PLA_CHANGED 
		| dataOutput.changeStatus == WIN_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED)
	{

		for (int i = 0; i < dataOutput.horseNum; i++ )
		{
			dataType = WINTYPE;
		 
			//封装一个WIN
			TagWPDataInfo WPData;

			WPData.HorseID = dataOutput.mHorseInfo.horseID[i];
			// from 1 to 14 .
			WPData.HorseNO = i +1 ;
 
		 	WPData.WinValue = dataOutput.WIN[i];
				 
			WPData.RaceID = Global::raceId;
			//顺计时
			WPData.AtTime = Global::countRaceTime;
			

			if (dataType == 0)
			{
				//写入系统日志
				Global::systemLog->append(QString(tr("ERROR")), tr("writeHistoryData func : dataType = 0." ),
					SystemLog::ERROR_TYPE);
			}
			//写文件
		//	Global::historyIdentifyDataWS << dataType << WPData.RaceID << WPData.HorseID << WPData.HorseNO
		//					<<WPData.WinValue<< WPData.AtTime;

			logContentOut << QString("Type") << QString("WINTYPE") << QString::number(dataType) 
				<< QString("RaceID") << QString::number(WPData.RaceID) << QString("HorseID") <<
				QString::number(WPData.HorseID) << QString("HorseNo") << QString::number(WPData.HorseNO) 
				<< QString("WINValue") << QString::number(WPData.WinValue) << QString("AtTime") 
				<< QString::number(WPData.AtTime);
		}
		 

	}
	// 写入pla
	if (dataOutput.changeStatus == WIN_PLA_CHANGED | dataOutput.changeStatus == PLA_CHANGED
		| dataOutput.changeStatus == PLA_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED
		)
	{
		for (int i = 0; i < dataOutput.horseNum; i++)
		{
			dataType = PLATYPE;
			//封装一个PLA
			TagWPDataInfo WPData;

			WPData.HorseID = dataOutput.mHorseInfo.horseID[i];

			if (WPData.HorseID > 10558  | WPData.HorseID < 0 )
			{
				//写入系统日志
				Global::systemLog->append(QString(tr("ERROR")), tr("writeHistoryData func : HorseID超出范围."),
					SystemLog::ERROR_TYPE);
			}
			//从1 到14 
			WPData.HorseNO = i+ 1; 

			WPData.WinValue = dataOutput.PLA[i];

			WPData.RaceID = Global::raceId ;
			WPData.AtTime = Global::countRaceTime;

			if (dataType == 0)
			{
			
				//写入系统日志
				Global::systemLog->append(QString(tr("ERROR")), tr("writeHistoryData func : dataType = 0 ."),
					SystemLog::ERROR_TYPE);
			}
			//写文件
		//	Global::historyIdentifyDataWS << dataType<< WPData.RaceID << WPData.HorseID 
		//					<< WPData.HorseNO <<WPData.WinValue<< WPData.AtTime;
			/*
			logContentOut << QString("Type") << QString::number(dataType) << QString::number(WPData.RaceID) <<
				QString::number(WPData.HorseID) << QString::number(WPData.HorseNO) << QString::number(WPData.WinValue)
				<< QString::number(WPData.AtTime);

				*/
			logContentOut << QString("Type") << QString("PLAType") << QString::number(dataType)
				<< QString("RaceID") << QString::number(WPData.RaceID) << QString("HorseID") <<
				QString::number(WPData.HorseID) << QString("HorseNo") << QString::number(WPData.HorseNO)
				<< QString("WINValue") << QString::number(WPData.WinValue) << QString("AtTime")
				<< QString::number(WPData.AtTime);


		}

	}
	// 写入qin qpl
	if (dataOutput.changeStatus == QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_QIN_QPL_CHANGED
		| dataOutput.changeStatus == PLA_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED

		 )
	{
		if (dataOutput.isQPL)
		{
			dataType = QPLTYPE;
		}
		else
			dataType = QINTYPE;

		for (int i = 1; i <= dataOutput.horseNum; i++)
		{
			
			for (int j = 1; j < i; j++)
			{
				 
				//封装一个 QIN QPL
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = Global::raceId;//所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (j <= 7) // 正表
				{					
					QDataInfo.QinValue = dataOutput.QPL_QIN[j - 1][i];
				}
				else //补充图表
				{
					QDataInfo.QinValue = dataOutput.QPL_QIN[i - 8][j - 8];
				}

				//写文件
			//	Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
			//		<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				/*
				logContentOut << QString("Type") << QString::number(dataType) << QString::number(QDataInfo.RaceID) <<
					QString::number(QDataInfo.HorseID) << QString::number(QDataInfo.HorseNO) <<
					QString::number(QDataInfo.YNO)	<< QString::number(QDataInfo.QinValue)
					<< QString::number(QDataInfo.AtTime);
				*/

				logContentOut << QString("Type") << QString("QINQPL") << QString::number(dataType)
					<< QString("RaceID") << QString::number(QDataInfo.RaceID) << QString("HorseID")  
					<<QString::number(QDataInfo.HorseID) << QString("HorseNo") << QString::number(QDataInfo.HorseNO)
					<< QString("YNO") << QString::number(QDataInfo.YNO)
					<< QString("Value") << QString::number(QDataInfo.QinValue) << QString("AtTime")
					<< QString::number(QDataInfo.AtTime);

			}

			for (int j = i + 1; j <= dataOutput.horseNum; /* HORSENUMBER_1; */ j++)
			{

				//封装一个WIN
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = Global::raceId;//所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (i <= 7) // 正表
				{
					QDataInfo.QinValue = dataOutput.QPL_QIN[i - 1][j];
				}
				else //补充图表
				{
					QDataInfo.QinValue = dataOutput.QPL_QIN[j - 8][i - 8];
				}

				//写文件
			//	Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
			//		<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				/*
				logContentOut << QString("Type") << QString::number(dataType) << QString::number(QDataInfo.RaceID) <<
					QString::number(QDataInfo.HorseID) << QString::number(QDataInfo.HorseNO) <<
					QString::number(QDataInfo.YNO)
					<< QString::number(QDataInfo.QinValue)
					<< QString::number(QDataInfo.AtTime);

				*/

				logContentOut << QString("Type") << QString("QINQPL") << QString::number(dataType)
					<< QString("RaceID") << QString::number(QDataInfo.RaceID) << QString("HorseID") <<
					QString::number(QDataInfo.HorseID) << QString("HorseNo") << QString::number(QDataInfo.HorseNO)
					<< QString("YNO")<<QString::number(QDataInfo.YNO)
					<< QString("Value") << QString::number(QDataInfo.QinValue) << QString("AtTime")
					<< QString::number(QDataInfo.AtTime);
			}


		}

		
	}
	
	 
}

/**
* @brief 停止识别
*/
void BllDataIdentify::stop()
{
	Global::pauseDataIdentifyTag = true;
}

/*
	初始化全局
*/
void BllDataIdentify::initGlobal()
{

	//历史视频起始帧位置
	Global::videoStartPos = 0;
	//识别视频类型

//	Global::videoType = 0;

	//历史视频文件，快进累加
	Global::frameAccValue = 1;

	/***********比赛数据***********/
	Global::raceId;//比赛唯一识别ID，服务端获得
	
	Global::session = 0;//比赛场次号

	Global::raceTime = 0;//比赛时间

	// 请求到的全局场次号id
	Global::requestRaceId = 0;

	//本场场次号的全局id 已经请求 标志位

	Global::isSessionRaceIdRequested = false;
	//比赛已经开始标志
	Global::raceHasStarted = 0;
	//比赛当前场次计时 
	Global::countRaceTime = 0;
	Global::historyFrameCount = 0;
	Global::timerCount = 0;

	// 是否用户校正了场次号
	Global::isSessioncalibrated = false;

 
}

