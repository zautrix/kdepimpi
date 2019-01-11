/* (c) 2002-2004 by Marcin Wiacek */
/* based on some work from MSDN and others */
/* based on some work from Gnokii (www.gnokii.org)
 * (C) 1999-2000 Hugh Blemings & Pavel Janik ml. (C) 2001-2004 Pawel Kot 
 * GNU GPL version 2 or later
 */
/* Due to a problem in the source code management, the names of some of
 * the authors have unfortunately been lost. We do not mean to belittle
 * their efforts and hope they will contact us to see their names
 * properly added to the Copyright notice above.
 * Having published their contributions under the terms of the GNU
 * General Public License (GPL) [version 2], the Copyright of these
 * authors will remain respected by adhering to the license they chose
 * to publish their code under.
 */

#include "../../gsmstate.h"

#ifdef GSM_ENABLE_SERIALDEVICE
#ifdef WIN32

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <memory.h>

#include "../../gsmcomon.h"
#include "ser_w32.h"

static GSM_Error serial_close(GSM_StateMachine *s)
{
	GSM_Device_SerialData *d = &s->Device.Data.Serial;

	/* Disables all monitored events for device */
	SetCommMask(d->hPhone, 0);

	/* Discards all characters from input/output buffer and terminates
	 * pending read/write operations
	 */
	PurgeComm(d->hPhone, PURGE_TXABORT | PURGE_RXABORT |
			     PURGE_TXCLEAR | PURGE_RXCLEAR);

	/* Clears the DTR (data-terminal-ready) signal */
	EscapeCommFunction(d->hPhone, CLRDTR);

	/* Restores old settings */
	if (SetCommState(d->hPhone, &d->old_settings)==0) {
		GSM_OSErrorInfo(s, "SetCommState in serial_close");
	}

	/* Closes device */
	if (CloseHandle(d->hPhone)==0) {
		GSM_OSErrorInfo(s, "CloseHandle in serial_close");
	}

	return ERR_NONE;
}

static GSM_Error serial_open (GSM_StateMachine *s)
{
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;
	DCB 			dcb;
	unsigned char 		DeviceName[80],DeviceName2[80];
	int			i;
#ifdef GSM_ENABLE_FBUS2DKU5
	HKEY 			hKey;
	DWORD 			DeviceNameLen, KeyNameLen;
	unsigned char		KeyName[100];
#endif

	strcpy(DeviceName2,s->CurrentConfig->Device);

#ifdef GSM_ENABLE_FBUS2DKU5
	if (s->ConnectionType == GCT_FBUS2DKU5) {
		smprintf(s,"Reading DKU5 device\n");
		DeviceName2[0] = 0;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
			smprintf(s,"Error opening key\n");
			return ERR_DEVICENOTWORK;
		}
		i = 0;
		while(1) {
			DeviceNameLen = 80;
			KeyNameLen = 100;
			if (RegEnumValue(hKey,i,KeyName,&KeyNameLen,NULL,NULL,DeviceName2,&DeviceNameLen) != ERROR_SUCCESS) {
				smprintf(s,"Error reading key value\n");
				return ERR_DEVICENOTWORK;
			}
//			smprintf(s,"Key name is %s, value is %s\n",KeyName,DeviceName2);
			if (!strncmp(KeyName,"\\Device\\AtmelVirtualPort",24)) break;
			i++;
		}
		RegCloseKey(hKey);
		if (strlen(DeviceName2) == 0) return ERR_DEVICENOTWORK;
		smprintf(s,"DKU5 device is \"%s\"\n",DeviceName2);
		//nodriver
	}
