#ifndef STLOGIC_H
#define STLOGIC_H
#include <QWidget>
#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>
#include "mainwindow.h"
#include <queue>
#include "aboutdialog.h"

namespace skypetab
{
class SkypeTab : public QObject
{
    Q_OBJECT
	QWidget*_trayIcon;
	QMenu*_trayMenu;
	QDialog*_aboutDialog;
	bool _nextMenuIsMainMenu;
	void onTrayIcon();
public:
    explicit SkypeTab(QObject *parent = 0);
	static std::queue<WId> pendingWindows;
	static bool initialized;
	static void onNewWindow (WId window);
	STabMainWindow* mainWindow;

protected:
	void timerEvent(QTimerEvent *);
	bool eventFilter(QObject * obj, QEvent * ev);
signals:

public slots:
	void init();
private slots:
	void onMenuShow();
};
}
#endif // STLOGIC_H
