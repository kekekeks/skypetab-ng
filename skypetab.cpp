/*
skypetab.cpp
Author:
     Nikita Tsukanov <keks9n@gmail.com>

Copyright (c) 2011 Nikita Tsukanov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "skypetab.h"
#include <QTimer>
#include <QApplication>
#include <QMouseEvent>
#include "focusguard.h"
#include "mainwindow.h"
#include "qintercept.h"
#include <QtDebug>
#include "x11.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <queue>
namespace skypetab
{

SkypeTab::SkypeTab(QObject *parent) :
    QObject(parent)
{
	QTimer::singleShot(200, this, SLOT(init()));
	AddSignalIntercept("QSystemTrayIcon", SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayMenuActivated(QSystemTrayIcon::ActivationReason)),
			SIGNAL(raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason)));
}

void SkypeTab::init()
{
	_trayIcon=0;
	_trayMenu=0;
	_aboutDialog=new AboutDialog();
	mainWindow=new STabMainWindow();
	printf("Created main window\n");
	mainWindow->show();
	this->startTimer(50);
}

void SkypeTab::onMenuShow()
{
	QList<QAction*> actions= _trayMenu->actions();
	QMenu *myMenu=new QMenu("SkypeTab");

	myMenu->addAction("About SkypeTab", _aboutDialog, SLOT(exec()));
	_trayMenu->insertMenu(actions[actions.length()-1],myMenu);

}

void SkypeTab::onTrayMenuActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason!=QSystemTrayIcon::Context)
	{
		onTrayIcon();
	}
	else
	{
		raiseTrayMenuActivated(reason);
		if(_trayIcon==0)
		{
			_trayIcon=qobject_cast<QSystemTrayIcon*>(sender());
			_trayMenu=_trayIcon->contextMenu();
			connect(_trayMenu, SIGNAL(aboutToShow()),this, SLOT(onMenuShow()));
		}
	}
}
void SkypeTab::timerEvent(QTimerEvent *)
{
	while(pendingWindows.size()!=0)
	{
		Window win=pendingWindows.front();
		pendingWindows.pop();
		QWidget* widget=QWidget::find(win);
		if(widget!=NULL)
		{
			const QMetaObject* meta=widget->metaObject();
			printf("%s (%p) is under control\n", meta->className(), widget->winId());
			static const char*names[]={"Skype::ChatWindow", "Skype::TransferWindow", 0};
			const char**name=names;
			const char* classname=meta->className();
			while(*name!=0)
			{

				if(0==strcmp(classname, *name))
				{
					mainWindow->show();
					mainWindow->activateWindow();
					X11::Flush();
					mainWindow->AddTab(widget);

					break;
				}
				name++;
			}
			if(0==strcmp(classname, "QWidget"))
			{
				QString title=widget->windowTitle();
				if(title.contains("Skype")&&title.contains("Beta"))
					mainWindow->SetMainWindow(widget);
			}
		}


	}
}



bool SkypeTab::initialized=false;
std::queue<WId> SkypeTab::pendingWindows;
void SkypeTab::onNewWindow(WId win)
{
	tryInit();
	pendingWindows.push(win);
}

void SkypeTab::tryInit()
{
	if(!initialized)
	{
		initialized=true;
		new SkypeTab();
	}
}

void SkypeTab::onTrayIcon()
{
		mainWindow->show();
		mainWindow->activateWindow();
}
}
