/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>                   
                                                                        
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or   
    (at your option) any later version.                                 
                                                                        
    This program is distributed in the hope that it will be useful,     
    but WITHOUT ANY WARRANTY; without even the implied warranty of      
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        
    GNU General Public License for more details.                        
                                                                        
    You should have received a copy of the GNU General Public License   
    along with this program; if not, write to the Free Software         
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      

#include <qstring.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

#include <kglobal.h>
#include <klocale.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kiconloader.h>
#include <kfontdialog.h>

#ifndef KAB_EMBEDDED    
#include <kimageio.h>
#else //KAB_EMBEDDED    
#endif //KAB_EMBEDDED    

#include <kconfig.h>

#include "colorlistbox.h"

#include "configuretableviewdialog.h"

ConfigureTableViewWidget::ConfigureTableViewWidget( KABC::AddressBook *ab,
                                                    QWidget *parent, 
                                                    const char *name )
  : ViewConfigureWidget( ab, parent, name )
{
  QWidget *page = addPage( i18n( "Look & Feel" ), QString::null,
                           KGlobal::iconLoader()->loadIcon( "looknfeel",
                           KIcon::Panel ) );
  
  mPage = new LookAndFeelPage( page );
}
                                           
ConfigureTableViewWidget::~ConfigureTableViewWidget()
{
}

void ConfigureTableViewWidget::restoreSettings( KConfig *config )
{
  ViewConfigureWidget::restoreSettings( config );

  mPage->restoreSettings( config );
}

void ConfigureTableViewWidget::saveSettings( KConfig *config )
{
  ViewConfigureWidget::saveSettings( config );

  mPage->saveSettings( config );
}



LookAndFeelPage::LookAndFeelPage(QWidget *parent, const char *name)
  : QVBox(parent, name)
{
  initGUI();
  
  // Set initial state
  enableBackgroundToggled(mBackgroundBox->isChecked());
}
    
void LookAndFeelPage::restoreSettings( KConfig *config )
{
  mAlternateButton->setChecked(config->readBoolEntry("ABackground", true));
  mLineButton->setChecked(config->readBoolEntry("SingleLine", false));
  mToolTipBox->setChecked(config->readBoolEntry("ToolTips", true));
      
  if (!mAlternateButton->isChecked() & !mLineButton->isChecked())
    mNoneButton->setChecked(true);
  
  mBackgroundBox->setChecked(config->readBoolEntry("Background", false));
  mBackgroundName->lineEdit()->setText(config->readEntry("BackgroundName"));

  // colors
  cbEnableCustomColors->setChecked( config->readBoolEntry( "EnableCustomColors", false ) );
  QColor c;
  //qDebug("LookAndFeelPage::restoreSettings make base color configurable");  
  
#ifndef KAB_EMBEDDED  
  c = KGlobalSettings::baseColor();
#else //KAB_EMBEDDED  
  c = QColor(0,0,0);
#endif //KAB_EMBEDDED  
  
  c = colorGroup().background();
  lbColors->insertItem( new ColorListItem( i18n("Background Color"),
        config->readColorEntry( "BackgroundColor", &c ) ) );
  c = colorGroup().foreground();
  lbColors->insertItem( new ColorListItem( i18n("Text Color"),
        config->readColorEntry( "TextColor", &c ) ) );
  c = colorGroup().button();
  lbColors->insertItem( new ColorListItem( i18n("Header Background Color"),
        config->readColorEntry( "HeaderBackgroundColor", &c ) ) );
  c = colorGroup().buttonText();
  lbColors->insertItem( new ColorListItem( i18n("Header Text Color"),
        config->readColorEntry( "HeaderTextColor", &c ) ) );
  c = colorGroup().highlight();
  lbColors->insertItem( new ColorListItem( i18n("Highlight Color"),
        config->readColorEntry( "HighlightColor", &c ) ) );
  c = colorGroup().highlightedText();
  lbColors->insertItem( new ColorListItem( i18n("Highlighted Text Color"),
        config->readColorEntry( "HighlightedTextColor", &c ) ) );  
  c = colorGroup().background();
  lbColors->insertItem( new ColorListItem( i18n("Alternating Background Color"),
        config->readColorEntry( "AlternatingBackgroundColor", &c ) ) );
  
  enableColors();
  
  // fonts
  QFont fnt = font();
  updateFontLabel( config->readFontEntry( "TextFont", &fnt ), (QLabel*)lTextFont );
  fnt.setBold( true );
  updateFontLabel( config->readFontEntry( "HeaderFont",  &fnt ), (QLabel*)lHeaderFont );
  cbEnableCustomFonts->setChecked( config->readBoolEntry( "EnableCustomFonts", false ) );
  enableFonts();

}
    
