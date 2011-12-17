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
	mainWindow=0;
	AddSignalIntercept("QSystemTrayIcon", SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayMenuActivated(QSystemTrayIcon::ActivationReason)),
			SIGNAL(_raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason)));
}

void SkypeTab::init()
{
	if(mainWindow!=0)
		return;
	mainWindow=new STabMainWindow();
	_trayIcon=0;
	_trayMenu=0;
	_aboutDialog=new AboutDialog();

	printf("Created main window\n");
	mainWindow->show();
	this->startTimer(50);
	_stagingArea=(new QWidget())->winId();
}


void SkypeTab::onMenuShow()
{
	if(_trayMenu==0)
		return;
	QList<QAction*> actions= _trayMenu->actions();
	if(actions.length()==0)
		return;
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
			onMenuShow();
		}
	}
}



bool SkypeTab::initialized=false;
SkypeTab* SkypeTab::_instance=0;
QWidget*SkypeTab::_mainSkypeWindow=0;
static bool _returnImmediately=false;
WId SkypeTab::onNewWindow()
{
	tryInit();
	QWidget*widget=WindowCreationInitiator;
	if(widget==0)
		return 0;
	if(_returnImmediately)
		return 0;
	const QMetaObject* meta=widget->metaObject();

	static const char*names[]={"Skype::ChatWindow", "Skype::TransferWindow", 0};
	const char**name=names;
	const char* classname=meta->className();
	while(*name!=0)
	{

		if(0==strcmp(classname, *name))
		{
			_instance->_pendingWindows.append(widget);
			return _instance->_stagingArea;
		}
		name++;
	}
	return 0;

}

void SkypeTab::tryInit()
{
	if(!initialized)
	{
		initialized=true;
		_instance=new SkypeTab();
	}
}

void SkypeTab::onTryShow(QWidget *widget)
{
	if (_mainSkypeWindow)
		return;
	tryInit();

	//Walk on widget's hierarhy
	while (widget)
	{
		if(0==strcmp(widget->metaObject()->className(), "QWidget"))
		{
			QString title=widget->windowTitle();
			if(title.contains("Skype")&&title.contains("Beta"))
			{
				_mainSkypeWindow=widget;
				_instance->init();
				_instance->mainWindow->SetMainWindow(widget);
				break;
			}
		}
		widget=widget->parentWidget();
	}
}

void SkypeTab::raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason reason)
{
	_raiseTrayMenuActivated(reason);
}

void SkypeTab::onTrayIcon()
{
		mainWindow->show();
		mainWindow->activateWindow();
}

void SkypeTab::timerEvent(QTimerEvent *)
{
	while(_pendingWindows.length()!=0)
	{
		QWidget*widget=_pendingWindows[0];
		_pendingWindows.removeAt(0);

		mainWindow->show();
		X11::Flush();

		mainWindow->AddTab(widget);
		mainWindow->activateWindow();

	}
}
}
