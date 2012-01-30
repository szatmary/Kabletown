/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#ifndef PREFRENCES_H
#define PREFRENCES_H

#include "bwcheck.h"

#include <QMovie>
#include <QWidget>
#include <QCloseEvent>

namespace Ui {
    class Prefrences;
}

class Prefrences : public QWidget
{
    Q_OBJECT
public:
    explicit Prefrences(QWidget *parent = 0);
    ~Prefrences();
    virtual void keyPressEvent ( QKeyEvent * event );
signals:
    void login();
private slots:
    void on_signIn_clicked();
    void refreshFinished(UpdateStatus, QDate, double, double);
private:
    QMovie throbber;
    Ui::Prefrences *ui;
};

#endif // PREFRENCES_H
