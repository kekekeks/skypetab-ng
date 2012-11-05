#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

Display *XDisplay;

Window FindWindowByClass(Window parent, const char* className)
{
	Window root, *children;
	unsigned int nchildren;
	Atom WM_CLASS = XInternAtom(XDisplay, "WM_CLASS",0);
	XQueryTree(XDisplay, parent, &root, &parent,&children, &nchildren);
	Window ret=0;
	for(unsigned int c=0; c<nchildren; c++)
	{
		Window win=children[c];

		unsigned long bytes_after, len;
		unsigned char *value;
		int format;
		Atom actual_type;
		if(XGetWindowProperty(XDisplay,win,WM_CLASS,0,1024,False,XA_STRING, &actual_type, &format,
						   &len,&bytes_after,&value)==Success)
		{
			if(value!=0)
			{
				value[len-1]=0;


				if(strcmp((char*)value, className) == 0)
				{
					XFree(value);
					ret=win;
					break;
				}
				XFree(value);
			}
			ret=FindWindowByClass(win, className);
			if(ret!=0)
				break;
		}
	}
	XFree(children);
	return ret;
}

Window FindWindowByClass(const char* className)
{
	return FindWindowByClass(XRootWindow(XDisplay, 0), className);
}

void SendXMessage(Window win, const char* message)
{
	XEvent ev;
	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = win;
	ev.xclient.message_type = XA_STRING;
	ev.xclient.format = 8;

	//FIXME: possible buffer overflow
	strcpy(ev.xclient.data.b, message);
	int ret=XSendEvent(XDisplay, win, 0, NoEventMask, &ev);
	printf("XSendEvent: %i\n", ret);
	XSync(XDisplay, False);
}

int main(int argc, char *argv[])
{
	XDisplay=XOpenDisplay(NULL);



	const char* windowClassName = "SkypeTab";
	const char* wClPrefix = "--skypetab-class=";
	size_t wClPrefixLen = strlen(wClPrefix);
	for(int c=1; c<argc; c++)
	{
		if(strncmp(argv[c], wClPrefix, wClPrefixLen) == 0)
		{
			windowClassName=argv[c] + wClPrefixLen;
		}
	}

	Window instance = FindWindowByClass (windowClassName);
	if(instance!=0)
	{
		SendXMessage(instance, "ACT");
		printf("Activated previous instance\n");
		return 0;
	}

	char*newArgv[1024];
	memset(newArgv, 0, sizeof(newArgv));
	newArgv[0]="skype";
	for(int c=1; c<argc; c++)
		newArgv[c]=argv[c];

	execvp("skype", newArgv);
	int err=errno;

	char errS[1024];
	errS[0]=0;
	strerror_r(err, errS, 1024);
	printf("execvp(\"skype\", ...) failed: %i %s\n", err, errS);

}
