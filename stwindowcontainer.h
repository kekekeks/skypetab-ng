#ifndef STWINDOWCONTAINER_H
#define STWINDOWCONTAINER_H

#include <QWidget>
#include <QEvent>
namespace skypetab
{
class STWindowContainer : public QWidget
{
	Q_OBJECT
	QWidget *_embedded;
	QWidget *_inputArea;
	bool _destroyed;
public:

	explicit STWindowContainer(QWidget* parent=0);
	void setInputFocus();
	void embedWindow (QWidget* widgetToEmbed);

	virtual void resizeEvent ( QResizeEvent * event );
	QString getWindowTitle();
	QString getShortWindowTitle(int maxSize);
	bool hasAlerts();
	bool isDestroyed();
	bool isAvailable();
	QWidget* widget();
	QWidget* inputArea();
	time_t _embeddedAt;
	virtual void focusInEvent(QFocusEvent *);
	virtual void dragEnterEvent(QDragEnterEvent *);
	virtual void dropEvent(QDropEvent *ev);
signals:
    
private slots:
	void onDestroyed(QObject*);
	void delayedEmbed();
};
}
#endif // STWINDOWCONTAINER_H
