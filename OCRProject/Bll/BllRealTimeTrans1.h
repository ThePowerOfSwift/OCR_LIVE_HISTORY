﻿#ifndef BLLREALTIMETRANS1_H
#define BLLREALTIMETRANS1_H

#include <QObject>
#include "Include/Global.h"
#include "Include/HorseDataStruct.h"

//#define  HISTORY_SUBMIT 

class BllRealTimeTrans1 : public QObject
{
	Q_OBJECT

public:
	BllRealTimeTrans1(QObject *parent = 0 );
	~BllRealTimeTrans1();

	DataOutput priDataOutput;

	//主控制器网络通讯接口类
	Network *mcsNetClient;
	//全局客户端状态
	ClientCmdStatus client_cmd_status;

	//
	qint32 serverNo; 
private:
	CCycleBuffer * mSendDataBuffer;
	bool serverNotConnected;

	bool serverSubmitFailed;

	//标记服务器连接状态

	bool curServerState;
	bool priServerState;
	
public slots:

	/**
	* @brief 连接服务器
	* @param ip,port
	*/
	void connectToHost(QString serverIp, qint32 serverPort);
	/**
	* @brief 断开服务器
	*/
	void disconnectToHost();
	/**
	* @brief 识别端登录请求指令
	*/
	void clientLogin();

	/**
	* @brief 识别端处理服务端返回的数据
	*/
	void handleLogin(QByteArray result, int descriptor);
	/**
	* @brief 识别端请求马信息表指令
	*/
	void requestHorse();
	/**
	* @brief 识别端处理服务端-请求马信息表指令
	*/
	void handleRequestHorse(QByteArray result, int descriptor);
	/**
	* @brief 识别端根据RaceNO(场次号)请求RaceID指令
	* @param qint32 raceNo 场次号
	*/
	void requestRaceID(int session );
	/**
	* @brief 识别端处理服务端-请求RaceID指令
	*/
	void handleRequestRaceID(QByteArray result, int descriptor);
	/**
	* @brief 识别端提交比赛时长指令
	* @param qint32 raceTime 比赛时长
	*/
	void submitRaceTime(qint32 raceTime);
	/**
	* @brief 识别端处理服务端-提交比赛时长指令
	*/
	void handleSubmitRaceTime(QByteArray result, int descriptor);

	/**
	* @brief 识别端发送提交实时数据指令
	*/
	void submitRealData(DataOutput outputStruct, QByteArray array ,int imageWidth, int imageHeight);
	/**
	* @brief 提交win实时数据
	*/
	int submitWINOrPLA(DataOutput &outputStruct, QString type);
	/**
	* @brief 提交实时数据-QIN或QPL
	* @param ouputStruct：算法传递来的数据，type:传送类型默认QIN
	*/
	int submitQINOrQPL(DataOutput &outputStruct, QString type);
	/**
	* @brief 识别端处理服务端-提交实时数据指令
	*/
	void handleSubmitRealData(QByteArray result, int descriptor);
	/**
	* @brief 识别端处理服务端返回的数据
	* @param QByteArray:数据，int:socket的描述符
	*/
	void handleReceiveData(QByteArray result, int descriptor);
	/**
	* @brief 识别端处理服务端返回的数据
	*/
	void handleConnect();
	/**
	* @brief 识别端处理服务端返回的数据
	*/
	void handleDisConnect();


	/**
	* @brief 发送缓存区数据,缓存区数据为发送失败缓存的数据
	*/

	void sendBufferDataToServer();



	/**
	* @brief 为了保持连接，在空闲时候请求raceID
	*/
	void requestRaceIDForKeepAlive( int session );

	/*
	*  最后一场结束了，发送比赛结束指令
	*/

	void submitRaceEnd();

signals:
	void statuChanged(QString status);
private:

	int connectCount;

	bool replyNoReceived;

	int dataType ;
	int dataLength ;

	char * data;

	bool dataHasBeenRead;

	//检测是否场次号发生了变化
	bool isSessionChanged;

	int priSession;

	DataOutput mDataOutput;

	bool stopReadBuffData;
};


 
#endif // BLLREALTIMETRANS_H
