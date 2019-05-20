/***************************************************************************
 *            softgamearea.cpp
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

#include "softgamearea.h"

SoftGameArea::SoftGameArea(QWidget *parent, SessionDialog *sessionDialog, QLabel *bar) :
    //QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
        QWidget(parent)
{
    //setAttribute(Qt::WA_OpaquePaintEvent,true);
    size=300;
    setFixedSize(size+size/6,size+size/6);
    setAutoFillBackground(false);

    this->bar=bar;

    currentSet=0;

    isOnline=0;
    tnet=0;

    mode=0;

    board=0;
    xRealCenterPoints=0;
    xPoints=0;
    xRealEndPoints=0;
    xRealStartPoints=0;
    oPoints=0;
    oRealPoints=0;
    lines=0;
    endPoints=0;
    startPoints=0;
    nOPoints=0;
    winPoints=0;

    nTextPoints=0;
    textPoints=0;

    background = QBrush(QColor(255,255,255));

    this->sessionDialog=sessionDialog;

    timer = new QTimer(this);
    debugTimer = new QTimer(this);

    connect(debugTimer, SIGNAL(timeout()), this, SLOT(debug()));
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));

    //debugTimer->start(1000);

    //newGame(sessionDialog->getGridSize(),sessionDialog->getWinSize(),true);
    quickSettings();
}

void SoftGameArea::debug(){
    if(sessionDialog->isHidden() && !animating && !symAnimating && !symAnimating2 && !drawingText && !animatingWin)update();
}

void SoftGameArea::animate(){
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 400;
    if(symAnimating) elapsedX = (elapsedX + qobject_cast<QTimer*>(sender())->interval()) % 300;
    if(symAnimating2) elapsedO = (elapsedO + qobject_cast<QTimer*>(sender())->interval()) % 300;
    repaint();
    //update();
}

void SoftGameArea::sendLogout(){
    if(tnet)tnet->sendLogout();
    endNetGame();
}

QString SoftGameArea::getStatus(){
    QString lMove("[[");
    QString rMove("]]");
    QString wsize(QString::number(isOnline?winsize:sessionDialog->getWinSize()));
    QString cset(QString::number(currentSet+1));
    QString tset(QString::number(isOnline?sets:sessionDialog->getSetsNum()));
    QString p1name(isOnline?this->p1name:sessionDialog->getP1Name());
    QString p2name(isOnline?this->p2name:sessionDialog->getP2Name());
    return tr("WIN SIZE:")+wsize+" - " + tr("SET:") +cset+"/"+tset+" - "+
            (!board->who_plays()?lMove:"") + p1name +(sessionDialog->isHuman(0)?"":QString("(")+sessionDialog->getStringIA1()+")")+(!board->who_plays()?rMove:"")+" VS. "+
            (board->who_plays()?lMove:"") + p2name +(sessionDialog->isHuman(1)?"":QString("(")+sessionDialog->getStringIA2()+")")+(board->who_plays()?rMove:"");
}

void SoftGameArea::newNetGame(int dim, int sets, bool newContext, TrisNet *tnet, QString p1name, QString p2name){
    this->tnet=tnet; //Copying the network instance utility class

    //If I am the master then I am also the p1(with the circle as sign)
    isOnline=1+(!tnet->iAmMaster());

    this->p1name=p1name;
    this->p2name=p2name;

    this->dim=dim;
    this->winsize=dim==3||dim==5?3:dim==7?5:6;
    this->sets=sets;

    if(newContext && tnet->iAmMaster()){
        //In this case we have to choose who is the first player of the first set
        firstPlayer=qrand()>(RAND_MAX/2);
        //Now we have to tell to the other player(slave) about the decision
        tnet->sendFirstToMove(firstPlayer);
    }

    if(!newContext)synced=false;
    else{
        synced=true;
        connect(tnet,SIGNAL(setSync()),SLOT(syncing()));
        connect(tnet,SIGNAL(chatMess(QString)),this,SIGNAL(chatMess(QString)));

        //The peer seems to be dead so I have to close this session emitting a signal
        connect(tnet,SIGNAL(deadPeer()),this,SLOT(endNetGame()));
    }

    connect(tnet, SIGNAL(moveReceived(QPoint)), this, SLOT(peerMoveArrived(QPoint)));


    if(tnet->iAmMaster() || !newContext){
        mode=1;
        newGame(dim,winsize,newContext);
    }
    else connect(tnet, SIGNAL(firstReceived(bool)), this, SLOT(startAsSlave(bool)));
}

void SoftGameArea::syncing(){
    synced=true;
}

void SoftGameArea::startAsSlave(bool first){    
    disconnect(tnet, SIGNAL(firstReceived(bool)), 0, 0);
    this->firstPlayer=first;
    mode=1;
    newGame(dim,winsize,true);
}

void SoftGameArea::newGame(int dim, int winsize, bool newContext){
    if(newContext) currentSet=0;

    this->dim=dim;
    this->winsize=winsize;
    if(board){
        delete board;
        board=0;
    }
    board = new Tavola(dim,winsize);


    if(lines){
        delete lines;
        lines=0;
    }
    if(endPoints){
        delete endPoints;
        endPoints=0;
    }
    if(startPoints){
        delete startPoints;
        startPoints=0;
    }
    initialized=false;
    lines=new QLine[dim*2];
    endPoints=new int[dim*2];
    startPoints=new int[dim*2];

    ial1=(ia_level)sessionDialog->getIA1();
    ial2=(ia_level)sessionDialog->getIA2();

    if(newContext){
        points1=0;
        points2=0;
    }

    animating=true;
    symAnimating=false;
    symAnimating2=false;
    animatingWin=false;
    elapsed=0;
    elapsedX=0;
    elapsedO=0;
    animcount=0;
    if(nOPoints){
        delete nOPoints;
        nOPoints=0;
    }

    nOPoints = new int[dim*dim];

    if(xPoints && xRealCenterPoints && xRealEndPoints && xRealStartPoints){
        delete xPoints;
        delete xRealCenterPoints;
        delete xRealEndPoints;
        delete xRealStartPoints;
        xPoints=0;
        xRealCenterPoints=0;
        xRealEndPoints=0;
        xRealStartPoints=0;
    }
    xPoints = new QStack<QPoint>();
    xRealStartPoints = new QStack<QPoint>();
    xRealEndPoints = new QStack<QPoint>();
    xRealCenterPoints = new QStack<QPoint>();

    if(oPoints && oRealPoints){
        for(int i=0;i<oRealPoints->size();++i) delete oRealPoints->at(i);
        delete oPoints;
        delete oRealPoints;
        oPoints=0;
        oRealPoints=0;
    }
    oPoints = new QStack<QPointF>();
    oRealPoints= new QStack<QPointF*>();

    if(textPoints && nTextPoints){
        for(int i=0;i<textPoints->size();++i) delete textPoints->at(i);
        delete textPoints;
        delete nTextPoints;
    }
    textPoints = new QStack<QPointF*>();
    nTextPoints = new QStack<int>();
    drawingText = false;
    actualDrawingText = -1;
    tie=false;

    if(winPoints){
        delete winPoints;
        winPoints=0;
    }

    bar->setText(getStatus());

    winnerPainted=false;

    if(newContext && !isOnline) firstPlayer=qrand()>(RAND_MAX/2);
    if((currentSet+firstPlayer)%2) board->cambia_turno();

    update();
}

void SoftGameArea::quickSettings(){
    mode=0;
    newGame(2,2,true);
}

void SoftGameArea::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    //painter.fillRect(event->rect(), background);

//    QPen pen1(Qt::lightGray, 0.2, Qt::SolidLine);
  //  painter.setPen(pen1);
    /*for(int i=0;i<size+100;i+=11){
        painter.drawLine(5,i,size+size/6-5,i);
        painter.drawLine(i,5,i,size+size/6-5);
    }*/
    painter.translate(25,25);

    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(sessionDialog->getGridColour(), 4, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    if(!initialized){
        //srand((int)&painter);
        for(int i=1;i<dim;++i){
            int aux=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/3);
            int aux2=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/3);
            if(dim==2){
                aux=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/6)-20;
                aux2=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/6);
            }
            lines[i-1]=QLine(0,aux2,0,aux2);
            lines[dim*2-i]=QLine(aux,0,aux,0);

            endPoints[dim*2-i]=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/3);
            endPoints[i-1]=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/3);
            if(dim==2){
                endPoints[dim*2-i]=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/6);
                endPoints[i-1]=(size/dim)*i+qrand()/(float)RAND_MAX*(size/dim/6);
            }
            startPoints[dim*2-i]=aux;
            startPoints[i-1]=aux2;
        }
    }


    if(animating && !drawingText){
        if(animcount<dim-1){
            float delta=((elapsed/(float)300)*(-startPoints[dim*2-animcount-1]+endPoints[dim*2-animcount-1]));
            lines[dim*2-animcount-1].setP2(QPoint(startPoints[dim*2-animcount-1]+delta,(elapsed/(float)300)*size-5));
        }else{
            float delta=((elapsed/(float)300)*(-startPoints[animcount-dim+1]+endPoints[animcount-dim+1]));
            lines[animcount-dim+1].setP2(QPoint((elapsed/(float)300)*size-5,startPoints[animcount-dim+1]+delta));
        }

        if(elapsed>=300){
            elapsed=0;
            if(animcount>=(dim-1)*2){
                animating=false;
                animcount=0;
                timer->stop();
            }else{
                //QSound::play("paper.wav");
                animcount++;
            }
        }
    }

    for(int i=1;i<dim;++i){
        painter.drawLine(lines[i-1]);
        painter.drawLine(lines[dim*2-i]);
        if(!timer->isActive())timer->start(25);
    }

    initialized=true;


    QFont fonts;
    fonts.setPixelSize(size/dim/6);
    painter.setFont(fonts);
    switch(mode){
    case 0:
        painter.drawPixmap(0,0,QPixmap(":main/quick.png"));
        pen.setColor(Qt::magenta);
        painter.setPen(pen);
        painter.drawText(10,size-size/dim/15,size+40,30,Qt::AlignHCenter||Qt::AlignCenter,tr("CLICK ON A RULE SET"));
        break;
    case 1:
        if(!animating)drawPoints(&painter);
        if(!animating && (!drawingText || tie))for(int i=0;i<dim;++i){
            for(int j=0;j<dim;++j){
                if(!board->libera(i,j)){
                    if(board->croce(i,j)){
                        drawX(getCenter(QPoint(j,i)),size/dim/3,&painter);
                    }else{
                        drawO(getCenter(QPoint(j,i)),size/dim/4,&painter);
                    }
                }
            }
        }

        if(!timer->isActive() && !animating && !board->who_plays() && (!sessionDialog->isHuman(0) || isOnline)  && !symAnimating && !symAnimating2 && !board->finished()){
            if(!isOnline){
                timer->stop();
                int x,y;
                x=TrisMachine::get_move(*board,y,false,ial1);
                if(board->contrassegna(x,y,false)){
                    update();
                    return;
                }
            }
        }

        if(!timer->isActive() && !animating && board->who_plays() && (!sessionDialog->isHuman(1) || isOnline) && !symAnimating && !symAnimating2 && !board->finished()){
            if(!isOnline){
                timer->stop();
                int x,y;
                x=TrisMachine::get_move(*board,y,true,ial2);
                if(board->contrassegna(x,y,true)){
                    update();
                    return;
                }
            }
        }




        if(animatingWin || (!symAnimating && !symAnimating2 && !drawingText && (~(*board) || !(*board)))) drawWinning(&painter);
        if(!animatingWin && !symAnimating && !symAnimating2 && !animating && board->finished()){
            pen.setColor(Qt::magenta);
            painter.setPen(pen);
            QFont font;
            font.setPixelSize(15);
            painter.setFont(font);
            painter.drawText(0,size+size/15,tr("CLICK TO CONTINUE"));

            if(!(~(*board) || !(*board))){
                tie=true;
                QPen pen2(sessionDialog->getWinColour(), 3, Qt::SolidLine);
                pen2.setCapStyle(Qt::RoundCap);
                painter.setPen(pen2);
                QFont textFont;
                textFont.setPixelSize(100);
                painter.setFont(textFont);
                drawText(&painter,3,"D",QPointF(10,65));
                drawText(&painter,4,"R",QPointF(75,65));
                drawText(&painter,5,"A",QPointF(130, 65));
                drawText(&painter,6,"W",QPointF(180,65));

            }
        }

        break;
    }
    bar->setText(getStatus());
}

