﻿#include "OcrControl.h"
#include "Include/HorseDataStruct.h"
#include <QPixmap>
#include <QTableWidget>
#include <QFileDialog>
OcrControl::OcrControl(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	 

	//网络
	bllRealTimeTrans = new BllRealTimeTrans();
	threadRealTime = new ThreadRealTime();
	bllRealTimeTrans->moveToThread(threadRealTime);
	threadRealTime->start();
	
	QObject::connect(bllRealTimeTrans, SIGNAL(statuChanged(QString)), this, SLOT(appendStatus(QString)));//网络状态反馈
	QObject::connect(this, SIGNAL(connect(QString, qint32)), bllRealTimeTrans, SLOT(connectToHost(QString, qint32)));//连接
	QObject::connect(this, SIGNAL(disconnect()), bllRealTimeTrans, SLOT(disconnectToHost()));//断开
	QObject::connect(this, SIGNAL(login()), bllRealTimeTrans, SLOT(clientLogin()));//登录
	QObject::connect(this, SIGNAL(requestHorseInfo()), bllRealTimeTrans, SLOT(requestHorse()));//请求马信息
	QObject::connect(this, SIGNAL(requestRaceId(qint32)), bllRealTimeTrans, SLOT(requestRaceID(qint32)));//请求马信息
	QObject::connect(this, SIGNAL(submitRaceTime(qint32)), bllRealTimeTrans, SLOT(submitRaceTime(qint32)));//请求马信息
	//QObject::connect(this, SIGNAL(submitReal()), bllRealTimeTrans, SLOT(submitRealData(DataOutput)));//请求马信息


	QObject::connect(Global::mcsNetClient, SIGNAL(disConnectToHostSuccessSignal()), this, SLOT(reConnect()));//断开重连



	//采集
	//QObject::connect(this, SIGNAL(startAcq()), Global::myIAcq, SLOT(read()));//开始采集
	//QObject::connect(this, SIGNAL(stopAcq()), Global::myIAcq,  SLOT(unLoadDevice()));//停止采集

	//算法
	bllDataIdentify = new BllDataIdentify();//数据识别
	threadDataIdentify = new ThreadDataIdentify();//数据识别线程
	
	bllDataIdentify->moveToThread(threadDataIdentify);

	QObject::connect(bllDataIdentify, SIGNAL(readyRead(DataOutput, QByteArray, int, int)), bllRealTimeTrans, SLOT(submitRealData(DataOutput, QByteArray,int, int)));//开始发送
	QObject::connect(bllDataIdentify, SIGNAL(readyRead(DataOutput, QByteArray,int,int)), this, SLOT(updateData(DataOutput, QByteArray,int,int)));//停止计算

	//显示广告	
	QObject::connect(bllDataIdentify, SIGNAL(readyReadBmp(DataOutput, QByteArray, int, int)), this, SLOT(updateADData(DataOutput, QByteArray, int, int)));//停止计算

	QObject::connect(this, SIGNAL(startIdentify(QString, int)), bllDataIdentify, SLOT(start(QString, int)));//开始计算
	QObject::connect(this, SIGNAL(stopIdentify()), bllDataIdentify, SLOT(stop()));//停止计算

	// 马名训练

	mHorseNameTrain = new HorseNameTrain;
	
	//用户从界面修改了 场次号 和倒计时 

	QObject::connect(ui.sessionLineEdit, SIGNAL(textChanged()), bllDataIdentify, SLOT(sessionNumTextChanged())); 
	QObject::connect(ui.raceTimeLineEdit, SIGNAL(textChanged()), bllDataIdentify, SLOT(sessionCountDownTextChanged()));

	ui.fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//表格为整行选择
	ui.fileTableWidget->resizeColumnsToContents();	// 显示文件名 

	//QObject::connect(Global::myIAcq, SIGNAL(stopDataIdentify()), bllDataIdentify, SLOT(stop()));//停止计算


	// 历史视频 快进 

	
//	QObject::connect(this, SIGNAL(setSignal( )), bllDataIdentify, SLOT(testSlot( )));


	int labelHeight = 17;
	
	//初始化马信息
	for (int i = 0; i < 14; i++)
	{
		QFrame* frame = new QFrame(ui.horseFrame);
		frame->setObjectName(QStringLiteral("frame"));

		frame->setStyleSheet(QLatin1String("QFrame#frame\n"
			"{\n"
			"	border-bottom: 0px solid #9D9D9D;\n"
			"	border-radius:0px;\n"
			"    padding:0 0px;\n"
			"	margin-right: 0px;\n"
			"\n"
			"}\n"
			""));
		frame->setFrameShape(QFrame::StyledPanel);
		frame->setFrameShadow(QFrame::Plain);
		QHBoxLayout* horizontalLayout_2 = new QHBoxLayout(frame);
		horizontalLayout_2->setSpacing(0);
		horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
		horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
		horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
		QLineEdit* indexLbel_3 = new QLineEdit(frame);
		indexLbel_3->setObjectName(QStringLiteral("indexLbel_3"));
		QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(indexLbel_3->sizePolicy().hasHeightForWidth());
		indexLbel_3->setSizePolicy(sizePolicy);
		indexLbel_3->setAlignment(Qt::AlignLeft);

		indexLbel_3->setMinimumSize(QSize(50, labelHeight));
		indexLbel_3->setMaximumSize(QSize(50, labelHeight));
		indexLabelList.append(indexLbel_3);
		horizontalLayout_2->addWidget(indexLbel_3);

		QLineEdit* horseNameEdit_3 = new QLineEdit(frame);
		horseNameEdit_3->setObjectName(QStringLiteral("horseNameEdit_3"));
		QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Maximum);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(horseNameEdit_3->sizePolicy().hasHeightForWidth());
		horseNameEdit_3->setSizePolicy(sizePolicy1);
		horseNameEdit_3->setMinimumSize(QSize(16777215, labelHeight));
		horseNameEdit_3->setMaximumSize(QSize(16777215, labelHeight));
		horseNameEditList.append(horseNameEdit_3);
		horizontalLayout_2->addWidget(horseNameEdit_3);

		QLineEdit* winLbl_3 = new QLineEdit(frame);
		winLbl_3->setObjectName(QStringLiteral("winLbl_3"));
		sizePolicy.setHeightForWidth(winLbl_3->sizePolicy().hasHeightForWidth());
		winLbl_3->setSizePolicy(sizePolicy);
		QFont font1;
		font1.setPointSize(12);
		winLbl_3->setFont(font1);
		winLbl_3->setAlignment(Qt::AlignLeft);
		winLbl_3->setMinimumSize(QSize(30, labelHeight));
		winLbl_3->setMaximumSize(QSize(30, labelHeight));
		winLableList.append(winLbl_3);
		horizontalLayout_2->addWidget(winLbl_3);

		QLineEdit* PLALbl_3 = new QLineEdit(frame);
		PLALbl_3->setObjectName(QStringLiteral("PLALbl_3"));
		sizePolicy.setHeightForWidth(PLALbl_3->sizePolicy().hasHeightForWidth());
		PLALbl_3->setSizePolicy(sizePolicy);
		PLALbl_3->setFont(font1);
		PLALbl_3->setAlignment(Qt::AlignLeft);

		PLALbl_3->setMinimumSize(QSize(30, labelHeight));
		PLALbl_3->setMaximumSize(QSize(30, labelHeight));
		plaLableList.append(PLALbl_3);
		horizontalLayout_2->addWidget(PLALbl_3);

		horizontalLayout_2->setStretch(0, 1);
		horizontalLayout_2->setStretch(1, 4);
		horizontalLayout_2->setStretch(2, 3);
		horizontalLayout_2->setStretch(3, 3);

		ui.verticalLayout_4->addWidget(frame);


		
	}

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	ui.verticalLayout_4->addItem(verticalSpacer);


	int labelWidth = 35 ;
	 
	for (int j = 0; j < 7; j++)
	{
		QList<QLineEdit*> list;
		QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
		horizontalLayout_3->setSpacing(0);
		horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
  
		horizontalLayout_3->setContentsMargins(1, 1, 1, 1);

		QLineEdit* qinLbl_1 = new QLineEdit(ui.qinFrame);
		qinLbl_1->setObjectName(QStringLiteral("qinLbl_1"));
		QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(qinLbl_1->sizePolicy().hasHeightForWidth());

		qinLbl_1->setSizePolicy(sizePolicy2);

		qinLbl_1->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_1->setMaximumSize(QSize(labelWidth, 14));
	//	qinLbl_1->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));

		list.append(qinLbl_1);
		horizontalLayout_3->addWidget(qinLbl_1);

		QLineEdit* qinLbl_2 = new QLineEdit(ui.qinFrame);
		qinLbl_2->setObjectName(QStringLiteral("qinLbl_2"));
		sizePolicy2.setHeightForWidth(qinLbl_2->sizePolicy().hasHeightForWidth());
		qinLbl_2->setSizePolicy(sizePolicy2);
		qinLbl_2->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_2->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_2->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_2);
		horizontalLayout_3->addWidget(qinLbl_2);

		QLineEdit* qinLbl_3 = new QLineEdit(ui.qinFrame);
		qinLbl_3->setObjectName(QStringLiteral("qinLbl_3"));
		sizePolicy2.setHeightForWidth(qinLbl_3->sizePolicy().hasHeightForWidth());
		qinLbl_3->setSizePolicy(sizePolicy2);

		qinLbl_3->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_3->setMaximumSize(QSize(labelWidth, 14));
		//	qinLbl_3->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_3);
		horizontalLayout_3->addWidget(qinLbl_3);

		QLineEdit* qinLbl_4 = new QLineEdit(ui.qinFrame);
		qinLbl_4->setObjectName(QStringLiteral("qinLbl_4"));
		sizePolicy2.setHeightForWidth(qinLbl_4->sizePolicy().hasHeightForWidth());
		qinLbl_4->setSizePolicy(sizePolicy2);
		qinLbl_4->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_4->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_4->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_4);
		horizontalLayout_3->addWidget(qinLbl_4);

		QLineEdit* qinLbl_5 = new QLineEdit(ui.qinFrame);
		qinLbl_5->setObjectName(QStringLiteral("qinLbl_5"));
		sizePolicy2.setHeightForWidth(qinLbl_5->sizePolicy().hasHeightForWidth());
		qinLbl_5->setSizePolicy(sizePolicy2);
		qinLbl_5->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_5->setMaximumSize(QSize(labelWidth, 14));
		//	qinLbl_5->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_5);
		horizontalLayout_3->addWidget(qinLbl_5);

		QLineEdit* qinLbl_6 = new QLineEdit(ui.qinFrame);
		qinLbl_6->setObjectName(QStringLiteral("qinLbl_6"));
		sizePolicy2.setHeightForWidth(qinLbl_6->sizePolicy().hasHeightForWidth());
		qinLbl_6->setSizePolicy(sizePolicy2);
		qinLbl_6->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_6->setMaximumSize(QSize(labelWidth, 14));
		//	qinLbl_6->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_6);
		horizontalLayout_3->addWidget(qinLbl_6);

		QLineEdit* qinLbl_7 = new QLineEdit(ui.qinFrame);
		qinLbl_7->setObjectName(QStringLiteral("qinLbl_7"));
		sizePolicy2.setHeightForWidth(qinLbl_7->sizePolicy().hasHeightForWidth());
		qinLbl_7->setSizePolicy(sizePolicy2);
		qinLbl_7->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_7->setMaximumSize(QSize(labelWidth, 14));
		//	qinLbl_7->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_7);
		horizontalLayout_3->addWidget(qinLbl_7);

		QLineEdit* qinLbl_8 = new QLineEdit(ui.qinFrame);
		qinLbl_8->setObjectName(QStringLiteral("qinLbl_8"));
		sizePolicy2.setHeightForWidth(qinLbl_8->sizePolicy().hasHeightForWidth());
		qinLbl_8->setSizePolicy(sizePolicy2);
		qinLbl_8->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_8->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_8->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_8);
		horizontalLayout_3->addWidget(qinLbl_8);

		QLineEdit* qinLbl_9 = new QLineEdit(ui.qinFrame);
		qinLbl_9->setObjectName(QStringLiteral("qinLbl_9"));
		sizePolicy2.setHeightForWidth(qinLbl_9->sizePolicy().hasHeightForWidth());
		qinLbl_9->setSizePolicy(sizePolicy2);
		qinLbl_9->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_9->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_9->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_9);
		horizontalLayout_3->addWidget(qinLbl_9);

		QLineEdit* qinLbl_10 = new QLineEdit(ui.qinFrame);
		qinLbl_10->setObjectName(QStringLiteral("qinLbl_10"));
		sizePolicy2.setHeightForWidth(qinLbl_10->sizePolicy().hasHeightForWidth());
		qinLbl_10->setSizePolicy(sizePolicy2);
		qinLbl_10->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_10->setMaximumSize(QSize(labelWidth, 14));
		//	qinLbl_10->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_10);
		horizontalLayout_3->addWidget(qinLbl_10);

		QLineEdit* qinLbl_11 = new QLineEdit(ui.qinFrame);
		qinLbl_11->setObjectName(QStringLiteral("qinLbl_11"));
		sizePolicy2.setHeightForWidth(qinLbl_11->sizePolicy().hasHeightForWidth());
		qinLbl_11->setSizePolicy(sizePolicy2);
		qinLbl_11->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_11->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_11->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_11);
		horizontalLayout_3->addWidget(qinLbl_11);

		QLineEdit* qinLbl_12 = new QLineEdit(ui.qinFrame);
		qinLbl_12->setObjectName(QStringLiteral("qinLbl_12"));
		sizePolicy2.setHeightForWidth(qinLbl_12->sizePolicy().hasHeightForWidth());
		qinLbl_12->setSizePolicy(sizePolicy2);
		qinLbl_12->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_12->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_12->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_12);
		horizontalLayout_3->addWidget(qinLbl_12);

		QLineEdit* qinLbl_13 = new QLineEdit(ui.qinFrame);
		qinLbl_13->setObjectName(QStringLiteral("qinLbl_13"));
		sizePolicy2.setHeightForWidth(qinLbl_13->sizePolicy().hasHeightForWidth());
		qinLbl_13->setSizePolicy(sizePolicy2);
		qinLbl_13->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_13->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_13->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_13);
		horizontalLayout_3->addWidget(qinLbl_13);

		QLineEdit* qinLbl_14 = new QLineEdit(ui.qinFrame);
		qinLbl_14->setObjectName(QStringLiteral("qinLbl_14"));
		sizePolicy2.setHeightForWidth(qinLbl_14->sizePolicy().hasHeightForWidth());
		qinLbl_14->setSizePolicy(sizePolicy2);
		qinLbl_14->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_14->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_14->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_14);
		horizontalLayout_3->addWidget(qinLbl_14);

		QLineEdit* qinLbl_15 = new QLineEdit(ui.qinFrame);
		qinLbl_15->setObjectName(QStringLiteral("qinLbl_15"));
		sizePolicy2.setHeightForWidth(qinLbl_15->sizePolicy().hasHeightForWidth());
		qinLbl_15->setSizePolicy(sizePolicy2);
		qinLbl_15->setMinimumSize(QSize(labelWidth, 14));
		qinLbl_15->setMaximumSize(QSize(labelWidth, 14));
		//qinLbl_15->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_15);
		horizontalLayout_3->addWidget(qinLbl_15);

		qinList.append(list);
		ui.verticalLayout_5->addLayout(horizontalLayout_3);
	}
	 

	//设置版本号 
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::white);
	ui.versionNumLabel->setText(VERSION_NUM);
	ui.versionNumLabel->setPalette(pe);


	//设置进度

	ui.videoProgressBar->setRange(0, 100);
	ui.videoProgressBar->setValue(0);


	ui.videoPosLineEdit->setText(0);


	// 文件数目为 0
	historyVideoFileNum = 0;

	historyVideoFileProcessedCount = 0;

	//显示广告	
	QObject::connect(bllDataIdentify, SIGNAL(readNextFile()), this, SLOT(startProcessHistoryVideo ()));//停止计算

	// 有了新的场次号，请求raceID
	QObject::connect(bllDataIdentify, SIGNAL(requestRaceIdSig()), bllRealTimeTrans, SLOT(requestRaceID()));

 
	// 连接 63台checkbox
	QObject::connect(ui.is63TAICheckBox, SIGNAL(clicked(bool)), this, SLOT(is63TAICheckBoxStateChanged(bool)));


	liveBackupDataFileCreated = false;
	
	 
}

