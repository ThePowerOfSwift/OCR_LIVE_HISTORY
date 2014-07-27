#ifndef ThreadDataIdentify_H
#define ThreadDataIdentify_H

#include <QThread>

class ThreadDataIdentify : public QThread
{
	Q_OBJECT

public:
	ThreadDataIdentify(QObject  *parent = 0);
	~ThreadDataIdentify();
	void run();
private:
	
};

#endif // ThreadDataIdentify_H
