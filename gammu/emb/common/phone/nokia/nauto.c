/* (c) 2002-2003 by Marcin Wiacek */

#include <string.h>
#include <time.h>

#include "../../gsmcomon.h"
#include "../../gsmstate.h"
#include "nfunc.h"

#if defined(GSM_ENABLE_NOKIA_DCT3) || defined(GSM_ENABLE_NOKIA_DCT4)

static GSM_Reply_Function NAUTOReplyFunctions[] = {
	{DCT3DCT4_ReplyGetModelFirmware,"\xD2",0x02,0x00,ID_GetModel		},
	{DCT3DCT4_ReplyGetModelFirmware,"\xD2",0x02,0x00,ID_GetFirmware		},

	{NULL,				"\x00",0x00,0x00,ID_None			}
};

GSM_Phone_Functions NAUTOPhone = {
	"NAUTO",
	NAUTOReplyFunctions,
	NONEFUNCTION,			/*	Initialise		*/
	NONEFUNCTION,			/*	Terminate 		*/
	GSM_DispatchMessage,
	NOTSUPPORTED,			/* 	ShowStartInfo		*/
	NOKIA_GetManufacturer,
	DCT3DCT4_GetModel,
	DCT3DCT4_GetFirmware,
	NOTSUPPORTED,			/*	GetIMEI			*/
	NOTSUPPORTED,			/*	GetOriginalIMEI		*/
	NOTSUPPORTED,			/*	GetManufactureMonth	*/
	NOTSUPPORTED,			/*	GetProductCode		*/
	NOTSUPPORTED,			/*	GetHardware		*/
	NOTSUPPORTED,			/*	GetPPM			*/
	NOTSUPPORTED,			/*	GetSIMIMSI		*/
	NOTSUPPORTED,			/*	GetDateTime		*/
	NOTSUPPORTED,			/*	SetDateTime		*/
	NOTSUPPORTED,			/*	GetAlarm		*/
	NOTSUPPORTED,			/*	SetAlarm		*/
	NOTSUPPORTED,			/* 	GetLocale		*/
	NOTSUPPORTED,			/* 	SetLocale		*/
	NOTSUPPORTED,			/*	PressKey		*/
	NOTSUPPORTED,			/*	Reset			*/
	NOTSUPPORTED,			/*	ResetPhoneSettings	*/
	NOTSUPPORTED,			/*	EnterSecurityCode	*/
	NOTSUPPORTED,			/*	GetSecurityStatus	*/
	NOTSUPPORTED,			/*	GetDisplayStatus	*/
	NOTSUPPORTED,			/*	SetAutoNetworkLogin	*/
	NOTSUPPORTED,			/*	GetBatteryCharge	*/
	NOTSUPPORTED,			/*	GetSignalQuality	*/
	NOTSUPPORTED,			/*	GetNetworkInfo		*/
	NOTSUPPORTED,     		/*  	GetCategory 		*/
 	NOTSUPPORTED,       		/*  	AddCategory 		*/
        NOTSUPPORTED,      		/*  	GetCategoryStatus 	*/	
	NOTSUPPORTED,			/*	GetMemoryStatus		*/
	NOTSUPPORTED,			/*	GetMemory		*/
	NOTSUPPORTED,			/*	GetNextMemory		*/
	NOTSUPPORTED,			/*	SetMemory		*/
	NOTSUPPORTED,			/*	AddMemory		*/
	NOTSUPPORTED,			/*	DeleteMemory		*/
	NOTIMPLEMENTED,			/*	DeleteAllMemory		*/
	NOTSUPPORTED,			/*	GetSpeedDial		*/
	NOTSUPPORTED,			/*	SetSpeedDial		*/
	NOTSUPPORTED,			/*	GetSMSC			*/
	NOTSUPPORTED,			/*	SetSMSC			*/
	NOTSUPPORTED,			/*	GetSMSStatus		*/
	NOTSUPPORTED,			/*	GetSMS			*/
	NOTSUPPORTED,			/*	GetNextSMS		*/
	NOTSUPPORTED,			/*	SetSMS			*/
	NOTSUPPORTED,			/*	AddSMS			*/
	NOTSUPPORTED,			/* 	DeleteSMS 		*/
	NOTSUPPORTED,			/*	SendSMSMessage		*/
	NOTSUPPORTED,			/*	SendSavedSMS		*/
	NOTSUPPORTED,			/*	SetIncomingSMS		*/
	NOTSUPPORTED,			/* 	SetIncomingCB		*/
	NOTSUPPORTED,			/*	GetSMSFolders		*/
 	NOTSUPPORTED,			/* 	AddSMSFolder		*/
 	NOTSUPPORTED,			/* 	DeleteSMSFolder		*/
	NOTSUPPORTED,			/*	DialVoice		*/
	NOTSUPPORTED,			/*	AnswerCall		*/
	NOTSUPPORTED,			/*	CancelCall		*/
 	NOTSUPPORTED,			/* 	HoldCall 		*/
 	NOTSUPPORTED,			/* 	UnholdCall 		*/
 	NOTSUPPORTED,			/* 	ConferenceCall 		*/
 	NOTSUPPORTED,			/* 	SplitCall		*/
 	NOTSUPPORTED,			/* 	TransferCall		*/
 	NOTSUPPORTED,			/* 	SwitchCall		*/
 	NOTSUPPORTED,			/* 	GetCallDivert		*/
 	NOTSUPPORTED,			/* 	SetCallDivert		*/
 	NOTSUPPORTED,			/* 	CancelAllDiverts	*/
	NOTSUPPORTED,			/*	SetIncomingCall		*/
	NOTSUPPORTED,			/*  	SetIncomingUSSD		*/
	NOTSUPPORTED,			/*	SendDTMF		*/
	NOTSUPPORTED,			/*	GetRingtone		*/
	NOTSUPPORTED,			/*	SetRingtone		*/
	NOTSUPPORTED,			/*	GetRingtonesInfo	*/
	NOTSUPPORTED,			/* 	DeleteUserRingtones	*/
	NOTSUPPORTED,			/*	PlayTone		*/
	NOTSUPPORTED,			/*	GetWAPBookmark		*/
	NOTSUPPORTED,			/* 	SetWAPBookmark 		*/
	NOTSUPPORTED, 			/* 	DeleteWAPBookmark 	*/
	NOTSUPPORTED,			/* 	GetWAPSettings 		*/
	NOTSUPPORTED,			/* 	SetWAPSettings 		*/
	NOTSUPPORTED,			/* 	GetMMSSettings		*/
 	NOTSUPPORTED,			/* 	SetMMSSettings		*/
	NOTSUPPORTED,			/*	GetBitmap		*/
	NOTSUPPORTED,			/*	SetBitmap		*/
	NOTSUPPORTED,			/*	GetToDoStatus		*/
	NOTSUPPORTED,			/*	GetToDo			*/
	NOTSUPPORTED,			/*	GetNextToDo		*/
	NOTSUPPORTED,			/*	SetToDo			*/
	NOTSUPPORTED,			/*	AddToDo			*/
	NOTSUPPORTED,			/*	DeleteToDo		*/
	NOTSUPPORTED,			/*	DeleteAllToDo		*/
	NOTSUPPORTED,			/*	GetCalendarStatus	*/
	NOTSUPPORTED,			/*	GetCalendar		*/
    	NOTSUPPORTED,			/*  	GetNextCalendar		*/
	NOTSUPPORTED,			/*	SetCalendar		*/
	NOTSUPPORTED,			/*	AddCalendar		*/
	NOTSUPPORTED,			/*	DeleteCalendar		*/
	NOTSUPPORTED,			/*	DeleteAllCalendar	*/
	NOTSUPPORTED,			/* 	GetCalendarSettings	*/
	NOTSUPPORTED,			/* 	SetCalendarSettings	*/
	NOTSUPPORTED,			/*	GetNoteStatus		*/
	NOTSUPPORTED,			/*	GetNote			*/
	NOTSUPPORTED,			/*	GetNextNote		*/
	NOTSUPPORTED,			/*	SetNote			*/
	NOTSUPPORTED,			/*	AddNote			*/
	NOTSUPPORTED,			/* 	DeleteNote		*/
	NOTSUPPORTED,			/*	DeleteAllNotes		*/
	NOTSUPPORTED, 			/*	GetProfile		*/
	NOTSUPPORTED, 			/*	SetProfile		*/
    	NOTSUPPORTED,			/*  	GetFMStation        	*/
    	NOTSUPPORTED,			/*  	SetFMStation        	*/
	NOTSUPPORTED,			/* 	ClearFMStations		*/
	NOTSUPPORTED,			/* 	GetNextFileFolder	*/
	NOTSUPPORTED,			/* 	GetFilePart		*/
	NOTSUPPORTED,			/* 	AddFilePart		*/
	NOTSUPPORTED, 			/* 	GetFileSystemStatus	*/
	NOTSUPPORTED,			/* 	DeleteFile		*/
	NOTSUPPORTED,			/* 	AddFolder		*/
	NOTSUPPORTED,			/* 	GetGPRSAccessPoint	*/
	NOTSUPPORTED			/* 	SetGPRSAccessPoint	*/
};

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
