#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QList>
#include <QTabWidget>
#include <QSplitter>
#include "stwindowcontainer.h"
#include "contactlistcontainer.h"
#include "stwatcher.h"


namespace skypetab
{

class STabMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit STabMainWindow(QWidget *parent = 0);
    ~STabMainWindow();
	STWindowContainer *AddTab(QWidget *tab);
	void SetMainWindow(QWidget *tab);
	QWidget *activeWidget;
	void toggleContacts();
protected:
	void timerEvent(QTimerEvent*);
	virtual void resizeEvent(QResizeEvent *event);
	bool contactsHidden();
	virtual void closeEvent(QCloseEvent *);
	void windowActivationChange(bool active);
private:
	QTabWidget* _tabs;
	ContactListContainer* _contacts;
	QSplitter*_splitter;
	QList<STWatcher*> _watchers;
	QList<int> getSizes(bool contactsHidden, QSize *newSize=0);

private slots:
	void tabChanged (int index);
	void tabChangedAfterShock();
	void tabCloseRequested(int index);
};

}
#endif // MAINWINDOW_H
