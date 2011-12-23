#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit AboutDialog(QWidget *parent = 0);
	~AboutDialog();
public slots:
	void link(const QString &qsLink);
private:
	Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
