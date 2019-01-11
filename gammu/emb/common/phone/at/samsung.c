/* Samsung-specific functions
 * Copyright (C) 2004 Claudio Matsuoka <cmatsuoka@gmail.com>
 * Tested with S300 only!
 */
                                           
#include "../../gsmstate.h"

#ifdef GSM_ENABLE_ATGEN

#include <string.h>
#include <time.h>
#include <ctype.h>

#include "../../misc/coding/coding.h"
#include "../../gsmcomon.h"
#include "../../service/sms/gsmsms.h"
#include "../pfunc.h"

#include "atgen.h"
#include "samsung.h"

/* Binary frame size */
#define BLKSZ 1024

struct ModelRes {
	char *model;
	int width;
	int height;
};

static struct ModelRes modres[] = {
	{ "S100", 128, 128 },
	{ "S200", 128, 113 },
	{ "S300", 128,  97 },
	{ "S500", 128, 128 },
	{ "T100", 128, 128 },
	{ "E700", 128, 128 },
	{ NULL, 0, 0 }
};

/*
 * CRC functions from the Granch SBNI12 Linux driver by
 * Denis I. Timofeev <timofeev@granch.ru>
 */
static unsigned int crc32tab[] = {
	0xD202EF8D,  0xA505DF1B,  0x3C0C8EA1,  0x4B0BBE37,
	0xD56F2B94,  0xA2681B02,  0x3B614AB8,  0x4C667A2E,
	0xDCD967BF,  0xABDE5729,  0x32D70693,  0x45D03605,
	0xDBB4A3A6,  0xACB39330,  0x35BAC28A,  0x42BDF21C,
	0xCFB5FFE9,  0xB8B2CF7F,  0x21BB9EC5,  0x56BCAE53,
	0xC8D83BF0,  0xBFDF0B66,  0x26D65ADC,  0x51D16A4A,
	0xC16E77DB,  0xB669474D,  0x2F6016F7,  0x58672661,
	0xC603B3C2,  0xB1048354,  0x280DD2EE,  0x5F0AE278,
	0xE96CCF45,  0x9E6BFFD3,  0x0762AE69,  0x70659EFF,
	0xEE010B5C,  0x99063BCA,  0x000F6A70,  0x77085AE6,
	0xE7B74777,  0x90B077E1,  0x09B9265B,  0x7EBE16CD,
	0xE0DA836E,  0x97DDB3F8,  0x0ED4E242,  0x79D3D2D4,
	0xF4DBDF21,  0x83DCEFB7,  0x1AD5BE0D,  0x6DD28E9B,
	0xF3B61B38,  0x84B12BAE,  0x1DB87A14,  0x6ABF4A82,
	0xFA005713,  0x8D076785,  0x140E363F,  0x630906A9,
	0xFD6D930A,  0x8A6AA39C,  0x1363F226,  0x6464C2B0,
	0xA4DEAE1D,  0xD3D99E8B,  0x4AD0CF31,  0x3DD7FFA7,
	0xA3B36A04,  0xD4B45A92,  0x4DBD0B28,  0x3ABA3BBE,
	0xAA05262F,  0xDD0216B9,  0x440B4703,  0x330C7795,
	0xAD68E236,  0xDA6FD2A0,  0x4366831A,  0x3461B38C,
	0xB969BE79,  0xCE6E8EEF,  0x5767DF55,  0x2060EFC3,
	0xBE047A60,  0xC9034AF6,  0x500A1B4C,  0x270D2BDA,
	0xB7B2364B,  0xC0B506DD,  0x59BC5767,  0x2EBB67F1,
	0xB0DFF252,  0xC7D8C2C4,  0x5ED1937E,  0x29D6A3E8,
	0x9FB08ED5,  0xE8B7BE43,  0x71BEEFF9,  0x06B9DF6F,
	0x98DD4ACC,  0xEFDA7A5A,  0x76D32BE0,  0x01D41B76,
	0x916B06E7,  0xE66C3671,  0x7F6567CB,  0x0862575D,
	0x9606C2FE,  0xE101F268,  0x7808A3D2,  0x0F0F9344,
	0x82079EB1,  0xF500AE27,  0x6C09FF9D,  0x1B0ECF0B,
	0x856A5AA8,  0xF26D6A3E,  0x6B643B84,  0x1C630B12,
	0x8CDC1683,  0xFBDB2615,  0x62D277AF,  0x15D54739,
	0x8BB1D29A,  0xFCB6E20C,  0x65BFB3B6,  0x12B88320,
	0x3FBA6CAD,  0x48BD5C3B,  0xD1B40D81,  0xA6B33D17,
	0x38D7A8B4,  0x4FD09822,  0xD6D9C998,  0xA1DEF90E,
	0x3161E49F,  0x4666D409,  0xDF6F85B3,  0xA868B525,
	0x360C2086,  0x410B1010,  0xD80241AA,  0xAF05713C,
	0x220D7CC9,  0x550A4C5F,  0xCC031DE5,  0xBB042D73,
	0x2560B8D0,  0x52678846,  0xCB6ED9FC,  0xBC69E96A,
	0x2CD6F4FB,  0x5BD1C46D,  0xC2D895D7,  0xB5DFA541,
	0x2BBB30E2,  0x5CBC0074,  0xC5B551CE,  0xB2B26158,
	0x04D44C65,  0x73D37CF3,  0xEADA2D49,  0x9DDD1DDF,
	0x03B9887C,  0x74BEB8EA,  0xEDB7E950,  0x9AB0D9C6,
	0x0A0FC457,  0x7D08F4C1,  0xE401A57B,  0x930695ED,
	0x0D62004E,  0x7A6530D8,  0xE36C6162,  0x946B51F4,
	0x19635C01,  0x6E646C97,  0xF76D3D2D,  0x806A0DBB,
	0x1E0E9818,  0x6909A88E,  0xF000F934,  0x8707C9A2,
	0x17B8D433,  0x60BFE4A5,  0xF9B6B51F,  0x8EB18589,
	0x10D5102A,  0x67D220BC,  0xFEDB7106,  0x89DC4190,
	0x49662D3D,  0x3E611DAB,  0xA7684C11,  0xD06F7C87,
	0x4E0BE924,  0x390CD9B2,  0xA0058808,  0xD702B89E,
	0x47BDA50F,  0x30BA9599,  0xA9B3C423,  0xDEB4F4B5,
	0x40D06116,  0x37D75180,  0xAEDE003A,  0xD9D930AC,
	0x54D13D59,  0x23D60DCF,  0xBADF5C75,  0xCDD86CE3,
	0x53BCF940,  0x24BBC9D6,  0xBDB2986C,  0xCAB5A8FA,
	0x5A0AB56B,  0x2D0D85FD,  0xB404D447,  0xC303E4D1,
	0x5D677172,  0x2A6041E4,  0xB369105E,  0xC46E20C8,
	0x72080DF5,  0x050F3D63,  0x9C066CD9,  0xEB015C4F,
	0x7565C9EC,  0x0262F97A,  0x9B6BA8C0,  0xEC6C9856,
	0x7CD385C7,  0x0BD4B551,  0x92DDE4EB,  0xE5DAD47D,
	0x7BBE41DE,  0x0CB97148,  0x95B020F2,  0xE2B71064,
	0x6FBF1D91,  0x18B82D07,  0x81B17CBD,  0xF6B64C2B,
	0x68D2D988,  0x1FD5E91E,  0x86DCB8A4,  0xF1DB8832,
	0x616495A3,  0x1663A535,  0x8F6AF48F,  0xF86DC419,
	0x660951BA,  0x110E612C,  0x88073096,  0xFF000000
};

