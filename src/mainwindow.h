#include <qwidget.h>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class Plot;
class Knob;
class WheelBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();


    double signalInterval() const;

    Plot *d_plot;
    QPushButton* d_status;

Q_SIGNALS:
    void sampPeriodChanged(double);//this is the actual sampling thread period

    void timeDivChanged(double);
    void ampDivChanged(double);

    //POG-intermidiate between sampling thread and plot
    void forceReset();

    void statusChanged(bool);

    void chColorChange(int,QColor);

public Q_SLOTS:

    void setampDiv(double);
    void setChPos(double);
    void setChGain(double);
    void setChChoice(int);
    void setChCoupling(int);
    void setChColor(QString);


    void setHorPos(double);
    void setTimeDiv(double);

    void setPeriod(int,double);
    void setVMax(int,double);
    void setVp(int,double);
    void setVpp(int,double);

    void setTriggerLevel(double);

    void statusChange();


    //POG-intermidiate between sampling thread and plot
    void callReset();


private:
    //Channel
    Knob *d_chPosKnob;
    QDoubleSpinBox *d_chPosSp;
    Knob *d_chGainKnob;
    QDoubleSpinBox *d_chGainSp;
    QComboBox * d_chChoose;
    int d_chChoice;
    Knob *d_ampDivKnob;
    QDoubleSpinBox *d_ampDivSp;
    double d_ampDiv;
    QComboBox *d_chCoupling;
    QLabel** d_chColors;
    QComboBox** d_chColorBox;

    //horizontal
    Knob *d_timeDivKnob;
    QDoubleSpinBox *d_timeDivSp;
    double d_timeDiv;
    Knob *d_horPosKnob;
    QDoubleSpinBox *d_horPosSp;


    //trigger
    Knob *d_triggerKnob;
    QDoubleSpinBox *d_triggerSp;
    QComboBox *d_triggerSlope;
    QComboBox *d_triggerSource;

    //General
    QDoubleSpinBox *d_sampPeriodSp;//sampling period
    QSpinBox *d_refreshRate;
    QComboBox *d_background;
    QComboBox *d_operation;

    QLabel** d_chVMax;
    QLabel** d_chVp;
    QLabel** d_chVpp;
    QLabel** d_chPeriod;
    QLabel** d_chFrequency;


};
