/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

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
