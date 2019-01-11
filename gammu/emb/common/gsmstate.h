/* (c) 2002-2004 by Marcin Wiacek & Michal Cihar */

#ifndef __gsm_state_h
#define __gsm_state_h

#include <time.h>

#include "config.h"
#include "misc/cfg.h"

typedef struct _GSM_StateMachine 	GSM_StateMachine;
typedef struct _GSM_User	 	GSM_User;
typedef struct _OnePhoneModel	 	OnePhoneModel;
typedef struct _GSM_Reply_Function	GSM_Reply_Function;

#ifdef GSM_ENABLE_NOKIA3320
#  include "phone/nokia/dct4/n3320.h"
#endif
#ifdef GSM_ENABLE_NOKIA3650
#  include "phone/nokia/dct4/n3650.h"
#endif
#ifdef GSM_ENABLE_NOKIA650
#  include "phone/nokia/dct3/n0650.h"
#endif
#ifdef GSM_ENABLE_NOKIA6110
#  include "phone/nokia/dct3/n6110.h"
#endif
#ifdef GSM_ENABLE_NOKIA6510
#  include "phone/nokia/dct4/n6510.h"
#endif
#ifdef GSM_ENABLE_NOKIA7110
#  include "phone/nokia/dct3/n7110.h"
#endif
#ifdef GSM_ENABLE_NOKIA9210
#  include "phone/nokia/dct3/n9210.h"
#endif
#ifdef GSM_ENABLE_ATGEN
#  include "phone/at/atgen.h"
#endif
#ifdef GSM_ENABLE_ALCATEL
#  include "phone/alcatel/alcatel.h"
#endif
#ifdef GSM_ENABLE_OBEXGEN
#  include "phone/obex/obexgen.h"
#endif
#ifdef GSM_ENABLE_MROUTERGEN
#  include "phone/symbian/mroutgen.h"
#endif

#ifndef GSM_USED_MBUS2
#  undef GSM_ENABLE_MBUS2
#endif
#ifndef GSM_USED_FBUS2
#  undef GSM_ENABLE_FBUS2
#endif
#ifndef GSM_USED_FBUS2DLR3
#  undef GSM_ENABLE_FBUS2DLR3
#endif
#ifndef GSM_USED_FBUS2DKU5
#  undef GSM_ENABLE_FBUS2DKU5
#endif
#ifndef GSM_USED_FBUS2PL2303
#  undef GSM_ENABLE_FBUS2PL2303
#endif
#ifndef GSM_USED_FBUS2BLUE
#  undef GSM_ENABLE_FBUS2BLUE
#endif
#ifndef GSM_USED_FBUS2IRDA
#  undef GSM_ENABLE_FBUS2IRDA
#endif
#ifndef GSM_USED_PHONETBLUE
#  undef GSM_ENABLE_PHONETBLUE
#endif
#ifndef GSM_USED_AT
#  undef GSM_ENABLE_AT
#endif
#ifndef GSM_USED_IRDAOBEX
#  undef GSM_ENABLE_IRDAOBEX
#endif
#ifndef GSM_USED_BLUEOBEX
#  undef GSM_ENABLE_BLUEOBEX
#endif
#ifndef GSM_USED_ALCABUS
#  undef GSM_ENABLE_ALCABUS
#endif
#ifndef GSM_USED_IRDAPHONET
#  undef GSM_ENABLE_IRDAPHONET
#endif
#ifndef GSM_USED_BLUEFBUS2
#  undef GSM_ENABLE_BLUEFBUS2
#endif
#ifndef GSM_USED_BLUEPHONET
#  undef GSM_ENABLE_BLUEPHONET
#endif
#ifndef GSM_USED_BLUEAT
#  undef GSM_ENABLE_BLUEAT
#endif
#ifndef GSM_USED_IRDAAT
#  undef GSM_ENABLE_IRDAAT
#endif
#ifndef GSM_USED_MROUTERBLUE
#  undef GSM_ENABLE_MROUTERBLUE
#endif

#if defined(GSM_ENABLE_NOKIA3320) || defined(GSM_ENABLE_NOKIA650) || defined(GSM_ENABLE_NOKIA6110) || defined(GSM_ENABLE_NOKIA7110) || defined(GSM_ENABLE_NOKIA9210)
#  define GSM_ENABLE_NOKIA_DCT3
#endif
#if defined(GSM_ENABLE_NOKIA3650) || defined(GSM_ENABLE_NOKIA6510)
#  define GSM_ENABLE_NOKIA_DCT4
#endif

#include "protocol/protocol.h"
#if defined(GSM_ENABLE_FBUS2) || defined(GSM_ENABLE_FBUS2IRDA) || defined(GSM_ENABLE_FBUS2DLR3) || defined(GSM_ENABLE_FBUS2BLUE) || defined(GSM_ENABLE_BLUEFBUS2) || defined(GSM_ENABLE_FBUS2DKU5) || defined(GSM_ENABLE_FBUS2PL2303)
#  include "protocol/nokia/fbus2.h"
#endif
#ifdef GSM_ENABLE_MBUS2
#  include "protocol/nokia/mbus2.h"
#endif
#if defined(GSM_ENABLE_PHONETBLUE) || defined(GSM_ENABLE_IRDAPHONET) || defined(GSM_ENABLE_BLUEPHONET)
#  include "protocol/nokia/phonet.h"
#endif
#if defined(GSM_ENABLE_AT) || defined(GSM_ENABLE_BLUEAT) || defined(GSM_ENABLE_IRDAAT)
#  include "protocol/at/at.h"
#endif
#ifdef GSM_ENABLE_ALCABUS
#  include "protocol/alcatel/alcabus.h"
#endif
#if defined(GSM_ENABLE_IRDAOBEX) || defined(GSM_ENABLE_BLUEOBEX)
#  include "protocol/obex/obex.h"
#endif
#if defined(GSM_ENABLE_MROUTERBLUE)
#  include "protocol/symbian/mrouter.h"
#endif

#define GSM_ENABLE_SERIALDEVICE
#ifndef GSM_USED_SERIALDEVICE
#  undef GSM_ENABLE_SERIALDEVICE
#endif
#define GSM_ENABLE_IRDADEVICE
#ifndef GSM_USED_IRDADEVICE
#  undef GSM_ENABLE_IRDADEVICE
#endif
#define GSM_ENABLE_BLUETOOTHDEVICE
#ifndef GSM_USED_BLUETOOTHDEVICE
#  undef GSM_ENABLE_BLUETOOTHDEVICE
#endif

#ifdef DJGPP
#  undef GSM_ENABLE_IRDADEVICE
#  undef GSM_ENABLE_IRDAPHONET
#  undef GSM_ENABLE_IRDAOBEX
#  undef GSM_ENABLE_IRDAAT
#  undef GSM_ENABLE_FBUS2IRDA

#  undef GSM_ENABLE_BLUETOOTHDEVICE
#  undef GSM_ENABLE_BLUEPHONET
#  undef GSM_ENABLE_BLUEOBEX
#  undef GSM_ENABLE_BLUEAT
#  undef GSM_ENABLE_BLUEFBUS2
#  undef GSM_ENABLE_PHONETBLUE
#  undef GSM_ENABLE_FBUS2BLUE
#  undef GSM_ENABLE_MROUTERBLUE
#endif

#ifndef WIN32
#  ifdef ENABLE_LGPL
#    undef GSM_ENABLE_IRDADEVICE
#    undef GSM_ENABLE_IRDAPHONET
#    undef GSM_ENABLE_IRDAOBEX
#    undef GSM_ENABLE_IRDAAT
#    undef GSM_ENABLE_FBUS2IRDA

