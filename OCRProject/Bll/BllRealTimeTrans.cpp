#include "BllRealTimeTrans.h"
#include "Include/HorseDataStruct.h"
#include "qmath.h"
#include "math.h"
BllRealTimeTrans::BllRealTimeTrans(QObject *parent)
	: QObject(parent)
{
	
	connectCount = 0;

	replyNoReceived = false;

	dataLength = 0;
	dataType = 0;


	isSessionChanged = false;
	priSession = 0;

	stopReadBuffData = false;

	//服务器网络通讯接口
	mcsNetClient = new Network("Server", Global::serverIpAddr, Global::serverPort);

	//正常接收到服务端回复
	//connect(Global::mcsNetClient, SIGNAL(clientReceiveDataSignal(QByteArray, int)), this, SLOT(handleReceiveData(QByteArray, int)));
	//客户端连接上服务端
	connect(mcsNetClient, SIGNAL(connectToHostSuccessSignal()), this, SLOT(handleConnect()));
	//客户端断开了服务端
	connect(mcsNetClient, SIGNAL(disConnectToHostSuccessSignal()), this, SLOT(handleDisConnect()));

	//用于标记 几号服务器链路 

	// 0 为 server 
	// 1 为server1 

	serverNo = 0; 


	serverNotConnected = false;

	serverSubmitFailed = false;


	mSendDataBuffer = new CCycleBuffer();
	
	curServerState = false;

	priServerState = false;

	memset(&mDataOutput,0,sizeof(DataOutput)) ;

	//20s
 


}



void BllRealTimeTrans::timerEvent(QTimerEvent *event)
{
	int session;
//	requestRaceIDForKeepAlive(Global::session);


}

