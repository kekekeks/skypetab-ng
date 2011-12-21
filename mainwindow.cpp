/*
mainwindow.cpp
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

#define protected public
#include <QWidget>
#undef protected
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include "mainwindow.h"
#include <QTabWidget>
#include <QtDebug>
#include <focusguard.h>
#include "x11.h"
#include "skypetab.h"
namespace skypetab
{

STabMainWindow::STabMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	_contacts=new ContactListContainer();
	setMinimumSize(640,400);
	_tabs=NULL;
	_splitter=new QSplitter(this);

	_tabs=new QTabWidget();
	_tabs->setTabsClosable(true);
	_tabs->setMinimumWidth(300);
	_splitter->addWidget(_tabs);
	FocusGuard::addGuardedWidget(this);

	startTimer(300);
	activeWidget=0;
	_tabs->setFocusPolicy(Qt::NoFocus);

	connect(_tabs, SIGNAL(currentChanged(int)),  this, SLOT(tabChanged(int)));
	connect(_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));


	_splitter->insertWidget(0, _contacts);
	_splitter->setCollapsible(1, false);


	QApplication::instance()->installEventFilter(this);


}

STabMainWindow::~STabMainWindow()
{

}


STWindowContainer* STabMainWindow::AddTab(QWidget* w)
{
	STWindowContainer*c=new STWindowContainer();
	_tabs->addTab(c, w->windowTitle());
	_tabs->setCurrentWidget(c);

	c->embedWindow(w);

	return c;
}

void STabMainWindow::SetMainWindow(QWidget *tab)
{

	_contacts->embedWindow(tab);
}


void STabMainWindow::toggleContacts()
{
	_splitter->setSizes(getSizes(!contactsHidden()));
}

void STabMainWindow::timerEvent(QTimerEvent *)
{
	QIcon wicon=QApplication::windowIcon();
	bool foundActive=false;
	for(int i=0; i<_tabs->count();)
	{
		STWindowContainer *cont=(STWindowContainer*)_tabs->widget(i);
		if(!cont->isAvailable())
		{
			if(!cont->isDestroyed())
			{
				_watchers.append(new STWatcher(cont->widget()));

			}
			_tabs->removeTab(i);
			delete cont;
			continue;
		}
		QString title=cont->getShortWindowTitle(15);
		bool hasAlets=cont->hasAlerts();
		QIcon icon=cont->widget()->windowIcon();
		_tabs->setTabText(i, title);
		_tabs->setTabIcon(i, icon);

		if ((!foundActive)&&(0==strcmp (cont->widget()->metaObject()->className(), "Skype::ChatWindow")))
		{
			wicon=icon;
			if(hasAlets)
			{
				foundActive=true;
			}

		}
		i++;
	}
	if(foundActive)
		QApplication::alert(this);
	if((_tabs->count()==0)&&contactsHidden())
	{
		_splitter->setSizes(getSizes(false));
	}
	_splitter->setCollapsible(0, (_tabs->count()!=0));
	for(int i=0; i<_watchers.length();)
	{
		STWatcher*w=_watchers[i];
		bool needDelete=false;
		if(w->isDestroyed())
			needDelete=true;
		else if(w->widget()->isVisible())
		{
			needDelete=true;
			//This code forces Qt to create a new top-level window
			w->widget()->setParent(this);
			w->widget()->setParent(NULL);
		}
		if(needDelete)
		{
			_watchers.removeAt(i);
			delete w;
		}
		else
			i++;
	}
	if(_tabs->currentIndex()!=-1)
	{
		STWindowContainer *container=(STWindowContainer*)_tabs->currentWidget();
		setWindowTitle(container->getWindowTitle());
		if(isActiveWindow())
			container->setInputFocus();
	}
	else
		setWindowTitle(_contacts->title());


	if(windowIcon().cacheKey()!=wicon.cacheKey())
		setWindowIcon(wicon);

}

void STabMainWindow::resizeEvent(QResizeEvent *event)
{
	QSize s=event->size();
	SkypeTab::settings.setValue("window/geometry",QVariant::fromValue(geometry()));

	QList<int> ss=getSizes(contactsHidden(), &s);
	_splitter->resize(s);
	_splitter->setSizes(ss);
}



bool STabMainWindow::contactsHidden()
{
	return _splitter->sizes()[0]==0;
}

void STabMainWindow::closeEvent(QCloseEvent *ev)
{
	for(int i=_tabs->count()-1; i>=0; i--)
	{
		tabCloseRequested(i);
	}
	ev->accept();
}

void STabMainWindow::windowActivationChange(bool active)
{
	if(active&&(FocusGuard::lastManualFocusChangeTime<time(0)-2))
	{
		if(_tabs->count())
			_tabs->currentWidget()->setFocus();
	}
	return QMainWindow::windowActivationChange(active);
}

bool STabMainWindow::eventFilter(QObject *obj, QEvent *ev)
{
	if(ev->type()==QEvent::KeyPress)
	{
		if(!obj->isWidgetType())
			return false;
		if(_tabs->count()<2)
			return false;
		QKeyEvent*kev=(QKeyEvent*)ev;
		int action=0; // 1 - previous, 2 - next
		if(kev->modifiers()&Qt::ControlModifier)
		{
			if(kev->key()==Qt::Key_Tab)
				action=2;
			if(kev->key()==Qt::Key_PageDown)
				action=2;
			if(kev->key()==Qt::Key_PageUp)
				action=1;
		}
		if((kev->modifiers()&Qt::ShiftModifier)&&(kev->key()==Qt::Key_Tab))
			action=1;
		if(action==0)
			return false;
		QWidget *w=qobject_cast<QWidget*>(obj);
		while(w->parentWidget()!=0)
			w=w->parentWidget();
		int index=-1;
		for(int c=0; c<_tabs->count(); c++)
			if(((STWindowContainer*)_tabs->widget(c))->widget()==w)
			{
				index=c;
				break;
			}

		if(index==-1)
			return 0;

		if(action==2)
			index++;
		else if(action==1)
			index--;
		if(index<0)
			index=_tabs->count()-1;
		if(index>=_tabs->count())
			index=0;
		_tabs->setCurrentIndex(index);

	}
	return false;
}

void STabMainWindow::setVisible(bool visible)
{
	if(visible)
	{
		if(!this->isVisible())
		{
			bool maximized=SkypeTab::settings.value(tr("window/maximized"), QVariant::fromValue(false)).toBool();
			QVariant _geo=SkypeTab::settings.value("window/geometry");
			if(_geo.isValid())
			{
				setGeometry(_geo.toRect());
			}
			if(maximized)
			{
				setWindowState(Qt::WindowMaximized);
			}
			QMainWindow::setVisible(true);
		}
		QApplication::setActiveWindow(this);
	}
	else
		QMainWindow::setVisible(false);
}

void STabMainWindow::changeEvent(QEvent *ev)
{
	QMainWindow::changeEvent(ev);
	if(ev->type()==QEvent::WindowStateChange)
	{
		if(!isVisible())
			return;
		SkypeTab::settings.setValue("window/maximized", isMaximized());
		if(isMaximized())
		{
			QRect normal=normalGeometry();
			SkypeTab::settings.setValue("window/geometry",QVariant::fromValue(normal));
		}

	}
}


QList<int> STabMainWindow::getSizes(bool contactsHidden, QSize*newSize)
{

	static bool overrideHidden=true;
	if(overrideHidden)
	{
		//for first run
		contactsHidden=false;
		overrideHidden=false;

	}
	QList<int> rv;
	QSize curSize=size();
	if(newSize==0)
	{
		newSize=&curSize;
	}
	if(contactsHidden)
	{
		rv.append(0);
		rv.append(newSize->width());
		return rv;
	}
	int c=_splitter->sizes()[0];
	if(c<300)
		c=300;
	int t=newSize->width()-c;
	if(t<300)
	{
		c-=(300-t);
		t=300;
	}
	rv.append(c);
	rv.append(t);
	return rv;
}






void STabMainWindow::tabChanged(int)
{
	QTimer::singleShot(30, this, SLOT(tabChangedAfterShock()));
}

void STabMainWindow::tabChangedAfterShock()
{
	int index=_tabs->currentIndex();
	if(index!=-1)
	{
		STWindowContainer*cont=(STWindowContainer*)_tabs->currentWidget();
		X11::Flush();
		QApplication::setActiveWindow(this);
		cont->setFocus();
		X11::Flush();
	}
}

void STabMainWindow::tabCloseRequested(int index)
{
	STWindowContainer* cont=qobject_cast<STWindowContainer*>(_tabs->widget(index));
	cont->widget()->close();
}
}