void LookAndFeelPage::saveSettings( KConfig *config )
{
  config->writeEntry("ABackground", mAlternateButton->isChecked());
  config->writeEntry("SingleLine", mLineButton->isChecked());
  config->writeEntry("ToolTips", mToolTipBox->isChecked());
  config->writeEntry("Background", mBackgroundBox->isChecked());
  config->writeEntry("BackgroundName", mBackgroundName->lineEdit()->text());

  // colors
  config->writeEntry( "EnableCustomColors", cbEnableCustomColors->isChecked() );
  if ( cbEnableCustomColors->isChecked() ) // ?? - Hmmm.
  {
    config->writeEntry( "BackgroundColor", lbColors->color( 0 ) );
    config->writeEntry( "TextColor", lbColors->color( 1 ) );
    config->writeEntry( "HeaderBackgroundColor", lbColors->color( 2 ) );
    config->writeEntry( "HeaderTextColor", lbColors->color( 3 ) );
    config->writeEntry( "HighlightColor", lbColors->color( 4 ) );
    config->writeEntry( "HighlightedTextColor", lbColors->color( 5 ) );
    config->writeEntry( "AlternatingBackgroundColor", lbColors->color( 6 ) );
  }
  // fonts
  config->writeEntry( "EnableCustomFonts", cbEnableCustomFonts->isChecked() );
  if ( cbEnableCustomFonts->isChecked() )
  {
    config->writeEntry( "TextFont", lTextFont->font() );
    config->writeEntry( "HeaderFont", lHeaderFont->font() );
  }

}

void LookAndFeelPage::setTextFont()
{
  QFont f( lTextFont->font() );
#ifndef KAB_EMBEDDED  
  if ( KFontDialog::getFont( f, false, this ) == QDialog::Accepted )
    updateFontLabel( f, lTextFont );
#else //KAB_EMBEDDED  
  bool ok;
  QFont fout = KFontDialog::getFont( f, ok);
  if ( ok )
    updateFontLabel( fout, lTextFont );
#endif //KAB_EMBEDDED  
}

void LookAndFeelPage::setHeaderFont()
{
  QFont f( lHeaderFont->font() );
#ifndef KAB_EMBEDDED  
  if ( KFontDialog::getFont( f,false, this ) == QDialog::Accepted )
    updateFontLabel( f, lHeaderFont );
#else //KAB_EMBEDDED  
  bool ok;
  QFont fout = KFontDialog::getFont( f, ok);
  if ( ok )
    updateFontLabel( fout, lHeaderFont );
#endif //KAB_EMBEDDED  
}

void LookAndFeelPage::enableFonts()
{
  vbFonts->setEnabled( cbEnableCustomFonts->isChecked() );
  if ( cbEnableCustomFonts->isChecked() )
      vbFonts->setFocus();
}

void LookAndFeelPage::enableColors()
{
    lbColors->setEnabled( cbEnableCustomColors->isChecked() );
    if ( cbEnableCustomColors->isChecked() )
        lbColors->setFocus();
}
    
