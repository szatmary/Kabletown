/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include "db.h"
#include "main.h"
#include "qtsingleapplication.h"
#include "cocoaappinitializer.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QSettings>
#include <QResource>
#include <QMessageBox>
#include <QApplication>
#include <QPluginLoader>
#include <QDesktopServices>
#include <QCryptographicHash>

#ifdef Q_WS_MAC
#include <CoreFoundation/CFArray.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

Main::Main()
: m_menuBarIcon( tr( APP_NAME ) )
, m_engine(0)
{
    // set up signals
    connect(&m_prefs, SIGNAL(login()), this, SLOT(refresh()) );
    qApp->setQuitOnLastWindowClosed( false );

    // Setup Menu
    QMenu *menu = new QMenu;
    updateAction_ = menu->addAction( tr("Check Now"), this, SLOT(forceRefresh()));
    menu->addSeparator();

    asofAction_      = menu->addAction( tr("As of: %1"          ).arg( tr("Never"  ) ) );
    usedAction_      = menu->addAction( tr("Used: %1"           ).arg( tr("Unknown") ) );
    remainingAction_ = menu->addAction( tr("Remaining: %1"      ).arg( tr("Unknown") ) );
    estAction_       = menu->addAction( tr("Estimated Usage: %1").arg( tr("Unknown") ) );

    asofAction_     ->setEnabled( false );
    usedAction_     ->setEnabled( false );
    remainingAction_->setEnabled( false );
    estAction_      ->setEnabled( false );

    menu->addSeparator();
    chartsAction_      = menu->addAction( tr("Charts")        , this, SLOT(showCharts()));
    prefrencesAction_  = menu->addAction( tr("Preferences..."), this, SLOT(showPrefrences()));

    openAtLoginAction_ = menu->addAction( tr("Open at Login"), this, SLOT(toggleOpenAtLogin()));
    openAtLoginAction_->setCheckable(true);
    menu->addSeparator();
    menu->addAction( tr("Quit"), this, SLOT(quit()));

    m_menuBarIcon.setContextMenu( menu );
    openAtLoginAction_->setChecked( openAtLogin() );

    setupEngine();
    forceRefresh();
}

void Main::setupEngine()
{
    if ( 0 != m_engine )
        delete m_engine;

    QString isp = Db::getPref("isp" , "Comcast" );
    if(isp == "CableOne")
    {

    } else {
        m_engine = new CheckBw_Comcast;
    }

    connect(m_engine, SIGNAL(finished(UpdateStatus,QDate,double,double)), this    , SLOT(refreshFinished(UpdateStatus,QDate,double,double)));
    connect(m_engine, SIGNAL(finished(UpdateStatus,QDate,double,double)), &m_prefs, SLOT(refreshFinished(UpdateStatus,QDate,double,double)));
    connect(&m_refreshTimer, SIGNAL(timeout()), this    , SLOT(refresh()));
    connect(&m_refreshTimer, SIGNAL(timeout()), &m_prefs, SLOT(refresh()));
}

void Main::quit()
{
    Db::close();
    qApp->quit();
}

bool Main::openAtLogin()
{
#if defined (Q_WS_MAC)
    return ( 0 != getOpenAtLoginItem() );
#elif defined (Q_WS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
    return ! settings.value("Kabletown").toString().isEmpty();
#endif
}

// YES, this is duplicate code, it is repeated verbatim in the setOpenAtLogin function.
//		but, there were a lot of issues getting it to work as a function call so I am
//		leaving it in a working, but non-ideal state
#ifdef Q_WS_MAC
LSSharedFileListItemRef Main::getOpenAtLoginItem()
{
    LSSharedFileListItemRef existingItem = NULL;

    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef appURL = CFBundleCopyBundleURL(bundle);

    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
    if (loginItems) {
        UInt32 seed = 0U;
        CFArrayRef currentLoginItems = (CFArrayRef)LSSharedFileListCopySnapshot(loginItems, &seed);
        CFIndex count = CFArrayGetCount(currentLoginItems);

        for (int i = 0; i < count; i++)
        {
            LSSharedFileListItemRef item = (LSSharedFileListItemRef)CFArrayGetValueAtIndex(currentLoginItems, i);

            UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
            CFURLRef URL = NULL;
            OSStatus err = LSSharedFileListItemResolve(item, resolutionFlags, &URL, /*outRef*/ NULL);
            if (err == noErr) {
                Boolean foundIt = CFEqual(URL, appURL);
                CFRelease(URL);
                if (foundIt) {
                    existingItem = item;
                    return item;
                }
            }
        }
    }
    return NULL;
}
#endif