#    undef GSM_ENABLE_BLUETOOTHDEVICE
#    undef GSM_ENABLE_BLUEPHONET
#    undef GSM_ENABLE_BLUEOBEX
#    undef GSM_ENABLE_BLUEAT
#    undef GSM_ENABLE_BLUEFBUS2
#    undef GSM_ENABLE_PHONETBLUE
#    undef GSM_ENABLE_FBUS2BLUE
#    undef GSM_ENABLE_MROUTERBLUE
#  endif
#endif

#ifdef GSM_ENABLE_SERIALDEVICE
#  include "device/serial/ser_w32.h"
#  include "device/serial/ser_unx.h"
#  include "device/serial/ser_djg.h"
#endif
#ifdef GSM_ENABLE_IRDADEVICE
#  include "device/irda/irda.h"
#endif
#ifdef GSM_ENABLE_BLUETOOTHDEVICE
#  include "device/bluetoth/bluetoth.h"
#endif

#include "service/gsmpbk.h"
#include "service/gsmnet.h"
#include "service/gsmring.h"
#include "service/gsmcal.h"
#include "service/gsmdata.h"
#include "service/gsmlogo.h"
#include "service/gsmmisc.h"
#include "service/gsmprof.h"
#include "service/gsmcall.h"
#include "service/sms/gsmsms.h"
#include "service/sms/gsmems.h"
#include "service/sms/gsmmulti.h"
#include "service/backup/gsmback.h"


/* ------------------------- Device layer ---------------------------------- */

/**
 * Device functions, each device has to provide these.
 */
typedef struct {
	/**
	 * Opens device.
	 */
	GSM_Error (*OpenDevice)        (GSM_StateMachine *s);
	/**
	 * Closes device.
	 */
	GSM_Error (*CloseDevice)       (GSM_StateMachine *s);
	/**
	 * Sets parity for device.
	 */
	GSM_Error (*DeviceSetParity)   (GSM_StateMachine *s, bool parity);
	/**
	 * Sets dtr (data to read) and rts (ready to send) flags.
	 */
	GSM_Error (*DeviceSetDtrRts)   (GSM_StateMachine *s, bool dtr, bool rts);
	/**
	 * Sets device speed.
	 */
	GSM_Error (*DeviceSetSpeed)    (GSM_StateMachine *s, int speed);
	/**
	 * Attempts to read nbytes from device.
	 */
	int       (*ReadDevice)        (GSM_StateMachine *s, void *buf, size_t nbytes);
	/**
	 * Attempts to read nbytes from device.
	 */
	int       (*WriteDevice)       (GSM_StateMachine *s, void *buf, size_t nbytes);
} GSM_Device_Functions;

#ifdef GSM_ENABLE_SERIALDEVICE
	extern GSM_Device_Functions SerialDevice;
#endif
#ifdef GSM_ENABLE_IRDADEVICE
	extern GSM_Device_Functions IrdaDevice;
#endif
#ifdef GSM_ENABLE_BLUETOOTHDEVICE
	extern GSM_Device_Functions BlueToothDevice;
#endif

/**
 * Structure containing device specific data and pointer to device functions -
 * @ref GSM_Device_Functions. The data are in a union, so you can use only 
 * one device at one time.
 */
typedef struct {
	union {
		char fake;
#ifdef GSM_ENABLE_SERIALDEVICE
		GSM_Device_SerialData		Serial;
#endif
#ifdef GSM_ENABLE_IRDADEVICE
		GSM_Device_IrdaData		Irda;
#endif
#ifdef GSM_ENABLE_BLUETOOTHDEVICE
		GSM_Device_BlueToothData	BlueTooth;
#endif
	} Data;
	GSM_Device_Functions *Functions;
} GSM_Device;

/* ---------------------- Protocol layer ----------------------------------- */

/**
 * Protocol functions, each protocol has to implement these.
 */
typedef struct {
	/**
	 * Writes message to device.
	 */
	GSM_Error (*WriteMessage) (GSM_StateMachine *s, unsigned char *buffer,
				   int length, unsigned char type);
	/**
	 * This one is called when character is received from device.
	 */
	GSM_Error (*StateMachine) (GSM_StateMachine *s, unsigned char rx_char);
	/**
	 * Protocol initialisation.
	 */
	GSM_Error (*Initialise)   (GSM_StateMachine *s);
	/**
	 * Protocol termination.
	 */
	GSM_Error (*Terminate)    (GSM_StateMachine *s);
} GSM_Protocol_Functions;

#ifdef GSM_ENABLE_MBUS2
	extern GSM_Protocol_Functions MBUS2Protocol;
#endif
#if defined(GSM_ENABLE_FBUS2) || defined(GSM_ENABLE_FBUS2IRDA) || defined(GSM_ENABLE_FBUS2DLR3) || defined(GSM_ENABLE_FBUS2DKU5) || defined(GSM_ENABLE_FBUS2BLUE) || defined(GSM_ENABLE_BLUEFBUS2) || defined(GSM_ENABLE_FBUS2PL2303)
	extern GSM_Protocol_Functions FBUS2Protocol;
#endif
#if defined(GSM_ENABLE_PHONETBLUE) || defined(GSM_ENABLE_IRDAPHONET) || defined(GSM_ENABLE_BLUEPHONET)
	extern GSM_Protocol_Functions PHONETProtocol;
#endif
#if defined(GSM_ENABLE_AT) || defined(GSM_ENABLE_BLUEAT) || defined(GSM_ENABLE_IRDAAT)
	extern GSM_Protocol_Functions ATProtocol;
#endif
#ifdef GSM_ENABLE_ALCABUS
	extern GSM_Protocol_Functions ALCABUSProtocol;
#endif
#if defined(GSM_ENABLE_IRDAOBEX) || defined(GSM_ENABLE_BLUEOBEX)
	extern GSM_Protocol_Functions OBEXProtocol;
#endif
#if defined(GSM_ENABLE_MROUTERBLUE)
	extern GSM_Protocol_Functions MROUTERProtocol;
#endif

/**
 * Structure containing protocol specific data and pointer to protocol
 * functions - @ref GSM_Protocol_Functions. The data are in a structure, so
 * you may use more protocols at once and switch between them.
 */
typedef struct {
	struct {
		char fake;
#ifdef GSM_ENABLE_MBUS2
		GSM_Protocol_MBUS2Data		MBUS2;
#endif
#if defined(GSM_ENABLE_FBUS2) || defined(GSM_ENABLE_FBUS2IRDA) || defined(GSM_ENABLE_FBUS2DLR3) || defined(GSM_ENABLE_FBUS2DKU5) || defined(GSM_ENABLE_FBUS2PL2303) || defined(GSM_ENABLE_FBUS2BLUE) || defined(GSM_ENABLE_BLUEFBUS2)
		GSM_Protocol_FBUS2Data		FBUS2;
#endif
#if defined(GSM_ENABLE_PHONETBLUE) || defined(GSM_ENABLE_IRDAPHONET) || defined(GSM_ENABLE_BLUEPHONET)
		GSM_Protocol_PHONETData		PHONET;
#endif
#if defined(GSM_ENABLE_AT) || defined(GSM_ENABLE_BLUEAT) || defined(GSM_ENABLE_IRDAAT)
		GSM_Protocol_ATData		AT;
#endif
#ifdef GSM_ENABLE_ALCABUS
		GSM_Protocol_ALCABUSData	ALCABUS;
#endif
#if defined(GSM_ENABLE_IRDAOBEX) || defined(GSM_ENABLE_BLUEOBEX)
		GSM_Protocol_OBEXData		OBEX;
#endif
#if defined(GSM_ENABLE_MROUTERBLUE)
		GSM_Protocol_MROUTERData	MROUTER;
#endif
	} Data;
	GSM_Protocol_Functions *Functions;
} GSM_Protocol;

