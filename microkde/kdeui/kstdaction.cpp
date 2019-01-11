/* This file is part of the KDE libraries
   Copyright (C) 1999,2000 Kurt Granroth <granroth@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kstdaction.h"

#include <qtoolbutton.h>
#include <qwhatsthis.h>

//US #include <kaboutdata.h>
#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
//US #include <kstdaccel.h>
//US #include <kmainwindow.h>

namespace KStdAction
{

struct KStdActionInfo
{
	StdAction id;
/*US	KStdAccel::StdAccel idAccel;*/
	const char* psName;
	const char* psLabel;
	const char* psWhatsThis;
	const char* psIconName;
};

const KStdActionInfo g_rgActionInfo[] =
{
	{ New,           /*USKStdAccel::New,*/ "file_new", I18N_NOOP("&New"), 0, "filenew" },
	{ Open,          /*USKStdAccel::Open,*/ "file_open", I18N_NOOP("&Open..."), 0, "fileopen" },
	{ OpenRecent,    /*USKStdAccel::AccelNone,*/ "file_open_recent", I18N_NOOP("Open &Recent"), 0, 0 },
	{ Save,          /*USKStdAccel::Save,*/ "file_save", I18N_NOOP("&Save"), 0, "filesave" },
	{ SaveAs,        /*USKStdAccel::AccelNone,*/ "file_save_as", I18N_NOOP("Save &As..."), 0, "filesaveas" },
	{ Revert,        /*USKStdAccel::AccelNone,*/ "file_revert", I18N_NOOP("Re&vert"), 0, "revert" },
	{ Close,         /*USKStdAccel::Close,*/ "file_close", I18N_NOOP("&Close"), 0, "fileclose" },
	{ Print,         /*USKStdAccel::Print,*/ "file_print", I18N_NOOP("&Print..."), 0, "fileprint" },
	{ PrintPreview,  /*USKStdAccel::AccelNone,*/ "file_print_preview", I18N_NOOP("Print Previe&w..."), 0, "filequickprint" },
	{ Mail,          /*USKStdAccel::AccelNone,*/ "file_mail", I18N_NOOP("&Mail..."), 0, "mail_send" },
	{ Quit,          /*USKStdAccel::Quit,*/ "file_quit", I18N_NOOP("&Exit"), 0, "exit" },

	{ Undo,          /*USKStdAccel::Undo,*/ "edit_undo", I18N_NOOP("&Undo"), 0, "undo" },
	{ Redo,          /*USKStdAccel::Redo,*/ "edit_redo", I18N_NOOP("Re&do"), 0, "redo" },
	{ Cut,           /*USKStdAccel::Cut,*/ "edit_cut", I18N_NOOP("Cu&t"), 0, "editcut" },
	{ Copy,          /*USKStdAccel::Copy,*/ "edit_copy", I18N_NOOP("&Copy"), 0, "editcopy" },
	{ Paste,         /*USKStdAccel::Paste,*/ "edit_paste", I18N_NOOP("&Paste"), 0, "editpaste" },
	{ SelectAll,     /*USKStdAccel::SelectAll,*/ "edit_select_all", I18N_NOOP("Select &All"), 0, 0 },
	{ Deselect,      /*USKStdAccel::Deselect,*/ "edit_deselect", I18N_NOOP("Dese&lect"), 0, 0 },
	{ Find,          /*USKStdAccel::Find,*/ "edit_find", I18N_NOOP("&Find..."), 0, "find" },
	{ FindNext,      /*USKStdAccel::FindNext,*/ "edit_find_next", I18N_NOOP("Find &Next"), 0, "next" },
	// FIXME: rename edit_find_last to edit_find_prev for KDE 4
	{ FindPrev,      /*USKStdAccel::FindPrev,*/ "edit_find_last", I18N_NOOP("Find Pre&vious"), 0, "previous" },
	{ Replace,       /*USKStdAccel::Replace,*/ "edit_replace", I18N_NOOP("&Replace..."), 0, 0 },

	{ ActualSize,    /*USKStdAccel::AccelNone,*/ "view_actual_size", I18N_NOOP("&Actual Size"), 0, 0 },
	{ FitToPage,     /*USKStdAccel::AccelNone,*/ "view_fit_to_page", I18N_NOOP("&Fit to Page"), 0, 0 },
	{ FitToWidth,    /*USKStdAccel::AccelNone,*/ "view_fit_to_width", I18N_NOOP("Fit to Page &Width"), 0, 0 },
	{ FitToHeight,   /*USKStdAccel::AccelNone,*/ "view_fit_to_height", I18N_NOOP("Fit to Page &Height"), 0, 0 },
	{ ZoomIn,        /*USKStdAccel::ZoomIn,*/ "view_zoom_in", I18N_NOOP("Zoom &In"), 0, "viewmag+" },
	{ ZoomOut,       /*USKStdAccel::ZoomOut,*/ "view_zoom_out", I18N_NOOP("Zoom &Out"), 0, "viewmag-" },
	{ Zoom,          /*USKStdAccel::AccelNone,*/ "view_zoom", I18N_NOOP("&Zoom..."), 0, "viewmag" },
	{ Redisplay,     /*USKStdAccel::AccelNone,*/ "view_redisplay", I18N_NOOP("&Redisplay"), 0, "reload" },

	{ Up,            /*USKStdAccel::Up,*/ "go_up", I18N_NOOP("&Up"), 0, "up" },
	// The following three have special i18n() needs for sLabel
	{ Back,          /*USKStdAccel::Back,*/ "go_back", 0, 0, "back" },
	{ Forward,       /*USKStdAccel::Forward,*/ "go_forward", 0, 0, "forward" },
	{ Home,          /*USKStdAccel::Home,*/ "go_home", 0, 0, "gohome" },
	{ Prior,         /*USKStdAccel::Prior,*/ "go_previous", I18N_NOOP("&Previous Page"), 0, "previous" },
	{ Next,          /*USKStdAccel::Next,*/ "go_next", I18N_NOOP("&Next Page"), 0, "next" },
	{ Goto,          /*USKStdAccel::AccelNone,*/ "go_goto", I18N_NOOP("&Go To..."), 0, 0 },
	{ GotoPage,      /*USKStdAccel::AccelNone,*/ "go_goto_page", I18N_NOOP("&Go to Page..."), 0, "goto" },
	{ GotoLine,      /*USKStdAccel::GotoLine,*/ "go_goto_line", I18N_NOOP("&Go to Line..."), 0, 0 },
	{ FirstPage,     /*USKStdAccel::Home,*/ "go_first", I18N_NOOP("&First Page"), 0, "top" },
	{ LastPage,      /*USKStdAccel::End,*/ "go_last", I18N_NOOP("&Last Page"), 0, "bottom" },

	{ AddBookmark,   /*USKStdAccel::AddBookmark,*/ "bookmark_add", I18N_NOOP("&Add Bookmark"), 0, "bookmark_add" },
	{ EditBookmarks, /*USKStdAccel::AccelNone,*/ "bookmark_edit", I18N_NOOP("&Edit Bookmarks"), 0, "bookmark" },

	{ Spelling,      /*USKStdAccel::AccelNone,*/ "tools_spelling", I18N_NOOP("&Spelling..."), 0, "spellcheck" },

	{ ShowMenubar,   /*USKStdAccel::ShowMenubar,*/ "options_show_menubar", I18N_NOOP("Show &Menubar"), 0, "showmenu" },
	{ ShowToolbar,   /*USKStdAccel::AccelNone,*/ "options_show_toolbar", I18N_NOOP("Show &Toolbar"), 0, 0 },
	{ ShowStatusbar, /*USKStdAccel::AccelNone,*/ "options_show_statusbar", I18N_NOOP("Show St&atusbar"), 0, 0 },
	{ SaveOptions,   /*USKStdAccel::AccelNone,*/ "options_save_options", I18N_NOOP("&Save Settings"), 0, 0 },
	{ KeyBindings,   /*USKStdAccel::AccelNone,*/ "options_configure_keybinding", I18N_NOOP("Configure S&hortcuts..."), 0,"configure_shortcuts" },
	{ Preferences,   /*USKStdAccel::AccelNone,*/ "options_configure", I18N_NOOP("&Configure %1..."), 0, "configure" },
	{ ConfigureToolbars, /*USKStdAccel::AccelNone,*/ "options_configure_toolbars", I18N_NOOP("Configure Tool&bars..."), 0,"configure_toolbars" },
	{ ConfigureNotifications, /*USKStdAccel::AccelNone,*/ "options_configure_notifications", I18N_NOOP("Configure &Notifications..."), 0, "knotify" },

	{ Help,          /*USKStdAccel::Help,*/ "help", 0, 0, "help" },
	{ HelpContents,  /*USKStdAccel::AccelNone,*/ "help_contents", I18N_NOOP("%1 &Handbook"), 0, "contents" },
	{ WhatsThis,     /*USKStdAccel::WhatsThis,*/ "help_whats_this", I18N_NOOP("What's &This?"), 0, "contexthelp" },
	{ TipofDay,      /*USKStdAccel::AccelNone,*/ "help_show_tip", I18N_NOOP("Tip of the &Day"), 0, "idea" },
	{ ReportBug,     /*USKStdAccel::AccelNone,*/ "help_report_bug", I18N_NOOP("&Report Bug..."), 0, 0 },
	{ AboutApp,      /*USKStdAccel::AccelNone,*/ "help_about_app", I18N_NOOP("&About %1"), 0, 0 },
	{ AboutKDE,      /*USKStdAccel::AccelNone,*/ "help_about_kde", I18N_NOOP("About &KDE"), 0,"about_kde" },
	{ ActionNone, /*USKStdAccel::AccelNone,*/ 0, 0, 0, 0 }
};