static unsigned int GetCRC(char *data, int size)
{
	unsigned int crc = 0;

	while (size--)
		crc = crc32tab[(crc ^ *data++) & 0xff] ^ ((crc >> 8) & 0x00FFFFFF);

	return crc;
}

/*
 * Frame transfer
 */

static GSM_Error WaitFor(GSM_StateMachine *s, char *t, int ttl)
{
	char 		readbuf[100];
	int 		n;
	unsigned int 	sec;
        GSM_DateTime    Date;

        GSM_GetCurrentDateTime (&Date);
        sec = Date.Second;

	n = s->Device.Functions->ReadDevice(s, readbuf, 80);
	readbuf[n] = 0;
	while (strstr(readbuf, t) == NULL && (sec + ttl) >= Date.Second) {
		my_sleep(5000);
		n = s->Device.Functions->ReadDevice(s, readbuf, 80);
		readbuf[n] = 0;
        	GSM_GetCurrentDateTime (&Date);
	}

	return (sec + ttl) >= Date.Second ? ERR_NONE : ERR_TIMEOUT;
}

static GSM_Error SetSamsungFrame(GSM_StateMachine *s, unsigned char *buff, int size, GSM_Phone_RequestID id)
{
	GSM_Phone_Data		*Phone = &s->Phone.Data;
	GSM_Error		error;
	int			i, count;

	count = size / BLKSZ;
	
	for (i = 0; i < count; i++) {
		error = WaitFor(s, ">", 4);
 		if (error!=ERR_NONE) return error;

 		error = s->Protocol.Functions->WriteMessage(s,
			buff + i * BLKSZ, BLKSZ, 0x00);
 		if (error!=ERR_NONE) return error;
	}

	error = WaitFor(s, ">", 4);
 	if (error!=ERR_NONE) return error;
	error = s->Protocol.Functions->WriteMessage(s,
		buff + i * BLKSZ, size%BLKSZ, 0x00);
	if (error!=ERR_NONE) return error;

	error = GSM_WaitFor(s, "", 0, 0x00, 4, id);
	if (error!=ERR_NONE) return error;

	return Phone->DispatchError;
}

