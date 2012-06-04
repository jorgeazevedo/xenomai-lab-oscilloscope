#include "plot.h"
#include "curvedata.h"
#include "signaldata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>
#include <QDebug>



Plot::Plot(QWidget *parent):
    QwtPlot(parent),
    trigger(1),
    waitFortrigger(0),
    currentY(0),
    d_timerId(-1),
    d_interval(0.0, 10.0),
    d_refreshRate(30)

{
    //int d_paintedPoints[numChannels]
    d_paintedPoints=new int[OpSettings::instance().lastChannel()];
    for(int i=0;i<OpSettings::instance().lastChannel();i++)
        d_paintedPoints[i]=0;


    //d_operation=true;
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot(false);

    // The backing store is important, when working with widget
    // overlays ( f.e rubberbands for zooming ). 
    // Here we don't have them and the internal 
    // backing store of QWidget is good enough.

    canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);


#if defined(Q_WS_X11)
    // Even if not recommended by   TrollTech, Qt::WA_PaintOutsidePaintEvent
    // works on X11. This has a nice effect on the performance.
    
    canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

    // Disabling the backing store of Qt improves the performance
    // for the direct painter even more, but the canvas becomes
    // a native window of the window system, receiving paint events
    // for resize and expose operations. Those might be expensive
    // when there are many points and the backing store of
    // the canvas is disabled. So in this application
    // we better don't both backing stores.

    if ( canvas()->testPaintAttribute( QwtPlotCanvas::BackingStore ) )
    {
        /*
         * I chagne WA_PaintOnScreen to false but I have no ideia
         * what it does. It just simplifies my upgrade from 1 to 3
         * channels since updateCurve gets shorter
         * THIS ISN'T CALLED, IN FACT
         */
        canvas()->setAttribute(Qt::WA_PaintOnScreen, false);
        canvas()->setAttribute(Qt::WA_NoSystemBackground, true);
    }


#endif

    d_grad1=0;
    d_grad2=0;
    d_gridcolor=0;
    initGradient(0);

    plotLayout()->setAlignCanvasToScales(true);

    setAxisTitle(QwtPlot::yLeft, "Amplitude [numbaros]");

    setAxisTitle(QwtPlot::xBottom, "Time [s]");
    setAxisScale(QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue()); 
    setAxisScale(QwtPlot::yLeft, -200.0, 200.0);



    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle(QwtPlotMarker::Cross);
    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::gray, 0.0, Qt::DashLine));
    d_origin->attach(this);

    //QwtPlotCurve *d_curve[3];

    int numChannels=OpSettings::instance().lastChannel();
    d_curve = new QwtPlotCurve*[numChannels];

    for(int i=0;i<numChannels;i++){
        d_curve[i] = new QwtPlotCurve();
        d_curve[i]->setStyle(QwtPlotCurve::Lines);

    #if 1
        d_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    #endif
    #if 1
        d_curve[i]->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
    #endif
        d_curve[i]->setData(new CurveData(i));
        d_curve[i]->attach(this);
        d_curve[i]->setPen(QPen(OpSettings::instance().getColor(i)));
    }
    /*

    //Color ordered preference
    enum Qt::GlobalColor list[5];
    list[0]=Qt::green;
    list[1]=Qt::white;
    list[2]=Qt::cyan;
    list[3]=Qt::yellow;
    list[4]=Qt::lightGray;


    for(int i=0;i<numChannels;i++)
        d_curve[i]->setPen(QPen(list[i]));
*/
    //d_curve[0]->setPen(QPen(Qt::green));

}

Plot::~Plot()
{
    delete d_directPainter;
    delete [] d_paintedPoints;
    //TODO: review
    for(int i=0;i<OpSettings::instance().lastChannel();i++)
        delete d_curve[i];

}

