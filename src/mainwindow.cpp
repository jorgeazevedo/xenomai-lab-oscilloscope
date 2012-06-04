#include "mainwindow.h"
#include "signaldata.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QtGlobal>

#include "operationsettings.h"

MainWindow::MainWindow(QWidget *parent):
    QWidget(parent)
{
    //this is gonna be used all over this constructor to run
    //thru all the different channels in for cycles
    int numChannels=OpSettings::instance().lastChannel();

    d_chChoice=0;
    const double intervalLength = 1.0; // seconds

    d_plot = new Plot(this);
    d_plot->setIntervalLength(intervalLength);


    //d_timerWheel = new WheelBox("Sampling Period [ms]", 0.0, 20.0, 0.1, this);
    d_sampPeriodSp = new QDoubleSpinBox(this);
    d_sampPeriodSp->setValue(20.0);
    d_sampPeriodSp->setMinimum(0.1);
    d_sampPeriodSp->setMaximum(100);

    d_refreshRate = new QSpinBox(this);
    d_refreshRate->setValue(30);
    d_refreshRate->setMinimum(1);
    d_refreshRate->setMaximum(300);

    //Time Div combo
    d_timeDiv=intervalLength;
    d_timeDivKnob = new Knob("Time Div [ms]", 0.0001, 3.0, this);
    d_timeDivKnob->setValue(intervalLength);
    d_timeDivSp = new QDoubleSpinBox(this);
    d_timeDivSp->setMinimum(0.0001);
    d_timeDivSp->setMaximum(50);
    d_timeDivSp->setValue(intervalLength);
    d_timeDiv=intervalLength;

    d_chPosKnob = new Knob("Vertical Position", -200, 200.0, this);
    d_chPosKnob->setValue(0);
    d_chPosSp = new QDoubleSpinBox(this);
    d_chPosSp->setMinimum(-200);
    d_chPosSp->setMaximum(200);
    d_chPosSp->setValue(0);

    d_chGainKnob = new Knob("Gain", 0.001, 10.0, this);
    d_chGainKnob->setValue(1);
    d_chGainSp = new QDoubleSpinBox(this);
    d_chGainSp->setMinimum(0.001);
    d_chGainSp->setMaximum(10);
    d_chGainSp->setValue(1);


    d_ampDivKnob = new Knob("Amplitude Div [V]", 0.0, 500.0, this);
    d_ampDivKnob->setValue(200);
    d_ampDivSp = new QDoubleSpinBox(this);
    d_ampDivSp->setMinimum(1);
    d_ampDivSp->setMaximum(500);
    d_ampDivSp->setValue(200);

    QLabel *ch0_coupling = new QLabel();
    ch0_coupling->setText("Coupling:");
    ch0_coupling->setAlignment(Qt::AlignCenter);

    d_chCoupling = new QComboBox(this);
    d_chCoupling->insertItem(0,"AC");
    d_chCoupling->insertItem(1,"DC");
    d_chCoupling->setCurrentIndex(1);

    d_horPosKnob = new Knob("Horizontal Position", -1, 1, this);
    d_horPosKnob->setValue(0);

    d_horPosSp = new QDoubleSpinBox(this);
    d_horPosSp->setMinimum(-1);
    d_horPosSp->setMaximum(1);
    d_horPosSp->setValue(0);

    d_triggerKnob = new Knob("Level [%]", -100.0, 100.0, this);
    d_triggerKnob->setValue(0);

    d_triggerSp = new QDoubleSpinBox(this);
    d_triggerSp->setMinimum(-100);
    d_triggerSp->setMaximum(100);
    d_triggerSp->setValue(0);

    d_operation = new QComboBox(this);
    d_operation->insertItem(0,"Continuous");
    d_operation->insertItem(1,"Triggered");
    d_operation->setCurrentIndex(0);

    d_status = new QPushButton(this);
    d_status->setText("Off");


    QLabel *trigger_slope = new QLabel();
    trigger_slope->setText("Slope:");
    trigger_slope->setAlignment(Qt::AlignCenter);

    d_triggerSlope= new QComboBox(this);
    d_triggerSlope->insertItem(0,"Rising");
    d_triggerSlope->insertItem(1,"Falling");

    QLabel *trigger_source = new QLabel();
    trigger_source->setText("Source:");
    trigger_source->setAlignment(Qt::AlignCenter);

    d_triggerSource = new QComboBox(this);

    for(int i=0;i<OpSettings::instance().lastChannel();i++)
        d_triggerSource->insertItem(i,QString("%1 %2").arg("Channel").arg(i));

    d_background = new QComboBox(this);
    d_background->insertItem(0,"Blue");
    d_background->insertItem(1,"White");
    d_background->insertItem(2,"Green");
/*
    d_ch0Pipe = new QComboBox(this);
    d_ch0Pipe->insertItem(0,"pid_ouput1");
    d_ch0Pipe->insertItem(1,"pid_outpu2");
    d_ch0Pipe->insertItem(2,"Refresh");
    //d_ch0Pipe->setMinimumWidth(d_ch0Pipe->minimumSizeHint().width());
*/
    QLabel *labelHorizontal = new QLabel(this);
    //label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    labelHorizontal->setTextFormat(Qt::RichText);
    labelHorizontal->setText("<b>Axis</b>");
    labelHorizontal->setAlignment(Qt::AlignHCenter);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);



    QLabel *labelSignal = new QLabel(this);
    labelSignal->setTextFormat(Qt::RichText);
    labelSignal->setText("<b>Signal</b>");
    labelSignal->setAlignment(Qt::AlignHCenter);

    QLabel *ch_choose = new QLabel();
    ch_choose->setText("Signal:");
    ch_choose->setAlignment(Qt::AlignCenter);

    d_chChoose = new QComboBox(this);
    for(int i=0;i<OpSettings::instance().lastChannel();i++)
        d_chChoose->insertItem(i,QString("%1 %2").arg("Channel").arg(i));

    d_chChoose->setCurrentIndex(d_chChoice);

    //labelVertical->setAlignment(Qt::AlignHCenter);


