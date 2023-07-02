#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector2D>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
	Q_OBJECT
signals:
	void haveANewTargetPoint(QVector2D position);
public:
	Widget(QWidget *parent = nullptr);
	~Widget();
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
public slots:
	void manipulator_solved();
private:
	Ui::Widget *ui;
	QBrush *targetBrush, *effectorBrush;
	QPen *linkPen;
};
#endif // WIDGET_H

