#include <qwt_sampling_thread.h>
#include <QObject>
#include <QFile>
#include <QDataStream>

#include "mtrx.h"
#include "opsettings.h"

class Plot;

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread(QObject *parent = NULL);
    virtual ~SamplingThread();

    double amplitude() const;

Q_SIGNALS:
    void Triggered();
    void newVMax(int,double);
    void newVp(int,double);
    void newVpp(int,double);
    void newPeriod(int,double);

public Q_SLOTS:
    void turnOn();
    void turnOff();

protected:
    virtual void sample(double elapsed);

private:
    void getSample(int channel);

    bool tryTrigger(double sample);
    bool triggerCondition(double* array);

    void calculatePeriod(int channel, double elapsed);
    void calculateAmplitudes(int channel, int coupling);

    double dcBlock(int channel,double input);

    double windowAverage(double);
    bool openFile(int index,QString path);

    virtual double value(double timeStamp) const;

    QFile** file;
    QDataStream** pipe;
    double d_frequency;
    double d_amplitude;
    double d_previous;
    int* d_coupling;
    double** d_sample;
    int pipe_fd;
    double *Vp;
    double *Vp_min;
    double *V_max;
};
