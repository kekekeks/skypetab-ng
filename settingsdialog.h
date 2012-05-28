#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QBoxLayout>
#include <QCheckBox>
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
	void addSettingsCheckbox(QBoxLayout*layout, QString title, QString setting, bool defValue, QList<QString> &disables);
	void addSettingsCheckbox(QBoxLayout*layout, QString title, QString setting, bool defValue, QString disables);
	void addSettingsCheckbox(QBoxLayout*layout, QString title, QString setting, bool defValue);
	QString autostartPath;
protected:

public slots:
	virtual void execIt();
private slots:
	void namedCheckboxChanged(int value);
	void checkBoxChanged (QCheckBox* cb, int value);

};
}
#endif // SETTINGSDIALOG_H
