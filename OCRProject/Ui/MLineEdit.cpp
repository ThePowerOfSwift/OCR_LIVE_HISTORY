#include "../Ui/MLineEdit.h"

MLineEdit::MLineEdit(QWidget *parent)
:QLineEdit(parent)
{
	 
}

MLineEdit::~MLineEdit()
{

}

void MLineEdit::mousePressEvent(QMouseEvent *event)
{

	this->setText(" ");
	 
}
 
