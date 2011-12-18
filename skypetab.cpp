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
#include "settingsdialog.h"
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

void SkypeTab::_stage2Init()
{
	if(mainWindow!=0)
		return;
	mainWindow=new STabMainWindow();
	_trayIcon=0;
	_trayMenu=0;
	_myMenu=new QMenu("SkypeTab");
	_myMenu->addAction("Settings", new SettingsDialog(), SLOT(execIt()));
	_myMenu->addAction("About SkypeTab", new AboutDialog(), SLOT(exec()));


	printf("Created main window\n");
	mainWindow->show();
	this->startTimer(50);
	_stagingArea=(new QWidget())->winId();
}


void SkypeTab::stage1Init()
{
	static bool done=false;
	if(done)
		return;
	done=true;
	_instance=new SkypeTab();

}

void SkypeTab::stage2Init()
{
	static bool done=false;
	if(done)
		return;
	done=true;
	stage1Init();
	_instance->_stage2Init();
}


void SkypeTab::onMenuShow()
{
	if(_trayMenu==0)
		return;
	QList<QAction*> actions= _trayMenu->actions();
	if(actions.length()==0)
		return;

	_trayMenu->insertMenu(actions[actions.length()-1],_myMenu);

}

void SkypeTab::onTrayMenuActivated(QSystemTrayIcon::ActivationReason reason)
{
	stage2Init();
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



SkypeTab* SkypeTab::_instance=0;
QWidget*SkypeTab::_mainSkypeWindow=0;
QSettings SkypeTab::settings("kekekeks","skypetab-ng");
bool* SkypeTab::enabledTabClassesList=0;

const char* SkypeTab::tabClassesList[][2]={
	{"Skype::ChatWindow", "Chats"},
	{"Skype::TransferWindow", "File transfers"},
	{"",""},//default classes are above this line
	{"Skype::CallWindow", "Calls"},
{0,0}};



WId SkypeTab::onNewWindow()
{
	//We should be completely initialized at this moment

	QWidget*widget=WindowCreationInitiator;
	if(widget==0)
		return 0;
	const QMetaObject* meta=widget->metaObject();


	loadEnabledTabClassesList();
	const char* classname=meta->className();
	printf("Creating window for %s\n", classname);

	for(int c=0; tabClassesList[c][0]!=0; c++)
	{
		const char *name=tabClassesList[c][0];
		if(0==strcmp(classname, name))
		{
			QString keyName("tabClasses/");
			keyName=keyName.append(classname);

			if(enabledTabClassesList[c])
			{
				printf("%s is under control", classname);
				_instance->_pendingWindows.append(widget);
				return _instance->_stagingArea;
			}
			break;
		}
	}
	return 0;

}

void SkypeTab::onTryShow(QWidget *widget)
{
	if (_mainSkypeWindow)
		return;
	stage2Init();

	//Walk on widget's hierarhy
	while (widget)
	{
		if(0==strcmp(widget->metaObject()->className(), "QWidget"))
		{
			QString title=widget->windowTitle();
			if(title.contains("Skype")&&title.contains("Beta"))
			{
				_mainSkypeWindow=widget;

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
	stage2Init();
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

void SkypeTab::loadEnabledTabClassesList()
{
	if(enabledTabClassesList!=0)
		return;
	int cnt=0;
	for(int c=0; tabClassesList[c][0]!=0; c++)
		cnt++;
	enabledTabClassesList=new bool[cnt];
	bool isDefault=true;
	for(int c=0; tabClassesList[c][0]!=0; c++)
	{
		const char *name=tabClassesList[c][0];
		QString keyName("tabClasses/");
		keyName=keyName.append(name);
		bool enabled= (
				((!settings.contains(keyName))&&isDefault) //If no setting for this entry, use default flag
				||
				(settings.value(keyName).toInt()==1)
		);
		enabledTabClassesList[c]=enabled;
		if(name[0]==0)
			isDefault=false;
	}
}
}
