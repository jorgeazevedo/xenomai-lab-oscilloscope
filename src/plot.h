#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <QMutex>

#include "opsettings.h"

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget * = NULL);
    virtual ~Plot();

    void start();
    virtual void replot();

    bool trigger;
    //double triggerLevel;
    //int triggerSlope;
    bool waitFortrigger;

    void incrementInterval();
    double intervalLength();
    double currentY;



    Plot* setCurrentY(double);
    Plot* waitForTrigger(bool);
    bool waitForTrigger();

public Q_SLOTS:
    void setIntervalLength(double);
    void setYaxis(double);
    void setRefreshRate(int);
    void resetCurrentY();
    void resetAxis();
    void turnOn();
    void turnOff();
    void returnFromTrigger();


    void liveSwitch(bool);
    void setLineColor(int,QColor);
    void initGradient(int value);

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void timerEvent(QTimerEvent *);

private:
    QPointF updateCurves();
    QPointF updateCurve(int i);

    void stopTimer();
    void restartTimer(int period);
    void restartTimer();

    QwtPlotMarker *d_origin;
    QwtPlotCurve **d_curve;
    int* d_paintedPoints;

    QwtPlotDirectPainter *d_directPainter;

    int d_timerId;
    QwtInterval d_interval;

    int d_refreshRate;
    QColor* d_grad1;
    QColor* d_grad2;
    QColor* d_gridcolor;

    QwtSystemClock d_clock;
};
