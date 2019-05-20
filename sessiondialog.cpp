/***************************************************************************
 *            sessiondialog.cpp
 *
 *  ven apr 23 19:48:20 CEST 2010
 *  Copyright  2010  Simone Tobia
 *  Email simone.tobia@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "sessiondialog.h"
#include "ui_sessiondialog.h"

SessionDialog::SessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionDialog)
{
    ui->setupUi(this);

    ui->diffCombo1->insertItem(0,"STUPID");
    ui->diffCombo2->insertItem(0,"STUPID");
    ui->diffCombo1->insertItem(1,"NORMAL");
    ui->diffCombo2->insertItem(1,"NORMAL");
    ui->diffCombo1->insertItem(2,"SMART");
    ui->diffCombo2->insertItem(2,"SMART");

    ui->diffCombo1->insertItem(3,"HARDCORE");
    ui->diffCombo2->insertItem(3,"HARDCORE");

    ui->pen1color->setStandardColors();
    ui->pen2color->setStandardColors();
    ui->wincolor->setStandardColors();
    ui->gridcolor->setStandardColors();
    ui->pen1color->setCurrentColor(Qt::black);
    ui->pen2color->setCurrentColor(Qt::red);
    ui->wincolor->setCurrentColor(Qt::green);
    ui->gridcolor->setCurrentColor(Qt::blue);

    ui->diffCombo1->setCurrentIndex(2);
    ui->diffCombo2->setCurrentIndex(2);

    connect(this->ui->gridSize, SIGNAL(valueChanged(int)), this, SLOT(setWinGrid()));
    connect(this->ui->g3x3, SIGNAL(clicked()),this,SLOT(setDefault()));
    connect(this->ui->g5x5, SIGNAL(clicked()),this,SLOT(set5x5()));
    connect(this->ui->g7x7, SIGNAL(clicked()),this,SLOT(set7x7()));
    connect(this->ui->g9x9, SIGNAL(clicked()),this,SLOT(set9x9()));
    connect(this->ui->gridSize, SIGNAL(sliderMoved(int)),this,SLOT(setCustom()));
    connect(this->ui->gridWin, SIGNAL(sliderMoved(int)),this,SLOT(setCustom()));
}

SessionDialog::~SessionDialog()
{
    delete ui;
}

void SessionDialog::setDefault(){
    this->ui->gridSize->setValue(3);
    this->ui->gridWin->setValue(3);
    this->ui->g3x3->setChecked(true);
}

void SessionDialog::set5x5(){
    this->ui->gridSize->setValue(5);
    this->ui->gridWin->setValue(3);
    this->ui->g5x5->setChecked(true);
}

void SessionDialog::set7x7(){
    this->ui->gridSize->setValue(7);
    this->ui->gridWin->setValue(5);
    this->ui->g7x7->setChecked(true);
}

void SessionDialog::set9x9(){
    this->ui->gridSize->setValue(9);
    this->ui->gridWin->setValue(6);
    this->ui->g9x9->setChecked(true);
}

void SessionDialog::setCustom(){
    this->ui->customRadio->setChecked(true);
}

void SessionDialog::setWinGrid(){
    this->ui->gridWin->setMaximum(this->ui->gridSize->value());
    this->ui->gridWin->setValue(this->ui->gridSize->value());
}

void SessionDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int SessionDialog::getGridSize(){
    return ui->gridSize->value();
}

int SessionDialog::getWinSize(){
    return ui->gridWin->value();
}

bool SessionDialog::isHuman(int player){
    return player==0?ui->humanRadio1->isChecked():ui->humanRadio2->isChecked();
}

int SessionDialog::getIA1(){
    return ui->diffCombo1->currentIndex();
}

int SessionDialog::getIA2(){
    return ui->diffCombo2->currentIndex();
}

QString SessionDialog::getStringIA1(){
    return ui->diffCombo1->currentText();
}

QString SessionDialog::getStringIA2(){
    return ui->diffCombo2->currentText();
}

QString SessionDialog::getP1Name(){
    return ui->player1Name->text();
}

QString SessionDialog::getP2Name(){
    return ui->player2Name->text();
}

QColor SessionDialog::getPen1(){
    return this->ui->pen1color->currentColor();
}

QColor SessionDialog::getPen2(){
    return this->ui->pen2color->currentColor();
}

QColor SessionDialog::getGridColour(){
    return this->ui->gridcolor->currentColor();
}

QColor SessionDialog::getWinColour(){
    return this->ui->wincolor->currentColor();
}

int SessionDialog::getSetsNum(){
    return this->ui->setsSize->value();
}
