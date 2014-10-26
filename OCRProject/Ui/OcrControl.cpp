#include "OcrControl.h"
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
	
	bllRealTimeTrans->serverNo = 0;

	//第二条线路
	bllRealTimeTrans1 = new BllRealTimeTrans1();
	threadRealTime1 = new ThreadRealTime();
	//启动线程
	bllRealTimeTrans1->moveToThread(threadRealTime1);
	threadRealTime1->start();

	bllRealTimeTrans1->serverNo = 1 ;
	
	//第一条线路

	QObject::connect(bllRealTimeTrans, SIGNAL(statuChanged(QString)), this, SLOT(appendStatus(QString)));//网络状态反馈
	QObject::connect(this, SIGNAL(connectToServer(QString, qint32)), bllRealTimeTrans, SLOT(connectToHost(QString, qint32)));//连接
	QObject::connect(this, SIGNAL(disconnect()), bllRealTimeTrans, SLOT(disconnectToHost()));//断开
	QObject::connect(this, SIGNAL(login()), bllRealTimeTrans, SLOT(clientLogin()));//登录+
	

	 //第二条线路

	QObject::connect(bllRealTimeTrans1, SIGNAL(statuChanged(QString)), this, SLOT(appendStatus(QString)));//网络状态反馈
	QObject::connect(this, SIGNAL(connectToServer1(QString, qint32)), bllRealTimeTrans1, SLOT(connectToHost(QString, qint32)));//连接
	QObject::connect(this, SIGNAL(disconnect()), bllRealTimeTrans1, SLOT(disconnectToHost()));//断开
	QObject::connect(this, SIGNAL(login()), bllRealTimeTrans1, SLOT(clientLogin()));//登录+


	//QObject::connect(bllRealTimeTrans->mcsNetClient, SIGNAL(disConnectToHostSuccessSignal()), 
	//	this, SLOT(reConnect()));//断开重连


//	QObject::connect(bllRealTimeTrans1->mcsNetClient, SIGNAL(disConnectToHostSuccessSignal()),
//		this, SLOT(reConnect()));//断开重连



	//采集
	//QObject::connect(this, SIGNAL(startAcq()), Global::myIAcq, SLOT(read()));//开始采集
	//QObject::connect(this, SIGNAL(stopAcq()), Global::myIAcq,  SLOT(unLoadDevice()));//停止采集

	//算法
	bllDataIdentify = new BllDataIdentify();//数据识别
	threadDataIdentify = new ThreadDataIdentify();//数据识别线程
	
	bllDataIdentify->moveToThread(threadDataIdentify);


	//发送比赛时长 当前比赛时间+倒计时 atTime + raceTime  atTime 为CountRaceTime
	QObject::connect(bllDataIdentify, SIGNAL(submitRaceTimeSig(qint32)), bllRealTimeTrans, SLOT(submitRaceTime(qint32)));



	QObject::connect(bllDataIdentify, SIGNAL(readyRead(DataOutput, QByteArray, int, int)), bllRealTimeTrans, SLOT(submitRealData(DataOutput, QByteArray,int, int)));//开始发送
	
	//发送比赛时长 当前比赛时间+倒计时 atTime + raceTime  atTime 为CountRaceTime
	QObject::connect(bllDataIdentify, SIGNAL(submitRaceTimeSig(qint32)), bllRealTimeTrans1, SLOT(submitRaceTime(qint32)));



	QObject::connect(bllDataIdentify, SIGNAL(readyRead(DataOutput, QByteArray, int, int)), bllRealTimeTrans1, SLOT(submitRealData(DataOutput, QByteArray,int, int)));//开始发送
	


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

	 

	// 发送缓存区数据
	QObject::connect(bllDataIdentify, SIGNAL(sendBufferDataSig()), 
					 bllRealTimeTrans, SLOT(sendBufferDataToServer()) );

	QObject::connect(bllDataIdentify, SIGNAL(sendBufferDataSig()),
		bllRealTimeTrans1, SLOT(sendBufferDataToServer()));


	
	initUi();
	
	 

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
	QObject::connect(bllDataIdentify, SIGNAL(requestRaceIdSig(int)), bllRealTimeTrans, SLOT(requestRaceID(int)));

 
	// 连接 63台checkbox
	QObject::connect(ui.is63TAICheckBox, SIGNAL(clicked(bool)), this, SLOT(is63TAICheckBoxStateChanged(bool)));

	// 连接是否锁定改变按钮被按下checkbox
	QObject::connect(ui.lockCaliCheckBox, SIGNAL(clicked(bool)), this, SLOT(islockCaliCheckBoxStatusChanged(bool)));
	

	liveBackupDataFileCreated = false;
	
	 
	// 连接 63台checkbox
	QObject::connect(bllRealTimeTrans->mcsNetClient, SIGNAL(clientSocketStateSignal(QAbstractSocket::SocketState socketState)), this, \
		SLOT(getClientSocketState(QAbstractSocket::SocketState socketState)));

	QObject::connect(bllRealTimeTrans1->mcsNetClient, SIGNAL(clientSocketStateSignal(QAbstractSocket::SocketState socketState)), this, \
		SLOT(getClientSocketState(QAbstractSocket::SocketState socketState)));



	// 写数据文件

	logFile.setFileName("RDBSDataAfterChange.txt");
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
		qDebug("logFile open Failed \n");

 
	priSession = 0;

