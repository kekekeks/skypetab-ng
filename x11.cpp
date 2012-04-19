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
#include <X11/Xutil.h>
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
	void SetClassName(WId window, QString className)
	{
		XClassHint classHint;
		QByteArray name= className.toLocal8Bit();
		classHint.res_name = name.data();
		classHint.res_class = name.data();
		XSetClassHint(XDisplay, window, &classHint);
	}

}
}
