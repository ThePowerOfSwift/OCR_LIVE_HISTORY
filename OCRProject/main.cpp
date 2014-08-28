#include <QtWidgets/QApplication>
#include "Include/Global.h"
#include "Ui/OcrControl.h"
//#include "Include/HorseDataStruct.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <QTcpSocket>
using namespace cv;
using namespace std;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Global global;
	global.init();

	OcrControl w;
	w.show();
	return a.exec();
}