void SoftGameArea::peerMoveArrived(QPoint p){
    if(board->contrassegna(p.x(),p.y(),isOnline==1)){
        update();
        return;
    }
}

void SoftGameArea::mouseReleaseEvent(QMouseEvent *event){
    QPoint slot=(getSlot(event->posF()-QPointF(25,25)));

    switch(mode){
    case 0:
        mode=1;
        if(slot.x()==0){
            if(slot.y()==0){
                sessionDialog->setDefault();
                newGame(3,3,true);
            }else{
                sessionDialog->set7x7();
                newGame(7,5,true);
            }
        }else{
            if(slot.y()==0){
                sessionDialog->set5x5();
                newGame(5,3,true);
            }else{
                sessionDialog->set9x9();
                newGame(9,6,true);
            }
        }

        break;
    case 1:
        if(board->finished() || animating || symAnimating || symAnimating2 || animatingWin || drawingText){
            if(board->finished() && !animatingWin){
                if(!(*board))points2++;
                else if((~(*board))) points1++;
                currentSet++;
                int diff=(points1-points2)>=0?points1-points2:points2-points1;
                int remaining=isOnline?sets:sessionDialog->getSetsNum()-currentSet;
                if(!isOnline){
                    if(currentSet>=sessionDialog->getSetsNum() || diff>remaining){
                        QMessageBox::information((QWidget*)this->parent()->parent(), tr("Session finished"),
                                                 (points1>points2?sessionDialog->getP1Name():(points2>points1?sessionDialog->getP2Name():
                                                                                              tr("Nobody")))+tr(" wins this Game Session!"),
                                                 QMessageBox::Ok);
                        currentSet=0;
                        //newGame(sessionDialog->getGridSize(),sessionDialog->getWinSize(),true);
                        quickSettings();
                    }else{
                        newGame(this->dim, this->winsize,false);
                    }
                }else{
                    //I have to send the results to the server on THIS channel
                    //(Server side can check data because everyone sends its own copy of results)
                    tnet->sendResults(points1,points2);

                    if(currentSet>=sets || diff>remaining){
                        //emit endGame(true);
                        QMessageBox::information((QWidget*)this->parent()->parent(), tr("Match finished"),
                                                 (points1>points2?p1name:points2>points1?p2name:tr("Nobody"))+tr(" wins this Game Session!"),
                                                 QMessageBox::Ok);
                        currentSet=0;

                        //if(tnet->iAmMaster()){
                            //In this case I have to tell to the server who wins
                            //tnet->sendResults(points1,points2);
                        //}

                        //emit endGame();

                        endNetGame();

                    }else{
                        synced=false;
                        newGame(this->dim, this->winsize,false);
                        tnet->sendSetSync();
                    }
                }
            }
            return;
        }
        if(((isOnline==1&&synced) || (isOnline==0&&sessionDialog->isHuman(0))) && !board->who_plays()){
            bool moved=board->contrassegna(slot.x(),slot.y(),false);
            if(isOnline && moved)
                tnet->sendMove(QPoint(slot.x(),slot.y()));
        }else if(((isOnline==2&&synced) || (isOnline==0&&sessionDialog->isHuman(1))) && board->who_plays()){
            bool moved=board->contrassegna(slot.x(),slot.y(),true);
            if(isOnline && moved)
                tnet->sendMove(QPoint(slot.x(),slot.y()));
        }

        break;
    }

    update();
}

