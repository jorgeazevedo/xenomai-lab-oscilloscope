#ifndef OPSETTINGS_H
#define OPSETTINGS_H

#include <QObject>
#include <QMutex>
#include <QColor>

class OpSettings : public QObject
{
    Q_OBJECT
public:
    enum Coupling
    {
       AC,
       DC
    };

    enum Triggers
    {
        RisingEdge,
        FallingEdge
    };

    enum StateMachine
    {
        Stop,
        Plot,
        Wait
    };

    class Channel
    {
    public:
        Channel(): Pos(0),Gain(1){
            pipePath=0;
            Coup=DC;
            Color=QColor(Qt::green);
        }

        ~Channel(){
            if(pipePath!=0)
                delete pipePath;
        }
        double Pos;
        double Gain;
        enum Coupling Coup;
        QColor Color;
        QString* pipePath;
    };

    static OpSettings &instance();


    //Horizontal Settings
    void setHorPos(const double&);  //by reference, read only
    double getHorPos() const;       //does not change object (*this)

    //Channel settings
    void addChannel(QString path);
    int lastChannel() const;
    QString channelPath(const int& i) const;
    QString channelPipe(const int& i) const;
    void setPos(int i,const double&);
    double getPos(int) const;
    void setGain(int i,const double&);
    double getGain(int) const;
    enum Coupling getCoupling(int) const;
    void setCoupling(int,int);
    void setColor(int,QColor);
    void setColor(int,QString);
    QColor getColor(int) const;
    QString getColorName(int) const;

    //Trigger settings and Operation
    enum Triggers getTriggerSlope() const;
    int getTriggerSource() const;
    void setTriggerLevel(double);
    double getTriggerLevel() const;
    void setWaitForT(bool);
    bool getWaitForT() const;
    bool isTriggered() const;
    //void setTrigger(bool);


    //Sampling
    void resetCurrentX(void);
    void setCurrentX(double);
    void incCurrentX(double);
    double getCurrentX() const;
    QMutex mutex;   //public facilitates QMutexLocker

    //State Machine
    enum StateMachine currentState() const;
    void setState(enum StateMachine);


Q_SIGNALS:
    void turnOn();
    void turnOff();
    void liveSwitch(bool);

public Q_SLOTS:
    void setTriggerSlope(int);
    void setTriggerSource(int);
    void setTrigger(int);
    void setStatus(bool);
    void setOperation(int);


private:
    OpSettings(QObject *parent = 0);

    virtual ~OpSettings();


    //Horizontal Settings
    double horPos;

    //Ch1 Settings
    int channelNum;
    QString* channelPipePath[5];
    Channel* ch;
    double ch1Pos;
    enum Coupling ch1Coupling;
    enum Qt::GlobalColor* orderedColorList;

    //Trigger settings and Operation
    bool trigger;
    double triggerLevel;
    int triggerSource;
    enum Triggers triggerSlope;
    bool waitForT;

    //Sampling
    double currentX;

    //State Machine
    enum StateMachine stateMach;
};

#endif // OPSETTINGS_H
