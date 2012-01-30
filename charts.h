#ifndef CHARTS_H
#define CHARTS_H

#include <QDate>
#include <QMovie>
#include <QTimer>
#include <QWidget>

namespace Ui
{
    class Charts;
}

class Charts : public QWidget
{
    Q_OBJECT

public:
    explicit Charts(QWidget *parent = 0);
    ~Charts();
    virtual void keyPressEvent ( QKeyEvent * event );
public slots:
    void refreshChart();
private slots:
    void on_prev_clicked();
    void on_next_clicked();
    void on_month_currentIndexChanged(int index);
    void on_year_valueChanged(int );
    void on_this_month_clicked();
    void on_redAlert_stateChanged(int );

    void on_yellowAlert_stateChanged(int );

    void on_estimated_stateChanged(int );

private:
    Ui::Charts *ui;
};

#endif // CHARTS_H
