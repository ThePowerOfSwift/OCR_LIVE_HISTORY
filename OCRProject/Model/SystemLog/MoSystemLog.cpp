/**
*
* 项   目:  PET/CT软件 模型类
* 文件名称:  MoSystemLog.cpp
* 文件路径： Model/MoSystemLog.cpp
* 摘   要:   系统日志模型类，用于记录系统运行中出现的错误、信息以及关键操作。
* 作   者： 刘兆邦
* 完成日期： 2012年08月17日
* Copyright (c) 2012, 中科院苏州医工所医学影像室电子组.All rights reserved.
* Version v1.0
*/
#include "MoSystemLog.h"

MoSystemLog::MoSystemLog()
{
}

MoSystemLog::~MoSystemLog()
{
}
/**
 *函数介绍：重构 赋值运算符
 *输入参数：
 *返回值：无
 */
MoSystemLog& MoSystemLog::operator = ( const MoSystemLog& mo)
{   
	logId = mo.getLogId();
	logTitle = mo.getLogTitle();
	logDetail = mo.getLogDetail();
	logType = mo.getLogType();
	logTime = mo.getLogTime();
	tag = mo.getTag();
	remark = mo.getRemark();

    return *this;
}


QDataStream& operator>>(QDataStream& in, MoSystemLog& mo)
{
     //日志Id
	qint32 mLogId;
	//日志标题
	QString mLogTitle;
	//日志内容
	QString mLogDetail;
	//日志类型(INFO(INFO(操作记录、执行状态) :0,WARN(警告):1,ERROR(普通错误):2,FATAL(严重错误):3)
	QString mLogType;
	//日志时间
	QDateTime mLogTime;
	//标记(0表示正常状态,1:删除状态)
	QString mTag;
	//备注
	QString mRemark;

    in >> mLogId >> mLogTitle >> mLogDetail >> mLogType >> mLogTime >> mTag >> mRemark;
    mo.setLogId(mLogId);
	mo.setLogTitle(mLogTitle);
	mo.setLogDetail(mLogDetail);
	mo.setLogType(mLogType);
	mo.setLogTime(mLogTime);
	mo.setTag(mTag);
	mo.setRemark(mRemark);

    return in;
}

QDataStream& operator<<(QDataStream& out, const MoSystemLog& mo)
{
    out << mo.getLogId() << mo.getLogTitle() << mo.getLogDetail() << mo.getLogType() << mo.getLogTime() 
		<< mo.getTag() << mo.getRemark();
    return out;

}

