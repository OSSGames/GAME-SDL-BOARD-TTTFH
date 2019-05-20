/***************************************************************************
 *            trisnet.h
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

#ifndef TRISNET_H
#define TRISNET_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>

#include <QString>
#include <QStringList>

#include <QtScript>

#define baseURI "http://tttfh.sourceforge.net/server/"
#define loginPage "client.php"

//After this time the peer is considered dead
#define deadPeerTimeoutPeriod 25000
//After this time a keep alive message is sent
#define keepAlivePeriod 7000

class TrisNet : public QObject{
    Q_OBJECT

public:
    TrisNet(QObject *parent, bool master, QNetworkAccessManager *netManager, QString channelID, QString sessionID);
    ~TrisNet();

    void sendMove(QPoint p);

    bool iAmMaster(){return master;}    

    //If we are master we have to tell to the slave who starts the first set
    void sendFirstToMove(bool firstPlayer);

    //If we are master we have to tell to the server the match's results
    void sendResults(int masterPoints, int slavePoints);

    void sendSetSync();

    void sendChatMess(QString mess);

public slots:
    void keepAlive();
    void receiveNextEvent(QNetworkReply *reply);
    void sendLogout();
    void deadTimeout();


signals:
    void moveReceived(QPoint p);
    void deadPeer(); //Called when the next peer seems to be dead
    void firstReceived(bool first);
    void setSync();
    void chatMess(QString mess);

private:
    bool master; //Tells if this client is the master of the match

    QTimer *keepAliveTimer;
    QTimer *deadPeerTimeoutTimer;

    QString sessionID;

    QNetworkAccessManager *netManager;

    QString channelID;

    int packetCount;
};

#endif // TRISNET_H
