// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef SystemLogThread_H
#define SystemLogThread_H

#include <QThread>
#include "Model/SystemLog/MoSystemLog.h"
class SystemLogThread : public QThread
{
	Q_OBJECT

public:
	SystemLogThread(QObject *parent = 0);
	~SystemLogThread();
	QString fileName;
	FILE *projectFile;
	//线程start(),默认执行run()函数
	void run();
	public slots:
	//保存系统日志
	void saveSystemLog(MoSystemLog mo);
signals:
private:
};

#endif // SystemLogThread_H
