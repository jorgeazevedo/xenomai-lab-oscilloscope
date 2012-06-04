#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>
//mine
//#include <sys/types.h>
//#include <unistd.h>
//#include <native/pipe.h>
//#include <sys/ioctl.h>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QObject>
 #include <QMutexLocker>
#include <QMessageBox>

#include "plot.h"
//#include "opsettings.h"

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

SamplingThread::SamplingThread(QObject *parent):
    QwtSamplingThread(parent),
    d_frequency(5.0),
    d_amplitude(20.0),
    d_previous(0),
    pipe_fd(-1)
{
    //file=-1;
    //pipe=-1;
    int numChannels=OpSettings::instance().lastChannel();

    Vp=new double[numChannels];
    for(int i=0;i<numChannels;i++)
        Vp[i]=0;

    Vp_min=new double[numChannels];
    for(int i=0;i<numChannels;i++)
        Vp_min[i]=0;

    V_max=new double[numChannels];
    for(int i=0;i<numChannels;i++)
        V_max[i]=0;

    //QFile* file[num_channels];
    //QDataStream* pipe[num_channels];

    file = new QFile*[numChannels];
    pipe = new QDataStream*[numChannels];


    for(int i=0;i<numChannels;i++){
        file[i]=0,pipe[i]=0;
        qDebug() << "Opening file at" << OpSettings::instance().channelPath(i);
        openFile(i,OpSettings::instance().channelPath(i));
    }

    d_coupling=new int[numChannels];
    d_sample=new double*[numChannels];
    for(int i=0;i<numChannels;i++)
        d_sample[i]=new double[2];


}

SamplingThread::~SamplingThread()
{
    int numChannels=OpSettings::instance().lastChannel();
    //TODO: review
    for(int i=0;i<numChannels;i++){
        delete pipe[i];

        file[i]->close();
        delete file[i];
    }

    delete [] d_coupling;

    for(int i = 0; i < 3; ++i)
        delete [] d_sample[i];
    delete [] d_sample;

    delete [] Vp;
    delete [] Vp_min;
    delete [] V_max;


}

