#ifndef DB_H
#define DB_H

#include "bwcheck.h"

#include <QDate>
#include <QString>
#include <QDateTime>
#include <QSqlDatabase>

extern QSqlDatabase g_db;

class Db
{
public:
    static void    open();
    static void    close();

    static void    addInfo(QDateTime start, QDateTime done, UpdateStatus status, QDate date, int gbs);
    static void    setPref(QString name, QString value);
    static QString getPref(QString name, QString defaut = QString() );
    static QList< QPair<QDate, quint32> > getHistoricalData(int start, int end);
    static QDateTime firstHistoricalSample();
    static QDateTime lastHistoricalSample();
};

#endif // DB_H
