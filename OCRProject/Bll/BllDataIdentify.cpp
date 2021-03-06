﻿#include "BllDataIdentify.h"
#include <QDebug>
#include "Include/HorseDataStruct.h"
#include <QDir>

 
BllDataIdentify::BllDataIdentify(QObject *parent)
	: QObject(parent)
{
	 
	 
 
 

	pauseDataIdentifyTag = false;//初始化

	for (int i = 0; i < 30; i++)
		memset( myRaceTimeStruct, 0, sizeof(raceNumTimeStruct)*30);
	for (int i = 0; i < 20;i++)
		memset( myRaceNumberStruct, 0, sizeof(raceNumTimeStruct)*20);
	

	memset((void * )&priDataOutput, 0, sizeof(DataOutput));

	memset((void *)&lastQINQPLDATA, 0, sizeof(DataOutput));

 
 
	tenSencond3DataSend = false ;
	tenSencondLastQINorQPLDataSend = false ;


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

		lastQINQPLDATA.mHorseInfo.horseID[i] = 0;

		lastQINQPLDATA.mHorseInfo.isSCR[i] = false;
	}

	isRaceSessionDetected = false;

	isRightRaceTimeCountDownDetected = false;

	frameAccValue = 0;

	Global::frameAccValue = 1;


	//打开文件

	//重新获取目录
	//QString runPath = QCoreApplication::applicationDirPath();

	//QDir::setCurrent(runPath);

	horseNameIdDataFile.setFileName(".//dat//horseNameIdDataFile.txt");

	if (!horseNameIdDataFile.open(QIODevice::ReadOnly | QIODevice::Text))

	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("horseNameIdDataFile 文件打开错误"))
			+ Global::historyVideoDate, SystemLog::INFO_TYPE);
	}

	horseNameIdData.setDevice(&horseNameIdDataFile);

	while (!horseNameIdData.atEnd())
	{
		horseNameIdStr += horseNameIdData.readLine();

	}

	 
	//horseNameIdStr = horseNameIdStr.mid(20000, horseNameIdStr.size() - 20000);
	horseNameIdDataFile.close();

	// 历史马匹 名字记录
	horseNameHistoryDataFile.setFileName(".//dat//horseNameHistoryDataFile.txt");
	if (!horseNameHistoryDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("horseNameHistoryDataFile 文件打开错误"))
			+ Global::historyVideoDate, SystemLog::INFO_TYPE);
	}

	horseNameHistoryData.setDevice(&horseNameHistoryDataFile);

	while (!horseNameHistoryData.atEnd())
	{
		horseNameHistoryStr += horseNameHistoryData.readLine();

	}

	horseNameIdDataFile.close();


	// 打开实时直播的文件

	QDateTime liveDate = QDateTime::currentDateTime();//获取系统现在的时间
	QString liveDateStr  = liveDate.toString("yyyyMMdd"); //设置显示格式



	QString liveHorseNameFileName;
	liveHorseNameFileName = liveDateStr + QString("horseName.txt") ;

	liveHorseNameFileName = QString(".//dat//") + liveHorseNameFileName;
	liveHorseNameFile.setFileName(liveHorseNameFileName);

	
	if (!liveHorseNameFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("liveHorseNameFile 文件打开错误"))
			+ Global::historyVideoDate, SystemLog::INFO_TYPE);
	}

	liveHorseNameTs.setDevice(&liveHorseNameFile);

	while (!liveHorseNameTs.atEnd())
	{
		liveHorseNameStr += liveHorseNameTs.readLine();

	}

 

	raceCountDownTime = 0;

	QINQPLTransformed = false;

	// 未设置bmp count
	bmpCountIsSet = false;


	//将未识别出来的正确的horseID的马名 保存到文档里面用于纠错

	notFindHorseIDFile.setFileName("notFindHorseIDFile.txt");

	if (!notFindHorseIDFile.open(QIODevice::Append|QIODevice::Text))
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("notFindHorseIDFile 文件打开错误"))
		, SystemLog::INFO_TYPE);
	}

	notFindHorseIdTextStream.setDevice(&notFindHorseIDFile);

	 

	horseIDDataFile.setFileName(".//dat//horseIDDATA.dat");
	//读写，并且追加形式
	if (!horseIDDataFile.open(QIODevice::ReadWrite))
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("horseIDDataFile 文件打开错误"))
			, SystemLog::INFO_TYPE);
	}
	horseIDDataDsIn.setDevice(&horseIDDataFile);


	HorseIDStruct oneHorseIDData;
	 
	while (!horseIDDataFile.atEnd())
	{
		horseIDDataDsIn >> oneHorseIDData.horseID >> oneHorseIDData.horseName;
		mHorseIDData.append(oneHorseIDData);
		mHorseIDNameData.append(oneHorseIDData.horseName);
	}
	horseIDDataFile.close();
	
	//重新打开一次 用于写
	horseIDDataFile.setFileName(".//dat//horseIDDATA.dat");
	if (!horseIDDataFile.open(QIODevice::Append))
	{
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("horseIDDataFile 文件打开错误"))
			, SystemLog::INFO_TYPE);
	}
	horseIDDataDsOut.setDevice(&horseIDDataFile);

	

}

