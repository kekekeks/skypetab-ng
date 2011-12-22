#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "skypetab.h"
#include <QCheckBox>
#include <QMessageBox>
namespace skypetab
{
SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	QWidget*group=findChild<QWidget*>("tabClasses");
	QBoxLayout*layout=new QBoxLayout(QBoxLayout::TopToBottom);
	group->setLayout(layout);
	//I know that copy/paste is a bad practice, but I'm too lazy to extract a function
	bool isDefault=true;
	for(int c=0; SkypeTab::tabClassesList[c][0]!=0; c++)
	{
		const char *name=SkypeTab::tabClassesList[c][0];
		if(name[0]==0)
			isDefault=false;
		else
		{
			QString keyName("tabClasses/");
			keyName=keyName.append(name);

			addSettingsCheckbox(layout,QString::fromAscii(SkypeTab::tabClassesList[c][1]),keyName, isDefault);
			/*
			bool isChecked=(
					((!SkypeTab::settings.contains(keyName))&&isDefault) //If no setting for this entry, use default flag
					||
					(SkypeTab::settings.value(keyName).toInt()==1)
			);
			QCheckBox* cb=new QCheckBox(QString::fromAscii(SkypeTab::tabClassesList[c][1]));
			cb->setChecked(isChecked);
			cb->setObjectName(keyName);
			layout->addWidget(cb);
			connect(cb, SIGNAL(stateChanged(int)), this, SLOT(namedCheckboxChanged(int)));
			cb->show();
			*/
		}
	}
	group=findChild<QWidget*>("startOptions");
	layout=new QBoxLayout(QBoxLayout::TopToBottom);
	group->setLayout(layout);
	addSettingsCheckbox(layout, "Don't show window at start", "startup/hidden", false);
	this->resize(width(), minimumHeight());
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::addSettingsCheckbox(QBoxLayout *layout, QString title, QString keyName, bool isDefault)
{
	bool isChecked=(
			((!SkypeTab::settings.contains(keyName))&&isDefault) //If no setting for this entry, use default flag
			||
			(SkypeTab::settings.value(keyName).toInt()==1)
	);
	QCheckBox* cb=new QCheckBox(title);
	cb->setChecked(isChecked);
	cb->setObjectName(keyName);
	layout->addWidget(cb);
	connect(cb, SIGNAL(stateChanged(int)), this, SLOT(namedCheckboxChanged(int)));
	cb->show();
}



void SettingsDialog::execIt()
{
	QDialog::exec();
	QMessageBox msgbox("SkypeTab", "Changes will take effect after the next start of Skype™",
					   QMessageBox::Information,0,0,0);
	msgbox.exec();
}

void SettingsDialog::namedCheckboxChanged(int value)
{
	if(value>=1)
		value=1;
	SkypeTab::settings.setValue(sender()->objectName(), QVariant::fromValue(value));
}
}
