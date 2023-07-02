#include "widget.h"
#include "manipulator.h"

#include <QApplication>

Manipulator *gManipulator=nullptr;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	gManipulator=new Manipulator;
	gManipulator->setJointAngle(0, -90);
	gManipulator->setLinkLength(0, 100);
	gManipulator->setJointAngle(1, 45);
	gManipulator->setLinkLength(1, 100);
	gManipulator->setJointAngle(2, -90);
	gManipulator->setLinkLength(2, 50);

	gManipulator->setDesiredDeviation(0.01);

	gManipulator->setMaxIterations(3000);
	gManipulator->setAnimationDelay(0);

	Widget w;
	w.show();
	return a.exec();
}

