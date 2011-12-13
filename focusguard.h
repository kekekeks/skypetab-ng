#ifndef FOCUSGUARD_H
#define FOCUSGUARD_H

#include <QObject>
#include <QWidget>
#include <QList>
namespace skypetab
{
class FocusGuard : public QObject
{
	Q_OBJECT
	QList<QObject*> _watched;
	static FocusGuard _instance;
	bool _initialized;
	explicit FocusGuard();
	void init();
public:

	virtual bool eventFilter(QObject *obj, QEvent *event);
	static void addGuardedWidget(QWidget*widget);
signals:
	
public slots:
	
private slots:

	void onDestroyed(QObject *obj);
};
}
#endif // FOCUSGUARD_H