BllRealTimeTrans::~BllRealTimeTrans()
{
	//delete[] data;
	if (mcsNetClient != 0)
	{
		delete mcsNetClient;
		mcsNetClient = 0;
	}


	//重新获取目录
	QString runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);
	//退到上一层目录
	QDir::setCurrent("../");

	QDir::setCurrent("../");


	QDir::setCurrent(".//OCRProject//liveBackupData//");


	//将缓存区数据写入文件

	// 写数据文件
	QFile dataBuffFile;
	QString fileName;
	fileName = QString("RDBSDataBuffFile_") + QString::number(serverNo) + QString(".txt");
	dataBuffFile.setFileName(fileName);
	if (!dataBuffFile.open(QIODevice::WriteOnly))
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), fileName + QString(tr("文件打开失败"))
		 , SystemLog::INFO_TYPE);

	QDataStream dataBufferDs;

	dataBufferDs.setDevice(&dataBuffFile);

	while (Global::sendDataCCycleBuffer->getUsedSize() >= sizeof(DataOutput ))
	{
		Global::sendDataCCycleBuffer->read((char*)&mDataOutput, sizeof(DataOutput));
		
		QByteArray ba;
		ba.append((char *)&mDataOutput, sizeof(DataOutput));
		dataBufferDs << ba;
	}

	dataBuffFile.close();

	//退回到exe路径
	runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);
}
/**
* @brief 连接服务器
*/
void BllRealTimeTrans::connectToHost(QString serverIp,qint32 serverPort)
{
	client_cmd_status = ClientCmdStatus::connect_status;
	mcsNetClient->connectToHost(serverIp, serverPort);

}
/**
* @brief 断开服务器
*/
void BllRealTimeTrans::disconnectToHost()
{
	client_cmd_status = ClientCmdStatus::disconnect_status;
	mcsNetClient->disConnectToHost();
}
/**
* @brief 识别端登录请求指令
*/
void BllRealTimeTrans::clientLogin()
{

	TagProtocolMsg msg;
	msg.MsgID = 4;
	msg.nDataSize = 0;
	strcpy(msg.Param, "Username\n Password");
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));

	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::login_status;
	mcsNetClient->sendData(byteArray,false);//发送数据，且不需要关闭socket
	emit statuChanged(QString("服务器 %1，识别端：正常，尝试登录中...").arg(serverNo));
	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleLogin(result, descriptor);//处理login返回数据
	}
	else
		emit statuChanged(QString("服务器 % 1，识别端：错误，登陆失败。").arg(serverNo));
}
/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleLogin(QByteArray result, int descriptor)
{
	/**************/
	/*1. 识别端登录请求指令
	例：第1步发送获取赛程表的指令： PROTOCOLMSG msg;
	msg.MsgID = 4;
	msg.nDataSize = 0;
	msg.Param = "Username\n Password";
	msg.Check = "RDBS";
	第2步收到服务端回应信息：OK(验证通过)	NO(验证失败)
	第3步根据返回结果继续下一步操作*/
	/**************/

	QString reply;
	reply = result.data();
	emit statuChanged(QString("服务器 %2，识别端：正常，服务端回复登陆，%1").arg(reply).arg(serverNo));
}
/**
* @brief 识别端请求马信息表指令
*/
void BllRealTimeTrans::requestHorse()
{
	
	TagProtocolMsg msg;
	msg.MsgID = 5;
	msg.nDataSize = 0;
	strcpy(msg.Param, "");
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));
	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::request_horse_status;
	mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket



	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleRequestHorse(result, descriptor);//处理login返回数据
	}
	else
		emit statuChanged(QString("服务器 % 1 识别端：错误, 请求马信息表指令失败。").arg(serverNo));
}
/**
* @brief 识别端处理服务端-请求马信息表指令
*/
void BllRealTimeTrans::handleRequestHorse(QByteArray result, int descriptor)
{
	/**************/
	/*2. 识别端请求马信息表指令
		例：第1步发送获取马信息表的指令： PROTOCOLMSG msg;
	msg.MsgID = 5;
	msg.nDataSize = 0;
	msg.Param = "";
	msg.Check = "RDBS";
	第2步收到服务端返回的数据大小：int(数据大小)
	第3步开始接收赛程表信息：N个HORSEINFO
	第4步验证信息完整性，如不完整跳至第1步继续*/
	/**************/
	/*
	
	//返回大小
	int horseNum = 0 ;
	qint32 size = horseNum * sizeof(TagHorseInfo);//初始化默认模拟数据

#if moni_test == 0
	//真正采集
	memcpy(&size, result.data(), sizeof(qint32));
	for (int i = 0; i < size / sizeof(TagHorseInfo); i++ //真正解析
	{
	TagHorseInfo horse;
	memcpy(&horse, result.data(), sizeof(TagHorseInfo));
	Global::horseInfoList.append(horse);
	}
#else
	//测试代码
	for (int i = 0; i < size / sizeof(TagHorseInfo); i++)//模拟马匹
	{
		TagHorseInfo horse;
		horse.HorseID = i;
		strcpy(horse.HorseName, QString("马%1").arg(i).toLocal8Bit().data());
		Global::horseInfoList.append(horse);
	}
#endif

	
	

	emit statuChanged(QString("服务端：回复，马信息，%1匹马").arg(size / sizeof(TagHorseInfo)));

	*/
}


/**
* @brief 识别端根据RaceNO(场次号)请求RaceID指令
*/
void BllRealTimeTrans::requestRaceID( int session )
{
#ifndef HISTORY_SUBMIT
	if (Global::isHistoryVideo)
	{
		return;
	}
#endif
	TagProtocolMsg msg;
	msg.MsgID = 6;
	msg.nDataSize = session ;
	strcpy(msg.Param, "");
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));
	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::request_race_id_status;
	mcsNetClient->sendData(byteArray, false);		//发送数据，且不需要关闭socket


	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		serverSubmitFailed = false;
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleRequestRaceID(result, descriptor);//处理login返回数据
		//将 场次号请求raceid 标志位 清空
		//Global::isSessionRaceIdRequested = true;
	}
	else
	{
		
		emit statuChanged(QString("服务器 %1,识别端：错误，请求RaceID指令失败。").arg(serverNo));
 
		serverSubmitFailed = false ; 
 
	}
		

	
}



/**
* @brief 为了保持连接使用请求服务器raceID，保存不显示
*/
void BllRealTimeTrans::requestRaceIDForKeepAlive(int session)
{
#ifndef HISTORY_SUBMIT
	if (Global::isHistoryVideo)
	{
		return;
	}
#endif
	TagProtocolMsg msg;
	msg.MsgID = 6;
	msg.nDataSize = session;
	strcpy(msg.Param, "");
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));
	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::request_race_id_status;
	mcsNetClient->sendData(byteArray, false);		//发送数据，且不需要关闭socket


	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		 
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据
		 
	}
	else
	{

		emit statuChanged(QString("服务器 %1,识别端：为保持连接请求RaceID失败。").arg(serverNo));
  
	}

	//检测是否是最后一场