//trigger
    QLabel *labelTrigger = new QLabel(this);
    labelTrigger->setTextFormat(Qt::RichText);
    labelTrigger->setText("<b>Trigger</b>");
    labelTrigger->setAlignment(Qt::AlignHCenter);

//INFO

    /*
    QLabel *global1 = new QLabel();
    global1->setTextFormat(Qt::RichText);
    global1->setText("<b>Global</b>");
    global1->setAlignment(Qt::AlignRight);
    */

    QLabel *sampPeriod = new QLabel();
    sampPeriod->setTextFormat(Qt::RichText);
    sampPeriod->setText("Sampling P. [<i>ms</i>]");
    sampPeriod->setAlignment(Qt::AlignHCenter);

    QLabel *background = new QLabel();
    background->setTextFormat(Qt::RichText);
    background->setText("Background");
    background->setAlignment(Qt::AlignHCenter);

    QLabel *operation = new QLabel();
    operation->setTextFormat(Qt::RichText);
    operation->setText("Operation");
    operation->setAlignment(Qt::AlignHCenter);

    QLabel *status = new QLabel();
    status->setTextFormat(Qt::RichText);
    status->setText("Status");
    status->setAlignment(Qt::AlignHCenter);

    QLabel *refreshRate = new QLabel();
    refreshRate->setTextFormat(Qt::RichText);
    refreshRate->setText("Refresh Rate [<i>ms</i>]");
    refreshRate->setAlignment(Qt::AlignHCenter);
