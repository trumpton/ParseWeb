#include "mainwindow.h"
#include "qregularexpression.h"
#include "ui_mainwindow.h"
#include "decodehtml.h"
#include "icons.h"

#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QEventLoop>
//#include <QRegExp>
#include <QRegularExpression>
#include <QString>
#include "../Lib/alertsound.h"
#include "../Lib/supportfunctions.h"
#include "fileio.h"

// Debugging
//#define DEBUGREMOVETAGS
#undef DEBUGREMOVETAGS

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
              "    URL - URL to search, with the supplied keyword location identified with {{KEYWORD}}\n"
              "    HEADER - HTTP Headers to submit, for example: Accept: *\\nOrigin: www.site.com\n"
              "    USERAGENT - Override default web browser user agent\n"
              "    TESTFILE - HTML File to use instead of URL\n"
              "    SEARCH - Search pattern with text surrounding results.  Data to be processed for the response is identified with {{SEARCHDATA}}\n"
              "    DELIMIT - Sequence marking the end of an entry\n"
              "    REMOVECOMMENTS - If set to true, ensures that <!-- --> comments are removed before parsing\n"
              "    REPLACEn - Replaces the text given with the text provided in WITHn (n=1-4).  Wildcards * and ? can be used.\n"
              "    NEWLINE - Sequence to be used for a newline in the results\n"
              "    OUTPUTPREFIX - Test to insert at the start of an output\n"
              "    ISO8559RESPONSE - true/false - identify if the response is in ISO8559 format\n"
              "    SEARCHPROMPT - Text prompt for the GUI input\n"
              "    ACCESSIBLESEARCHNAME - Accessible name for input text prompt\n"
              "    ACCESSIBLEOUTPUTNAME - Accessible name for text output prompt\n"
              "    ICON - Icon to use for application\n"
              "    DEBUG - Causes raw html to be captured to local disk\n\n"
              "Notes\n"
              " -----\n"
              "  Sequence of operation:\n"
              "    1. Load URL or File\n"
              "    2. Remove Comments if requested\n"
              "    3. SEARCH and discard any data outside {{SEARCHDATA}} - note that the '*' wildcard can be used\n"
              "    4. Replace REPLACEn sequence in HTML with WITHn\n"
              "    5. Replace <br> with newlines\n"
              "    6. Replace DELIMIT sequence in HTML with '----' delimiter\n"
              "    7. Replace NEWLINE sequence in HTML with a newline\n"
              "    8. Expand &amp; html sequences etc.\n"
              "    9. Remove all HTML tags and extra white space\n"
              "\n"
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
              "REPLACE1=\"<faultytag\"\n"
              "WITH1=\"<faultytag />\"\n"
              "DELIMIT=\"</tr>\"\n"
              "REMOVECOMMENTS=\"false\"\n"
              "OUTPUTPREFIX=\"Synonyms of\"\n"
              "ISO8559RESPONSE=\"true\"\n"
              "SEARCHPROMPT=\"French Word\"\n"
              "ACCESSIBLESEARCHNAME=\"French Synonym Search\"\n"
              "ACCESSIBLEOUTPUTNAME=\"French Synonym Results\"\n"
              "ICON=\"france\"\n"
              "DEBUG=\"false\"\n"
              "\n"
              "Search Hints\n"
              "------------\n"
              "\n"
              "When searching, prefix a search with a ! to get debug information in the response\n";

        ui->dicSearchResults->setFocus() ;
        ui->dicSearchResults->setPlainText(err);
        return true ;

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

QStringList MainWindow::BuildHeadersList(QString headers)
{
    QStringList headerslist ;
    QStringList headerentries ;
    headerentries = headers.split("\\n") ;
    for (int a=0; a<headerentries.length(); a++) {
        QStringList oneheader = headerentries.at(a).split(":") ;
        if (oneheader.length()==2) {
            headerslist.push_back(oneheader.at(0).trimmed()) ;
            headerslist.push_back(oneheader.at(1).trimmed()) ;
        }
    }
    return headerslist ;
}

QString MainWindow::Get(QString link, QString useragent, QStringList& headers)
{
    QString GetResponse ;

    QNetworkAccessManager manager ;
    QNetworkReply *reply ;
    QByteArray linkba = link.toLatin1() ;
    QUrl url(linkba) ;
    QNetworkRequest request(url) ;
    QEventLoop eventLoop ;

    //
    // Add useragent
    //

    if (useragent.isEmpty()) {
        useragent = DEFAULTUSERAGENT ;
    }
    request.setRawHeader("User-Agent", useragent.toLatin1()) ;

    //
    // Add headers
    //

    if (!headers.isEmpty()) {
        for (int a=0; a< headers.length(); a+=2) {
            request.setRawHeader(headers.at(a).toLatin1(), headers.at(a+1).toLatin1()) ;
        }
    }

    // Force automatic handling of 301 redirects
    // Not required in QT6.2 - redirects followed automatically
    // request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true) ;

    // get the page
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    reply = manager.get(request);
    eventLoop.exec() ;
    int replycode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() ;

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
                GetResponse = QString::fromUtf8(reply->readAll()) ;
            }
        } else {
          GetResponse = "ERR: " + reply->errorString() ;
        }
        break ;

    }

    return GetResponse ;
}