OcrControl::~OcrControl()
{
	//停止实时提交
	if (threadRealTime->isRunning())
	{
		threadRealTime->quit();
	}
	//删除实时网络传输业务
	if (bllRealTimeTrans!=0)
		delete bllRealTimeTrans;
	//删除网络传输线程
	if (threadRealTime != 0)
	{
		//threadRealTime->terminate();
		threadRealTime->wait();
		delete threadRealTime;
	}
	//删除识别算法线程
	if (threadDataIdentify->isRunning())
	{
		Global::pauseDataIdentifyTag = true;//停止识别
		threadDataIdentify->quit();
	}


	if (Global::S_CCycleBuffer != 0)
	{
		delete Global::S_CCycleBuffer;
	}


	Global::historyIdentifyDataFile.close();

	liveBackupFile.close();

}
/**
* @brief 连接服务器
*/
void OcrControl::on_connectBtn_clicked()
{
	emit connect("58.67.161.109", 9068);
}
/**
* @brief 断开服务器
*/
void OcrControl::on_disconnectBtn_clicked()
{
	
	
	emit disconnect();
}
/**
* @brief 登陆服务器
*/
void OcrControl::on_loginBtn_clicked()
{
	emit login();
}
/**
* @brief 请求马信息
*/
void OcrControl::on_requestHorseInfoBtn_clicked()
{
	emit requestHorseInfo();
}