/* -------------------------- Phone layer ---------------------------------- */

/**
 * Phone requests identificators, these are used for internally identifying
 * which operation is being performed.
 */
typedef enum {
	ID_None=1,
	ID_GetModel,
	ID_GetFirmware,
	ID_EnableSecurity,
	ID_GetIMEI,
	ID_GetDateTime,
	ID_GetAlarm,
	ID_GetMemory,
	ID_GetMemoryStatus,
	ID_GetSMSC,
	ID_GetSMSMessage,
	ID_EnableEcho,
	ID_EnableErrorInfo,
	ID_SetOBEX,
	ID_SetUSSD,
	ID_GetNote,
	ID_SetNote,
	ID_GetSignalQuality,
	ID_GetBatteryCharge,
	ID_GetSMSFolders,
	ID_GetSMSFolderStatus,
	ID_GetSMSStatus,
	ID_AddSMSFolder,
	ID_GetNetworkInfo,
	ID_GetRingtone,
	ID_DialVoice,
	ID_GetCalendarNotesInfo,
	ID_GetCalendarNote,
	ID_GetSecurityCode,
	ID_GetWAPBookmark,
	ID_GetBitmap,
	ID_SaveSMSMessage,
	ID_CancelCall,
	ID_SetDateTime,
	ID_SetAlarm,
	ID_DisableConnectFunc,
	ID_EnableConnectFunc,
	ID_AnswerCall,
	ID_SetBitmap,
	ID_SetRingtone,
	ID_DeleteSMSMessage,
	ID_DeleteCalendarNote,
	ID_SetPath,
	ID_SetSMSC,
	ID_SetProfile,
	ID_SetMemory,
	ID_DeleteMemory,
	ID_SetCalendarNote,
	ID_SetIncomingSMS,
	ID_SetIncomingCB,
	ID_GetCalendarNotePos,
	ID_Initialise,
	ID_GetConnectSet,
	ID_SetWAPBookmark,
	ID_GetLocale,
	ID_SetLocale,
	ID_GetCalendarSettings,
	ID_SetCalendarSettings,
	ID_GetGPRSPoint,
	ID_SetGPRSPoint,
	ID_EnableGPRSPoint,
	ID_DeleteWAPBookmark,
	ID_Netmonitor,
	ID_HoldCall,
	ID_UnholdCall,
	ID_ConferenceCall,
	ID_SplitCall,
	ID_TransferCall,
	ID_SwitchCall,
	ID_GetManufactureMonth,
	ID_GetProductCode,
	ID_GetOriginalIMEI,
	ID_GetHardware,
	ID_GetPPM,
	ID_GetSMSMode,
	ID_GetSMSMemories,
	ID_GetManufacturer,
	ID_SetMemoryType,
	ID_SetMemoryCharset,
	ID_GetMMSSettings,
	ID_SetSMSParameters,
	ID_GetFMStation,
	ID_SetFMStation,
	ID_GetLanguage,
	ID_SetFastSMSSending,
	ID_Reset,
	ID_GetToDo,
	ID_PressKey,
	ID_DeleteAllToDo,
	ID_SetLight,
	ID_Divert,
	ID_SetToDo,
	ID_PlayTone,
	ID_GetChatSettings,
	ID_GetSyncMLSettings,
	ID_GetSyncMLName,
	ID_GetSecurityStatus,
	ID_EnterSecurityCode,
	ID_GetProfile,
	ID_GetRingtonesInfo,
	ID_MakeAuthentication,
	ID_GetSpeedDial,
	ID_ResetPhoneSettings,
	ID_SendDTMF,
	ID_GetDisplayStatus,
	ID_SetAutoNetworkLogin,
	ID_SetConnectSet,
	ID_SetMMSSettings,
	ID_GetSIMIMSI,
	ID_GetFileInfo,
	ID_FileSystemStatus,
	ID_GetFile,
	ID_AddFile,
	ID_AddFolder,
	ID_DeleteFile,
#ifdef GSM_ENABLE_ALCATEL
    	/* AT mode */
    	ID_SetFlowControl,
    	ID_AlcatelConnect,
	ID_AlcatelProtocol,
    
    	/* Binary mode */
    	ID_AlcatelAttach,
    	ID_AlcatelDetach,
    	ID_AlcatelCommit,
    	ID_AlcatelCommit2,
	ID_AlcatelEnd,
    	ID_AlcatelClose,
   	ID_AlcatelStart,
    	ID_AlcatelSelect1,
    	ID_AlcatelSelect2,
    	ID_AlcatelSelect3,
    	ID_AlcatelBegin1,
   	ID_AlcatelBegin2,
    	ID_AlcatelGetIds1,
    	ID_AlcatelGetIds2,
        ID_AlcatelGetCategories1,
        ID_AlcatelGetCategories2,
        ID_AlcatelGetCategoryText1,
        ID_AlcatelGetCategoryText2,
        ID_AlcatelAddCategoryText1,
        ID_AlcatelAddCategoryText2,
    	ID_AlcatelGetFields1,
    	ID_AlcatelGetFields2,
    	ID_AlcatelGetFieldValue1,
    	ID_AlcatelGetFieldValue2,    
   	ID_AlcatelDeleteItem1,
   	ID_AlcatelDeleteItem2,
   	ID_AlcatelDeleteField,
	ID_AlcatelCreateField,
	ID_AlcatelUpdateField,
#endif
	ID_IncomingFrame,

	ID_User1,
	ID_User2,
	ID_User3,
	ID_User4,
	ID_User5,
	ID_User6,
	ID_User7,
	ID_User8,
	ID_User9,
	ID_User10,

	ID_EachFrame
} GSM_Phone_RequestID;

/**
 * Phone related data are stored here.
 */
