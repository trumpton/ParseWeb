#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "decodehtml.h"
#include "icons.h"

#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QRegExp>
#include <QString>
#include "../Lib/alertsound.h"
#include "../Lib/supportfunctions.h"

#define VERSION "2017-10-17"

#undef DEBUGRESPONSE

#ifdef DEBUGRESPONSE
#include "fileio.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dicSearchText->setFocus() ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::parseIni(int argc, char *argv[])
{
    QString err ;

    // Load INI file from Data directory, TrumptonApps directory, otherwise application directory

    bool iniok=false ;
    QString inifile ;

    if (!iniok) {
        inifile = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/TrumptonApps/ParseWeb.ini") ;
        iniok = ini.init(inifile) ;
    }

    if (!iniok) {
        inifile = QApplication::applicationDirPath() + QString("/ParseWeb.ini") ;
        iniok = ini.init(inifile) ;
    }

    if (!iniok || argc>2) {
        errorOkDialog(this, "ParseWeb Error", "Invalid configuration or Unable to find ParseWeb.ini.") ;

        err = "\nParseWeb.ini not found.  It must reside in ~/TrumptonApps or the executable folder\n\n"
              "ParseWeb.ini\n"
              "------------\n\n"
              " This ini file has a different section for each online resource (e.g. dictionary)\n"
              " Each section contains the following fields:\n\n"
              "    APPNAME - Name of the Application\n"
              "    SHORTAPPNAME - Shortened Name of the Application\n"
              "    URL - URL to search, with the supplied keyword being put into {{double braces}}\n"
              "    SEARCH - Search pattern with text surrounding results.  Data to be used surrounded by {{double braces}}\n"
              "    DELIMIT - Sequence marking the end of an entry\n"
              "    NEWLINE - Sequence to be used for a newline in the results\n"
              "    OUTPUTPREFIX - Test to insert at the start of an output\n"
              "    ISO8559RESPONSE - true/false - identify if the response is in ISO8559 format\n"
              "    SEARCHPROMPT - Text prompt for the GUI input\n"
              "    ACCESSIBLESEARCHNAME - Accessible name for input text prompt\n"
              "    ACCESSIBLEOUTPUTNAME - Accessible name for text output prompt\n"
              "    ICON - Icon to use for application\n\n"
              "Notes\n"
              " -----\n"
              "  Strings should be quoted.\n"
              "  Quotes should be delimited with \\\"\n"
              "\n"
              ""
              "Valid Icons\n"
              "------------\n\n"
              VALIDICONS
              "\n\n"
              "Example\n"
              "-------\n\n"
              "[synonyms]\n"
              "APPNAME=\"Synonymes\"\n"
              "SHORTAPPNAME=\"Synonymes\"\n"
              "URL=\"http://www.synonyms-fr.com/synonym.php?word={{KEYWORD}}\"\n"
              "SEARCH=\"Synonyms of {{SEARCHDATA}}</table>\"\n"
              "DELIMIT=\"</tr>\"\n"
              "OUTPUTPREFIX=\"Synonyms of\"\n"
              "ISO8559RESPONSE=\"true\"\n"
              "SEARCHPROMPT=\"French Word\"\n"
              "ACCESSIBLESEARCHNAME=\"French Synonym Search\"\n"
              "ACCESSIBLEOUTPUTNAME=\"French Synonym Results\"\n"
              "ICON=\"france\"\n"
              "\n"
              "Search Hints\n"
              "------------\n"
              "\n"
              "When searching, prefix a search with a ! to get debug information in the response\n";

        ui->dicSearchResults->setFocus() ;
        ui->dicSearchResults->setPlainText(err);

    } else {

        // Populate drop-down
        for (int i=0; i< ini.numSections(); i++) {
            QString section = ini.getSection(i) ;
            if (!ini.isset(section, DISABLED)) {
                QString sectiontxt = ini.get(section, APPNAME) ;
                if (sectiontxt=="") sectiontxt=section ;
                ui->dicResource->addItem(sectiontxt, section) ;
            }
        }

        // Select section 0
        QString firstsection = ini.getSection(0) ;
        ui->dicResource->setCurrentIndex(0) ;
        ini.selectSection(firstsection) ;

        // Override with identifies selection if provided
        if (argc==2) {
            QString section = QString(argv[1]) ;
            if (!ini.selectSection(section)) {
                err = QString("Section ") + section + QString(" does not exist in ini file") ;
            } else {
                // Select argv[1] in dropdown list
                ui->dicResource->selectData(section) ;
                ini.selectSection(section) ;
            }
        }

        ui->dicResource->setReadOnly(false);

        // Setup widget
        configureApplication() ;

        if (err=="") {
            return true ;
        } else {
            ui->dicSearchResults->setPlainText(err) ;
            ui->dicSearchResults->setFocus() ;
            return false ;
        }
    }
}


