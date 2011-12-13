#ifndef STWATCHER_H
#define STWATCHER_H

#include <QObject>
#include <QWidget>
class STWatcher : public QObject
{
	Q_OBJECT
	bool _destroyed;
	QWidget* _widget;
public:
	explicit STWatcher(QWidget* widget, QObject *parent=0);
	bool isDestroyed();
	QWidget* widget();

signals:
	
public slots:
	void OnDestroy();
};

#endif // STWATCHER_H
