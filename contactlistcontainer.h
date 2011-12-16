#ifndef CONTACTLISTCONTAINER_H
#define CONTACTLISTCONTAINER_H

#include <QWidget>
#include "stwindowcontainer.h"
namespace skypetab
{
class ContactListContainer : public QWidget
{
	Q_OBJECT
	QWidget* _embedded;
public:
	explicit ContactListContainer(QWidget *parent = 0);
	virtual void resizeEvent ( QResizeEvent * event );
	void embedWindow (QWidget*window);
	virtual void timerEvent(QTimerEvent * e);
	virtual QSize sizeHint() const;
	QString title();
signals:
	
public slots:
	
};
}
#endif // CONTACTLISTCONTAINER_H