/*
    QLabel *ch0_pipe = new QLabel(this);
    ch0_pipe->setTextFormat(Qt::RichText);
    ch0_pipe->setText("Pipe");
    ch0_pipe->setAlignment(Qt::AlignHCenter);
*/

    QLabel *ch_freq = new QLabel(this);
    ch_freq->setTextFormat(Qt::RichText);
    ch_freq->setText("Frequency [<i>Hz</i>]");
    ch_freq->setAlignment(Qt::AlignHCenter);



    d_chFrequency = new QLabel*[numChannels];
    for(int i=0;i<numChannels;i++){
        d_chFrequency[i]=new QLabel(this);
        d_chFrequency[i]->setTextFormat(Qt::RichText);
        d_chFrequency[i]->setText("<i>--,--</i>");
        d_chFrequency[i]->setAlignment(Qt::AlignHCenter);
    }


    QLabel *ch_period = new QLabel(this);
    ch_period->setTextFormat(Qt::RichText);
    ch_period->setText("Period [<i>ms</i>]");
    ch_period->setAlignment(Qt::AlignHCenter);

    d_chPeriod = new QLabel*[numChannels];
    for(int i=0;i<numChannels;i++){
        d_chPeriod[i]=new QLabel(this);
        d_chPeriod[i]->setTextFormat(Qt::RichText);
        d_chPeriod[i]->setText("<i>--,--</i>");
        d_chPeriod[i]->setAlignment(Qt::AlignHCenter);
    }

    QLabel *ch_vp = new QLabel(this);
    ch_vp->setTextFormat(Qt::RichText);
    ch_vp->setText("V<sub>p</sub");
    ch_vp->setAlignment(Qt::AlignHCenter);

    d_chVp = new QLabel*[numChannels];
    for(int i=0;i<numChannels;i++){
        d_chVp[i]=new QLabel(this);
        d_chVp[i]->setTextFormat(Qt::RichText);
        d_chVp[i]->setText("<i>--,--</i>");
        d_chVp[i]->setAlignment(Qt::AlignHCenter);
    }

    QLabel *ch_vpp = new QLabel(this);
    ch_vpp->setTextFormat(Qt::RichText);
    ch_vpp->setText("V<sub>pp</sub>");
    ch_vpp->setAlignment(Qt::AlignHCenter);

    d_chVpp = new QLabel*[numChannels];
    for(int i=0;i<numChannels;i++){
        d_chVpp[i]=new QLabel(this);
        d_chVpp[i]->setTextFormat(Qt::RichText);
        d_chVpp[i]->setText("<i>--,--</i>");
        d_chVpp[i]->setAlignment(Qt::AlignHCenter);
    }

    QLabel *ch_vmax = new QLabel(this);
    ch_vmax->setTextFormat(Qt::RichText);
    ch_vmax->setText("V<sub>MAX</sub>");
    ch_vmax->setAlignment(Qt::AlignHCenter);

    d_chVMax = new QLabel*[numChannels];
    for(int i=0;i<numChannels;i++){
        d_chVMax[i]=new QLabel(this);
        d_chVMax[i]->setTextFormat(Qt::RichText);
        d_chVMax[i]->setText("<i>--,--</i>");
        d_chVMax[i]->setAlignment(Qt::AlignHCenter);
    }

    QHBoxLayout *whole = new QHBoxLayout(this);

    QVBoxLayout *infoNKnobs = new QVBoxLayout();

    QTabWidget *topTabs = new QTabWidget();

    QWidget* operationTab = new QWidget();
    QWidget* settingsTab = new QWidget();
    QWidget* measurementsTab = new QWidget();

    QGroupBox *globalGroup1 = new QGroupBox("Global");
    QGroupBox *globalGroup2 = new QGroupBox("Global");

    QGroupBox *signalGroup1 = new QGroupBox("Signal");
    QGroupBox *signalGroup2 = new QGroupBox("Signal");
    QGroupBox *signalGroup3 = new QGroupBox("Signal");

    QLabel *chLabel[10][3];
    for(int i=0;i<numChannels;i++){
        for(int j=0;j<3;j++){
            chLabel[i][j] = new QLabel(QString("%1 %2").arg("Channel").arg(i));
            chLabel[i][j]->setAlignment(Qt::AlignCenter);
        }
    }

    QLabel* chPipeTitle1 = new QLabel("Pipe");
    chPipeTitle1->setAlignment(Qt::AlignCenter);
    QLabel* chColorTitle1 = new QLabel("Color");
    chColorTitle1->setAlignment(Qt::AlignCenter);

    QLabel* chColorTitle2 = new QLabel("Color");
    chColorTitle2->setAlignment(Qt::AlignCenter);


    /*
     * Create QGroupBoxes for the various channels
     * Titles are Channel X
     * we need three of them, one for each tab, hence
     * chGroup[CHANNEL][TAB]
     */

    QGroupBox *chGroup[3][3];
    for(int i=0;i<OpSettings::instance().lastChannel();i++){
            for(int j=0;j<3;j++)
                 chGroup[i][j] = new QGroupBox(QString("%1 %2").arg("Channel").arg(i));

    }

    QVBoxLayout* vTab1= new QVBoxLayout();
    QVBoxLayout* vTab2= new QVBoxLayout();
    QVBoxLayout* vTab3= new QVBoxLayout();

    QGridLayout *globalOperation = new QGridLayout();
    QGridLayout *signalOperation = new QGridLayout();
    //QGridLayout *ch0Operation = new QGridLayout();
    QGridLayout *globalSettings = new QGridLayout();
    QGridLayout *signalSettings = new QGridLayout();
    //QGridLayout *ch0Settings = new QGridLayout();
    QGridLayout *ch0Measurements = new QGridLayout();
    QGridLayout *signalMeasurements = new QGridLayout();

    QHBoxLayout *controls = new QHBoxLayout();

    QVBoxLayout* vHorizontal = new QVBoxLayout();

    QVBoxLayout* vVertical= new QVBoxLayout();

    QHBoxLayout *vVerticalCoupling = new QHBoxLayout();
    QHBoxLayout *vVerticalChChoose = new QHBoxLayout();

    QVBoxLayout* vTrigger= new QVBoxLayout();

    QHBoxLayout *vTriggerSlope = new QHBoxLayout();

    QHBoxLayout *vTriggerSource = new QHBoxLayout();


    whole->addWidget(d_plot,50);
    whole->addLayout(infoNKnobs,50);

    infoNKnobs->addWidget(topTabs,25);
    infoNKnobs->addStretch(30);
    infoNKnobs->addLayout(controls,85);

    //Top

    topTabs->addTab(operationTab,"Operation");
    topTabs->addTab(settingsTab,"Settings");
    topTabs->addTab(measurementsTab,"Measurements");

    operationTab->setLayout(vTab1);
    settingsTab->setLayout(vTab2);
    measurementsTab->setLayout(vTab3);

    vTab1->addWidget(globalGroup1);
    vTab1->addWidget(signalGroup1);

    //for(int i=0;i<OpSettings::instance().lastChannel();i++){
    //    vTab1->addWidget(chGroup[i][0]);
   // }

    globalGroup1->setLayout(globalOperation);
    globalOperation->addWidget(operation,0,0);
    globalOperation->addWidget(d_operation,1,0);
    globalOperation->addWidget(status,0,1);
    globalOperation->addWidget(d_status,1,1);
    globalOperation->setAlignment(Qt::AlignCenter);

    signalGroup1->setLayout(signalOperation);
    signalOperation->addWidget(chPipeTitle1,0,1);
    signalOperation->addWidget(chColorTitle1,0,2);

    /*
     * Populate signalOperation with channel information
     */

    QLabel** chPipe= new QLabel*[OpSettings::instance().lastChannel()];
    d_chColors = new QLabel*[OpSettings::instance().lastChannel()];

    for(int i=0;i<OpSettings::instance().lastChannel();i++){
        signalOperation->addWidget(chLabel[i][0],i+1,0);

        chPipe[i]=new QLabel(OpSettings::instance().channelPipe(i));
        chPipe[i]->setAlignment(Qt::AlignHCenter);
        signalOperation->addWidget(chPipe[i],i+1,1);

        //chColors[i]=new QLabel(OpSettings::instance().channelColor(i));
        d_chColors[i]=new QLabel(OpSettings::instance().getColorName(i));
        d_chColors[i]->setAlignment(Qt::AlignHCenter);
        signalOperation->addWidget(d_chColors[i],i+1,2);
    }