/**
* @brief 请求RaceId
*/
void OcrControl::on_requestRaceIdBtn_clicked()
{
	emit requestRaceId(1);//每次只有1场
}
/**
* @brief 提交比赛时长
*/
void OcrControl::on_submitRaceTimeBtn_clicked()
{
	emit submitRaceTime(35);
}
/**
* @brief 提交实时数据
*/
void OcrControl::on_submitRealBtn_clicked()
{
	emit submitReal();
}
/**
* @brief 追加网络状态
*/
void OcrControl::appendStatus(QString status)
{
	ui.textBrowser->append(status);
}
/**
* @brief 开始采集
*/
void OcrControl::on_startAcqBtn_clicked()
{

	if (Global::isHistoryVideoIdentifyRuning == true )
	{
		return;
	}
	Global::stopDataIdentifyTag = false;//开始模拟标示符
	//Global::threadAcq->start();//采集启动
	//emit startAcq();//开始采集
	
//	threadDataIdentify->start();//开始识别
//	fileName = "E:\\BaiduYunDownload\\20130109184858_clip1.wmv";
//	emit startIdentify(fileName, videoType);//开始识别
	 
	// 历史视频获取起始帧位置，方便测试

	QString posText = ui.videoPosLineEdit->text();

	Global::videoStartPos = posText.toInt();
	//	视频种类选择  

	int videoType;
	int curIndex = ui.videoTypeComboBox->currentIndex();
	QString text = ui.videoTypeComboBox->currentText();

	if (text == "亚洲台")
	{
		Global::isHistoryVideo = true;
		videoType =YAZHOUTAI;
	}
	else if (text == "香港18台")
	{
		Global::isHistoryVideo = true;
		videoType = HK18TAI;
	}
	else if (text == "香港18台D14")
	{
		Global::isHistoryVideo = true;
		videoType = HK18D14;
	}

	else if (text == "直播")
	{
		Global::isHistoryVideo = false;
		videoType = LIVE ;
	}

	Global::videoType = videoType;


	//是历史视频 的话，那么应该读取视频文件 
	if (videoType != LIVE )
	{
		threadDataIdentify->start();//开始识别 
		startProcessHistoryVideo(); 
	}
	else //实时直播
	{
		threadDataIdentify->start();//开始识别

		emit startIdentify(NULL, false );//开始识别
		Global::myIAcq->read();
	}
	 

}