//	if (Global::countRaceTime > 40 )
	{
	//	submitRaceEnd();
	}

}


/**
* @brief 识别端处理服务端-请求RaceID指令
*/
void BllRealTimeTrans::handleRequestRaceID(QByteArray result, int descriptor)
{
	/**************/
	/*3.识别端根据RaceNO(场次号)请求RaceID指令
		例：第1步发送请求RaceID指令：  PROTOCOLMSG msg;
		msg.MsgID = 6;
		msg.nDataSize = RaceNO;
		msg.Param = "";
		msg.Check = "RDBS";
		第2步接收数据： int(RaceID)*/
	/**************/

	//raceId
	qint32 raceId;
	memcpy(&raceId, result.data(), sizeof(qint32));
	Global::requestRaceId = raceId;
	Global::raceId = raceId;

	//标记已经获取了
	Global::isThisSessionRaceIDRequested[Global::session] = true;
	Global::requestedRaceID[Global::session] = raceId;

	emit statuChanged(QString("服务器 %2 ,服务端：回复，raceId，%1").arg(raceId).arg(serverNo));

	//写入系统日志
	Global::systemLog->append(QString(tr("信息")), QString("服务器") + QString::number(serverNo)
		+ QString(tr("服务端：回复，raceID")) +QString::number(raceId), SystemLog::INFO_TYPE);


}
/**
* @brief 识别端提交比赛时长指令
*/
void BllRealTimeTrans::submitRaceTime(qint32 raceTime)
{
#ifndef HISTORY_SUBMIT
	if (Global::isHistoryVideo)
	{
		return;
	}

#endif
	//如果服务器未连接 直接退出
	if (serverNotConnected)
	{
		return;
	}

	TagProtocolMsg msg;
	msg.MsgID = 7;
	msg.nDataSize = 0;
	QString raceTimeStr;

	raceTimeStr = QString::number(raceTime);


	QByteArray ba = raceTimeStr.toLatin1();


	strcpy(msg.Param, ba.data());
	//strcpy(msg.Param, "35");//比赛分钟数
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));
	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::submit_race_time_status;
	mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket



	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleSubmitRaceTime(result, descriptor);//处理login返回数据

		serverSubmitFailed = false  ;
		//写入系统日志
		Global::systemLog->append(QString(tr("信息")),QString("服务器")+QString::number(serverNo)
			+ QString(tr("服务器０：　服务端：回复，提交比赛时长"))
			+ raceTimeStr, SystemLog::INFO_TYPE);

		emit statuChanged(QString("服务器０：识别端：提交比赛时长： %1 指令 。").arg(raceTimeStr) +
			QString("倒计时：") + QString::number(Global::raceTime) + QString("顺计时：")
			+ QString::number(Global::countRaceTime)) ;


	}
	else
	{
		emit statuChanged("服务器０：识别端：错误，提交比赛时长指令失败。");
		serverSubmitFailed = true ;
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString("服务器") + QString::number(serverNo) +
			QString(tr("服务器０：识别端：错误，提交比赛时长指令失败"))
			+ raceTimeStr, SystemLog::INFO_TYPE);

	}
}
		
/**
* @brief 识别端处理服务端-提交比赛时长指令
*/
void BllRealTimeTrans::handleSubmitRaceTime(QByteArray result, int descriptor)
{
	/**************/
	/*4.识别端提交比赛时长指令
		例：第1步发送提交比赛时长指令： PROTOCOLMSG msg;
	msg.MsgID = 7;
	msg.nDataSize = 0;
	msg.Param = "比赛分钟数";
	msg.Check = "RDBS";
	第2步接收回应： OK*/
	/**************/

	//回复OK
	QString reply = result.data();


	 
	if (reply == "OK")
	{
		Global::isThisTotalSessionTimeSumbit[Global::session] = true;
		serverSubmitFailed = false ;
		emit statuChanged(QString("服务器 %2,服务端：回复，接收到比赛时长指令，%1").arg(reply).arg(serverNo));

	}
	else
	{
		
		emit statuChanged(QString("服务器 %2,服务端：未接收到回复，未，%1").arg(reply).arg(serverNo)) ;
		serverSubmitFailed = true ;
	}
}

