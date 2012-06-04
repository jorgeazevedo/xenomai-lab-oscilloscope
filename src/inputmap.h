#ifndef INPUTMAP_H
#define INPUTMAP_H

#include "opsettings.h"
#include <QList>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QComboBox>
#include <QAction>
#include <QDebug>

class InputMap : public QDialog
{
    Q_OBJECT
public:
    explicit InputMap(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
    void setColumn(int index);
    void setRow(int index);
    void setInput(int index);

private:

    void setBasicLayout();
    QVBoxLayout* outterHolder;
    QLabel* introLabel;
    QLabel* ch0Label;
    QLabel* ch1Label;
    QLabel* ch2Label;
    QLabel* ch3Label;
    QLabel* ch4Label;
    QLabel* inputChannelLabel;
    QLabel* rowLabel;
    QLabel* columnLabel;
    QList<QComboBox*> inputComboList;
    QList<QComboBox*> rowComboList;
    QList<QComboBox*> columnComboList;

    QGridLayout* masterGrid;
    QDialogButtonBox* buttons;

};

#endif // INPUTMAP_H
