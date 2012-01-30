#ifndef BWCHECK_H
#define BWCHECK_H

#include <QTimer>
#include <QAction>
#include <QWebView>
#include <QDateTime>
#include <QNetworkRequest>

enum UpdateStatus
{
    UPDATE_SUCCESS = 0,
    UPDATE_ERROR,
    UPDATE_TIMEOUT,
    UPDATE_LOGIN_FAILED,
    UPDATE_PARSE_ERROR
};

enum CheckBwEngine
{
    COMCAST = 0,
    CABLEONE
};

class CheckBw : public QObject
{
Q_OBJECT
public:
    CheckBw();
    virtual void checkNow() = 0;
//    virtual void stopNow() = 0;
signals:
    // status, date from website, gigs used, cap
    void finished(UpdateStatus, QDate, double, double);
};


class CheckBw_Comcast : public CheckBw
{
Q_OBJECT
public:
    CheckBw_Comcast();
    void checkNow();
//    void stopNow();
private slots:
    void finish();
    void loadFinished(bool);
    void linkClicked(QUrl);
private:
    QNetworkRequest request;
    int             gbs;
    UpdateStatus    status;

    QDateTime       start;
    QDate           date;
    QWebView        webView;
};


#endif // BWCHECK_H
