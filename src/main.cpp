#include <qapplication.h>
#include "mainwindow.h"
#include "samplingthread.h"
#include "plot.h"
#include "blockio.h"

#include "operationsettings.h"

#include "inputmap.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);


    argv[2][1]='u';
    qDebug() << "0" << argv[0] << argv[1] << argv[2];

    parse_args(argc,argv);
    for(int i=0;i<io.inputp_num;i++){
        //qDebug() << io.inputp_strings[i];
        OpSettings::instance().addChannel(io.inputp_strings[i]);
    }

    /*
    QString pipe0("/proc/xenomai/registry/native/pipes/sine");
    QString pipe1("/proc/xenomai/registry/native/pipes/square");
    QString pipe2("/proc/xenomai/registry/native/pipes/pwm");
    //QString pipe3("/proc/xenomai/registry/native/pipes/sig4");

    OpSettings::instance().addChannel(pipe0);
    OpSettings::instance().addChannel(pipe1);
    OpSettings::instance().addChannel(pipe2);
    //OpSettings::instance().addChannel(pipe3);
    */

    InputMap a;
    if(!a.exec())
        return 0;

    MainWindow window;

    //window.resize(800,400);
    window.resize(950,500);

/*
    OpSettings::instance().setCoupling(0,0);
    OpSettings::instance().setGain(0,4.5);
    OpSettings::instance().setPos(0,8);

    OpSettings::instance().setCoupling(1,1);
    OpSettings::instance().setGain(1,2123.2);
    OpSettings::instance().setPos(1,-123);
    */
/*
    qDebug() << OpSettings::instance().channelPipe(0) << "-" << OpSettings::instance().getCoupling(0);

    qDebug() << OpSettings::instance().channelPipe(1) << "-" << OpSettings::instance().getCoupling(1);
*/

    SamplingThread samplingThread;
    samplingThread.setInterval(window.signalInterval());

    window.connect(&window, SIGNAL(sampPeriodChanged(double)),
        &samplingThread, SLOT(setInterval(double)));

    Plot *plotz=window.d_plot;
    QObject::connect(&samplingThread, SIGNAL(Triggered()),
                     plotz, SLOT(turnOn()) );

    QObject::connect(&samplingThread, SIGNAL(newVMax(int,double)),
                     &window, SLOT(setVMax(int,double)));
    QObject::connect(&samplingThread, SIGNAL(newVp(int,double)),
                     &window, SLOT(setVp(int,double)));
    QObject::connect(&samplingThread, SIGNAL(newVpp(int,double)),
                     &window, SLOT(setVpp(int,double)));
    QObject::connect(&samplingThread, SIGNAL(newPeriod(int,double)),
                     &window, SLOT(setPeriod(int,double)));



    QObject::connect(&(OpSettings::instance()), SIGNAL(turnOn()),
                     plotz, SLOT(turnOn()) );
    QObject::connect(&(OpSettings::instance()), SIGNAL(turnOn()),
                     &samplingThread, SLOT(turnOn()) );


    QObject::connect(&(OpSettings::instance()), SIGNAL(turnOff()),
                     plotz, SLOT(turnOff()) );
    QObject::connect(&(OpSettings::instance()), SIGNAL(turnOff()),
                     &samplingThread, SLOT(turnOff()) );

    QObject::connect(&(OpSettings::instance()), SIGNAL(liveSwitch(bool)),
                     plotz, SLOT(liveSwitch(bool)) );

    //QObject::connect(&window, SIGNAL(statusChanged(bool)),
    //                 &samplingThread, SLOT(setOperation(bool)) );
    window.show();

    //&(OpSettings::instance())(setStatus(true)) );
    OpSettings::instance().setStatus(true);
    //OpSettings::instance().setState(OpSettings::Plot);
    //samplingThread.start();
    //window.start();

    bool ok = app.exec(); 

    samplingThread.stop();
    samplingThread.wait(1000);

    return ok;
}