BllDataIdentify::~BllDataIdentify()
{

	notFindHorseIDFile.close();
}
/**
* @brief 初始化算法
*/
void BllDataIdentify::init()
{
 
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
		//如果场次号发生变化 那么清空结构体
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
			 Global::oneSessionTotalRaceTime = 0;

			 raceCountDownTime = 0;
		 	
		 }
		 
		//获取马名 由于outputStruct 每次都会清空，所以需要每次都要获取
		 if (Global::isHistoryVideo)
		 {
			 getHorseNameFromDataFileHistory(videoFileName, outputStruct);
		 }
		 else
		 {
			 getHorseNameFromDataFileLive(videoFileName, outputStruct);

		 }

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
		if (Global::isHistoryVideo)
		{
			getHorseNameFromDataFileHistory(videoFileName, outputStruct);
		}
		else
		{
			getHorseNameFromDataFileLive(videoFileName, outputStruct);

		}

		// 标志位清零
		Global::isSessioncalibrated = false;
		
		isRightRaceTimeCountDownDetected = false;
	 
		raceTimeCountDownNear9 = false;

		Global::isCountTimeCalied = false ;

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

		//qDebug("DataNewCount = %d  raceTime = %d ",dataNewCount,outputStruct.raceTime );
	}
	else
	{
		//最大内容出现计数次数 
		int maxContentCount = 0 ;
		int maxContent = 0 ;
		 

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
		 
		//
		//第一次一定要检测到倒计时 发生了递减1 才能认为识别正确了倒计时
		//并且 	Global::isCountTimeCalied = false;
		if (maxContentCount >= 8 & maxContent > 10 & isRightRaceTimeCountDownDetected == false
			& Global::isCountTimeCalied == false )
		{
			//第一次一定要检测到倒计时 发生了递减1 才能认为识别正确了倒计时
			if (Global::raceTime - maxContent == 1)
			{
				isRightRaceTimeCountDownDetected = true ;
				Global::raceTime = maxContent ;
				//总时长
				emit submitRaceTimeSig(Global::raceTime + Global::countRaceTime);
				// 保存计时
				Global::totalSessionTime[Global::session] = Global::raceTime + Global::countRaceTime;
			}
			raceCountDownTime = maxContent ;

			Global::raceTime = maxContent;
		}
		
		 
		 
			//检测到 比赛时间 到10min 了，这时 
		if (maxContent == 10 )
		{
			raceTimeCountDownNear9 = true;
		}
		
		//清空
		memset(myRaceNumberStruct, 0, sizeof(raceNumTimeStruct));
		memset(myRaceTimeStruct, 0, sizeof(raceNumTimeStruct));



	}
	
  

	sessionChangedDly1 = sessionChanged;

	 
	return EXEC_SUCCESS ;

}

