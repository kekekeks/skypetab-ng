#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QBoxLayout>
namespace Ui {
class SettingsDialog;
}
namespace skypetab
{
class SettingsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();
	
private:
	Ui::SettingsDialog *ui;
	void addSettingsCheckbox(QBoxLayout*layout, QString title, QString setting, bool defValue);
protected:

public slots:
	virtual void execIt();
private slots:
	void namedCheckboxChanged(int value);

};
}
#endif // SETTINGSDIALOG_H
