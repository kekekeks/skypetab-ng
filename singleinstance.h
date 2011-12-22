#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QWidget>
namespace skypetab
{
class SingleInstance : public QWidget
{
	Q_OBJECT
	static SingleInstance* _instance;
public:
	explicit SingleInstance(QWidget *parent = 0);
	static bool activatePreviousInstance();
	static void createWatcher();
	bool x11Event(XEvent *ev);
signals:
	
public slots:
	
};
}
#endif // SINGLEINSTANCE_H