/*
    chGroup[0][0]->setLayout(ch0Operation);
    ch0Operation->addWidget(ch0_pipe,0,0);
    ch0Operation->addWidget(d_ch0Pipe,1,0);
    ch0Operation->setAlignment(Qt::AlignCenter);
*/

    vTab2->addWidget(globalGroup2);
    vTab2->addWidget(signalGroup2);


    globalGroup2->setLayout(globalSettings);
    globalSettings->addWidget(sampPeriod,0,1);
    globalSettings->addWidget(d_sampPeriodSp,1,1);
    globalSettings->addWidget(refreshRate,0,2);
    globalSettings->addWidget(d_refreshRate,1,2);
    globalSettings->addWidget(background,0,3);
    globalSettings->addWidget(d_background,1,3);
    globalSettings->setAlignment(Qt::AlignCenter);

    signalGroup2->setLayout(signalSettings);
    signalSettings->addWidget(chColorTitle2,0,1);

    //QLabel** chPipe= new QLabel*[OpSettings::instance().lastChannel()];
    d_chColorBox= new QComboBox*[OpSettings::instance().lastChannel()];

    for(int i=0;i<OpSettings::instance().lastChannel();i++){
        signalSettings->addWidget(chLabel[i][1],i+1,0);

        d_chColorBox[i]=new QComboBox();
        d_chColorBox[i]->insertItem(0,"green");
        d_chColorBox[i]->insertItem(1,"white");
        d_chColorBox[i]->insertItem(2,"cyan");
        d_chColorBox[i]->insertItem(3,"yellow");
        d_chColorBox[i]->insertItem(4,"lightGray");
        d_chColorBox[i]->insertItem(5,"darkGray");
        d_chColorBox[i]->insertItem(6,"red");
        d_chColorBox[i]->insertItem(7,"black");
        d_chColorBox[i]->insertItem(8,"blue");
        d_chColorBox[i]->insertItem(9,"magenta");
        d_chColorBox[i]->setCurrentIndex(i);
        signalSettings->addWidget(d_chColorBox[i],i+1,1);

        connect(d_chColorBox[i],SIGNAL(currentIndexChanged(QString)),
                this,SLOT(setChColor(QString)));

    }


    vTab3->addWidget(signalGroup3);

    signalGroup3->setLayout(signalMeasurements);
    /*
     * Populate Measurements tab with channel X labels
     */
    for(int i=0;i<numChannels;i++){
        signalMeasurements->addWidget(chLabel[i][2],i+1,0);
        signalMeasurements->addWidget(d_chFrequency[i],i+1,1);
        signalMeasurements->addWidget(d_chPeriod[i],i+1,2);
        signalMeasurements->addWidget(d_chVp[i],i+1,3);
        signalMeasurements->addWidget(d_chVpp[i],i+1,4);
        signalMeasurements->addWidget(d_chVMax[i],i+1,5);

    }
    signalMeasurements->addWidget(ch_freq,0,1);

    signalMeasurements->addWidget(ch_period,0,2);

    signalMeasurements->addWidget(ch_vp,0,3);

    signalMeasurements->addWidget(ch_vpp,0,4);

    signalMeasurements->addWidget(ch_vmax,0,5);



    signalMeasurements->setAlignment(Qt::AlignCenter);


    //Controls

    //vVertical->addStretch(30);
    vVertical->addWidget(labelSignal);
    vVertical->addWidget(d_chPosKnob);
    vVertical->addWidget(d_chPosSp);
    vVertical->addWidget(d_chGainKnob);
    vVertical->addWidget(d_chGainSp);
    vVertical->addLayout(vVerticalChChoose);

    vVertical->addLayout(vVerticalCoupling);
    vVertical->addStretch(10);

    vVerticalCoupling->addWidget(ch0_coupling);
    vVerticalCoupling->addWidget(d_chCoupling);

    vVerticalChChoose->addWidget(ch_choose);
    vVerticalChChoose->addWidget(d_chChoose);

    vHorizontal->addWidget(labelHorizontal);
    vHorizontal->addWidget(d_ampDivKnob);
    vHorizontal->addWidget(d_ampDivSp);
    vHorizontal->addWidget(d_timeDivKnob);
    vHorizontal->addWidget(d_timeDivSp);
    vHorizontal->addStretch(10);

    //vTrigger->addStretch(30);
    vTrigger->addWidget(labelTrigger);
    vTrigger->addWidget(d_triggerKnob);
    vTrigger->addWidget(d_triggerSp);
    vTrigger->addWidget(d_horPosKnob);
    vTrigger->addWidget(d_horPosSp);
    vTrigger->addLayout(vTriggerSlope);
    vTrigger->addLayout(vTriggerSource);
    vTrigger->addStretch(10);

    vTriggerSlope->addWidget(trigger_slope);
    vTriggerSlope->addWidget(d_triggerSlope);

    vTriggerSource->addWidget(trigger_source);
    vTriggerSource->addWidget(d_triggerSource);

    controls->addLayout(vVertical);
    controls->addLayout(vHorizontal);
    controls->addLayout(vTrigger);


