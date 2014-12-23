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

	QFile file("out.txt");
	if (!file.open(QIODevice::Append | QIODevice::Text))
		return 1;

	QTextStream out(&file);
	out << "The magic number is: " << 49 << "\n";


	OcrControl w;
	w.show();
	return a.exec();
}
