/*
stwatcher.cpp
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

#include "stwatcher.h"

STWatcher::STWatcher(QWidget *widget,QObject *parent) :
	QObject(parent)
{
	_widget=widget;
	_destroyed=false;
	connect(widget, SIGNAL(destroyed()), this, SLOT(OnDestroy()));
}

bool STWatcher::isDestroyed()
{
	return _destroyed;
}

QWidget *STWatcher::widget()
{
	return _widget;
}

void STWatcher::OnDestroy()
{
	_destroyed=true;
}