//ch0AMPLITUDE
    //ch0 Position Knob
    connect(d_chPosKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setChPos(double)) );
    connect(d_chPosSp, SIGNAL(valueChanged(double)),
            this, SLOT(setChPos(double)) );
    //ch Gain Knob
    connect(d_chGainKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setChGain(double)) );
    connect(d_chGainSp, SIGNAL(valueChanged(double)),
            this, SLOT(setChGain(double)) );

    //ch0 Div Knob
    connect(d_ampDivKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setampDiv(double)));
    connect(d_ampDivSp, SIGNAL(valueChanged(double)),
            this,SLOT(setampDiv(double)) );
    connect(this, SIGNAL(ampDivChanged(double)),
            d_plot, SLOT(setYaxis(double)) );

    //Ch Choose
    connect(d_chChoose,SIGNAL(currentIndexChanged(int)),
            this,SLOT(setChChoice(int)));

    //ch0 Coupling
    connect(d_chCoupling,SIGNAL(currentIndexChanged(int)),
            this,SLOT(setChCoupling(int)) );
            //&(OpSettings::instance()),SLOT(setchCoupling(int)) );

    //ch color
    connect(this,SIGNAL(chColorChange(int,QColor)),
            d_plot,SLOT(setLineColor(int,QColor)));

