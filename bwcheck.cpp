#include "db.h"
#include "bwcheck.h"

#include <QDebug>
#include <QBuffer>
#include <QWebFrame>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>

#define COMCAST_LOGIN_URL  "https://login.comcast.net/login?forceAuthn=1&continue=%2fSecure%2fUsageMeterDetail.aspx&s=ccentral-cima&r=comcast.net"
#define COMCAST_LOGOUT_URL "https://customer.comcast.com/LogOut/logout.aspxs"

CheckBw::CheckBw()
{}

CheckBw_Comcast::CheckBw_Comcast()
: CheckBw()
, request( QUrl( COMCAST_LOGIN_URL ) )
, gbs(0)
, status(UPDATE_TIMEOUT)
{}

void CheckBw_Comcast::checkNow()
{
    QByteArray user = QByteArray::fromBase64( Db::getPref("user").toAscii() );
    QByteArray pass = QByteArray::fromBase64( Db::getPref("pass").toAscii() );

    webView.settings()->setAttribute( QWebSettings::AutoLoadImages        , false );
    webView.settings()->setAttribute( QWebSettings::PrivateBrowsingEnabled, true );

    connect( &webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    webView.page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    connect(webView.page(),SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    if ( user.isEmpty() || pass.isEmpty() )
    {
        status = UPDATE_LOGIN_FAILED;
        finish();
    } else {
        QByteArray postData = QString("user=").toUtf8() + user + QString("&passwd=").toUtf8() + pass;
        start = QDateTime::currentDateTime();
        webView.load(request, QNetworkAccessManager::PostOperation, postData);
    }
}

void CheckBw_Comcast::linkClicked(QUrl url)
{
    qDebug() << "linkClicked( " << url << " )";
}


void CheckBw_Comcast::finish()
{
    webView.stop();
    Db::addInfo(start, QDateTime::currentDateTime(), status, date, gbs);
    emit finished(status, date, gbs, 250); // Comcast cap is hard coded as 250
}

void CheckBw_Comcast::loadFinished(bool ok)
{
    if ( webView.url().isEmpty() )
    {// Network is likely down
        status = UPDATE_ERROR;
        finish();
    }

    if ( ! ok )
    {
        // TODO look into why this failed, Maybe we could not connect
        return;
    }

    QString html = webView.page()->mainFrame()->toHtml();

    qDebug() << "Loaded" << webView.url();
    if ("/LogOut/logout.aspxs" == webView.url().path())
    {
        // Status should already be set if we get here
        finish();
    }
    else
    if ("/Secure/Users.aspx" == webView.url().path())
    {
        status = UPDATE_PARSE_ERROR;
        finish();
    }
    else
    if ("/login" == webView.url().path() && -1 != html.indexOf("The username and password entered do not match") )
    { // TODO figure out if login passed
        status = UPDATE_LOGIN_FAILED;
        finish();
    }
    else
    if ( webView.url().path().endsWith("UsageMeterDetail.aspx") )
    {
        QRegExp datere("As of (\\d+/\\d+/\\d+)");
        QRegExp bwre("UsedWrapper\">(?:&lt;)?(\\d+)GB");
        if ( -1 == bwre.indexIn(html) )
        {
            status = UPDATE_PARSE_ERROR;
        }
        else
        {
            // On less thing to go wrong. If we can't parse the date, just use todays date
            if ( -1 == datere.indexIn(html) )
                date = QDate::currentDate();
            else
                date   = QDate::fromString( datere.cap(1), "M/d/yyyy" );

            status = UPDATE_SUCCESS;
            gbs    = bwre.cap( 1 ).toInt();
        }

        webView.load( QUrl( COMCAST_LOGOUT_URL ) );
    }
}
