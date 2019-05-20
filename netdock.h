#ifndef NETDOCK_H
#define NETDOCK_H

#include <QDockWidget>
#include <QWebFrame>
#include "qled.h"

#include "softgamearea.h"
#include "trisnet.h"
#include "jstranslator.h"

#define VER "3.3.0"

namespace Ui {
    class netDock;
}

class netDock : public QDockWidget {
    Q_OBJECT
public:
    netDock(QWidget *parent = 0, QLed *led=0, SoftGameArea *gameArea=0);
    ~netDock();

    QString version;

public slots:
    void initialPage(bool ok);
    void logged(QString sessionID);
    void logout();
    void reload();
    void endMatch(bool winner); //Winner is true if the calling player is the set's winner
    void startMatch(bool master, int gridSize, int sets, QString p1name, QString p2name, QString channelID);
    void localeInject();

signals:
    void matchStarted();
    void sendLogout();


protected:
    void changeEvent(QEvent *e);

private:
    Ui::netDock *ui;

    JSTranslator *translator;

    QString sessionID;

    QLed *led;
    SoftGameArea *gameInstance;
    TrisNet *tnet;
};

#endif // NETDOCK_H