/**
* @brief 判断数据是否为新数据
*/
LONG BllDataIdentify::winPlaDivTen(DataOutput &outputStruct)
{
	//无数据区域设置为0 
	for (int i = outputStruct.horseNum; i < 14;i ++ )
	{
		outputStruct.WIN[i].dataValue = 0;
		outputStruct.PLA[i].dataValue = 0;
	}
	for (int i = outputStruct.horseNum - 7; i < 7;i++)
	{
		for (int j = 0; j < i; j++)
		{
			outputStruct.QPL[i][j].dataValue = 0;
			outputStruct.QIN[i][j].dataValue = 0;


		}

	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = outputStruct.horseNum + 1; j < 15; j++)
		{
			outputStruct.QPL[i][j].dataValue = 0;
			outputStruct.QIN[i][j].dataValue = 0;
		}
			 
	}

	//设置 退赛马匹


	
	// 所有的win pla 设置为 ÷ 10
	int dataBigger10 = 0;
	float dataSmaller10 = 0;
	float dataTemp = 0;
	for (int i = 0; i < 14; i++)
	{
		dataTemp = outputStruct.WIN[i].dataValue / 10;

		if (dataTemp >= 10)
		{
			outputStruct.WIN[i].dataValue = (int)dataTemp;
		}
		else
		{
			dataBigger10 = (int)outputStruct.WIN[i].dataValue;
			outputStruct.WIN[i].dataValue = (float)(dataBigger10 /10.0 );
		}

		dataTemp = outputStruct.PLA[i].dataValue / 10;

		if (dataTemp >= 10)
		{
			outputStruct.PLA[i].dataValue = (int)dataTemp;
		}
		else
		{
			dataBigger10 = (int)outputStruct.PLA[i].dataValue;
			outputStruct.PLA[i].dataValue = (float)(dataBigger10 / 10.0);
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
	

	bool dataOutOfRangePLA = false ;
	bool dataOutOfRangeWIN = false;
	bool dataOutOfRangeQINQPL = false;
 

	int  dataOutOfRangeCount = 0 ;
	int  WINChangedNum = 0;
	int  PLAChangedNum = 0;
	int  QINQPLCHangedNum = 0;


	//

	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

	// 判断WIN数据是否变化
	outputStruct.changeStatus = 0 ;
	

	//当QIN QPL发生切换时候 ，等待3个定时器周期，在发送新的数据，保持数据稳定。
	//等待5幅图片的时间	
	//如果刚开始的数据为零 那么，认为是刚开始
	if (priDataOutput.isQPL != outputStruct.isQPL & QINQPLTransformed == false 
		| abs(priDataOutput.WIN[0].dataValue - 0)*10 < 0.2)
	{
		//if (sessionChanged == false)
		{
			QINQPLTransformedCountDown = 3 ;
			QINQPLTransformed = true ;
		}
	 
	}
	 
	//如果场次号发生了变化，那么等待3张图片，数据稳定开始发送
	if (Global::isSessionChanged)
	{
		sessionChangedCountDown = 3 ;
		sessionChanged = true;

	}

		if (sessionChangedCountDown == 0 )
		{
			for (int i = 0; i < outputStruct.horseNum; i++)
			{
				if (outputStruct.mHorseInfo.isSCR[i] != priDataOutput.mHorseInfo.isSCR[i])
				{
					WINChangedNum++ ;
				}
				if (abs(outputStruct.WIN[i].dataValue - priDataOutput.WIN[i].dataValue) > 0.05)
				{
			 
					WINChangedNum++;

				}
				if (abs((outputStruct.WIN[i].dataValue - priDataOutput.WIN[i].dataValue)) 
					/ priDataOutput.WIN[i].dataValue > 0.6 )
				{
					dataOutOfRangeWIN = true;
					dataOutOfRangeCount++;

					Global::systemLog->append(QString(tr("WIN 数据 出现超出范围。")), 
						QString(tr("")), SystemLog::INFO_TYPE);
				 
				}

			}


			for (int i = 0; i < outputStruct.horseNum; i++)
			{
				if (outputStruct.mHorseInfo.isSCR[i] != priDataOutput.mHorseInfo.isSCR[i])
				{
					PLAChangedNum++;
				}
				if (abs(outputStruct.PLA[i].dataValue - priDataOutput.PLA[i].dataValue) >  0.05)
				{
					//qDebug("PLA:i=%d ,new is %f pri is %f ", i,  outputStruct.PLA[i], priDataOutput.PLA[i]);
					//outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
					PLAChangedNum++;
				}
				if (abs((outputStruct.PLA[i].dataValue - priDataOutput.PLA[i].dataValue)) 
					/ priDataOutput.PLA[i].dataValue > 0.6
				
					)
				{
					dataOutOfRangePLA = true;
					dataOutOfRangeCount++;

					Global::systemLog->append(QString(tr("PLA 数据 出现超出范围。")),
						QString(tr("")), SystemLog::INFO_TYPE);

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


				if (outputStruct.mHorseInfo.isSCR[i] != priDataOutput.mHorseInfo.isSCR[i])
				{
					QINQPLCHangedNum ++;
				}
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
				if (outputStruct.isQPL)
				{
					if (abs(outputStruct.QPL[i][j].dataValue - priDataOutput.QPL[i][j].dataValue) > 0.05)
					{
						//qDebug("QIN_QPL:i=%d ,j=%d new is %f pri is %f ", i, j, outputStruct.QPL_QIN[i][j], priDataOutput.QPL_QIN[i][j]);
						//outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
						QINQPLCHangedNum++;
					}
					if (priDataOutput.isQPL == outputStruct.isQPL)
					{
						if (outputStruct.QPL[i][j].dataValue == 0.0)
						{
							continue;
						}
						if (abs((outputStruct.QPL[i][j].dataValue - priDataOutput.QPL[i][j].dataValue)) 
							/ priDataOutput.QPL[i][j].dataValue > 0.6

							)
						{

							dataOutOfRangeQINQPL = true;
#ifdef QDEBUG_OUTPUT
							qDebug("i=%d,j=%d,cur = %f , pri =%f", i, j, outputStruct.QPL[i][j].dataValue, 
								priDataOutput.QPL[i][j].dataValue);
#endif
							dataOutOfRangeCount++;

							Global::systemLog->append(QString(tr("QPL 数据 出现超出范围。")),
								QString(tr("")), SystemLog::INFO_TYPE);

						}

					}
				}
				else  // QIN 
				{
					if (abs(outputStruct.QIN[i][j].dataValue 
						- priDataOutput.QIN[i][j].dataValue) > 0.05)
					{
						//qDebug("QIN_QPL:i=%d ,j=%d new is %f pri is %f ", i, j, outputStruct.QPL_QIN[i][j], priDataOutput.QPL_QIN[i][j]);
						//outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
						QINQPLCHangedNum++;
					}
					if (priDataOutput.isQPL == outputStruct.isQPL)
					{
						if (outputStruct.QIN[i][j].dataValue == 0.0)
						{
							continue;
						}
						if (abs((outputStruct.QIN[i][j].dataValue - priDataOutput.QIN[i][j].dataValue)) 
							/ priDataOutput.QIN[i][j].dataValue > 0.6

							)
						{

							dataOutOfRangeQINQPL = true;
#ifdef QDEBUG_OUTPUT
							qDebug("i=%d,j=%d,cur = %f , pri =%f", i, j,
								outputStruct.QIN[i][j].dataValue, priDataOutput.QIN[i][j].dataValue);
#endif
							dataOutOfRangeCount++;

							Global::systemLog->append(QString(tr("QIN 数据 出现超出范围。")),
								QString(tr("")), SystemLog::INFO_TYPE);
						}

					}
				}
				

			}
		}
	}
	

	//数据超出本应有的范围，那么就不会发送此时的数据， 此时应该避免掉 比赛刚开始，有数据的时候，此时应该发送数据。
	//添加 Global::raceHasStarted 来控制 第一次，即 比赛未开始，那么可以发送的。
	// 如果场次号发生了变化
	if (Global::raceHasStarted == 0 | QINQPLTransformed | dataOutOfRangeWIN == false |
		dataOutOfRangePLA == false | dataOutOfRangeQINQPL == false |
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
		

		if (WINChangedNum > 0 & dataOutOfRangeWIN == false )
		{
		//	if (sessionChangedCountDown == 0)
 				outputStruct.changeStatus = WIN_CHANGED;
		}
		if (PLAChangedNum > 0 & dataOutOfRangePLA == false )
		{
		//	if (sessionChangedCountDown == 0)
				outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
		}
		if (QINQPLCHangedNum > 0 & dataOutOfRangeQINQPL == false)
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
	/* 
	//保留上一组qin 或者qpl 变化前的数据。
	if (outputStruct.isQPL != priDataOutput.isQPL ) 
	{
		lastQINQPLDATA = priDataOutput;

	}
	*/
	//如果提交了倒计时，那么此时显示 10s 并发送一组数据，不管是否 数据发生变化

	

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

	//赋值顺计时
	outputStruct.countRaceTime = Global::countRaceTime;
	

	if (Global::isSessionChanged)
		Global::tenSecondNotifyNeeded = false;

	//如果倒计时不是小于1 那么 忽略 提交 倒计时10s 请求
	if ( (Global::totalSessionTime[outputStruct.session] - Global::countRaceTime) >= 1)
	{
		Global::tenSecondNotifyNeeded = false;
	}
	 

	if (Global::tenSecondNotifyNeeded == true & Global::tenSecondNotifyDataWriteen1 == false 
		& Global::tenSecondNotifyDataWriteen2 == false )
	{
		outputStruct.countRaceTime = 99 ;
		
		outputStruct.changeStatus = QIN_QPL_CHANGED | WIN_CHANGED | PLA_CHANGED  ;
		Global::tenSecondNotifyDataWriteen1 = true ;
		Global::tenSecondNotifyDataWriteen2 = true ;

	}



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

		
		//请求新场次号
		emit requestRaceIdSig(Global::session);

	}

		
	return 1;
}

/**
* @brief 将数据写入文件
*/
int BllDataIdentify::saveToLocalFile(char *data, QString path)
{
 
#ifdef WRITE_IMAGES_BEFORE_DataIdentify
		
	 
		// 设置目录
		QString runPath = QCoreApplication::applicationDirPath();
		QDir myDir ;
		//QDir::setCurrent(runPath);
		//退到上一层目录
		//QDir::setCurrent("../");

		//QDir::setCurrent("../");


		QDateTime liveDate = QDateTime::currentDateTime();//获取系统现在的时间
		QString liveDateStr = liveDate.toString("yyyyMMdd"); //设置显示格式

		QString newPath ;
		newPath = runPath + QString("//") + liveDateStr;
		
		if (myDir.exists(newPath))
		{
			QDir::setCurrent(newPath);
		}
		else 
		{
			myDir.mkdir(newPath);
			QDir::setCurrent(newPath);
		}



		if (!bmpCountIsSet)
		{
			QDir dir;
			path = dir.currentPath();
			path.append("/");
			QDir dirNew(path);

			QFileInfoList fileInfoList = dirNew.entryInfoList();

			QFileInfo fileInfo = fileInfoList.at(fileInfoList.count() - 1);

			QString fileNameOri = fileInfo.fileName();

			int fileNum;
			int max = 0;
			for (int i = 0; i < fileInfoList.count(); i++)
			{

				QFileInfo fileInfo = fileInfoList.at(i);;
				QString fileName = fileInfo.fileName();

				if (fileName == "." | fileName == "..")
				{
					continue;
				}
				fileName = fileName.mid(0, fileName.size() - 4);
				fileNum = fileName.toInt();

				if (fileNum > max)
				{
					max = fileNum;
				}
			}
			bmpCount = max;
			bmpCountIsSet = true;

		}

		Mat mat_temp(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, data);

		bmpCount++;
		QPixmap pixmap;
		QImage myImage;
		myImage = QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);


		QString bmpFileName;
		bmpFileName = QString(".bmp");
		bmpFileName.prepend(QString::number(bmpCount));

		// 转换RGB888 到QIMAGE
		for (int h = 0; h < IMAGE_HEIGHT; h++) {
			// scanLine returns a ptr to the start of the data for that row 
			memcpy(myImage.scanLine(h), (data + IMAGE_WIDTH * 3 * h),
				IMAGE_WIDTH * 3);
		}

		/*
		QDir *temp = new QDir;
		bool exist = temp->exists(".//liveImageData1");
		if (exist)
			qDebug("exit");
		else
		{
			bool ok = temp->mkdir(".//liveImageData1");
			if (ok)
				qDebug("mkdir success ");
		}
		QFile bmpFile ;
		QString oriFileName = bmpFileName;
		bmpFile.setFileName(bmpFileName) ;
		for (int i = 0; bmpFile.exists(); i++)
		{
		bmpFileName = oriFileName;
		bmpFileName.prepend("_");
		bmpFileName.prepend(QString::number(i));
		bmpFile.setFileName(bmpFileName);
		if (!bmpFile.exists())
		{
		break;
		}

		}
		*/
	
		
	
		myImage.save(bmpFileName);
#ifndef OFFLINE_DEBUG

		myImage.save(bmpFileName);

#endif // !OFFLINE_DEBUG
#ifdef OFFLINE_DEBUG 
	 
		qDebug("读入第 %d 幅图像 \n", bmpCount);
#endif
		// 设置目录
		runPath = QCoreApplication::applicationDirPath();

		QDir::setCurrent(runPath);
 
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
		//写入系统日志
	 
		progressPercent = 100 * f / (totalFrames/ videoFps);
		QString fileName;
		fileName = QString("ReadFrame") +
			QString::number(f % 5)
			+ QString(".bmp");
		QByteArray ba;
		ba.append(fileName.toLatin1());
		imwrite(ba.data(), frameMat);
	 
		algorithmExecHistory(videoType, NULL, frameMat, progressPercent);


		Sleep(1200);
		 

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

		if ( emptyData == true )
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

		if ((mYZTDataIdentify).haveDataFlag == false | rtValue == EXIT_THIS_OCR ) //广告
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
			//qDebug() << "empty image data" << endl;

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


		if ((mHK18DataIdentify).haveDataFlag == false | rtValue == EXIT_THIS_OCR ) //广告
		{
			//显示图片，但是不输出结果
			emit readyReadBmp((mHK18DataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

		}
		else if ((mHK18DataIdentify).haveDataFlag == true | rtValue == EXIT_THIS_OCR )
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
				//qDebug() << "empty image data" << endl;

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


			if ((mHK63D14DataIdentify).haveDataFlag == false | rtValue == EXIT_THIS_OCR ) //广告
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
		// Hk18 D14 
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


		 

			if ((mHK18D14DataIdentify).haveDataFlag == false | rtValue == EXIT_THIS_OCR )  //广告
			{
				//显示图片，但是不输出结果
				emit readyReadBmp((mHK18D14DataIdentify).dataOutput, byteArray, imageWidth, imageHeight);

			}
			else if ((mHK18D14DataIdentify).haveDataFlag == true )
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
	
	return EXEC_SUCCESS;
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
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("读取图像为空"))
			, SystemLog::INFO_TYPE);
		 
	}

	int imageWidth;
	int imageHeight;

	QByteArray byteArray;

	bool isHistoryVideo;
	 
	imageWidth = IMAGE_WIDTH;
	imageHeight = IMAGE_HEIGHT;

	byteArray.append((char *)imageBuf, imageWidth * imageHeight * 3);
	 

	int rtValue = dataIdentifyClass.identify();
	//获取算法数据
	DataOutput outputStruct = dataIdentifyClass.dataOutput;

	if (rtValue == EXIT_THIS_OCR )
	{
		// 识别图片发生问题，当做广告图片处理
		outputStruct.haveDataFlag = false;
		emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
		return EXIT_THIS_OCR ;
	}
	if (dataIdentifyClass.haveDataFlag == false) //广告
	{
		//显示图片，但是不输出结果
		emit readyReadBmp(dataIdentifyClass.dataOutput, byteArray, imageWidth, imageHeight);

		if (Global::sendDataCCycleBuffer->getBufSize() >= sizeof(DataOutput))
		{
			emit readyRead(dataIdentifyClass.dataOutput, byteArray, imageWidth, imageHeight);
		}

		//广告删除 标记
		Global::tenSecondNotifyNeeded = false;

	}
	else if (dataIdentifyClass.haveDataFlag == true)
	{
	
		//数据处理，屏蔽无效数据为-1

		isDataOutputNew(outputStruct);

		//数据有改变才会发送信号
		if (outputStruct.changeStatus > 0)
		{
			//一直写入 ，在发送的时候会检查是否获取了raceID			 
			{
				writeOutputDataIntoBuffer( outputStruct );
 
			}
	 
			emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
 
		}
		else //如果没有改变，那么此时可以发送缓存未发送的数据
		{
			if (Global::sendDataCCycleBuffer->getBufSize() >= sizeof(DataOutput) 
				| Global::sendDataCCycleBuffer1->getBufSize() >= sizeof(DataOutput) )
			{ 
				emit readyRead(outputStruct, byteArray, imageWidth, imageHeight);
			}
 
		}
		 
	}

	return 1;
}
/*
将要发送的数据写入环形buffer

*/

