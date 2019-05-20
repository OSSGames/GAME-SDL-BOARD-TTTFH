/***************************************************************************
 *            main.cpp
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
#include <QtGui/QApplication>
#include "tttfhgui.h"
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator myappTranslator;
    myappTranslator.load(":langs/tttfh_" + QLocale::system().name());
         a.installTranslator(&myappTranslator);

    TTTFHGui w;
    w.show();
    return a.exec();
}
