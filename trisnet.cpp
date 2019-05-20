/***************************************************************************
 *            trisnet.cpp
 *
 *  Sun May  9 21:21:06 CEST 2010
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

#include "trisnet.h"

/* Match Messages
   move:x_y  - In game move coordinates
   chat:sender;mess - In game chat message
   keepAlive - Informs about peer activity
   first - Master message to know who starts the first set
   setSync - Synchronization message between two sets
   winner:p1;p2 - Set's end message that informs about points of every player
  */

TrisNet::TrisNet(QObject *parent,bool master, QNetworkAccessManager *netManager, QString channelID, QString sessionID): QObject(parent)
{
    this->sessionID=sessionID;
    this->master=master;
    this->netManager=netManager;
    this->channelID=channelID;
    this->packetCount=4;

    //netManager = new QNetworkAccessManager(this);

    deadPeerTimeoutTimer=new QTimer(this);
    keepAliveTimer = new QTimer(this);



    //Now I start the timer on which timeout the peer is considered dead
    //deadPeerTimeoutTimer->setSingleShot(true);
    QObject::connect(deadPeerTimeoutTimer, SIGNAL(timeout()),this,SLOT(deadTimeout()));
    deadPeerTimeoutTimer->start((unsigned)deadPeerTimeoutPeriod);

    //Now I have to start my keepAlive timer
    QObject::connect(keepAliveTimer, SIGNAL(timeout()),this,SLOT(keepAlive()));
    keepAliveTimer->start(keepAlivePeriod);

    connect(netManager, SIGNAL(finished(QNetworkReply*)),SLOT(receiveNextEvent(QNetworkReply*)));

    QUrl url(QString(baseURI)+"control.php");
    url.addQueryItem(QString("authType"),QString("logged"));
    url.addQueryItem(QString("sessionID"),sessionID);
    url.addQueryItem(QString("TYPE"),QString("receive"));
    url.addQueryItem(QString("channelID"),channelID);
    url.addQueryItem(QString("seq"),QString::number(packetCount));

    netManager->get(QNetworkRequest(url));
}

TrisNet::~TrisNet(){
    deadPeerTimeoutTimer->stop();
    keepAliveTimer->stop();
    delete deadPeerTimeoutTimer;
    delete keepAliveTimer;
}

void TrisNet::receiveNextEvent(QNetworkReply *reply){
    char buffer[1024];
    if((reply->readLine(buffer, 1023))==-1)return;

    QString json(QString(reply->readLine(2048)));

    if(json.length()<2 || !json.contains(QString("200")) || !json.contains(QString("message"))) return;

    int first=json.indexOf("\":\"",json.indexOf("message"))+3;
    int last=json.indexOf("\"",first);

    QString message(json.mid(first,last-first));
    //cout << packetCount << json.toStdString().c_str() << endl;
    if(message.contains(QString(":"))){
        //cout << message.toStdString().c_str() << endl;
        //I have to recognize the kind of message
        QStringList parts(message.split(":"));
        if(parts.at(0)=="keepAlive"){
            if((parts.at(1)=="1"&&!master)||(parts.at(1)=="0"&&master)){
                //The peer is alive
                deadPeerTimeoutTimer->stop();
                //deadPeerTimeoutTimer->setSingleShot(true);

                deadPeerTimeoutTimer->start((unsigned)deadPeerTimeoutPeriod);
                connect(deadPeerTimeoutTimer, SIGNAL(timeout()),SLOT(deadTimeout()));
            }
        }else if(parts.at(0)=="first"){
            //I have received who is the first to start
            emit firstReceived(parts.at(1).toInt());
        }else if(parts.at(0)=="move"){
            //I have received a move from the peer
            QStringList move(parts.at(1).split("_"));
            int x=move.at(0).toInt();
            int y=move.at(1).toInt();
            emit moveReceived(QPoint(x,y));
        }else if(parts.at(0)=="setSync"){
            //New set sync point
            emit setSync();
        }else if(parts.at(0)=="chat"){
            //I have received a new chat message
            emit chatMess(QUrl::fromPercentEncoding(parts.at(1).toUtf8()));
        }
    }
    packetCount++;
    //Now I request the next packet
    QUrl url(QString(baseURI)+"control.php");

    url.addQueryItem(QString("authType"),QString("logged"));
    url.addQueryItem(QString("sessionID"),sessionID);
    url.addQueryItem(QString("TYPE"),QString("receive"));
    url.addQueryItem(QString("channelID"),channelID);
    url.addQueryItem(QString("seq"),QString::number(packetCount));

    netManager->get(QNetworkRequest(url));

    //reply->deleteLater();
}

void TrisNet::deadTimeout(){
    deadPeerTimeoutTimer->stop();
    keepAliveTimer->stop();

    QMessageBox::critical((QWidget*)this->parent(), tr("Player ends game"), tr("The other player left this match!"), QMessageBox::Ok);


    emit deadPeer();
}

void TrisNet::sendChatMess(QString mess){
    //I have to encode the message so that it can be sent on an HTTP's get
    //request(in the URL)
    //mess=mess;

    QUrl url(QString(baseURI)+"control.php");

    url.addQueryItem(QString("authType"),QString("logged"));
    url.addQueryItem(QString("sessionID"),sessionID);
    url.addQueryItem(QString("TYPE"),QString("send"));
    url.addQueryItem(QString("channelID"),channelID);
    url.addQueryItem(QString("message"),QString("chat:")+QUrl::toPercentEncoding(mess,QByteArray(),"_:-"));
    url.addQueryItem(QString("destID"),QString::number((unsigned)4000000000));

    netManager->get(QNetworkRequest(url));
}

void TrisNet::sendMove(QPoint p){
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logged&sessionID="+sessionID+"&TYPE=send&channelID="+channelID+"&message=move:"+QString::number(p.x())+"_"+QString::number(p.y())+"&destID=4000000000")));
}

void TrisNet::sendSetSync(){
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logged&sessionID="+sessionID+"&TYPE=send&channelID="+channelID+"&message=setSync:sS&destID=4000000000")));
}

void TrisNet::sendFirstToMove(bool firstPlayer){
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logged&sessionID="+sessionID+"&TYPE=send&channelID="+channelID+"&message=first:"+QString::number(firstPlayer)+"&destID=4000000000")));
}

void TrisNet::sendLogout(){
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logout&sessionID="+sessionID)));
}

void TrisNet::sendResults(int masterPoints, int slavePoints){
    //Now I have to send the match results to the server
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logged&sessionID="+sessionID+"&TYPE=send&channelID="+channelID+"&message=winner:"+QString(masterPoints)+";"+QString(slavePoints)+"&destID=4000000000")));

    //Both master and slave sends the result with appropriate answer
    //This ensures a basic control on the server side
    //In every case the messages must be the same to be considered valid
}


void TrisNet::keepAlive(){
    netManager->get(QNetworkRequest(QUrl(QString(baseURI)+"control.php?authType=logged&sessionID="+sessionID+"&TYPE=send&channelID="+channelID+"&message=keepAlive:"+QString(master?"1":"0")+"&destID=4000000000")));
}
