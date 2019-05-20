/***************************************************************************
 *            tttfhgui.h
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
#ifndef TTTFHGUI_H
#define TTTFHGUI_H

#include <QMainWindow>
#include <QGridLayout>

#include <QLabel>
#include "qled.h"

#include "sessiondialog.h"
#include "aboutdialog.h"
#include "softgamearea.h"
#include "netdock.h"
#include "chatdock.h"

namespace Ui {
    class TTTFHGui;
}

class TTTFHGui : public QMainWindow {
    Q_OBJECT
public:
    TTTFHGui(QWidget *parent = 0);
    ~TTTFHGui();

    QStatusBar* getStatusBar(){return statusBar();}
    QLed* getLed(){return onlineLed;}


public slots:
        void newSession();
        void restart();
        void quick();
        void about();
        void netGame();
        void toggleOnline();
        void aboutQT();

        void showChat();
        void endChat(bool);
        void sendLogout();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);


private:
    Ui::TTTFHGui *ui;
    SessionDialog *sessionDialog;
    AboutDialog *aboutDialog;
    netDock *ndock;
    chatDock *cdock;


    SoftGameArea *softGameArea;
    QLabel *statusLabel;
    QLed *onlineLed;
};

#endif // TTTFHGUI_H