#endif

	if ((s->ConnectionType == GCT_FBUS2DKU5) ||
	    (!strncmp(DeviceName2,"com",3) && strlen(DeviceName2)>3)) {
		sprintf(DeviceName,"\\\\.\\COM%i",atoi(DeviceName2+3));
	} else {
		strcpy(DeviceName,DeviceName2);
	}

	smprintf(s,"Device is %s\n",DeviceName);

	/* Allows for reading/writing, no device sharing */
	d->hPhone = CreateFile(DeviceName,
			       GENERIC_READ | GENERIC_WRITE,
			       0,
			       0,
			       OPEN_EXISTING,
			       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			       NULL);

	if (d->hPhone == INVALID_HANDLE_VALUE) {
		i = GetLastError();
		GSM_OSErrorInfo(s, "CreateFile in serial_open");
		if (i == 2)   return ERR_DEVICENOTWORK; //can't find specified file
		if (i == 5)   return ERR_DEVICEBUSY;    //access denied
		if (i == 31)  return ERR_DEVICENOTWORK; //attached device not working
		if (i == 123) return ERR_DEVICENOTEXIST;
		return ERR_DEVICEOPENERROR;
	}

	d->old_settings.DCBlength = sizeof(DCB);
	if (GetCommState(d->hPhone, &d->old_settings)==0) {
		GSM_OSErrorInfo(s, "ReadDevice in serial_open");
		return ERR_DEVICEREADERROR;
	}

	/* When char will be received, we will receive notifications */
	SetCommMask(d->hPhone, EV_RXCHAR);

	/* Sets size for input/output buffer */
	SetupComm(d->hPhone, 4096, 4096);

	/* Discards all characters from input/output buffer and terminates
	 * pending read/write operations
	 */
	PurgeComm(d->hPhone, PURGE_TXABORT | PURGE_RXABORT |
			     PURGE_TXCLEAR | PURGE_RXCLEAR);

 	memcpy(&dcb, &d->old_settings, sizeof(DCB));

	dcb.ByteSize 	 = 8;
	dcb.Parity 	 = NOPARITY; 
	dcb.StopBits 	 = ONESTOPBIT;

	/* No Xon/Xof flow control */
//	dcb.fOutX 	 = false;		
//	dcb.fInX 	 = false;
	
	/* Hardware flow control */
//	dcb.fOutxDsrFlow = true;
//	dcb.fOutxCtsFlow = true;
//	dcb.fDtrControl  = DTR_CONTROL_HANDSHAKE;
//	dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE;

    	/* Initialise the port settings */
	if (SetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "WriteDevice in serial_open");
		return ERR_DEVICEOPENERROR;
	}       	

	return ERR_NONE;
}

static GSM_Error serial_setparity (GSM_StateMachine *s, bool parity)
{
	DCB 			dcb;
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;

	dcb.DCBlength = sizeof(DCB);
	if (GetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "ReadDevice in serial_setparity");
		return ERR_DEVICEREADERROR;
	}

	if (parity) {
		dcb.Parity = ODDPARITY;
	} else {
		dcb.Parity = NOPARITY; 
	}

	if (SetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "WriteDevice in serial_setparity");
		return ERR_DEVICEPARITYERROR;
	}

	return ERR_NONE;
}

static GSM_Error serial_setdtrrts(GSM_StateMachine *s, bool dtr, bool rts)
{
	DCB        		dcb;
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;

	dcb.DCBlength = sizeof(DCB);
	if (GetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "ReadDevice in serial_setdtrrts");
		return ERR_DEVICEREADERROR;
	}

	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl  = DTR_CONTROL_DISABLE;
	if (dtr) dcb.fDtrControl = DTR_CONTROL_ENABLE;

	dcb.fOutxCtsFlow = 0;
	dcb.fRtsControl  = RTS_CONTROL_DISABLE;
	if (rts) dcb.fRtsControl = RTS_CONTROL_ENABLE;

	/* no software (Xon/Xof) flow control */
	dcb.fInX = dcb.fOutX = 0;

	if (SetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "WriteDevice in serial_setdtrrts");
		return ERR_DEVICEDTRRTSERROR;
	}

	/* the rest of function checks, if setting was really done */

	dcb.DCBlength = sizeof(DCB);
	GetCommState(d->hPhone, &dcb);

	dbgprintf("Serial device:");
	dbgprintf(" DTR is ");
	switch (dcb.fDtrControl) {
		case DTR_CONTROL_ENABLE    : dbgprintf("up");        break;
		case DTR_CONTROL_DISABLE   : dbgprintf("down");      break;
		case DTR_CONTROL_HANDSHAKE : dbgprintf("handshake"); break;
	}
	dbgprintf(", RTS is ");
	switch (dcb.fRtsControl) {
		case RTS_CONTROL_ENABLE    : dbgprintf("up");        break;
		case RTS_CONTROL_DISABLE   : dbgprintf("down");      break;
		case RTS_CONTROL_HANDSHAKE : dbgprintf("handshake"); break;
		case RTS_CONTROL_TOGGLE    : dbgprintf("toggle");    break;
	}
	dbgprintf("\n");
	if ( dtr && dcb.fDtrControl != DTR_CONTROL_ENABLE ) return ERR_DEVICEDTRRTSERROR;
	if (!dtr && dcb.fDtrControl != DTR_CONTROL_DISABLE) return ERR_DEVICEDTRRTSERROR;
	if ( rts && dcb.fRtsControl != RTS_CONTROL_ENABLE ) return ERR_DEVICEDTRRTSERROR;
	if (!rts && dcb.fRtsControl != RTS_CONTROL_DISABLE) return ERR_DEVICEDTRRTSERROR;

	return ERR_NONE;
}

