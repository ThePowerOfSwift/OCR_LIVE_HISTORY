/**
* 项    目:  Ct-ARS软件 模型类
* 文件名称:  DgSystemLog.h
* 文件路径： Model/SystemLog/DgSystemLog.h
* 摘    要:  系统日志委托类，负责处理Model中数据，供View使用
* 作    者： 刘兆邦
* 完成日期： 2012年08月30日
* Copyright (c) 2012, 中科院苏州医工所医学影像室电子组.All rights reserved.
* Version v1.0
*/

#include <QtGui>
#include <QItemDelegate>
#include <QStyleOptionProgressBarV2>
#include "DgSystemLog.h"
#include <QApplication>
//重载绘制函数
void DgSystemLog::paint(QPainter *painter, const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	//如果是第3列'日志详细内容'
	//if (index.column() == 3)
	//{
	//	//获得当前项值
	//	QString logDetail = index.data().toString();
	//       //按钮的风格选项  
	//       QStyleOptionButton *checkBoxOption = new QStyleOptionButton();  
	//       checkBoxOption->state |= QStyle::State_Enabled;  
	//       //根据值判断是否选中  
	//       if(checked)  
	//       {  
	//           checkBoxOption->state |= QStyle::State_On;  
	//       }  
	//       else  
	//       {  
	//           checkBoxOption->state |= QStyle::State_Off;  
	//       }  
	//       //返回QCheckBox几何形状  
	//       checkBoxOption->rect = CheckBoxRect(option);  
	//       //绘制QCheckBox  
	//       QApplication::style()->drawControl(QStyle::CE_CheckBox,checkBoxOption,painter); 
	//}
	////如果是第7列'备注'
	//else 
	//if (index.column() == 7)
	//{
	//	//获得当前项值
	//	QString remark = index.data().toString();
	//	//设置'病人Id'格式字符串: P:00000x;6位10进制数，不足补0;
	//	QString text = QString("P:%1").arg(remark);


	//	//获取项风格设置
	//	QStyleOptionViewItem myOption = option;
	//	myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
	//	myOption.decorationPosition = QStyleOptionViewItem::Right;
	//	myOption.font = QFont("Times", 10, QFont::Bold);;
	//	myOption.showDecorationSelected = true;
	//	myOption.textElideMode = Qt::ElideNone;
	//	//绘制文本
	//	QApplication::style()->drawItemText ( painter, myOption.rect , myOption.displayAlignment, QApplication::palette(), true,text );

	//}
	//如果是第6列
	//else if (index.column() == 11)
	//{

	//const QAbstractItemModel *itemModel = index.model();
	////获得索引对应Model中的数据
	//int finishedSubTaskNum = index.data().toInt();
	//int subTaskNum = itemModel->data(itemModel->index(index.row(),8), Qt::DisplayRole).toInt();
	////进度条的风格选项
	//QStyleOptionProgressBarV2 progressBarOption;
	//progressBarOption.rect = option.rect;
	//progressBarOption.minimum = 0;
	//progressBarOption.maximum = subTaskNum;
	//progressBarOption.progress = finishedSubTaskNum;
	//double t = finishedSubTaskNum/subTaskNum;
	//progressBarOption.text = QString("%1 / %2").arg(finishedSubTaskNum,2,10,QChar('0')).arg(subTaskNum,2,10,QChar('0'));
	//progressBarOption.textVisible = true;
	////绘制进度条
	//QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

	//}
	//如果是第6列
	//   if (index.column() == 6)
	//   {
	//	//获取值 
	//	bool checked = true;
	//       //bool checked = index.model()->data(index, Qt::DisplayRole).toBool();  
	//       //按钮的风格选项  
	//       QStyleOptionButton *checkBoxOption = new QStyleOptionButton();  
	//       checkBoxOption->state |= QStyle::State_Enabled;  
	//       //根据值判断是否选中  
	//       if(checked)  
	//       {  
	//           checkBoxOption->state |= QStyle::State_On;  
	//       }  
	//       else  
	//       {  
	//           checkBoxOption->state |= QStyle::State_Off;  
	//       }  
	//       //返回QCheckBox几何形状  
	//       checkBoxOption->rect = CheckBoxRect(option);  
	//       //绘制QCheckBox  
	//       QApplication::style()->drawControl(QStyle::CE_CheckBox,checkBoxOption,painter); 
	//}
	//   else
	//   {
	//否则调用默认委托
	QStyledItemDelegate::paint(painter, option, index);
	//}

}
//生成QCheckBox
QRect DgSystemLog::CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions)const
{
	//绘制按钮所需要的参数
	QStyleOptionButton checkBoxStyleOption;
	//按照给定的风格参数 返回元素子区域
	QRect checkBoxRect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
	//返回QCheckBox坐标，移动到单元格正中间
	QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
		viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);


	//返回QCheckBox几何形状
	return QRect(checkBoxPoint, checkBoxRect.size());
}
//重载 返回大小
QSize DgSystemLog::sizeHint(const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	//   //如果某项是星星
	//   if (index.column() == 9)
	//   {
	//
	//       return QSize(50,1);
	//   }
	//else
	//{
	return QStyledItemDelegate::sizeHint(option, index);
	//   }
	return QStyledItemDelegate::sizeHint(option, index);
}

