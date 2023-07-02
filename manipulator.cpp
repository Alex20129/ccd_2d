#include "manipulator.h"

#include <cmath>

Manipulator::Manipulator(QObject *parent) : QObject(parent)
{
	pJointAngle=new QVector<double>;
	pLinkLength=new QVector<double>;
	pLinkVector=new QVector<QVector2D>;
	JointPosition=new QVector<QVector2D>;
	pAnimationTimer=new QTimer(this);
	pMaxIterations=1000;
	pDesiredDeviation=0.1;
	pIterationsPerformed=
	pAnimationDelay=
	pActiveJoint=
	pIsBusy=0;
}

void Manipulator::setJointAngle(uint joint_id, double joint_angle)
{
	while(joint_id+1 > pJointAngle->size())
	{
		pJointAngle->append(0.0);
		pLinkLength->append(0.0);
	}
	while(joint_angle>360.0)
	{
		joint_angle=joint_angle-360;
	}
	while(joint_angle<0.0)
	{
		joint_angle=joint_angle+360;
	}
	pJointAngle->replace(joint_id, joint_angle);
	recalculatePose();
}

void Manipulator::setLinkLength(uint link_id, double link_length)
{
	uint j;
	while(link_id+1 > pLinkLength->size())
	{
		pJointAngle->append(0.0);
		pLinkLength->append(0.0);
	}
	if(link_length>0xFFFFFFFF)
	{
		link_length=0xFFFFFFFF;
	}
	if(link_length<1.0)
	{
		link_length=1.0;
	}
	pLinkLength->replace(link_id, link_length);
	for(j=0, pMaxLength=0; j<pLinkLength->size(); j++)
	{
		pMaxLength+=pLinkLength->at(j);
	}
	recalculatePose();
}

double Manipulator::getLinkLength(uint link_id)
{
	return(pLinkLength->at(link_id));
}

void Manipulator::setDesiredDeviation(double deviation)
{
	pDesiredDeviation=deviation;
}

void Manipulator::setMaxIterations(uint max_iterations)
{
	pMaxIterations=max_iterations;
}

void Manipulator::setAnimationDelay(uint delay)
{
	pAnimationDelay=delay;
}

QVector2D Manipulator::getTargetPosition()
{
	return(pTargetPosition);
}

void Manipulator::setTargetPosition(QVector2D position)
{
	pTargetPosition=position;
	if(pTargetPosition.length()>pMaxLength)
	{
		pTargetPosition.normalize();
		pTargetPosition*=pMaxLength;
	}
	if(!pIsBusy)
	{
		pIterationsPerformed=0;
		pActiveJoint=0;
		pAnimationTimer->singleShot(pAnimationDelay, this, SLOT(on_AnimationTimer_timeout()));
	}
}

void Manipulator::solveIKforJoint(uint joint_id)
{
	QVector2D targetV=pTargetPosition-JointPosition->at(joint_id);
	QVector2D effectorV=EffectorPosition-JointPosition->at(joint_id);
	double dp=QVector2D::dotProduct(targetV, effectorV);
	double jointAngleCorrection;
	double lp=targetV.length()*effectorV.length();
	double cosa=dp/lp;
	if(cosa<(double)(-1.0))
	{
		cosa=(double)(-1.0);
	}
	else if(cosa>(double)1.0)
	{
		cosa=(double)1.0;
	}
	else if((double)(cosa)!=(double)(cosa)) //is nan?
	{
		fprintf(stdout, "cosa nan.\n");
		cosa=(double)0.0;
	}
	jointAngleCorrection=acos(cosa);
	jointAngleCorrection/=M_PI;
	jointAngleCorrection*=180.0;
	setJointAngle(joint_id, pJointAngle->at(joint_id)+jointAngleCorrection);
	targetV=pTargetPosition-JointPosition->at(joint_id);
	effectorV=EffectorPosition-JointPosition->at(joint_id);
	dp=QVector2D::dotProduct(targetV, effectorV);
	if(jointAngleCorrection<acos(dp/lp)/M_PI*180.0)
	{
		jointAngleCorrection*=2.0;
		setJointAngle(joint_id, pJointAngle->at(joint_id)-jointAngleCorrection);
	}
	emit(poseChanged());
}

void Manipulator::recalculatePose()
{
	uint j;
	double a;
	QVector2D newPosition(0, 0);
	QVector2D newVector;

	JointPosition->clear();
	pLinkVector->clear();

	for(j=0, a=0; j<pJointAngle->size(); j++)
	{
		a+=pJointAngle->at(j);
		newVector.setX(cos(a*M_PI/180.0)*pLinkLength->at(j));
		newVector.setY(sin(a*M_PI/180.0)*pLinkLength->at(j));
		pLinkVector->append(newVector);
	}

	for(j=0; j<pJointAngle->size(); j++)
	{
		JointPosition->append(newPosition);
		newPosition=JointPosition->at(j)+pLinkVector->at(j);
	}

	EffectorPosition=newPosition;
}

void Manipulator::on_AnimationTimer_timeout()
{
	pIsBusy=1;
	pActiveJoint--;
	if(pActiveJoint>=pJointAngle->size())
	{
		pActiveJoint=pJointAngle->size()-1;
	}
	solveIKforJoint(pActiveJoint);
	pIterationsPerformed++;
	if(pIterationsPerformed>=pMaxIterations)
	{
		fprintf(stdout, "Maximum iterations reached.\n");
		fprintf(stdout, "Deviation: %lf\n", pActualDeviation);
		fprintf(stdout, "Iterations Performed: %u\n", pIterationsPerformed);
		pIterationsPerformed=0;
		pActiveJoint=0;
		pIsBusy=0;
		emit(solved());
		return;
	}
	pActualDeviation=(pTargetPosition-EffectorPosition).length();
	if(pActualDeviation<pDesiredDeviation)
	{
		fprintf(stdout, "Desired deviation reached.\n");
		fprintf(stdout, "Deviation: %lf\n", pActualDeviation);
		fprintf(stdout, "Iterations Performed: %u\n", pIterationsPerformed);
		pIterationsPerformed=0;
		pActiveJoint=0;
		pIsBusy=0;
		emit(solved());
		return;
	}
	pAnimationTimer->singleShot(pAnimationDelay, this, SLOT(on_AnimationTimer_timeout()));
}

