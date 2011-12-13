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
}

void SkypeTab::init()
{
	_trayIcon=0;
	_trayMenu=0;
	_nextMenuIsMainMenu=false;
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
			if((_trayMenu==0)&&(_nextMenuIsMainMenu)&&(strcmp(classname, "QMenu")==0))
			{

				_trayMenu=qobject_cast<QMenu*>(widget);
				connect(_trayMenu, SIGNAL(aboutToShow()),this, SLOT(onMenuShow()));
				onMenuShow();


			}

			if(_trayIcon==0)
			{
				_trayIcon=widget;
				_trayIcon->installEventFilter(this);
			}

		}


	}
}

bool SkypeTab::eventFilter(QObject *, QEvent *ev)
{
	if(ev->type()==QEvent::MouseButtonDblClick)
	{
		onTrayIcon();
		return true;
	}
	else if(ev->type()==QEvent::MouseButtonPress)
	{
		QMouseEvent*mev=(QMouseEvent*)ev;
		if(mev->button()==Qt::RightButton)
			_nextMenuIsMainMenu=true;
		else
		{
			onTrayIcon();
			return true;
		}
	}


	return false;
}

bool SkypeTab::initialized=false;
std::queue<WId> SkypeTab::pendingWindows;
void SkypeTab::onNewWindow(WId win)
{
	if(!initialized)
	{
		initialized=true;
		new SkypeTab();
	}
	pendingWindows.push(win);
}

void SkypeTab::onTrayIcon()
{
	if(mainWindow->isActiveWindow())
		mainWindow->toggleContacts();
	else
	{
		mainWindow->show();
		mainWindow->activateWindow();
	}
}
}
