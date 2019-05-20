/***************************************************************************
 *            sessiondialog.h
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
#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>

namespace Ui {
    class SessionDialog;
}

class SessionDialog : public QDialog {
    Q_OBJECT
public:
    SessionDialog(QWidget *parent = 0);
    ~SessionDialog();

    int getGridSize();
    int getWinSize();
    bool isHuman(int player);
    int getIA1();
    int getIA2();
    QString getStringIA1();
    QString getStringIA2();
    QString getP1Name();
    QString getP2Name();
    QColor getPen1();
    QColor getPen2();
    QColor getGridColour();
    QColor getWinColour();
    int getSetsNum();
protected:
    void changeEvent(QEvent *e);

public slots:
    void setWinGrid();
    void setDefault();
    void set5x5();
    void set7x7();
    void set9x9();
    void setCustom();

private:
    Ui::SessionDialog *ui;
};

#endif // SESSIONDIALOG_H
