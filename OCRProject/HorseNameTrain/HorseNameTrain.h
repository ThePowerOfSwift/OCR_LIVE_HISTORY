#pragma once
#include <QDir>
#include "Include/Global.h"
#include <QStringList>

#define  QDEBUG_OUTPUT 
// 处理马名 训练 识别 
class HorseNameTrain
{
public:
	HorseNameTrain();
	~HorseNameTrain();

	//马名字存储 本地txt文件 
	QFile character2File ;
	QFile character3File ;
	QFile character4File;

	QStringList cha2HorseNameStringList;
	QStringList cha3HorseNameStringList;
	QStringList cha4HorseNameStringList;


};

