#include <QApplication>
#include <QX11Info>
#include "../x11.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	skypetab::X11::XDisplay = QX11Info::display();


	QString windowClassName = "SkypeTab";
	foreach (QString cmdArg, QApplication::instance()->arguments())
	{
		QString wClPrefix = "--skypetab-class=";
		if(cmdArg.startsWith(wClPrefix))
		{
			windowClassName=cmdArg.mid(wClPrefix.length());
		}
	}

	WId instance = skypetab::X11::FindWindowByClass (windowClassName);
	if(instance!=0)
	{
		skypetab::X11::SendXMessage(instance, "ACT");
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