void SoftGameArea::endNetGame(){
    emit endGame(true);
    //Always when match finishes, the game returns to offline mode
    //The general status obviously remains online(green led)
    isOnline=0;

    disconnect(tnet, SIGNAL(moveReceived(QPoint)), 0, 0);

    //Now the TrisNet instance is not used anymore so I can delete it
    delete tnet;
    quickSettings();

}

void SoftGameArea::sendChatMessage(QString mess){
    tnet->sendChatMess((tnet->iAmMaster()?p1name:p2name)+QString(";")+mess);
}

QPoint SoftGameArea::getCenter(QPoint slot){
    return QPoint(((size/dim))*slot.x()+(size/dim)/2,(((size/dim))*slot.y()+(size/dim)/2));
}

QPoint SoftGameArea::getSlot(QPointF coord){
    QPoint slot;

    bool found=false;
    int i=0;
    double m,b;
    while(!found && i<dim){
        m=(endPoints[i]-startPoints[i])/((double)(size-5));
        b=(size-5)*(startPoints[i])/((double)(size-5));

        if(m*coord.x()+b > coord.y()) found=true;

        ++i;
    }

    slot.setY(i-1);

    found=false;
    i=0;
    while(!found && i<dim){
        m=(endPoints[dim*2-i-1]-startPoints[dim*2-i-1])/((double)(size-5));
        b=(size-5)*(startPoints[dim*2-i-1])/((double)(size-5));

        if(m*coord.y()+b > coord.x()) found=true;

        ++i;
    }

    slot.setX(i-1);


    return slot;
}