void Main::toggleOpenAtLogin()
{
    setOpenAtLogin( openAtLoginAction_->isChecked() );
}

void Main::setOpenAtLogin(bool open)
{
    if ( open == openAtLogin() )
        return; // already set to the requested state

#if defined (Q_WS_MAC)
    LSSharedFileListRef loginListRef = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);

    CFBundleRef bundle;
    CFURLRef appURL;
    bundle = CFBundleGetMainBundle();
    appURL = CFBundleCopyBundleURL(bundle);

    if (loginListRef)
    {
        LSSharedFileListItemRef loginItemRef = NULL;

        // Tell the OS that this is a startup item
        if ( open )
        {
             loginItemRef = LSSharedFileListInsertItemURL(loginListRef, kLSSharedFileListItemLast,
                 NULL, NULL, (CFURLRef)appURL, NULL, NULL);
        }
        // Tell the OS NOT to start at startup.
        else
        {
            CFBundleRef bundle = CFBundleGetMainBundle();
            CFURLRef appURL = CFBundleCopyBundleURL(bundle);

            LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
            if (loginItems)
            {
                UInt32 seed = 0U;
                CFArrayRef currentLoginItems = (CFArrayRef)LSSharedFileListCopySnapshot(loginItems, &seed);
                CFIndex count = CFArrayGetCount(currentLoginItems);

                for (int i = 0; i < count; i++)
                {
                    LSSharedFileListItemRef item = (LSSharedFileListItemRef)CFArrayGetValueAtIndex(currentLoginItems, i);

                    UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
                    CFURLRef URL = NULL;
                    OSStatus err = LSSharedFileListItemResolve(item, resolutionFlags, &URL, /*outRef*/ NULL);
                    if (err == noErr)
                    {
                        Boolean foundIt = CFEqual(URL, appURL);
                        CFRelease(URL);
                        if (foundIt)
                        {
                            LSSharedFileListItemRemove(loginItems, item);
                        }
                    }
                }
            }
        }

        // Cleanup the used refs
        if (loginItemRef)
        {
            CFRelease(loginItemRef);
        }

        CFRelease(loginListRef);
    }
#elif defined (Q_WS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if ( open ) {
        QString path = QDir::toNativeSeparators( qApp->arguments().at(0) );
        settings.setValue("Kabletown", path );
    } else {
        settings.remove("Kabletown");
    }
#endif
}

void Main::showPrefrences()
{
    m_prefs.setHidden( false );
    m_prefs.raise();
}

void Main::showCharts()
{
    m_charts.setHidden( false );
    m_charts.raise();
}

void Main::forceRefresh()
{
    m_manual_update = true;
    refresh();
}

void Main::refresh()
{
    if( ! m_engine )
        return; // error!

    // we want to reload the prefrences when we refresh
    m_refreshTimer.stop();
    int refresh_rate  = Db::getPref( "refresh_rate", QString().setNum( DEFAULT_REFRESH_RATE ) ).toInt();
    m_refreshTimer.setInterval( refresh_rate );
    m_refreshTimer.start();

    red_limit     = Db::getPref( "red_alert"   , QString().setNum( DEFAULT_RED_ALERT    ) ).toDouble();
    yellow_limit  = Db::getPref( "yellow_alert", QString().setNum( DEFAULT_YELLOW_ALERT ) ).toDouble();

    QByteArray user = QByteArray::fromBase64( Db::getPref("user").toUtf8() );
    QByteArray pass = QByteArray::fromBase64( Db::getPref("pass").toUtf8() );

    if ( user.isEmpty() || pass.isEmpty() )
    {
        //if there is no username / passwoord show prefrences!
        showPrefrences();
    } else {
        m_refrestStart = QDateTime::currentDateTime();

        m_menuBarIcon.setBlink(true);
        updateAction_->setText( tr("Checking...") );
        updateAction_->setEnabled( false );
        m_engine->checkNow();
        m_timeoutTimer.setInterval( Db::getPref( "refresh_rate", QString().setNum( DEFAULT_NET_TIMEOUT ) ).toInt() );
        connect(&m_timeoutTimer,SIGNAL(timeout()),this,SLOT(refreshTimeout()));
    }
}

void Main::refreshTimeout()
{
    refreshFinished(UPDATE_TIMEOUT, QDate(), 0, 0 );
}