typedef struct {
	/**
	 * Phone IMEI (or serial number).
	 */
	char			IMEI[MAX_IMEI_LENGTH];
	/**
	 * Phone manufacturer as reported by phone.
	 */
	char			Manufacturer[MAX_MANUFACTURER_LENGTH];
	/**
	 * Phone model as reported by phone.
	 */
	char			Model[MAX_MODEL_LENGTH];
	/**
	 * Model information, pointer to static @ref allmodels array.
	 */
	OnePhoneModel		*ModelInfo;
	/**
	 * Phone version as reported by phone. It doesn't have to be numerical
	 * at all.
	 */
	char			Version[MAX_VERSION_LENGTH];
	/**
	 * Phone version date, might be empty for some models.
	 */
	char			VerDate[MAX_VERSION_DATE_LENGTH];
	/**
	 * Phone version as number, if applicable.
	 */
	double			VerNum;
	/**
	 * Cache for hardware version used by some modules.
	 */
	char			HardwareCache[50];
	/**
	 * Cache for product code version used by some modules.
	 */
	char			ProductCodeCache[50];
	
	/**
	 * Counter used for disabling startup info on phone, see
	 * @ref GSM_Phone_Functions::ShowStartInfo . After this is 0, the startup info is hidden.
	 */
	int			StartInfoCounter;

	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_GPRSAccessPoint	*GPRSPoint;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SpeedDial		*SpeedDial;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_DateTime		*DateTime;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_Alarm		*Alarm;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_MemoryEntry		*Memory;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_MemoryStatus	*MemoryStatus;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SMSC		*SMSC;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_MultiSMSMessage	*GetSMSMessage;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SMSMessage		*SaveSMSMessage;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SMSMemoryStatus	*SMSStatus;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SMSFolders		*SMSFolders;
	/**
	 * Used internally by phone drivers.
	 */
	int                 	*VoiceRecord;
	/**
	 * Used internally by phone drivers.
	 */
	int			CallID;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SignalQuality	*SignalQuality;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_BatteryCharge	*BatteryCharge;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_NetworkInfo		*NetworkInfo;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_Ringtone		*Ringtone;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_CalendarEntry	*Cal;
	/**
	 * Used internally by phone drivers.
	 */
	unsigned char		*SecurityCode;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_WAPBookmark		*WAPBookmark;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_MultiWAPSettings	*WAPSettings;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_Bitmap		*Bitmap;
	/**
	 * Used internally by phone drivers.
	 */
	unsigned char		*Netmonitor;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_MultiCallDivert	*Divert;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_ToDoEntry		*ToDo;
	GSM_NoteEntry		*Note;
	/**
	 * Used internally by phone drivers.
	 */
	bool			PressKey;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_SecurityCodeType	*SecurityStatus;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_Profile		*Profile;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_AllRingtonesInfo	*RingtonesInfo;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_DisplayFeatures	*DisplayFeatures;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_FMStation		*FMStation;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_Locale		*Locale;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_CalendarSettings	*CalendarSettings;
	/**
	 * Used internally by phone drivers.
	 */
	unsigned char		*PhoneString;
	/**
	 * Used internally by phone drivers.
	 */
	int			StartPhoneString;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_File		*FileInfo;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_File		*File;
	/**
	 * Pointer to structure used internally by phone drivers.
	 */
	GSM_FileSystemStatus	*FileSystemStatus;
	GSM_ChatSettings	*ChatSettings;
	GSM_SyncMLSettings	*SyncMLSettings;

	/**
	 * Should phone notify about incoming calls?
	 */
	bool			EnableIncomingCall;
	/**
	 * Should phone notify about incoming SMSes?
	 */
	bool			EnableIncomingSMS;
	/**
	 * Should phone notify about incoming CBs?
	 */
	bool			EnableIncomingCB;
	/**
	 * Should phone notify about incoming USSDs?
	 */
	bool			EnableIncomingUSSD;

	/**
	 * Last message received from phone.
	 */
	GSM_Protocol_Message	*RequestMsg;
	/**
	 * Last message sent by Gammu.
	 */
	GSM_Protocol_Message	*SentMsg;
	/**
	 * What operation is being performed now, see @ref GSM_Phone_RequestID
	 * for possible values.
	 */
	GSM_Phone_RequestID	RequestID;
	/**
	 * Error returned by function in phone module.
	 */
	GSM_Error		DispatchError;
	
	/**
	 * Structure with private phone modules data.
	 */
	struct {
		int			 fake;
#ifdef GSM_ENABLE_NOKIA3320
		GSM_Phone_N3320Data	 N3320;
#endif
#ifdef GSM_ENABLE_NOKIA3650
		GSM_Phone_N3650Data	 N3650;
#endif
#ifdef GSM_ENABLE_NOKIA650
		GSM_Phone_N650Data	 N650;
#endif
#ifdef GSM_ENABLE_NOKIA6110
		GSM_Phone_N6110Data	 N6110;
#endif
#ifdef GSM_ENABLE_NOKIA6510
		GSM_Phone_N6510Data	 N6510;
#endif
#ifdef GSM_ENABLE_NOKIA7110
		GSM_Phone_N7110Data	 N7110;
#endif
#ifdef GSM_ENABLE_ATGEN
		GSM_Phone_ATGENData	 ATGEN;
#endif
#ifdef GSM_ENABLE_ALCATEL
		GSM_Phone_ALCATELData	 ALCATEL;
#endif
#ifdef GSM_ENABLE_OBEXGEN
		GSM_Phone_OBEXGENData	 OBEXGEN;
#endif
#ifdef GSM_ENABLE_MROUTERGEN
		GSM_Phone_MROUTERGENData MROUTERGEN;
#endif
	} Priv;
} GSM_Phone_Data;

/**
 * Structure for defining reply functions.
 *
 * Function is called when requestID matches current operation or is 
 * ID_IncomingFrame and msgtype matches start message and (if msgtype is just
 * one character) subtypechar is zero or subtypechar-th character of message
 * matches subtype.
 *
 * Should be used in array with last element containing ID_None as requestID.
 */
struct _GSM_Reply_Function {
	/**
	 * Pointer to function that should be executed.
	 */
	GSM_Error (*Function)	(GSM_Protocol_Message msg, GSM_StateMachine *s);
	/**
	 * Message type, if it is longer than 1 character, it disables subtype
	 * checking.
	 */
	unsigned char		*msgtype;
	/**
	 * Which character of message should be checked as subtype. Zero to
	 * disable subtype checking.
	 */
	int			subtypechar;
	/**
	 * Subtype to be checked.
	 */
	unsigned char		subtype;
	/**
	 * Phone request when this can be called, use ID_IncomingFrame when
	 * you want to use this in any state.
	 */
	GSM_Phone_RequestID	requestID;
};

/**
 * Structure defining phone functions.
 */
