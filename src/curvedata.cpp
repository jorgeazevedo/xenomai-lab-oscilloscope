#include "curvedata.h"
#include "signaldata.h"

CurveData::CurveData(int i)//:QwtSeriesData()
{

    channel=i;
}

const SignalData &CurveData::values() const
{
    return SignalData::instance(channel);
}

SignalData &CurveData::values() 
{
    return SignalData::instance(channel);
}

QPointF CurveData::sample(size_t i) const
{
    return SignalData::instance(channel).value(i);
}

size_t CurveData::size() const
{
    return SignalData::instance(channel).size();
}

QRectF CurveData::boundingRect() const
{
    return SignalData::instance(channel).boundingRect();
}