/*
	获取马名
Race Date 	20060101
Start one Session 	 02
Toal horse number 12
*/


void OcrControl::getHorseNameFromDataFile(   )
{ 
	 
}

void OcrControl::startProcessHistoryVideo()
{
	
 
	threadDataIdentify->quit();// 退出 	 
	while (threadDataIdentify->isRunning())
	{
		Sleep(1);
	}
	threadDataIdentify->start();//开始识别 

	//如果处理掉的文件
	if (historyVideoFileNum > 0)
	{
		fileName = takeTopFile(0);

		if (Global::videoType == YAZHOUTAI)
		{
			QString label = ".wmv";
			int pos = 0;
			pos = fileName.indexOf(label);
			if (pos <= 0 )
			{
				label = ".bmp";
				if (fileName.indexOf(label) <= 0)
				{
					return;
				}
			}

		}

		if (Global::videoType == HK18D14)
		{
			
			QString label = ".d14";
			int pos = 0;
			pos = fileName.indexOf(label);

			QString label1 = ".h64";

			int pos1 = fileName.indexOf(label1);

			if (pos < 0 & pos1 < 0 )
			{
				 
				label = ".bmp";
				if (fileName.indexOf(label) <= 0)
				{
					return;
				}
				 
			}

		}

		if (Global::videoType == HK18TAI)
		{
			QString label = ".wmv";
			int pos = 0;
			pos = fileName.indexOf(label);
			if (pos <= 0)
			{
				label = ".bmp";
				if (fileName.indexOf(label) <= 0)
				{
					return;
				}
			}

		}
		 
		//fileName = "E:\\BaiduYunDownload\\20130109184858_clip1.wmv";
		emit startIdentify(fileName, Global::videoType);//开始识别
 
	}
 
	historyVideoFileNum = ui.fileTableWidget->rowCount();

	 

	 
}
/**
* @brief 停止采集
*/
void OcrControl::on_stopAcqBtn_clicked()
{
	  
		Global::stopDataIdentifyTag = true;

		emit stopAcq();//停止采集

		//停止采集 while 循环
		 
		Global::myIAcq->unLoadDevice();
		//Global::threadAcq->quit();

		//emit stopDataIdentify();//停止识别
 
		threadDataIdentify->quit();
		 
}


/**
* @brief 更新广告图片
*/

void OcrControl::updateADData(DataOutput  output, QByteArray  array,int imageWidth, int imageHeight)
{
		
		//更新图片信息
		QPixmap pixmap;
		QImage myImage;

		int imageLength = imageWidth*imageHeight * 3;
		char * buffer;
		buffer = new char[imageLength];
		memcpy(buffer, array.data(), imageLength);
		myImage = QImage(imageWidth, imageHeight, QImage::Format_RGB888);


		// 转换RGB888 到QIMAGE

		for (int h = 0; h < imageHeight; h++) {
			// scanLine returns a ptr to the start of the data for that row 
			memcpy(myImage.scanLine(h), (buffer + imageWidth * 3 * h),
				imageWidth * 3);
		}

		QString curFileName = Global::myIAcq->getTestImageName();
		curFileName = curFileName.mid(curFileName.count() - 9, 9);
		ui.imageFileNameLabel->setText(curFileName);
 
		if (ui.videoTypeComboBox->currentText() != QString("直播"))
		{
			//更新进度条
			ui.videoProgressBar->setValue(output.videoProgressPercent);

			myImage = myImage.scaled(518, 432);
		}
		else 
			myImage = myImage.scaled(518, 432);
		
		pixmap = pixmap.fromImage(myImage);
		ui.imageLbl->setPixmap(pixmap);
		delete[] buffer;
		 
		QPalette pe;
		pe.setColor(QPalette::WindowText, Qt::white);
		ui.adTimeLbl->setText("ADTime");
		ui.adTimeLbl->setPalette(pe);
		ui.adTimeLbl->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));
		

		ui.CountRaceTimeLineEdit->setText(QString::number(Global::countRaceTime) ) ;
		ui.adTimeLbl->setPalette(pe);
		ui.adTimeLbl->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));

 
}
/**
* @brief 更新数据
*/


void OcrControl::updateData(DataOutput output, QByteArray array,int imageWidth, int imageHeight)
{

	//获取当前 输出结果
	mDataOutput = output;


	//如果此时 网络中断 那么开启 写入本地文件模式
	//直播
	if (!Global::isHistoryVideo )
	{
		if (Global::serverSubmitFailed)
		{
			writeHistoryData(mDataOutput);
		}
	}
	//else
	//比较是否发生变化 无论直播，还是历史
	{
		isDataOutputNew(mDataOutput);
	}

	int imageLength = imageWidth*imageHeight * 3;

	if (SHOW_ADBMP | output.haveDataFlag )
	{
		//更新
		ui.sessionLineEdit->setText(QString::number(Global::session));//更新全局场次号
		ui.raceTimeLineEdit->setText(QString::number(Global::raceTime));//更新全局比赛时间
		ui.CountRaceTimeLineEdit->setText(QString::number(Global::countRaceTime)); //顺计时
		//更新图片信息
		QPixmap pixmap;
		QImage myImage;
		
		char * buffer;
		buffer = new char[imageLength];
		memcpy(buffer, array.data(), imageLength);
		myImage = QImage(imageWidth, imageHeight, QImage::Format_RGB888);


		// 转换RGB888 到QIMAGE
		
		for (int h = 0; h < imageHeight; h++) {
			// scanLine returns a ptr to the start of the data for that row 
			memcpy(myImage.scanLine(h), (buffer + imageWidth * 3 * h),
				imageWidth * 3);
		}
 

		if (ui.videoTypeComboBox->currentText() != QString("直播"))
		{
			//更新进度条
			ui.videoProgressBar->setValue(output.videoProgressPercent);

			myImage = myImage.scaled(518, 432);
		}
		else
			myImage = myImage.scaled(518, 432);

		pixmap = pixmap.fromImage(myImage);
		ui.imageLbl->setPixmap(pixmap);
		delete[] buffer;
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::white);
	if (output.haveDataFlag)
	{
		
		ui.adTimeLbl->setText("raceTime");
		ui.adTimeLbl->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
		//	ui.adTimeLbl->setStyleSheet(QStringLiteral("background-color: Green;"));
		ui.adTimeLbl->setPalette(pe);
		ui.sessionLineEdit->setText(QString::number(Global::session));//更新全局场次号
		ui.raceTimeLineEdit->setText(QString::number(Global::raceTime));//更新全局比赛时间 倒计时
		 

		updateUiData(mDataOutput, array);//更新马信息
		if (output.isQPL)
		{
			 
			updateQINQPLData(mDataOutput, array);//更新QPL信息
		}
		else
		{
		 
			updateQINQPLData(mDataOutput, array);//更新QIn信息
		}
			
	}
	
	QString curFileName = Global::myIAcq->getTestImageName();
	curFileName = curFileName.mid(curFileName.count() - 9, 9);
	ui.imageFileNameLabel->setText(curFileName);
	
	ui.CountRaceTimeLineEdit->setText(QString::number(Global::countRaceTime)); //顺计时

	ui.adTimeLbl->setPalette(pe);
	ui.adTimeLbl->setStyleSheet(QStringLiteral("background-color: rgb(255, 130, 80);"));

 

}

