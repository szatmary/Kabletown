/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

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
