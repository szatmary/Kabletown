#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H
#ifndef FOR_APP_STORE

#include <QFile>
#include <QDialog>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace Ui {
    class UpdateDialog;
}

enum UpdateState
{
    UPDATE_CHECKING,
    UPDATE_AVAILABLE,
    UPDATE_DOWNLOADING,
    UPDATE_READY,
    UPDATE_IDLE
};

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();
    void checkNow();
private slots:
    void accepted();
    void rejected();
    void readyRead();
    void checkFinished();
    void dowloadFinished();
    void dowloadProgress(qint64,qint64);
private:
    QString os();

    Ui::UpdateDialog *ui;
    UpdateState m_state;

    QUrl                  m_url;
    QString               m_filePath;
    QFile                 m_file;
    QNetworkRequest       m_req;
    QNetworkReply        *m_rpl;
    QNetworkAccessManager m_nam;
};

#endif // FOR_APP_STORE
#endif // UPDATEDIALOG_H