static const KStdActionInfo* infoPtr( StdAction id )
{
	for( uint i = 0; g_rgActionInfo[i].id != ActionNone; i++ ) {
		if( g_rgActionInfo[i].id == id )
			return &g_rgActionInfo[i];
	}
	return 0;
}

QStringList stdNames()
{
    QStringList result;

    for( uint i = 0; g_rgActionInfo[i].id != ActionNone; i++ )
        if (g_rgActionInfo[i].psLabel)
            result.append(i18n(g_rgActionInfo[i].psLabel));
    return result;
}

KAction* create( StdAction id, const char *name, const QObject *recvr, const char *slot, KActionCollection* parent )
{
	KAction* pAction = 0;
	const KStdActionInfo* pInfo = infoPtr( id );
	kdDebug(125) << "KStdAction::create( " << id << "=" << (pInfo ? pInfo->psName : (const char*)0) << ", " << parent << ", " << name << " )" << endl; // ellis
	if( pInfo ) {
		QString sLabel, iconName = pInfo->psIconName;
		switch( id ) {
		 case Back: sLabel = i18n("go back", "&Back"); 
//US			if (QApplication::reverseLayout() )
//US			    iconName = "forward";
			break; 

		 case Forward: sLabel = i18n("go forward", "&Forward"); 
//US			if (QApplication::reverseLayout() )
//US			    iconName = "back";
			break;  
		
		 case Home: sLabel = i18n("beginning (of line)", "&Home"); break;
		 case Help: sLabel = i18n("show help", "&Help"); break;
//US		 case AboutApp: iconName = kapp->miniIconName();
		 case Preferences:
		 case HelpContents:
			{
//US			const KAboutData *aboutData = KGlobal::instance()->aboutData();
//US			QString appName = (aboutData) ? aboutData->programName() : QString::fromLatin1(qApp->name());
			QString appName = QString::fromLatin1(KGlobal::getAppName());
			sLabel = i18n(pInfo->psLabel).arg(appName);
			}
			break;
		 default: sLabel = i18n(pInfo->psLabel);
		}
		
/*US		if (QApplication::reverseLayout()){
			if (id == Prior) iconName = "next";
			if (id == Next ) iconName = "previous";
		}
*/				
//US		KShortcut cut = KStdAccel::shortcut(pInfo->idAccel);
		KShortcut cut;
		switch( id ) {
		 case OpenRecent:
			pAction = new KRecentFilesAction( sLabel, cut,
					recvr, slot,
					parent, (name) ? name : pInfo->psName );
			break;
		 case ShowMenubar:
		 case ShowToolbar:
		 case ShowStatusbar:
			KToggleAction *ret;
			ret = new KToggleAction( sLabel, pInfo->psIconName, cut,
					recvr, slot,
					parent, (name) ? name : pInfo->psName );
			ret->setChecked( true );
			pAction = ret;
			break;
		 default:
			pAction = new KAction( sLabel, iconName, cut,
					recvr, slot,
					parent, (name) ? name : pInfo->psName );
			break;
		}
	}
	return pAction;
}

