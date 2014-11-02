#pragma once
#ifndef MLINE_EDIT_H
#define MLINE_EDIT_H

#include <QLineEdit>  
#include <QMouseEvent>

class MLineEdit : public QLineEdit
{
	Q_OBJECT

public:

	explicit MLineEdit(QWidget *parent = 0);
	~MLineEdit();
	 

protected:
 
	void mousePressEvent(QMouseEvent *event);
//	void mouseReleaseEvent(QMouseEvent *event);
 
private:

 };

#endif //MLINE_EDIT_H
 
