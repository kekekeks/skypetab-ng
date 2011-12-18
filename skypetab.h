#ifndef STLOGIC_H
#define STLOGIC_H
#include <QWidget>
#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>
#include "mainwindow.h"
#include <queue>
#include "aboutdialog.h"
#include <QSettings>
namespace skypetab
{
class SkypeTab : public QObject
{
    Q_OBJECT
	QSystemTrayIcon*_trayIcon;
	QMenu*_trayMenu;
	void onTrayIcon();
	QList<QWidget*> _pendingWindows;
	virtual void timerEvent(QTimerEvent *);
	WId _stagingArea;
	QMenu*_myMenu;
	static bool *enabledTabClassesList;
	static void loadEnabledTabClassesList();


	void _stage2Init();
public:
    explicit SkypeTab(QObject *parent = 0);
	static QWidget*_mainSkypeWindow;
	static SkypeTab*_instance;
	static WId onNewWindow ();
	static void stage1Init();
	static void stage2Init();
	STabMainWindow* mainWindow;
	static void onTryShow(QWidget*widget);
	void raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason reason);
	static QSettings settings;

	static const char*tabClassesList[][2];
signals:
	void _raiseTrayMenuActivated (QSystemTrayIcon::ActivationReason reason);

private slots:
	void onMenuShow();
	void onTrayMenuActivated (QSystemTrayIcon::ActivationReason reason);
};
}
#endif // STLOGIC_H