//	logContentOut.setDevice(&logFile);


	for (int i = 0; i < HORSENUMBER; i++)
	{
		mDataOutput.winCaliFlag[i] = false;
		mDataOutput.plaCaliFlag[i] = false;
	}

	for (int i = 0; i < QIN_QPL_ROW; i++)
	{
 
		for (int j = 0; j < QIN_QPL_COL; j++)
		{
			mDataOutput.qplQinCaliFlag[i][j]= false ;

		}
	}

	//按钮灰色

	ui.appendFileBtn->setEnabled(false);
	ui.videoStartFrameCountBtn->setEnabled(false);
	ui.loginBtn->setEnabled(false);

	ui.reconnectCheckBox->setEnabled(false);

	//从用户界面获取数据

	Global::serverIpAddr = QString("58.67.161.109");
	Global::serverPort = 9069;

	ui.svrIpAddrLineEdit->setText(Global::serverIpAddr);
	ui.svrPortLineEdit->setText(QString::number(Global::serverPort));
	


	Global::serverIpAddr1 = QString("120.24.103.168");
	Global::serverPort1 = 9069;



	//绑定combox 信号槽

	connect(ui.serverNameComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(upDateSvrLineEdit(QString)));
	
	
}

void OcrControl::upDateSvrLineEdit(QString serverName)
{


	//QString serverName;
	serverName = ui.serverNameComboBox->currentText();

	if (serverName == "服务器")
	{
		ui.svrPortLineEdit->setText(QString::number(Global::serverPort));
		ui.svrIpAddrLineEdit->setText((Global::serverIpAddr));

	
	}
	else if (serverName == "服务器1")
	{

		ui.svrPortLineEdit->setText(QString::number(Global::serverPort1));
		ui.svrIpAddrLineEdit->setText((Global::serverIpAddr1));

	}


}
OcrControl::~OcrControl()
{
	//停止实时提交
	if (threadRealTime->isRunning())
	{
		threadRealTime->quit();
	}
	//停止实时提交
	if (threadRealTime1->isRunning())
	{
		threadRealTime1->quit();
	}

	//删除实时网络传输业务
	if (bllRealTimeTrans !=0 )
		delete bllRealTimeTrans;

	//删除实时网络传输业务
	if (bllRealTimeTrans1 != 0 )
		delete bllRealTimeTrans1 ;


	//删除网络传输线程
	if (threadRealTime != 0)
	{
		//threadRealTime->terminate();
		threadRealTime->wait();
		delete threadRealTime;
	}
	//删除网络传输线程
	if (threadRealTime1 != 0)
	{
		//threadRealTime->terminate();
		threadRealTime1->wait();
		delete threadRealTime1;
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
void OcrControl::getClientSocketState(QAbstractSocket::SocketState socketState)
{
	
	switch (socketState)
	{
	case QAbstractSocket::UnconnectedState:
		qDebug() << "UnconnectedState:The socket is not connected.";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：未连上.")).arg(clientName), QString(tr("未连上远程计算机IP:%1,port:%2,UnconnectedState:The socket is not connected.")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::HostLookupState:
		qDebug() << "HostLookupState:The socket is performing a host name lookup.";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：正在查询主机中......")).arg(clientName), QString(tr("正在查询远程计算机IP:%1,port:%2,HostLookupState:The socket is performing a host name lookup.")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::ConnectingState:
		qDebug() << "ConnectingState:The socket has started establishing a connection.";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：正在创建网络连接......")).arg(clientName), QString(tr("正在创建网络连接，远程计算机IP:%1,port:%2,ConnectingState:The socket has started establishing a connection.")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::ConnectedState:
		qDebug() << "ConnectedState:A connection is established.";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：TCP连接创建成功.")).arg(clientName), QString(tr("网络已经连接成功，远程计算机IP:%1,port:%2,ConnectedState:A connection is established.")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::BoundState:
		qDebug() << "BoundState:The socket is bound to an address and port (for servers).";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：服务端已绑定TCP Socket的IP和端口.")).arg(clientName), QString(tr("服务端已经绑定IP和端口，远程计算机IP:%1,port:%2,BoundState:The socket is bound to an address and port (for servers).")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::ClosingState:
		qDebug() << "ClosingState:The socket is about to close (data may still be waiting to be written).";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：即将关闭状态.")).arg(clientName), QString(tr("网络即将关闭,远程计算机IP:%1,port:%2,ClosingState:The socket is about to close (data may still be waiting to be written).")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	case QAbstractSocket::ListeningState:
		qDebug() << "ListeningState:For internal use only.";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：侦听状态")).arg(clientName), QString(tr("网络侦听状态,远程计算机IP:%1,port:%2")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
		break;
	default:
		qDebug() << "网络其他状态";
		//Global::systemLog->append(QString(tr("%1网络客户端状态：其他状态.")).arg(clientName), QString(tr("网络其他状态,远程计算机IP:%1,port:%2")).arg(remoteHostIp).arg(remoteHostPort), SystemLog::INFO);
	}
}

/**
* @brief 连接服务器
*/
void OcrControl::on_connectBtn_clicked()
{
	emit connectToServer(Global::serverIpAddr , Global::serverPort);

	emit connectToServer1(Global::serverIpAddr1,Global::serverPort1);

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

	QString a = QString("AV");

	Global::liveCardVideoSource = ui.sdkCardVideoSourceComboBox->currentText();

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

		Global::myIAcq->init();
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
	
	/*
	threadDataIdentify->quit();// 退出 	 
	while (threadDataIdentify->isRunning())
	{
		Sleep(1);
	}
	
	threadDataIdentify->start();//开始识别 

	*/

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

/*
	录入用户输入的服务器设置

*/

void OcrControl::on_svrConfigSaveBtn_clicked()
{

	



	QString serverName; 
	serverName = ui.serverNameComboBox->currentText();

	if (serverName == "服务器")
	{

		QString svrPortStr = ui.svrPortLineEdit->text();

		Global::serverIpAddr = ui.svrIpAddrLineEdit->text();
		Global::serverPort = svrPortStr.toInt();


	}
	else if (serverName == "服务器1")
	{

		QString svrPortStr = ui.svrPortLineEdit->text();

		Global::serverIpAddr1 = ui.svrIpAddrLineEdit->text();
		Global::serverPort1 = svrPortStr.toInt();


	}

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

			//myImage = myImage.scaled(518, 432);
		}
		//else 
			//myImage = myImage.scaled(518, 432);

		
	//	myImage.scaled(ui.imageLbl->width(), ui.imageLbl->height());

		myImage = myImage.scaled(518 * 0.85, 432 * 0.85);
		pixmap = pixmap.fromImage(myImage);
		ui.imageLbl->setPixmap(pixmap);
		delete[] buffer;
		 
		QPalette pe;
		pe.setColor(QPalette::WindowText, Qt::white);
		ui.adTimeLbl->setText("广告");
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
	//将标记 写入 output
	

	if (Global::isHistoryVideo)
	{

		//写入系统日志
		Global::systemLog->append(QString(tr("历史数据 ")), QString(""),
			SystemLog::INFO_TYPE);

		//如果QPL QIN发生切换那么，就不要在继续保持数值了
		if (mDataOutput.isQPL == output.isQPL)
		{
			if (Global::islockCali)
			{
				for (int i = 0; i < HORSENUMBER; i++)
				{

					output.winCaliFlag[i] = mDataOutput.winCaliFlag[i];
					output.plaCaliFlag[i] = mDataOutput.plaCaliFlag[i];

					if (mDataOutput.winCaliFlag[i])
					{
						output.WIN[i] = mDataOutput.WIN[i];
					}
					if (mDataOutput.plaCaliFlag[i])
					{
						output.PLA[i] = mDataOutput.PLA[i];
					}
				}

				for (int i = 0; i < qinList.size(); i++)
				{

					QList<MLineEdit*> list = qinList.at(i);
					for (int j = 0; j < list.size(); j++)
					{
						output.qplQinCaliFlag[i][j] = mDataOutput.qplQinCaliFlag[i][j];

						if (output.qplQinCaliFlag[i][j])
						{
							output.QPL[i][j] = mDataOutput.QPL[i][j];
							output.QIN[i][j] = mDataOutput.QIN[i][j];

						}

					}
				}


			}

		}

	
	}
	

	mDataOutput = output;
	 
	//else
	//比较是否发生变化 无论直播，还是历史
	{
		isDataOutputNew(mDataOutput,priDataOutputForShow);
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

			//myImage = myImage.scaled(518, 432);
		}
	//	else
		//	myImage = myImage.scaled(518, 432);
		
		

		//myImage.scaled(ui.imageLbl->width(), ui.imageLbl->height());
		myImage = myImage.scaled(518*0.85, 432*0.85);

		pixmap = pixmap.fromImage(myImage);
		ui.imageLbl->setPixmap(pixmap);
		delete[] buffer;
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::white);
	/*
	//写入系统日志
	Global::systemLog->append(QString(tr("更新ui数据 1 ")), tr(" updateData"),
		SystemLog::INFO_TYPE);
	*/
	if (output.haveDataFlag)
	{
		/*
		//写入系统日志
		Global::systemLog->append(QString(tr("更新ui数据 2 ")), tr(" updateData"),
			SystemLog::INFO_TYPE);
		*/
		ui.adTimeLbl->setText("比赛");
		ui.adTimeLbl->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
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

 

	//赋值
	if (mDataOutput.changeStatus > 0)
	{
		//更新 前一个结果
		priDataOutputForShow = mDataOutput;

	}
	



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
	//更新马的名字		
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
			horseNameEditList[i]->setStyleSheet("MLineEdit{background:rgb(62,120,56);color: rgb(255,255,255)}");
		}
			
		else
		{
			indexLabelList[i]->setText(QString("0"));
		}
	}


	//如果马匹数量小于14 ，那么 剩下的地方显示 0
	for (int i = horseNameEditList.size(); i < HORSENUMBER;i ++)
	{
		
		horseNameEditList[i]->setText(QString(""));
		
	}

	for (int i = 0; i < winLableList.size(); i++)
	{

		winLableList[i]->setText(QString::number(output.WIN[i]));

		/*
		//写入系统日志
		Global::systemLog->append(QString(tr("WIN ")), QString::number(output.WIN[i]),
			SystemLog::INFO_TYPE);
		*/
		if (output.isWinPlaHasGroundColor[i][0] )
		{
		//	winLableList[i]->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
		}

		if (output.winChangedFlag[i]  )
		{
			winLableList[i]->setStyleSheet("MLineEdit{background: rgb(42,81,37);color: rgb(255,255,255)}");
		}
		else
		{
			 
			winLableList[i]->setStyleSheet("MLineEdit{background:rgb(62,120,56);color: rgb(255,255,255)}");
	
		}

	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		plaLableList[i]->setText(QString::number(output.PLA[i]));
	
		if (output.plaChangedFlag[i] )
		{
			plaLableList[i]->setStyleSheet("MLineEdit{background: rgb(42,81,37);color: rgb(255,255,255)}");
		}
		else
		{

			plaLableList[i]->setStyleSheet("MLineEdit{background: rgb(62,120,56);color: rgb(255,255,255)}");
		}
		if (output.isWinPlaHasGroundColor[i][1] )
		{
	//		plaLableList[i]->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
		}

	}
	/*
	//赋值 
	if (mDataOutput.changeStatus > 0)
	{
		//更新 前一个结果
		priDataOutput = mDataOutput;

	}
	*/


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
		QList<MLineEdit*> list = qinList.at(i);
		for (int j = 0; j <  list.size(); j++)
		{
			MLineEdit * label = list.at(j);
			if (output.isQPL)
			{
				label->setText(QString::number(output.QPL[i][j]));

			}
			else
			{
				label->setText(QString::number(output.QIN[i][j]));

			}
			

			if (output.qplQinChangedFlag[i][j])
			{
				label->setStyleSheet("MLineEdit{background:rgb(42,81,37);color: rgb(255,255,255)}");
			}
			else
			{
				
				label->setStyleSheet("MLineEdit{background: rgb(62,120,56);color: rgb(255,255,255)}");
			}
			//如果是0 ，那么 使用黑底白色
			if (output.QPL[i][j] == 0 | output.QIN[i][j] == 0 )
			{
				label->setStyleSheet("MLineEdit{background:rgb(42,81,37);color: rgb(255,255,255)}");
			}

			if (output.isQplQinHasGroundColor[i][j])
			{
			//	label->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
			}

		}
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe1;
	 pe1.setColor(QPalette::WindowText, Qt::white);
	  
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<MLineEdit*> list = qinList.at(i);
		MLineEdit * label = list.at(i);		
		 
		label->setText(QString::number(i + 8));

		label->setStyleSheet("MLineEdit{background: gray;color: #FFFFFF}");
		 

	}

	// 设置无数据区域 i 0-6 j 0-14 （0,1）（1,2）~（6,7）
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<MLineEdit*> list = qinList.at(i);
		MLineEdit * label = list.at(i + 1);
		
		//label->setPalette(pe1);
		label->setStyleSheet("MLineEdit{background: gray;color: #FFFFFF}");
		//label->setText(QString::number(i + 1));

		label->setText(QString::number(Global::timerCount));

	}


}