typedef struct {
	/**
	 * Names of supported models separated by |. Must contain at least one
	 * name.
	 */
	char			    	*models;
	/**
	 * Array of reply functions for the phone, see 
	 * @ref GSM_Reply_Function for details about it.
	 */
	GSM_Reply_Function	      	*ReplyFunctions;
	/**
	 * Initializes phone.
	 */
	GSM_Error (*Initialise)	 	(GSM_StateMachine *s);
	/**
	 * Terminates phone communication.
	 */
	GSM_Error (*Terminate)	  	(GSM_StateMachine *s);
	/**
	 * Dispatches messages from phone, at the end it should call 
	 * @ref GSM_DispatchMessage.
	 */
	GSM_Error (*DispatchMessage)    (GSM_StateMachine *s);
	/**
	 * Enables showing information on phone display.
	 */
	GSM_Error (*ShowStartInfo)      (GSM_StateMachine *s, bool enable);
	/**
	 * Reads manufacturer from phone.
	 */
	GSM_Error (*GetManufacturer)    (GSM_StateMachine *s);
	/**
	 * Reads model from phone.
	 */
	GSM_Error (*GetModel)	   	(GSM_StateMachine *s);
	/**
	 * Reads firmware information from phone.
	 */
	GSM_Error (*GetFirmware)	(GSM_StateMachine *s);
	/**
	 * Reads IMEI/serial number from phone.
	 */
	GSM_Error (*GetIMEI)	    	(GSM_StateMachine *s);
	/**
	 * Gets date and time from phone.
	 */
	GSM_Error (*GetOriginalIMEI)    (GSM_StateMachine *s, char *value);
	/**
	 * Gets month when device was manufactured.
	 */
	GSM_Error (*GetManufactureMonth)(GSM_StateMachine *s, char *value);
	/**
	 * Gets product code of device.
	 */
	GSM_Error (*GetProductCode)     (GSM_StateMachine *s, char *value);
	/**
	 * Gets hardware information about device.
	 */
	GSM_Error (*GetHardware)	(GSM_StateMachine *s, char *value);
	/**
	 * Gets PPM (Post Programmable Memory) info from phone
	 * (in other words for Nokia get, which language pack is in phone)
	 */
	GSM_Error (*GetPPM)	     	(GSM_StateMachine *s, char *value);
	/**
	 * Gets SIM IMSI from phone.
	 */
	GSM_Error (*GetSIMIMSI)	 	(GSM_StateMachine *s, char *IMSI);
	/**
	 * Reads date and time from phone.
	 */
	GSM_Error (*GetDateTime)	(GSM_StateMachine *s, GSM_DateTime *date_time);
	/**
	 * Sets date and time in phone.
	 */
	GSM_Error (*SetDateTime)	(GSM_StateMachine *s, GSM_DateTime *date_time);
	/**
	 * Reads alarm set in phone.
	 */
	GSM_Error (*GetAlarm)	   	(GSM_StateMachine *s, GSM_Alarm	*alarm);
	/**
	 * Sets alarm in phone.
	 */
	GSM_Error (*SetAlarm)	   	(GSM_StateMachine *s, GSM_Alarm *alarm);
	/**
	 * Gets locale from phone.
	 */
	GSM_Error (*GetLocale)	  	(GSM_StateMachine *s, GSM_Locale *locale);
	/**
	 * Sets locale of phone.
	 */
	GSM_Error (*SetLocale)	  	(GSM_StateMachine *s, GSM_Locale *locale);
	/**
	 * Emulates key press or key release.
	 */
	GSM_Error (*PressKey)	   	(GSM_StateMachine *s, GSM_KeyCode Key, bool Press);
	/**
	 * Performs phone reset.
	 */
	GSM_Error (*Reset)	      	(GSM_StateMachine *s, bool hard);
	/**
	 * Resets phone settings.
	 */
	GSM_Error (*ResetPhoneSettings) (GSM_StateMachine *s, GSM_ResetSettingsType Type);
	/**
	 * Enters security code (PIN, PUK,...) .
	 */
	GSM_Error (*EnterSecurityCode)  (GSM_StateMachine *s, GSM_SecurityCode Code);
	/**
	 * Queries whether some security code needs to be entered./
	 */
	GSM_Error (*GetSecurityStatus)  (GSM_StateMachine *s, GSM_SecurityCodeType *Status);
	/**
	 * Acquired display status.
	 */
	GSM_Error (*GetDisplayStatus)   (GSM_StateMachine *s, GSM_DisplayFeatures *features);
	/**
	 * Enables network auto login.
	 */
	GSM_Error (*SetAutoNetworkLogin)(GSM_StateMachine *s);
	/**
	 * Gets information about batery charge and phone charging state.
	 */
	GSM_Error (*GetBatteryCharge)   (GSM_StateMachine *s, GSM_BatteryCharge *bat);
	/**
	 * Reads signal quality (strength and error rate).
	 */
	GSM_Error (*GetSignalQuality)   (GSM_StateMachine *s, GSM_SignalQuality *sig);
	/**
	 * Gets network information.
	 */
	GSM_Error (*GetNetworkInfo)     (GSM_StateMachine *s, GSM_NetworkInfo *netinfo);
	/**
	 * Reads category from phone.
	 */
	GSM_Error (*GetCategory)	(GSM_StateMachine *s, GSM_Category *Category);
	/**
	 * Adds category to phone.
	 */
	GSM_Error (*AddCategory)	(GSM_StateMachine *s, GSM_Category *Category);
	/**
	 * Reads category status (number of used entries) from phone.
	 */
	GSM_Error (*GetCategoryStatus)  (GSM_StateMachine *s, GSM_CategoryStatus *Status);
	/**
	 * Gets memory (phonebooks or calls) status (eg. number of used and
	 * free entries).
	 */
	GSM_Error (*GetMemoryStatus)    (GSM_StateMachine *s, GSM_MemoryStatus *status);
	/**
	 * Reads entry from memory (phonebooks or calls). Which entry should
	 * be read is defined in entry.
	 */
	GSM_Error (*GetMemory)	  	(GSM_StateMachine *s, GSM_MemoryEntry *entry);
	/**
	 * Reads entry from memory (phonebooks or calls). Which entry should
	 * be read is defined in entry. This can be easily used for reading all entries.
	 */
	GSM_Error (*GetNextMemory)      (GSM_StateMachine *s, GSM_MemoryEntry *entry, bool start);
	/**
	 * Sets memory (phonebooks or calls) entry.
	 */
	GSM_Error (*SetMemory)	  	(GSM_StateMachine *s, GSM_MemoryEntry *entry);
	/**
	 * Deletes memory (phonebooks or calls) entry.
	 */
	GSM_Error (*AddMemory)	  	(GSM_StateMachine *s, GSM_MemoryEntry *entry);
	/**
	 * Deletes memory (phonebooks or calls) entry.
	 */
	GSM_Error (*DeleteMemory)       (GSM_StateMachine *s, GSM_MemoryEntry *entry);
	/**
	 * Deletes all memory (phonebooks or calls) entries of specified type.
	 */
	GSM_Error (*DeleteAllMemory)    (GSM_StateMachine *s, GSM_MemoryType MemoryType);
	/**
	 * Gets speed dial.
	 */
	GSM_Error (*GetSpeedDial)       (GSM_StateMachine *s, GSM_SpeedDial *Speed);
	/**
	 * Sets speed dial.
	 */
	GSM_Error (*SetSpeedDial)       (GSM_StateMachine *s, GSM_SpeedDial *Speed);
	/**
	 * Gets SMS Service Center number and SMS settings.
	 */
	GSM_Error (*GetSMSC)	    	(GSM_StateMachine *s, GSM_SMSC *smsc);
	/**
	 * Sets SMS Service Center number and SMS settings.
	 */
	GSM_Error (*SetSMSC)	    	(GSM_StateMachine *s, GSM_SMSC *smsc);
	/**
	 * Gets information about SMS memory (read/unread/size of memory for
	 * both SIM and phone).
	 */
	GSM_Error (*GetSMSStatus)       (GSM_StateMachine *s, GSM_SMSMemoryStatus *status);
	/**
	 * Reads SMS message.
	 */
	GSM_Error (*GetSMS)	     	(GSM_StateMachine *s, GSM_MultiSMSMessage *sms);
	/**
	 * Reads next (or first if start set) SMS message. This might be
	 * faster for some phones than using @ref GetSMS for each message.
	 */
	GSM_Error (*GetNextSMS)	 	(GSM_StateMachine *s, GSM_MultiSMSMessage *sms, bool start);
	/**
	 * Sets SMS.
	 */
	GSM_Error (*SetSMS)	     	(GSM_StateMachine *s, GSM_SMSMessage *sms);
	/**
	 * Adds SMS to specified folder.
	 */
	GSM_Error (*AddSMS)	     	(GSM_StateMachine *s, GSM_SMSMessage *sms);
	/**
	 * Deletes SMS.
	 */
	GSM_Error (*DeleteSMS)	  	(GSM_StateMachine *s, GSM_SMSMessage *sms);
	/**
	 * Sends SMS.
	 */
	GSM_Error (*SendSMS)	    	(GSM_StateMachine *s, GSM_SMSMessage *sms);
	/**
	 * Sends SMS already saved in phone.
	 */
	GSM_Error (*SendSavedSMS)	(GSM_StateMachine *s, int Folder, int Location);
	GSM_Error (*SetFastSMSSending)  (GSM_StateMachine *s, bool enable);
	/**
	 * Enable/disable notification on incoming SMS.
	 */
	GSM_Error (*SetIncomingSMS)     (GSM_StateMachine *s, bool enable);
	/**
	 * Gets network information from phone.
	 */
	GSM_Error (*SetIncomingCB)      (GSM_StateMachine *s, bool enable);
	/**
	 * Returns SMS folders information.
	 */
	GSM_Error (*GetSMSFolders)      (GSM_StateMachine *s, GSM_SMSFolders *folders);
	/**
	 * Creates SMS folder.
	 */
	GSM_Error (*AddSMSFolder)       (GSM_StateMachine *s, unsigned char *name);
	/**
	 * Deletes SMS folder.
	 */
	GSM_Error (*DeleteSMSFolder)    (GSM_StateMachine *s, int ID);
	/**
	 * Dials number and starts voice call.
	 */
	GSM_Error (*DialVoice)	  	(GSM_StateMachine *s, char *Number, GSM_CallShowNumber ShowNumber);
	/**
	 * Accept current incoming call.
	 */
	GSM_Error (*AnswerCall)	 	(GSM_StateMachine *s, int ID, bool all);		     
	/**
	 * Deny current incoming call.
	 */
	GSM_Error (*CancelCall)	 	(GSM_StateMachine *s, int ID, bool all);
	/**
	 * Holds call.
	 */
	GSM_Error (*HoldCall)	   	(GSM_StateMachine *s, int ID);
	/**
	 * Unholds call.
	 */
	GSM_Error (*UnholdCall)	 	(GSM_StateMachine *s, int ID);
	/**
	 * Initiates conference call.
	 */
	GSM_Error (*ConferenceCall)     (GSM_StateMachine *s, int ID);
	/**
	 * Splits call.
	 */
	GSM_Error (*SplitCall)	  	(GSM_StateMachine *s, int ID);
	/**
	 * Transfers call.
	 */
	GSM_Error (*TransferCall)       (GSM_StateMachine *s, int ID, bool next);
	/**
	 * Switches call.
	 */
	GSM_Error (*SwitchCall)	 	(GSM_StateMachine *s, int ID, bool next);
	/**
	 * Gets call diverts.
	 */
	GSM_Error (*GetCallDivert)      (GSM_StateMachine *s, GSM_MultiCallDivert *divert);
	/**
	 * Sets call diverts.
	 */
	GSM_Error (*SetCallDivert)      (GSM_StateMachine *s, GSM_MultiCallDivert *divert);
	/**
	 * Cancels all diverts.
	 */
	GSM_Error (*CancelAllDiverts)   (GSM_StateMachine *s);
	/**
	 * Activates/deactivates noticing about incoming calls.
	 */
	GSM_Error (*SetIncomingCall)    (GSM_StateMachine *s, bool enable);
	/**
	 * Activates/deactivates noticing about incoming USSDs (UnStructured Supplementary Services).
	 */
	GSM_Error (*SetIncomingUSSD)    (GSM_StateMachine *s, bool enable);
	/**
	 * Sends DTMF (Dual Tone Multi Frequency) tone.
	 */
	GSM_Error (*SendDTMF)	   	(GSM_StateMachine *s, char *sequence);
	/**
	 * Gets ringtone from phone.
	 */
	GSM_Error (*GetRingtone)	(GSM_StateMachine *s, GSM_Ringtone *Ringtone, bool PhoneRingtone);
	/**
	 * Sets ringtone in phone.
	 */
	GSM_Error (*SetRingtone)	(GSM_StateMachine *s, GSM_Ringtone *Ringtone, int *maxlength);
	/**
	 * Acquires ringtone informaiton.
	 */
	GSM_Error (*GetRingtonesInfo)   (GSM_StateMachine *s, GSM_AllRingtonesInfo *Info);
	/**
	 * Deletes user defined ringtones from phone.
	 */
	GSM_Error (*DeleteUserRingtones)(GSM_StateMachine *s);
	/**
	 * Plays tone.
	 */
	GSM_Error (*PlayTone)	   	(GSM_StateMachine *s, int Herz, unsigned char Volume, bool start);
	/**
	 * Reads WAP bookmark.
	 */
	GSM_Error (*GetWAPBookmark)     (GSM_StateMachine *s, GSM_WAPBookmark *bookmark);
	/**
	 * Sets WAP bookmark.
	 */
	GSM_Error (*SetWAPBookmark)     (GSM_StateMachine *s, GSM_WAPBookmark *bookmark);
	/**
	 * Deletes WAP bookmark.
	 */
	GSM_Error (*DeleteWAPBookmark)  (GSM_StateMachine *s, GSM_WAPBookmark *bookmark);
	/**
	 * Acquires WAP settings.
	 */
	GSM_Error (*GetWAPSettings)     (GSM_StateMachine *s, GSM_MultiWAPSettings *settings);
	/**
	 * Changes WAP settings.
	 */
	GSM_Error (*SetWAPSettings)     (GSM_StateMachine *s, GSM_MultiWAPSettings *settings);
	/**
	 * Acquires MMS settings.
	 */
	GSM_Error (*GetMMSSettings)     (GSM_StateMachine *s, GSM_MultiWAPSettings *settings);
	/**
	 * Changes MMS settings.
	 */
	GSM_Error (*SetMMSSettings)     (GSM_StateMachine *s, GSM_MultiWAPSettings *settings);
	/**
	 * Acquires SyncML settings.
	 */
	GSM_Error (*GetSyncMLSettings)  (GSM_StateMachine *s, GSM_SyncMLSettings *settings);
	/**
	 * Changes SyncML settings.
	 */
	GSM_Error (*SetSyncMLSettings)  (GSM_StateMachine *s, GSM_SyncMLSettings *settings);
	/**
	 * Acquires chat/presence settings.
	 */
	GSM_Error (*GetChatSettings)    (GSM_StateMachine *s, GSM_ChatSettings *settings);
	/**
	 * Changes chat/presence settings.
	 */
	GSM_Error (*SetChatSettings)    (GSM_StateMachine *s, GSM_ChatSettings *settings);
	/**
	 * Gets bitmap.
	 */
	GSM_Error (*GetBitmap)	  	(GSM_StateMachine *s, GSM_Bitmap *Bitmap);
	/**
	 * Sets bitmap.
	 */
	GSM_Error (*SetBitmap)	  	(GSM_StateMachine *s, GSM_Bitmap *Bitmap);
	/**
	 * Gets status of ToDos (count of used entries).
	 */
	GSM_Error (*GetToDoStatus)      (GSM_StateMachine *s, GSM_ToDoStatus *status);
	/**
	 * Reads ToDo from phone.
	 */
	GSM_Error (*GetToDo)	    	(GSM_StateMachine *s, GSM_ToDoEntry *ToDo);
	/**
	 * Reads ToDo from phone.
	 */
	GSM_Error (*GetNextToDo)	(GSM_StateMachine *s, GSM_ToDoEntry *ToDo, bool start);
	/**
	 * Sets ToDo in phone.
	 */
	GSM_Error (*SetToDo)	    	(GSM_StateMachine *s, GSM_ToDoEntry *ToDo);
	/**
	 * Adds ToDo in phone.
	 */
	GSM_Error (*AddToDo)	    	(GSM_StateMachine *s, GSM_ToDoEntry *ToDo);
	/**
	 * Deletes ToDo entry in phone.
	 */
	GSM_Error (*DeleteToDo)	 	(GSM_StateMachine *s, GSM_ToDoEntry *ToDo);
	/**
	 * Deletes all todo entries in phone.
	 */
	GSM_Error (*DeleteAllToDo)      (GSM_StateMachine *s);
	/**
	 * Retrieves calendar status (number of used entries).
	 */
	GSM_Error (*GetCalendarStatus)  (GSM_StateMachine *s, GSM_CalendarStatus *Status);
	/**
	 * Retrieves calendar entry.
	 */
	GSM_Error (*GetCalendar)	(GSM_StateMachine *s, GSM_CalendarEntry *Note);
	/**
	 * Retrieves calendar entry. This is useful for continuous reading of all
	 * calendar entries.
	 */
	GSM_Error (*GetNextCalendar)    (GSM_StateMachine *s, GSM_CalendarEntry *Note, bool start);
	/**
	 * Sets calendar entry
	 */
	GSM_Error (*SetCalendar)	(GSM_StateMachine *s, GSM_CalendarEntry *Note);
	/**
	 * Adds calendar entry.
	 */
	GSM_Error (*AddCalendar)	(GSM_StateMachine *s, GSM_CalendarEntry *Note);
	/**
	 * Deletes calendar entry.
	 */
	GSM_Error (*DeleteCalendar)     (GSM_StateMachine *s, GSM_CalendarEntry *Note);
	/**
	 * Deletes all calendar entries.
	 */
	GSM_Error (*DeleteAllCalendar)  (GSM_StateMachine *s);
	/**
	 * Reads calendar settings.
	 */
	GSM_Error (*GetCalendarSettings)(GSM_StateMachine *s, GSM_CalendarSettings *settings);
	/**
	 * Sets calendar settings.
	 */
	GSM_Error (*SetCalendarSettings)(GSM_StateMachine *s, GSM_CalendarSettings *settings);
	/**
	 * Retrieves notes status (number of used entries).
	 */
	GSM_Error (*GetNotesStatus)  	(GSM_StateMachine *s, GSM_ToDoStatus *status);
	/**
	 * Retrieves notes entry.
	 */
	GSM_Error (*GetNote)		(GSM_StateMachine *s, GSM_NoteEntry *Note);
	/**
	 * Retrieves note entry. This is useful for continuous reading of all
	 * notes entries.
	 */
	GSM_Error (*GetNextNote)    	(GSM_StateMachine *s, GSM_NoteEntry *Note, bool start);
	/**
	 * Sets note entry
	 */
	GSM_Error (*SetNote)		(GSM_StateMachine *s, GSM_NoteEntry *Note);
	/**
	 * Adds note entry.
	 */
	GSM_Error (*AddNote)		(GSM_StateMachine *s, GSM_NoteEntry *Note);
	/**
	 * Deletes note entry.
	 */
	GSM_Error (*DeleteNote)     	(GSM_StateMachine *s, GSM_NoteEntry *Note);
	/**
	 * Deletes all notes entries.
	 */
	GSM_Error (*DeleteAllNotes)  	(GSM_StateMachine *s);
	/**
	 * Reads profile.
	 */
	GSM_Error (*GetProfile)	 	(GSM_StateMachine *s, GSM_Profile *Profile);
	/**
	 * Updates profile.
	 */
	GSM_Error (*SetProfile)	 	(GSM_StateMachine *s, GSM_Profile *Profile);
	/**
	 * Reads FM station.
	 */
	GSM_Error (*GetFMStation)       (GSM_StateMachine *s, GSM_FMStation *FMStation);
	/**
	 * Sets FM station.
	 */
	GSM_Error (*SetFMStation)       (GSM_StateMachine *s, GSM_FMStation *FMStation);
	/**
	 * Clears defined FM stations.
	 */
	GSM_Error (*ClearFMStations)    (GSM_StateMachine *s);
	/**
	 * Gets next filename from filesystem.
	 */
	GSM_Error (*GetNextFileFolder)  (GSM_StateMachine *s, GSM_File *File, bool start);
	/**
	 * Gets file part from filesystem.
	 */
	GSM_Error (*GetFilePart)	(GSM_StateMachine *s, GSM_File *File);
	/**
	 * Adds file part to filesystem.
	 */
	GSM_Error (*AddFilePart)	(GSM_StateMachine *s, GSM_File *File, int *Pos);
	/**
	 * Acquires filesystem status.
	 */
	GSM_Error (*GetFileSystemStatus)(GSM_StateMachine *s, GSM_FileSystemStatus *Status);
	/**
	 * Deletes file from filessytem.
	 */
	GSM_Error (*DeleteFile)	 	(GSM_StateMachine *s, unsigned char *ID);
	/**
	 * Adds folder to filesystem.
	 */
	GSM_Error (*AddFolder)	  	(GSM_StateMachine *s, GSM_File *File);
	/**
	 * Gets GPRS access point.
	 */
	GSM_Error (*GetGPRSAccessPoint) (GSM_StateMachine *s, GSM_GPRSAccessPoint *point);
	/**
	 * Sets GPRS access point.
	 */
	GSM_Error (*SetGPRSAccessPoint) (GSM_StateMachine *s, GSM_GPRSAccessPoint *point);
} GSM_Phone_Functions;

	extern GSM_Phone_Functions NAUTOPhone;
