#include "ThreadDataIdentify.h"
#include <QDebug>
ThreadDataIdentify::ThreadDataIdentify(QObject  *parent)
: QThread(parent)
{

}

ThreadDataIdentify::~ThreadDataIdentify()
{

}
void ThreadDataIdentify::run()
{
	qDebug() << "【ThreadDataIdentify】启动ThreadDataIdentify,线程Id" << QThread::currentThreadId();

	qDebug() << "【ThreadDataIdentify】运行结束!";
	this->exec();

}