const char* name( StdAction id )
{
	const KStdActionInfo* pInfo = infoPtr( id );
	return (pInfo) ? pInfo->psName : 0;
}

KAction *openNew( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( New, name, recvr, slot, parent ); }
KAction *open( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Open, name, recvr, slot, parent ); }
KRecentFilesAction *openRecent( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return (KRecentFilesAction*) KStdAction::create( OpenRecent, name, recvr, slot, parent ); }
KAction *save( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Save, name, recvr, slot, parent ); }
KAction *saveAs( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( SaveAs, name, recvr, slot, parent ); }
KAction *revert( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Revert, name, recvr, slot, parent ); }
KAction *print( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Print, name, recvr, slot, parent ); }
KAction *printPreview( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( PrintPreview, name, recvr, slot, parent ); }
KAction *close( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Close, name, recvr, slot, parent ); }
KAction *mail( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Mail, name, recvr, slot, parent ); }
KAction *quit( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Quit, name, recvr, slot, parent ); }
KAction *undo( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Undo, name, recvr, slot, parent ); }
KAction *redo( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Redo, name, recvr, slot, parent ); }
KAction *cut( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Cut, name, recvr, slot, parent ); }
KAction *copy( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Copy, name, recvr, slot, parent ); }
KAction *paste( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Paste, name, recvr, slot, parent ); }
KAction *selectAll( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( SelectAll, name, recvr, slot, parent ); }
KAction *deselect( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Deselect, name, recvr, slot, parent ); }
KAction *find( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Find, name, recvr, slot, parent ); }
KAction *findNext( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FindNext, name, recvr, slot, parent ); }
KAction *findPrev( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FindPrev, name, recvr, slot, parent ); }
KAction *replace( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Replace, name, recvr, slot, parent ); }
KAction *actualSize( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ActualSize, name, recvr, slot, parent ); }
KAction *fitToPage( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FitToPage, name, recvr, slot, parent ); }
KAction *fitToWidth( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FitToWidth, name, recvr, slot, parent ); }
KAction *fitToHeight( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FitToHeight, name, recvr, slot, parent ); }
KAction *zoomIn( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ZoomIn, name, recvr, slot, parent ); }
KAction *zoomOut( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ZoomOut, name, recvr, slot, parent ); }
KAction *zoom( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Zoom, name, recvr, slot, parent ); }
KAction *redisplay( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Redisplay, name, recvr, slot, parent ); }
KAction *up( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Up, name, recvr, slot, parent ); }
KAction *back( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Back, name, recvr, slot, parent ); }
KAction *forward( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Forward, name, recvr, slot, parent ); }
KAction *home( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Home, name, recvr, slot, parent ); }
KAction *prior( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Prior, name, recvr, slot, parent ); }
KAction *next( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Next, name, recvr, slot, parent ); }
KAction *goTo( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Goto, name, recvr, slot, parent ); }
KAction *gotoPage( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( GotoPage, name, recvr, slot, parent ); }
KAction *gotoLine( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( GotoLine, name, recvr, slot, parent ); }
KAction *firstPage( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( FirstPage, name, recvr, slot, parent ); }
KAction *lastPage( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( LastPage, name, recvr, slot, parent ); }
KAction *addBookmark( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( AddBookmark, name, recvr, slot, parent ); }
KAction *editBookmarks( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( EditBookmarks, name, recvr, slot, parent ); }
KAction *spelling( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Spelling, name, recvr, slot, parent ); }

