#ifndef MENUBARICON_H
#define MENUBARICON_H

#include <QSystemTrayIcon>
#include <QTimer>

enum MenuBarIconStatus
{
    UNKNOWN,
    ERROR,
    GREEN,
    YELLOW,
    RED
};

class MenuBarIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
    MenuBarIcon(QString toolTip);
    void setBlink(bool blink);
    void setStatus(MenuBarIconStatus status);
    MenuBarIconStatus status();
    QIcon getIcon(MenuBarIconStatus status);
private slots:
    void blinkTimerTimeout();
private:
    QTimer            m_blinkTimer;
    MenuBarIconStatus m_status;
    MenuBarIconStatus m_blinkStatus;
};

#endif // MENUBARICON_H