/* Answer format for binary data transfer
 *
 * SDNDCRC = 0xa : RECEIVECRC = 0xcbf53a1c : BINSIZE = 5
 * CRCERR
 */
static GSM_Error ReplySetSamsungFrame(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	unsigned long 		txcrc, rxcrc;
	int 			binsize;
	char 			*pos;

	/* Parse SDNDCRC */
	pos = strchr(msg.Buffer, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	txcrc = strtoul(pos, NULL, 0);
	smprintf(s, "Sent CRC     : 0x%lx\n", txcrc);

	/* Parse RECEIVECRC */
	pos = strchr(pos, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	rxcrc = strtoul(pos, NULL, 0);
	smprintf(s, "Reveived CRC : 0x%lx\n", rxcrc);

	/* Parse BINSIZE */
	pos = strchr(pos, '=');
	if (!pos) return ERR_UNKNOWN;
	pos++;
	binsize = strtoul(pos, NULL, 0);
	smprintf(s, "Binary size  : %d\n", binsize);

	return txcrc == rxcrc ? ERR_NONE : ERR_WRONGCRC;
}
  
/*
 * Bitmaps
 */

GSM_Error SAMSUNG_ReplyGetBitmap(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;
	unsigned char 		buffer[32];
	char 			*pos;
	int			location, count;
 
 	switch (Priv->ReplyState) {
 	case AT_Reply_OK:
		smprintf(s, "Bitmap info received\n");
		/* Parse +IMGR:location,name,0,0,0,0 */

		/* Parse location */
		pos = strchr(msg.Buffer, ':');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		location = atoi(pos);
		smprintf(s, "Location : %d\n", location);

		/* Parse name */
		pos = strchr(pos, '"');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		for (count = 0; count < 31; count++) {
			if (pos[count] == '"')
				break;
			buffer[count] = pos[count];
		}
		buffer[count] = 0;
		smprintf(s, "Name     : %s\n", buffer);
		s->Phone.Data.Bitmap->Name = malloc((strlen(buffer) + 1) * 2);
		if (s->Phone.Data.Bitmap->Name == NULL)
			return ERR_MOREMEMORY;
		EncodeUnicode(s->Phone.Data.Bitmap->Name, buffer, strlen(buffer));

		s->Phone.Data.Bitmap->Location = location;

		return ERR_NONE;
	case AT_Reply_Error:
		return ERR_UNKNOWN;
	case AT_Reply_CMSError:
	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
	        return ATGEN_HandleCMEError(s);
 	default:
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error SAMSUNG_ReplySetBitmap(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Bitmap sent\n");
	return ReplySetSamsungFrame(msg, s); 
}

GSM_Error SAMSUNG_GetBitmap(GSM_StateMachine *s, GSM_Bitmap *Bitmap)
{
	unsigned char req[100];

	s->Phone.Data.Bitmap=Bitmap;
	smprintf(s, "Getting bitmap\n");
	sprintf(req, "AT+IMGR=%d\r", Bitmap->Location-1);
	return GSM_WaitFor (s, req, strlen(req), 0x00, 4, ID_GetBitmap);
}

GSM_Error SAMSUNG_SetBitmap(GSM_StateMachine *s, GSM_Bitmap *Bitmap)
{
	unsigned char	req[100];
	unsigned long	crc;
	GSM_Error	error;
	char		name[50], *dot, *model;
	GSM_Phone_Data  *Data = &s->Phone.Data;
	int 		i;

	s->Phone.Data.Bitmap = Bitmap;
	smprintf(s, "Setting bitmap\n");

	if (Bitmap->Type != GSM_PictureBinary) {
		smprintf(s, "Invalid picture type\n");
		return ERR_INVALIDDATA;
	}

	if (Bitmap->BinaryPic.Type != PICTURE_GIF) {
		smprintf(s, "Invalid binary picture type\n");
		return ERR_INVALIDDATA;
	}

	/* Check if picture size matches phone model */
	model = GetModelData(NULL,Data->Model,NULL)->model;
	smprintf(s, "Checking picture size for %s\n", model);
	for (i = 0; modres[i].model; i++) {
		if (!strcmp(model, modres[i].model)) {
			if (Bitmap->BitmapWidth != modres[i].width ||
			    Bitmap->BitmapHeight != modres[i].height) {
				smprintf(s, "Model %s must use %d x %d picture size\n",
					modres[i].model, modres[i].width,
					modres[i].height);
				return ERR_INVALIDDATA;
			}
			break;
		}
	}
	if (modres[i].model == NULL) {
		smprintf(s, "Model \"%s\" is not supported.\n", Data->Model);
		return ERR_NOTSUPPORTED;
	}

	crc = GetCRC(Bitmap->BinaryPic.Buffer, Bitmap->BinaryPic.Length);

	/* Remove extension from file name */
	strncpy(name, DecodeUnicodeString(Bitmap->Name), 50);
	if ((dot = strrchr(name, '.')) != NULL)
		*dot = 0;

	sprintf(req, "AT+IMGW=0,\"%s\",2,0,0,0,0,100,%d,%u\r", name,
		Bitmap->BinaryPic.Length, (unsigned int)crc);

	error = s->Protocol.Functions->WriteMessage(s, req, strlen(req), 0x00);
	if (error!=ERR_NONE) return error;

	return SetSamsungFrame(s, Bitmap->BinaryPic.Buffer,
		Bitmap->BinaryPic.Length, ID_SetBitmap);
}

/*
 * Ringtones
 */

GSM_Error SAMSUNG_ReplyGetRingtone(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
 	GSM_Phone_ATGENData 	*Priv = &s->Phone.Data.Priv.ATGEN;
	unsigned char 		buffer[32];
	char 			*pos;
	int			location, length, count;
 
 	switch (Priv->ReplyState) {
 	case AT_Reply_OK:
		smprintf(s, "Ringtone info received\n");
		/* Parse +MELR:location,name,size */

		/* Parse location */
		pos = strchr(msg.Buffer, ':');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		location = atoi(pos);
		smprintf(s, "Location : %d\n", location);

		/* Parse name */
		pos = strchr(pos, '"');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		/* Ringtone.Name size is 20 chars */
		for (count = 0; count < 19; count++) {
			if (pos[count] == '"')
				break;
			buffer[count] = pos[count];
		}
		buffer[count] = 0;
		smprintf(s, "Name     : %s\n", buffer);
		EncodeUnicode(s->Phone.Data.Ringtone->Name,buffer,strlen(buffer));

		/* Parse ringtone length */
		pos = strchr(pos, ',');
		if (!pos) return ERR_UNKNOWN;
		pos++;
		length = atoi(pos);
		smprintf(s, "Length   : %d\n", length);

		/* S300 ringtones are always MMF */
		s->Phone.Data.Ringtone->Format = RING_MMF;
		s->Phone.Data.Ringtone->Location = location;
		s->Phone.Data.Ringtone->BinaryTone.Length = length;

		return ERR_NONE;
	case AT_Reply_Error:
		return ERR_UNKNOWN;
	case AT_Reply_CMSError:
	        return ATGEN_HandleCMSError(s);
	case AT_Reply_CMEError:
	        return ATGEN_HandleCMEError(s);
 	default:
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error SAMSUNG_GetRingtone(GSM_StateMachine *s, GSM_Ringtone *Ringtone, bool PhoneRingtone)
{
	unsigned char req[100];

	s->Phone.Data.Ringtone = Ringtone;
	smprintf(s, "Getting ringtone\n");
	sprintf(req, "AT+MELR=%d\r", Ringtone->Location-1);
	return GSM_WaitFor (s, req, strlen(req), 0x00, 4, ID_GetRingtone);
}

GSM_Error SAMSUNG_ReplySetRingtone(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Ringtone sent\n");
	return ReplySetSamsungFrame(msg, s); 
}
  
GSM_Error SAMSUNG_SetRingtone(GSM_StateMachine *s, GSM_Ringtone *Ringtone, int *maxlength)
{
	unsigned char	req[100];
	unsigned long	crc;
	GSM_Error	error;
	char		name[50], *dot;

	s->Phone.Data.Ringtone = Ringtone;
	smprintf(s, "Setting ringtone\n");

	if (Ringtone->Format != RING_MMF) {
		smprintf(s, "Not MMF ringtone\n");
		return ERR_INVALIDDATA;
	}

	/* Remove extension from file name */
	strncpy(name, DecodeUnicodeString(Ringtone->Name), 50);
	if ((dot = strrchr(name, '.')) != NULL) *dot = 0;

	crc = GetCRC(Ringtone->BinaryTone.Buffer, Ringtone->BinaryTone.Length);
	sprintf(req, "AT+MELW=0,\"%s\",4,%d,%u\r", name,
		Ringtone->BinaryTone.Length, (unsigned int)crc);

	error = s->Protocol.Functions->WriteMessage(s, req, strlen(req), 0x00);
	if (error!=ERR_NONE) return error;

	return SetSamsungFrame(s, Ringtone->BinaryTone.Buffer,
		Ringtone->BinaryTone.Length, ID_SetRingtone);
}

#endif