/**
* @brief 识别端发送提交实时数据指令
*/
void BllRealTimeTrans::submitRealData(DataOutput outputStruct, QByteArray array, 
										int imageWidth, int imageHeight)
{
#ifndef HISTORY_SUBMIT
	//如果是历史视频那么，则不提交
	if (Global::isHistoryVideo)
	{

		return;

	}
#endif
	priServerState = curServerState;
	//没有连接，此时请求连接 ,提交失败标志真，未连接状态为真 
	if (mcsNetClient->clientIsValid() != ConnectingState )
	{
		//标记连接状态
		curServerState = false;
	 
		serverNotConnected = true;		
		connectCount++;
		if (connectCount == 5 )
		{
			connectToHost(Global::serverIpAddr, Global::serverPort);
			connectCount = 0 ;
		}
		
	} // 连接 但是 submit failed  
	else 	 
	{
		curServerState = true;
		serverNotConnected = false ;
		serverSubmitFailed = false;
	}
	 
	//读取buffer
	if (serverNo == 0 )
	{
		mSendDataBuffer = Global::sendDataCCycleBuffer;
	}
	else if (serverNo == 1 )
	{
		mSendDataBuffer = Global::sendDataCCycleBuffer1 ;
	}
	int usedSize = mSendDataBuffer->getUsedSize();
	
	//如果里面有未发送的数据并且上次的数据发送完毕，那么读出进行发送
	//
	if (usedSize >= sizeof(DataOutput )  )
	{
		//必须连接才可以读取数据 stopReadBuffData ，用于标志是否 获取了相应的raceID
		//必须上次的数据发送完全才可以，否则继续发送
		if (!stopReadBuffData & !serverNotConnected & !serverSubmitFailed 
			 ) 
		{
			//所有的数据都发送
			if (mDataOutput.changeStatus == 0 )
			{
				//读入结构体
				mSendDataBuffer->read((char *)&mDataOutput, sizeof(DataOutput));
			}
		
			
		} //获取到场次号后发送
		else 
		{	
			//如果此时获取到了新场次号了，那么继续读取数据
			if (stopReadBuffData == true)
			{
				if (Global::isThisSessionRaceIDRequested[mDataOutput.session])
				{
					stopReadBuffData = false;
				}
				//如果服务器处于连接模式 ，那么请求raceId
				if (!serverNotConnected)
				{
					requestRaceID(mDataOutput.session);
				}
			}
			//直接返回
			return;
		}
		if (mDataOutput.session != priSession )
		{
			isSessionChanged = true;
		}
		else
		{
			isSessionChanged = false;
		}
		//如果新的场次号出来了，但是没有请求到场次号，那么此时，就应该停止读取数据了
		if (isSessionChanged & !Global::isThisSessionRaceIDRequested[mDataOutput.session])
		{ 
			stopReadBuffData = true ;
		}
		
		//如果此时获取到了新场次号了，那么继续读取数据
		if (stopReadBuffData == true)
		{
			if (Global::isThisSessionRaceIDRequested[mDataOutput.session])
			{
				stopReadBuffData = false;
			}
			//如果服务器处于连接模式 ，那么请求raceId
			if (!serverNotConnected)
			{
				requestRaceID(mDataOutput.session);
			}
		}

		priSession = mDataOutput.session;

		//必须提交成功才能进入 此次获取了RaceId才行 并且服务器一定要处于连接状态
		//并且处于连接状态
		if (!serverNotConnected &
			Global::isThisSessionRaceIDRequested[mDataOutput.session] & !serverNotConnected)
		{
			//检查是否有漏发的总时长
			if (Global::isThisTotalSessionTimeSumbit[mDataOutput.session] == false & Global::totalSessionTime[mDataOutput.session] > 10 )
			{
				submitRaceTime(Global::totalSessionTime[mDataOutput.session]);
			}
			//提交实时WIN数据
			if (mDataOutput.changeStatus == WIN_CHANGED)
			{
				if (submitWINOrPLA(mDataOutput, "WIN") == EXEC_SUCCESS )
				{
					mDataOutput.changeStatus -= WIN_CHANGED;
				}
			
			}
			else if (mDataOutput.changeStatus == PLA_CHANGED)
			{
				if (submitWINOrPLA(mDataOutput, "PLA") == EXEC_SUCCESS )
				{
					mDataOutput.changeStatus -= PLA_CHANGED;

				}
					
			}
			else if (mDataOutput.changeStatus == WIN_PLA_CHANGED)
			{
				if (submitWINOrPLA(mDataOutput, "WIN") == EXEC_SUCCESS)
				{
					mDataOutput.changeStatus -= WIN_CHANGED;
				}

				if (submitWINOrPLA(mDataOutput, "PLA") == EXEC_SUCCESS)
				{
					mDataOutput.changeStatus -= PLA_CHANGED;

				}
			}
			else if (mDataOutput.changeStatus == QIN_QPL_CHANGED)
			{
				if (mDataOutput.isQPL)
				{
					if (submitQINOrQPL(mDataOutput, "QPL") == EXEC_SUCCESS )
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED ;
					}
					
				}
				else
				{
					if (submitQINOrQPL(mDataOutput, "QIN") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}
					
				}
			}
			else if (mDataOutput.changeStatus == WIN_QIN_QPL_CHANGED)
			{
				
				if (submitWINOrPLA(mDataOutput, "WIN") == EXEC_SUCCESS )
				{
					mDataOutput.changeStatus -= WIN_CHANGED ;

				}
				

				if (mDataOutput.isQPL)
				{
					if (submitQINOrQPL(mDataOutput, "QPL") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}

				}
				else
				{
					if (submitQINOrQPL(mDataOutput, "QIN") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}

				}
			}
			else if (mDataOutput.changeStatus == PLA_QIN_QPL_CHANGED)
			{
				if (submitWINOrPLA(mDataOutput, "PLA")== EXEC_SUCCESS)
				{
					mDataOutput.changeStatus -= PLA_CHANGED ;

				}
				if (mDataOutput.isQPL)
				{
					if (submitQINOrQPL(mDataOutput, "QPL") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}

				}
				else
				{
					if (submitQINOrQPL(mDataOutput, "QIN") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}

				}
			}
			else if (mDataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED)
			{
				if (submitWINOrPLA(mDataOutput, "WIN") == EXEC_SUCCESS)
				{
					mDataOutput.changeStatus -= WIN_CHANGED;

				}
				if (submitWINOrPLA(mDataOutput, "PLA") == EXEC_SUCCESS)
				{
					mDataOutput.changeStatus -= PLA_CHANGED;

				}
				if (Global::tenSecondNotifyDataWriteen1 == true)
				{
					emit statuChanged(QString("客户端：发送倒计时10s提醒数据。服务器 %2").arg(serverNo));

					//两组数据都发送
					if (submitQINOrQPL(mDataOutput, "QPL") == EXEC_SUCCESS)
					{
						mDataOutput.changeStatus -= QIN_QPL_CHANGED;
					}

					submitQINOrQPL(mDataOutput, "QIN");

					Global::tenSecondNotifyNeeded = false;

					Global::tenSecondNotifyDataWriteen1 = false ;

				}
				else
				{
					if (mDataOutput.isQPL)
					{
						if (submitQINOrQPL(mDataOutput, "QPL") == EXEC_SUCCESS)
						{
							mDataOutput.changeStatus -= QIN_QPL_CHANGED;
						}

					}
					else
					{
						if (submitQINOrQPL(mDataOutput, "QIN") == EXEC_SUCCESS)
						{
							mDataOutput.changeStatus -= QIN_QPL_CHANGED;
						}

					}
				}

			}

		}


	}
	else  //数据为空了，那么这个时候，要与服务器保持连接 ，请求raceID，但不保存
	{

		requestRaceIDForKeepAlive(Global::session);
	}
	 
}



