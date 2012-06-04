#include "operationsettings.h"

OperationSettings::OperationSettings(QObject *parent) :
    QObject(parent),
    d_member(3),
    horPos(0)
{
}

OperationSettings::~OperationSettings()
{

}

OperationSettings &OperationSettings::instance()
{
    static OperationSettings opSet;
    return opSet;
}

int OperationSettings::whatMember()
{
    return d_member;
}

void OperationSettings::setHorPos(double value)
{
    horPos=value;
}

double OperationSettings::getHorPos(void) const
{
    return horPos;
}