void MainWindow::configureApplication()
{
    // Application configuration
    this->setWindowTitle(ini.get(APPNAME));
    this->setAccessibleName(ini.get(APPNAME)) ;
    this->setAccessibleDescription(ini.get(APPNAME)) ;
    ui->dicSearchPrompt->setText(ini.get(SEARCHPROMPT)) ;
    ui->dicSearchText->setAccessibleName(ini.get(ACCESSIBLESEARCHNAME)) ;
    ui->dicSearchResults->setAccessibleName(ini.get(ACCESSIBLEOUTPUTNAME)) ;
    QString iconpath=QString(":/icon/") + ini.get(ICON).toLower() + ".png" ;
    QIcon icon=QIcon(iconpath) ;
    if (icon.isNull()) {
        icon=QIcon(":/icon/parseweb.png") ;
    }
    setWindowIcon(icon) ;
}

QString MainWindow::Get(QString link)
{
    QString GetResponse ;

    QNetworkAccessManager manager ;
    QNetworkReply *reply ;
    QByteArray linkba = link.toLatin1() ;
    QUrl url(linkba) ;
    QNetworkRequest request(url) ;
    QEventLoop eventLoop ;

    // Force automatic handling of 301 redirects
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true) ;

    // get the page
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    reply = manager.get(request);
    eventLoop.exec() ;
    QVariant replycode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) ;

    // Parse the response

    QNetworkReply::NetworkError err = reply->error() ;

    switch (err) {
    case QNetworkReply::ConnectionRefusedError:
        GetResponse="ERR: Connection Refused" ;
        break ;
    case QNetworkReply::RemoteHostClosedError:
        GetResponse="ERR: Remote Host Closed Connection" ;
        break ;
    case QNetworkReply::HostNotFoundError:
        GetResponse="ERR: Host Not Found" ;
        break ;
    case QNetworkReply::UnknownServerError:
        GetResponse="ERR: Unknown Server Error" ;
        break ;
    default:
        // if page load is OK, get details, else set error string
        if (replycode==302) {
            GetResponse = "No match found" ;
        } else if (replycode==503) {
            GetResponse = "ERR: Service is temporarily unavailable, busy or undergoing maintenance" ;
        } else if (replycode>=200 && replycode<=299) {

            // Site specific coding
            if (ini.isset(ISO8559RESPONSE)) {
                GetResponse = QString::fromLatin1(reply->readAll()) ;
            } else {
                GetResponse = reply->readAll() ;
            }
        } else {
          GetResponse = "ERR: " + reply->errorString() ;
        }
        break ;

    }

#ifdef DEBUGRESPONSE
    writeFile("webresponse2.html", GetResponse) ;
#endif

    return GetResponse ;
}