QString MainWindow::ReplaceString(QString whats, QString withs, QString src)
{
    QString what = ini.get(whats) ;
    what = what.replace("\\", "\\\\") ;

    QString with = ini.get(withs) ;
    with = with.replace("\\", "\\\\") ;

/*
 *     QRegExp re(what) ;
 *   re.setMinimal(true);
 *   re.setPatternSyntax(QRegExp::WildcardUnix) ;
 *   return src.replace(re, with) ;
*/

    return src.replace(what, with) ;

}

QString MainWindow::RemoveTags(QString src, bool removecommentsonly)
{
    bool removetags=!removecommentsonly ;

    bool incomment=false ;
    bool instrings=false ;
    bool instringd=false ;
    bool intag=false ;
    QString dst ;
    int len = src.length() ;

#ifdef DEBUGREMOVETAGS
    writeFile("removetags.txt", QString("")) ;
#endif

    for (int i=0; i<src.length(); i++) {

        QChar ch = src.at(i) ;
        bool justclosedtag=false ;
        bool justclosedcommenttag=false ;

        if (!incomment && !instrings && !instringd && !intag && i<len-4) {
            // Identify comment start
            if (ch==QChar('<') && src.at(i+1)==QChar('!') && src.at(i+2)==QChar('-') && src.at(i+3)==QChar('-')) {
                incomment=true ;
            }

        } else if (incomment && i>3) {
            // Identify comment end
            if (ch==QChar('>') && src.at(i-1)==QChar('-') && src.at(i-2)==QChar('-')) {
                incomment=false ;
                justclosedcommenttag=true ;
            }
        }

        if (!instrings && !instringd && !incomment && !justclosedcommenttag) {

            if (!intag && ch==QChar('<')) {
                // Identify tag start
                intag=true ;

            } else if (intag && ch==QChar('>')) {
                // Identify tag end
                intag=false ;
                justclosedtag=true ;
            }

        }

        // Track if In Single Quotes
        if (!incomment && !instringd && intag) {
            if (ch==QChar('\'')) instrings=!instrings ;
        }

        // Track if In Double Quotes
        if (!incomment && !instrings && intag) {
            if (ch==QChar('\"')) instringd=!instringd ;
        }

        // Don't output character if:
        //
        //  removing-tags and (intag or just closing a tag)
        //  removing-comments and (incomment or just closing a comment tag)
        //

        QChar outputch = ch ;
        if ((removetags && (intag || justclosedtag))) {
            outputch = QChar(' ') ;
        }
        if ((incomment || justclosedcommenttag)) {
            outputch = QChar(' ') ;
        }

        dst.append(outputch) ;

#ifdef DEBUGREMOVETAGS
        QString entry ;
        if (removetags) entry.append(QString("T")); else entry.append(QString(" ")) ;
        if (incomment) entry.append(QString("#")); else entry.append(QString(" ")) ;
        if (intag) entry.append(QString("<")); else entry.append(QString(" ")) ;
        if (justclosedtag) entry.append(QString(">")); else entry.append(QString(" ")) ;
        if (instrings) entry.append(QString("\'")); else entry.append(QString(" ")) ;
        if (instringd) entry.append(QString("\"")); else entry.append(QString(" ")) ;
        entry.append(QString(" : ")) ;
        entry.append(ch) ;
        entry.append(QString(" ... ")) ;
        entry.append(outputch) ;
        entry.append("\n") ;
        appendFile("removetags.txt", entry) ;
#endif

    }

    return dst ;
}


