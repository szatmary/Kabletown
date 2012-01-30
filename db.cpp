/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include "db.h"

#include <QDir>
#include <QSqlQuery>
#include <QVariant>
#include <QDesktopServices>

QSqlDatabase g_db;

void Db::open()
{
    if ( ! g_db.isOpen() )
    {
        QString settingDir = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
        QDir().mkpath( settingDir );

        g_db = QSqlDatabase::addDatabase("QSQLITE");
        g_db.setDatabaseName( settingDir + "/info.db" );
        g_db.open(); // TODO test for errors

        // TODO test for missing, old versions of tables, create upgrade path for moving a database schema to the latest version
        g_db.exec( "CREATE TABLE prefs(id INTEGER PRIMARY KEY, name, val)" );
        g_db.exec( "CREATE UNIQUE INDEX idx_prefs_1 ON prefs (name)" );

        g_db.exec( "CREATE TABLE info3(id INTEGER PRIMARY KEY, start_time, elapased_time, status, date, gbs)" );
        g_db.exec( "CREATE INDEX idx_info3_1 ON info3(status,start_time)" );

//        Converts info2 to info3 table
        QSqlQuery query1("SELECT start_time, finished_time, status, date, gbs FROM info2");
        QSqlQuery query2("INSERT INTO info3 (start_time, elapased_time, status, date, gbs)VALUES(?,?,?,?,?)");
        while( query1.next() )
        {
            QDateTime start = QDateTime::fromString( query1.value(0).toString() );
            QDateTime end   = QDateTime::fromString( query1.value(1).toString() );
            query2.bindValue(0, start.toTime_t() );
            query2.bindValue(1, start.msecsTo( end ) );
            query2.bindValue(2, query1.value(2) );
            query2.bindValue(3, query1.value(3) );
            query2.bindValue(4, query1.value(4) );
            query2.exec();
        }

        g_db.exec( "DROP TABLE info2" );
    }
}

void Db::close()
{
    if ( g_db.isOpen() )
    {
        g_db.close();
    }

    QSqlDatabase::removeDatabase( g_db.connectionName() );
}

void Db::addInfo(QDateTime start, QDateTime done, UpdateStatus status, QDate date, int gbs)
{
    QSqlQuery query("INSERT INTO info3 (start_time, elapased_time, status, date, gbs)VALUES(?,?,?,?,?)");
    query.bindValue(0, start.toTime_t() );
    query.bindValue(1, start.msecsTo( done ) );
    query.bindValue(2, status );
    query.bindValue(3, date.toString() );
    query.bindValue(4, gbs );
    query.exec();
}

void Db::setPref(QString name, QString value)
{
    QSqlQuery query("REPLACE INTO prefs (name, val)VALUES(?,?)");
    query.bindValue(0, name );
    query.bindValue(1, value );
    query.exec();
}

QString Db::getPref(QString name, QString defaut )
{
    QSqlQuery query("SELECT val FROM prefs WHERE name = ?");
    query.bindValue(0, name );
    query.exec();
    return query.next() ? query.value(0).toString() : defaut;
}

QList< QPair<QDate, quint32> > Db::getHistoricalData(int month, int year)
{
    QList< QPair<QDate, quint32> > info;

    QDate day1, day2;
    int gbs1 = 0, gbs2 = 0;

    day1.setDate( year, month, 1 );
    quint32 start = QDateTime(day1, QTime(0,0,0) ).toTime_t();

    day1.setDate( year, month, day1.daysInMonth() );
    quint32 end = QDateTime(day1, QTime(23,59,59) ).toTime_t();

    day1 = QDate();

    QSqlQuery query("SELECT start_time, gbs FROM info3 WHERE status = 0 AND start_time >= ? AND start_time <= ? ORDER BY start_time");
    query.bindValue(0, start);
    query.bindValue(1, end );
    query.exec();
    while( query.next() )
    {
        day2 = QDateTime::fromTime_t( query.value(0).toInt() ).date();

        if ( ! day1.isValid() )
        {// first time through, set first valid day
            day1 = day2;
        }

        gbs2 = query.value(1).toInt();
        if ( gbs2 < gbs1 )
        {// this recors is an error, Usage can not decrease
            continue;
        }

        if ( day1 != day2 )
        { // one record/day
            info.push_back( QPair<QDate, quint32>(day1,gbs1) );;
            day1 = day2;
        }

        gbs1 = gbs2;
    }

    // add the last value in
    info.push_back( QPair<QDate, quint32>(day2,gbs1) );

    return info;
}


QDateTime Db::firstHistoricalSample()
{
    QSqlQuery query("SELECT start_time FROM info3 ORDER BY start_time ASC LIMIT 1");
    query.exec();
    query.next();
    return QDateTime::fromTime_t( query.value(0).toInt() );
}

QDateTime Db::lastHistoricalSample()
{
    QSqlQuery query("SELECT start_time FROM info3 ORDER BY start_time DESC LIMIT 1");
    query.exec();
    query.next();
    return QDateTime::fromTime_t( query.value(0).toInt() );
}
