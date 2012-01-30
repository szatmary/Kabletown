/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#ifndef MAIN_H
#define MAIN_H

#define DEFAULT_BW_LIMIT        250.0
#define DEFAULT_RED_ALERT       DEFAULT_BW_LIMIT
#define DEFAULT_YELLOW_ALERT    ( 0.9 * DEFAULT_BW_LIMIT )
#define DEFAULT_REFRESH_RATE    ( 60 * 60 * 1000 ) // 1 hour
#define DEFAULT_NET_TIMEOUT     (  5 * 60 * 1000 ) // 5 Minutes

#define D2S(A) QString().setNum(A, 'f', 1 )

#define ORG_NAME        "Tisza LLC"
#define APP_NAME        "Kabletown"
#define APP_VERSION     "2.0"
#define ORG_DOMAIN      "tisza.co"

// Qt includes
#include <QResource>
#include <QSystemTrayIcon>

// app includes
#include "charts.h"
#include "bwcheck.h"
#include "updatedialog.h"
#include "prefrences.h"
#include "menubaricon.h"

// Cocoa includes
#ifdef Q_WS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

class Main : public QObject
{
Q_OBJECT
public:
    Main();
    bool openAtLogin();
    void setOpenAtLogin(bool);

#if defined(Q_WS_MAC)
    LSSharedFileListItemRef getOpenAtLoginItem();
#endif

private slots:
    void quit();
    void setupEngine();
    void toggleOpenAtLogin();

    void showPrefrences();
    void showCharts();

    void refresh();
    void forceRefresh();
    void refreshTimeout();
    void refreshFinished(UpdateStatus, QDate, double, double);

private:
    void growl(QString txt, QSystemTrayIcon::MessageIcon icon);
    MenuBarIcon m_menuBarIcon;
    bool        m_manual_update;
    QDateTime   m_refrestStart;
    QTimer      m_refreshTimer;
    QTimer      m_timeoutTimer;

    Prefrences m_prefs;
    Charts     m_charts;

    CheckBw *m_engine;

    QAction *updateAction_;
    QAction *prefrencesAction_;
    QAction *chartsAction_;

    QAction *asofAction_;
    QAction *usedAction_;
    QAction *remainingAction_;
    QAction *estAction_;
    QAction *aboutAction_;
    QAction *openAtLoginAction_;

    double red_limit;
    double yellow_limit;

    UpdateDialog updater;
};

#endif // MAIN_H