void OcrControl::updateUiData(DataOutput output, QByteArray array)
{
	 

	ui.userInputLabel->setText(QString("原始"));
	//更新马信息
	for (int i = 0; i < indexLabelList.size(); i++)
	{
		if (i < output.horseNum)
		{
			indexLabelList[i]->setText(QString::number(i + 1));
		}
		else
		{
			indexLabelList[i]->setText(QString("0"));
		}
	}
		
	for (int i = 0; i < horseNameEditList.size(); i++)
	{
		QString horseName;
		wchar_t *horseNameChar = new wchar_t[6];
		memset(horseNameChar, 0, 6 * sizeof(wchar_t));
		memcpy(horseNameChar, output.mHorseInfo.horseName[i], 4 * sizeof(wchar_t));

		horseName = horseName.fromWCharArray(horseNameChar) ;

		if (i < output.horseNum)
		{
			horseNameEditList[i]->setText(horseName);
			horseNameEditList[i]->setStyleSheet("QLineEdit{background:rgb(62,120,56);color: rgb(255,255,255)}");
		}
			
		else
		{
			indexLabelList[i]->setText(QString("0"));
		}
	}
	//如果马匹数量小于14 ，那么 剩下的地方显示 0
	for (int i = horseNameEditList.size() - 1; i < HORSENUMBER;i ++)
	{
		 

		if (i < output.horseNum)
			horseNameEditList[i]->setText(QString(""));
		else
		{
			indexLabelList[i]->setText(QString(" "));
		}
	}

	for (int i = 0; i < winLableList.size(); i++)
	{

		winLableList[i]->setText(QString::number(output.WIN[i]));


		if (output.isWinPlaHasGroundColor[i][0] )
		{
		//	winLableList[i]->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
		}

		if (output.winChangedFlag[i]  )
		{
			winLableList[i]->setStyleSheet("QLineEdit{background: rgb(0,0,0);color: rgb(255,255,255)}");
		}
		else
		{
			 
			winLableList[i]->setStyleSheet("QLineEdit{background:rgb(62,120,56);color: rgb(255,255,255)}");
	
		}

	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		plaLableList[i]->setText(QString::number(output.PLA[i]));
	
		if (output.plaChangedFlag[i] )
		{
			plaLableList[i]->setStyleSheet("QLineEdit{background: rgb(0,0,0);color: rgb(255,255,255)}");
		}
		else
		{

			plaLableList[i]->setStyleSheet("QLineEdit{background: rgb(62,120,56);color: rgb(255,255,255)}");
		}
		if (output.isWinPlaHasGroundColor[i][1] )
		{
	//		plaLableList[i]->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
		}

	}

	//赋值 
	if (mDataOutput.changeStatus > 0)
	{
		//更新 前一个结果
		priDataOutput = mDataOutput;

	}


}

/**
* @brief 更新qin
*/
void OcrControl::updateQINQPLData(DataOutput output, QByteArray array)
{
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::white);

	ui.QINLabel->setPalette(pe);
	if (output.isQPL)
	{
		ui.QINLabel->setText("Q P L");
	}
	else
	{
		ui.QINLabel->setText("Q I N");
	}
	
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		for (int j = 0; j <  list.size(); j++)
		{
			QLineEdit * label = list.at(j);
			label->setText(QString::number(output.QPL_QIN[i][j]));

			

			if (output.qplQinChangedFlag[i][j])
			{
				label->setStyleSheet("QLineEdit{background:rgb(0,0,0);color: rgb(255,255,255)}");
			}
			else
			{
				
				label->setStyleSheet("QLineEdit{background: rgb(62,120,56);color: rgb(255,255,255)}");
			}
			//如果是0 ，那么 使用黑底白色
			if (output.QPL_QIN[i][i] == 0)
			{
				label->setStyleSheet("QLineEdit{background:rgb(0,0,0);color: rgb(255,255,255)}");
			}

			if (output.isQplQinHasGroundColor[i][j])
			{
			//	label->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
			}

		}
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe1;
	 pe1.setColor(QPalette::WindowText, Qt::white);
	  
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		QLineEdit * label = list.at(i);		
		 
		label->setText(QString::number(i + 8));

		label->setStyleSheet("QLineEdit{background: gray;color: #FFFFFF}");
		 

	}

	// 设置无数据区域 i 0-6 j 0-14 （0,1）（1,2）~（6,7）
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		QLineEdit * label = list.at(i + 1);
		
		//label->setPalette(pe1);
		label->setStyleSheet("QLineEdit{background: gray;color: #FFFFFF}");
		label->setText(QString::number(i + 1));
	}


}


void OcrControl::reConnect()
{
	if (ui.reconnectCheckBox->isChecked())
	{
		qDebug("识别端：重连服务器 \n");
		emit connect("58.67.161.109", 9068);
	}
	else
	{
		return;
	}

}

