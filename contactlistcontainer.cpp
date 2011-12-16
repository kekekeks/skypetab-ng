/*
contactlistcontainer.cpp
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
#include <QResizeEvent>
#include "contactlistcontainer.h"
#include "focusguard.h"
#include "skypetab.h"
#include "x11.h"

namespace skypetab
{
ContactListContainer::ContactListContainer(QWidget *parent) :
	QWidget(parent)
{
	_embedded=NULL;
	hide();
	setMinimumSize(300, 400);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

}

void ContactListContainer::resizeEvent(QResizeEvent *event)
{
	if(_embedded!=NULL)
	{
		_embedded->resize(event->size());
	}
}

void ContactListContainer::embedWindow(QWidget *window)
{
	window->setParent(this, 0);

	_embedded=window;
	startTimer(500);

	show();
	_embedded->move(0,0);
	/*
	if(_container!=NULL)
		delete _container;
	_container=new STWindowContainer();
	_container->setParent(this);
	show();
	_container->show();
	X11::Flush();
	_container->embedWindow(window);
	*/
}

void ContactListContainer::timerEvent(QTimerEvent *e)
{
	if(_embedded!=NULL)
	{
		_embedded->move(0, 0);
		if(_embedded->isHidden())
		{
			SkypeTab::_instance->raiseTrayMenuActivated(QSystemTrayIcon::Trigger);
		}
	}
	QWidget::timerEvent(e);
}

QSize ContactListContainer::sizeHint() const
{
	return QSize(200, 300);
}
}
