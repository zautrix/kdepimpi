#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include <kconfig.h>
#include <kprefs.h>
#include <klocale.h>
#include <kglobal.h>
#include <kfontdialog.h>

#include "settingsdialog.h"


SettingsDialog::SettingsDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : SettingsDialogUI( parent, name, modal, fl ) {

#if 0
    QTabWidget *topFrame = TabWidget2;

    QGridLayout *topLayout = new QGridLayout(topFrame,3,3);
    topLayout->setSpacing(2);
    topLayout->setMargin(3);
    KPrefsWidFont * tVFont;
    int i = 0;
    KPrefsWidFont *timeLabelsFont =
        addWidFont(i18n("OK"),i18n("Application(nr):"),
                   &(mAppFont),topFrame);
    topLayout->addWidget(timeLabelsFont->label(),i,0);
    topLayout->addWidget(timeLabelsFont->preview(),i,1);
    topLayout->addWidget(timeLabelsFont->button(),i,2);
    ++i;


    timeLabelsFont =
        addWidFont(i18n("Mon 15"),i18n("Compose Mail:"),
                   &(mComposeFont),topFrame);
    topLayout->addWidget(timeLabelsFont->label(),i,0);
    topLayout->addWidget(timeLabelsFont->preview(),i,1);
    topLayout->addWidget(timeLabelsFont->button(),i,2);
    ++i;

    KPrefsWidFont *timeBarFont =
        addWidFont(i18n("Mon 15"),i18n("Read Mail:"),
                 &(mReadFont),topFrame);
    topLayout->addWidget(timeBarFont->label(),i,0);
    topLayout->addWidget(timeBarFont->preview(),i,1);
    topLayout->addWidget(timeBarFont->button(),i,2);
    ++i;

    readConfig();
#endif
}

SettingsDialog::~SettingsDialog() {

}

void SettingsDialog::readConfig() {
    KConfig cfg = ( locateLocal("config","kopiemailrc" ));
    cfg.setGroup( "Settings" );
    showHtmlButton->setChecked( cfg.readBoolEntry( "showHtml", false ) );
    cfg.setGroup( "Compose" );
    checkBoxLater->setChecked( cfg.readBoolEntry( "sendLater", false ) );
    cfg.setGroup( "Applet" );
    cbEnableTaskbarApplet->setChecked( cfg.readBoolEntry( "Disabled", false ) );
    spCheckOften->setValue( cfg.readNumEntry( "CheckEvery", 5 ) );
    cbBlinkLed->setChecked( cfg.readBoolEntry( "BlinkLed", true ) );
    cbPlaySound->setChecked( cfg.readBoolEntry( "PlaySound", false ) );

}

void SettingsDialog::writeConfig() {
    KConfig cfg ( locateLocal("config","kopiemailrc" ));
    cfg.setGroup( "Settings" );
    cfg.writeEntry( "showHtml", showHtmlButton->isChecked() );
    cfg.setGroup( "Compose" );
    cfg.writeEntry( "sendLater", checkBoxLater->isChecked() );
    cfg.setGroup( "Applet" );
    cfg.writeEntry( "Disabled", cbEnableTaskbarApplet->isChecked() );
    cfg.writeEntry( "CheckEvery", spCheckOften->value() );
    cfg.writeEntry( "BlinkLed", cbBlinkLed->isChecked() );
    cfg.writeEntry( "PlaySound", cbPlaySound->isChecked() );
}

void SettingsDialog::accept() {
    writeConfig();
    QDialog::accept();
}
