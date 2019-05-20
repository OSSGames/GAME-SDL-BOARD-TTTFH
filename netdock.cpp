#include "netdock.h"
#include "ui_netdock.h"

netDock::netDock(QWidget *parent, QLed *led, SoftGameArea *gameArea):
    QDockWidget(parent),
    ui(new Ui::netDock)
{
    ui->setupUi(this);

    this->led=led;
    this->version=VER;
    gameInstance = gameArea;

    connect(gameInstance,SIGNAL(endGame(bool)),this, SLOT(endMatch(bool)));

    connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(initialPage(bool)));

    ui->webView->setUrl(QUrl(QString(baseURI)+QString(loginPage)));

    ui->webView->setContextMenuPolicy(Qt::PreventContextMenu);

    translator=new JSTranslator(this);

    connect(ui->webView->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),SLOT(localeInject()));

}

netDock::~netDock(){
    delete ui;
}

void netDock::startMatch(bool master, int gridSize, int sets, QString p1name, QString p2name, QString channelID){
    TrisNet *tnet = new TrisNet(this, master, this->ui->webView->page()->networkAccessManager(), channelID, sessionID);
    gameInstance->newNetGame(gridSize, sets, true, tnet, p1name, p2name);


    emit matchStarted();
}

void netDock::endMatch(bool winner){
    ui->webView->page()->mainFrame()->evaluateJavaScript("client.gameEnd();");
}

void netDock::localeInject(){
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject ( "Locale", translator );
}

void netDock::initialPage(bool ok){
    QWebFrame *page = ui->webView->page()->mainFrame();
    if(!ok){
        ui->webView->setHtml(QString("<h1>")+tr("Can't connect to Server!")+"</h1><br>"+tr("Maybe one of the following reasons:")+
                             "<br><ul><li>"+
                             tr("Check your Internet Connection")+"</li><li>"+
                             tr("Your client's version is too old!")+"</li><li>"+
                             tr("Server can be down for mainteance... Be Patient!")+"</li></ul>"+
                             "<input onclick='QtForm.reload()' type='button' value='"+tr("Retry")+"'/>"
                             );

    }else{        
        page->addToJavaScriptWindowObject ( "Locale", translator );
        page->evaluateJavaScript("tr();");
    }

    page->addToJavaScriptWindowObject ( "QtForm", this );


}

void netDock::reload(){
    ui->webView->setUrl(QUrl(QString(baseURI)+QString(loginPage)));
}

void netDock::logged(QString sessionID){
    this->sessionID=sessionID;
    //led->toggleValue();
    led->setValue(true);
    led->setToolTip(tr("You are Online"));
}

void netDock::logout(){
    //led->toggleValue();
    led->setValue(false);
    led->setToolTip(tr("You are Offline"));
    hide();
    emit sendLogout();
}

void netDock::changeEvent(QEvent *e)
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
