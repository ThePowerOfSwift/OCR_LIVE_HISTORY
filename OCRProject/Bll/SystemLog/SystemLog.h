/**
* 项   目:  PET/CT软件 CT-ARS
* 文件名称:  SystemLog.h
* 文件路径： Dal/SystemLog/SystemLog.h
* 摘   要:   系统日志类，负责对系统日志内容的存储，提供输入和输出接口
* 作   者： 刘兆邦
* 完成日期： 2012年08月07日
* Copyright (c) 2012, 中科院苏州医工所医学影像室电子组.
* All rights reserved.
* Version v1.0
*/
// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

#include <QObject>
#include <QMutex>
#include <QList>
#include "Model/SystemLog/MoSystemLog.h"
class SystemLog : public QObject
{
	Q_OBJECT

public:
	SystemLog(QObject *parent = 0);
	~SystemLog();
	//全局：系统日志缓冲区
	static QList<MoSystemLog> systemLogList;
	
	//日志缓冲区锁
	static QMutex mutexSystemLog;

	//日志类型枚举值
	enum LogType {
		//OPER(操作记录)
		OPER_TYPE = 0,
		//INFO(重要信息)
		INFO_TYPE = 1, 
		//WARN(警告)
		WARN_TYPE = 2,
		//ERROR(普通错误)
		ERROR_TYPE = 3,
		//FATAL(严重错误)
		FATAL_TYPE = 4
	};

public slots:
	
	//日志输入接口
	void append(QString logTitle, QString logDetail, SystemLog::LogType logType);
	//缓冲区间是否有日志
	static bool isEmpty();
	//获取缓冲区第一个系统日志
	static MoSystemLog getFirstLog();
	//删除缓冲区第一个系统日志
	static void removeFirstLog();
	//获取并删除缓冲区第一个系统日志
	static MoSystemLog takeFirstLog();
	//删除所有缓冲区所有系统日志
	void clear();
	//返回缓冲区日志数
	int size();
signals:
	//信号：有日志数据输入
	void dataEntered(MoSystemLog);

private:

};

#endif // SYSTEMLOG_H