#ifdef GSM_ENABLE_NOKIA3320
	extern GSM_Phone_Functions N3320Phone;
#endif
#ifdef GSM_ENABLE_NOKIA3650
	extern GSM_Phone_Functions N3650Phone;
#endif
#ifdef GSM_ENABLE_NOKIA6110
	extern GSM_Phone_Functions N6110Phone;
#endif
#ifdef GSM_ENABLE_NOKIA650
	extern GSM_Phone_Functions N650Phone;
#endif
#ifdef GSM_ENABLE_NOKIA6510
	extern GSM_Phone_Functions N6510Phone;
#endif
#ifdef GSM_ENABLE_NOKIA7110
	extern GSM_Phone_Functions N7110Phone;
#endif
#ifdef GSM_ENABLE_NOKIA9210
	extern GSM_Phone_Functions N9210Phone;
#endif
#ifdef GSM_ENABLE_ATGEN
	extern GSM_Phone_Functions ATGENPhone;
#endif
#ifdef GSM_ENABLE_ALCATEL
	extern GSM_Phone_Functions ALCATELPhone;
#endif
#ifdef GSM_ENABLE_OBEXGEN
	extern GSM_Phone_Functions OBEXGENPhone;
#endif
#ifdef GSM_ENABLE_MROUTERGEN
	extern GSM_Phone_Functions MROUTERGENPhone;
