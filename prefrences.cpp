/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include "db.h"
#include "main.h"
#include "prefrences.h"
#include "ui_prefrences.h"

#include <QDebug>

Prefrences::Prefrences(QWidget *parent)
: QWidget(parent)
, throbber(":/images/throbber.gif")
, ui(new Ui::Prefrences)
{
    ui->setupUi( this );
    setWindowTitle( APP_NAME );
    QByteArray user = QByteArray::fromBase64( Db::getPref("user").toUtf8() );
    QByteArray pass = QByteArray::fromBase64( Db::getPref("pass").toUtf8() );
    ui->user->setText( user );
    ui->pass->setText( pass );

    throbber.setScaledSize( ui->throbber->size() );

    // add version number to string
    ui->appLabel->setText( ui->appLabel->text().arg( APP_VERSION )  );

    setFixedSize( width(), height() );
//    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
}

Prefrences::~Prefrences()
{
    delete ui;
}

void Prefrences::keyPressEvent ( QKeyEvent * event )
{
    if ( Qt::ControlModifier & event->modifiers() && Qt::Key_M == event->key() )
        showMinimized();
}


void Prefrences::on_signIn_clicked()
{
    Db::setPref("user", ui->user->text().toUtf8().toBase64() );
    Db::setPref("pass", ui->pass->text().toUtf8().toBase64() );
    Db::setPref("isp" , ui->service->currentText() );

    ui->error->setText("");

    throbber.start();
    ui->throbber->setMovie( &throbber );

    ui->signIn->setEnabled( false );
    ui->user->setEnabled( false );
    ui->pass->setEnabled( false );
    emit login();
}

void Prefrences::refreshFinished(UpdateStatus status, QDate date, double gbs, double cap)
{
    Q_UNUSED(date);
    Q_UNUSED(gbs);

    ui->throbber->clear();
    throbber.stop();

    ui->signIn->setEnabled( true );
    ui->user->setEnabled( true );
    ui->pass->setEnabled( true );

    switch(status)
    {
    case UPDATE_SUCCESS:
        setHidden(true);
        ui->error->setText("");
        break;
    case UPDATE_ERROR:
        ui->error->setText("<font color=\"red\">Could not reach Comcast's website.</font><br><a href=\"http://tisza.co/Tisza_LLC/Kabletown_FAQ.html\">Click Here</a> for more information.");
        break;
    case UPDATE_TIMEOUT:
        ui->error->setText("<font color=\"red\">Timed out.</font><br><a href=\"http://tisza.co/Tisza_LLC/Kabletown_FAQ.html\">Click Here</a> for more information.");
        break;
    case UPDATE_LOGIN_FAILED:
        ui->error->setText("<font color=\"red\">Incorrect username or password.</font><br><a href=\"http://tisza.co/Tisza_LLC/Kabletown_FAQ.html\">Click Here</a> for more information.");
        break;
    case UPDATE_PARSE_ERROR:
        ui->error->setText("<font color=\"red\">Unknown or unexpected response from server</font><br><a href=\"http://tisza.co/Tisza_LLC/Kabletown_FAQ.html\">Click Here</a> for more information.");
        break;
    }
}