/**
* @brief 打开历史文件
*/
void OcrControl::on_loadFileBtn_clicked()
{
	QFileDialog *fd = new QFileDialog(this, tr("选择历史视频文件"), "F:/movie", "");
	fd->setFileMode(QFileDialog::ExistingFiles);
	fd->setViewMode(QFileDialog::Detail);
	QStringList nameFilters;
	nameFilters << "rmvb files (*.* *.*)"
		<< "kux files (*.kux *.KUX)";

	fd->setNameFilters(nameFilters);//设置文件类型过滤器
	QStringList fileNamesList;
	if (fd->exec() == QDialog::Accepted) // 取消则是：QDialog::Rejected
	{
		fileNamesList = fd->selectedFiles();
	}
	else{
		return;
	}
	QString fileName = fileNamesList.at(0).toLocal8Bit().constData();
	QDir dir = fd->directory();
	QString absolutePath = dir.absolutePath();
	QString canonicalPath = dir.canonicalPath();


	
	//ui.fileTableWidget->resizeRowsToContents();
	//内容窗口自适应
	//ui.tableView->resizeColumnsToContents();
	for (int i = 0; i < fileNamesList.size(); i++)
	{
		int m = ui.fileTableWidget->rowCount();//目前总行数
		ui.fileTableWidget->insertRow(ui.fileTableWidget->rowCount());//插入新行
		ui.fileTableWidget->setItem(m, 0, new QTableWidgetItem(fileNamesList.at(i)));//插入指定项
	}

	ui.fileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//	ui.fileTableWidget->setResizeMode(QHeaderView::Stretch);


	historyVideoFileNum = fileNamesList.size();

	historyVideoFileProcessedCount = 0;
}
/**
* @brief 追加历史文件
*/
void OcrControl::on_appendFileBtn_clicked()
{
	QString fileName = "F:/hk18d14_1.bmp";
	appendFile(fileName);
}
/**
* @brief 追加历史文件
*/
void OcrControl::appendFile(QString fileName)
{ 
	int m = ui.fileTableWidget->rowCount();//目前总行数
	ui.fileTableWidget->insertRow(ui.fileTableWidget->rowCount());//插入新行
	ui.fileTableWidget->setItem(m, 0, new QTableWidgetItem(fileName));//插入指定项


	//写入系统日志
	Global::systemLog->append(QString(tr("新添加历史视频文件")), tr("成功."), SystemLog::INFO_TYPE);

}
/**
* @brief 删除历史文件
*/
void OcrControl::on_delFileBtn_clicked()
{
	QString fileName;
	int row = ui.fileTableWidget->currentRow();
	if (row <0)
		fileName = takeTopFile();
	else
		fileName = takeTopFile(row);

	//写入系统日志
	Global::systemLog->append(QString(tr("删除头文件")), tr("成功."), SystemLog::INFO_TYPE);
}
/**
* @brief 删除历史文件
*/
QString OcrControl::takeTopFile(int row)
{
	if (ui.fileTableWidget->rowCount() > 0)
	{
		QString strText = ui.fileTableWidget->item(row, 0)->text();
		ui.fileTableWidget->removeRow(row);
		return strText;
	}
	else
		return NULL;

}
/**
* @brief 校正场次号 倒计时 
*/

void OcrControl::on_caliSessionCountDownBtn_clicked()
{

	QString sessionStr;
	sessionStr = ui.sessionLineEdit->text();
	Global::session = sessionStr.toInt();

	ui.sessionLineEdit->setText(QString::number(Global::session));//更新全局场次号


	Global::isSessioncalibrated = true;

	QString raceTimeStr;
	raceTimeStr = ui.CountRaceTimeLineEdit->text();

	//更新全局时间
	Global::raceTime = raceTimeStr.toInt();

}



/*
历史视频快进1min
*/
void OcrControl::on_advance3MinBtn_clicked()
{
	Global::frameAccValue = 60*3 ;

	preVideoAdvanceValue = Global::frameAccValue;
	
	Global::requestAdvanceDuringPause = true;

}

/*
历史视频快进1min
*/
void OcrControl::on_advance1MinBtn_clicked()
{
	Global::frameAccValue = 60;
	preVideoAdvanceValue = Global::frameAccValue;
	Global::requestAdvanceDuringPause = true;
}
/*
历史视频快进 30s
*/
void OcrControl::on_advance30SecBtn_clicked()
{
	Global::frameAccValue = 30;
	preVideoAdvanceValue = Global::frameAccValue;
	Global::requestAdvanceDuringPause = true;
}
/*
历史视频快进 10s
*/
void OcrControl::on_advance10SecBtn_clicked()
{
	Global::frameAccValue = 10;
	preVideoAdvanceValue = Global::frameAccValue;
	Global::requestAdvanceDuringPause = true;
}

void OcrControl::on_pullBackBtn_clicked()
{
	
	Global::frameAccValue = Global::frameAccValue - preVideoAdvanceValue;


	preVideoAdvanceValue = 30;

	Global::requestAdvanceDuringPause = true;
	
	/*
	if (preVideoAdvanceValue <= 1)
	{
		Global::frameAccValue = 1 ;
	}
	*/
}
void OcrControl::on_pauseCaliBtn_clicked()
{
	if (!Global::pauseDataIdentifyTag)
	{
		Global::pauseDataIdentifyTag = true;//停止模拟标示符

		ui.pauseCaliBtn->setText(QString("继续"));
	}
	else
	{
		Global::pauseDataIdentifyTag = false;
		ui.pauseCaliBtn->setText(QString("暂停"));
	}
	
}


void OcrControl::on_continueBtn_clicked()
{
	Global::pauseDataIdentifyTag = false;
}

//输入用户校正数据，主要是win pla ，qin qpl
void OcrControl::on_inputUserDataBtn_clicked()
{
	
 

	//更新马信息
	/*
	for (int i = 0; i < horseNameEditList.size(); i++)
	{
		QString horseName;
		horseName = horseNameEditList[i]->text();

	 
		
		wchar_t *horseNameChar = new wchar_t[6];
		memset(horseNameChar, 0, 6 * sizeof(wchar_t));
		horseName.toWCharArray(horseNameChar);
		 
		memcpy(resultData.mHorseInfo.horseName[i], horseNameChar, 4 * sizeof(wchar_t));

		horseName = horseName.fromWCharArray(horseNameChar);

		resultData.mHorseInfo.horseName[i] = horseName;
		if (horseName == "0")
		{
			break;
		}

	}
	*/
	for (int i = 0; i < winLableList.size(); i++)
	{

		QString win;
		win = winLableList[i]->text();

		mDataOutput.WIN[i] = win.toFloat();
		 
	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		QString pla;
		pla = plaLableList[i]->text();

		mDataOutput.PLA[i] = pla.toFloat();
	  
	}

	for (int i = 0; i < qinList.size(); i++)
	{
		QString qplQin;
		QList<QLineEdit*> list = qinList.at(i);
		for (int j = 0; j < list.size(); j++)
		{
			QLineEdit * label = list.at(j);
			 
			qplQin = label->text();

			mDataOutput.QPL_QIN[i][j] = qplQin.toFloat();
			 
		}
	}	 
	//更新界面 不更新图片，
	updateAfterUserInput(mDataOutput);

	//写入数据
	writeHistoryData(mDataOutput);

}