KToggleAction *showMenubar( const QObject *recvr, const char *slot, KActionCollection* parent, const char *_name )
{
    KToggleAction *ret;
    ret = new KToggleAction(i18n("Show &Menubar"), "showmenu", /*US KStdAccel::shortcut(KStdAccel::ShowMenubar)*/0, recvr, slot,
                            parent, _name ? _name : name(ShowMenubar));
    ret->setChecked(true);
    return ret;
}

KToggleAction *showToolbar( const QObject *recvr, const char *slot, KActionCollection* parent, const char *_name )
{
    KToggleAction *ret;
    ret = new KToggleAction(i18n("Show &Toolbar"), 0, recvr, slot, parent,
                            _name ? _name : name(ShowToolbar));
    ret->setChecked(true);
    return ret;

}

KToggleToolBarAction *showToolbar( const char* toolBarName, KActionCollection* parent, const char *_name )
{
    KToggleToolBarAction *ret;
    ret = new KToggleToolBarAction(toolBarName, i18n("Show &Toolbar"), parent,
                            _name ? _name : name(ShowToolbar));
    return ret;
}

KToggleAction *showStatusbar( const QObject *recvr, const char *slot,
                                         KActionCollection* parent, const char *_name )
{
    KToggleAction *ret;
    ret = new KToggleAction(i18n("Show St&atusbar"), 0, recvr, slot, parent,
                            _name ? _name : name(ShowStatusbar));
    ret->setChecked(true);
    return ret;
}

KAction *saveOptions( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( SaveOptions, name, recvr, slot, parent ); }
KAction *keyBindings( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( KeyBindings, name, recvr, slot, parent ); }
KAction *preferences( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Preferences, name, recvr, slot, parent ); }
KAction *configureToolbars( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ConfigureToolbars, name, recvr, slot, parent ); }
KAction *configureNotifications( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ConfigureNotifications, name, recvr, slot, parent ); }
KAction *help( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( Help, name, recvr, slot, parent ); }
KAction *helpContents( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( HelpContents, name, recvr, slot, parent ); }
KAction *whatsThis( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( WhatsThis, name, recvr, slot, parent ); }
KAction *tipOfDay( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( TipofDay, name, recvr, slot, parent ); }
KAction *reportBug( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( ReportBug, name, recvr, slot, parent ); }
KAction *aboutApp( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( AboutApp, name, recvr, slot, parent ); }
KAction *aboutKDE( const QObject *recvr, const char *slot, KActionCollection* parent, const char *name )
	{ return KStdAction::create( AboutKDE, name, recvr, slot, parent ); }

}