//创建编辑控件
QWidget *DgSystemLog::createEditor(QWidget *parent,
	const QStyleOptionViewItem &option,
	const QModelIndex &index) const

{
	//if (index.column() == 6) 
	//{  
	//       //时间编辑控件
	//       QTimeEdit *timeEdit = new QTimeEdit(parent);
	//       //时间编辑控件文本格式  
	//       timeEdit->setDisplayFormat("mm:ss");
	//       //如果编辑结束，激活提交和关闭槽
	//       connect(timeEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
	//       return timeEdit;
	//   }
	//音轨时间列  
	//   if (index.column() == 6) 
	//{  
	//       //时间编辑控件
	//       QTimeEdit *timeEdit = new QTimeEdit(parent);
	//       //时间编辑控件文本格式  
	//       timeEdit->setDisplayFormat("mm:ss");
	//       //如果编辑结束，激活提交和关闭槽
	//       connect(timeEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
	//       return timeEdit;
	//   }
	////如果某列是'已经完成子任务数'
	//else if (index.column() == 9)
	//{
	//	//获得Model
	//	const QAbstractItemModel *itemModel = index.model();
	//	//已经完成子任务数
	//       int finishedSubTaskNum = itemModel->data(index, Qt::DisplayRole).toInt();
	//	//总子任务书
	//	int subTaskNum = itemModel->data(itemModel->index(index.row(),8), Qt::DisplayRole).toInt();

	//
	//	//进度条显示进度
	//       QStyleOptionProgressBarV2 *progressBarOption = new QStyleOptionProgressBarV2();

	//	progressBarOption->minimum = 0;
	//       progressBarOption->maximum = subTaskNum;
	//	progressBarOption->progress = finishedSubTaskNum;
	//	progressBarOption->text = QString::number(20) + "--";
	//	progressBarOption->textVisible = true;
	//	//槽函数，后期用
	//       //connect(progressBar, SIGNAL(valueChanged ()),this, SLOT(commitAndCloseEditor()));
	//       return progressBarOption;
	//}
	//else
	//{
	//否则使用默认委托处理
	return QStyledItemDelegate::createEditor(parent, option, index);
	//}
}
//设置编辑控件中的数据
void DgSystemLog::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	//如果是第9列
	//if(index.column() == 9)
	//{
	//	//获得Model
	//	const QAbstractItemModel *itemModel = index.model();
	//	 //已完成任务数，返回模型中该行，第9列的数据
	//	int finishedSubTaskNum = itemModel->data(index, Qt::DisplayRole).toInt();
	//	//总子任务数，返回模型中该行，第8列的数据
	//	int subTaskNum = itemModel->data(itemModel->index(index.row(),8), Qt::DisplayRole).toInt();
	//	//获取当前项的进度条
	//       QStyleOptionProgressBarV2 *progressBarOption = qobject_cast<QStyleOptionProgressBarV2 *>(editor);
	//
	//	//给控件赋值
	//	progressBarOption->minimum = 0;
	//       progressBarOption->maximum = subTaskNum;
	//	progressBarOption->progress = finishedSubTaskNum;
	//	progressBarOption->text = QString::number(20) + "--";
	//	progressBarOption->textVisible = true;
	//}
	//else
	//{
	//       QStyledItemDelegate::setEditorData(editor, index);
	//   }
}
//设置模型中的数据
void DgSystemLog::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	////如果是第9列
	//if(index.column() == 9)
	//{
	//    //获取当前项的进度条
	//    QStyleOptionProgressBarV2 *progressBarOption = qobject_cast<QStyleOptionProgressBarV2 *>(editor);
	//    //设置模型数据
	//    model->setData(index, progressBarOption->progress);
	//} else
	//{
	//    QStyledItemDelegate::setModelData(editor, model, index);
	//}
}
//提交并关闭控件
void DgSystemLog::commitAndCloseEditor()
{
	//QStyleOptionProgressBarV2 *editor = qobject_cast<QStyleOptionProgressBarV2 *>(sender());
	//emit commitData(editor);
	//emit closeEditor(editor);
}

//提交并关闭控件
//void DgSystemLog::setProgressBar(QAbstractItemModel *model, const QModelIndex &index)
//{
//	设置模型数据
//    model->setData(index, progressBarOption->progress);
//    QStyleOptionProgressBarV2 *editor = qobject_cast<QStyleOptionProgressBarV2 *>(sender());
//    emit commitData(editor);
//    emit closeEditor(editor);
//}