/*
写入历史数据文件
*/

void OcrControl::writeHistoryData(DataOutput &dataOutput)
{


	//如果是直播则 
	if (!Global::isHistoryVideo  & liveBackupDataFileCreated == false )
	{
		liveBackupDataFileCreated = true;
		//获取exe路径
		QString runPath = QCoreApplication::applicationDirPath();

		QDir::setCurrent(runPath);
		//退到上一层目录
		QDir::setCurrent("../");

		QDir::setCurrent("../");


		QDir::setCurrent(".//OCRProject//liveBackupData//");
		// 写数据文件
		//如果是直播的时候中断 ，那么将数据写入本地文档，做备份，待比赛结束，将数据导入服务器
		
		liveCurDate = QDate::currentDate().toString("yyyyMMdd");
		liveBackupFile.setFileName(liveCurDate + QString(".txt"));

		QString curPath = QDir::currentPath();

		if (!liveBackupFile.exists())
		{

			if (!liveBackupFile.open(QIODevice::WriteOnly))
				qDebug("liveBackupFile open Failed");

			liveBackupDataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			liveBackupDataStream.setDevice(&liveBackupFile);

		}
		else
		{
			liveBackupFile.remove();
			if (!liveBackupFile.open(QIODevice::WriteOnly))
				qDebug("liveBackupFile open Failed");

			liveBackupDataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			liveBackupDataStream.setDevice(&liveBackupFile);

		}


	}

	if (videoFileDate != Global::historyVideoDate & !Global::isHistoryVideo )
	{


		Global::historyIdentifyDataFile.close();
		//
		QString runPath = QCoreApplication::applicationDirPath();
	
		QDir::setCurrent(runPath);
		//退到上一层目录
		QDir::setCurrent("../");

		QDir::setCurrent("../");


		QDir::setCurrent(".//OCRProject//");
		// 写数据文件
		//如果是直播的时候中断 ，那么将数据写入本地文档，做备份，待比赛结束，将数据导入服务器
		if (Global::isHistoryVideo)
		{
			Global::historyIdentifyDataFile.setFileName(QString(".//historyIdentifyData//") + Global::historyVideoDate + QString(".txt"));

		}
	 
		
		QString curPath = QDir::currentPath();

		if (!Global::historyIdentifyDataFile.exists())
		{

			if (!Global::historyIdentifyDataFile.open(QIODevice::WriteOnly))
				qDebug("historyIdentifyDataFile open Failed");

			Global::historyIdentifyDataWS.setFloatingPointPrecision(QDataStream::SinglePrecision);
			Global::historyIdentifyDataWS.setDevice(&Global::historyIdentifyDataFile);

		}
		else
		{
			Global::historyIdentifyDataFile.remove();
			if (!Global::historyIdentifyDataFile.open(QIODevice::WriteOnly))
				qDebug("historyIdentifyDataFile open Failed");

			Global::historyIdentifyDataWS.setFloatingPointPrecision(QDataStream::SinglePrecision);
			Global::historyIdentifyDataWS.setDevice(&Global::historyIdentifyDataFile);

		}
		videoFileDate = Global::historyVideoDate;

	}
 

	//

//	QString logStr;

//	QTextStream logContentOut(&logFile);
	//
	int dataType = 0;
	QString raceIdStr;
	if (Global::session < 10)
	{
		if (Global::isHistoryVideo)
		{
			raceIdStr = Global::historyVideoDate + QString("0") + QString::number(Global::session);

		}
		else
		{
			raceIdStr = liveCurDate  + QString("0") + QString::number(Global::session);

		}
	}
	else
	{
		if (Global::isHistoryVideo)
		{
			raceIdStr = Global::historyVideoDate  + QString::number(Global::session);

		}
		else
		{
			raceIdStr = liveCurDate  + QString::number(Global::session);

		}
	 
	}

	//检测数据是否发生了变化
	isDataOutputNew(mDataOutput);


	Global::raceId = raceIdStr.toInt();

	//写入win
	if (dataOutput.changeStatus == WIN_CHANGED | dataOutput.changeStatus == WIN_PLA_CHANGED
		| dataOutput.changeStatus == WIN_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED)
	{

		for (int i = 0; i < dataOutput.horseNum; i++)
		{
			dataType = WINTYPE;

			//封装一个WIN
			TagWPDataInfo WPData;

			WPData.HorseID = dataOutput.mHorseInfo.horseID[i];
			// from 1 to 14 .
			WPData.HorseNO = i + 1;

			WPData.WinValue = dataOutput.WIN[i];

			WPData.RaceID = Global::raceId;
			//顺计时
			WPData.AtTime = Global::countRaceTime;


			if (dataType == 0)
			{
				qDebug("dataType = 0 ");
			}
			if (Global::isHistoryVideo)
			{
				//写文件
				Global::historyIdentifyDataWS << dataType << WPData.RaceID << WPData.HorseID << WPData.HorseNO
					<< WPData.WinValue << WPData.AtTime;
			}
			else
			{
				//写文件
				liveBackupDataStream<< dataType << WPData.RaceID << WPData.HorseID << WPData.HorseNO
					<< WPData.WinValue << WPData.AtTime;
			}
			

			/*
			logContentOut << QString("Type") << QString::number(dataType) << QString::number(WPData.RaceID) <<
				QString::number(WPData.HorseID) << QString::number(WPData.HorseNO) << QString::number(WPData.WinValue)
				<< QString::number(WPData.AtTime);
			*/
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

			if (WPData.HorseID > 10558 | WPData.HorseID < 0)
			{
				qDebug("HorseId > 10558 ");
			}
			//从1 到14 
			WPData.HorseNO = i + 1;

			WPData.WinValue = dataOutput.PLA[i];

			WPData.RaceID = Global::raceId;
			WPData.AtTime = Global::countRaceTime;

			if (dataType == 0)
			{
				qDebug("dataType = 0 ");
			}

			if (Global::isHistoryVideo)
			{
				//写文件
				Global::historyIdentifyDataWS << dataType << WPData.RaceID << WPData.HorseID
					<< WPData.HorseNO << WPData.WinValue << WPData.AtTime;
			}
			else
			{
				liveBackupDataStream << dataType << WPData.RaceID << WPData.HorseID
					<< WPData.HorseNO << WPData.WinValue << WPData.AtTime;
			}

			/*
			logContentOut << QString("Type") << QString::number(dataType) << QString::number(WPData.RaceID) <<
				QString::number(WPData.HorseID) << QString::number(WPData.HorseNO) << QString::number(WPData.WinValue)
				<< QString::number(WPData.AtTime);
			*/
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

				if (Global::isHistoryVideo)
				{
					//写文件
					Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
						<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				}
				else
				{
					liveBackupDataStream << dataType << QDataInfo.RaceID << QDataInfo.HorseID
						<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				}
			
				/*
				logContentOut << QString("Type") << QString::number(dataType) << QString::number(QDataInfo.RaceID) <<
					QString::number(QDataInfo.HorseID) << QString::number(QDataInfo.HorseNO) <<
					QString::number(QDataInfo.YNO)
					<< QString::number(QDataInfo.QinValue)
					<< QString::number(QDataInfo.AtTime);
				*/
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

				if (Global::isHistoryVideo)
				{
					//写文件
					Global::historyIdentifyDataWS << dataType << QDataInfo.RaceID << QDataInfo.HorseID
						<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				}
				else
				{
					liveBackupDataStream << dataType << QDataInfo.RaceID << QDataInfo.HorseID
						<< QDataInfo.HorseNO << QDataInfo.YNO << QDataInfo.QinValue << QDataInfo.AtTime;
				}
				/*
				logContentOut << QString("Type") << QString::number(dataType) << QString::number(QDataInfo.RaceID) <<
					QString::number(QDataInfo.HorseID) << QString::number(QDataInfo.HorseNO) <<
					QString::number(QDataInfo.YNO)
					<< QString::number(QDataInfo.QinValue)
					<< QString::number(QDataInfo.AtTime);
				 */

			}


		}


	}
	
	if (mDataOutput.changeStatus > 0 )
	{
		//更新 前一个结果
		priDataOutput = mDataOutput;

	}

	 
	
	//退回上一目录
	QDir::setCurrent("../");

}


