/*
x11int.cpp
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
#include "mainwindow.h"
#include "skypetab.h"
#include <dlfcn.h>
#include "x11.h"
#include <X11/Xlib.h>
using namespace skypetab;
typedef Window (*XCreateSimpleWindow_Proto)(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
XCreateSimpleWindow_Proto orig_XCreateSimpleWindow=(XCreateSimpleWindow_Proto)dlsym(RTLD_NEXT, "XCreateSimpleWindow");

extern "C" Window XCreateSimpleWindow(Display *display, Window parent, int x, int y, unsigned int width, unsigned int height,
						   unsigned int border_width, unsigned long border, unsigned long background)
{
	X11::XDisplay=display;
	Window win= orig_XCreateSimpleWindow(display, parent, x, y, width, height, border_width, border, background);
	skypetab::SkypeTab::onNewWindow(win);
	return win;
}

typedef Window (*XCreateWindow_Proto)(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual *, unsigned long, XSetWindowAttributes *);
XCreateWindow_Proto orig_XCreateWindow=(XCreateWindow_Proto)dlsym(RTLD_NEXT,"XCreateWindow");

extern "C" Window XCreateWindow(Display *display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int
			 _class, Visual *visual, unsigned long valuemask, XSetWindowAttributes *attributes)
{
	X11::XDisplay=display;
	Window win= orig_XCreateWindow(display, parent, x, y, width, height, border_width, depth, _class, visual, valuemask, attributes);
	skypetab::SkypeTab::onNewWindow(win);
	return win;
}

