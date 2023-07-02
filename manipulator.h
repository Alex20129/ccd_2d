#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <QVector2D>
#include <QVector>
#include <QTimer>

class Manipulator : public QObject
{
	Q_OBJECT
signals:
	void poseChanged();
	void solved();
public slots:
	void setTargetPosition(QVector2D position);
public:
	Manipulator(QObject *parent=nullptr);
	void setJointAngle(uint joint_id, double joint_angle);
	void setLinkLength(uint link_id, double link_length);
	double getLinkLength(uint link_id);

	void setDesiredDeviation(double deviation);
	void setMaxIterations(uint max_iterations);
	void setAnimationDelay(uint delay);
	QVector2D getTargetPosition();
	void solveIKforJoint(uint joint_id);

	QVector<QVector2D> *JointPosition;
	QVector2D EffectorPosition;
private slots:
	void on_AnimationTimer_timeout();
private:
	void recalculatePose();
	double pDesiredDeviation, pActualDeviation;
	QVector2D pTargetPosition;
	QVector<double> *pJointAngle;
	QVector<double> *pLinkLength;
	QVector<QVector2D> *pLinkVector;
	QTimer *pAnimationTimer;
	uint pActiveJoint, pIterationsPerformed, pMaxIterations;
	uint pAnimationDelay;
	uint pMaxLength;
	bool pIsBusy;
};

#endif // MANIPULATOR_H

