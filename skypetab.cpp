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
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include "focusguard.h"
#include "mainwindow.h"
#include "qintercept.h"
#include <QtDebug>
#include "x11.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <queue>
#include "singleinstance.h"
namespace skypetab
{

SkypeTab::SkypeTab(QObject *parent) :
    QObject(parent)
{
	mainWindow=0;
	AddSignalIntercept("QSystemTrayIcon", SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayMenuActivated(QSystemTrayIcon::ActivationReason)),
			SIGNAL(_raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason)));
	//Make sure that this library won't be loaded into any other process
	QString env=QString::fromLocal8Bit(getenv("LD_PRELOAD"));
	Dl_info nfo;
	dladdr((void*)XCreateSimpleWindow, &nfo);
	QString path=QString::fromLocal8Bit(nfo.dli_fname);
	env=env.remove(path).remove("libskypetab-ng.so");
	setenv("LD_PRELOAD", env.toLocal8Bit().data(), 1);

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
	if(!settings.value("startup/hidden", QVariant::fromValue(false)).toBool())
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
	if((settings.value("startup/activate", QVariant::fromValue(false)).toBool()) &&
				SingleInstance::activatePreviousInstance())
		exit(0);
	else
		SingleInstance::createWatcher();
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

bool SkypeTab::onWindowActivation(QWidget *widget)
{
	stage2Init();
	while(widget->parentWidget())
		widget=widget->parentWidget();
	QString cl=QString::fromLocal8Bit(widget->metaObject()->className());
	if(_instance->mainWindow->activateTab(widget))
	{
		QApplication::setActiveWindow(widget);
		X11::Flush();
		X11::Sync(false);
		return false;
	}
	return true;
}


QIcon SkypeTab::onSetIcon(const QIcon& icon, QSystemTrayIcon* tray)
{
	QSize iconsize = icon.actualSize(QSize(99,99));
	if (_trayPixmap == 0)
		_trayPixmap = new QPixmap(iconsize);

	if (_trayIcon == 0 && tray != 0)
		_trayIcon = tray;
	*_trayPixmap = icon.pixmap(iconsize);
	return GetIcon();

}

QIcon SkypeTab::GetIcon()
{
	QPixmap newPixmap = *_trayPixmap;
	QPainter painter(&newPixmap);

	QString string = "";
	if (_activeMsgsCount == 0)
		return QIcon(newPixmap);

	string = QString::number(_activeMsgsCount);
	painter.setRenderHints(QPainter::Antialiasing);

	QBrush back(QColor(40, 40, 120, 255));
	QPen front(QColor(255, 255, 255, 255));
	painter.setBrush(back);
	painter.drawEllipse(QPoint(newPixmap.width()-7, newPixmap.height()-7), 7, 7);

	painter.setPen(front);
	QFont font=painter.font();

	font.setPointSize(8);
	painter.setFont(font);
	painter.drawText(newPixmap.width()-14, newPixmap.height()-14, 14, 14, Qt::AlignHCenter | Qt::AlignVCenter, string);

	return QIcon(newPixmap);
}

void SkypeTab::updateTrayIcon(int count)
{
	if(_activeMsgsCount==count)
		return;
	_activeMsgsCount = count;
	if (_trayPixmap==0)
		return;
	if (_trayIcon != 0) {
		(_trayIcon->*realSetIcon)(GetIcon());
	}
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
		if(_trayMenu==0)
		{
			if (_trayIcon==0)
				_trayIcon=qobject_cast<QSystemTrayIcon*>(sender());

			_trayMenu=_trayIcon->contextMenu();
			connect(_trayMenu, SIGNAL(aboutToShow()),this, SLOT(onMenuShow()));
			onMenuShow();
		}
	}
}



SkypeTab* SkypeTab::_instance=0;
QWidget*SkypeTab::_mainSkypeWindow=0;
QPixmap*SkypeTab::_trayPixmap=0;
QSystemTrayIcon*SkypeTab::_trayIcon=0;
int SkypeTab::_activeMsgsCount=0;
bool SkypeTab::_manualTrayUpdate=false;
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
