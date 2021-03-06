#ifndef STLOGIC_H
#define STLOGIC_H
#include <QWidget>
#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QIcon>
#include "mainwindow.h"
#include <queue>
#include "aboutdialog.h"
#include <QSettings>
namespace skypetab
{
enum WinManager
{
	Generic, Tile, Unity
};

class SkypeTab : public QObject
{
	Q_OBJECT
	static QSystemTrayIcon*_trayIcon;
	QMenu*_trayMenu;
	void onTrayIcon();
	QList<QWidget*> _pendingWindows;
	virtual void timerEvent(QTimerEvent *);
	WId _stagingArea;
	QMenu*_myMenu;
	static bool *enabledTabClassesList;
	static void loadEnabledTabClassesList();
	static QPixmap* _trayPixmap;
	static int _activeMsgsCount;
	static bool _manualTrayUpdate;
	bool _oldMainWindowEnabled;
	void _stage2Init();
	bool _newStyleContactList;
public:
    explicit SkypeTab(QObject *parent = 0);
	static QWidget*_mainSkypeWindow;
	static SkypeTab*_instance;
	static QString windowClassName;
	static WId onNewWindow ();
	static void stage0Init();
	static void stage1Init();
	static void stage2Init();
	static bool onWindowActivation(QWidget*widget);
	static QIcon onSetIcon(const QIcon&, QSystemTrayIcon*);
	static QIcon GetIcon();
	STabMainWindow* mainWindow;
	static void onTryShow(QWidget*widget);
	void raiseTrayMenuActivated(QSystemTrayIcon::ActivationReason reason);
	static void updateTrayIcon(int);
	static bool needTabActivation (QWidget* widget);
	static QSettings settings;
	static WinManager winManager;
	static void onSetContextMenu(QMenu*menu);
	static const char*tabClassesList[][2];
	static bool mainSkypeWindowEnabled();
signals:
	void _raiseTrayMenuActivated (QSystemTrayIcon::ActivationReason reason);

private slots:
	void onMenuShow();
	void onTrayMenuActivated (QSystemTrayIcon::ActivationReason reason);
	void onMainWindowDestroyed();
};
}
#endif // STLOGIC_H
