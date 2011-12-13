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
#include <X11/Xlib.h>
namespace skypetab
{
namespace X11
{

	Display* XDisplay;

	QString GetStringAtom(Window win, Atom atom)
	{
		static Atom utf8Atom=0;
		if(utf8Atom==0)
		{
			utf8Atom=XInternAtom(XDisplay, "UTF8_STRING",0);
		}

		Atom typeReturned;
		int formatReturned;
		unsigned long nitemsReturned, unused;
		unsigned char*ret;
		if (XGetWindowProperty(XDisplay, win, atom,
			0, 1024, false, utf8Atom, &typeReturned,
			&formatReturned, &nitemsReturned, &unused, &ret)
			 == Success)
		{
			return QString::fromUtf8((char*)ret, (formatReturned/8)*nitemsReturned);
		}
		return QString("ERROR");
	}

	QString GetWindowName(Window window)
	{
		static Atom _NET_WM_NAME=0;
		if(_NET_WM_NAME==0)
		{
			_NET_WM_NAME=XInternAtom(XDisplay, "_NET_WM_NAME",0);
		}
		return GetStringAtom(window, _NET_WM_NAME);

	}
	QList<Window> GetChildren(Window window)
	{
		Window root, parent;
		Window*children;
		unsigned int nchildren=0;
		XQueryTree(XDisplay, window,&root, &parent, &children, &nchildren);
		QList<Window> rv;
		for(unsigned int c=0; c<nchildren;c++)
			rv.append(children[c]);
		return rv;
	}
	void Flush()
	{
		XFlush(XDisplay);
	}
	Window GetInputFocus()
	{
		Window w;
		int i;
		XGetInputFocus(XDisplay, &w, &i);
		return w;
	}

}
}
