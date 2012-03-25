/*
qintercept.cpp
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

#include <QtGui/QSystemTrayIcon>
#include <QList>
#include "skypetab.h"
#include "qintercept.h"
#include <dlfcn.h>

namespace skypetab
{
	QWidget*WindowCreationInitiator=0;
	struct SignalInterceptInfo
	{
		const char* className;
		const char* sourceSignalName;
		QObject*target;
		const char* targetSlotName;
		const char* targetSignalName;
	};
	QList<SignalInterceptInfo*> _signalInterceptors;
	void AddSignalIntercept(const char* className,const char*signalName, QObject*target, const char* targetSlot, const char* targetSignal)
	{
		SignalInterceptInfo* nfo=new SignalInterceptInfo();
		nfo->className=className;
		nfo->sourceSignalName=signalName;
		nfo->target=target;
		nfo->targetSlotName=targetSlot;
		nfo->targetSignalName=targetSignal;
		_signalInterceptors.append(nfo);
	}

}
using namespace skypetab;

typedef void (QWidget::*activateWindowProto)();
activateWindowProto realActivateWindow=0;

extern void QWidget::activateWindow()
{
	if(realActivateWindow==0)
	{
		realActivateWindow=&QWidget::activateWindow;
		void *ptr;
		memcpy(&ptr, &realActivateWindow, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&realActivateWindow, &ptr, sizeof(ptr));
	}
	if(SkypeTab::onWindowActivation(this))
		(this->*realActivateWindow)();

}


typedef bool (*connect_proto) ( const QObject * sender, const char * signal, const QObject * receiver, const char * method, Qt::ConnectionType type);
connect_proto real_connect=0;

bool QObject::connect ( const QObject * sender, const char * signal, const QObject * receiver, const char * method, Qt::ConnectionType type)
{

	if(real_connect==0)
	{
		real_connect=QObject::connect;
		Dl_info nfo;
		dladdr((void*)real_connect, &nfo);
		real_connect=(connect_proto)dlsym(RTLD_NEXT, nfo.dli_sname);
		SkypeTab::stage1Init();
	}

	if(sender!=0)
	{
		const char* className=sender->metaObject()->className();
		SignalInterceptInfo*nfo=0;
		foreach(nfo, _signalInterceptors)
		{
			if((0==strcmp(nfo->className, className)))//&&(0==strcmp(nfo->sourceSignalName,signal)))
			{

				QString propName("_intercepted_");
				propName.append(signal);
				QVariant prop=sender->property(propName.toUtf8().data());
				if(prop.type()==QVariant::Invalid)
				{
					if(!real_connect(sender, signal, nfo->target, nfo->targetSlotName, Qt::AutoConnection))
						return false;
					const_cast<QObject*>(sender)->setProperty(propName.toUtf8().data(), qVariantFromValue(1));
				}
				return real_connect(nfo->target, nfo->targetSignalName, receiver, method, type);

			}
		}
	}
	return real_connect(sender, signal, receiver, method, type);
}


typedef void (QWidget::*setVisibleProto)(bool visible);
setVisibleProto realSetVisible=0;


extern void QWidget::setVisible(bool visible)
{
	if(realSetVisible==0)
	{
		//It's a virtual function and I cann't figure
		//out how to get it's symbol name at runtime

		void *ptr=dlsym(RTLD_NEXT, "_ZN7QWidget10setVisibleEb");
		memcpy(&realSetVisible, &ptr, sizeof(ptr));
	}

	SkypeTab::onTryShow(this);
	if(parentWidget()==0)
		winId(); //Force it to create a window _now_

	(this->*realSetVisible)(visible);
}

typedef void (QWidget::*setParentProto)(QWidget*parent);
setParentProto realSetParent=0;
extern void QWidget::setParent(QWidget *parent)
{
	if(realSetParent==0)
	{
		realSetParent=&QWidget::setParent;
		void *ptr;
		memcpy(&ptr, &realSetParent, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&realSetParent, &ptr, sizeof(ptr));
	}
	if(parent==0)
	{
		QWidget*oldInitiator=WindowCreationInitiator;
		WindowCreationInitiator=const_cast<QWidget*>(this);
		(this->*realSetParent)(0);
		WindowCreationInitiator=oldInitiator;
	}
	else
		return (this->*realSetParent)(parent);
}

typedef WId (QWidget::*winIdproto)() const;
winIdproto realWinId=0;
extern WId QWidget::winId() const
{
	if(realWinId==0)
	{
		realWinId=&QWidget::winId;
		void *ptr;
		memcpy(&ptr, &realWinId, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&realWinId, &ptr, sizeof(ptr));
	}
	if(parentWidget()==0)
	{
		QWidget*oldInitiator=WindowCreationInitiator;
		WindowCreationInitiator=const_cast<QWidget*>(this);
		WId rv=(this->*realWinId)();
		WindowCreationInitiator=oldInitiator;
		return rv;
	}
	else
		return (this->*realWinId)();
}

setIconProto realSetIcon=0;
extern void QSystemTrayIcon::setIcon(const QIcon &icon)
{
	if(realSetIcon==0)
	{
		realSetIcon=&QSystemTrayIcon::setIcon;
		void *ptr;
		memcpy(&ptr, &realSetIcon, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&realSetIcon, &ptr, sizeof(ptr));
	}
	QIcon new_icon = SkypeTab::onSetIcon(icon, this);
	(this->*realSetIcon)(new_icon);

}

typedef void (QSystemTrayIcon::*setContextMenuProto)(QMenu * menu);
setContextMenuProto realSetContextMenu=0;
extern void QSystemTrayIcon::setContextMenu(QMenu*menu)
{
	if(realSetContextMenu==0)
	{
		realSetContextMenu=&QSystemTrayIcon::setContextMenu;
		void *ptr;
		memcpy(&ptr, &realSetContextMenu, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&realSetContextMenu, &ptr, sizeof(ptr));
	}
	SkypeTab::onSetContextMenu(menu);
	(this->*realSetContextMenu)(menu);

}
