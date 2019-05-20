#ifndef JSTRANSLATOR_H
#define JSTRANSLATOR_H

#include <QObject>
#include <QString>

class JSTranslator : public QObject
{
Q_OBJECT
public:
    explicit JSTranslator(QObject *parent = 0);

signals:

public slots:
    QString username(){return tr("Username:");};
    QString password(){return tr("Password:");};
    QString send(){return tr("Send");};
    QString reg(){return tr("Register for free");};
    QString confirm(){return tr("Confirm Password:");};
    QString back(){return tr("Return to login");};
    QString play(){return tr("Play");};
    QString reject(){return tr("Reject");};
    QString accept(){return tr("Accept");};
    QString wants(){return tr("wants to play with you");};
    QString user(){return tr("User");};
    QString login(){return tr("Login");};
    QString logout(){return tr("Logout");};
    QString cancel(){return tr("Cancel");}
    QString setsnum(){return tr("Number of Sets:");};
    QString mode(){return tr("Mode:");};
    QString win3(){return tr("win in 3");};
    QString win5(){return tr("win in 5");};
    QString win6(){return tr("win in 6");};
    QString free(){return tr("Free");};
    QString grid(){return tr("Grid");};
    QString sets(){return tr("Sets");};
    QString reqs(){return tr("Requests");};
    QString code(){return tr("Insert code below:");};    
};

#endif // JSTRANSLATOR_H