void SoftGameArea::drawO(QPoint center, int radius, QPainter *painter){
    QMutexLocker locker(&drawMutex);

    QPen pen(sessionDialog->getPen1(), 3, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    int found=oPoints->indexOf(center);

    if(found!=-1){        
        if(symAnimating2 && center==oPoints->top()){
            painter->save();
            painter->translate(oPoints->at(found)+QPoint(0,size/dim/2));
            painter->rotate(-90);
            painter->drawPolyline(oRealPoints->top(),(elapsedO/(float)200)*nOPoints[found]);
            painter->restore();

            if(elapsedO>=200){
                symAnimating2=false;
                elapsedO=0;
                if(!symAnimating)timer->stop();
            }
        }else{
            bar->setText(getStatus());
            painter->save();
            painter->translate(oPoints->at(found)+QPoint(0,size/dim/2));
            painter->rotate(-90);
            painter->drawPolyline(oRealPoints->at(found),nOPoints[found]);
            painter->restore();
        }
    }else{        
        QPainterPath ellipse;
        ellipse.addEllipse(0,0,radius*2+qrand()/(float)RAND_MAX*(size/dim/3),radius*2+qrand()/(float)RAND_MAX*(size/dim/4));
        
        QPolygonF poly(ellipse.toFillPolygon());

        nOPoints[oPoints->size()]=poly.size()-(poly.size())*(qrand()/(float)RAND_MAX*0.02);

        oPoints->push(center);

        QPointF *poly2=new QPointF[nOPoints[oPoints->size()-1]];
        oRealPoints->push(poly2);
        symAnimating2=true;
        elapsedO=0;
        timer->start(25);


        int mem1,mem2,aux1=0,aux2=0,aux21=0,aux22=0;
        for (int i = 0; i < nOPoints[oPoints->size()-1]-1; i++) {
            mem1=qrand()/(float)RAND_MAX*(1.3+1.2*i/nOPoints[oPoints->size()-1]);
            mem2=qrand()/(float)RAND_MAX*(1.3+1.2*i/nOPoints[oPoints->size()-1]);

            poly2[i]=QPointF(poly.at(i)+QPointF(aux1+mem1+aux21,aux2+mem2+aux22));
            aux21=aux1;
            aux22=aux2;
            aux1=mem1;
            aux2=mem2;
        }
        poly2[nOPoints[oPoints->size()-1]-1]=QPointF(poly.at(nOPoints[oPoints->size()-1]-1)+QPointF(qrand()/(float)RAND_MAX*(size/dim/10),qrand()/(float)RAND_MAX*(size/dim/10)));


    }
}

void SoftGameArea::drawX(QPoint center, int radius, QPainter *painter){
    QMutexLocker locker(&drawMutex);



    QPen pen(sessionDialog->getPen2(), 3, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    int found=xPoints->indexOf(center);
    if(found!=-1){
        if(symAnimating && center==xPoints->top()){
            QPoint central(xRealCenterPoints->at(found));
            QPoint diff(xRealStartPoints->at(found).x(), xRealStartPoints->at(found).y());
            QPoint diff2(xRealEndPoints->at(found).x(), xRealEndPoints->at(found).y());

            if(animcount==0){
                QPoint delta=((elapsedX/(float)150)*(-(central+diff)+(central-diff)));
                painter->drawLine(central-diff,central-diff-delta);
            }else{
                QPoint delta=((elapsedX/(float)150)*(-(central+diff2)+(central-diff2)));
                painter->drawLine(central-diff2,central-diff2-delta);
                painter->drawLine(central-diff,central+diff);
            }
            if(elapsedX>=150){
                elapsedX=0;
                if(animcount==0){
                    animcount++;
                    elapsedX=0;
                }else{
                    animcount=0;
                    symAnimating=false;

                    if(!symAnimating2)timer->stop();

                }
            }
        }else{
            bar->setText(getStatus());
            QPoint diff(xRealStartPoints->at(found).x(), xRealStartPoints->at(found).y());
            QPoint diff2(xRealEndPoints->at(found).x(), xRealEndPoints->at(found).y());
            painter->drawLine(xRealCenterPoints->at(found)-diff, xRealCenterPoints->at(found)+diff);
            painter->drawLine((xRealCenterPoints->at(found)-diff2), (xRealCenterPoints->at(found)+diff2));
        }
    }else{
        xPoints->push(center);
        int r=radius/2;
        symAnimating=true;
        timer->start(25);
        elapsedX=0;
        animcount=0;

        xRealStartPoints->push(QPoint(r+qrand()/(float)RAND_MAX*(size/dim/10), r+qrand()/(float)RAND_MAX*(size/dim/10)));
        xRealEndPoints->push(QPoint(r+qrand()/(float)RAND_MAX*(size/dim/10), -r+qrand()/(float)RAND_MAX*(size/dim/10)));
        xRealCenterPoints->push(center+QPoint(qrand()/(float)RAND_MAX*(size/dim/10), qrand()/(float)RAND_MAX*(size/dim/10)));
    }

}

void SoftGameArea::drawPoints(QPainter *painter){
    QPen textPen(sessionDialog->getPen1(), 3, Qt::SolidLine);
    painter->setPen(textPen);
    QFont textFont;
    textFont.setPixelSize(30);
    painter->setFont(textFont);
    this->drawText(painter,0,QString::number(points1),QPointF(size-size/5,size+size/15));

    QPen pen2(sessionDialog->getGridColour(), 6, Qt::SolidLine);
    pen2.setCapStyle(Qt::RoundCap);
    painter->setPen(pen2);
    //painter->drawText(size-40, size+20, "10");
    this->drawText(painter,1,"-",QPointF(size-size/8,size+size/15));

    QPen pen3(sessionDialog->getPen2(), 3, Qt::SolidLine);
    pen3.setCapStyle(Qt::RoundCap);
    painter->setPen(pen3);
    this->drawText(painter,2,QString::number(points2),QPointF(size-size/16,size+size/15));
}

void SoftGameArea::drawText(QPainter *painter, int index, QString text, QPointF point){
    if(actualDrawingText==-1)actualDrawingText=0;
    if(actualDrawingText==index){
        if(drawingText){
            painter->drawPolyline(textPoints->top(),(elapsed/(float)300)*nTextPoints->at(index));
            if(elapsed>=300){
                drawingText=false;
                elapsed=0;
                actualDrawingText++;
                if(!animating && !symAnimating2 && !symAnimating)timer->stop();
            }
        }else{
            QPainterPath dText;
            dText.addText(point,painter->font(),text);

            QPolygonF poly(dText.toFillPolygon());

            nTextPoints->push(text==QString("0")?poly.size()/2-1:poly.size());

            QPointF *poly2=new QPointF[nTextPoints->top()];
            textPoints->push(poly2);
            drawingText=true;
            elapsed=0;
            if(!timer->isActive())timer->start(25);


            int mem1,mem2,aux1=0,aux2=0,aux21=0,aux22=0;
            for (int i = 1; i < nTextPoints->top()-1; i++) {
                mem1=qrand()/(float)RAND_MAX*(1.3+0.5*i/nTextPoints->top());
                mem2=qrand()/(float)RAND_MAX*(1.3+0.5*i/nTextPoints->top());

                poly2[i]=QPointF(poly.at(i)+QPointF(aux1+mem1+aux21,aux2+mem2+aux22));
                aux21=aux1;
                aux22=aux2;
                aux1=mem1;
                aux2=mem2;
            }
            poly2[nTextPoints->top()-1]=QPointF(poly.at(nTextPoints->top()-1)+QPointF(qrand()/(float)RAND_MAX*(size/dim/20),qrand()/(float)RAND_MAX*(size/dim/20)));
            poly2[0]=QPointF((poly.at(0)+QPointF(qrand()/(float)RAND_MAX*(size/dim/10),qrand()/(float)RAND_MAX*(size/dim/20))));


        }
    }
    if(actualDrawingText>index)painter->drawPolyline(textPoints->at(index),nTextPoints->at(index));

}

void SoftGameArea::drawWinning(QPainter *painter){
    QPen pen(sessionDialog->getWinColour(), 3, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    QPainterPath ellipse;
    painter->save();
    int length=size/dim*board->get_winsize()+5; //winning sign length
    int offset=0; //winning sign offset from coordinate system origin

    if(board->isWinnerOnRow()){
        int rowX=board->getWinnerRow('X', offset);
        int row=rowX==-1?board->getWinnerRow('O', offset):rowX;
        ellipse.addEllipse((offset+1)*(size/dim),row*(size/dim),length,size/dim+5);
    }else if(board->isWinnerOnColumn()){
        int colX=board->getWinnerColumn('X', offset);
        int col=colX==-1?board->getWinnerColumn('O', offset):colX;
        ellipse.addEllipse(col*(size/dim),(offset+1)*(size/dim),size/dim+5,length);
    }else{
        int Yoffset;
        char winner=(~(*board))?'O':'X';
        bool side=board->getWinnerDiag(winner,offset,Yoffset);

        if(side){
            painter->translate(Yoffset>0?Yoffset*(size/dim):0,Yoffset<0?-Yoffset*(size/dim):0);
            painter->rotate(45);
            ellipse.addEllipse((offset+1)*(1.4*size/dim)+size/dim/1.4/2,-size/dim/1.4/2,(length)*1.3,(size)/dim/1.4);
        }else{            
            painter->translate(Yoffset>0?-Yoffset*(size/dim):0,(Yoffset<0?-Yoffset*(size/dim):0)+(size-size/dim/1.4/2));
            painter->rotate(-45);
            ellipse.addEllipse(offset*(1.4*size/dim),0,(length)*1.3,(size)/dim/1.4);
        }
    }

    if(!animatingWin && !winnerPainted){
        QPolygonF poly(ellipse.toFillPolygon());
        nWinPoints=poly.size();
        winPoints = new QPointF[nWinPoints];

        int mem1,mem2,aux1=0,aux2=0,aux21=0,aux22=0;
        for(int i=0;i<nWinPoints;++i){
            mem1=qrand()/(float)RAND_MAX*2;
            mem2=qrand()/(float)RAND_MAX*2;

            winPoints[i]=*(new QPointF(poly.at(i)+QPointF(aux1+mem1+aux21,aux2+mem2+aux22)));
            aux21=aux1;
            aux22=aux2;
            aux1=mem1;
            aux2=mem2;
        }

        animatingWin=true;
        elapsed=0;
        if(!timer->isActive())timer->start(25);
        painter->restore();
    }else{
        painter->drawPolyline(winPoints,(elapsed/(float)400)*nWinPoints);
        painter->restore();
        if(elapsed>=360){
            animatingWin=false;
            winnerPainted=true;
            timer->stop();

        }
    }
}