void LookAndFeelPage::initGUI()
{
  int spacing = KDialog::spacingHint();
  int margin = KDialog::marginHint();
  
  QTabWidget *tabs = new QTabWidget( this );

  // General
  QVBox *generalTab = new QVBox( this, "generaltab" );
  
  generalTab->setSpacing( spacing );
  generalTab->setMargin( margin );

  QButtonGroup *group = new QButtonGroup(1, Qt::Horizontal, 
                                         i18n("Row Separator"), generalTab);
      
  mAlternateButton = new QRadioButton(i18n("Alternating backgrounds"),
                                      group, "mAlternateButton");
  mLineButton = new QRadioButton(i18n("Single line"), group, "mLineButton");
  mNoneButton = new QRadioButton(i18n("None"), group, "mNoneButton");
      
  mBackgroundBox = new QCheckBox(i18n("Enable background image:"), generalTab,
                                 "mBackgroundBox");
  connect(mBackgroundBox, SIGNAL(toggled(bool)),
          SLOT(enableBackgroundToggled(bool)));
  // LR image not implemented
  mBackgroundBox->setEnabled( false );
  mBackgroundName = new KURLRequester(generalTab, "mBackgroundName");
#ifndef KAB_EMBEDDED    
  mBackgroundName->setMode(KFile::File | KFile::ExistingOnly |
                           KFile::LocalOnly);
  mBackgroundName->setFilter(KImageIO::pattern(KImageIO::Reading));
#endif //KAB_EMBEDDED    
  
  // ToolTip Checkbox
  mToolTipBox = new QCheckBox(i18n("Enable contact tooltips"), generalTab,
                              "mToolTipBox");

  tabs->addTab( generalTab, i18n("&General") );
    
  // Colors
  QVBox *colorTab = new QVBox( this, "colortab" );
  colorTab->setSpacing( spacing );
  colorTab->setMargin( spacing );
  cbEnableCustomColors = new QCheckBox( i18n("&Enable custom Colors"), colorTab );
  connect( cbEnableCustomColors, SIGNAL(clicked()), this, SLOT(enableColors()) );
  lbColors = new ColorListBox( colorTab );
  tabs->addTab( colorTab, i18n("&Colors") );
  
  QWhatsThis::add( cbEnableCustomColors, i18n(
        "If custom colors are enabled, you may choose the colors for the view below. "
        "Otherwise colors from your current KDE color scheme are used."
        ) );
  QWhatsThis::add( lbColors, i18n(
        "Double click or press RETURN on a item to select a color for the related strings in the view."
        ) );
  
  // Fonts
  QVBox *fntTab = new QVBox( this, "fonttab" );
  
  fntTab->setSpacing( spacing );
  fntTab->setMargin( spacing );
  
  cbEnableCustomFonts = new QCheckBox( i18n("&Enable custom fonts"), fntTab );
  connect( cbEnableCustomFonts, SIGNAL(clicked()), this, SLOT(enableFonts()) );
  
  vbFonts = new QWidget( fntTab );
  QGridLayout *gFnts = new QGridLayout( vbFonts, 2, 3 );
  gFnts->setSpacing( spacing );
  gFnts->setAutoAdd( true );
  gFnts->setColStretch( 1, 1 );  
  QLabel *lTFnt = new QLabel( i18n("&Text font:"), vbFonts );
  lTextFont = new QLabel( vbFonts );
  lTextFont->setFrameStyle( QFrame::Panel|QFrame::Sunken );
#ifndef KAB_EMBEDDED    
  btnFont = new KPushButton( i18n("Choose..."), vbFonts );
#else //KAB_EMBEDDED    
  btnFont = new QPushButton( i18n("Choose..."), vbFonts );
#endif //KAB_EMBEDDED    
  
  lTFnt->setBuddy( btnFont );
  
  connect( btnFont, SIGNAL(clicked()), this, SLOT(setTextFont()) );
  
  QLabel *lHFnt = new QLabel( i18n("&Header font:"), vbFonts );
  lHeaderFont = new QLabel( vbFonts );
  lHeaderFont->setFrameStyle( QFrame::Panel|QFrame::Sunken );
#ifndef KAB_EMBEDDED    
  btnHeaderFont = new KPushButton( i18n("Choose..."), vbFonts );
#else //KAB_EMBEDDED    
  btnHeaderFont = new QPushButton( i18n("Choose..."), vbFonts );
#endif //KAB_EMBEDDED    
  lHFnt->setBuddy( btnHeaderFont );
  connect( btnHeaderFont, SIGNAL(clicked()), this, SLOT(setHeaderFont()) );
  
  fntTab->setStretchFactor( new QWidget( fntTab ), 1 );

  QWhatsThis::add( cbEnableCustomFonts, i18n(
        "If custom fonts are enabled, you may choose which fonts to use for this view below. "
        "Otherwise the default KDE font will be used, in bold style for the header and "
        "normal style for the data."
        ) );
          
  tabs->addTab( fntTab, i18n("&Fonts") );

}

void LookAndFeelPage::enableBackgroundToggled(bool enabled)
{
  mBackgroundName->setEnabled(enabled);
}


void LookAndFeelPage::updateFontLabel( QFont fnt, QLabel *l )
{
  l->setFont( fnt );
  l->setText(  QString( fnt.family() + " %1" ).arg( fnt.pointSize() ) );
}

#ifndef KAB_EMBEDDED    
#include "configuretableviewdialog.moc"
#endif //KAB_EMBEDDED    