bool SamplingThread::openFile(int index,QString path)
{

QMutexLocker locker(&OpSettings::instance().mutex);

    //GAIN MUTEX
    //kill current file
    if(pipe[index]!=0)
        delete pipe[index];

    if(file[index]!=0){
        file[index]->close();
        delete file[index];
    }


    file[index]=new QFile(path);

    //Don't forget that this is a separate thread,
    //and hence we don't just return or exit(1) on an error
    if(!file[index]->exists()){
        int ret = QMessageBox::critical(0,"Osciloscope",
                                          "Failed to open:\n"
                                          +path+
                                          "\nFile doesn't exist!",
                                       QMessageBox::Ok);

      turnOff();
    }
    else{
        if(!file[index]->open( QIODevice::ReadOnly | QIODevice::Unbuffered)){
            int ret = QMessageBox::critical(0,"Osciloscope",
                                              "Failed to open:\n"
                                              +path+
                                              "\n",
                                           QMessageBox::Ok);

            turnOff();
        }
        else{
            pipe[index]=new QDataStream(file[index]);    // read the data serialized from the file
            pipe[index]->setFloatingPointPrecision(QDataStream::DoublePrecision);
            pipe[index]->setByteOrder(QDataStream::LittleEndian);

        }
    }


    return true;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

void SamplingThread::turnOn()
{
    qDebug() << "Sampling thread Started";
    d_previous=0; //like in constructor
    //OpSettings::instance().resetCurrentX();
    //SignalData::instance(0).clear();
    start();
}

void SamplingThread::turnOff()
{
    qDebug() << "Sampling thread Stopped";
    stop();
}

double SamplingThread::windowAverage(double value)
{
    static double window[5]={0,0,0,0,0};
    static int i=0;

    window[i]=value;

    i++;
    if(i>4)
        i=0;

    return 0.2*window[0]+0.2*window[1]+0.2*window[2]+0.2*window[3]+0.2*window[4];

}

bool SamplingThread::tryTrigger(double sample)
{
    static double sampleMemory[2]={0,0};

    sampleMemory[0]=sampleMemory[1];
    sampleMemory[1]=sample;

    //this is the sample! restart plot cycle
    if(triggerCondition(sampleMemory)){
            OpSettings::instance().setState(OpSettings::Plot);
            OpSettings::instance().resetCurrentX();

            for(int i=0;i<OpSettings::instance().lastChannel();i++)
                SignalData::instance(i).clear();

            Q_EMIT Triggered();
            Vp[0]=sampleMemory[1];
            Vp_min[0]=sampleMemory[1];

            return true;
    }
    else
        return false;


}

void SamplingThread::calculatePeriod(int channel, double elapsed)
{

    //Calculate period using zeroCrossing algorithm
    static double zeroCrossing[10][2];
    static double sampleMemory[10][2]={1,1};
    static int i=0;

    sampleMemory[channel][0]=sampleMemory[channel][1];
    sampleMemory[channel][1]=d_sample[channel][0];

    //if previous<0 and current>0, we crossed zero
    if((sampleMemory[channel][0]<0)&&(sampleMemory[channel][1])>0){
        zeroCrossing[channel][i]=elapsed;
        i++;
        if(i>1){
            //i=0;
            //Q_EMIT newPeriod(0,windowAverage(zeroCrossing[1]-zeroCrossing[0]));
            Q_EMIT newPeriod(channel,zeroCrossing[channel][1]-zeroCrossing[channel][0]);
            zeroCrossing[channel][0]=zeroCrossing[channel][1];
            i=1;
        }
    }

}

void SamplingThread::calculateAmplitudes(int channel,int coupling)
{
    //TODO: These things raise a precedence problem!

    //Check for overral V maximum V_max
    if(d_sample[channel][coupling]>V_max[channel]){
        V_max[channel]=d_sample[channel][coupling];
        Q_EMIT newVMax(channel,V_max[channel]);
    }

    //Check for local (in this screen) V maximum Vp
    if(d_sample[channel][coupling]>Vp[channel]){
        Vp[channel]=d_sample[channel][coupling];
        Q_EMIT newVp(channel,Vp[channel]);
    }

    //Check for local (in this screen) V minimum Vp_min
    if(d_sample[channel][coupling]<Vp_min[channel]){
        Vp_min[channel]=d_sample[channel][coupling];
        Q_EMIT newVpp(channel,Vp[channel]-Vp_min[channel]);
    }

}

void SamplingThread::getSample(int channel)
{
    /*
     * d_sample[channel][0] - AC
     * d_sample{channel][1] - DC
     */
    //*pipe[channel] >> d_sample[channel][1];
    Matrix A;
    //char* b= new char[100];

    //qDebug() << "Before readRawData Channel-" << channel;
    pipe[channel]->readRawData(reinterpret_cast<char*>(&A),sizeof(Matrix));
    //qDebug() << "After readRawData Channel-" << channel;

//    A=(Matrix*)b;

    d_sample[channel][1]=A.matrix[0][0];

    d_sample[channel][1]+=OpSettings::instance().getPos(channel);
    d_sample[channel][1]*=OpSettings::instance().getGain(channel);

    d_sample[channel][0]=dcBlock(channel,d_sample[channel][1]);

    //delete [] b;
    //;

/*
    d_sample[channel][0]*=OpSettings::instance().getGain(channel);
    d_sample[channel][0]+=OpSettings::instance().getPos(channel);
*/

}


//elapsed-Time since the thread was started in miliseconds
//period of this thing is set by calling  setInterval
void SamplingThread::sample(double elapsed)
{
    static double sample=0;
    static double sample_ac=0;

QMutexLocker locker(&OpSettings::instance().mutex);

        //get Y axis sample
        //
        //Input sample, work out DC and AC, add offset
        //sample_dc[0]=sample_dc[1];
        for(int i=0;i<OpSettings::instance().lastChannel();i++)
            getSample(i);



        //calculate X axis
        OpSettings::instance().incCurrentX(elapsed-d_previous);
        d_previous=elapsed;

        //choose a sample for triggering
        for(int i=0;i<OpSettings::instance().lastChannel();i++)
            d_coupling[i]=OpSettings::instance().getCoupling(i);

        int triggerSource=OpSettings::instance().getTriggerSource();

        sample=d_sample[triggerSource][d_coupling[triggerSource]];


        switch(OpSettings::instance().currentState()){
            case OpSettings::Wait:

                if(tryTrigger(sample)){

                        for(int i=0;i<OpSettings::instance().lastChannel();i++)
                            SignalData::instance(i).append(QPointF(OpSettings::instance().getCurrentX(), d_sample[i][d_coupling[i]]));
                }

                break;
            case OpSettings::Plot:




                for(int i=0;i<OpSettings::instance().lastChannel();i++){
                    calculatePeriod(i,elapsed);
                    calculateAmplitudes(i,d_coupling[i]);
                    SignalData::instance(i).append(QPointF(OpSettings::instance().getCurrentX(), d_sample[i][d_coupling[i]]));

                    //qDebug() << i << "-" << QPointF(OpSettings::instance().getCurrentX(), d_sample[i][d_coupling[i]]);
                }
                break;
        case OpSettings::Stop:
            break;

        }

        //qDebug() << "Sampling returned";

}

double SamplingThread::value(double timeStamp) const
{
    const double period = 1.0 / d_frequency;

    const double x = ::fmod(timeStamp, period);
    const double v = d_amplitude * qFastSin(x / period * 2 * M_PI);

    return v;
}

double SamplingThread::dcBlock(int channel,double input)
{
    static double last_input[10]={0,0,0,0,0,0,0,0,0,0};
    static double last_output[10]={0,0,0,0,0,0,0,0,0,0};
    double output;

    //y[n]=x[n]-x[n-1]+a*y[n-1]
    output=input-last_input[channel]+0.95*last_output[channel];
    last_input[channel]=input;
    last_output[channel]=output;

    return output;
}

bool SamplingThread::triggerCondition(double *array)
{
    //take notice! instance() is from this namespace!
    //using namespace OpSettings::;

    double triggerLevel=OpSettings::instance().getTriggerLevel();
    enum OpSettings::Triggers edge=OpSettings::instance().getTriggerSlope();

    switch(edge){
    case OpSettings::RisingEdge:
        return (array[1]>=triggerLevel)&&(array[0]<triggerLevel);
        break;
    case OpSettings::FallingEdge:
        return (array[1]<=triggerLevel)&&(array[0]>triggerLevel);

    }

    return false;
}
