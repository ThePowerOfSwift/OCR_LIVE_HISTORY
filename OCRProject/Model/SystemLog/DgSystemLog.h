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
// 源代码编码必须是: UTF-8(BOM)  
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef DgSystemLog_H
#define DgSystemLog_H

#include <QStyledItemDelegate>

class DgSystemLog : public QStyledItemDelegate
{
	Q_OBJECT

public:
	DgSystemLog(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

	//重载绘制函数
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//重载返回大小
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//重载创建编辑控件
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//重载设置控件数据
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	//重载设置模型数据
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	//返回QCheckBox几何形状
	QRect CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions) const;
	public slots:
	void commitAndCloseEditor();

private:

};

#endif
