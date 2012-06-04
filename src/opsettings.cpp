#include "opsettings.h"
#include "signaldata.h"
#include <QDebug>

OpSettings::OpSettings(QObject *parent) :
    QObject(parent),
    horPos(0)
{
    //This should be load from preferences file
    trigger=false;
    triggerSource=0;
    ch1Coupling=DC;
    channelNum=0;
    ch = new Channel[10];

    orderedColorList = new enum Qt::GlobalColor[10];

    orderedColorList[0]=Qt::green;
    orderedColorList[1]=Qt::white;
    orderedColorList[2]=Qt::cyan;
    orderedColorList[3]=Qt::yellow;
    orderedColorList[4]=Qt::lightGray;
    orderedColorList[5]=Qt::darkGray;
    orderedColorList[6]=Qt::red;
    orderedColorList[7]=Qt::black;
    orderedColorList[8]=Qt::blue;
    orderedColorList[9]=Qt::magenta;

}

OpSettings::~OpSettings()
{
    delete [] ch;

}

OpSettings &OpSettings::instance()
{
    static OpSettings opSet;
    return opSet;
}

/*
 * Horizontal Settings
 */

void OpSettings::setHorPos(const double& value)
{
    horPos=value;
}

double OpSettings::getHorPos(void) const
{
    return horPos;
}

/*
 * Ch1 Settings
 */

void OpSettings::addChannel(QString path)
{
    ch[channelNum].pipePath = new QString(path);
    //channelPipePath[channelNum] = new QString(path);
    ch[channelNum].Color=QColor(orderedColorList[channelNum]);
    channelNum++;
}

int OpSettings::lastChannel() const
{
    return channelNum;
}

QString OpSettings::channelPath(const int& i) const
{
    //return *channelPipePath[i];
    return *ch[i].pipePath;
}

QString OpSettings::channelPipe(const int& i) const
{
    QString string= *ch[i].pipePath;

    return string.remove(0,36);
}

void OpSettings::setPos(int i,const double& value)
{
    ch[i].Pos=value;
    //ch1Pos=value;
}

double OpSettings::getPos(int channel) const
{
    return ch[channel].Pos;
}

void OpSettings::setGain(int i,const double& value)
{
    ch[i].Gain=value;
}

double OpSettings::getGain(int channel) const
{
    return ch[channel].Gain;
}

void OpSettings::setCoupling(int i,int value)
{

    ch[i].Coup=(enum OpSettings::Coupling) value;

    return;
}

enum OpSettings::Coupling OpSettings::getCoupling(int channel) const
{
    return ch[channel].Coup;
}

void OpSettings::setColor(int i,QColor color)
{
    ch[i].Color=color;
}

void OpSettings::setColor(int i,QString color)
{
    qDebug() << "I'm here";
    ch[i].Color=QColor(color);
}

QColor OpSettings::getColor(int i) const
{
    return ch[i].Color;
}

QString OpSettings::getColorName(int i) const
{
    QString* c;

    if(ch[i].Color==Qt::green){
        c=new QString("green");
        return *c;
    }else if(ch[i].Color==Qt::white){
        c=new QString("white");
        return *c;
    }else if(ch[i].Color==Qt::cyan){
        c=new QString("cyan");
        return *c;
    }else if(ch[i].Color==Qt::yellow){
        c=new QString("yellow");
        return *c;
    }else if(ch[i].Color==Qt::lightGray){
        c=new QString("darkGray");
        return *c;
    }else if(ch[i].Color==Qt::darkGray){
        c=new QString("darkGray");
        return *c;
    }else if(ch[i].Color==Qt::red){
        c=new QString("black");
        return *c;
    }else if(ch[i].Color==Qt::black){
        c=new QString("black");
        return *c;
    }else if(ch[i].Color==Qt::blue){
        c=new QString("blue");
        return *c;
    }else if(ch[i].Color==Qt::magenta){
        c=new QString("magenta");
        return *c;
    }

    return ch[i].Color.name();

}

/*
 * Trigger Settings
 */

void OpSettings::setTriggerSlope(int value)
{
    triggerSlope=(enum Triggers) value;
}

enum OpSettings::Triggers OpSettings::getTriggerSlope() const
{
    return triggerSlope;
}

int OpSettings::getTriggerSource() const
{
    return triggerSource;

}

void OpSettings::setTriggerLevel(double value)
{
    triggerLevel=value;
}

double OpSettings::getTriggerLevel() const
{
    return triggerLevel;
}

void OpSettings::setWaitForT(bool value)
{
    waitForT=value;
}

bool OpSettings::getWaitForT() const
{
    return waitForT;
}

void OpSettings::setTrigger(int value)
{
    trigger=(bool)value;
}

bool OpSettings::isTriggered() const
{
    return trigger;
}

void OpSettings::setTriggerSource(int value)
{
    qDebug() << "hello" << value;
    triggerSource=value;
}

void OpSettings::setStatus(bool status)
{

    if(status){
        /* setting state to Plot or Wait
         * always requires a reset of vector data
         * and CurrentX. Better do this here
         * rather than in samplingthread or plot
         * mostly due to race conditions.
         * Examples of this are in sampling
         * thread returning from trigger
         * and live switch
         */
        for(int i=0;i<lastChannel();i++)
            SignalData::instance(i).clear();

        resetCurrentX();

        if(isTriggered())
            setState(Wait);
        else
            setState(Plot);


        Q_EMIT turnOn();
    }
    else{
        setState(Stop);
        Q_EMIT turnOff();
    }

}

void OpSettings::setOperation(int value)
{
    switch(currentState()){
        case Plot:
        case Wait://this is a live switch
            Q_EMIT liveSwitch((bool)value);
            break;
        case Stop://this is an offline switch
            setTrigger(value);
    }

}


/*
 * Sampling
 */

void OpSettings::resetCurrentX()
{
    //horPos only works on triggered mode
    if(trigger)
        currentX=horPos;
    else
        currentX=0;
}

void OpSettings::setCurrentX(double value)
{
    currentX=value;
}

double OpSettings::getCurrentX() const
{
    return currentX;
}

void OpSettings::incCurrentX(double value)
{
    currentX+=value;
}

enum OpSettings::StateMachine OpSettings::currentState() const
{

    return stateMach;
}

void OpSettings::setState(enum OpSettings::StateMachine state)
{
    stateMach=state;
}