/**
* @brief 提交实时数据-WIN或PLA
* @param ouputStruct：算法传递来的数据，type:传送类型默认win
*/
int BllRealTimeTrans::submitWINOrPLA(DataOutput& ouputStruct, QString type)
{
	TagProtocolMsg msg;
	msg.MsgID = 8;
	msg.nDataSize = ouputStruct.horseNum *sizeof(TagWPDataInfo);// N个WPDATAINFO或QDATAINFO结构数据，按马编号
	int dataType = 0;
	if (type == "WIN")
	{
		dataType = WINTYPE;
		strcpy(msg.Param, "WIN");//"WIN(或PLA或QIN或QPL)";
	}
	else if (type == "PLA")
	{
		dataType = PLATYPE;
		strcpy(msg.Param, "PLA");//"WIN(或PLA或QIN或QPL)";
	}
	strcpy(msg.Check, "RDBS");


	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::submit_real_data_status;
	mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket


	QByteArray sendBlock;

	//稍微需要处理一下循环发送数据//
	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		serverSubmitFailed = false;
		QByteArray result;
		int descriptor;//引用
		mcsNetClient->readAllMessage(result, descriptor);//读取数据

		QString reply = result.data();
		emit statuChanged(QString("服务器 %2 服务端：回复，接收到实时数据指令，%1").arg(reply).arg(serverNo));
		if (reply == "OK")//提交WIN
		{
			//*************还得发送数据：WIN(或PLA或QIN或QPL)*********//

			for (int i = 1; i <= ouputStruct.horseNum; /* HORSENUMBER_1; */i++)
			{
				//封装一个WIN
				TagWPDataInfo WPData;
				WPData.HorseID = ouputStruct.mHorseInfo.horseID[i-1] ;
				WPData.HorseNO = i;

				if (type == "WIN")
				{
					WPData.WinValue = ouputStruct.WIN[i - 1].dataValue;
				}
				else if (type == "PLA")
				{
					WPData.WinValue = ouputStruct.PLA[i - 1].dataValue;

				}
				WPData.RaceID = Global::requestedRaceID[mDataOutput.session];
				/*
				if (Global::tenSecondNotifyNeeded == true )
				{
					WPData.AtTime = 99 ;
					if (i == ouputStruct.horseNum) 
					{	
						Global::tenSecondNotifyNeeded = false ;

					}
				}
				else
				{
					WPData.AtTime = ouputStruct.countRaceTime ;
				}
				*/
				WPData.AtTime = ouputStruct.countRaceTime;

				sendBlock.append((char*)&WPData, sizeof(TagWPDataInfo));
			}
			//****************************************************//
			mcsNetClient->sendData(sendBlock, false);//发送数据，且不需要关闭socket
			emit statuChanged(QString("服务器 %2  识别端：发送实时数据-%1").arg(type).arg(serverNo));
			
			serverSubmitFailed = false ;

			
			// 若对方有回复 
			bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
			if (success)
			{
				QByteArray result;
				mcsNetClient->readAllMessage(result, descriptor);//读取数据

				QString reply = result.data();
				if (reply == "OK")// 
				{
					emit statuChanged(QString("服务器 %3：服务端：回复 OK，接收到正确数据-%1，%2 .").arg(type).arg(reply).arg(serverNo));
					return EXEC_SUCCESS;
				}
				else
				{
					emit statuChanged(QString("服务器 %3：服务端：回复 NO，未接收到正常数据-%1，%2 .").arg(type).arg(reply).arg(serverNo));
					return EXEC_FAILED;
				}
				

			}
			else
			{
				emit statuChanged(QString("服务器 %1 服务端：未回复。 未接受到正常数据").arg(serverNo) );
				return EXEC_FAILED;
			}

		
			//**若对方有回复****//
		}

	}
	else
	{
		//将数据写入文件
	
		emit statuChanged(QString("服务器 %1 ，识别端：发送提交数据指令失败.").arg(serverNo));

		serverNotConnected = true;
		serverSubmitFailed = true ;
		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString("服务器 %1").arg(serverNo)
			+QString(tr("识别端：错误，提交实时数据指令失败"))
			+ type, SystemLog::INFO_TYPE);
		 
		return EXEC_FAILED;
	}

}

