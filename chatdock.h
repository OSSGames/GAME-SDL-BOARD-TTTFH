#ifndef CHATDOCK_H
#define CHATDOCK_H

#include <QDockWidget>
#include <QTextCursor>
#include <QScrollBar>
#include <QColor>

namespace Ui {
    class chatDock;
}

class chatDock : public QDockWidget {
    Q_OBJECT
public:
    chatDock(QWidget *parent, QColor p1color, QColor p2color, QString p1name);
    ~chatDock();

protected:
    void changeEvent(QEvent *e);

signals:
    void sendMess(QString mess);

public slots:
    void regMess(); //Registers a new message from THIS client
    void newMess(QString mess); //new incoming message

private:
    Ui::chatDock *ui;

    QColor p1color;
    QColor p2color;

    QString p1name;
};

#endif // CHATDOCK_H
