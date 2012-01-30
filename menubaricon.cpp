/*
    (C) 2012 Matthew Szatmary <szatmary@gmail.com>
    This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
    Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include "menubaricon.h"

MenuBarIcon::MenuBarIcon(QString toolTip)
{
    setToolTip( toolTip );
    m_blinkTimer.setInterval(500);
    connect(&m_blinkTimer,SIGNAL(timeout()), this, SLOT(blinkTimerTimeout()));
    setBlink(false);
    setStatus(UNKNOWN);
    show();
}

QIcon MenuBarIcon::getIcon(MenuBarIconStatus status)
{
    switch(status)
    {
    case ERROR:  return QIcon(":/icons/error.png") ;
    case GREEN:  return QIcon(":/icons/green.png") ;
    case YELLOW: return QIcon(":/icons/yellow.png") ;
    case RED:    return QIcon(":/icons/red.png") ;
    default:     return QIcon(":/icons/unknown.png") ;
    }
}

void MenuBarIcon::setBlink(bool blink)
{
    if ( blink )
    {
        // start blinking with the current icon
        m_blinkStatus = m_status;
        m_blinkTimer.start();
    }
    else
    {
        m_blinkTimer.stop();
        setIcon( getIcon( m_status ) );
    }
}

void MenuBarIcon::setStatus(MenuBarIconStatus status)
{
    m_status = status;
    setIcon( getIcon( m_status ) );
}

MenuBarIconStatus MenuBarIcon::status()
{
    return m_status;
}

void MenuBarIcon::blinkTimerTimeout()
{
    switch(m_blinkStatus)
    {
    case GREEN:  m_blinkStatus = YELLOW; break;
    case YELLOW: m_blinkStatus = RED; break;
    case RED:    m_blinkStatus = UNKNOWN; break;
    default:     m_blinkStatus = GREEN; break;
    }

    setIcon( getIcon( m_blinkStatus ) );
}
