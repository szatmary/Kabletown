/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include "charts.h"
#include "ui_charts.h"


#include "db.h"
#include "main.h"

Charts::Charts(QWidget *parent)
: QWidget(parent)
, ui(new Ui::Charts)
{
    ui->setupUi(this);
    setFixedSize( width(), height() );

    // default values
    ui->msg->setHidden( true );
    ui->year->setRange( 2010, 2038 );
    on_this_month_clicked();
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
}

Charts::~Charts()
{
    delete ui;
}

void Charts::keyPressEvent ( QKeyEvent * event )
{
    if ( Qt::ControlModifier & event->modifiers() && Qt::Key_M == event->key() )
        showMinimized();
}

void Charts::refreshChart()
{
    QDate date( ui->year->value(), ui->month->currentIndex() + 1, 1);
    QList< QPair<QDate, quint32> > data = Db::getHistoricalData( date.month(), date.year() );

    // Disable EST line for all but current month
    QDate today = QDate::currentDate();
    if ( date.year() == today.year() && date.month() == today.month() )
    {
        ui->estimated->setEnabled( true );
    } else {
        ui->estimated->setEnabled( false );
    }

    // Setup the chart
    ui->mychart->clear();
    ui->mychart->setVLabels( 0, DEFAULT_BW_LIMIT, 25 );
    ui->mychart->setHLabels( 1,date.daysInMonth() );
    ui->mychart->setRange( 0, 1, DEFAULT_BW_LIMIT, date.daysInMonth() );

    if( 2 > data.count() )
    {
        ui->msg->setHidden( false );
        ui->estimated->setEnabled( false );
    }
    else
    {
        ui->msg->setHidden( true );
    }

    if ( ui->estimated->isChecked() && ui->estimated->isEnabled() )
    {
        qreal gpd = qreal( data.last().second ) / qMax( 1, data.last().first.day() - 1);
        GraphLine *line = ui->mychart->addLine();
        line->setPen( QPen( QColor( 0x00, 0x00, 0x00) ) );
        line->addPoint(1, gpd);
        line->addPoint(date.daysInMonth(), gpd * date.daysInMonth());
    }

    if ( ui->yellowAlert->isChecked() )
    {
        GraphLine *line = ui->mychart->addLine();
        line->setPen( QPen( QColor( 0xcb, 0xb2, 0x2d) ) );
        line->addPoint(1, 0.9 * DEFAULT_BW_LIMIT / date.daysInMonth() );
        line->addPoint(date.daysInMonth(), 0.9 * DEFAULT_BW_LIMIT);
    }

    if ( ui->redAlert->isChecked() )
    {
        GraphLine *line = ui->mychart->addLine();
        line->setPen( QPen( QColor( 0xff, 0x4e, 0x52) ) );
        line->addPoint(1, DEFAULT_BW_LIMIT / date.daysInMonth() );
        line->addPoint(date.daysInMonth(), DEFAULT_BW_LIMIT);
    }

    // TODO how do I set Z order?
    // Graph total usage
    GraphLine *line2 = ui->mychart->addLine();
    line2->setPen( QPen( QColor(0x4d,0x89,0xf9) ) );
    uint32_t prev = 0;
    for( int i = 0 ; i < data.count() ; ++i)
    {
        line2->addPoint( data.at(i).first.day(), data.at(i).second - prev );
        prev = data.at(i).second;
    }

    // Graph daily usage
    GraphLine *line1 = ui->mychart->addLine();
    line1->setPen ( QPen( QColor(0x4d,0x89,0xf9) ) );
    line1->setBrush( QBrush( QColor(0x6c,0xd9,0xfd) ) );

    for( int i = 0 ; i < data.count() ; ++i)
        line1->addPoint( data.at(i).first.day(), data.at(i).second );
}

void Charts::on_prev_clicked()
{
    if( 0 == ui->month->currentIndex() )
    {
        ui->year->setValue( ui->year->value() - 1 );
        ui->month->setCurrentIndex( 11 );
    } else {
        ui->month->setCurrentIndex( ui->month->currentIndex() - 1 );
    }

    refreshChart();
}

void Charts::on_next_clicked()
{
    if( 11 == ui->month->currentIndex() )
    {
        ui->year->setValue( ui->year->value() + 1 );
        ui->month->setCurrentIndex( 0 );
    } else {
        ui->month->setCurrentIndex( ui->month->currentIndex() + 1 );
    }

    refreshChart();
}

void Charts::on_this_month_clicked()
{
    QDate date = QDate::currentDate();
    ui->year->setValue( date.year() );
    ui->month->setCurrentIndex( date.month() - 1 );
}


void Charts::on_month_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    refreshChart();
}

void Charts::on_year_valueChanged(int value)
{
    Q_UNUSED(value);
    refreshChart();
}

void Charts::on_redAlert_stateChanged(int )
{
    refreshChart();
}

void Charts::on_yellowAlert_stateChanged(int )
{
    refreshChart();
}

void Charts::on_estimated_stateChanged(int )
{
    refreshChart();
}
