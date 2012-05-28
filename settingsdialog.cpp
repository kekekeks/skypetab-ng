#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "skypetab.h"
#include <QCheckBox>
#include <QMessageBox>
#include <QFile>
#include <stdio.h>
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
	group=findChild<QWidget*>("miscOptions");
	layout=new QBoxLayout(QBoxLayout::TopToBottom);
	group->setLayout(layout);
	addSettingsCheckbox(layout, "Don't close tabs with the window", "tabs/noClose", false);
	addSettingsCheckbox(layout, "Always show additional event counter", "tabCounter/alwaysShow", false);
	if(SkypeTab::winManager==Unity)
	{
		addSettingsCheckbox(layout, "Do not wiggle constantly in Unity launcher", "unity/noConstantUrgency", false);
		addSettingsCheckbox(layout, "Minimize on close", "unity/noClose", true, "tabCounter/alwaysShow");
	}
	group=findChild<QWidget*>("startOptions");
	layout=new QBoxLayout(QBoxLayout::TopToBottom);
	group->setLayout(layout);
	addSettingsCheckbox(layout, "Don't show window at start", "startup/hidden", false);
	addSettingsCheckbox(layout, "Activate previous instance", "startup/activate", true);

	autostartPath=QString(getenv("HOME"));
	autostartPath=autostartPath.append("/.config/autostart/skypetab.desktop");
	QCheckBox*cb=new QCheckBox("Start automatically");
	cb->setChecked(QFile::exists(autostartPath));
	cb->setObjectName("autostart");
	layout->addWidget(cb);

	this->resize(width(), minimumHeight());
	foreach(cb, this->findChildren<QCheckBox*>())
	{
		checkBoxChanged(cb, cb->isChecked()?1:0);
	}

}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::addSettingsCheckbox(QBoxLayout *layout, QString title, QString keyName, bool isDefault, QList<QString> &disables)
{
	bool isChecked=(
			((!SkypeTab::settings.contains(keyName))&&isDefault) //If no setting for this entry, use default flag
			||
			(SkypeTab::settings.value(keyName).toInt()==1)
	);
	QCheckBox* cb=new QCheckBox(title);
	cb->setChecked(isChecked);
	cb->setObjectName(keyName);
	QList<QVariant> vlist;
	QString dstr;
	foreach (dstr, disables)
	{
		vlist.append(QVariant::fromValue(dstr));
	}

	cb->setProperty("disables", QVariant::fromValue(vlist));
	layout->addWidget(cb);
	connect(cb, SIGNAL(stateChanged(int)), this, SLOT(namedCheckboxChanged(int)));
	cb->show();
}

void SettingsDialog::addSettingsCheckbox(QBoxLayout *layout, QString title, QString keyName, bool isDefault)
{
	QList<QString> list;
	addSettingsCheckbox(layout, title, keyName, isDefault, list);
}

void SettingsDialog::addSettingsCheckbox(QBoxLayout *layout, QString title, QString keyName, bool isDefault, QString disables)
{
	QList<QString> list;
	list.append(disables);
	addSettingsCheckbox(layout, title, keyName, isDefault, list);
}



void SettingsDialog::execIt()
{
	QDialog::exec();
	QCheckBox*cb=findChild<QCheckBox*>("autostart");
	if(cb->isChecked())
	{
		FILE*file=fopen(autostartPath.toLocal8Bit().data(), "w");
		fprintf(file,"[Desktop Entry]\nEncoding=UTF-8\nName=SkypeTab\nComment=A program that adds tabs to Skype\nIcon=skype\nType=Application\nExec=skypetab-ng\nStartupNotify=False\nTerminal=false");
		fclose(file);
	}
	else
	{
		QFile::remove(autostartPath);
	}

	QMessageBox msgbox("SkypeTab", "Some changes will take effect after the next start of Skype™",
					   QMessageBox::Information,0,0,0);
	msgbox.exec();
}

void SettingsDialog::namedCheckboxChanged(int value)
{
	checkBoxChanged((QCheckBox*)sender(), value);
}

void SettingsDialog::checkBoxChanged(QCheckBox *ccb, int value)
{
	if(value>=1)
		value=1;
	QList<QVariant> vdisables=ccb->property ("disables").value<QList<QVariant>>();
	QVariant vdis;
	foreach(vdis, vdisables)
	{
		QCheckBox* cb=findChild<QCheckBox*>(vdis.toString());
		cb->setEnabled(value==0);
	}

	SkypeTab::settings.setValue(ccb->objectName(), QVariant::fromValue(value));
}
}
