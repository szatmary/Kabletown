#ifndef FOR_APP_STORE
#include "main.h"
#include "updatedialog.h"
#include "ui_updatedialog.h"

#include <QDir>
#include <QProcess>
#include <QNetworkInterface>

#define UPDATER_URL "http://tisza.co/cgi-bin/cbmupdate.cgi"
#define PROGRESS_SCALE 1000000

UpdateDialog::UpdateDialog(QWidget *parent)
: QDialog(parent)
, ui(new Ui::UpdateDialog)
, m_state(UPDATE_IDLE)
#if defined(Q_WS_MAC)
, m_filePath( QDir( QDir().tempPath() ).filePath("cbm_update.pkg") )
#elif defined(Q_WS_WIN)
, m_filePath( QDir( QDir().tempPath() ).filePath("cbm_update.exe") )
#endif
, m_file( m_filePath )
{
    ui->setupUi(this);
    ui->progressBar->setRange(0,PROGRESS_SCALE);
    connect(ui->Cancel  ,SIGNAL(clicked()), this, SLOT(rejected()));
    connect(ui->Continue,SIGNAL(clicked()), this, SLOT(accepted()));
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::checkNow()
{
    ui->Cancel->setEnabled( true );
    ui->Continue->setEnabled( true );
    ui->progressBar->setHidden( true );

    if ( UPDATE_IDLE != m_state)
        return;

    m_state = UPDATE_CHECKING;
    QUrl url( UPDATER_URL );
    url.addQueryItem("v", APP_VERSION );
    url.addQueryItem("o", os() );
    m_req.setUrl( url );

    qDebug() << "Sending request" << url;
    m_rpl = m_nam.get( m_req );
    connect(m_rpl,SIGNAL(finished()),this,SLOT(checkFinished()));
}

QString UpdateDialog::os()
{
#if defined(Q_WS_MAC)
    switch( QSysInfo::MacintoshVersion )
    {
    default: return "MV_UNKNOWN";
    case QSysInfo::MV_CHEETAH: return "MV_CHEETAH";
    case QSysInfo::MV_PUMA: return "MV_PUMA";
    case QSysInfo::MV_JAGUAR: return "MV_JAGUAR";
    case QSysInfo::MV_PANTHER: return "MV_PANTHER";
    case QSysInfo::MV_TIGER: return "MV_TIGER";
    case QSysInfo::MV_LEOPARD: return "MV_LEOPARD";
    case QSysInfo::MV_SNOWLEOPARD: return "MV_SNOWLEOPARD";
    case QSysInfo::MV_LION: return "MV_LION";
    }
#elif defined (Q_WS_WIN)
    switch( QSysInfo::WindowsVersion )
    {
    default: return "WV_UNKNOWN";
    case QSysInfo::WV_NT: return "WV_NT";
    case QSysInfo::WV_2000: return "WV_2000";
    case QSysInfo::WV_XP: return "WV_XP";
    case QSysInfo::WV_2003: return "WV_2003";
    case QSysInfo::WV_VISTA: return "WV_VISTA";
    case QSysInfo::WV_WINDOWS7: return "WV_WINDOWS7";
    }
#else
#error "OS_UNKNOWN"
#endif
}

void UpdateDialog::accepted()
{
    switch(m_state)
    {
    default: return;
    case UPDATE_AVAILABLE:
        qDebug() << "Downloading" << m_url << "to" << m_filePath;
        m_file.open( QIODevice::Truncate | QIODevice::WriteOnly );
        m_req.setUrl(m_url);
        m_rpl = m_nam.get( m_req );
        connect(m_rpl,SIGNAL(finished()),this,SLOT(dowloadFinished()));
        connect(m_rpl,SIGNAL(readyRead()),this,SLOT(readyRead()));
        connect(m_rpl,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(dowloadProgress(qint64,qint64)));
        ui->text->setText("Please wait while the update downloads.");
        ui->Continue->setEnabled( false );
        ui->progressBar->setHidden( false );
        m_state = UPDATE_DOWNLOADING;
        break;
    case UPDATE_READY:
        m_file.close();
#if defined(Q_WS_MAC)
        QProcess::startDetached("/usr/bin/open", QStringList() << m_filePath);
#elif defined(Q_WS_WIN)
        QProcess::startDetached( m_filePath );
#endif
        qApp->quit();
        break;
    }
}

void UpdateDialog::rejected()
{
    if ( UPDATE_DOWNLOADING == m_state )
    {
        m_rpl->abort();
        m_rpl->deleteLater();
    }

    hide();
    m_state = UPDATE_IDLE;
}

void UpdateDialog::readyRead()
{
    m_file.write( m_rpl->readAll() );
}

void UpdateDialog::dowloadFinished()
{
    if ( QNetworkReply::NoError == m_rpl->error() )
    {
        ui->Continue->setEnabled( true );
        ui->text->setText("Your update is ready. Click Continue to quit and install.");
        m_state = UPDATE_READY;
    } else {
        ui->text->setText("An Error occoured while downloading.");
        ui->Cancel->setEnabled( false );
        m_state = UPDATE_IDLE;
    }
}

void UpdateDialog::dowloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setValue( PROGRESS_SCALE * bytesReceived / bytesTotal );
}


void UpdateDialog::checkFinished()
{
    if ( QNetworkReply::NoError == m_rpl->error() )
    {
        int  code = m_rpl->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        m_url  = m_rpl->header( QNetworkRequest::LocationHeader ).toUrl();
        if( 302 == code && m_url.isValid() )
        {
            show();
            m_state = UPDATE_AVAILABLE;
        } else
            m_state = UPDATE_IDLE;
    } else
        m_state = UPDATE_IDLE;

    m_rpl->deleteLater();
}
#endif // FOR_APP_STORE
