/*
x11.cpp
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
#include "x11.h"
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
namespace skypetab
{
namespace X11
{

	Display* XDisplay;
	void Flush()
	{
		XFlush(XDisplay);
	}
	void Sync(bool discard)
	{
		XSync(XDisplay, discard);
	}

	Window GetInputFocus()
	{
		Window w;
		int i;
		XGetInputFocus(XDisplay, &w, &i);
		return w;
	}


	int ApplicationErrorHandler(Display*, XErrorEvent* ev)
	{
		fprintf(stderr, "X11 ERROR %d, ignoring\n", ev->error_code);
		return 0;
	}

	void SetupErrorHandler ()
	{
		XSetErrorHandler(ApplicationErrorHandler);
	}

	void SetClassName(WId window, QString className)
	{
		XClassHint classHint;
		QByteArray name= className.toLocal8Bit();
		classHint.res_name = name.data();
		classHint.res_class = name.data();
		XSetClassHint(XDisplay, window, &classHint);
	}

	WId FindWindowByClass(WId parent, QString& className)
	{
		WId root, *children;
		unsigned int nchildren;
		Atom WM_CLASS = XInternAtom(X11::XDisplay, "WM_CLASS",0);
		XQueryTree(X11::XDisplay, parent, &root, &parent,&children, &nchildren);
		WId ret=0;
		for(unsigned int c=0; c<nchildren; c++)
		{
			WId win=children[c];

			unsigned long bytes_after, len;
			unsigned char *value;
			int format;
			Atom actual_type;
			if(XGetWindowProperty(X11::XDisplay,win,WM_CLASS,0,1024,False,XA_STRING, &actual_type, &format,
							   &len,&bytes_after,&value)==Success)
			{
				if(value!=0)
				{
					value[len-1]=0;
					QString wclass = QString::fromLatin1((char*)value);
					XFree(value);
					if(wclass==className)
					{
						ret=win;
						break;
					}
				}
				ret=FindWindowByClass(win, className);
				if(ret!=0)
					break;
			}
		}
		XFree(children);
		return ret;
	}
	WId FindWindowByClass(QString className)
	{
		return FindWindowByClass(XRootWindow(XDisplay, 0), className);
	}

	void SendXMessage(WId win, QString message)
	{
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xclient.type = ClientMessage;
		ev.xclient.window = win;
		ev.xclient.message_type = XA_STRING;
		ev.xclient.format = 8;

		//FIXME: possible buffer overflow
		strcpy(ev.xclient.data.b, message.toLatin1().data());
		int ret=XSendEvent(X11::XDisplay, win, 0, NoEventMask, &ev);
		printf("XSendEvent: %i\n", ret);
		XSync(X11::XDisplay, False);
	}






}
}
