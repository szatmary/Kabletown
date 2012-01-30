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