void OcrControl::reConnect()
{
	if (ui.reconnectCheckBox->isChecked())
	{
		qDebug("识别端：重连服务器 \n");
		emit connectToServer(Global::serverIpAddr,Global::serverPort);

		emit connectToServer1(Global::serverIpAddr1, Global::serverPort1);

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

void OcrControl::on_caliSessionBtn_clicked()
{

	QString sessionStr;
	sessionStr = ui.sessionLineEdit->text();
	Global::session = sessionStr.toInt();

	ui.sessionLineEdit->setText(QString::number(Global::session));//更新全局场次号


	if (Global::session == sessionStr.toInt())
	{
		Global::isSessioncalibrated = false ;
		Global::isSessionChanged	= false ;
	}
	else
	{
		Global::isSessioncalibrated = true ;
		Global::isSessionChanged	= true ;
	}





}


/**
* @brief 校正场次号 倒计时
*/

void OcrControl::on_caliCountDownBtn_clicked()
{


	QString raceTimeStr;
	raceTimeStr = ui.raceTimeLineEdit->text();

	//更新全局时间
	Global::raceTime = raceTimeStr.toInt();


	//
	QString countRaceTime;
	countRaceTime = ui.CountRaceTimeLineEdit->text();

	Global::countRaceTime = countRaceTime.toInt();

	Global::isCountTimeCalied = true;
	/*
	//更新顺计时
	QString raceTimeStr;
	raceTimeStr = ui.CountRaceTimeLineEdit->text();

	//更新全局时间
	Global::raceTime = raceTimeStr.toInt();

	*/



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


/*
历史视频快进 1 s
*/
void OcrControl::on_advance1SecBtn_clicked()
{
	Global::frameAccValue = 1;
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
 
//输入用户校正数据，主要是win pla ，qin qpl
void OcrControl::on_inputUserDataBtn_clicked()
{
	
 
 
	for (int i = 0; i < winLableList.size(); i++)
	{

		QString win;
		win = winLableList[i]->text();

		//如果锁定了校正，那么校正值不会发生改变

		if (Global::islockCali)
		{
			if (!mDataOutput.winCaliFlag[i])
			{
				//只有没被矫正过的才能赋值
				if (abs(mDataOutput.WIN[i] - win.toFloat()) > 0.01)
				{
					mDataOutput.WIN[i] = win.toFloat();
					mDataOutput.winCaliFlag[i] = true;
				}
				else
				{
					mDataOutput.winCaliFlag[i] = false;
				}
			}
		 
		}
		else
		{
			if (abs(mDataOutput.WIN[i] - win.toFloat()) > 0.01)
			{
				mDataOutput.WIN[i] = win.toFloat();
			}
			mDataOutput.winCaliFlag[i] = false;
		}
		/*
		else
		{
			//只有没被矫正过的才能赋值
			if (abs(mDataOutput.WIN[i] - win.toFloat()) > 0.01 )
			{
				mDataOutput.WIN[i] = win.toFloat();
				mDataOutput.winCaliFlag[i] = true;
			}
			else
			{
				mDataOutput.winCaliFlag[i] = false;
			}
		}
		*/
	


		 
	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		QString pla;
		pla = plaLableList[i]->text();
 
	  
		//如果锁定了校正，那么校正值不会发生改变

		if (Global::islockCali)
		{
			if (!mDataOutput.plaCaliFlag[i])
			{
				if (abs(mDataOutput.PLA[i] - pla.toFloat()) > 0.01)
				{
					mDataOutput.PLA[i] = pla.toFloat();

				}
			}
		}
		else
		{
			//只有没被矫正过的才能赋值
			if (abs(mDataOutput.PLA[i] - pla.toFloat()) > 0.01 )
			{
				mDataOutput.PLA[i] = pla.toFloat();
				mDataOutput.plaCaliFlag[i] = true;
			}
			else
			{
				mDataOutput.plaCaliFlag[i] = false;
			}
		}

	
	}

	for (int i = 0; i < qinList.size(); i++)
	{
		QString qplQin;
		QList<MLineEdit*> list = qinList.at(i);
		for (int j = 0; j < list.size(); j++)
		{
			MLineEdit * label = list.at(j);
			 
			qplQin = label->text();

			if (Global::islockCali)
			{
				if (!mDataOutput.qplQinCaliFlag[i][j])
				{
					if (mDataOutput.isQPL)
					{
						if (abs(mDataOutput.QPL[i][j] - qplQin.toFloat()) > 0.01)
						{
							mDataOutput.QPL[i][j] = qplQin.toFloat();

						}
					}
					else
					{
						if (abs(mDataOutput.QIN[i][j] - qplQin.toFloat()) > 0.01)
						{
							mDataOutput.QIN[i][j] = qplQin.toFloat();

						}
					}
					
				}
			}
			else
			{
				if (mDataOutput.isQPL)
				{
					if (abs(mDataOutput.QPL[i][j] - qplQin.toFloat()) > 0.01)
					{
						mDataOutput.QPL[i][j] = qplQin.toFloat();
						mDataOutput.qplQinCaliFlag[i][j] = true;
					}
					else
					{
						mDataOutput.qplQinCaliFlag[i][j] = false;
					}
				}
				else
				{
					if (abs(mDataOutput.QIN[i][j] - qplQin.toFloat()) > 0.01)
					{
						mDataOutput.QIN[i][j] = qplQin.toFloat();
						mDataOutput.qplQinCaliFlag[i][j] = true;
					}
					else
					{
						mDataOutput.qplQinCaliFlag[i][j] = false;
					}
					 
				}

				 
			}
		
			 
			 
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

	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString currentDate = currentDateTime.toString("yyyy-MM-dd hh:mm:ss ddd");

	QTextStream logContentOut(&logFile);
	 

	//历史视频 并且发现了新的日期，那么这个时候要创建新的文件
	// 如果发现新的文件已经存在那么首先删除
	if (videoFileDate != Global::historyVideoDate & Global::isHistoryVideo )
	{


		Global::historyIdentifyDataFile.close();
		// 设置目录
		QString runPath = QCoreApplication::applicationDirPath();
	
		QDir::setCurrent(runPath);
		//退到上一层目录
		QDir::setCurrent("../");

		QDir::setCurrent("../");


		QDir::setCurrent(".//OCRProject//");
		// 写数据文件
		//如果是直播的时候中断 ，那么将数据写入本地文档，做备份，待比赛结束，将数据导入服务器
		
		Global::historyIdentifyDataFile.setFileName(QString(".//historyIdentifyData//") + Global::historyVideoDate + QString(".txt"));

		// 写 赛程文件 
		raceFile.setFileName(QString(".//historyIdentifyData//") + Global::historyVideoDate + QString("RaceInfo.txt")) ;
		
		QString curPath = QDir::currentPath();
		//如果文件不存在
		//if ( !Global::historyIdentifyDataFile.exists() )
		{

			if (!Global::historyIdentifyDataFile.open(QIODevice::WriteOnly|QIODevice::Append))
				qDebug("historyIdentifyDataFile open Failed");

			Global::historyIdentifyDataWS.setFloatingPointPrecision(QDataStream::SinglePrecision);
			Global::historyIdentifyDataWS.setDevice(&Global::historyIdentifyDataFile);

			appendStatus(QString("创建历史数据文件成功 文件名为") + Global::historyIdentifyDataFile.fileName() + currentDate);
		}
	 
		// 赛程文件信息写入

	//	if (!raceFile.exists())
		{

			if (!raceFile.open(QIODevice::WriteOnly | QIODevice::Append))
				qDebug("historyIdentifyDataFile open Failed");

			raceDataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			raceDataStream.setDevice(&raceFile);

		}
	 
		videoFileDate = Global::historyVideoDate;

	}
 
  //创建 raceId 日期+场次号 20100701 01 
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


	//检测数据是否发生了变化  结果数据被保存在了 OcrControl 这个类的成员变量里面
	isDataOutputNew(dataOutput,priDataOutputForWrite);

	// 检查是否 场次号发生了变化
	//一直写入
   //	if (Global::session != priSession  & priSession > 0)
	if (priSession > 0 )	
	{
 
		RACEINFO mRaceInfo;

		QByteArray raceDate;
		raceDate = Global::historyVideoDate.toLatin1();

		mRaceInfo.RaceID = 0;
		mRaceInfo.RaceNO = priSession;
		 
		memcpy(mRaceInfo.RaceDate, raceDate.data(), 8);

		QTime curTime = QTime::currentTime();
		QString curTimeStr = curTime.toString("hh:mm:ss");

		QByteArray curTimeBa;
		curTimeBa = curTimeStr.toLatin1();

		memcpy(mRaceInfo.RaceTime, curTimeBa.data(), 8);

		mRaceInfo.CountTime = priCountRaceTime;
	
		mRaceInfo.HorseCount = dataOutput.horseNum;

		//修改日期格式
		QString historyDate;

		historyDate = Global::historyVideoDate.mid(0,4);
		historyDate += QString("-"); 
		historyDate += Global::historyVideoDate.mid(4, 2);

		historyDate += QString("-");
		historyDate += Global::historyVideoDate.mid(6, 2);

		//写入文件
		raceDataStream << mRaceInfo.RaceID << mRaceInfo.RaceNO << historyDate << curTimeStr
					   << mRaceInfo.CountTime << mRaceInfo.HorseCount ;
	}
	 

	
	if (dataOutput.changeStatus > 0)
	{
		//更新 前一个结果
		priDataOutputForWrite = mDataOutput;

	}
	

	Global::raceId = raceIdStr.toInt();

	priSession = Global::session ;
	priCountRaceTime = Global::countRaceTime;
	//检查数据是否发生变化，并写入
	if (dataOutput.changeStatus == WIN_CHANGED | dataOutput.changeStatus == WIN_PLA_CHANGED
		| dataOutput.changeStatus == WIN_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED)
	{
		appendStatus(QString("输入WIN 数据") + currentDate );
		for (int i = 0; i < dataOutput.horseNum; i++)
		{
			dataType = WINTYPE;			
			//封装一个WIN
			TagWPDataInfo WPData;

			WPData.HorseID = dataOutput.mHorseInfo.horseID[i];
			// from 1 to 14 .
			WPData.HorseNO = i + 1;

			WPData.WinValue = dataOutput.WIN[i];

			WPData.RaceID = dataOutput.session ;
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
			
			logContentOut << QString("T_") << QString("WN") << QString::number(dataType)
				<< QString("RD") << QString::number(WPData.RaceID) << QString("HD") <<
				QString::number(WPData.HorseID) << QString("HN") << QString::number(WPData.HorseNO)
				<< QString("WV") << QString::number(WPData.WinValue) << QString("AT")
				<< QString::number(WPData.AtTime);

		}


	}
	// 写入pla
	if (dataOutput.changeStatus == WIN_PLA_CHANGED | dataOutput.changeStatus == PLA_CHANGED
		| dataOutput.changeStatus == PLA_QIN_QPL_CHANGED | dataOutput.changeStatus == WIN_PLA_QIN_QPL_CHANGED
		)
	{
		appendStatus(QString("输入 PLA 数据") + currentDate);
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

			WPData.RaceID = dataOutput.session;
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

			logContentOut << QString("T_") << QString("PL") << QString::number(dataType)
				<< QString("RD") << QString::number(WPData.RaceID) << QString("HD") <<
				QString::number(WPData.HorseID) << QString("HN") << QString::number(WPData.HorseNO)
				<< QString("WV") << QString::number(WPData.WinValue) << QString("AT")
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
			appendStatus(QString("输入 QPL 数据") + currentDate );
		}
		else
		{
			appendStatus(QString("输入 QIN 数据") + currentDate );
			dataType = QINTYPE;
		}
			

		for (int i = 1; i <= dataOutput.horseNum; i++)
		{

			for (int j = 1; j < i; j++)
			{

				//封装一个 QIN QPL
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = dataOutput.session;//所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (j <= 7) // 正表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[j - 1][i];
					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[j - 1][i];

					}
					
				}
				else //补充图表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[i - 8][j - 8];
					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[i - 8][j - 8];

					}
					
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
				logContentOut << QString("T_") << QString("Q") << QString::number(dataType)
					<< QString("RD") << QString::number(QDataInfo.RaceID) << QString("HD")
					<< QString::number(QDataInfo.HorseID) << QString("HN") << QString::number(QDataInfo.HorseNO)
					<< QString("YN") << QString::number(QDataInfo.YNO)
					<< QString("V") << QString::number(QDataInfo.QinValue) << QString("AT")
					<< QString::number(QDataInfo.AtTime);
			}

			for (int j = i + 1; j <= dataOutput.horseNum; /* HORSENUMBER_1; */ j++)
			{

				//封装一个WIN
				TagQDataInfo QDataInfo;
				QDataInfo.RaceID = dataOutput.session; //所属赛事ID
				QDataInfo.HorseID = dataOutput.mHorseInfo.horseID[i - 1];//马的唯一编号可关联马信息表
				QDataInfo.HorseNO = i;//本场比赛中马的序号，比如第3号，1-13
				QDataInfo.YNO = j;//在Y轴上的第几号，跟它组合得出的数据 2-14
				QDataInfo.AtTime = Global::countRaceTime;
				if (i <= 7) // 正表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[i - 1][j];

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[i - 1][j];

					}
				}
				else //补充图表
				{
					if (dataOutput.isQPL)
					{
						QDataInfo.QinValue = dataOutput.QPL[j - 8][i - 8];

					}
					else
					{
						QDataInfo.QinValue = dataOutput.QIN[j - 8][i - 8];

					}
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
				logContentOut << QString("T_") << QString("Q") << QString::number(dataType)
					<< QString("RD") << QString::number(QDataInfo.RaceID) << QString("HD")
					<< QString::number(QDataInfo.HorseID) << QString("HN") << QString::number(QDataInfo.HorseNO)
					<< QString("YN") << QString::number(QDataInfo.YNO)
					<< QString("V") << QString::number(QDataInfo.QinValue) << QString("AT")
					<< QString::number(QDataInfo.AtTime);

			}


		}


	}
	 
	//退回到exe路径
	QString runPath = QCoreApplication::applicationDirPath();

	QDir::setCurrent(runPath);

 

}


/**
* @brief 判断数据是否为新数据 同时判断数据是否过大 如果变化范围超过此数值的原值的2/1
则认为是错误，丢弃。采用原有 识别结果。
*/
int OcrControl::isDataOutputNew(DataOutput &outputStruct, DataOutput &priOutputStruct)
{
	 
	int  WINChangedNum = 0;
	int  PLAChangedNum = 0;
	int  QINQPLCHangedNum = 0;
	// 判断WIN数据是否变化
	outputStruct.changeStatus = 0;


 
	for (int i = 0; i < outputStruct.horseNum; i++)
	{
		if (abs(outputStruct.WIN[i] - priOutputStruct.WIN[i]) > 0.05)
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
		if (abs(outputStruct.PLA[i] - priOutputStruct.PLA[i]) >  0.05)
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

			if (abs(outputStruct.QPL[i][j] - priOutputStruct.QPL[i][j]) > 0.05
				| abs(outputStruct.QIN[i][j] - priOutputStruct.QIN[i][j]) > 0.05
			)
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

void OcrControl::updateAfterUserInput(DataOutput  &output)
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
			winLableList[i]->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
		}

	}
	for (int i = 0; i < plaLableList.size(); i++)
	{

		plaLableList[i]->setText(QString::number(output.PLA[i]));
		if (output.isWinPlaHasGroundColor[i][1] == true)
		{
			plaLableList[i]->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
		}

	}


	//更新qpl qin
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<MLineEdit*> list = qinList.at(i);
		for (int j = 0; j < list.size(); j++)
		{
			MLineEdit * label = list.at(j);

			if (output.isQPL)
			{
				label->setText(QString::number(output.QPL[i][j]));

			}
			else
			{
				label->setText(QString::number(output.QIN[i][j]));

			}

			if (output.isQplQinHasGroundColor[i][j] == true)
			{
				label->setStyleSheet("MLineEdit{background: green;color: #FFFFFF}");
			}


		}
	}
	// 设置无数据区域 i 0-6 j 0-14 （0,0）（1,1）~（6,6）
	QPalette pe1;
	pe1.setColor(QPalette::WindowText, Qt::white);

	for (int i = 0; i < qinList.size(); i++)
	{
		QList<MLineEdit*> list = qinList.at(i);
		MLineEdit * label = list.at(i);

		label->setText(QString::number(i + 8));

		label->setStyleSheet("MLineEdit{background: gray;color: #FFFFFF}");


	}

	// 设置无数据区域 i 0-6 j 0-14 （0,1）（1,2）~（6,7）
	for (int i = 0; i < qinList.size(); i++)
	{
		QList<MLineEdit*> list = qinList.at(i);
		MLineEdit * label = list.at(i + 1);

		//label->setPalette(pe1);
		label->setStyleSheet("MLineEdit{background: gray;color: #FFFFFF}");
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


/*
实时获取 是否锁定修改 check box 状态
*/
void OcrControl::islockCaliCheckBoxStatusChanged(bool)
{
	Global::islockCali = ui.lockCaliCheckBox->isChecked();
}



/*
初始化用户界面
*/
void OcrControl::initUi()
{
	int labelHeight = 18;

	int labelWidth = 40;


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

		MLineEdit* indexLbel_3 = new MLineEdit(frame);
		
		indexLbel_3->setObjectName(QStringLiteral("indexLbel_3"));
		QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(indexLbel_3->sizePolicy().hasHeightForWidth());
		indexLbel_3->setSizePolicy(sizePolicy);
		indexLbel_3->setAlignment(Qt::AlignLeft);

		indexLbel_3->setMinimumSize(QSize(labelWidth, labelHeight));
		indexLbel_3->setMaximumSize(QSize(labelWidth, labelHeight));
		indexLabelList.append(indexLbel_3);
		horizontalLayout_2->addWidget(indexLbel_3);

		MLineEdit* horseNameEdit_3 = new MLineEdit(frame);
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

		MLineEdit* winLbl_3 = new MLineEdit(frame);
		winLbl_3->setObjectName(QStringLiteral("winLbl_3"));
		sizePolicy.setHeightForWidth(winLbl_3->sizePolicy().hasHeightForWidth());
		winLbl_3->setSizePolicy(sizePolicy);
		QFont font1;
		font1.setPointSize(12);
		winLbl_3->setFont(font1);
		winLbl_3->setAlignment(Qt::AlignLeft);
		winLbl_3->setMinimumSize(QSize(labelWidth, labelHeight));
		winLbl_3->setMaximumSize(QSize(labelWidth, labelHeight));
		winLableList.append(winLbl_3);
		horizontalLayout_2->addWidget(winLbl_3);

		MLineEdit* PLALbl_3 = new MLineEdit(frame);
		PLALbl_3->setObjectName(QStringLiteral("PLALbl_3"));
		sizePolicy.setHeightForWidth(PLALbl_3->sizePolicy().hasHeightForWidth());
		PLALbl_3->setSizePolicy(sizePolicy);
		PLALbl_3->setFont(font1);
		PLALbl_3->setAlignment(Qt::AlignLeft);

		PLALbl_3->setMinimumSize(QSize(labelWidth, labelHeight));
		PLALbl_3->setMaximumSize(QSize(labelWidth, labelHeight));
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


	labelHeight = 14 ;
	for (int j = 0; j < 7; j++)
	{
		QList<MLineEdit*> list;
		QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
		horizontalLayout_3->setSpacing(0);
		horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));

		horizontalLayout_3->setContentsMargins(1, 1, 1, 1);

		MLineEdit* qinLbl_1 = new MLineEdit(ui.qinFrame);
		qinLbl_1->setObjectName(QStringLiteral("qinLbl_1"));
		QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(qinLbl_1->sizePolicy().hasHeightForWidth());

		qinLbl_1->setSizePolicy(sizePolicy2);

		qinLbl_1->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_1->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_1->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));

		list.append(qinLbl_1);
		horizontalLayout_3->addWidget(qinLbl_1);

		MLineEdit* qinLbl_2 = new MLineEdit(ui.qinFrame);
		qinLbl_2->setObjectName(QStringLiteral("qinLbl_2"));
		sizePolicy2.setHeightForWidth(qinLbl_2->sizePolicy().hasHeightForWidth());
		qinLbl_2->setSizePolicy(sizePolicy2);
		qinLbl_2->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_2->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_2->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_2);
		horizontalLayout_3->addWidget(qinLbl_2);

		MLineEdit* qinLbl_3 = new MLineEdit(ui.qinFrame);
		qinLbl_3->setObjectName(QStringLiteral("qinLbl_3"));
		sizePolicy2.setHeightForWidth(qinLbl_3->sizePolicy().hasHeightForWidth());
		qinLbl_3->setSizePolicy(sizePolicy2);

		qinLbl_3->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_3->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_3->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_3);
		horizontalLayout_3->addWidget(qinLbl_3);

		MLineEdit* qinLbl_4 = new MLineEdit(ui.qinFrame);
		qinLbl_4->setObjectName(QStringLiteral("qinLbl_4"));
		sizePolicy2.setHeightForWidth(qinLbl_4->sizePolicy().hasHeightForWidth());
		qinLbl_4->setSizePolicy(sizePolicy2);
		qinLbl_4->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_4->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_4->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_4);
		horizontalLayout_3->addWidget(qinLbl_4);

		MLineEdit* qinLbl_5 = new MLineEdit(ui.qinFrame);
		qinLbl_5->setObjectName(QStringLiteral("qinLbl_5"));
		sizePolicy2.setHeightForWidth(qinLbl_5->sizePolicy().hasHeightForWidth());
		qinLbl_5->setSizePolicy(sizePolicy2);
		qinLbl_5->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_5->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_5->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_5);
		horizontalLayout_3->addWidget(qinLbl_5);

		MLineEdit* qinLbl_6 = new MLineEdit(ui.qinFrame);
		qinLbl_6->setObjectName(QStringLiteral("qinLbl_6"));
		sizePolicy2.setHeightForWidth(qinLbl_6->sizePolicy().hasHeightForWidth());
		qinLbl_6->setSizePolicy(sizePolicy2);
		qinLbl_6->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_6->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_6->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_6);
		horizontalLayout_3->addWidget(qinLbl_6);

		MLineEdit* qinLbl_7 = new MLineEdit(ui.qinFrame);
		qinLbl_7->setObjectName(QStringLiteral("qinLbl_7"));
		sizePolicy2.setHeightForWidth(qinLbl_7->sizePolicy().hasHeightForWidth());
		qinLbl_7->setSizePolicy(sizePolicy2);
		qinLbl_7->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_7->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_7->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_7);
		horizontalLayout_3->addWidget(qinLbl_7);

		MLineEdit* qinLbl_8 = new MLineEdit(ui.qinFrame);
		qinLbl_8->setObjectName(QStringLiteral("qinLbl_8"));
		sizePolicy2.setHeightForWidth(qinLbl_8->sizePolicy().hasHeightForWidth());
		qinLbl_8->setSizePolicy(sizePolicy2);
		qinLbl_8->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_8->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_8->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_8);
		horizontalLayout_3->addWidget(qinLbl_8);

		MLineEdit* qinLbl_9 = new MLineEdit(ui.qinFrame);
		qinLbl_9->setObjectName(QStringLiteral("qinLbl_9"));
		sizePolicy2.setHeightForWidth(qinLbl_9->sizePolicy().hasHeightForWidth());
		qinLbl_9->setSizePolicy(sizePolicy2);
		qinLbl_9->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_9->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_9->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_9);
		horizontalLayout_3->addWidget(qinLbl_9);

		MLineEdit* qinLbl_10 = new MLineEdit(ui.qinFrame);
		qinLbl_10->setObjectName(QStringLiteral("qinLbl_10"));
		sizePolicy2.setHeightForWidth(qinLbl_10->sizePolicy().hasHeightForWidth());
		qinLbl_10->setSizePolicy(sizePolicy2);
		qinLbl_10->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_10->setMaximumSize(QSize(labelWidth, labelHeight));
		//	qinLbl_10->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_10);
		horizontalLayout_3->addWidget(qinLbl_10);

		MLineEdit* qinLbl_11 = new MLineEdit(ui.qinFrame);
		qinLbl_11->setObjectName(QStringLiteral("qinLbl_11"));
		sizePolicy2.setHeightForWidth(qinLbl_11->sizePolicy().hasHeightForWidth());
		qinLbl_11->setSizePolicy(sizePolicy2);
		qinLbl_11->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_11->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_11->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_11);
		horizontalLayout_3->addWidget(qinLbl_11);

		MLineEdit* qinLbl_12 = new MLineEdit(ui.qinFrame);
		qinLbl_12->setObjectName(QStringLiteral("qinLbl_12"));
		sizePolicy2.setHeightForWidth(qinLbl_12->sizePolicy().hasHeightForWidth());
		qinLbl_12->setSizePolicy(sizePolicy2);
		qinLbl_12->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_12->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_12->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_12);
		horizontalLayout_3->addWidget(qinLbl_12);

		MLineEdit* qinLbl_13 = new MLineEdit(ui.qinFrame);
		qinLbl_13->setObjectName(QStringLiteral("qinLbl_13"));
		sizePolicy2.setHeightForWidth(qinLbl_13->sizePolicy().hasHeightForWidth());
		qinLbl_13->setSizePolicy(sizePolicy2);
		qinLbl_13->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_13->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_13->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_13);
		horizontalLayout_3->addWidget(qinLbl_13);

		MLineEdit* qinLbl_14 = new MLineEdit(ui.qinFrame);
		qinLbl_14->setObjectName(QStringLiteral("qinLbl_14"));
		sizePolicy2.setHeightForWidth(qinLbl_14->sizePolicy().hasHeightForWidth());
		qinLbl_14->setSizePolicy(sizePolicy2);
		qinLbl_14->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_14->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_14->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_14);
		horizontalLayout_3->addWidget(qinLbl_14);

		MLineEdit* qinLbl_15 = new MLineEdit(ui.qinFrame);
		qinLbl_15->setObjectName(QStringLiteral("qinLbl_15"));
		sizePolicy2.setHeightForWidth(qinLbl_15->sizePolicy().hasHeightForWidth());
		qinLbl_15->setSizePolicy(sizePolicy2);
		qinLbl_15->setMinimumSize(QSize(labelWidth, labelHeight));
		qinLbl_15->setMaximumSize(QSize(labelWidth, labelHeight));
		//qinLbl_15->setStyleSheet(QStringLiteral("background-color: rgb(145, 130, 255);"));
		list.append(qinLbl_15);
		horizontalLayout_3->addWidget(qinLbl_15);

		qinList.append(list);
		ui.verticalLayout_5->addLayout(horizontalLayout_3);
	}

}