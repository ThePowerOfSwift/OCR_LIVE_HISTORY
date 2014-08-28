#include "BllRealTimeTrans.h"
#include "Include/HorseDataStruct.h"
#include "qmath.h"
#include "math.h"
BllRealTimeTrans::BllRealTimeTrans(QObject *parent)
	: QObject(parent)
{
	//正常接收到服务端回复
	//connect(Global::mcsNetClient, SIGNAL(clientReceiveDataSignal(QByteArray, int)), this, SLOT(handleReceiveData(QByteArray, int)));
	//客户端连接上服务端
	connect(Global::mcsNetClient, SIGNAL(connectToHostSuccessSignal()), this, SLOT(handleConnect()));
	//客户端断开了服务端
	connect(Global::mcsNetClient, SIGNAL(disConnectToHostSuccessSignal()), this, SLOT(handleDisConnect()));


	connectCount = 0;

	replyNoReceived = false;

	dataLength = 0;
	dataType = 0;
}

BllRealTimeTrans::~BllRealTimeTrans()
{
	//delete[] data;
}
/**
* @brief 连接服务器
*/
void BllRealTimeTrans::connectToHost(QString serverIp,qint32 serverPort)
{
	Global::client_cmd_status = ClientCmdStatus::connect_status;
	Global::mcsNetClient->connectToHost(serverIp, serverPort);

}
/**
* @brief 断开服务器
*/
void BllRealTimeTrans::disconnectToHost()
{
	Global::client_cmd_status = ClientCmdStatus::disconnect_status;
	Global::mcsNetClient->disConnectToHost();
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

	Global::client_cmd_status = ClientCmdStatus::login_status;
	Global::mcsNetClient->sendData(byteArray,false);//发送数据，且不需要关闭socket
	emit statuChanged("识别端：正常，尝试登录中...");
	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleLogin(result, descriptor);//处理login返回数据
	}
	else
		emit statuChanged("识别端：错误，登陆失败。");
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
	emit statuChanged(QString("识别端：正常，服务端回复登陆，%1").arg(reply));
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

	Global::client_cmd_status = ClientCmdStatus::request_horse_status;
	Global::mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket



	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleRequestHorse(result, descriptor);//处理login返回数据
	}
	else
		emit statuChanged("识别端：错误,请求马信息表指令失败。");
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
}
/**
* @brief 识别端根据RaceNO(场次号)请求RaceID指令
*/
void BllRealTimeTrans::requestRaceID( )
{
#ifndef HISTORY_SUBMIT
	if (Global::isHistoryVideo)
	{
		return;
	}
#endif
	TagProtocolMsg msg;
	msg.MsgID = 6;
	msg.nDataSize = Global::session;
	strcpy(msg.Param, "");
	strcpy(msg.Check, "RDBS");

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));
	//测试结构体
	TagProtocolMsg msg1;
	memcpy(&msg1, byteArray.data(), sizeof(TagProtocolMsg));

	Global::client_cmd_status = ClientCmdStatus::request_race_id_status;
	Global::mcsNetClient->sendData(byteArray, false);		//发送数据，且不需要关闭socket


	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleRequestRaceID(result, descriptor);//处理login返回数据
		//将 场次号请求raceid 标志位 清空
		Global::isSessionRaceIdRequested = true;
	}
	else
	{
		//请求失败
		Global::serverSubmitFailed = true;
		emit statuChanged("识别端：错误，请求RaceID指令失败。");

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
	emit statuChanged(QString("服务端：回复，raceId，%1").arg(raceId));
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

	Global::client_cmd_status = ClientCmdStatus::submit_race_time_status;
	Global::mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket



	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	if (success)
	{
		QByteArray result;
		int descriptor;
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据
		handleSubmitRaceTime(result, descriptor);//处理login返回数据
	}
	else
		emit statuChanged("识别端：错误，提交比赛时长指令失败。");
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

	emit statuChanged(QString("服务端：回复，提交比赛时长指令，%1").arg(reply));

}