//TIME
    //Time Div Knob
    connect(d_timeDivKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setTimeDiv(double)) );
    connect(d_timeDivSp, SIGNAL(valueChanged(double)),
            this,SLOT(setTimeDiv(double)) );
    connect(this,SIGNAL(timeDivChanged(double)),
            d_plot,SLOT(setIntervalLength(double)) );


    //Horizontal Position Knob
    connect(d_horPosKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setHorPos(double)) );
    connect(d_horPosSp, SIGNAL(valueChanged(double)),
            this, SLOT(setHorPos(double)) );

//TRIGGER
    //Level knob
    connect(d_triggerKnob, SIGNAL(valueChanged(double)),
            this,SLOT(setTriggerLevel(double)) );
    connect(d_triggerSp, SIGNAL(valueChanged(double)),
            this, SLOT(setTriggerLevel(double)) );

    connect(d_triggerSlope,SIGNAL(currentIndexChanged(int)),
            &(OpSettings::instance()),SLOT(setTriggerSlope(int)) );

    //Trigger Source
    connect(d_triggerSource,SIGNAL(currentIndexChanged(int)),
            &(OpSettings::instance()),SLOT(setTriggerSource(int)) );

//OTHERS

    connect(this,SIGNAL(forceReset()),
            d_plot,SLOT(resetAxis()));

    connect(d_sampPeriodSp, SIGNAL(valueChanged(double)),
        SIGNAL(sampPeriodChanged(double)));

    connect(d_refreshRate, SIGNAL(valueChanged(int)),
            d_plot, SLOT(setRefreshRate(int)) );

    connect(d_operation,SIGNAL(currentIndexChanged(int)),
            &(OpSettings::instance()),SLOT(setOperation(int)) );

    connect(d_status, SIGNAL(clicked()),
            this, SLOT(statusChange()));
    connect(this, SIGNAL(statusChanged(bool)),
            &(OpSettings::instance()), SLOT(setStatus(bool)) );

/*
    connect(d_ch0Color, SIGNAL(currentIndexChanged(int)),
            d_plot, SLOT(setLineColor(int)));
    */
    connect(d_background, SIGNAL(currentIndexChanged(int)),
            d_plot, SLOT(initGradient(int)) );

}


//POG-intermidiate between sampling thread and plot
void MainWindow::callReset(){
    Q_EMIT forceReset();
}

void MainWindow::setTriggerLevel(double value)
{
    OpSettings::instance().setTriggerLevel(value);

    //align knob and spinbox
    if(d_triggerKnob->value()!=value)
        d_triggerKnob->setValue(value);
    if(d_triggerSp->value()!=value)
        d_triggerSp->setValue(value);
}