void BllDataIdentify::writeOutputDataIntoBuffer(DataOutput &dataOutput)
{
	 
	//赋值 session 

	dataOutput.session = Global::session;

	//写入缓存
	if (Global::sendDataCCycleBuffer->getFreeSize() > sizeof(DataOutput))
	{
		int l = sizeof(DataOutput);
		Global::sendDataCCycleBuffer->write((char*)&dataOutput, sizeof(DataOutput));
		 
	}

	//写入缓存
	if (Global::sendDataCCycleBuffer1->getFreeSize() > sizeof(DataOutput))
	{
		int l = sizeof(DataOutput);
		Global::sendDataCCycleBuffer1->write((char*)&dataOutput, sizeof(DataOutput));

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
			saveToLocalFile((char*)data, path);

#ifndef ONLY_SAVE_IMAGES
			//算法
			algorithmExecLive(videoType, data, Mat());
#endif
		}

		while (Global::pauseDataIdentifyTag)
		{
			Sleep(1);
		}

 
	}

	delete[] data;
	
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

void BllDataIdentify::getHorseNameFromDataFileLive(QString fileName, DataOutput &outputStruct)
{

	//清空list

	horseNameList.clear();
	horseIdList.clear();
	


	QDateTime liveDate = QDateTime::currentDateTime();//获取系统现在的时间
	QString liveDateStr = liveDate.toString("yyyyMMdd"); //设置显示格式



	QString searchLabel;
	searchLabel = liveDateStr + QString("Start one Session \t ");

	if (Global::session < 10 )
	{
		searchLabel += QString::number(0) + QString::number(Global::session);
	}
	else 
		searchLabel += QString::number(Global::session);

	int oneSessionStrPos;
	oneSessionStrPos = liveHorseNameStr.indexOf(searchLabel);


	if (oneSessionStrPos < 1)
	{
		/*
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString(tr("日期错误"))
			+ liveDateStr, SystemLog::INFO_TYPE);
		*/
	}

	QString oneSessionStr;
	oneSessionStr = liveHorseNameStr.mid(oneSessionStrPos, 140);

	int horseNum = 0;
	QString horseNumsStr = oneSessionStr.mid(48, 1);
	horseNum = horseNumsStr.toInt();

	QString horseNamePartStr;
	horseNamePartStr = oneSessionStr.mid(49, 96);

	QString oneHorseName;
	QString cha;
	//读取直播场次里面的马名 
	int horseCount = 0;
	for (int i = 0; i < horseNamePartStr.size(); i++)
	{

		cha = horseNamePartStr.mid(i, 1);
		if (cha > "z")
		{
			oneHorseName += cha;
		}

		//已经读取一个 马名 
		if ((cha >= QString("1") & cha <= QString("9")) | cha == QString("E"))
		{
			if (oneHorseName.size() >= 2)
			{
				if (oneHorseName.size() != 2 & oneHorseName.size() != 3
					& oneHorseName.size() != 4)
				{
					//写入系统日志
					Global::systemLog->append(QString(tr("ERROR")), QString(tr("马名 超出有效值范围.")) + oneHorseName,
						SystemLog::ERROR_TYPE);

				}

				horseNameList.append(oneHorseName);
				QString oneHorseNameNew;
				oneHorseNameNew = oneHorseName + QString(",");
				wchar_t * encodedName;
				
				int pos = mHorseIDNameData.indexOf(oneHorseNameNew);

				//int pos1 = mHorseIDNameData.lastIndexOf(oneHorseName);

				 
				//r如果没找到 相应马匹那么 将新马匹编号写入数据文件
				if (pos == -1 )
				{
					 
					notFindHorseIdTextStream << QString("场次号:日期") << liveDateStr << QString("场次号:")
						<< QString::number(outputStruct.session) << QString("序列号：")
						<< QString::number(horseCount) << QString("马名：") << oneHorseName << QString("\n");

					notFindHorseIdTextStream.flush();

					oneHorseName.toWCharArray(outputStruct.mHorseInfo.horseName[horseCount]);

					//编号
					HorseIDStruct tempHorseDataStructOld;
					HorseIDStruct tempHorseDataStructNew;
					tempHorseDataStructOld = mHorseIDData.at(mHorseIDData.size() - 1);
					tempHorseDataStructNew.horseID = tempHorseDataStructOld.horseID + 1;
					tempHorseDataStructNew.horseName = oneHorseName+QString(",");
					//新编的插入
					mHorseIDData.append(tempHorseDataStructNew);
					//新编的写入文件
					horseIDDataDsOut << tempHorseDataStructNew.horseID << tempHorseDataStructNew.horseName ;
				 
					mHorseIDNameData.append(tempHorseDataStructNew.horseName);

					horseIDDataFile.flush();

					outputStruct.mHorseInfo.horseID[horseCount] = tempHorseDataStructNew.horseID ;
					horseCount++;

				}
				else
				{
					HorseIDStruct tempHorseDataStructOld;
					tempHorseDataStructOld = mHorseIDData.at(pos);
					outputStruct.mHorseInfo.horseID[horseCount] = tempHorseDataStructOld.horseID;
					oneHorseName.toWCharArray(outputStruct.mHorseInfo.horseName[horseCount]);
					horseCount++;

				}				
				oneHorseName = QString("");
				
			}

		}


	}
}
/*
获取马名
Race Date 	20060101
Start one Session 	 02
Toal horse number 12
*/
void BllDataIdentify::getHorseNameFromDataFileHistory(QString fileName,DataOutput &outputStruct)
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
		//日期发生了变化
		if (Global::historyVideoDate != fileName.mid(startPos + 1, 8))
		{
			Global::historyVideoDate = fileName.mid(startPos + 1, 8);
			//写入系统日志
			Global::systemLog->append(QString(tr("信息")), QString(tr("当前识别历史文件日期为"))
				+ Global::historyVideoDate, SystemLog::INFO_TYPE);
		}
		 
	}
	else
	{
		labelPos = fileName.indexOf("01N");
		//日期发生了变化
		if (Global::historyVideoDate != fileName.mid(labelPos + 3, 8))
		{
			Global::historyVideoDate = fileName.mid(labelPos + 3, 8);
			//写入系统日志
			Global::systemLog->append(QString(tr("信息")), QString(tr("当前识别历史文件日期为"))
				+ Global::historyVideoDate, SystemLog::INFO_TYPE);
		}
		
	 
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


	if (oneSessionStrPos <  1 ) 
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
	 
 
 
	writeHistoryData( dataOutput);
}