void Plot::initGradient(int value)
{
    QPalette pal = canvas()->palette();

    //if these members alread have new QColors assigned,
    //delete them. (avoid memory leak)
    if(d_grad1!=0)
        delete d_grad1;
    if(d_grad2!=0)
        delete d_grad2;
    if(d_gridcolor!=0)
        delete d_gridcolor;


    switch(value){
    case 0: //blue
        d_grad1= new QColor(0, 49, 110);
        d_grad2= new QColor(0, 87, 174);
        d_gridcolor = new QColor(Qt::gray);
        break;
    case 1: //white
        d_grad1= new QColor(255,255,255);
        d_grad2= new QColor(255,255,255);
        d_gridcolor = new QColor(Qt::black);
        break;
    case 2: //green
        d_grad1= new QColor(0, 110, 54);
        d_grad2= new QColor(0, 174, 53);
        d_gridcolor = new QColor(Qt::gray);

    }


#if QT_VERSION >= 0x040400
    QLinearGradient gradient( 0.0, 0.0, 1.0, 0.0 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );

    gradient.setColorAt(0.0, *d_grad1 );//white
    gradient.setColorAt(1.0, *d_grad2 );

    pal.setBrush(QPalette::Window, QBrush(gradient));
#else
    pal.setBrush(QPalette::Window, QBrush( color ));
#endif

    canvas()->setPalette(pal);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(*d_gridcolor, 0.0, Qt::DotLine));
    grid->enableX(true);
    grid->enableXMin(true);
    grid->enableY(true);
    grid->enableYMin(false);
    grid->attach(this);
}

void Plot::start()
{
    d_clock.start();
    d_timerId = startTimer(d_refreshRate);
}

void Plot::stopTimer()
{
    //in case stopTimer is called on a
    //non existing timer
    if(d_timerId!=-1){
        killTimer(d_timerId);
        //like in constructor
        d_timerId=-1;
    }
}

void Plot::restartTimer(int period)
{

    if(d_timerId!=-1)
        killTimer(d_timerId);

    replot();
    d_refreshRate=period;
    d_timerId = startTimer(period);


}

void Plot::restartTimer()
{
    if(d_timerId!=-1)
        killTimer(d_timerId);
    replot();
    d_timerId = startTimer(d_refreshRate);
}


void Plot::replot()
{
    int numChannels=OpSettings::instance().lastChannel();
    CurveData* data[numChannels];



    for(int i=0;i<numChannels;i++){
        data[i] = (CurveData *)d_curve[i]->data();
        data[i]->values().lock();
    }


    QwtPlot::replot();

    for(int i=0;i<numChannels;i++){
        d_paintedPoints[i] = data[i]->size();
    }

    for(int i=0;i<numChannels;i++){
        data[i]->values().unlock();
    }

}

double Plot::intervalLength(){

    return d_interval.width();
}

bool Plot::waitForTrigger()
{


    return waitFortrigger;
}

Plot* Plot::waitForTrigger(bool value)
{

    waitFortrigger=value;

    return this;
}


Plot* Plot::setCurrentY(double value){

    currentY=value;

    return this;
}

void Plot::setIntervalLength(double interval)
{

    if ( interval > 0.0 && interval != d_interval.width() )
    {
        d_interval.setMaxValue(d_interval.minValue() + interval);
        setAxisScale(QwtPlot::xBottom, 
            d_interval.minValue(), d_interval.maxValue());

        replot();
    }
}

void Plot::setYaxis(double value)
{
    setAxisScale(QwtPlot::yLeft, -value, value);
    replot();

}
void Plot::resetAxis()
{
    //POG-Why does this work commented?!
    //CurveData *data = (CurveData *)d_curve[1]->data();
    //data->values().clear();
    //qDebug() << "Plot values have been cleared!";

    //qDebug() << "Min - " << d_interval.minValue() << " Max - " << d_interval.maxValue();

    double delta=d_interval.maxValue() - d_interval.minValue();

    d_interval.setMinValue(0);
    d_interval.setMaxValue(delta);
    setAxisScale(QwtPlot::xBottom,
        d_interval.minValue(), d_interval.maxValue());

    //qDebug() << "Min - " << d_interval.minValue() << " Max - " << d_interval.maxValue();

    replot();

}

void Plot::setRefreshRate(int period)
{
    /*
      * if timer isn't running, just register
      * change. It well get picked on next start
      * If it's running, then restart it with
      * correct value.
      */

    if(d_timerId==-1)
        d_refreshRate=period;
    else
        restartTimer(period);
}

void Plot::resetCurrentY()
{
    //horPos only works on triggered mode
    if(trigger)
        currentY=OpSettings::instance().getHorPos();
    else
        currentY=0;
}

void Plot::turnOn()
{
QMutexLocker locker(&OpSettings::instance().mutex);

    //delete current curve
    //SignalData::instance(0).clear();

    //resetCurrentY();
    resetAxis();

    //If we're in trigger mode, just mark
    //waitForTrigger. Samplingthread will
    //restart plot timer when the sample is right
    //Otherwise, start ploting immediately
    if(OpSettings::instance().currentState()==OpSettings::Plot)
        restartTimer();

}