/**
* @brief 判断数据是否为新数据 同时判断数据是否过大 如果变化范围超过此数值的原值的2/1
则认为是错误，丢弃。采用原有 识别结果。
*/
int OcrControl::isDataOutputNew(DataOutput &outputStruct)
{

	 

 
	int  WINChangedNum = 0;
	int  PLAChangedNum = 0;
	int  QINQPLCHangedNum = 0;
	// 判断WIN数据是否变化
	outputStruct.changeStatus = 0;


 
	for (int i = 0; i < outputStruct.horseNum; i++)
	{
		if (abs(outputStruct.WIN[i] - priDataOutput.WIN[i]) > 0.05)
		{
			//qDebug("WIN : i=%d , new is %f pri is %f ", i,  outputStruct.WIN[i], priDataOutput.WIN[i]);
			//outputStruct.changeStatus = WIN_CHANGED;
			WINChangedNum++;
			outputStruct.winChangedFlag[i] = true;

		}
		else
		{
			outputStruct.winChangedFlag[i] = false;
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
			outputStruct.plaChangedFlag[i] = true;
		}
		else
		{
			outputStruct.plaChangedFlag[i] = false;
		}
			 
	}
	 
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
				outputStruct.qplQinChangedFlag[i][j] = true;
			}
			else
			{
				outputStruct.qplQinChangedFlag[i][j] = false;
			}
				 
		}
	}
	 

  
	//数据超出本应有的范围，那么就不会发送此时的数据， 此时应该避免掉 比赛刚开始，有数据的时候，此时应该发送数据。
	//添加 Global::raceHasStarted 来控制 第一次，即 比赛未开始，那么可以发送的。
	// 如果场次号发生了变化
	 
		if (WINChangedNum > 0)
		{
			 
				outputStruct.changeStatus = WIN_CHANGED;
		}
		if (PLAChangedNum > 0)
		{
			 
				outputStruct.changeStatus = outputStruct.changeStatus | PLA_CHANGED;
		}
		if (QINQPLCHangedNum > 0)
		{
			 
				outputStruct.changeStatus = outputStruct.changeStatus | QIN_QPL_CHANGED;
		}
 
 
	return 1;
}
void OcrControl::updateAfterUserInput(DataOutput  output)
{
	//更新马信息
	/*
	for (int i = 0; i < horseNameEditList.size(); i++)
	{
		QString horseName;
		wchar_t *horseNameChar = new wchar_t[6];
		memset(horseNameChar, 0, 6 * sizeof(wchar_t));
		memcpy(horseNameChar, output.mHorseInfo.horseName[i], 4 * sizeof(wchar_t));

		horseName = horseName.fromWCharArray(horseNameChar);

		if (i < output.horseNum)
			horseNameEditList[i]->setText(horseName);
		else
		{
			indexLabelList[i]->setText(QString("0"));
		}
	 
	}
	*/
	for (int i = 0; i < winLableList.size(); i++)
	{

		winLableList[i]->setText(QString::number(output.WIN[i]));


		if (output.isWinPlaHasGroundColor[i][0] == true)
		{
			winLableList[i]->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
		}

	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		plaLableList[i]->setText(QString::number(output.PLA[i]));
		if (output.isWinPlaHasGroundColor[i][1] == true)
		{
			plaLableList[i]->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
		}

	}


	//更新qpl qin
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		for (int j = 0; j < list.size(); j++)
		{
			QLineEdit * label = list.at(j);
			label->setText(QString::number(output.QPL_QIN[i][j]));

			if (output.isQplQinHasGroundColor[i][j] == true)
			{
				label->setStyleSheet("QLineEdit{background: green;color: #FFFFFF}");
			}


		}
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe1;
	pe1.setColor(QPalette::WindowText, Qt::white);

	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		QLineEdit * label = list.at(i);

		label->setText(QString::number(i + 8));

		label->setStyleSheet("QLineEdit{background: gray;color: #FFFFFF}");


	}

	// 设置无数据区域 i 0-6 j 0-14 （0,1）（1,2）~（6,7）
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<QLineEdit*> list = qinList.at(i);
		QLineEdit * label = list.at(i + 1);

		//label->setPalette(pe1);
		label->setStyleSheet("QLineEdit{background: gray;color: #FFFFFF}");
		label->setText(QString::number(i + 1));
	}


	ui.userInputLabel->setText(QString("修改"));


}

/*
实时获取 63台check box 状态
*/
void OcrControl::is63TAICheckBoxStateChanged(bool)
{
	Global::is63TAIVideoData = ui.is63TAICheckBox->isChecked();
}