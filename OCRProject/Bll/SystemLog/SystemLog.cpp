#include "SystemLog.h"

SystemLog::SystemLog(QObject *parent)
	: QObject(parent)
{

}

SystemLog::~SystemLog()
{

}
//系统日志缓冲区
QList<MoSystemLog> SystemLog::systemLogList;
//日志缓冲区锁
QMutex SystemLog::mutexSystemLog;

/**
 *刘兆邦-添加
 *函数介绍: 日志输入接口
 *输入参数: logTitle:日志标题, logDetail:日志详细内容, logType:日志类型
 *返回值: 无
 */
void SystemLog::append(QString logTitle, QString logDetail, SystemLog::LogType logType)
{
	//注册该对象为元对象可识别系统，元对象将会将该参数保存起来，供队列连接调用。特别针对多线程
	qRegisterMetaType<MoSystemLog>("MoSystemLog");
	MoSystemLog moSystemLog;
	moSystemLog.setLogTitle(logTitle);
	moSystemLog.setLogDetail(logDetail);
	moSystemLog.setLogType(QString::number(logType));
	moSystemLog.setLogTime(QDateTime::currentDateTime());
	//标记(0表示正常状态,1:删除状态)
	moSystemLog.setTag("0");
	moSystemLog.setRemark("");

	mutexSystemLog.lock();
	systemLogList.append(moSystemLog);
	mutexSystemLog.unlock();

	//发出缓冲区新增日志的信号
	emit dataEntered(moSystemLog);
}
/**
 *刘兆邦-添加
 *函数介绍: 缓冲区间是否有日志
 *输入参数: 无
 *返回值: true:空,false:非空
 */
bool SystemLog::isEmpty()
{
	if(systemLogList.isEmpty())
		return true;
	else
		return false;
}
//获取缓冲区第一个系统日志;
/**
 *刘兆邦-添加
 *函数介绍: 获取缓冲区第一个系统日志
 *输入参数: 无
 *返回值: MoSystemLog:日志类对象
 */

MoSystemLog SystemLog::getFirstLog()
{
	return systemLogList.first();
}
/**
 *刘兆邦-添加
 *函数介绍: 删除缓冲区第一个系统日志
 *输入参数: 无
 *返回值: 无
 */
void SystemLog::removeFirstLog()
{
	systemLogList.removeFirst();
}
/**
 *刘兆邦-添加
 *函数介绍: 获取并删除缓冲区第一个系统日志
 *输入参数: 无
 *返回值: 无
 */
MoSystemLog SystemLog::takeFirstLog()
{
	return systemLogList.takeFirst();
}
/**
 *刘兆邦-添加
 *函数介绍: 删除所有缓冲区所有系统日志
 *输入参数: 无
 *返回值: int:删除日志数
 */
void SystemLog::clear()
{
	systemLogList.clear();
}
/**
 *刘兆邦-添加
 *函数介绍: 返回缓冲区日志数
 *输入参数: 无
 *返回值: int:删除日志数
 */
int SystemLog::size()
{
	return systemLogList.size();
}
