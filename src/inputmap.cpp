#include "inputmap.h"

InputMap::InputMap(QWidget *parent) :
    QDialog(parent)
{
    setBasicLayout();

    //Generate options for Input Channel comboBox
    QStringList options;
    OpSettings* op=&OpSettings::instance();
    int numInputs=op->lastChannel();

    for(int i=0;i<op->lastChannel();i++)
        options << QString::number(i)+" - "+op->channelPipe(i);

    options << "--";

    //create comboBoxes and add them to masterGrid
    for(int i=0;i<5;i++){
        QComboBox* box= new QComboBox(this);
        box->addItems(options);
        box->setCurrentIndex((i>numInputs)?numInputs:i);

        masterGrid->addWidget(box,i+1,1);
        inputComboList << box;

        connect(box, SIGNAL(currentIndexChanged(int)),
                this, SLOT(setInput(int)) );
    }

    QStringList rowOptions;
    for(int i=0;i<5;i++){
        rowOptions << QString::number(i);
    }

    for(int i=0;i<5;i++){
        QComboBox* box = new QComboBox(this);
        box->addItems(rowOptions);

        masterGrid->addWidget(box,i+1,2);
        rowComboList << box;

        connect(box, SIGNAL(currentIndexChanged(int)),
                this, SLOT(setRow(int)) );
    }


    for(int i=0;i<5;i++){
        QComboBox* box = new QComboBox(this);
        box->addItems(rowOptions);

        masterGrid->addWidget(box,i+1,3);
        columnComboList << box;

        connect(box, SIGNAL(currentIndexChanged(int)),
                this, SLOT(setColumn(int)) );
    }




    setWindowTitle("Input map");
    setFixedHeight(sizeHint().height());
}

void InputMap::setBasicLayout()
{
    outterHolder = new QVBoxLayout;

    introLabel = new QLabel("Please choose an input map.",this);
    ch0Label = new QLabel("Channel 0",this);
    ch1Label = new QLabel("Channel 1",this);
    ch2Label = new QLabel("Channel 2",this);
    ch3Label = new QLabel("Channel 3",this);
    ch4Label = new QLabel("Channel 4",this);
    inputChannelLabel = new QLabel("Input Channel",this);
    rowLabel = new QLabel("Row",this);
    columnLabel = new QLabel("Column",this);

    masterGrid = new QGridLayout;
    masterGrid->addWidget(ch0Label,1,0);
    masterGrid->addWidget(ch1Label,2,0);
    masterGrid->addWidget(ch2Label,3,0);
    masterGrid->addWidget(ch3Label,4,0);
    masterGrid->addWidget(ch4Label,5,0);

    masterGrid->addWidget(inputChannelLabel,0,1);
    masterGrid->addWidget(rowLabel,0,2);
    masterGrid->addWidget(columnLabel,0,3);

    buttons = new QDialogButtonBox(
                QDialogButtonBox::Ok|
                QDialogButtonBox::Cancel,
                Qt::Horizontal,this);
    connect(buttons, SIGNAL(accepted()),
            this, SLOT(accept()) );
    connect(buttons, SIGNAL(rejected()),
            this, SLOT(reject()) );


    outterHolder->addWidget(introLabel);
    outterHolder->addLayout(masterGrid);
    outterHolder->addWidget(buttons);

    setLayout(outterHolder);
}

void InputMap::setInput(int index)
{
    //Find out channel
    int i;
    for(i=0;i<5;i++){
        if(inputComboList.at(i)==QObject::sender())
            break;

    }

    qDebug() << "Channel " << i << "Input :" << index;
}

void InputMap::setColumn(int index)
{
    //Find out channel
    int i;
    for(i=0;i<5;i++){
        if(columnComboList.at(i)==QObject::sender())
            break;

    }

    qDebug() << "Channel " << i << "Column :" << index;
}

void InputMap::setRow(int index)
{
    //Find out channel
    int i;
    for(i=0;i<5;i++){
        if(rowComboList.at(i)==QObject::sender())
            break;

    }

    qDebug() << "Channel " << i << "Row :" << index;
}