/**
* @brief 提交实时数据-QIN或QPL
* @param ouputStruct：算法传递来的数据，type:传送类型默认win
*/
int BllRealTimeTrans::submitQINOrQPL(DataOutput &ouputStruct, QString type)
{
	TagProtocolMsg msg;
	msg.MsgID = 8;
	msg.nDataSize = (ouputStruct.horseNum - 1)*ouputStruct.horseNum  * sizeof(TagQDataInfo);// N个WPDATAINFO或QDATAINFO结构数据，按马编号
	if (type == "QIN")
		strcpy(msg.Param, "QIN");//"WIN(或PLA或QIN或QPL)";
	else if (type == "QPL")
		strcpy(msg.Param, "QPL");//"WIN(或PLA或QIN或QPL)";
	strcpy(msg.Check, "RDBS");


	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));

	client_cmd_status = ClientCmdStatus::submit_real_data_status;
	mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket


	//**稍微需要处理一下循环发送数据****//
	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{
		
		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据

		QString reply = result.data();
		emit statuChanged(QString("服务器 %3 服务端：接收到发送数据指令-%1,%2").arg(type).arg(reply).arg(serverNo));
		if (reply == "OK")//提交QIN
		{
			//*************还得发送数据：WIN(或PLA或QIN或QPL)*********//
			QByteArray sendBlock;
			//每一列

			for (int i = 1; i <= ouputStruct.horseNum; i++)
			{
				for (int j = 1; j < i; j++)
				{
					//封装一个WIN
					TagQDataInfo QDataInfo;
					QDataInfo.RaceID = Global::requestedRaceID[mDataOutput.session];//所属赛事ID
					QDataInfo.HorseID = ouputStruct.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
					QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
					QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
					QDataInfo.AtTime = mDataOutput.countRaceTime ;
					if (j <= 7) // 正表
					{
						if (type == "QIN")//数据值，由XNO与YNO组合得出 QPL+QIN[7][15]
							QDataInfo.QinValue = ouputStruct.QIN[j - 1][i].dataValue;
						else if (type == "QPL")
							QDataInfo.QinValue = ouputStruct.QPL[j - 1][i].dataValue;
					}
					else //补充图表
					{
						if (type == "QIN")//数据值，由XNO与YNO组合得出 QPL+QIN[7][15]
							QDataInfo.QinValue = ouputStruct.QIN[i - 8][j - 8].dataValue;
						else if (type == "QPL")
							QDataInfo.QinValue = ouputStruct.QPL[i - 8][j - 8].dataValue;
					}


					//发送
					sendBlock.append((char*)&QDataInfo, sizeof(TagQDataInfo));

				}

				for (int j = i + 1; j <= ouputStruct.horseNum; /* HORSENUMBER_1; */ j++)
				{

					//封装一个WIN
					TagQDataInfo QDataInfo;
					QDataInfo.RaceID = Global::requestedRaceID[ouputStruct.session];//所属赛事ID
					QDataInfo.HorseID = ouputStruct.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
					QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
					QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
					QDataInfo.AtTime = ouputStruct.countRaceTime;
					if (i <= 7) // 正表
					{
						if (type == "QIN")//数据值，由XNO与YNO组合得出 QPL+QIN[7][15]
							QDataInfo.QinValue = ouputStruct.QIN[i - 1][j].dataValue;
						else if (type == "QPL")
							QDataInfo.QinValue = ouputStruct.QPL[i - 1][j].dataValue;
					}
					else //补充图表
					{
						if (type == "QIN")//数据值，由XNO与YNO组合得出 QPL+QIN[7][15]
							QDataInfo.QinValue = ouputStruct.QIN[j - 8][i - 8].dataValue;
						else if (type == "QPL")
							QDataInfo.QinValue = ouputStruct.QPL[j - 8][i - 8].dataValue;
					}


					//发送
					sendBlock.append((char*)&QDataInfo, sizeof(TagQDataInfo));
				}


			}


			//****************************************************//
			mcsNetClient->sendData(sendBlock, false);//发送数据，且不需要关闭socket
			emit statuChanged(QString("服务器 %2 识别端：发送数据-%1，14个.").arg(type).arg(serverNo));

			serverSubmitFailed = false ;

			
			// 若对方有回复 
			bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
			if (success)
			{
				QByteArray result;
				mcsNetClient->readAllMessage(result, descriptor);//读取数据

				QString reply = result.data();
				if (reply == "OK")// 
				{
					emit statuChanged(QString("服务器 %3：服务端：回复 OK，接收到了数据-%1，%2 .").arg(type).arg(reply).arg(serverNo));
					return EXEC_SUCCESS;
				}
				else
				{
					emit statuChanged(QString("服务器 %3：服务端：回复 NO，未正确接收到数据-%1，%2 .").arg(type).arg(reply).arg(serverNo));
					return EXEC_FAILED;
				}


			}
			else
			{
				emit statuChanged(QString("服务器 %1 服务端：未回复，未接收到数据.").arg(serverNo));
				return EXEC_FAILED;
			}

		
			//**若对方有回复****//
		}

	}
	else
	{
		//将数据写入文件
		serverNotConnected = true;
		serverSubmitFailed = true ;

		emit statuChanged(QString("服务器 %2 识别端：错误，提交实时数据指令-%1,失败.").arg(type).arg(serverNo));


		//写入系统日志
		Global::systemLog->append(QString(tr("错误")), QString("服务器 %1").arg(serverNo) 
			+QString(tr("识别端：错误，提交实时数据指令失败"))
			+ type + Global::requestedRaceID[ouputStruct.session], SystemLog::INFO_TYPE);

		return EXEC_FAILED ;


	}

}



