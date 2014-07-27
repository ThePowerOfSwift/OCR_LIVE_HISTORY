#include "HorseNameTrain.h"


HorseNameTrain::HorseNameTrain()
{
	qDebug("Enter HorseNameTrain Class \n");
	//打开文件
	character2File.setFileName(".\\horseNameFile\\2CharacterHorse.txt");
	if (!character2File.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qDebug() << "HorseNameTrain : Can't open the character2File file!" << endl;
	}
	character3File.setFileName(".\\horseNameFile\\3CharacterHorse.txt");
	if (!character3File.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qDebug() << "HorseNameTrain : Can't open the character3File file!" << endl;
	}
	character4File.setFileName(".\\horseNameFile\\4CharacterHorse.txt");
	if (!character4File.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qDebug() << "HorseNameTrain : Can't open the character4File file!" << endl;
	}


	QTextStream cha2FileIn(&character2File);
	
 
	while (!cha2FileIn.atEnd()) {
		QString line = cha2FileIn.readLine();		 
		cha2HorseNameStringList.push_back(line);
	}
	QTextStream cha3FileIn(&character3File);
	while (!cha3FileIn.atEnd()) {
		QString line = cha3FileIn.readLine();
		cha3HorseNameStringList.push_back(line);
	}

	QTextStream cha4FileIn(&character4File);
	while (!cha4FileIn.atEnd()) {
		QString line = cha4FileIn.readLine();
		cha4HorseNameStringList.push_back(line);
	}

	int pos = cha4HorseNameStringList.indexOf("軍事機密");
	 

}


HorseNameTrain::~HorseNameTrain()
{


}
