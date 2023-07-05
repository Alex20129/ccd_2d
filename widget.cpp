#include "widget.h"
#include "./ui_widget.h"
#include "manipulator.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

extern Manipulator *gManipulator;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
	targetBrush=new QBrush(QColor(32, 32, 255), Qt::SolidPattern);
	effectorBrush=new QBrush(QColor(255, 0, 0), Qt::SolidPattern);
	linkPen=new QPen();
	linkPen->setWidth(3);
	linkPen->setColor(QColor(32, 32, 123));

	ui->setupUi(this);
	this->setWindowTitle("ccd 2d");

	connect(this, SIGNAL(haveANewTargetPoint(QVector2D)), gManipulator, SLOT(setTargetPosition(QVector2D)));
	connect(gManipulator, SIGNAL(poseChanged()), this, SLOT(repaint()));
	connect(gManipulator, SIGNAL(solved()), this, SLOT(manipulator_solved()));
}

Widget::~Widget()
{
	delete ui;
}

void Widget::manipulator_solved()
{
	targetBrush->setColor(QColor(32, 255, 32));
	repaint();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_F11)
	{
		event->accept();
		if(this->isFullScreen())
		{
			this->showNormal();
		}
		else
		{
			this->showFullScreen();
		}
	}
}

void Widget::mousePressEvent(QMouseEvent *event)
{
	event->accept();
	QVector2D newTargetPosition;
	newTargetPosition.setX(event->pos().x()-this->width()/2);
	newTargetPosition.setY(event->pos().y()-this->height()/2);
//	qDebug()<<newTargetPosition;
	targetBrush->setColor(QColor(32, 32, 255));
	emit(haveANewTargetPoint(newTargetPosition));
}

void Widget::paintEvent(QPaintEvent *event)
{
	uint j, jnum=gManipulator->JointPosition->size(), dmax=gManipulator->getMaxLength();
	QPainter local_painter(this);
	QPoint p0, p1;
	event->accept();
	local_painter.setPen(*linkPen);

	p0.rx()=this->width()/2;
	p0.ry()=this->height()/2;
	local_painter.drawEllipse(p0, dmax, dmax);

	for(j=1; j<jnum; j++)
	{
		p0.rx()=gManipulator->JointPosition->at(j-1).x()+this->width()/2;
		p0.ry()=gManipulator->JointPosition->at(j-1).y()+this->height()/2;
		p1.rx()=gManipulator->JointPosition->at(j).x()+this->width()/2;
		p1.ry()=gManipulator->JointPosition->at(j).y()+this->height()/2;
		local_painter.drawEllipse(p0, 3, 3);
		local_painter.drawLine(p0, p1);
	}

	p0.rx()=gManipulator->JointPosition->at(j-1).x()+this->width()/2;
	p0.ry()=gManipulator->JointPosition->at(j-1).y()+this->height()/2;
	p1.rx()=gManipulator->EffectorPosition.x()+this->width()/2;
	p1.ry()=gManipulator->EffectorPosition.y()+this->height()/2;

	local_painter.drawEllipse(p0, 3, 3);
	local_painter.drawLine(p0, p1);
	local_painter.setBrush(*effectorBrush);
	local_painter.drawEllipse(p1, 3, 3);

	p0.rx()=gManipulator->getTargetPosition().x()+this->width()/2;
	p0.ry()=gManipulator->getTargetPosition().y()+this->height()/2;
	local_painter.setBrush(*targetBrush);
	local_painter.drawEllipse(p0, 4, 4);
}