/*
*  最后一场结束了，发送比赛结束指令
*/

void BllRealTimeTrans::submitRaceEnd()
{
	TagProtocolMsg msg;
	memset(&msg, 0, 64);
	strcpy(msg.Check, "RDBS");
	msg.MsgID = 18; // 结束指令
 
	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));


	client_cmd_status = ClientCmdStatus::submit_real_data_status;
	mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket


	//**稍微需要处理一下循环发送数据****//
	bool success = mcsNetClient->waitForReadyRead(30000);//阻塞等待
	if (success)
	{

		QByteArray result;
		int descriptor;
		mcsNetClient->readAllMessage(result, descriptor);//读取数据

		QString reply = result.data();
		emit statuChanged(QString("服务器 %2 服务端：回复，提交比赛结束指令-%1！").arg(reply).arg(serverNo));
		if (reply == "OK")//提交QIN
		{
			emit statuChanged(QString("服务器 %1 服务端：回复，提交比赛结束指令成功！").arg(serverNo));

		}
		else
		{
			emit statuChanged(QString("服务器 %1 服务端：回复，提交比赛结束指令失败！").arg(serverNo));

		}
	}
}

/**
* @brief 识别端处理服务端-提交实时数据指令
*/
void BllRealTimeTrans::handleSubmitRealData(QByteArray result, int descriptor)
{
	/**************/
	/*5. 识别端发送提交实时数据指令
		例：第1步发送提交数据指令：  	 PROTOCOLMSG msg;
	msg.MsgID = 8;
	msg.nDataSize = N;
	msg.Param = "WIN(或PLA或QIN或QPL)";
	msg.Check = "RDBS";
	第2步接收回应：OK
		第3步开始发送数据： N(马数)个WPDATAINFO或QDATAINFO结构数据，按马编号						顺序排列
		第4步不断循环识别到新数据后再提交*/
	/**************/

	//回复OK
	QString reply = result.data();
	emit statuChanged(QString("服务器 %2 服务端：回复，实时数据指令，%1").arg(reply).arg(serverNo));

	//*************还得发送数据：WIN(或PLA或QIN或QPL)*********//
	/*tagWPDataInfo或者tagQDataInfo*/
	//****************************************************//
	mcsNetClient->sendData(result, false);//发送数据，且不需要关闭socket
}
/*
@brief 发送缓存区数据,缓存区数据为发送失败缓存的数据
*/
void BllRealTimeTrans::sendBufferDataToServer()
{
	 
}

/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleReceiveData(QByteArray result, int descriptor)
{
	switch (client_cmd_status)
	{
	case no_status:
		break;
	case connect_status:
		break;
	case login_status:
		handleLogin(result, descriptor);
		break;
	case request_horse_status:
		handleRequestHorse(result, descriptor);
		break;
	case request_race_id_status:
		handleRequestRaceID(result, descriptor);
		break;
	case submit_race_time_status:
		handleSubmitRaceTime(result, descriptor);
		break;
	case submit_real_data_status:
		handleSubmitRealData(result, descriptor);
		break;
	default:
		break;
	}
}	


/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleConnect()
{
	emit statuChanged(QString("服务器 %1 识别端：正常，客户端连接上服务端，.").arg(serverNo) ) ;

	//已经连接上
	serverSubmitFailed = false;
}
/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleDisConnect()
{
	emit statuChanged(QString("服务器 %1 识别端:正常，客户端已断开连接。").arg(serverNo) ) ;

	//网络中断了 则重连服务器
	if (serverSubmitFailed == true)
	{
 		//connectToHost(Global::serverIpAddr, Global::serverPort);

	}

}

