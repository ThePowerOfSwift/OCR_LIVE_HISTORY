#include "SystemLogThread.h"
#include <QDebug>
#include <QMutex>
#include "Bll/SystemLog/SystemLog.h"
//#include "Bll/SystemLog/BllSystemLog.h"
#include "Model/SystemLog/MoSystemLog.h"
SystemLogThread::SystemLogThread(QObject *parent)
: QThread(parent)
{
	////从界面打开
	//fileName = "SystemLog.txt";
	//projectFile = fopen(fileName.toLocal8Bit().data(), "a+");

}

SystemLogThread::~SystemLogThread()
{
	////关闭文件
	//if (fclose(projectFile) != 0)
	//	qDebug() << "【SystemLogThread】关闭文件失败，文件名：" << fileName;
}
/**
*函数介绍：线程start()之后,默认执行run()函数
*输入参数：无
*返回值：
*/
void SystemLogThread::run()
{
	qDebug() << "【启动系统日志线程】....线程Id" << QThread::currentThreadId();


	//启动事件循环，等待exit()被call 
	this->exec();

}

/**
*函数介绍：保存系统日志
*输入参数：无
*返回值：
*/
void SystemLogThread::saveSystemLog(MoSystemLog mo)
{

	QString log = QString("ID:%1,标题:%2,日志内容：%3,日志类型:%4,日志时间:%5\n").arg(QString::number(mo.getLogId())).arg(mo.getLogTitle()).arg(mo.getLogDetail()).arg(mo.getLogType()).arg(mo.getLogTime().toString());
	QByteArray array = log.toLocal8Bit();
	char* logChar = array.data();

	//从界面打开
	QString fileName = "SystemLog.txt";
	FILE *projectFile = fopen(fileName.toLocal8Bit().data(), "a+");
	if (!projectFile)
	{
		qDebug() << "【SystemLogThread】数据文件" << projectFile << "，打开失败！\n";
	}
	//写入磁盘
	int result = fwrite(logChar, sizeof(char), array.size(), projectFile);
	//拷贝数据失败
	if (result < 0)
	{
		qDebug() << "【SystemLogThread】写入文件失败,文件名：" << fileName;

	}
	else
	{
		qDebug() << "【SystemLogThread】将USB数据写入文件成功,文件名：" << fileName;
	}
	//关闭文件
	if (fclose(projectFile) != 0)
		qDebug() << "【SystemLogThread】关闭文件失败，文件名：" << fileName;

}
