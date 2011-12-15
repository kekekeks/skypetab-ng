#include <QtGui/QSystemTrayIcon>
#include <QList>
#include "skypetab.h"
#include <dlfcn.h>

namespace skypetab
{
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
		SkypeTab::tryInit();
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

/*
typedef void (QSystemTrayIcon::*setIconProto)(const QIcon &icon);
setIconProto real_setIcon=0;
extern void QSystemTrayIcon::setIcon(const QIcon &icon)
{
	if(real_setIcon==0)
	{
		real_setIcon=&QSystemTrayIcon::setIcon;
		void *ptr;
		memcpy(&ptr, &real_setIcon, sizeof(ptr));
		Dl_info nfo;
		dladdr(ptr, &nfo);
		ptr=dlsym(RTLD_NEXT, nfo.dli_sname);
		memcpy(&real_setIcon, &ptr, sizeof(ptr));
	}
	(this->*real_setIcon)(icon);

	//this->metaObject()->
}
*/