#endif

typedef struct {
	GSM_Phone_Data		 Data;
	GSM_Phone_Functions	*Functions;
} GSM_Phone;

/* --------------------------- User layer ---------------------------------- */

struct _GSM_User {
	GSM_Reply_Function		*UserReplyFunctions;

	void	  (*IncomingCall)	(char *Device, GSM_Call	      call);
	void 	  (*IncomingSMS)	(char *Device, GSM_SMSMessage sms);
	void 	  (*IncomingCB)		(char *Device, GSM_CBMessage  cb);
	void 	  (*IncomingUSSD)	(char *Device, char	      *Text);
	void 	  (*SendSMSStatus)	(char *Device, int 	      status, int MessageReference);
};

/* --------------------------- Statemachine layer -------------------------- */

typedef enum {
	GCT_MBUS2=1,
	GCT_FBUS2,
	GCT_FBUS2DLR3,
	GCT_FBUS2DKU5,
	GCT_FBUS2PL2303,
	GCT_FBUS2BLUE,
	GCT_FBUS2IRDA,
	GCT_PHONETBLUE,
	GCT_AT,
	GCT_MROUTERBLUE,

	GCT_IRDAOBEX,
	GCT_IRDAAT,
	GCT_IRDAPHONET,
	GCT_BLUEFBUS2,
	GCT_BLUEAT,
	GCT_BLUEPHONET,
	GCT_BLUEOBEX
} GSM_ConnectionType;

