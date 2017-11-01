#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_maindialog.h"
#include "CPartitionHeaderInfo.h"

class maindialog : public QDialog
{
	Q_OBJECT

public:
	maindialog(QWidget *parent = 0);
	~maindialog();

private:
	Ui::maindialogClass ui;

	CPartitionHeaderInfo m_PartitionHeaderInfo;

	void ClearTableWidgetPartition();

	
private slots:
	void test();
	void OpenBin();
	void ClickedPartitionLists(QModelIndex index);
	void PartitionListsChanged(int index);
};

#endif // MAINDIALOG_H