void MainWindow::statusChange()
{
    if(d_status->text()=="Off"){
        d_status->setText("On");
        Q_EMIT statusChanged(false);
    }
    else{
        d_status->setText("Off");
        Q_EMIT statusChanged(true);
    }

}

void MainWindow::setampDiv(double value)
{
    d_ampDiv=value;
    //align knob and spinbox
    if(d_ampDivKnob->value()!=value)
        d_ampDivKnob->setValue(value);
    if(d_ampDivSp->value()!=value)
        d_ampDivSp->setValue(value);

    //This will then call plot's setYaxis slot
    Q_EMIT ampDivChanged(value);
}

void MainWindow::setChPos(double value)
{
    OpSettings::instance().setPos(d_chChoice,value);

    //align knob and spinbox
    if(d_chPosKnob->value()!=value)
        d_chPosKnob->setValue(value);
    if(d_chPosSp->value()!=value)
        d_chPosSp->setValue(value);

}

void MainWindow::setChGain(double value)
{
    OpSettings::instance().setGain(d_chChoice,value);

    //align knob and spinbox
    if(d_chGainKnob->value()!=value)
        d_chGainKnob->setValue(value);
    if(d_chGainSp->value()!=value)
        d_chGainSp->setValue(value);

}

void MainWindow::setChCoupling(int value)
{

    OpSettings::instance().setCoupling(d_chChoice,value);

    if(d_chCoupling->currentIndex()!=value)
        d_chCoupling->setCurrentIndex(value);
}

void MainWindow::setChColor(QString color)
{
    //Find out which channel needs color changing
    int i;
    for(i=0;i<OpSettings::instance().lastChannel();i++){
        if(d_chColorBox[i]==QObject::sender())
            break;
    }

    OpSettings::instance().setColor(i,color);
    d_chColors[i]->setText(color);
    Q_EMIT chColorChange(i,color);


        /*
    OpSettings::instance().setCoupling(d_chChoice,value);

    if(d_chCoupling->currentIndex()!=value)
        d_chCoupling->setCurrentIndex(value);
    */
}

void MainWindow::setChChoice(int value)
{
    d_chChoice=value;

    setChGain(OpSettings::instance().getGain(d_chChoice));
    setChPos(OpSettings::instance().getPos(d_chChoice));
    setChCoupling(OpSettings::instance().getCoupling(d_chChoice));

}



void MainWindow::setHorPos(double value)
{
    OpSettings::instance().setHorPos(value);

    //align knob and spinbox
    if(d_horPosKnob->value()!=value)
        d_horPosKnob->setValue(value);
    if(d_horPosSp->value()!=value)
        d_horPosSp->setValue(value);


}

void MainWindow::setTimeDiv(double value)
{
    d_timeDiv=value;
    //align knob and spinbox
    if(d_timeDivKnob->value()!=value)
        d_timeDivKnob->setValue(value);
    if(d_timeDivSp->value()!=value)
        d_timeDivSp->setValue(value);

    //This will then call plot's setIntervalLength slot
    Q_EMIT timeDivChanged(value);

}

void MainWindow::setPeriod(int channel, double value)
{
    d_chFrequency[channel]->setText(QString("%1 %2").arg(1/value, 0, 'f', 2 ).arg("Hz"));
    d_chPeriod[channel]->setText(QString("%1 %2").arg(value, 0, 'f', 2 ).arg("s"));
}

void MainWindow::setVMax(int channel, double value)
{
    //qDebug() << "Channel" << channel << "Vmax:" << value;
    d_chVMax[channel]->setText(QString("%1").arg(value, 0, 'f', 2 ));
}

void MainWindow::setVp(int channel, double value)
{
    //qDebug() << "Channel" << channel << "Vp:" << value;
    d_chVp[channel]->setText(QString("%1").arg(value, 0, 'f', 2 ));
}

void MainWindow::setVpp(int channel, double value)
{
    //qDebug() << "Channel" << channel << "Vpp:" << value;
    d_chVpp[channel]->setText(QString("%1").arg(value, 0, 'f', 2 ));
}



void MainWindow::start()
{
    d_plot->start();
}


double MainWindow::signalInterval() const
{
    return d_sampPeriodSp->value();
}
