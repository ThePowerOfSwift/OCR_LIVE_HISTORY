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
	qDebug() << "��ThreadDataIdentify������ThreadDataIdentify,�߳�Id" << QThread::currentThreadId();

	qDebug() << "��ThreadDataIdentify�����н���!";
	this->exec();

}