void Main::refreshFinished(UpdateStatus status, QDate date, double gbs_used, double gbs_cap)
{
    m_timeoutTimer.stop();
    Db::addInfo(m_refrestStart, QDateTime::currentDateTime(), status, date, gbs_used);

    m_menuBarIcon.setBlink(false);
    updateAction_->setText( tr("Check Now") );
    updateAction_->setEnabled( true );
    m_charts.refreshChart();

    if ( UPDATE_SUCCESS != status )
    {
        if ( ERROR != m_menuBarIcon.status() || m_manual_update )
        {
            growl( tr("An error occurred while retrieving information from Comcast's website."), QSystemTrayIcon::Critical);
        }

        m_menuBarIcon.setStatus( ERROR );
        return;
    }

    double today                   = date.day();
    double days_in_month           = date.daysInMonth();
    double days_past_in_month      = qMax( 1.0, today - 1); // does not count today as passed unless today is first day of the month
    double days_remaining_in_month = days_in_month - days_past_in_month; // should never be 0

    int    gbs_remain              = gbs_cap - gbs_used;

    double gbs_used_percent        = 100 * ( gbs_used / gbs_cap );
    double gbs_used_per_day        = gbs_used / days_past_in_month;

    double gbs_remain_percent      = 100 * ( gbs_remain / gbs_cap);
    double gbs_remain_per_day      = gbs_remain / days_remaining_in_month;

    double gbs_estimate            = gbs_used_per_day * days_in_month;
    double gbs_estimate_percent    = 100 * ( gbs_estimate / gbs_cap);

    asofAction_     ->setText( tr("%1").arg( date.toString() ) );
    usedAction_     ->setText( tr("Used: %1GB - %2% - %3GB/day")     .arg( gbs_used   ).arg( D2S(gbs_used_percent  ) ).arg( D2S(gbs_used_per_day  ) ) );
    remainingAction_->setText( tr("Remaining: %1GB - %2% - %3GB/day").arg( gbs_remain ).arg( D2S(gbs_remain_percent) ).arg( D2S(gbs_remain_per_day) ) );
    estAction_      ->setText( tr("Estimated: %1GB - %2%").arg( D2S(gbs_estimate) ).arg( D2S(gbs_estimate_percent) ) );

    QString growlText;
    growlText += tr("%1\n").arg( date.toString() );
    growlText += tr("Used: %1GB - %2%\n"     ).arg( gbs_used          ).arg( D2S(gbs_used_percent    ) );
    growlText += tr("Remaining: %1GB - %2%\n").arg( gbs_remain        ).arg( D2S(gbs_remain_percent  ) );
    growlText += tr("Estimated: %1GB - %2%\n").arg( D2S(gbs_estimate) ).arg( D2S(gbs_estimate_percent) );

    if ( gbs_estimate >= red_limit )
    {
        if ( RED != m_menuBarIcon.status() || m_manual_update )
        {
            growl( growlText, QSystemTrayIcon::Critical);
        }

        m_menuBarIcon.setStatus( RED );
    }
    else if ( gbs_estimate >= yellow_limit )
    {
        if( YELLOW != m_menuBarIcon.status() || m_manual_update)
        {
            growl( growlText, QSystemTrayIcon::Warning);
        }

        m_menuBarIcon.setStatus( YELLOW );
    }
    else
    {
        if ( GREEN != m_menuBarIcon.status() || m_manual_update)
        {
            growl( growlText, QSystemTrayIcon::Information);
        }

        m_menuBarIcon.setStatus( GREEN );
    }

    m_manual_update = false;
}

void Main::growl(QString txt, QSystemTrayIcon::MessageIcon icon)
{
    if ( ! Db::getPref("no_growl", "0").toInt() )
        m_menuBarIcon.showMessage(APP_NAME, txt, icon );
}

int main(int argc, char *argv[])
{
#if defined(Q_WS_MAC)
    // setup the autorelease pool to avoid mem leak errors
    CocoaAppInitializer cocoAppInit; Q_UNUSED(cocoAppInit);
#endif

    QCoreApplication::setApplicationName( APP_NAME );
    QCoreApplication::setOrganizationName( ORG_NAME );
    QCoreApplication::setOrganizationDomain( ORG_DOMAIN );

    QtSingleApplication a(argc, argv);
    if ( a.isRunning() )
        return 0;

    a.setWindowIcon( QIcon(":/icons/icon.png") );

    Db::open();
    Main m; Q_UNUSED(m);
    return a.exec();
}
