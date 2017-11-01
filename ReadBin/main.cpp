#include "maindialog.h"
#include <QtWidgets/QApplication>

typedef union
{
	int aa;
	unsigned short bb[2];
}cccc;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	maindialog w;
	w.show();
	return a.exec();
}
