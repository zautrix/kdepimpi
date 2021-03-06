/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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
// $Id: kprefswidget.h,v 1.2 2004/09/22 04:45:20 zautrix Exp $

#ifndef _KPREFSWIDGET_H
#define _KPREFSWIDGET_H

#include <qptrlist.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <qwidget.h>

class KPrefs;

class KColorButton;
class QCheckBox;
class QLabel;
class QSpinBox;
class QButtonGroup;

/**
  @short Base class for widgets used by @ref KPrefsDialog.
  @author Cornelius Schumacher
  @see KPrefsDialog

  This class provides the interface for the preferences widgets used by
  KPrefsDialog.
*/
class KPrefsWid : public QObject
{
  Q_OBJECT
  public:
    /**
      This function is called to read value of the setting from the
      stored configuration and display it in the widget.
    */
    virtual void readConfig() = 0;
    /**
      This function is called to write the current setting of the widget to the
      stored configuration.
    */
    virtual void writeConfig() = 0;

    //connect to this signal if you want to be notified of changes
  signals:
    void modified();

};

/**
  @short Widget for bool settings in @ref KPrefsDialog.
  
  This class provides a widget for configuring bool values. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management.
*/
class KPrefsWidBool : public KPrefsWid
{
  public:
    /**
      Create a bool widget consisting of a QCheckbox.
      
      @param text      Text of QCheckBox.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidBool(const QString &text,bool *reference,QWidget *parent);
    
    /**
      Return the QCheckbox used by this widget.
    */
    QCheckBox *checkBox();
    
    void readConfig();
    void writeConfig();
    
  private:
    bool *mReference;
    
    QCheckBox *mCheck;
};

/**
  @short Widget for time settings in @ref KPrefsDialog.
  
  This class provides a widget for configuring time values. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management.
*/
class KPrefsWidTime : public KPrefsWid
{
  public:
    /**
      Create a time widget consisting of a label and a spinbox.
      
      @param text      Text of Label.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidTime(const QString &text,int *reference,QWidget *parent);
    
    /**
      Return QLabel used by this widget.
    */
    QLabel *label();
    /**
      Return QSpinBox used by this widget.
    */
    QSpinBox *spinBox();
    
    void readConfig();
    void writeConfig();
    
  private:
    int *mReference;
    
    QLabel *mLabel;
    QSpinBox *mSpin;
};

/**
  @short Widget for color settings in @ref KPrefsDialog.
  
  This class provides a widget for configuring color values. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management.
*/
class KPrefsWidColor : public  KPrefsWid
{
  public:
    /**
      Create a color widget consisting of a test field and a button for opening
      a color dialog.
      
      @param text      Text of button.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidColor(const QString &text,QColor *reference,QWidget *parent);
    /**
      Destruct color setting widget.
    */
    ~KPrefsWidColor();
    
    /**
      Return QLabel for the button
    */
    QLabel *label();
    /**
      Return button opening the color dialog.
    */
    KColorButton *button();
    
    void readConfig();
    void writeConfig();
    
  private:
    QColor *mReference;
    
    QLabel *mLabel;
    KColorButton *mButton;
};

/**
  @short Widget for font settings in @ref KPrefsDialog.
  
  This class provides a widget for configuring font values. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management.
*/
class KPrefsWidFont : public KPrefsWid
{
  Q_OBJECT
  public:
    /**
      Create a font widget consisting of a test field and a button for opening
      a font dialog.
      
      @param label     Text of label.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidFont(const QString &sampleText,const QString &labelText,
                  QFont *reference,QWidget *parent);
    /**
      Destruct font setting widget.
    */
    ~KPrefsWidFont();
    
    /**
      Return label.
    */
    QLabel *label();
    /**
      Return QFrame used as preview field.
    */
    QLabel *preview();
    /**
      Return button opening the font dialog.
    */
    QPushButton *button();
    
    void readConfig();
    void writeConfig();

  protected slots:
    void selectFont();
    
  private:
    QFont *mReference;
    
    QLabel *mLabel;
    QLabel *mPreview;
    QPushButton *mButton;
};

/**
  @short Widget for settings represented by a group of radio buttons in
  @ref KPrefsDialog.
  
  This class provides a widget for configuring selections. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management. The
  setting is interpreted as an int value, corresponding to the position of the
  radio button. The position of the button is defined by the sequence of @ref
  addRadio() calls, starting with 0.
*/
class KPrefsWidRadios : public KPrefsWid
{
  public:
    /**
      Create a widget for selection of an option. It consists of a box with
      several radio buttons.
      
      @param text      Text of main box.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidRadios(const QString &text,int *reference,QWidget *parent);
    virtual ~KPrefsWidRadios();

    /**
      Add a radio button.
      
      @param text Text of the button.
    */
    void addRadio(const QString &text);
    