/*
	写入历史数据文件
*/

void BllDataIdentify::writeHistoryData(DataOutput &dataOutput)
{
 
 //
	/*
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
 
			WPData.WinValue = dataOutput.WIN[i].dataValue;
				 
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

			WPData.WinValue = dataOutput.PLA[i].dataValue;

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
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[j - 1][i].dataValue;

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[j - 1][i].dataValue;

					}
				}
				else //补充图表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[i - 8][j - 8].dataValue;

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[i - 8][j - 8].dataValue;

					}
					 
				}

				//写文件
			//	Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
			//		<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				 
 
			}

			for (int j = i + 1; j <= dataOutput.horseNum;   j++)
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
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[i - 1][j].dataValue;

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[i - 1][j].dataValue;
					}
				}
				else //补充图表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[j - 8][i - 8].dataValue;

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[j - 8][i - 8].dataValue;
					}
					 
				}

				//写文件
			//	Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
			//		<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				 
			}


		}

		
	}
	
	 */
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
	
	Global::session = 1;//比赛场次号

	Global::raceTime = 0;//比赛时间

	// 请求到的全局场次号id
	Global::requestRaceId = 0;

	//本场场次号的全局id 已经请求 标志位

	 
	//比赛已经开始标志
	Global::raceHasStarted = 0;
	//比赛当前场次计时 
	Global::countRaceTime = 0;
	Global::historyFrameCount = 0;
	Global::timerCount = 0;

	// 是否用户校正了场次号
	Global::isSessioncalibrated = false;

 

	// 场次号 是否请求到了
	for (int i = 0; i < 14; i++)
	{
		Global::isThisSessionRaceIDRequested[i] = false;

		Global::requestedRaceID[i] = 0;

		Global::totalSessionTime[i] = 0;

		Global::isThisTotalSessionTimeSumbit[i] = false;
	}


}

