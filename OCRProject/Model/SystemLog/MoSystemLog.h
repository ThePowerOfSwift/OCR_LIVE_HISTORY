/**
*
* 项   目:  PET/CT软件 模型类
* 文件名称:  MoSystemLog.h
* 文件路径： Model/MoSystemLog.h
* 摘   要:   系统日志模型类，用于记录系统运行中出现的错误、信息以及关键操作。
* 作   者： 刘兆邦
* 完成日期： 2012年08月17日
* Copyright (c) 2012, 中科院苏州医工所医学影像室电子组.All rights reserved.
* Version v1.0
*/
// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef MoSystemLog_H
#define MoSystemLog_H
#include <QtCore>


class MoSystemLog
{
public:
	MoSystemLog();
	~MoSystemLog();
	MoSystemLog& operator = (const MoSystemLog&);


	friend QDataStream& operator >> (QDataStream&, MoSystemLog&);
	friend QDataStream& operator << (QDataStream&, const MoSystemLog&);

	//获取日志Id
	qint32 getLogId() const
	{
		return logId;
	}
	//设定日志Id
	void setLogId(qint32 m_logId)
	{
		logId = m_logId;
	}
	//获取日志标题
	QString getLogTitle() const
	{
		return logTitle;
	}
	//设定日志标题
	void setLogTitle(QString m_logTitle)
	{
		logTitle = m_logTitle;
	}
	//获取日志内容
	QString getLogDetail() const
	{
		return logDetail;
	}
	//设定日志内容
	void setLogDetail(QString m_logDetail)
	{
		logDetail = m_logDetail;
	}
	//获取日志类型(INFO(INFO(操作记录、执行状态) :0,WARN(警告):1,ERROR(普通错误):2,FATAL(严重错误):3)
	QString getLogType() const
	{
		return logType;
	}
	//设定日志类型(INFO(INFO(操作记录、执行状态) :0,WARN(警告):1,ERROR(普通错误):2,FATAL(严重错误):3)
	void setLogType(QString m_logType)
	{
		logType = m_logType;
	}
	//获取日志时间
	QDateTime getLogTime() const
	{
		return logTime;
	}
	//设定日志时间
	void setLogTime(QDateTime m_logTime)
	{
		logTime = m_logTime;
	}
	//获取标记(0表示正常状态,1:删除状态)
	QString getTag() const
	{
		return tag;
	}
	//设定标记(0表示正常状态,1:删除状态)
	void setTag(QString m_tag)
	{
		tag = m_tag;
	}
	//获取备注
	QString getRemark() const
	{
		return remark;
	}
	//设定备注
	void setRemark(QString m_remark)
	{
		remark = m_remark;
	}
private:
	//日志Id
	qint32 logId;
	//日志标题
	QString logTitle;
	//日志内容
	QString logDetail;
	//日志类型(INFO(INFO(操作记录、执行状态) :0,WARN(警告):1,ERROR(普通错误):2,FATAL(严重错误):3)
	QString logType;
	//日志时间
	QDateTime logTime;
	//标记(0表示正常状态,1:删除状态)
	QString tag;
	//备注
	QString remark;
};

#endif // MoDataAddress_H