    /**
      Return the box widget used by this widget.
    */
    QButtonGroup *groupBox();
    
    void readConfig();
    void writeConfig();
    
  private:
    int *mReference;

    QButtonGroup *mBox;
};


/**
  @short Widget for string settings in @ref KPrefsDialog.
  
  This class provides a widget for configuring string values. It is meant to be
  used by KPrefsDialog. The user is responsible for the layout management.
*/
class KPrefsWidString : public KPrefsWid
{
  public:
    /**
      Create a string widget consisting of a test label and a line edit.
      
      @param text      Text of label.
      @param reference Pointer to variable read and written by this widget.
      @param parent    Parent widget.
    */
    KPrefsWidString(const QString &text,QString *reference,QWidget *parent,QLineEdit::EchoMode echomode=QLineEdit::Normal);
    /**
      Destructor.
    */
    virtual ~KPrefsWidString();

    /**
      Return label used by this widget.
    */
    QLabel *label();
    /**
      Return QLineEdit used by this widget.
    */
    QLineEdit *lineEdit();
    
    void readConfig();
    void writeConfig();
    
  private:
    QString *mReference;

    QLabel *mLabel;
    QLineEdit *mEdit;
};


/**
  @short Base class for a preferences widget.
  
  This class provides the framework for a preferences widget. You have to
  subclass it and add the code to create the actual configuration widgets and
  do the layout management.
  
  KPrefsWidget provides functions to add subclasses of @ref KPrefsWid. For
  these widgets the reading, writing and setting to default values is handled
  automatically. Custom widgets have to be handled in the functions @ref
  usrReadConfig() and @ref usrWriteConfig().
*/
class KPrefsWidget : public QWidget
{
    Q_OBJECT
  public:
    /**
      Create a KPrefsDialog for a KPrefs object.
      
      @param prefs  KPrefs object used to access te configuration.
      @param parent Parent widget.
      @param name   Widget name.
    */
    KPrefsWidget(KPrefs *prefs,QWidget *parent=0,const char *name=0);
    /**
      Destructor.
    */
    virtual ~KPrefsWidget();

    /**
      Register a custom KPrefsWid object.
    */
    void addWid(KPrefsWid *);
    /**
      Register a @ref KPrefsWidBool object.
      
      @param text      Text of bool widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidBool *addWidBool(const QString &text,bool *reference,QWidget *parent);
    /**
      Register a @ref KPrefsWidTime object.
      
      @param text      Text of time widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidTime *addWidTime(const QString &text,int *reference,QWidget *parent);
    /**
      Register a @ref KPrefsWidColor object.
      
      @param text      Text of color widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidColor *addWidColor(const QString &text,QColor *reference,QWidget *parent);
    /**
      Register a @ref KPrefsWidRadios object.
      
      @param text      Text of radio button box widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidRadios *addWidRadios(const QString &text,int *reference,QWidget *parent);
    /**
      Register a @ref KPrefsWidString object.
      
      @param text      Text of string widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidString *addWidString(const QString &text,QString *reference,QWidget *parent);
    /**
      Register a password @ref KPrefsWidString object, with echomode set to QLineEdit::Password.
      
      @param text      Text of string widget.
      @param reference Reference to variable storing the setting.
      @param parent    Parent widget.
    */
    KPrefsWidString *addWidPassword (const QString &text,QString *reference,QWidget *parent);
    /**
      Register a @ref KPrefsWidFont object.
      
      @param sampleText Sample text of font widget.
      @param buttonText Button text of font widget.
      @param reference  Reference to variable storing the setting.
      @param parent     Parent widget.
    */
    KPrefsWidFont *addWidFont(const QString &sampleText,const QString &buttonText,
                              QFont *reference,QWidget *parent);

  public slots:
    /** Set all widgets to default values. */
    void setDefaults();
  
    /** Read preferences from config file. */
    void readConfig();

    /** Write preferences to config file. */
    void writeConfig();

    /** connect this slot to all UI elements */
    void modified();

  signals:
    /** Emitted when the a changed configuration has been stored. */
    //US    void configChanged();
    void changed( bool );
    
  protected slots:
    /** Apply changes to preferences */
    //US    void slotApply();  

    //US    void accept();
    /** Accept changes to preferences and close dialog */
    //US    void slotOk();
  
    /** Set preferences to default values */
    //US    void slotDefault();

  protected:
    /** Implement this to read custom configuration widgets. */
    virtual void usrReadConfig() {}
    /** Implement this to write custom configuration widgets. */
    virtual void usrWriteConfig() {}

  private:
    KPrefs *mPrefs;

    QPtrList<KPrefsWid> mPrefsWids;
};

#endif
