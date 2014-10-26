
#ifndef OCRCONTROL_H
#define OCRCONTROL_H

#include <QWidget>
#include <QFile>
#include "ui_OcrControl.h"
#include "Bll/BllRealTimeTrans.h"
#include "Include/HorseDataStruct.h"
#include "Bll/ThreadRealTime.h"
#include "Bll/BllRealTimeTrans1.h"
#include "Include/Global.h"
#include "Bll/BllDataIdentify.h"
#include "Bll/ThreadDataIdentify.h"

#include "HorseNameTrain/HorseNameTrain.h"

#include "../ui/MLineEdit.h"

#define  SHOW_ADBMP 1

class OcrControl : public QWidget
{
	Q_OBJECT

public:
	OcrControl(QWidget *parent = 0) ;
	~OcrControl() ;
public slots:
	//连接服务器
	void on_connectBtn_clicked();
	//断开服务器
	void on_disconnectBtn_clicked();
	//登陆服务器
	void on_loginBtn_clicked();
 
	//追加网络状态
	void appendStatus(QString status);

	/**
	* @brief 开始采集
	*/
	void on_startAcqBtn_clicked();
	/**
	* @brief 停止采集
	*/
	void on_stopAcqBtn_clicked();
	/**
	* @brief 更新界面
	*/
	void updateData(DataOutput output, QByteArray array, int imageWidth, int imageHeight);
	// 更新广告图片
	void updateADData(DataOutput output, QByteArray array,int imageWidth, int imageHeight);
	void updateUiData(DataOutput output, QByteArray array);
	void updateQINQPLData(DataOutput output, QByteArray array);
	 
	/**
	* @brief 断开重连
	*/
	void reConnect();

	/**
	* @brief 打开历史文件
	*/
	void on_loadFileBtn_clicked();
	/**
	* @brief 追加历史文件
	*/
	void on_appendFileBtn_clicked();
	/**
	* @brief 删除历史文件
	*/
	void on_delFileBtn_clicked();

	/**
	* @brief 追加文件接口函数
	*/
	void appendFile(QString fileName);
	/**
	* @brief 删除头文件接口函数
	*/
	QString takeTopFile(int row = 0);

	/**
	* @brief 校正场次号 倒计时 按钮按下 
	*/
	void on_caliSessionBtn_clicked();


	/*
	
	*/

	void on_caliCountDownBtn_clicked();

	/* 
	* @brief 开始处理历史视频
	*/
	void startProcessHistoryVideo();


	/*
	历史视频快进3min
	*/
	void on_advance3MinBtn_clicked();

	/*
	   历史视频快进1min
	*/
	void on_advance1MinBtn_clicked();
	/*
	历史视频快进 30s 
	*/
	void on_advance30SecBtn_clicked();
	/*
	历史视频快进 10s
	*/
	void on_advance10SecBtn_clicked();
	/*
	历史视频快进 1s
	*/
	void on_advance1SecBtn_clicked();

	/*
		视频回撤
	*/
	void on_pullBackBtn_clicked();
	/*
	 暂停采集
	*/
	void on_pauseCaliBtn_clicked();

	/*
		@brief 输入用户校正过后的数据
	*/
	void on_inputUserDataBtn_clicked();

 
	/*
			录入服务器设置
	*/

	void on_svrConfigSaveBtn_clicked();


	/*
	 依据不同的 combox选择项，即不同的服务器，显示不同的地址配置
	*/
	void upDateSvrLineEdit(QString);

	/*
		获取今天马匹排位表
	*/


	/*
	用户输入完毕后，进行更新界面，主要是 马名，场次号 ，win pla qin qpl数据
	*/

	void updateAfterUserInput(DataOutput  &output);


	/*
	  获取马名
	*/

	void getHorseNameFromDataFile(   );


	/*
		63台checkbox 状态改变
	*/

	void is63TAICheckBoxStateChanged( bool);
	/*
		是否锁定修改
	*/


	void islockCaliCheckBoxStatusChanged(bool);
	/*
		写入数据文件
	*/
	void  writeHistoryData(DataOutput &dataOutput) ;


	int  isDataOutputNew(DataOutput &outputStruct,DataOutput &priOutputStruct);
	void getClientSocketState(QAbstractSocket::SocketState socketState);


	// 初始化用户界面

	void initUi();
signals:
	void connectToServer(QString,qint32);
	void connectToServer1(QString, qint32);

	void disconnect();
	void login();
	void requestHorseInfo();
	void requestRaceId(qint32 raceNO);
	void submitRaceTime(qint32 raceTime);
	void submitReal();

	void startAcq();
	void stopAcq();
	//实时直播
	void startIdentify (QString fileName,int videoType );//开始识别
	void stopIdentify();//停止识别


	void getHorseNameFromDataFileSig();
 

private:
	Ui::OcrControl ui;
	BllRealTimeTrans* bllRealTimeTrans;//实时传输
	ThreadRealTime* threadRealTime;//实时传输 
	//第二个服务器 发送数据 
	BllRealTimeTrans1 *bllRealTimeTrans1;
	ThreadRealTime *threadRealTime1;


	BllDataIdentify* bllDataIdentify;//数据识别
	ThreadDataIdentify* threadDataIdentify;//数据识别线程

	//识别的数据

	DataOutput mDataOutput;
	DataOutput priDataOutputForShow;
	DataOutput priDataOutputForWrite;


	QString videoFileDate;

	//***马信息**//
	QList<MLineEdit*> indexLabelList;
	QList<MLineEdit*> horseNameEditList;
	QList<MLineEdit*> winLableList;
	QList<MLineEdit*> plaLableList;

	//QIN信息
	QList<QList<MLineEdit*>> qinList;
	//QPL信息
	QList<QList<MLineEdit*>> qplList;


	// 马名样本采集训练

	HorseNameTrain  * mHorseNameTrain ;

	//文件列表数量

	int historyVideoFileNum;

	int historyVideoFileProcessedCount;

	//保存上一次 快进的数据值

	int preVideoAdvanceValue;
//	ReadHistoryVideo myReadHistoryVideo;

	//历史文件 名称
	QString fileName;
	
	// 实时数据备份文件被创立
	bool liveBackupDataFileCreated;

	//实时直播的时候当前日期

	QString liveCurDate;


	QFile liveBackupFile;
	QDataStream liveBackupDataStream;

	// 调试用文件

	QFile logFile;

	QFile raceFile;
	QDataStream raceDataStream;

	int priSession;

	int priCountRaceTime;
//	QTextStream logContentOut ;
};

#endif // OCRCONTROL_H
