/***************************************************************************
 *            tttfhgui.cpp
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
#include "tttfhgui.h"
#include "ui_tttfhgui.h"

TTTFHGui::TTTFHGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TTTFHGui)
{
    ui->setupUi(this);

    sessionDialog = new SessionDialog;
    connect(this->ui->actionNew, SIGNAL(activated()), this, SLOT(newSession()));
    connect(this->ui->actionRestart, SIGNAL(activated()), this, SLOT(restart()));
    connect(this->ui->actionAbout, SIGNAL(activated()), this, SLOT(about()));
    connect(this->ui->actionQuick_Rules, SIGNAL(activated()), this, SLOT(quick()));
    connect(ui->actionNet, SIGNAL(triggered()), SLOT(netGame()));

    statusLabel=new QLabel("");
    onlineLed = new QLed();
    onlineLed->setOffColor((QLed::ledColor)0);
    onlineLed->setOnColor((QLed::ledColor)1);
    onlineLed->setFixedSize(25,25);
    onlineLed->setToolTip(tr("You are Offline"));
    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(onlineLed);

    softGameArea = new SoftGameArea(this,sessionDialog,statusLabel);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(softGameArea,0,0);
    ui->centralWidget->setLayout(layout);

    aboutDialog = new AboutDialog;

    ndock=0;
    cdock=0;

    QAction *newSession=new QAction(QIcon(":main/newSession.png"),tr("New Match"), ui->mainToolBar);
    QAction *restart=new QAction(QIcon(":main/newGame.png"),tr("Restart Set"), ui->mainToolBar);
    QAction *quick=new QAction(QIcon(":main/quickSession.png"),tr("Quick rules"), ui->mainToolBar);
    QAction *net = new QAction(QIcon(":/main/net.png"),tr("Net Match"),ui->mainToolBar);

    ui->mainToolBar->addAction(newSession);
    ui->mainToolBar->addAction(net);
    ui->mainToolBar->addAction(quick);
    ui->mainToolBar->addAction(restart);

    connect(restart, SIGNAL(triggered()), this, SLOT(restart()));
    connect(quick, SIGNAL(triggered()), this, SLOT(quick()));
    connect(newSession, SIGNAL(triggered()), this, SLOT(newSession()));
    connect(net, SIGNAL(triggered()), this, SLOT(netGame()));
    connect(ui->aboutQT, SIGNAL(triggered()), this, SLOT(aboutQT()));

    ui->wplabel->hide();
}

TTTFHGui::~TTTFHGui(){
    delete ui;
}

void TTTFHGui::aboutQT(){
    QMessageBox::aboutQt(this);
}

void TTTFHGui::sendLogout(){
    ui->wplabel->hide();
    if(softGameArea && softGameArea->isOnline)softGameArea->sendLogout();
    if(ndock){
        disconnect(ndock,0,0,0);
        //delete ndock;
        ndock=0;
        if(cdock)endChat(false);
    }
}

void TTTFHGui::netGame(){
    if(!ndock){
        ndock = new netDock(this, onlineLed, softGameArea);
        ndock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
        addDockWidget(Qt::LeftDockWidgetArea, ndock);
        connect(ndock,SIGNAL(matchStarted()),SLOT(showChat()));
        connect(softGameArea,SIGNAL(endGame(bool)),this,SLOT(endChat(bool)));
        connect(ndock,SIGNAL(sendLogout()),this,SLOT(sendLogout()));
    }
    if(ndock->isHidden()){
        ui->wplabel->show();
        ndock->show();
    }
    else{
        ui->wplabel->hide();
        ndock->hide();
    }
    //showChat();
}

void TTTFHGui::closeEvent(QCloseEvent *e){
    if(onlineLed->value()) this->sendLogout();
    QMainWindow::closeEvent(e);
}

void TTTFHGui::showChat(){
    if(!cdock){
        cdock = new chatDock(this,sessionDialog->getPen1(),sessionDialog->getPen2(), softGameArea->p1name);
        //cdock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
        addDockWidget(Qt::LeftDockWidgetArea,cdock);

        connect(cdock,SIGNAL(sendMess(QString)),softGameArea,SLOT(sendChatMessage(QString)));
        connect(softGameArea,SIGNAL(chatMess(QString)),cdock,SLOT(newMess(QString)));
        cdock->show();
    }
    //cdock->show();
}

void TTTFHGui::endChat(bool nop){
    disconnect(softGameArea,SIGNAL(endGame(bool)),0,0);
    disconnect(cdock,SIGNAL(sendMess(QString)),softGameArea,0);
    cdock->hide();
    delete cdock;
    cdock=0;
}

void TTTFHGui::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TTTFHGui::restart(){
    if(softGameArea->mode && !softGameArea->isOnline)softGameArea->newGame(sessionDialog->getGridSize(),sessionDialog->getWinSize(),false);
}

void TTTFHGui::quick(){
    if(!softGameArea->isOnline)softGameArea->quickSettings();
    else{
        QMessageBox msgBox;
        msgBox.setText(tr("You are playing online."));
        msgBox.setInformativeText(tr("Do you want to logout and return to offline mode?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if(ret==QMessageBox::Yes){
            sendLogout();
            softGameArea->quickSettings();
        }
    }
}

void TTTFHGui::about(){
    aboutDialog->exec();
}

void TTTFHGui::newSession(){    
    if(softGameArea->isOnline){QMessageBox msgBox;
        msgBox.setText(tr("You are playing online."));
        msgBox.setInformativeText(tr("Do you want to logout and return to offline mode?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if(ret==QMessageBox::Yes){
            sendLogout();
        }else{
            return;
        }
    }

    sessionDialog->show();

    if (sessionDialog->exec() == QDialog::Accepted) {        
        softGameArea->mode=1;
        softGameArea->newGame(sessionDialog->getGridSize(),sessionDialog->getWinSize(),true);
    }
}

void TTTFHGui::toggleOnline(){
    if(onlineLed->value()){
        //I have to send a logout message to the remote server
        //ndock->hide();
        ui->wplabel->hide();
    }else{
        //In this case I have to show the network interface
        ui->wplabel->show();
        this->netGame();
    }
}