QString MainWindow::ParseWebResponse(QString resp)
{
    QString result ;
    QString search = ini.get(SEARCH) ;

    if (ini.get(DEBUG).toLower().compare("true")==0) {
        writeFile("parseweb-02-searchpattern.txt", search) ;
    }

    if (resp.left(4).compare("ERR:")==0) {
        result = resp ;

    } else {

        // Replace newlines and remove linefeeds from response
        resp.replace("\n", "\\n") ;
        resp.replace("\r", "") ;

        // Process and parse ini entry
        search = search.replace("\\", "\\\\") ;
        search = search.replace("[", "\\[") ;
        search = search.replace("]", "\\]") ;
        search = search.replace("*", "[^>]") ;
//        search = search.replace("*", "\\*") ;
        search = search.replace("+", "\\+") ;
        search = search.replace("?", "\\?") ;
        search = search.replace(".", "\\.") ;
        search = search.replace("^", "\\^") ;
        search = search.replace("$", "\\$") ;
        search = search.replace("(", "\\(") ;
        search = search.replace(")", "\\)") ;
//        search = search.replace("<", "\\<") ;
//        search = search.replace(">", "\\>") ;
        search = search.replace("{{SEARCHDATA}}", "(.*?)") ;
//        search = search.replace("{", "\\{") ;
//        search = search.replace("}", "\\}") ;


        if (ini.get(DEBUG).toLower().compare("true")==0) {
            writeFile("parseweb-03-parsedsearchpattern.txt", search) ;
        }

        // Remove Comments in web page if requested
        if (ini.get(REMOVECOMMENTS).toLower().compare("true")==0) {
            resp = RemoveTags(resp, true) ;
            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-04-uncommented.txt", resp) ;
            }
        }

        // Search and reformat the results
        // Note creating as static, then asigning setPattern() to suppress
        // 'Don't create temporary QRegularExpression objects' warning
        static QRegularExpression re ;
        re.setPattern(search) ;

        // Non-greedy capture allows for wildcards in delimiters
        re.setPatternOptions(QRegularExpression::MultilineOption|QRegularExpression::InvertedGreedinessOption) ;
        QRegularExpressionMatch rem ;

        rem = re.match(resp) ;

        bool hasMatch = rem.hasMatch();
        bool hasPartialMatch = rem.hasPartialMatch();

        if (hasMatch || hasPartialMatch) {

            QStringList captured = rem.capturedTexts() ;

            result = result + captured[1] ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-05-extracteddata.txt", result) ;
            }

            // Replace pre-defined sequences in the output
            result = ReplaceString(REPLACE1, WITH1, result) ;
            result = ReplaceString(REPLACE2, WITH2, result) ;
            result = ReplaceString(REPLACE3, WITH3, result) ;
            result = ReplaceString(REPLACE4, WITH4, result) ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-06-replacedwith.txt", result) ;
            }

            // Translate spaces, accents and newlines
            result.replace("\\n", " ") ;
            result.replace("<br>", "\n") ;
            result.replace("<br/>", "\n") ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-07-newlines.txt", result) ;
            }

            // Site Specific Delimiter for Entries (temporarily use \r)
            if (ini.get(DELIMIT)!="") {
                result.replace(ini.get(DELIMIT),"\r") ;
            }
            if (ini.get(NEWLINE)!="") {
                result.replace(ini.get(NEWLINE),"\n") ;
            }

            static QRegularExpression r1("[ \\t]+\\n") ; result.replace(r1, "\n") ;
            static QRegularExpression r2("\\n+") ; result.replace(r2, "\n") ;
            static QRegularExpression r3("\\n ") ; result.replace(r3, "\n") ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-08-delimit.txt", result) ;
            }

            // Remove tags
            result = RemoveTags(result) ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-09-removedtags.txt", result) ;
            }

            // Expand html escaped sequences
            result = decodehtml(result) ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-10-decodedcharacters.txt", result) ;
            }

            // Collapse \r (remove white space) and replace \r with real delimiter
            static QRegularExpression r4("\\r[ \\t\\n]+") ; result.replace(r4, "\r") ;
            static QRegularExpression r5("[\\r]+") ; result.replace(r5, "\r") ;
            static QRegularExpression r6("\\r+") ; result.replace(r6, "\n--------------------\n") ;

            // Remove rogue spaces and tidy up
            static QRegularExpression r7("[\\t ]+") ; result.replace(r7, " ") ;
            result = result.trimmed() ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-11-whitespaces.txt", result) ;
            }

            QString outputprefix = ini.get(OUTPUTPREFIX) ;
            if (!outputprefix.isEmpty()) result = outputprefix + " " + result ;

            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile("parseweb-12-output.txt", result) ;
            }

        } else {

            result = result + QString("No match found\nor unable to parse response from website.") ;

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

        QString resp ;
        QStringList headerslist ;

        // Build list of headers
        QString headers = ini.get(HEADERS) ;
        if (!headers.isEmpty()) {
            headerslist = BuildHeadersList(headers) ;
        }

        // Get data from test file or from url
        QString tf = ini.get(TESTFILE) ;
        if (!ini.get(TESTFILE).isEmpty()) {
            resp = readFile(ini.get(TESTFILE)) ;
        } else {
            QString url = ini.get(URL) ;
            if (ini.get(DEBUG).toLower().compare("true")==0) {
                writeFile(QString("parseweb-00-url.txt"), url) ;
            }
            resp  = Get(url.replace("{{KEYWORD}}", word), ini.get(USERAGENT), headerslist) ;
        }

        if (ini.get(DEBUG).toLower().compare("true")==0) {
            writeFile(QString("parseweb-01-rawresponse.txt"), resp) ;
        }

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
    Q_UNUSED(arg1) ;
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
    QString text ;

    if (QString(GITHASH).length()>0) text = text + QString("ParseWeb Version: %1.\n").arg(GITHASH) ;
    if (QString(LIBHASH).length()>0) text = text + QString("Library Version: %1.\n").arg(LIBHASH) ;
    text = text + QString("Build Date: %1.\n").arg(BUILDDATE) ;

     warningOkDialog(this, "ParseWeb", text);
}
