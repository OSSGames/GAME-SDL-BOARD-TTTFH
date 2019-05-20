/***************************************************************************
 *            softgamearea.h
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

#ifndef SOFTGAMEAREA_H
#define SOFTGAMEAREA_H

#include <QWidget>

#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QPoint>
#include <QMouseEvent>
#include <QTimer>
#include <QStack>
#include <QStatusBar>
#include <QLabel>

#include <math.h>

#include <iostream>

#include <QMessageBox>

#include <QMutex>
#include <QMutexLocker>

#include "Tavola.h"
#include "TrisMachine.h"

#include "sessiondialog.h"

#include "trisnet.h"

class QPainter;
class QPaintEvent;
class QMouseEvent;


class SoftGameArea : public QWidget
{
Q_OBJECT
public:
    explicit SoftGameArea(QWidget *parent = 0, SessionDialog *sessionDialog=0, QLabel *bar=0);

    void newGame(int dim, int winsize, bool newContext);
    void newNetGame(int dim, int sets, bool newContext, TrisNet *tnet, QString p1name, QString p2name); //Starts a new net game
    void quickSettings();

    int mode;
    int isOnline; //0 for no, 1 for yes p1, 2 for yes p2

    QString p1name;
    QString p2name;

signals:
    void endGame(bool);
    void chatMess(QString);

public slots:
    void animate();
    void debug();

    void peerMoveArrived(QPoint p);
    void startAsSlave(bool first);
    void syncing();
    void sendLogout();
    void endNetGame();

    void sendChatMessage(QString mess);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


private:
    QBrush background;

    TrisNet *tnet;


    int size;
    int dim;
    int winsize;
    int currentSet;
    int sets;

    bool synced;

    QLabel *bar;

    bool initialized;
    bool animating;
    bool symAnimating;
    bool symAnimating2;
    bool animatingWin;
    bool tie;
    int animcount;
    bool winnerPainted;
    QTimer *timer;

    QTimer *debugTimer;

    int elapsed;
    int elapsedX;
    int elapsedO;
    QLine *lines;
    int *endPoints;
    int *startPoints;
    QStack<QPoint> *xPoints;
    QStack<QPoint> *xRealStartPoints;
    QStack<QPoint> *xRealEndPoints;
    QStack<QPoint> *xRealCenterPoints;
    QStack<QPointF> *oPoints;
    QStack<QPointF*> *oRealPoints;
    int *nOPoints;
    QPointF *winPoints;
    int nWinPoints;

    //Text drawings
    QStack<QPointF*> *textPoints;
    QStack<int> *nTextPoints;
    bool drawingText;
    int actualDrawingText;

    int points1;
    int points2;

    Tavola *board;

    void drawO(QPoint center, int radius, QPainter *painter);
    void drawX(QPoint center, int radius, QPainter *painter);
    void drawPoints(QPainter *painter);
    void drawWinning(QPainter *painter);

    void drawText(QPainter *painter, int index, QString text, QPointF point);

    QPoint getCenter(QPoint slot);
    QPoint getSlot(QPointF coord);

    SessionDialog *sessionDialog;

    ia_level ial1;
    ia_level ial2;

    QString getStatus();

    bool firstPlayer; //Who starts this match

    mutable QMutex drawMutex;
};

#endif // SOFTGAMEAREA_H