/**
* @brief 识别端发送提交实时数据指令
*/
void BllRealTimeTrans::submitRealData(DataOutput outputStruct, QByteArray array, int imageWidth, int imageHeight)
{
#ifndef HISTORY_SUBMIT
	//如果是历史视频那么，则不提交
	if (Global::isHistoryVideo)
	{

		return;

	}
#endif
	/*
	 //网络中断了 则重连服务器
	if (Global::serverSubmitFailed == true )
	{		 
		connectToHost(SERVER_IP_ADDRESS,SERVER_PORT);
	
	}
	*/
	//没有连接，此时请求连接
	if (Global::mcsNetClient->clientIsValid() != ConnectingState)
	{
		qDebug("test re connect ");
		
		connectCount++;
		if (connectCount == 30 )
		{
			connectToHost(SERVER_IP_ADDRESS, SERVER_PORT);
			connectCount++;
		}
		
	}
		
	if ( !Global::isSessionRaceIdRequested )
	{
		requestRaceID();
	}
	//如果提交失败 那么就不要传输了 直接返回 
	// serverSubmitFailed 会在网络重新连接上的时候 置为false 

	if (Global::serverSubmitFailed )
	{
		return;
	}
	//读取buffer
	int usedSize = Global::sendDataCCycleBuffer->getUsedSize();
	if (usedSize > 2)
	{
		 if (replyNoReceived == false)
		 {
			 Global::sendDataCCycleBuffer->read((char *)&dataType, sizeof(int));
			 Global::sendDataCCycleBuffer->read((char *)&dataLength, sizeof(int));
			 data = new char[dataLength];

			 Global::sendDataCCycleBuffer->read((char *)data, dataLength);

			 dataHasBeenRead = true;
		 }
		if (dataHasBeenRead )
		{
			//读出多组数据
			if (dataType == WINTYPE | dataType == PLATYPE)
			{

				submitWINOrPLA(data, dataLength, dataType);
			}
			else if (dataType == QINTYPE | dataType == QPLTYPE)
			{

				submitQINOrQPL(data, dataLength, dataType);

			}
			else
			{
				qDebug("BllRealTimeTrans: wrong dataType ");
			}
		}
		
		if (replyNoReceived == false)
		{
			dataHasBeenRead = false;
		}

		 
	 
	}
	 
 
}
/**
* @brief 提交实时数据-WIN或PLA
* @param ouputStruct：算法传递来的数据，type:传送类型默认win
*/
void BllRealTimeTrans::submitWINOrPLA(char *data, int dataLength,int dataType)
{
	TagProtocolMsg msg;
	msg.MsgID = 8;
	msg.nDataSize = dataLength;
	 
	QString  type;
	if (dataType == WINTYPE )
	{
		type = QString("WIN");
		dataType = WINTYPE;
		strcpy(msg.Param, "WIN");//"WIN(或PLA或QIN或QPL)";
	}		
	else if  (dataType == PLATYPE)
	{
		type = QString("PLA");
		dataType = PLATYPE;
		strcpy(msg.Param, "PLA");//"WIN(或PLA或QIN或QPL)";
	}
	
	strcpy(msg.Check, "RDBS");
	

	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));

	Global::client_cmd_status = ClientCmdStatus::submit_real_data_status;
	Global::mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket

	//data char * 转为 QByteArray
	QByteArray sendBlock;
	sendBlock.append(data,dataLength);
	//memcpy(sendBlock.data(),data,dataLength);
	//sendBlock = QByteArray(data);

	//稍微需要处理一下循环发送数据//
	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	//bool success = false;
	if (success)
	{
		Global::serverSubmitFailed = false;
		QByteArray result;
		int descriptor;//引用
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据

		QString reply = result.data();

		if (reply == "NO")
		{
			replyNoReceived = true;
			qDebug("wait");
		}

	//	emit statuChanged(QString("服务端：回复，提交实时数据指令，%1,2%").arg(reply).arg(type));

		emit statuChanged(QString("服务端：回复%2，提交实时数据指令-%1").arg(type).arg(reply));

		if (reply == "OK")//提交WIN
		{
			replyNoReceived = false;
			//****************************************************//
			Global::mcsNetClient->sendData(sendBlock, false);//发送数据，且不需要关闭socket
			emit statuChanged(QString("服务端：回复，提交实时数据-%1").arg(type));


			//**若对方有回复****//
			//bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
			//if (success)
			//{
			//	QByteArray result;
			//	Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据

			//	QString reply = result.data();
			//	emit statuChanged(QString("服务端：回复，提交实时数据-%1，%2").arg(type).arg(reply));
			//}
			//else
			//{
			//	emit statuChanged(QString("服务端：错误，提交实时数据--%1，%2，14个.").arg(type).arg(reply));
			//}
			//**若对方有回复****//
		}
		else
		{
			qDebug("Submit WIN  PLA reply No \n");
		}

	}
	else
	{
		 
		//将数据写入文件
		Global::serverSubmitFailed = true;
		replyNoReceived = true;
		emit statuChanged("识别端：错误，提交实时数据指令失败.") ;

		 
	}
		
}

