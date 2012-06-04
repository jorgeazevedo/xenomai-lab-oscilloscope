#ifndef OPERATIONSETTINGS_H
#define OPERATIONSETTINGS_H

#include <QObject>

class OperationSettings : public QObject
{
    Q_OBJECT
public:
    static OperationSettings &instance();

    int whatMember();

    void setHorPos(double);//hardcore:const double&
    double getHorPos() const;

//signals:

//public slots:
private:
    OperationSettings(QObject *parent = 0);

    virtual ~OperationSettings();

    int d_member;

    double horPos;

};

#endif // OPERATIONSETTINGS_H
