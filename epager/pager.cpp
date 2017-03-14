/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>
#include <QButtonGroup>
#include <QToolButton>
#include <QWheelEvent>
#include <QtDebug>
#include <QSignalMapper>
#include "utils/mystyle.h"
#include "utils/xdesktoputils.h"
#include "pager.h"

//_________________________________________________________________________________
Pager::Pager( QWidget* parent)
    : QWidget( parent),
      m_pSignalMapper(new QSignalMapper(this)),
      m_DeskCount(1)
{
    this->setObjectName("DesktopSwitch");
    this->setWindowTitle(tr("Desktop Switch"));

    QFont font=parent->font();
   font.setPointSize(parent->font().pointSize());
   setFont(font);
   setContentsMargins(0,0,0,0);

    m_GroupBtns = new QButtonGroup(this);

    connect ( m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(setDesktop(int)));

    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    mHBoxLayout = new QHBoxLayout(this);
    mHBoxLayout->setSpacing(1);
    mHBoxLayout->setContentsMargins(0, 0, 0, 0);
    mHBoxLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    setContentsMargins(0, 0, 0, 0);

    m_size=QSize(26,16);

    loadSettings();
    qApp->installNativeEventFilter(this);
    rechargeDesktop();

}
void Pager::loadSettings()
{
    //get parent colors
    QSettings setting;
    setting.beginGroup("Main");
    QString mparentColor=setting.value("BgColor","#404244").toString();
    setting.endGroup();

    //get setting for the taskbar
    setting.beginGroup("Pager");

    QString  ActiveBgColor=setting.value("ActiveBgColor","#1E90FF").toString();
    QString  ActiveFgColor=setting.value("ActiveFgColor","#FFFFFF").toString();
    int      Style=setting.value("Style",3).toInt();
    setting.endGroup();

    QString mystyle;
    if(Style<1||Style>5)
        mystyle=  MyStyle::taskbarStyle(Style).arg(mparentColor);
    else
        mystyle=  MyStyle::taskbarStyle(Style).arg(mparentColor).arg(ActiveBgColor).arg(ActiveFgColor);


   setStyleSheet(mystyle);
}



bool Pager::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);


        switch (event->response_type & ~0x80) {


        case XCB_PROPERTY_NOTIFY:
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);
            //TODO fix atom name


            if(property->atom==XDesktop::atom("_NET_DESKTOP_NAMES"))
            { qDebug()<<"_NET_DESKTOP_NAMES"<<property->atom; rechargeDesktop();}
            else if(property->atom==XDesktop::atom("_NET_CURRENT_DESKTOP"))
            { qDebug()<<"_NET_CURRENT_DESKTOP"<<property->atom; actvateBtnDesktop();}
            else if(property->atom==XDesktop::atom("_NET_NUMBER_OF_DESKTOPS"))
            {  qDebug()<<"_NET_NUMBER_OF_DESKTOPS"<<property->atom; rechargeDesktop();}


            break;

            }


        }

        return false;
    }



void Pager::rechargeDesktop()
{
    int count = qMax(XDesktop::count(), 1);

    if (m_DeskCount != count)
    {

        m_DeskCount = count;
        m_desktopNames = XDesktop::names();

        setupBtns();
    }

    if (m_desktopNames != XDesktop::names())
    {
        m_desktopNames = XDesktop::names();

        setupBtns();
    }

    int activeDesk = qMax(XDesktop::active(), 0);
    m_GroupBtns->button(activeDesk)->setChecked(true);

}


//__________________________________________________________________________________
void Pager::setupBtns()
{

    foreach (QAbstractButton * b, m_GroupBtns->buttons())
    {

        m_pSignalMapper->removeMappings(b);
        m_GroupBtns->removeButton(b);
        delete b;

    }




    qDebug()<<"m_DeskCount"<<m_DeskCount;
    for (int i = 0; i < m_DeskCount; ++i)
    {

        ToolButton * btn = new ToolButton(this);
        btn->setText(QString::number(i+1));
        btn->setCheckable(true);
        btn->setToolTip( tr("Desktop %1").arg(i+1));
        m_pSignalMapper->setMapping(btn, i);
        connect(btn, SIGNAL(activated()), m_pSignalMapper, SLOT(map())) ;
        //btn->setMaximumWidth(26);
        mHBoxLayout->addWidget(btn);

        m_GroupBtns->addButton(btn, i);
    }

    int activeDesk = qMax(XDesktop::active(), 0);
    QAbstractButton * button = m_GroupBtns->button(activeDesk);
    if (button)
        button->setChecked(true);

    connect(m_GroupBtns, SIGNAL(buttonClicked(int)),
            this, SLOT(setDesktop(int)));



}

Pager::~Pager()
{

}

//_________________________________________________________________________________
void Pager::setDesktop(int desktop)
{

    XDesktop::setCurrent(desktop);
    int activeDesk = qMax(XDesktop::active(), 0);
    m_GroupBtns->button(activeDesk)->setChecked(true);

}

//__________________________________________________________________________________
void Pager::wheelEvent(QWheelEvent* e)
{
    int max = XDesktop::count()- 1;
    int delta = e->delta() < 0 ? 1 : -1;
    int current = XDesktop::active()+ delta;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    XDesktop::setCurrent(current);
}

void Pager::actvateBtnDesktop()
{
     int activeDesk = qMax(XDesktop::active(), 0);
    QAbstractButton * button = m_GroupBtns->button(activeDesk);
    if (button)
       button->setChecked(true);
    else
        rechargeDesktop();
}

/*************************Virtual**********************************/

void Pager::goDesktop(int arg)
{
    int max = XDesktop::count()- 1;

    int current = XDesktop::active()+arg;

    if (current > max)
        current = 0;
    else if (current < 0)
        current =max;

    //          qDebug()<<current<<max;
    XDesktop::setCurrent(current);
}



void Pager::setSize(QSize size)
{


    m_size= size;

    setupBtns();

}