void Plot::turnOff()
{
QMutexLocker locker(&OpSettings::instance().mutex);
//mutex.lock();
    //if button was clicked during
    //waitForTrigger cycle
    //if(waitFortrigger)
    //    waitFortrigger=false;

    //OpSettings::instance().setState(OpSettings::Stop);

    stopTimer();

    //d_operation=false;
//mutex.unlock();
}


void Plot::liveSwitch(bool value)
{
    //TODO: I should exhaust all possibilities of combinations
    //between value, state machine and trigger
QMutexLocker locker(&OpSettings::instance().mutex);
    stopTimer();

    OpSettings::instance().setTrigger(value);
    OpSettings::instance().resetCurrentX();

    //delete current curves
    for(int i=0;i<OpSettings::instance().lastChannel();i++)
        SignalData::instance(i).clear();

    //resetCurrentY();
    resetAxis();

    //if we're deactivating trigger during wait
    if((OpSettings::instance().currentState()==OpSettings::Wait)
        &&(value==false))
        OpSettings::instance().setState(OpSettings::Plot);

    if(OpSettings::instance().currentState()==OpSettings::Plot)
        restartTimer();
    //else we're on wait...
}

void Plot::returnFromTrigger()
{
    /*
     * This function is similar to turnOn(),
     * except currentY and data points are reset
     * from sampling thread to avoid race conditions
     */
 QMutexLocker locker(&OpSettings::instance().mutex);
 //mutex.lock();
    resetAxis();
    restartTimer();
 //mutex.unlock();
}

void Plot::setLineColor(int channel, QColor color)
{
    d_curve[channel]->setPen(QPen(color));

}

QPointF Plot::updateCurves()
{
    int numChannels=OpSettings::instance().lastChannel();
    QPointF *lastPoint = new QPointF[numChannels];

    for(int i=0;i<numChannels;i++)
        lastPoint[i]=updateCurve(i);

    //TODO: should return the trigger source or the highest
    return lastPoint[0];

}

QPointF Plot::updateCurve(int i)
{
    CurveData *data = (CurveData *)d_curve[i]->data();
    data->values().lock();

    const int numPoints = data->size();
    //If the number of points in memory is greater than the number of points
    //painted on screen, then we have new paint points. Otherwise, just return!
    if ( numPoints > d_paintedPoints[i] )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
             * In our case this is never done!
             */
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */


            const QwtScaleMap xMap = canvasMap( d_curve[i]->xAxis() );
            const QwtScaleMap yMap = canvasMap( d_curve[i]->yAxis() );

            QRectF br = qwtBoundingRect( *data, 
                       d_paintedPoints[i] - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
            d_directPainter->setClipRegion( clipRect );
        }

        d_directPainter->drawSeries(d_curve[i],
                                    d_paintedPoints[i] - 1, numPoints - 1);
        d_paintedPoints[i] = numPoints;
    }

    //POG: this should be last point to appear on screen
    QPointF returnPoint(data->values().boundingRect().bottomRight());

    data->values().unlock();

    return returnPoint;
}


void Plot::incrementInterval()
{

    d_interval = QwtInterval(d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width());

    CurveData *data;
    for(int i=0;i<OpSettings::instance().lastChannel();i++){
        data = (CurveData *)d_curve[i]->data();
        data->values().clearStaleValues(d_interval.minValue());
    }


    // To avoid, that the grid is jumping, we disable 
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = *axisScaleDiv(QwtPlot::xBottom);
    scaleDiv.setInterval(d_interval);

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks(i);
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks(i, ticks);
    }
    setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);

   // d_paintedPoints[0] = 0;
    for(int i=0;i<OpSettings::instance().lastChannel();i++){
        d_paintedPoints[i]=0;
    }

    replot();
}

void Plot::timerEvent(QTimerEvent *event)
{

    if ( event->timerId() == d_timerId ){
        //PROPOSED: updateCurve should return last point plotted
        QPointF lastPoint=updateCurves();
       // qDebug() << "Plotted:" << lastPoint;
        //if screen is full
        if (lastPoint.x()>d_interval.maxValue()){
            if(OpSettings::instance().isTriggered()){
                stopTimer();
                OpSettings::instance().setState(OpSettings::Wait);
            }
            else
                incrementInterval();
        }
    }

    QwtPlot::timerEvent(event);
}

void Plot::resizeEvent(QResizeEvent *event)
{
    d_directPainter->reset();
    QwtPlot::resizeEvent(event);
}
