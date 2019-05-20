#include "chatdock.h"
#include "ui_chatdock.h"

chatDock::chatDock(QWidget *parent, QColor p1color, QColor p2color, QString p1name) :
    QDockWidget(parent),
    ui(new Ui::chatDock)
{
    ui->setupUi(this);
    this->p1color=p1color;
    this->p2color=p2color;
    this->p1name=p1name;
    connect(ui->send,SIGNAL(clicked()),this,SLOT(regMess()));
}

chatDock::~chatDock()
{

    delete ui;
}

void chatDock::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void chatDock::newMess(QString mess){
    QTextCursor cursor(ui->chat->textCursor());
    cursor.movePosition(QTextCursor::End);
    QStringList crumbs(mess.split(";"));
    QString color(crumbs.at(0)==p1name?p1color.name():p2color.name());
    cursor.insertHtml(QString("<b style='color:")+color+QString(";'>")+crumbs.at(0)+QString("</b>: <i>")+crumbs.at(1)+QString("</i><br>"));
    QScrollBar *bar = ui->chat->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void chatDock::regMess(){
    QString mess(ui->say->text());
    if(mess.isEmpty()) return;
    ui->say->setText("");

    //The message will be displayed in textBrowser when it is
    //received back from the net

    emit sendMess(mess);
}