QString MainWindow::ParseWebResponse(QString resp)
{
    QString result ;
    if (resp.left(4).compare("ERR:")==0) {
        result = resp ;
    } else {

        // Process and parse ini entry
        QString search = ini.get(SEARCH) ;
        search = search.replace("\\", "\\\\") ;
        search = search.replace("[", "\\[") ;
        search = search.replace("]", "\\]") ;
        search = search.replace("*", "\\*") ;
        search = search.replace("+", "\\+") ;
        search = search.replace("?", "\\?") ;
        search = search.replace(".", "\\.") ;
        search = search.replace("^", "\\^") ;
        search = search.replace("$", "\\$") ;
        search = search.replace("(", "\\(") ;
        search = search.replace(")", "\\)") ;
        search = search.replace("<", "\\<") ;
        search = search.replace(">", "\\>") ;
        search = search.replace("{{SEARCHDATA}}", "(.*)") ;
        search = search.replace("{", "\\{") ;
        search = search.replace("}", "\\}") ;

        // Search and reformat the results
        QRegExp re(search) ;
        re.setMinimal(true) ;
        if (re.indexIn(resp)>=0) {
            result=re.cap(1) ;

            // Translate spaces, accents and newlines
            result.remove("\r") ;
            result.replace("\n", " ") ;
            result = decodehtml(result) ;
            result.replace("<br>", "\n") ;
            result.replace("<br/>", "\n") ;

            // Site Specific Delimiter for Entries
            if (ini.get(DELIMIT)!="") {
                result.replace(ini.get(DELIMIT),"\n----------------\n") ;
            }
            if (ini.get(NEWLINE)!="") {
                result.replace(ini.get(NEWLINE),"\n") ;
            }

            // Remove tags, rogue spaces and tidy up
            result.remove(QRegExp("<[^>]*>"));
            result.replace(QRegExp("[ \\t]+\\n"), "\n") ;
            result.replace(QRegExp("\\n+"), "\n") ;
            result.replace(QRegExp("[\\t ]+"), " ") ;
            result.replace(QRegExp("\\n "), "\n") ;
            result.replace(QRegExp("^\\W+"), "") ;

            QString outputprefix = ini.get(OUTPUTPREFIX) ;
            if (!outputprefix.isEmpty()) result = outputprefix + " " + result ;

        } else {
            result = QString("No match found\nor unable to parse response from website.") ;
        }
    }
    return result ;
}

QString MainWindow::toPercentEncoding(QString& src)
{
    QString result ;
    QByteArray ba = src.toLatin1() ;
    QByteArray pe = ba.toPercentEncoding() ;
    result = pe ;
    return result ;
}


void MainWindow::on_dicSearchText_editingFinished()
{
    if (ui->dicSearchText->text().isEmpty()) {
        ui->dicSearchResults->setPlainText("No search requested") ;
        play(Error) ;
    } else {
        QString word=ui->dicSearchText->text() ;
        this->setWindowTitle(word + " - " + ini.get(APPNAME));
        word = toPercentEncoding(word) ;

        // Site Specific Configuration
        QString url = ini.get(URL) ;
        QString resp = Get(url.replace("{{KEYWORD}}", word)) ;

        if (resp.left(14).compare("No match found")==0) {
            play(NotFound) ;
        } else if (resp.left(4).compare("ERR:")==0) {
            play(Error) ;
        } else {
            resp = ParseWebResponse(resp) ;
            if (resp.left(14).compare("No match found")==0) {
                play(NotFound) ;
            } else {
                play(Found) ;
            }
        }
        ui->dicSearchResults->setPlainText(resp) ;

        // Switch focus, but prevent additional editingFinished
        // messages from being sent (e.g. when editing finished
        // when enter pressed).

        ui->dicSearchText->blockSignals(true) ;
        ui->dicSearchResults->setFocus() ;
        ui->dicSearchText->blockSignals(false) ;
    }
}

void MainWindow::on_dicResource_textChanged(const QString &arg1)
{
    QString selection = ui->dicResource->currentData() ;
    ini.selectSection(selection) ;
    configureApplication() ;
}

void MainWindow::on_actionChange_Resource_triggered()
{
        ui->dicResource->setFocus() ;
}

void MainWindow::on_dicResource_returnPressed()
{
    ui->dicSearchText->setFocus() ;
}

void MainWindow::on_actionE_xit_triggered()
{
    close() ;
}

void MainWindow::on_action_About_triggered()
{
     warningOkDialog(this, "ParseWeb Version", QString("ParseWeb version " VERSION));
}
