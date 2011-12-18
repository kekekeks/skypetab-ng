/*
focusguard.cpp
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
#include "focusguard.h"
#include <QEvent>
#include <QApplication>
#include <QChildEvent>
#include <QMouseEvent>
#include "x11.h"
#include <X11/Xlib.h>
namespace skypetab
{
FocusGuard::FocusGuard() :
	QObject(0)
{
	_initialized=false;
}

void FocusGuard::init()
{
	if(!_initialized)
	{
		QApplication::instance()->installEventFilter(this);
		_initialized=true;
	}
}
FocusGuard FocusGuard::_instance;
time_t FocusGuard::lastManualFocusChangeTime=0;
bool FocusGuard::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type()==QEvent::MouseButtonPress)
	{
		QWidget *w=qobject_cast<QWidget*>(obj);
		QObject* parent=obj;
		while(parent!=0)
		{
			if(_watched.contains(parent))
				break;
			parent=parent->parent();
		}
		if(parent==0)
			return false;
		w=qobject_cast<QWidget*>(parent);
		lastManualFocusChangeTime=time(0);
		XSetInputFocus(X11::XDisplay, w->winId(), RevertToNone, CurrentTime);

	}
	return false;
}

void FocusGuard::addGuardedWidget(QWidget *widget)
{
	_instance.init();

	_instance._watched.append(widget);
	connect(widget,SIGNAL(destroyed(QObject*)), &_instance, SLOT(onDestroyed(QObject*)));
}

void FocusGuard::onDestroyed(QObject *obj)
{
	_watched.removeAll(obj);
}


}