typedef struct {
	/* Config file (or Registry or...) variables */
	char			Model[50];	   /* Model from config file 		*/
	char			DebugLevel[50];    /* Debug level			*/
	char 			*Device;	   /* Device name from config file 	*/
	char			*Connection;	   /* Connection type as string		*/
	char			*SyncTime;	   /* Synchronize time on startup? 	*/
	char			*LockDevice;	   /* Lock device ? (Unix)		*/
	char			*DebugFile;        /* Name of debug file		*/
	char 			*Localize;	   /* Name of localisation file		*/
	char			*StartInfo;	   /* Display something during start ?  */
	bool			UseGlobalDebugFile;/* Should we use global debug file?	*/
	bool			DefaultModel;
	bool			DefaultDebugLevel;
	bool			DefaultDevice;
	bool			DefaultConnection;
	bool			DefaultSyncTime;
	bool			DefaultLockDevice;
	bool			DefaultDebugFile;
	bool			DefaultLocalize;
	bool			DefaultStartInfo;
} GSM_Config;

#define MAX_CONFIG_NUM		5

struct _GSM_StateMachine {
	GSM_ConnectionType 	ConnectionType;				/* Type of connection as int			*/
	char			*LockFile;				/* Lock file name for Unix 			*/
	Debug_Info		di;					/* Debug information				*/
	bool			opened;					/* Is connection opened ?			*/
	GSM_Config		Config[MAX_CONFIG_NUM + 1];
	GSM_Config		*CurrentConfig;				/* Config file (or Registry or...) variables 	*/
	int			ConfigNum;
	INI_Section 		*msg;			/* Localisation strings structure    		*/
	int			ReplyNum;				/* How many times make sth. 			*/
	int			Speed;					/* For some protocols used speed		*/

	GSM_Device		Device;
	GSM_Protocol		Protocol;
	GSM_Phone		Phone;
	GSM_User		User;
};

/* ------------------------ Other general definitions ---------------------- */

GSM_Error GSM_RegisterAllPhoneModules	(GSM_StateMachine *s);

GSM_Error GSM_InitConnection		(GSM_StateMachine *s, int ReplyNum);
GSM_Error GSM_TerminateConnection	(GSM_StateMachine *s);

int 	  GSM_ReadDevice		(GSM_StateMachine *s, bool wait);

GSM_Error GSM_WaitForOnce		(GSM_StateMachine *s, unsigned char *buffer,
			  		 int length, unsigned char type, int time);

GSM_Error GSM_WaitFor			(GSM_StateMachine *s, unsigned char *buffer,
		       			 int length, unsigned char type, int time,
					 GSM_Phone_RequestID request);

GSM_Error GSM_DispatchMessage		(GSM_StateMachine *s);

INI_Section 				*GSM_FindGammuRC(void);

bool 	  GSM_ReadConfig		(INI_Section *cfg_info, GSM_Config *cfg, int num);

void 	  GSM_DumpMessageLevel2		(GSM_StateMachine *s, unsigned char *message, int messagesize, int type);
void 	  GSM_DumpMessageLevel3		(GSM_StateMachine *s, unsigned char *message, int messagesize, int type);

/* ---------------------- Phone features ----------------------------------- */

typedef enum {
	/* n6110.c */
	F_CAL33 = 1,	/* Calendar,3310 style - 10 reminders, Unicode, 3 coding types	*/
	F_CAL52,	/* Calendar,5210 style - full Unicode, etc.			*/
	F_CAL82,	/* Calendar,8250 style - "normal", but with Unicode		*/
	F_RING_SM,	/* Ringtones returned in SM format - 33xx			*/
	F_NORING,	/* No ringtones							*/
	F_NOPBKUNICODE,	/* No phonebook in Unicode					*/
	F_NOWAP,      	/* No WAP							*/
	F_NOCALLER,	/* No caller groups						*/
	F_NOPICTURE,	/* No Picture Images						*/
	F_NOPICTUREUNI,	/* No Picture Images text in Unicode				*/
	F_NOSTARTUP,	/* No startup logo						*/
	F_NOCALENDAR,	/* No calendar							*/
	F_NOSTARTANI,	/* Startup logo is not animated 				*/
	F_POWER_BATT,	/* Network and battery level get from netmonitor		*/
	F_PROFILES33,	/* Phone profiles in 3310 style					*/
	F_PROFILES51,	/* Phone profiles in 5110 style					*/
	F_MAGICBYTES,	/* Phone can make authentication with magic bytes		*/
	F_NODTMF,	/* Phone can't send DTMF					*/
	F_DISPSTATUS,	/* Phone return display status					*/
	F_NOCALLINFO,

	/* n3320.c */
	F_DAYMONTH,	/* Day and month reversed in pbk, when compare to GSM models	*/

	/* n6510.c */
	F_PBK35,	/* Phonebook in 3510 style with ringtones ID			*/
	F_PBKIMG,	/* Phonebook in 7250 style with picture ID			*/
	F_PBKTONEGAL,	/* Phonebook with selecting ringtones from gallery              */
	F_PBKSMSLIST,	/* Phonebook with SMS list					*/
	F_PBKUSER,	/* Phonebook with user ID					*/
	F_RADIO,	/* Phone with FM radio						*/
	F_TODO63,	/* ToDo in 6310 style - 0x55 msg type				*/
	F_TODO66,	/* ToDo in 6610 style - like calendar, with date and other	*/
	F_NOMIDI,	/* No ringtones in MIDI						*/
	F_BLUETOOTH,	/* Bluetooth support						*/
	F_NOFILESYSTEM,	/* No images, ringtones, java saved in special filesystem	*/
	F_NOMMS,	/* No MMS sets in phone						*/
	F_NOGPRSPOINT,	/* GPRS point are not useable					*/
	F_CAL35,	/* Calendar,3510 style - Reminder,Call,Birthday			*/
	F_CAL65,	/* Calendar,6510 style - CBMM, method 3				*/
	F_WAPMMSPROXY,	/* WAP & MMS settings contains first & second proxy		*/

	/* n6510.c && n7110.c */
	F_VOICETAGS,	/* Voice tags available						*/
	F_CAL62,	/* Calendar,6210 style - Call,Birthday,Memo,Meeting		*/
	F_NOTES,

	/* AT modules */
	F_SMSONLYSENT,	/* Phone supports only sent/unsent messages			*/
	F_BROKENCPBS, 	/* CPBS on some memories can hang phone				*/
	F_M20SMS,	/* Siemens M20 like SMS handling				*/
	F_SLOWWRITE,	/* Use slower writing which some phone need			*/
	F_SMSME900,	/* SMS in ME start from location 900 - case of Sagem		*/
	F_ALCATEL	/* Phone supports Alcatel protocol				*/
} Feature;

/* For models table */
struct _OnePhoneModel {
	char		*model;
	char		*number;
	char		*irdamodel;
	Feature		features[12];
};

bool 		IsPhoneFeatureAvailable	(OnePhoneModel *model, Feature feature);
OnePhoneModel 	*GetModelData		(char *model, char *number, char *irdamodel);

#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
int smprintf(GSM_StateMachine *s, const char *format, ...);

void GSM_OSErrorInfo(GSM_StateMachine *s, char *description);

#ifdef GSM_ENABLE_BACKUP
void GSM_GetPhoneFeaturesForBackup(GSM_StateMachine *s, GSM_Backup_Info *info);
#endif

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