/**
* @brief 提交实时数据-QIN或QPL
* @param ouputStruct：算法传递来的数据，type:传送类型默认win
*/
void BllRealTimeTrans::submitQINOrQPL(char *data, int dataLength,int dataType)
{
	 
	TagProtocolMsg msg;
	QString type;
	msg.MsgID = 8;
	msg.nDataSize =  dataLength ;// N个WPDATAINFO或QDATAINFO结构数据，按马编号
	if (dataType  == QINTYPE)
	{
		type = QString("QIN");
		strcpy(msg.Param, "QIN");//"WIN(或PLA或QIN或QPL)";
		dataType = QINTYPE;
	}
		
	else if (dataType == QPLTYPE)
	{
		type = QString("QPL");
		strcpy(msg.Param, "QPL");//"WIN(或PLA或QIN或QPL)";
		dataType = QPLTYPE;
	}
		
	strcpy(msg.Check, "RDBS");


	QByteArray byteArray;
	int m = sizeof(TagProtocolMsg);
	byteArray.append((char*)&msg, sizeof(TagProtocolMsg));

	Global::client_cmd_status = ClientCmdStatus::submit_real_data_status;
	Global::mcsNetClient->sendData(byteArray, false);//发送数据，且不需要关闭socket


	//**稍微需要处理一下循环发送数据****//
	bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
	QByteArray sendBlock ;
	sendBlock.append(data, dataLength);
	if (success)
	{
		Global::serverSubmitFailed = false;

		QByteArray result;
		int descriptor;
		Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据

		QString reply = result.data();
		emit statuChanged(QString("服务端：回复，提交实时数据指令-%1,%2").arg(type).arg(reply));
		if (reply == "NO")
		{
			replyNoReceived = true;
			qDebug("wait");
		}
		if (reply == "OK")//提交QIN
		{
			
			replyNoReceived = false;
			//****************************************************//
			Global::mcsNetClient->sendData(sendBlock, false);//发送数据，且不需要关闭socket
			emit statuChanged(QString("识别端：正常，提交实时数据-%1，14个.").arg(type));


			//**若对方有回复****//
			//bool success = Global::mcsNetClient->waitForReadyRead(3000);//阻塞等待
			//if (success)
			//{
			//	QByteArray result;
			//	Global::mcsNetClient->readAllMessage(result, descriptor);//读取数据

			//	QString reply = result.data();
			//	emit statuChanged(QString("服务端：回复，提交实时数据-%1，%2").arg(type).arg(reply));
			//}
			//else
			//{
			//	emit statuChanged(QString("服务端：错误，提交实时数据--%1，%2，14个.").arg(type).arg(reply));
			//}
			//**若对方有回复****//
		}
		else
		{
			qDebug("Submit Qin QPL reply No \n");
		}

	}
	else
	{
		//将数据写入文件
		replyNoReceived = true;
		Global::serverSubmitFailed = true;
		emit statuChanged(QString("识别端：错误，提交实时数据指令-%1,失败.").arg(type));
  

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
	emit statuChanged(QString("服务端：回复，实时数据指令，%1").arg(reply));

	//*************还得发送数据：WIN(或PLA或QIN或QPL)*********//
	/*tagWPDataInfo或者tagQDataInfo*/
	//****************************************************//
	Global::mcsNetClient->sendData(result, false);//发送数据，且不需要关闭socket
}
/*
@brief 发送缓存区数据,缓存区数据为发送失败缓存的数据
*/
void BllRealTimeTrans::sendBufferDataToServer()
{
	qDebug("test");
}

/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleReceiveData(QByteArray result, int descriptor)
{
	switch (Global::client_cmd_status)
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
	emit statuChanged("识别端：正常，客户端连接上服务端，.");

	//已经连接上
	Global::serverSubmitFailed = false;
}
/**
* @brief 识别端处理服务端返回的数据
*/
void BllRealTimeTrans::handleDisConnect()
{
	emit statuChanged("识别端:正常，客户端已断开连接。");

	//网络中断了 则重连服务器
	if (Global::serverSubmitFailed == true)
	{
 		//connectToHost(SERVER_IP_ADDRESS, SERVER_PORT);

	}

}