static GSM_Error serial_setspeed(GSM_StateMachine *s, int speed)
{
	DCB			dcb;
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;

	dcb.DCBlength = sizeof(DCB);
	if (GetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "ReadDevice in serial_setspeed");
		return ERR_DEVICEREADERROR;
	}

	dcb.BaudRate = speed;

	if (SetCommState(d->hPhone, &dcb)==0) {
		GSM_OSErrorInfo(s, "WriteDevice in serial_setspeed");
		return ERR_DEVICECHANGESPEEDERROR;
	}

	return ERR_NONE;	
}

static int serial_read(GSM_StateMachine *s, void *buf, size_t nbytes)
{
	COMSTAT			ComStat;
	DWORD			ErrorFlags, Length;
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;

	/* Gets information about a communications error and
	 * current status of device
	 */
	ClearCommError(d->hPhone, &ErrorFlags, &ComStat);
	Length = ComStat.cbInQue;

	/* Nothing to read */
	if (Length <= 0) return Length;

	/* Read without problems */
	if (ReadFile(d->hPhone, buf, Length, &Length, &d->osRead)) return Length;

	if (GetLastError() != ERROR_IO_PENDING) {
		GSM_OSErrorInfo(s, "serial_read1");
		Length = 0;
		ClearCommError(d->hPhone, &ErrorFlags, &ComStat);
		return Length;
	}
			
	while(1) {
		if (GetOverlappedResult(d->hPhone,&d->osRead, &Length, TRUE)) break;
		if (GetLastError() != ERROR_IO_INCOMPLETE) {
			GSM_OSErrorInfo(s, "serial_read2");
			/* an error occurred, try to recover */
			ClearCommError(d->hPhone, &ErrorFlags, &ComStat);
			break;
		}
	}
	return Length;
}

static int serial_write(GSM_StateMachine *s, void *buf, size_t nbytes)
{
	DWORD			BytesWritten,ErrorFlags,BytesSent=0;
	COMSTAT			ComStat;
	GSM_Device_SerialData 	*d = &s->Device.Data.Serial;

	if (WriteFile(d->hPhone, buf, nbytes, &BytesSent, &d->osWrite)) return BytesSent;

	if (GetLastError() != ERROR_IO_PENDING) {
	    	GSM_OSErrorInfo(s, "serial_write1");
	    	ClearCommError(d->hPhone, &ErrorFlags, &ComStat);
	    	return BytesSent;
	}

	while (1) {
		if (GetOverlappedResult(d->hPhone, &d->osWrite, &BytesWritten, TRUE)) break;
		if (GetLastError() != ERROR_IO_INCOMPLETE) {
			GSM_OSErrorInfo(s, "serial_write2");
			ClearCommError(d->hPhone, &ErrorFlags, &ComStat);
			break;
		}
		BytesSent += BytesWritten;
	}
	BytesSent += BytesWritten;

	return BytesSent;
}

GSM_Device_Functions SerialDevice = {
	serial_open,
	serial_close,
	serial_setparity,
	serial_setdtrrts,
	serial_setspeed,
	serial_read,
	serial_write
};

#endif
#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
