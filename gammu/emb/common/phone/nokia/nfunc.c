/* (c) 2002-2004 by Marcin Wiacek */
/* based on some work from Ralf Thelen, Gabriele Zappi and MyGnokii */

#include <string.h> /* memcpy only */
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "../../gsmstate.h"
#include "../../misc/coding/coding.h"
#include "../../service/sms/gsmsms.h"
#include "../pfunc.h"
#include "nfunc.h"

unsigned char N71_65_MEMORY_TYPES[] = {
	MEM_DC,		0x01,
	MEM_MC,		0x02,
	MEM_RC,		0x03,
	MEM_ME,		0x05,
	MEM_SM,		0x06,
	MEM_VM,		0x09,
	MEM7110_SP,	0x0e,
	MEM7110_CG,	0x10,
	MEM_ON,		0x17,
	  0x00,		0x00
};

int N71_65_PackPBKBlock(GSM_StateMachine *s, int id, int size, int no, unsigned char *buf, unsigned char *block)
{
	smprintf(s, "Packing phonebook block with ID = %i, block number = %i, block length = %i\n",id,no+1,size+6);

	block[0] 	= id;
	block[1] 	= 0;
	block[2] 	= 0;
	block[3] 	= size + 6;
	block[4] 	= no + 1;
	memcpy(block+5, buf, size);
	block[5+size] 	= 0;

	return (size + 6);
}

int N71_65_EncodePhonebookFrame(GSM_StateMachine *s, unsigned char *req, GSM_MemoryEntry entry, int *block2, bool DCT4, bool VoiceTag)
{
	int		count=0, len, i, block=0, j;
	char		string[500];
	unsigned char	type;
 		
	for (i = 0; i < entry.EntriesNum; i++) {
		type = 0;
		if (entry.Entries[i].EntryType == PBK_Number_General) type = N7110_PBK_NUMBER_GENERAL;
		if (entry.Entries[i].EntryType == PBK_Number_Mobile)  type = N7110_PBK_NUMBER_MOBILE;
		if (entry.Entries[i].EntryType == PBK_Number_Work)    type = N7110_PBK_NUMBER_WORK;
		if (entry.Entries[i].EntryType == PBK_Number_Fax)     type = N7110_PBK_NUMBER_FAX;
		if (entry.Entries[i].EntryType == PBK_Number_Home)    type = N7110_PBK_NUMBER_HOME;
		if (type != 0) {
			string[0] = type;
			len = UnicodeLength(entry.Entries[i].Text);

			string[1] = 0;
			string[2] = 0;

			/* DCT 3 */
			if (!DCT4) string[2] = entry.Entries[i].VoiceTag;

			string[3] = 0;
			string[4] = len*2+2;
			CopyUnicodeString(string+5,entry.Entries[i].Text);
			string[len * 2 + 5] = 0;
			count += N71_65_PackPBKBlock(s, N7110_PBK_NUMBER, len*2+6, block++, string, req+count);

			/* DCT 4 */
			if (DCT4 && VoiceTag) {
				block++;
				req[count++] = N6510_PBK_VOICETAG_ID;
				req[count++] = 0;
				req[count++] = 0;
				req[count++] = 8;
				req[count++] = 0x00;
				req[count++] = i+1;
				req[count++] = 0x00;
				req[count++] = entry.Entries[i].VoiceTag;
			}
			if (DCT4) {
				j = 0;
				while (entry.Entries[i].SMSList[j] != 0) {
					string[0] = i+1;
					string[1] = 0x00;
					string[2] = 0x02;
					string[3] = 0x00;
					string[4] = entry.Entries[i].SMSList[j];
					string[5] = 0x00;
					count += N71_65_PackPBKBlock(s, N6510_PBK_SMSLIST_ID, 6, block++, string, req+count);

					j++;
				}
			}
			continue;
		}
		if (entry.Entries[i].EntryType == PBK_Text_Note)   type = N7110_PBK_NOTE;
		if (entry.Entries[i].EntryType == PBK_Text_Postal) type = N7110_PBK_POSTAL;
		if (entry.Entries[i].EntryType == PBK_Text_Email)  type = N7110_PBK_EMAIL;
		if (entry.Entries[i].EntryType == PBK_Text_Email2) type = N7110_PBK_EMAIL;
		if (entry.Entries[i].EntryType == PBK_Text_Name)   type = N7110_PBK_NAME;
		if (entry.Entries[i].EntryType == PBK_Text_URL) {
			type = N7110_PBK_NOTE;
			if (DCT4) type = N6510_PBK_URL;
		}
		if (type != 0) {
			len = UnicodeLength(entry.Entries[i].Text);
			string[0] = len*2+2;
			CopyUnicodeString(string+1,entry.Entries[i].Text);
			string[len*2+1] = 0;
			count += N71_65_PackPBKBlock(s, type, len * 2 + 2, block++, string, req + count);
			continue;

		}
		if (entry.Entries[i].EntryType == PBK_Caller_Group) {
			if (!IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_PBK35)) {
				string[0] = entry.Entries[i].Number;
				string[1] = 0;
				count += N71_65_PackPBKBlock(s, N7110_PBK_GROUP, 2, block++, string, req + count);
			}
			continue;
		}
		if (entry.Entries[i].EntryType == PBK_RingtoneID) {
			if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_PBK35)) {
				string[0] = 0x00; 
				string[1] = 0x00;
				string[2] = entry.Entries[i].Number;
				count += N71_65_PackPBKBlock(s, N7110_PBK_RINGTONE_ID, 3, block++, string, req + count);
				count --;
				req[count-5] = 8;
			}
			continue;
		}
		if (entry.Entries[i].EntryType == PBK_PictureID) {
			if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_PBKIMG)) {
				string[0] = 0x00; 
				string[1] = 0x00;
				string[2] = 0x00;
				string[3] = 0x00;
				string[4] = 0x01;
				string[5] = entry.Entries[i].Number / 256;
				string[6] = entry.Entries[i].Number % 256;
				string[7] = 0x00;
				string[8] = 0x00;
				string[9] = 0x00;
				count += N71_65_PackPBKBlock(s, N6510_PBK_PICTURE_ID, 10, block++, string, req + count);
				req[count-1] = 0x01;
			}
			continue;
		}
		if (entry.Entries[i].EntryType == PBK_Text_UserID) {
			if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_PBKUSER)) {
				string[0] = UnicodeLength(entry.Entries[i].Text)*2;
				CopyUnicodeString(string+1,entry.Entries[i].Text);
				count += N71_65_PackPBKBlock(s, N6510_PBK_USER_ID, string[0]+2, block++, string, req+count);
				req[count-1]--;
			}
			continue;
		}
	}

	*block2=block;

	return count;
}

GSM_Error N71_65_DecodePhonebook(GSM_StateMachine	*s,
				 GSM_MemoryEntry 	*entry,
				 GSM_Bitmap 		*bitmap,
				 GSM_SpeedDial 		*speed,
				 unsigned char 		*MessageBuffer,
				 int 			MessageLength,
				 bool			DayMonthReverse)
{
	unsigned char 				*Block;
	int					length = 0, i;
	GSM_71_65_Phonebook_Entries_Types	Type;           

	entry->EntriesNum 	= 0;

	if (entry->MemoryType==MEM7110_CG) {
		bitmap->Text[0] 	= 0x00;
		bitmap->Text[1] 	= 0x00;
		bitmap->DefaultBitmap 	= true;
		bitmap->DefaultRingtone = true;
	}

	Block = &MessageBuffer[0];
	while (length != MessageLength) {
#ifdef DEBUG
		smprintf(s, "Phonebook entry block - length %i", Block[3]-6);
		if (di.dl == DL_TEXTALL || di.dl == DL_TEXTALLDATE) DumpMessage(di.df, di.dl, Block+5, Block[3]-6);
#endif
		if (entry->EntriesNum==GSM_PHONEBOOK_ENTRIES) {
			smprintf(s, "Too many entries\n");
			return ERR_UNKNOWNRESPONSE;
		}

		Type = 0;
		if (Block[0] == N7110_PBK_NAME) {
			Type = PBK_Text_Name;   smprintf(s,"Name ");
		}
		if (Block[0] == N7110_PBK_EMAIL) {
			Type = PBK_Text_Email;  smprintf(s,"Email ");
		}
		if (Block[0] == N7110_PBK_POSTAL) {
			Type = PBK_Text_Postal; smprintf(s,"Postal ");
		}
		if (Block[0] == N7110_PBK_NOTE) {
			Type = PBK_Text_Note;   smprintf(s,"Text note ");
		}
		if (Block[0] == N6510_PBK_URL) {
			Type = PBK_Text_URL;    smprintf(s,"URL ");
		}
		if (Type != 0) {
			if (Block[5]/2>GSM_PHONEBOOK_TEXT_LENGTH) {
				smprintf(s, "Too long text\n");
				return ERR_UNKNOWNRESPONSE;
			}
			memcpy(entry->Entries[entry->EntriesNum].Text,Block+6,Block[5]);
			entry->Entries[entry->EntriesNum].EntryType=Type;
			smprintf(s, " \"%s\"\n",DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
			if (Block[0] == N7110_PBK_NAME) {
				if (entry->MemoryType==MEM7110_CG) {
					memcpy(bitmap->Text,Block+6,Block[5]);
				}
			}
			entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;
		}

		if (Block[0] == N7110_PBK_DATETIME) {
			entry->Entries[entry->EntriesNum].EntryType=PBK_Date;
			NOKIA_DecodeDateTime(s, Block+6, &entry->Entries[entry->EntriesNum].Date);
			if (DayMonthReverse) {
				i = entry->Entries[entry->EntriesNum].Date.Month;
				entry->Entries[entry->EntriesNum].Date.Month = entry->Entries[entry->EntriesNum].Date.Day;
				entry->Entries[entry->EntriesNum].Date.Day   = i;
			}
			entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;
		}
		if (Block[0] == N6510_PBK_PICTURE_ID) {
			entry->Entries[entry->EntriesNum].EntryType=PBK_PictureID;
			smprintf(s, "Picture ID \"%i\"\n",Block[10]*256+Block[11]);
			entry->Entries[entry->EntriesNum].Number=Block[10]*256+Block[11];
			entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}

		if (Block[0] == N7110_PBK_NUMBER) {
			if (Block[5] == 0x00) {
				Type = PBK_Number_General;    smprintf(s,"General number ");
			}
			/* Not assigned dialed number */
			if (Block[5] == 0x01) {
				Type = PBK_Number_General;    smprintf(s,"General number ");
			}
			if (Block[5] == 0x0B) {
				Type = PBK_Number_General;    smprintf(s,"General number ");
			}
			/* In many firmwares 0x55 visible after using
			 * Save from Call Register menu and saving number
			 * to existing phonebook entry */
			if (Block[5] == 0x55) {
				Type = PBK_Number_General;    smprintf(s,"General number ");
			}
			if (Block[5] == N7110_PBK_NUMBER_GENERAL) {
				Type = PBK_Number_General;    smprintf(s,"General number ");
			}
			if (Block[5] == N7110_PBK_NUMBER_WORK) {
				Type = PBK_Number_Work;       smprintf(s,"Work number ");
			}
			if (Block[5] == N7110_PBK_NUMBER_FAX) {
				Type = PBK_Number_Fax;        smprintf(s,"Fax number ");
			}
			if (Block[5] == N7110_PBK_NUMBER_MOBILE) {
				Type = PBK_Number_Mobile;     smprintf(s,"Mobile number ");
			}
			if (Block[5] == N7110_PBK_NUMBER_HOME) {
				Type = PBK_Number_Home;       smprintf(s,"Home number ");
			}
			if (Type == 0x00) {
				smprintf(s, "Unknown number type %02x\n",Block[5]);
				return ERR_UNKNOWNRESPONSE;
			}
			entry->Entries[entry->EntriesNum].EntryType=Type;
			if (Block[9]/2>GSM_PHONEBOOK_TEXT_LENGTH) {
				smprintf(s, "Too long text\n");
				return ERR_UNKNOWNRESPONSE;
			}
			memcpy(entry->Entries[entry->EntriesNum].Text,Block+10,Block[9]);
			smprintf(s, " \"%s\"\n",DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
			/* DCT3 phones like 6210 */
			entry->Entries[entry->EntriesNum].VoiceTag = Block[7];
#ifdef DEBUG
			if (entry->Entries[entry->EntriesNum].VoiceTag != 0) smprintf(s, "Voice tag %i assigned\n",Block[7]);
#endif
			entry->Entries[entry->EntriesNum].SMSList[0] = 0;
			entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N7110_PBK_RINGTONE_ID) {
			if (entry->MemoryType==MEM7110_CG) {
				bitmap->RingtoneID=Block[5];
				if (Block[5] == 0x00) bitmap->RingtoneID=Block[7];
				smprintf(s, "Ringtone ID : %i\n",bitmap->RingtoneID);
				bitmap->DefaultRingtone 	= false;
				bitmap->FileSystemRingtone 	= false;
			} else {
				entry->Entries[entry->EntriesNum].EntryType=PBK_RingtoneID;
				smprintf(s, "Ringtone ID \"%i\"\n",Block[7]);
				entry->Entries[entry->EntriesNum].Number=Block[7];
				entry->EntriesNum ++;
			}

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N7110_PBK_LOGOON) {
			if (entry->MemoryType==MEM7110_CG) {
				bitmap->BitmapEnabled=(Block[5]==0x00 ? false : true);
				smprintf(s, "Logo : %s\n", bitmap->BitmapEnabled==true ? "enabled":"disabled");
			} else {
				return ERR_UNKNOWNRESPONSE;
			}

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N7110_PBK_GROUPLOGO) {
			if (entry->MemoryType==MEM7110_CG) {
				smprintf(s, "Caller logo\n");
				PHONE_DecodeBitmap(GSM_NokiaCallerLogo, Block+10, bitmap);
				bitmap->DefaultBitmap = false;
			} else {
				return ERR_UNKNOWNRESPONSE;
			}

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N7110_PBK_GROUP) {
			entry->Entries[entry->EntriesNum].EntryType=PBK_Caller_Group;
			smprintf(s, "Caller group \"%i\"\n",Block[5]);
			entry->Entries[entry->EntriesNum].Number=Block[5];
			if (Block[5]!=0) entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N6510_PBK_VOICETAG_ID) {
			smprintf(s, "Entry %i has voice tag %i\n",Block[5]-1,Block[7]);
			entry->Entries[Block[5]-1].VoiceTag = Block[7];

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}

		/* 6210 5.56, SIM speed dials or ME with 1 number */
		if (Block[0] == N7110_PBK_SIM_SPEEDDIAL) {
			if (entry->MemoryType==MEM7110_SP) {
#ifdef DEBUG
				smprintf(s, "location %i\n",(Block[6]*256+Block[7]));
#endif			
				speed->MemoryType = MEM_ME;
				if (Block[8] == 0x06) speed->MemoryType = MEM_SM;
				speed->MemoryLocation 	= (Block[6]*256+Block[7]);
				speed->MemoryNumberID 	= 2;
			} else {
				return ERR_UNKNOWNRESPONSE;
			}

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}

		if (Block[0] == N7110_PBK_SPEEDDIAL) {
			if (entry->MemoryType==MEM7110_SP) {
#ifdef DEBUG
				switch (Block[12]) {
					case 0x05: smprintf(s, "ME\n"); break;
					case 0x06: smprintf(s, "SM\n"); break;
					default	 : smprintf(s, "%02x\n",Block[12]);
				}
				smprintf(s, "location %i, number %i in location\n",
					(Block[6]*256+Block[7])-1,Block[14]);
#endif			
				switch (Block[12]) {
					case 0x05: speed->MemoryType = MEM_ME; break;
					case 0x06: speed->MemoryType = MEM_SM; break;
				}
				speed->MemoryLocation = (Block[6]*256+Block[7])-1;
				speed->MemoryNumberID = Block[14];
			} else {
				return ERR_UNKNOWNRESPONSE;
			}

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N6510_PBK_RINGTONEFILE_ID) {
			smprintf(s, "Ringtone ID with possibility of using filesystem\n");
			if (entry->MemoryType==MEM7110_CG) {
				if (Block[9] == 0x01) {
					smprintf(s, "Filesystem ringtone ID: %02x\n",Block[10]*256+Block[11]);
					bitmap->FileSystemRingtone = true;
				} else {
					smprintf(s, "Internal ringtone ID: %02x\n",Block[10]*256+Block[11]);
					bitmap->FileSystemRingtone = false;
				}
				bitmap->RingtoneID	= Block[10]*256+Block[11];
				bitmap->DefaultRingtone = false;
			} else {
				return ERR_UNKNOWNRESPONSE;
			}
			
			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;						
		}
		if (Block[0] == N6510_PBK_SMSLIST_ID) {
			smprintf(s, "Entry %i is assigned to SMS list %i\n",Block[5]-1,Block[9]);
			i = 0;
			while(entry->Entries[Block[5]-1].SMSList[i] != 0) i++;
			entry->Entries[Block[5]-1].SMSList[i+1] = 0;
			entry->Entries[Block[5]-1].SMSList[i]   = Block[9];

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;
		}
		if (Block[0] == N6510_PBK_USER_ID) {
			smprintf(s, "User ID:");
			entry->Entries[entry->EntriesNum].EntryType=PBK_Text_UserID;
			if (Block[5]/2>GSM_PHONEBOOK_TEXT_LENGTH) {
				smprintf(s, "Too long text\n");
				return ERR_UNKNOWNRESPONSE;
			}
			memcpy(entry->Entries[entry->EntriesNum].Text,Block+6,Block[5]);
			smprintf(s, " \"%s\"\n",DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
			entry->EntriesNum ++;

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}
		if (Block[0] == N7110_PBK_UNKNOWN1) {
			smprintf(s,"Unknown entry\n");

			length = length + Block[3];
			Block  = &Block[(int) Block[3]];
			continue;			
		}

		smprintf(s, "ERROR: unknown pbk entry 0x%02x\n",Block[0]);
		return ERR_UNKNOWNRESPONSE;
	}

	if (entry->EntriesNum == 0) return ERR_EMPTY;

	return ERR_NONE;
}

void NOKIA_GetDefaultCallerGroupName(GSM_StateMachine *s, GSM_Bitmap *Bitmap)
{
	Bitmap->DefaultName = false;
	if (Bitmap->Text[0]==0x00 && Bitmap->Text[1]==0x00) {
		Bitmap->DefaultName = true;
		switch(Bitmap->Location) {
		case 1: EncodeUnicode(Bitmap->Text,GetMsg(s->msg,"Family"),strlen(GetMsg(s->msg,"Family")));
			break;
		case 2: EncodeUnicode(Bitmap->Text,GetMsg(s->msg,"VIP"),strlen(GetMsg(s->msg,"VIP")));
			break;
		case 3: EncodeUnicode(Bitmap->Text,GetMsg(s->msg,"Friends"),strlen(GetMsg(s->msg,"Friends")));
			break;
		case 4: EncodeUnicode(Bitmap->Text,GetMsg(s->msg,"Colleagues"),strlen(GetMsg(s->msg,"Colleagues")));
			break;
		case 5: EncodeUnicode(Bitmap->Text,GetMsg(s->msg,"Other"),strlen(GetMsg(s->msg,"Other")));
			break;
		}
	}
}

void NOKIA_DecodeDateTime(GSM_StateMachine *s, unsigned char* buffer, GSM_DateTime *datetime)
{
	datetime->Year	= buffer[0] * 256 + buffer[1];
	datetime->Month	= buffer[2];
	datetime->Day	= buffer[3];

	datetime->Hour	 = buffer[4];
	datetime->Minute = buffer[5];
	datetime->Second = buffer[6];

	smprintf(s, "Decoding date and time\n");
	smprintf(s, "   Time: %02d:%02d:%02d\n",
		datetime->Hour, datetime->Minute, datetime->Second);
	smprintf(s, "   Date: %4d/%02d/%02d\n",
		datetime->Year, datetime->Month, datetime->Day);
}

#if defined(GSM_ENABLE_NOKIA_DCT3) || defined(GSM_ENABLE_NOKIA_DCT4)

/* --------------------- Some general Nokia functions ---------------------- */

void NOKIA_DecodeSMSState(GSM_StateMachine *s, unsigned char state, GSM_SMSMessage *sms)
{
	switch (state) {
		case 0x01 : sms->State = SMS_Read;   break;
		case 0x03 : sms->State = SMS_UnRead; break;
		case 0x05 : sms->State = SMS_Sent;   break;
		case 0x07 : sms->State = SMS_UnSent; break;
		default	  : smprintf(s, "Unknown SMS state: %02x\n",state);
	}
}

GSM_Error NOKIA_ReplyGetPhoneString(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	strcpy(s->Phone.Data.PhoneString, msg.Buffer+s->Phone.Data.StartPhoneString);
	return ERR_NONE;
}

/* Some strings are very easy. Some header, after it required string and 0x00.
 * We can get them using this function. We give frame to send (*string),
 * type of message (type), pointer for buffer for response (*value), request
 * type (request) and what is start byte in response for our string
 */
GSM_Error NOKIA_GetPhoneString(GSM_StateMachine *s, unsigned char *msgframe, int msglen, unsigned char msgtype, char *retvalue, GSM_Phone_RequestID request, int startresponse)
{
	retvalue[0] = 0;
	s->Phone.Data.StartPhoneString = startresponse;
	s->Phone.Data.PhoneString = retvalue;
	return GSM_WaitFor (s, msgframe, msglen,msgtype, 4, request);
}

GSM_Error NOKIA_GetManufacturer(GSM_StateMachine *s)
{
	strcpy(s->Phone.Data.Manufacturer,"Nokia");
	return ERR_NONE;
}

/* Many functions contains such strings:
 * (1. length/256) - exist or not
 * 2. length%256
 * 3. string (unicode, no termination)
 * This function read string to output and increases counter
 */
void NOKIA_GetUnicodeString(GSM_StateMachine *s, int *current, unsigned char *input, unsigned char *output, bool FullLength)
{
	int length;

	if (FullLength) {
		length = (input[*current]*256+input[*current+1])*2;
		memcpy(output,input+(*current+2),length);
		*current = *current + 2 + length;
	} else {
		length = (input[*current])*2;
		memcpy(output,input+(*current+1),length);
		*current = *current + 1 + length;
	}

	output[length  ] = 0;
	output[length+1] = 0;
}

int NOKIA_SetUnicodeString(GSM_StateMachine *s, unsigned char *dest, unsigned char *string, bool FullLength)
{
	int length;
	
	length = UnicodeLength(string);
	if (FullLength) {
		dest[0] = length / 256;
		dest[1] = length % 256;
		CopyUnicodeString(dest + 2, string);
		return 2+length*2;		
	} else {
		dest[0] = length % 256;
		CopyUnicodeString(dest + 1, string);
		return 1+length*2;
	}
}

/* Returns correct ID for concrete memory type */
GSM_MemoryType NOKIA_GetMemoryType(GSM_StateMachine *s, GSM_MemoryType memory_type, unsigned char *ID)
{
	int i=0;

	while (ID[i+1]!=0x00) {
		if (ID[i]==memory_type) return ID[i+1];
		i=i+2;
	}
	return 0xff;
}

void NOKIA_EncodeDateTime(GSM_StateMachine *s, unsigned char* buffer, GSM_DateTime *datetime)
{
	buffer[0] = datetime->Year / 256;
	buffer[1] = datetime->Year % 256;
	buffer[2] = datetime->Month;
	buffer[3] = datetime->Day;

	buffer[4] = datetime->Hour;
	buffer[5] = datetime->Minute;
}

void NOKIA_SortSMSFolderStatus(GSM_StateMachine *s, GSM_NOKIASMSFolder *Folder)
{
	int i,j;

	if (Folder->Number!=0) {
		/* Bouble sorting */
		i=0;
		while (i!=Folder->Number-1) {
			if (Folder->Location[i]>Folder->Location[i+1]) {
				j=Folder->Location[i];
				Folder->Location[i]=Folder->Location[i+1];
				Folder->Location[i+1]=j;
				i=0;
			} else {
				i++;
			}
		}
#ifdef DEBUG
		smprintf(s, "Locations: ");
		for (i=0;i<Folder->Number;i++) {
			smprintf(s, "%i ",Folder->Location[i]);
		}
		smprintf(s, "\n");
#endif
	}
}

void NOKIA_GetDefaultProfileName(GSM_StateMachine *s, GSM_Profile *Profile)
{
	if (Profile->DefaultName) {
		switch(Profile->Location) {
		case 1:	EncodeUnicode(Profile->Name,GetMsg(s->msg,"General"),strlen(GetMsg(s->msg,"General")));
			break;
		case 2: EncodeUnicode(Profile->Name,GetMsg(s->msg,"Silent"),strlen(GetMsg(s->msg,"Silent")));
			break;
		case 3: EncodeUnicode(Profile->Name,GetMsg(s->msg,"Meeting"),strlen(GetMsg(s->msg,"Meeting")));
			break;
		case 4:	EncodeUnicode(Profile->Name,GetMsg(s->msg,"Outdoor"),strlen(GetMsg(s->msg,"Outdoor")));
			break;
		case 5:	EncodeUnicode(Profile->Name,GetMsg(s->msg,"Pager"),strlen(GetMsg(s->msg,"Pager")));
			break;
		case 6: EncodeUnicode(Profile->Name,GetMsg(s->msg,"Car"),strlen(GetMsg(s->msg,"Car")));
			break;
		case 7: EncodeUnicode(Profile->Name,GetMsg(s->msg,"Headset"),strlen(GetMsg(s->msg,"Headset")));
			break;
		}
	}
}

/* - Shared for DCT3 (n6110.c, n7110.c, n9110.c) and DCT4 (n6510.c) phones - */

GSM_Error DCT3DCT4_ReplyCallDivert(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	GSM_MultiCallDivert 	*cd = s->Phone.Data.Divert;
	int			i,pos = 11,j;

	switch (msg.Buffer[3]) {
  	case 0x02:
		smprintf(s,"Message: Call divert status received\n");
  	  	smprintf(s,"   Divert type: ");
    		switch (msg.Buffer[6]) {
      			case 0x43: smprintf(s,"when busy");			break;
	      		case 0x3d: smprintf(s,"when not answered");		break;
      			case 0x3e: smprintf(s,"when phone off or no coverage");	break;
	      		case 0x15: smprintf(s,"all types of diverts");		break;
	      		default:   smprintf(s,"unknown %i",msg.Buffer[6]);	break;
	    	}
		/* 6150 */
		if (msg.Length == 0x0b) {
			cd->Response.EntriesNum = 0;
			return ERR_NONE;
		}
		cd->Response.EntriesNum = msg.Buffer[10];
		for (i=0;i<cd->Response.EntriesNum;i++) {
		    	smprintf(s,"\n   Calls type : ");
		      	switch (msg.Buffer[pos]) {
        		case 0x0b:
				smprintf(s,"voice");
				cd->Response.Entries[i].CallType = GSM_DIVERT_VoiceCalls;
				break;
        		case 0x0d:
				smprintf(s,"fax");
				cd->Response.Entries[i].CallType = GSM_DIVERT_FaxCalls;
				break;
 		       	case 0x19:
				smprintf(s,"data");
				cd->Response.Entries[i].CallType = GSM_DIVERT_DataCalls;
				break;
        		default:
				smprintf(s,"unknown %i",msg.Buffer[pos]);
				/* 6310i */
				cd->Response.EntriesNum = 0;
				return ERR_NONE;
				break;
	    		}
		    	smprintf(s,"\n");
			j = pos + 2;
			while (msg.Buffer[j] != 0x00) j++;
			msg.Buffer[pos+1] = j - pos - 2;
			GSM_UnpackSemiOctetNumber(cd->Response.Entries[i].Number,msg.Buffer+(pos+1),false);
	      		smprintf(s,"   Number     : %s\n",DecodeUnicodeString(cd->Response.Entries[i].Number));
	        	cd->Response.Entries[i].Timeout = msg.Buffer[pos+34];
	 	     	smprintf(s,"   Timeout    : %i seconds\n",msg.Buffer[pos+34]);
			pos+=35;
	    	}
    		return ERR_NONE;
  	case 0x03:
    		smprintf(s,"Message: Call divert status receiving error ?\n");
    		return ERR_UNKNOWN;
  	}
	return ERR_UNKNOWNRESPONSE;
}

static GSM_Error DCT3DCT4_CallDivert(GSM_StateMachine *s, GSM_MultiCallDivert *divert, bool get)
{
	int 		length = 0x09;
	unsigned char 	req[55] = {N6110_FRAME_HEADER, 0x01,
			   	   0x05, /* operation = Query */
	 			   0x00,
 	 			   0x00, /* divert type */
	 			   0x00, /* call type */
	 			   0x00};

	if (!get) {
		if (UnicodeLength(divert->Request.Number) == 0) {
			req[4]  = 0x04;
		} else {
			req[4]  = 0x03;
			req[8]  = 0x01;
			req[29] = GSM_PackSemiOctetNumber(divert->Request.Number, req + 9, false);
			req[52] = divert->Request.Timeout;
			length  = 55;
		}
	}
  	switch (divert->Request.DivertType) {
    		case GSM_DIVERT_AllTypes  : req[6] = 0x15; break;
    		case GSM_DIVERT_Busy      : req[6] = 0x43; break;
    		case GSM_DIVERT_NoAnswer  : req[6] = 0x3d; break;
    		case GSM_DIVERT_OutOfReach: req[6] = 0x3e; break;
    		default                   : return ERR_NOTIMPLEMENTED;
  	}

  	switch (divert->Request.CallType) {
    		case GSM_DIVERT_AllCalls  :                break;
    		case GSM_DIVERT_VoiceCalls: req[7] = 0x0b; break;
    		case GSM_DIVERT_FaxCalls  : req[7] = 0x0d; break;
    		case GSM_DIVERT_DataCalls : req[7] = 0x19; break;
    		default                   : return ERR_NOTIMPLEMENTED;
  	}

	s->Phone.Data.Divert = divert;
	smprintf(s, "Call divert\n");
	return GSM_WaitFor (s, req, length, 0x06, 10, ID_Divert);
}

GSM_Error DCT3DCT4_GetCallDivert(GSM_StateMachine *s, GSM_MultiCallDivert *divert)
{
	return DCT3DCT4_CallDivert(s,divert,true);
}

GSM_Error DCT3DCT4_SetCallDivert(GSM_StateMachine *s, GSM_MultiCallDivert *divert)
{
	return DCT3DCT4_CallDivert(s,divert,false);
}

GSM_Error DCT3DCT4_CancelAllDiverts(GSM_StateMachine *s)
{
	GSM_MultiCallDivert 	divert;
	unsigned char 		req[55] = {N6110_FRAME_HEADER, 0x01,
					   0x04, /* operation = Disable */
		 			   0x00,
		 			   0x02, /* divert type */
		 			   0x00, /* call type */
		 			   0x00};

	s->Phone.Data.Divert = &divert;
	smprintf(s, "Call divert\n");
	return GSM_WaitFor (s, req, 0x09, 0x06, 10, ID_Divert);
}

GSM_Error DCT3DCT4_ReplyGetActiveConnectSet(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	GSM_Phone_Data *Data = &s->Phone.Data;

	Data->WAPSettings->Active = false;
	if (Data->WAPSettings->Location - 1 == msg.Buffer[4]) {
		Data->WAPSettings->Active = true;
	}
	return ERR_NONE;
}

GSM_Error DCT3DCT4_GetActiveConnectSet(GSM_StateMachine *s)
{
	unsigned char GetSetreq[] = {N6110_FRAME_HEADER, 0x0F};

	smprintf(s, "Checking, if connection settings are active\n");
	return GSM_WaitFor (s, GetSetreq, 4, 0x3f, 4, ID_GetConnectSet);
}

GSM_Error DCT3DCT4_ReplySetActiveConnectSet(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Connection settings activated\n");
	return ERR_NONE;		
}

GSM_Error DCT3DCT4_SetActiveConnectSet(GSM_StateMachine *s, GSM_MultiWAPSettings *settings)
{
	unsigned char	reqActivate[] = {N6110_FRAME_HEADER, 0x12,
					 0x00};		/* Location */

	if (settings->Active) {
		reqActivate[4] = settings->Location-1;
		smprintf(s, "Activating connection settings number %i\n",settings->Location);
		return GSM_WaitFor (s, reqActivate, 5, 0x3f, 4, ID_SetMMSSettings);
	}
	return ERR_NONE;
}


GSM_Error DCT3DCT4_SendDTMF(GSM_StateMachine *s, char *DTMFSequence)
{
	unsigned char req[100] = {N6110_FRAME_HEADER, 0x50,
				  0x00}; 	/* Length */

	if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_NODTMF)) return ERR_NOTSUPPORTED;
	if (strlen(DTMFSequence) > 100 - 5) return ERR_NOTSUPPORTED;

  	req[4] = strlen(DTMFSequence);
  
	memcpy(req+5,DTMFSequence,strlen(DTMFSequence));

	smprintf(s, "Sending DTMF\n");
	return GSM_WaitFor (s, req, 5+strlen(DTMFSequence), 0x01, 4, ID_SendDTMF);
}

GSM_Error DCT3DCT4_ReplyGetWAPBookmark(GSM_Protocol_Message msg, GSM_StateMachine *s, bool FullLength)
{
	int 			tmp;
	GSM_Phone_Data		*Data = &s->Phone.Data;

	smprintf(s, "WAP bookmark received\n");
	switch (msg.Buffer[3]) {
	case 0x07:
		tmp = 4;

		Data->WAPBookmark->Location = msg.Buffer[tmp] * 256 + msg.Buffer[tmp+1];
		smprintf(s, "Location: %i\n",Data->WAPBookmark->Location);
		tmp = tmp + 2;

 		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer, Data->WAPBookmark->Title, FullLength);
		smprintf(s, "Title   : \"%s\"\n",DecodeUnicodeString(Data->WAPBookmark->Title));

 		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer, Data->WAPBookmark->Address, FullLength);
		smprintf(s, "Address : \"%s\"\n",DecodeUnicodeString(Data->WAPBookmark->Address));

		return ERR_NONE;
	case 0x08:
		switch (msg.Buffer[4]) {
		case 0x01:
			smprintf(s, "Security error. Inside WAP bookmarks menu\n");
			return ERR_INSIDEPHONEMENU;
		case 0x02:
			smprintf(s, "Invalid or empty\n");
			return ERR_INVALIDLOCATION;
		default:
			smprintf(s, "ERROR: unknown %i\n",msg.Buffer[4]);
			return ERR_UNKNOWNRESPONSE;
		}
		break;
	}
	return ERR_UNKNOWNRESPONSE;
}

GSM_Error DCT3DCT4_ReplySetWAPBookmark(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	switch (msg.Buffer[3]) {
	case 0x0A:
		smprintf(s, "WAP bookmark set OK\n");
		return ERR_NONE;
	case 0x0B:
		smprintf(s, "WAP bookmark setting error\n");
		switch (msg.Buffer[4]) {
		case 0x01:
			smprintf(s, "Security error. Inside WAP bookmarks menu\n");
			return ERR_INSIDEPHONEMENU;
		case 0x02:
			smprintf(s, "Can't write to empty location ?\n");
			return ERR_EMPTY;
		case 0x04:
			smprintf(s, "Full memory\n");
			return ERR_FULL;
		default:
			smprintf(s, "ERROR: unknown %i\n",msg.Buffer[4]);
			return ERR_UNKNOWNRESPONSE;
		}
	}
	return ERR_UNKNOWNRESPONSE;
}

GSM_Error DCT3DCT4_ReplyEnableConnectFunc(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Connection functions enabled\n");
	return ERR_NONE;
}

GSM_Error DCT3DCT4_EnableWAPFunctions(GSM_StateMachine *s)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x00};

	if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo,F_NOWAP)) return ERR_NOTSUPPORTED;

	smprintf(s, "Enabling WAP\n");
	return GSM_WaitFor (s, req, 4, 0x3f, 4, ID_EnableConnectFunc);
}

GSM_Error DCT3DCT4_ReplyDisableConnectFunc(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Connection functions disabled\n");
	return ERR_NONE;
}

GSM_Error DCT3DCT4_DisableConnectionFunctions(GSM_StateMachine *s)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x03};

	if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo,F_NOWAP)) return ERR_NOTSUPPORTED;

	smprintf(s, "Disabling connection settings\n");
	return GSM_WaitFor (s, req, 4, 0x3f, 4, ID_DisableConnectFunc);
}

GSM_Error DCT3DCT4_ReplyDelWAPBookmark(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	switch (msg.Buffer[3]) {
	case 0x0D:
		smprintf(s, "WAP bookmark deleted OK\n");
		return ERR_NONE;
	case 0x0E:
		smprintf(s, "WAP bookmark deleting error\n");
		switch (msg.Buffer[4]) {
		case 0x01:
			smprintf(s, "Security error. Inside WAP bookmarks menu\n");
			return ERR_SECURITYERROR;
		case 0x02:
			smprintf(s, "Invalid location\n");
			return ERR_INVALIDLOCATION;
		default:
			smprintf(s, "ERROR: unknown %i\n",msg.Buffer[4]);
			return ERR_UNKNOWNRESPONSE;
		}
	}
	return ERR_UNKNOWNRESPONSE;
}

GSM_Error DCT3DCT4_DeleteWAPBookmarkPart(GSM_StateMachine *s, GSM_WAPBookmark *bookmark)
{
	GSM_Error	error;
	unsigned char 	req[] = {N6110_FRAME_HEADER, 0x0C,
			         0x00, 0x00};		/* Location */

	req[5] = bookmark->Location;

	smprintf(s, "Deleting WAP bookmark\n");
	error = GSM_WaitFor (s, req, 6, 0x3f, 4, ID_DeleteWAPBookmark);
	if (error != ERR_NONE) {
		if (error == ERR_INVALIDLOCATION || error == ERR_INSIDEPHONEMENU) {
			DCT3DCT4_DisableConnectionFunctions(s);
		}
		return error;
	}

	return DCT3DCT4_DisableConnectionFunctions(s);
}

GSM_Error DCT3DCT4_GetWAPBookmarkPart(GSM_StateMachine *s, GSM_WAPBookmark *bookmark)
{
	GSM_Error	error;
	unsigned char 	req[] = {N6110_FRAME_HEADER, 0x06,
			         0x00, 0x00};		/* Location */

	req[5]=bookmark->Location-1;

	s->Phone.Data.WAPBookmark=bookmark;
	smprintf(s, "Getting WAP bookmark\n");
	error = GSM_WaitFor (s, req, 6, 0x3f, 4, ID_GetWAPBookmark);
	if (error != ERR_NONE) {
		if (error == ERR_INVALIDLOCATION || error == ERR_INSIDEPHONEMENU) {
			DCT3DCT4_DisableConnectionFunctions(s);
		}
		return error;
	}

	return DCT3DCT4_DisableConnectionFunctions(s);
}

GSM_Error DCT3DCT4_CancelCall(GSM_StateMachine *s, int ID)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x08, 0x00, 0x85};
	                                             
	req[4] 			= (unsigned char)ID;
	s->Phone.Data.CallID 	= ID;

	smprintf(s, "Canceling single call\n");
	return GSM_WaitFor (s, req, 6, 0x01, 4, ID_CancelCall);
}            

GSM_Error DCT3DCT4_AnswerCall(GSM_StateMachine *s, int ID)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x06, 0x00, 0x00};

	req[4] 			= (unsigned char)ID;
	s->Phone.Data.CallID 	= ID;

	smprintf(s, "Answering single call\n");
	return GSM_WaitFor (s, req, 6, 0x01, 4, ID_AnswerCall);
}

GSM_Error DCT3DCT4_ReplyGetModelFirmware(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	GSM_Lines	lines;
	GSM_Phone_Data	*Data = &s->Phone.Data;

	SplitLines(msg.Buffer, msg.Length, &lines, "\x20\x0A", 2, false);

	strcpy(Data->Model,GetLineString(msg.Buffer, lines, 4));
	smprintf(s, "Received model %s\n",Data->Model);
	Data->ModelInfo = GetModelData(NULL,Data->Model,NULL);

	strcpy(Data->VerDate,GetLineString(msg.Buffer, lines, 3));
	smprintf(s, "Received firmware date %s\n",Data->VerDate);

	strcpy(Data->Version,GetLineString(msg.Buffer, lines, 2));
	smprintf(s, "Received firmware version %s\n",Data->Version);
	GSM_CreateFirmwareNumber(s);

	return ERR_NONE;
}

GSM_Error DCT3DCT4_GetModel (GSM_StateMachine *s)
{
	unsigned char 	req[5] = {N6110_FRAME_HEADER, 0x03, 0x00};
	GSM_Error 	error;

	if (strlen(s->Phone.Data.Model)>0) return ERR_NONE;

	smprintf(s, "Getting model\n");
	error=GSM_WaitFor (s, req, 5, 0xd1, 3, ID_GetModel);
	if (error==ERR_NONE) {
		if (s->di.dl==DL_TEXT || s->di.dl==DL_TEXTALL ||
		    s->di.dl==DL_TEXTDATE || s->di.dl==DL_TEXTALLDATE) {
			smprintf(s, "[Connected model  - \"%s\"]\n",s->Phone.Data.Model);
			smprintf(s, "[Firmware version - \"%s\"]\n",s->Phone.Data.Version);
			smprintf(s, "[Firmware date    - \"%s\"]\n",s->Phone.Data.VerDate);
		}
	}
	return error;
}

GSM_Error DCT3DCT4_GetFirmware (GSM_StateMachine *s)
{
	unsigned char req[5] = {N6110_FRAME_HEADER, 0x03, 0x00};  
	GSM_Error error;

	if (strlen(s->Phone.Data.Version)>0) return ERR_NONE;
	
	smprintf(s, "Getting firmware version\n");
	error=GSM_WaitFor (s, req, 5, 0xd1, 3, ID_GetFirmware);
	if (error==ERR_NONE) {
		if (s->di.dl==DL_TEXT || s->di.dl==DL_TEXTALL ||
		    s->di.dl==DL_TEXTDATE || s->di.dl==DL_TEXTALLDATE) {
			smprintf(s, "[Connected model  - \"%s\"]\n",s->Phone.Data.Model);
			smprintf(s, "[Firmware version - \"%s\"]\n",s->Phone.Data.Version);
			smprintf(s, "[Firmware date    - \"%s\"]\n",s->Phone.Data.VerDate);
		}
	}
	return error;
}

/* ---------- Shared for n7110.c and n6510.c ------------------------------- */

GSM_Error N71_65_ReplyGetMemoryError(unsigned char error, GSM_StateMachine *s)
{
	switch (error) {
	case 0x27:
		smprintf(s, "No PIN\n");
		return ERR_SECURITYERROR;
	case 0x30:
		smprintf(s, "Invalid memory type\n");
		if (s->Phone.Data.Memory->MemoryType == MEM_ME) return ERR_EMPTY;
		if (s->Phone.Data.Memory->MemoryType == MEM_SM) return ERR_EMPTY;
		return ERR_NOTSUPPORTED;
	case 0x33:
		smprintf(s, "Empty location\n");
		s->Phone.Data.Memory->EntriesNum = 0;
		return ERR_EMPTY;
	case 0x34:
		smprintf(s, "Too high location ?\n");
		return ERR_INVALIDLOCATION;
	default:
		smprintf(s, "ERROR: unknown %i\n",error);
		return ERR_UNKNOWNRESPONSE;
	}
}

GSM_Error N71_65_ReplyWritePhonebook(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Phonebook entry written ");
	switch (msg.Buffer[6]) {
	case 0x0f:
		smprintf(s, " - error\n");
		switch (msg.Buffer[10]) {
		case 0x36:
			smprintf(s, "Too long name\n");
			return ERR_NOTSUPPORTED;
		case 0x3c:
			smprintf(s, "Can not add entry with 0 subentries\n");
			return ERR_NOTSUPPORTED;			
		case 0x3d:
			smprintf(s, "Wrong entry type\n");
			return ERR_NOTSUPPORTED;
		case 0x3e:
			smprintf(s, "Too much entries\n");
			return ERR_NOTSUPPORTED;
		default:
			smprintf(s, "ERROR: unknown %i\n",msg.Buffer[10]);
			return ERR_UNKNOWNRESPONSE;
		}
	default:
		smprintf(s, " - OK\n");
		return ERR_NONE;
	}
}

bool NOKIA_FindPhoneFeatureValue(GSM_StateMachine *s,
				 GSM_Profile_PhoneTableValue 	ProfileTable[],
				 GSM_Profile_Feat_ID		FeatureID,
				 GSM_Profile_Feat_Value		FeatureValue,
			    	 unsigned char 			*PhoneID,
			    	 unsigned char 			*PhoneValue) 
{
	int i=0;

	smprintf(s, "Trying to find feature %i with value %i\n",FeatureID,FeatureValue);
	while (ProfileTable[i].ID != 0x00) {
		if (ProfileTable[i].ID == FeatureID &&
		    ProfileTable[i].Value == FeatureValue) {
			*PhoneID	= ProfileTable[i].PhoneID;
			*PhoneValue	= ProfileTable[i].PhoneValue;
			return true;
		}
		i++;
	}
	return false;
}

#define PROFILE_CALLERGROUPS_GROUP1      0x01
#define PROFILE_CALLERGROUPS_GROUP2      0x02
#define PROFILE_CALLERGROUPS_GROUP3      0x04
#define PROFILE_CALLERGROUPS_GROUP4      0x08
#define PROFILE_CALLERGROUPS_GROUP5      0x10

void NOKIA_FindFeatureValue(GSM_StateMachine		*s,
			    GSM_Profile_PhoneTableValue ProfileTable[],
			    unsigned char 		ID,
			    unsigned char 		Value,
			    GSM_Phone_Data 		*Data,
			    bool			CallerGroups) 
{
	int i;

	if (CallerGroups) {
		smprintf(s, "Caller groups: %i\n", Value);
		Data->Profile->FeatureID [Data->Profile->FeaturesNumber] = Profile_CallerGroups;
		Data->Profile->FeaturesNumber++;
		for (i=0;i<5;i++) Data->Profile->CallerGroups[i] = false;
		if ((Value & PROFILE_CALLERGROUPS_GROUP1)==PROFILE_CALLERGROUPS_GROUP1) Data->Profile->CallerGroups[0] = true;
		if ((Value & PROFILE_CALLERGROUPS_GROUP2)==PROFILE_CALLERGROUPS_GROUP2) Data->Profile->CallerGroups[1] = true;
		if ((Value & PROFILE_CALLERGROUPS_GROUP3)==PROFILE_CALLERGROUPS_GROUP3) Data->Profile->CallerGroups[2] = true;
		if ((Value & PROFILE_CALLERGROUPS_GROUP4)==PROFILE_CALLERGROUPS_GROUP4) Data->Profile->CallerGroups[3] = true;
		if ((Value & PROFILE_CALLERGROUPS_GROUP5)==PROFILE_CALLERGROUPS_GROUP5) Data->Profile->CallerGroups[4] = true;
		return;
	}

	i = 0;
	while (ProfileTable[i].ID != 0x00) {
		if (ProfileTable[i].PhoneID == ID &&
		    ProfileTable[i].PhoneValue == Value) {
#ifdef DEBUG
			switch (ProfileTable[i].ID) {
			case Profile_KeypadTone		: smprintf(s, "Keypad tones\n"); 	 	  break;
			case Profile_CallAlert		: smprintf(s, "Call alert\n"); 		  break;
			case Profile_RingtoneVolume	: smprintf(s, "Ringtone volume\n"); 	  break;
			case Profile_MessageTone	: smprintf(s, "SMS message tones\n");  	  break;
			case Profile_Vibration		: smprintf(s, "Vibration\n"); 		  break;
			case Profile_WarningTone	: smprintf(s, "Warning (ang games) tones\n"); break;
			case Profile_AutoAnswer		: smprintf(s, "Automatic answer\n"); 	  break;
			case Profile_Lights		: smprintf(s, "Lights\n"); 			  break;
			case Profile_ScreenSaver	: smprintf(s, "Screen Saver\n"); 		  break;
			case Profile_ScreenSaverTime	: smprintf(s, "Screen Saver timeout\n");	  break;
			default				:					  break;
			}
#endif
			Data->Profile->FeatureID	[Data->Profile->FeaturesNumber] = ProfileTable[i].ID;
			Data->Profile->FeatureValue	[Data->Profile->FeaturesNumber] = ProfileTable[i].Value;
			Data->Profile->FeaturesNumber++;
			break;
		}
		i++;
	}
}

GSM_Profile_PhoneTableValue Profile71_65[] = {
	{Profile_KeypadTone,	 PROFILE_KEYPAD_OFF,		0x00,0x00},
	{Profile_KeypadTone,	 PROFILE_KEYPAD_LEVEL1,		0x00,0x01},
	{Profile_KeypadTone,	 PROFILE_KEYPAD_LEVEL2,		0x00,0x02},
	{Profile_KeypadTone,	 PROFILE_KEYPAD_LEVEL3,		0x00,0x03},
	/* Lights ? */
	{Profile_CallAlert,	 PROFILE_CALLALERT_RINGING,	0x02,0x00},
	{Profile_CallAlert,	 PROFILE_CALLALERT_ASCENDING,	0x02,0x01},
	{Profile_CallAlert,	 PROFILE_CALLALERT_RINGONCE,	0x02,0x02},
	{Profile_CallAlert,	 PROFILE_CALLALERT_BEEPONCE,	0x02,0x03},
	{Profile_CallAlert,	 PROFILE_CALLALERT_OFF,		0x02,0x05},
/*	{Profile_CallAlert,	 PROFILE_CALLALERT_CALLERGROUPS,0x02,0x07},	*/
	/* Ringtone ID */
	{Profile_RingtoneVolume, PROFILE_VOLUME_LEVEL1,		0x04,0x00},
	{Profile_RingtoneVolume, PROFILE_VOLUME_LEVEL2,		0x04,0x01},
	{Profile_RingtoneVolume, PROFILE_VOLUME_LEVEL3,		0x04,0x02},
	{Profile_RingtoneVolume, PROFILE_VOLUME_LEVEL4,		0x04,0x03},
	{Profile_RingtoneVolume, PROFILE_VOLUME_LEVEL5,		0x04,0x04},
	{Profile_MessageTone,	 PROFILE_MESSAGE_NOTONE,	0x05,0x00},
	{Profile_MessageTone,	 PROFILE_MESSAGE_STANDARD,	0x05,0x01},
	{Profile_MessageTone,	 PROFILE_MESSAGE_SPECIAL,	0x05,0x02},
	{Profile_MessageTone,	 PROFILE_MESSAGE_BEEPONCE,	0x05,0x03},
	{Profile_MessageTone,	 PROFILE_MESSAGE_ASCENDING,	0x05,0x04},
	{Profile_Vibration,	 PROFILE_VIBRATION_OFF,		0x06,0x00},
	{Profile_Vibration,	 PROFILE_VIBRATION_ON,		0x06,0x01},
	{Profile_WarningTone,	 PROFILE_WARNING_OFF,		0x07,0x00},
	{Profile_WarningTone,	 PROFILE_WARNING_ON,		0x07,0x01},
	/* Caller groups */
	{Profile_AutoAnswer,	 PROFILE_AUTOANSWER_OFF,	0x09,0x00},
	{Profile_AutoAnswer,	 PROFILE_AUTOANSWER_ON,		0x09,0x01},
	{0x00,			 0x00,				0x00,0x00}
};

GSM_Error NOKIA_SetIncomingSMS(GSM_StateMachine *s, bool enable)
{
	s->Phone.Data.EnableIncomingSMS = enable;
#ifdef DEBUG
	if (enable) {
		smprintf(s, "Enabling incoming SMS\n");
	} else {
		smprintf(s, "Disabling incoming SMS\n");
	}
#endif
	return ERR_NONE;
}

GSM_Error N71_65_ReplyUSSDInfo(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	unsigned char buffer[2000],buffer2[4000];

	if (s->Phone.Data.RequestID == ID_Divert) return ERR_NONE;

	memcpy(buffer,msg.Buffer+8,msg.Buffer[7]);
	buffer[msg.Buffer[7]] = 0x00;

	smprintf(s, "USSD reply: \"%s\"\n",buffer);

	if (s->Phone.Data.EnableIncomingUSSD && s->User.IncomingUSSD!=NULL) {
		EncodeUnicode(buffer2,buffer,strlen(buffer));
		s->User.IncomingUSSD(s->CurrentConfig->Device, buffer2);
	}

	return ERR_NONE;
}

GSM_Error NOKIA_SetIncomingUSSD(GSM_StateMachine *s, bool enable)
{
	s->Phone.Data.EnableIncomingUSSD = enable;
#ifdef DEBUG
	if (enable) {
		smprintf(s, "Enabling incoming USSD\n");
	} else {
		smprintf(s, "Disabling incoming USSD\n");
	}
#endif
	return ERR_NONE;
}

GSM_Error NOKIA_SetIncomingCall(GSM_StateMachine *s, bool enable)
{
	if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo,F_NOCALLINFO)) return ERR_NOTSUPPORTED;

	s->Phone.Data.EnableIncomingCall = enable;
#ifdef DEBUG
	if (enable) {
		smprintf(s, "Enabling incoming Call\n");
	} else {
		smprintf(s, "Disabling incoming Call\n");
	}
#endif
	return ERR_NONE;
}

GSM_Error N71_65_ReplyCallInfo(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	GSM_Call 	call;
	int		tmp;
	unsigned char 	buffer[200];

	call.Status 		= 0;
	call.CallIDAvailable 	= true;
	smprintf(s, "Call info, ");
	switch (msg.Buffer[3]) {
	case 0x02:
		smprintf(s, "Call established, waiting for answer\n");
		call.Status = GSM_CALL_CallEstablished;
		break;
	case 0x03:
		smprintf(s, "Call started\n");
		smprintf(s, "Call mode  : %i\n",msg.Buffer[5]);//such interpretation is in gnokii
		tmp = 6;
		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer,buffer,false);
		smprintf(s, "Number     : \"%s\"\n",DecodeUnicodeString(buffer));
		/* FIXME: read name from frame */

		call.Status = GSM_CALL_CallStart;
		break;			
	case 0x04:
		smprintf(s, "Remote end hang up\n");
		smprintf(s, "Cause Type : %i\n",msg.Buffer[5]);//such interpretation is in gnokii
		smprintf(s, "CC         : %i\n",msg.Buffer[6]);
		smprintf(s, "MM(?)      : %i\n",msg.Buffer[7]);
		smprintf(s, "RR(?)      : %i\n",msg.Buffer[8]);
		call.Status 	= GSM_CALL_CallRemoteEnd;
		call.StatusCode = msg.Buffer[6];
		break;
	case 0x05:
		smprintf(s, "Incoming call\n");
		smprintf(s, "Call mode  : %i\n",msg.Buffer[5]);//such interpretation is in gnokii
		tmp = 6;
		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer,buffer,false);
		smprintf(s, "Number     : \"%s\"\n",DecodeUnicodeString(buffer));
		/* FIXME: read name from frame */
		call.Status = GSM_CALL_IncomingCall;
		tmp = 6;
		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer,call.PhoneNumber,false);
		break;
	case 0x07:
		smprintf(s, "Call answer initiated\n");
		break;
	case 0x09:
		smprintf(s, "Call released\n");
		call.Status = GSM_CALL_CallLocalEnd;
		break;
	case 0x0a:
		smprintf(s, "Call is being released\n");
		break;
	case 0x0b:
		smprintf(s, "Meaning not known\n");
		call.CallIDAvailable = false;
		break;
	case 0x0c:
		smprintf(s, "Audio status\n");
		if (msg.Buffer[4] == 0x01) smprintf(s, "Audio enabled\n");
				      else smprintf(s, "Audio disabled\n");
		call.CallIDAvailable = false;
		break;
	case 0x0f:
	case 0x10:
		smprintf(s, "Meaning not known\n");
		call.CallIDAvailable = false;
		break;
	case 0x23:
		smprintf(s, "Call held\n");
		call.Status = GSM_CALL_CallHeld;
		break;
	case 0x25:
		smprintf(s, "Call resumed\n");
		call.Status = GSM_CALL_CallResumed;
		break;
	case 0x27:
		smprintf(s, "Call switched\n");
		call.Status = GSM_CALL_CallSwitched;
		break;
	case 0x53:
		smprintf(s, "Outgoing call\n");
		smprintf(s, "Call mode  : %i\n",msg.Buffer[5]);//such interpretation is in gnokii
		tmp = 6;
		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer,buffer,false);
		smprintf(s, "Number     : \"%s\"\n",DecodeUnicodeString(buffer));
		/* FIXME: read name from frame */
		call.Status = GSM_CALL_OutgoingCall;
		tmp = 6;
		NOKIA_GetUnicodeString(s, &tmp, msg.Buffer,call.PhoneNumber,false);
		break;
	case 0xA6:
	case 0xD2:
	case 0xD3:
		smprintf(s, "Meaning not known\n");
		call.CallIDAvailable = false;
		break;
	}
	if (call.CallIDAvailable) smprintf(s, "Call ID    : %d\n",msg.Buffer[4]);
	if (s->Phone.Data.EnableIncomingCall && s->User.IncomingCall!=NULL && call.Status != 0) {
		if (call.CallIDAvailable) call.CallID = msg.Buffer[4];
		s->User.IncomingCall(s->CurrentConfig->Device, call);
	}
	if (s->Phone.Data.RequestID == ID_DialVoice) {
		if (msg.Buffer[3] == 0x10) return ERR_NOTSUPPORTED;
	}
	if (s->Phone.Data.RequestID == ID_CancelCall) {
		if (msg.Buffer[3] == 0x09) {
			if (s->Phone.Data.CallID == msg.Buffer[4]) return ERR_NONE;
			/* when we canceled call and see frame about other
			 * call releasing, we don't give ERR_NONE for "our"
			 * call release command
			 */
			return ERR_NEEDANOTHERANSWER;
		}
	}
	if (s->Phone.Data.RequestID == ID_AnswerCall) {
		if (msg.Buffer[3] == 0x07) {
			if (s->Phone.Data.CallID == msg.Buffer[4]) return ERR_NONE;
			return ERR_NEEDANOTHERANSWER;
		}
	}
	return ERR_NONE;
}

void N71_65_GetCalendarRecurrance(GSM_StateMachine *s, unsigned char *buffer, GSM_CalendarEntry *entry)
{
	int Recurrance;

	Recurrance = buffer[0]*256 + buffer[1];
	/* 8760 hours = 1 year */
	if (Recurrance == 0xffff) Recurrance=8760;
	if (Recurrance != 0) {
		smprintf(s, "Recurrance   : %i hours\n",Recurrance);
		entry->Entries[entry->EntriesNum].EntryType	= CAL_RECURRANCE;
		entry->Entries[entry->EntriesNum].Number	= Recurrance;
		entry->EntriesNum++;
	}
}

/* method 2 */
GSM_Error N71_65_ReplyAddCalendar2(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Calendar note added\n");
	return ERR_NONE;
}

/* method 2 */
GSM_Error N71_65_AddCalendar2(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
	GSM_CalendarNoteType	NoteType;
	time_t     		t_time1,t_time2;
	GSM_DateTime		Date,date_time;
 	GSM_Error		error;
	long			diff;
 	int 			Text, Time, Alarm, Phone, Recurrance, EndTime, Location, length=25;
	unsigned char 		req[5000] = {
		N6110_FRAME_HEADER,
		0x40,
		0x00,				/* frame length - 7 */
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,		/* start time saved as difference */
		0x00,0x00,0xff,0xff,		/* alarm saved as difference */
		0x00,				/* frame length - 7 */
		0x00,				/* note type */
		0x00,0x00,			/* recurrance */
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00};		/* rest depends on note type */

 	NoteType = N71_65_FindCalendarType(Note->Type, s->Phone.Data.ModelInfo);

	if (IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_CAL62) ||
	    IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_CAL65)) {
		switch(NoteType) {
		case GSM_CAL_MEETING : req[18] = 0x01; length = 25; break;
		case GSM_CAL_CALL    : req[18] = 0x02; length = 27; break;
		case GSM_CAL_BIRTHDAY: req[18] = 0x04; length = 28; break;
		case GSM_CAL_MEMO    : req[18] = 0x08; length = 25; break;
		default          : return ERR_UNKNOWN;
		}
	} else {
		switch(NoteType) {
		case GSM_CAL_REMINDER: req[18] = 0x01; length = 25; break;
		case GSM_CAL_CALL    : req[18] = 0x02; length = 27; break;
		case GSM_CAL_BIRTHDAY: req[18] = 0x04; length = 28; break;
		case GSM_CAL_MEMO    : req[18] = 0x08; length = 25; break;
		default          : return ERR_UNKNOWN;
		}
	}

	GSM_CalendarFindDefaultTextTimeAlarmPhoneRecurrance(Note, &Text, &Time, &Alarm, &Phone, &Recurrance, &EndTime, &Location);

	if (Time == -1) return ERR_UNKNOWN;
	if (NoteType != GSM_CAL_BIRTHDAY) {
		Date.Year 	= 2030;	Date.Month 	= 01; Date.Day    = 01;
		Date.Hour 	= 00;	Date.Minute 	= 00; Date.Second = 00;
	} else {
		Date.Year 	= 2029; Date.Month 	= 12; Date.Day 	  = 31;
		Date.Hour 	= 22;   Date.Minute 	= 59; Date.Second = 58;
	}
	t_time1 = Fill_Time_T(Date,8);
	memcpy(&Date,&Note->Entries[Time].Date,sizeof(GSM_DateTime));
	if (NoteType != GSM_CAL_BIRTHDAY) {
		Date.Year -= 20;
	} else {
		Date.Year = 1980;
		Date.Hour = 22; Date.Minute = 58; Date.Second = 58;
	}
	t_time2 = Fill_Time_T(Date,8);
	diff	= t_time1-t_time2;
	smprintf(s, "  Difference : %li seconds\n", -diff);
	req[9]  = (unsigned char)(-diff >> 24);
	req[10] = (unsigned char)(-diff >> 16);
	req[11] = (unsigned char)(-diff >> 8);
	req[12] = (unsigned char)(-diff);
	if (NoteType == GSM_CAL_BIRTHDAY) {
		req[25] = Note->Entries[Time].Date.Year / 256;
		req[26] = Note->Entries[Time].Date.Year % 256;
		/* Recurrance = 1 year */
		req[19] = 0xff;
		req[20] = 0xff;
	}

	if (NoteType == GSM_CAL_CALL && Phone != -1) {
		req[25] = UnicodeLength(Note->Entries[Phone].Text);
		CopyUnicodeString(req+length,Note->Entries[Phone].Text);
		length += UnicodeLength(Note->Entries[Phone].Text)*2;
	}
 
	if (Alarm != -1) {
		if (NoteType == GSM_CAL_BIRTHDAY) {
			if (Note->Entries[Alarm].EntryType == CAL_SILENT_ALARM_DATETIME) req[27] = 0x01;
			error=s->Phone.Functions->GetDateTime(s,&date_time);
			switch (error) {
				case ERR_EMPTY:
				case ERR_NOTIMPLEMENTED:
					GSM_GetCurrentDateTime(&date_time);
					break;
				case ERR_NONE:
					break;
				default:
					return error;
			}
			Date.Year	= date_time.Year;
			Date.Hour   	= 23;
			Date.Minute 	= 59;
		} else {
			Date.Year += 20;
		}
		t_time2   = Fill_Time_T(Date,8);
		t_time1   = Fill_Time_T(Note->Entries[Alarm].Date,8);
		diff	  = t_time1-t_time2;

		/* Sometimes we have difference in minutes */
		if (NoteType == GSM_CAL_MEETING) diff = diff / 60;
		if (!IsPhoneFeatureAvailable(s->Phone.Data.ModelInfo, F_CAL35)) {
			if (NoteType == GSM_CAL_MEMO || NoteType == GSM_CAL_CALL) {
				diff = diff / 60;
			}
		}

		smprintf(s, "  Difference : %li seconds or minutes\n", -diff);
		req[13] = (unsigned char)(-diff >> 24);
		req[14] = (unsigned char)(-diff >> 16);
		req[15] = (unsigned char)(-diff >> 8);
		req[16] = (unsigned char)(-diff);
	}

	if (Recurrance != -1) {
		/* 8760 hours = 1 year */
		if (Note->Entries[Recurrance].Number >= 8760) {
			req[19] = 0xff;
			req[20] = 0xff;
		} else {
			req[19] = Note->Entries[Recurrance].Number / 256;
			req[20] = Note->Entries[Recurrance].Number % 256;
		}
	}

	if (Text != -1) {
		switch (NoteType) {
		case GSM_CAL_CALL:
			req[26] = UnicodeLength(Note->Entries[Text].Text);
			break;
		default:
			req[length++] = UnicodeLength(Note->Entries[Text].Text);
			if (NoteType == GSM_CAL_MEMO || NoteType == GSM_CAL_MEETING) req[length++] = 0x00;
		}
		CopyUnicodeString(req+length,Note->Entries[Text].Text);
		length += UnicodeLength(Note->Entries[Text].Text)*2;
	}

	req[length++] = 0x00;
	req[length++] = 0x00;

	req[4] = req[17] = length-7;

	smprintf(s, "Writing calendar note method 2\n");
	return GSM_WaitFor (s, req, length, 0x13, 4, ID_SetCalendarNote);
}

/* method 1*/
GSM_Error N71_65_ReplyGetCalendarNotePos1(GSM_Protocol_Message msg, GSM_StateMachine *s,int *FirstCalendarPos)
{
	smprintf(s, "First calendar location: %i\n",msg.Buffer[4]*256+msg.Buffer[5]);
	*FirstCalendarPos = msg.Buffer[4]*256+msg.Buffer[5];
	return ERR_NONE;
}

/* method 1*/
static GSM_Error N71_65_GetCalendarNotePos1(GSM_StateMachine *s)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x31};

	smprintf(s, "Getting first free calendar note location\n");
	return GSM_WaitFor (s, req, 4, 0x13, 4, ID_GetCalendarNotePos);
}

/* method 1 */
GSM_Error N71_65_ReplyAddCalendar1(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
#ifdef DEBUG
	smprintf(s, "Written calendar note type ");
	switch ((msg.Buffer[3]/2)-1) {
		case 0:	smprintf(s, "Meeting");	break;
		case 1:	smprintf(s, "Call");	break;
		case 2:	smprintf(s, "Birthday");break;
		case 3:	smprintf(s, "Reminder");break;
	}
	smprintf(s, " on location %d\n",msg.Buffer[4]*256+msg.Buffer[5]);
#endif
	return ERR_NONE;
}

/* method 1 */
GSM_Error N71_65_AddCalendar1(GSM_StateMachine *s, GSM_CalendarEntry *Note, int *FirstCalendarPos)
{
	long			seconds;
 	GSM_Error		error;
	GSM_DateTime		DT;
 	int 			Text, Time, Alarm, Phone, Recurrance, EndTime, Location, count=12;
	unsigned char 		req[5000] = {
		N6110_FRAME_HEADER,
		0x01,			/* note type 	*/
		0x00, 0x00,		/* location ? 	*/
		0x00,			/* entry type 	*/
		0x00,
		0x00, 0x00,		/* Year 	*/
		0x00,			/* Month 	*/
		0x00,			/* Day 		*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00};                          

	error=N71_65_GetCalendarNotePos1(s);
	if (error!=ERR_NONE) return error;
	if (FirstCalendarPos != NULL) {
		req[4] = *FirstCalendarPos/256;
		req[5] = *FirstCalendarPos%256;
	}

	switch(Note->Type) {
		case GSM_CAL_CALL    : req[3]=0x03; req[6]=0x02; break;
		case GSM_CAL_BIRTHDAY: req[3]=0x05; req[6]=0x04; break;
		case GSM_CAL_MEMO    : req[3]=0x07; req[6]=0x08; break;
		case GSM_CAL_MEETING :
		default		     : req[3]=0x01; req[6]=0x01; break;
	}

	GSM_CalendarFindDefaultTextTimeAlarmPhoneRecurrance(Note, &Text, &Time, &Alarm, &Phone, &Recurrance, &EndTime, &Location);

	if (Time == -1) return ERR_UNKNOWN;
	memcpy(&DT,&Note->Entries[Time].Date,sizeof(GSM_DateTime));
	req[8]	= DT.Year / 256;
	req[9]	= DT.Year % 256;
	req[10]	= DT.Month;
	req[11]	= DT.Day;

	switch(Note->Type) {
	case GSM_CAL_BIRTHDAY:
		/* byte 12 and 13 */
		req[count++] = 0x00;
		req[count++] = 0x00;

		/* Alarm - bytes 14 to 17 */
		req[count++] = 0x00;
		req[count++] = 0x00;
		req[count++] = 0xff;
		req[count++] = 0xff;
		if (Alarm != -1) {
#ifndef ENABLE_LGPL
			/* Comment from original source by Gabriele Zappi:
			 * I try with Time.Year = Alarm.Year. If negative, I increase 1 year,
			 * but only once ! This thing, because I may have Alarm period across
			 * a year. (eg. Birthday on 2001-01-10 and Alarm on 2000-12-27)
			 */
#endif
			DT.Year = Note->Entries[Alarm].Date.Year;
			seconds = Fill_Time_T(DT,8)-Fill_Time_T(Note->Entries[Alarm].Date,8);
			if (seconds<0L) {
				DT.Year++;
				seconds = Fill_Time_T(DT,8)-Fill_Time_T(Note->Entries[Alarm].Date,8);
			}
			if (seconds>=0L) {
				count -= 4;
				/* bytes 14 to 17 */
				req[count++] = (unsigned char)(seconds>>24);
				req[count++] = (unsigned char)((seconds>>16) & 0xff);
				req[count++] = (unsigned char)((seconds>>8) & 0xff);
				req[count++] = (unsigned char)(seconds & 0xff);
			}
			/* byte 18 */
			if (Note->Entries[Alarm].EntryType == CAL_SILENT_ALARM_DATETIME) req[count++] = 0x01; else req[count++] = 0x00;
		}

		/* byte 19 and next */
		if (Text != -1) {
			req[count++] = UnicodeLength(Note->Entries[Text].Text);
			CopyUnicodeString(req+count,Note->Entries[Text].Text);
			count=count+2*UnicodeLength(Note->Entries[Text].Text);
		} else {
			req[count++] = 0x00;
		}
		break;
	case GSM_CAL_MEMO:
		/* byte 12 and 13 */
		if (Recurrance != -1) {
			/* 8760 hours = 1 year */	
			if (Note->Entries[Recurrance].Number >= 8760) {
				req[count++] = 0xff;
				req[count++] = 0xff;
			} else {
				req[count++] = Note->Entries[Recurrance].Number / 256;
				req[count++] = Note->Entries[Recurrance].Number % 256;
			}
		} else {
			req[count++] = 0x00;
			req[count++] = 0x00;
		}

		/* byte 14 and next */
		if (Text != -1) {
			req[count++] = UnicodeLength(Note->Entries[Text].Text);
			req[count++] = 0x00;
			CopyUnicodeString(req+count,Note->Entries[Text].Text);
			count=count+2*UnicodeLength(Note->Entries[Text].Text);
		} else {
			req[count++] = 0x00;
			req[count++] = 0x00;
		}
		break;
	case GSM_CAL_MEETING:
	case GSM_CAL_CALL:
	default:
		/* byte 12 and 13 */
		req[count++] = DT.Hour;
		req[count++] = DT.Minute;

		/* Alarm - byte 14 and 15 */
		req[count++] = 0xff;
		req[count++] = 0xff;
		if (Alarm != -1) {
			seconds=Fill_Time_T(DT,8)-Fill_Time_T(Note->Entries[Alarm].Date,8);
			if (seconds>=0L) {
				count -= 2;
				req[count++] = ((unsigned char)(seconds/60L)>>8);
				req[count++] = (unsigned char)((seconds/60L)&0xff);
			}
		}

		/* byte 16 and 17 */
		if (Recurrance != -1) {
			/* 8760 hours = 1 year */
			if (Note->Entries[Recurrance].Number >= 8760) {
				req[count++] = 0xff;
				req[count++] = 0xff;
			} else {
				req[count++] = Note->Entries[Recurrance].Number / 256;
				req[count++] = Note->Entries[Recurrance].Number % 256;
			}
		} else {
			req[count++] = 0x00;
			req[count++] = 0x00;
		}

		/* byte 18 */
		if (Text != -1) {
			req[count++] = UnicodeLength(Note->Entries[Text].Text);
		} else {
			req[count++] = 0x00;
		}
		/* byte 19 */
		if (Note->Type == GSM_CAL_CALL && Phone != -1) {
			req[count++] = UnicodeLength(Note->Entries[Phone].Text);
		} else {
			req[count++] = 0x00;
		}
		if (Text != -1) {
			CopyUnicodeString(req+count,Note->Entries[Text].Text);
			count=count+2*UnicodeLength(Note->Entries[Text].Text);
		}
		if (Note->Type == GSM_CAL_CALL && Phone != -1) {
			CopyUnicodeString(req+count,Note->Entries[Phone].Text);
			count=count+2*UnicodeLength(Note->Entries[Phone].Text);
		}
		break;
	}
	req[count] = 0x00;
	smprintf(s, "Writing calendar note method 1\n");
	return GSM_WaitFor (s, req, count, 0x13, 4, ID_SetCalendarNote);
}

GSM_Error N71_65_ReplyDelCalendar(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	smprintf(s, "Deleted calendar note on location %d\n",msg.Buffer[4]*256+msg.Buffer[5]);
	return ERR_NONE;
}

GSM_Error N71_65_DelCalendar(GSM_StateMachine *s, GSM_CalendarEntry *Note)
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x0b,
			       0x00, 0x00};	/* location */

	req[4] = Note->Location / 256;
	req[5] = Note->Location % 256;

	smprintf(s, "Deleting calendar note\n");
	return GSM_WaitFor (s, req, 6, 0x13, 4, ID_DeleteCalendarNote);
}

/* method 1 */
GSM_Error N71_65_ReplyGetCalendarInfo1(GSM_Protocol_Message msg, GSM_StateMachine *s, GSM_NOKIACalToDoLocations *LastCalendar)
{
	int i,j=0;

	smprintf(s, "Info with calendar notes locations received method 1\n");
	while (LastCalendar->Location[j] != 0x00) j++;
	if (j >= GSM_MAXCALENDARTODONOTES) {
		smprintf(s, "Increase GSM_MAXCALENDARNOTES\n");
		return ERR_UNKNOWN;
	}
	if (j == 0) {
		LastCalendar->Number=msg.Buffer[4]*256+msg.Buffer[5];
		smprintf(s, "Number of Entries: %i\n",LastCalendar->Number);
	}
	smprintf(s, "Locations: ");
	i = 0;
	while (9+(i*2) <= msg.Length) {
		LastCalendar->Location[j++]=msg.Buffer[8+(i*2)]*256+msg.Buffer[9+(i*2)];
		smprintf(s, "%i ",LastCalendar->Location[j-1]);
		i++;
	}
	smprintf(s, "\nNumber of Entries in frame: %i\n",i);
	smprintf(s, "\n");
	LastCalendar->Location[j] = 0;
	if (i == 1 && msg.Buffer[8+(0*2)]*256+msg.Buffer[9+(0*2)] == 0) return ERR_EMPTY;
	if (i == 0) return ERR_EMPTY;
	return ERR_NONE;
}

/* method 1 */
GSM_Error N71_65_GetCalendarInfo1(GSM_StateMachine *s, GSM_NOKIACalToDoLocations *LastCalendar)
{
	GSM_Error	error;
	int		i;
	unsigned char 	req[] = {N6110_FRAME_HEADER, 0x3a,
			         0xFF, 0xFE};	/* First location number */

	LastCalendar->Location[0] = 0x00;
	LastCalendar->Number	  = 0;

	smprintf(s, "Getting locations for calendar method 1\n");
	error = GSM_WaitFor (s, req, 6, 0x13, 4, ID_GetCalendarNotesInfo);
	if (error != ERR_NONE && error != ERR_EMPTY) return error;

	while (1) {
		i=0;
		while (LastCalendar->Location[i] != 0x00) i++;
		if (i == LastCalendar->Number) break;
		if (i != LastCalendar->Number && error == ERR_EMPTY) {
			smprintf(s, "Phone doesn't support some notes with this method. Workaround\n");
			LastCalendar->Number = i;
			break;
		}
		smprintf(s, "i = %i %i\n",i,LastCalendar->Number);
		req[4] = LastCalendar->Location[i-1] / 256;
		req[5] = LastCalendar->Location[i-1] % 256;
		smprintf(s, "Getting locations for calendar\n");
		error = GSM_WaitFor (s, req, 6, 0x13, 4, ID_GetCalendarNotesInfo);
		if (error != ERR_NONE && error != ERR_EMPTY) return error;
	}
	return ERR_NONE;
}

/* method 1 */
GSM_Error N71_65_ReplyGetNextCalendar1(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int 			alarm,i;
	GSM_CalendarEntry	*entry = s->Phone.Data.Cal;

	smprintf(s, "Calendar note received method 1\n");

	/* Later these values can change */
	if (msg.Buffer[6]!=0x04) { /* Here not birthday */
		entry->Entries[0].Date.Year = msg.Buffer[8]*256+msg.Buffer[9];
	}
	entry->Entries[0].Date.Month 	= msg.Buffer[10];
	entry->Entries[0].Date.Day	= msg.Buffer[11];
	entry->Entries[0].Date.Hour	= msg.Buffer[12];
	entry->Entries[0].Date.Minute	= msg.Buffer[13];
	entry->Entries[0].Date.Second 	= 0;
	entry->Entries[0].EntryType 	= CAL_START_DATETIME;
	entry->EntriesNum++;

	switch (msg.Buffer[6]) {
	case 0x01:
		smprintf(s, "Meeting\n");
		entry->Type = GSM_CAL_MEETING;

		alarm=msg.Buffer[14]*256+msg.Buffer[15];
		if (alarm != 0xffff) {
			smprintf(s, "  Difference : %i seconds\n", alarm);
			memcpy(&entry->Entries[1].Date,&entry->Entries[0].Date,sizeof(GSM_DateTime));
			GetTimeDifference(alarm, &entry->Entries[1].Date, false, 60);
			entry->Entries[1].EntryType = CAL_ALARM_DATETIME;
			entry->EntriesNum++;
		}
		N71_65_GetCalendarRecurrance(s, msg.Buffer + 16, entry);

		memcpy(entry->Entries[entry->EntriesNum].Text, msg.Buffer+20, msg.Buffer[18]*2);
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[18]*2]   = 0;
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[18]*2+1] = 0;
		entry->Entries[entry->EntriesNum].EntryType		   = CAL_TEXT;
		smprintf(s, "Text         : \"%s\"\n", DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
		entry->EntriesNum++;
		return ERR_NONE;
	case 0x02:
		smprintf(s, "Call\n");
		entry->Type = GSM_CAL_CALL;

		alarm=msg.Buffer[14]*256+msg.Buffer[15];
		if (alarm != 0xffff) {
			smprintf(s, "  Difference : %i seconds\n", alarm);
			memcpy(&entry->Entries[1].Date,&entry->Entries[0].Date,sizeof(GSM_DateTime));
			GetTimeDifference(alarm, &entry->Entries[1].Date, false, 60);
			entry->Entries[1].EntryType = CAL_ALARM_DATETIME;
			entry->EntriesNum++;
		}
		N71_65_GetCalendarRecurrance(s, msg.Buffer + 16, entry);

		i = msg.Buffer[18] * 2;
		if (i!=0) {
			memcpy(entry->Entries[entry->EntriesNum].Text, msg.Buffer+20, i);
			entry->Entries[entry->EntriesNum].Text[i]   	= 0;
			entry->Entries[entry->EntriesNum].Text[i+1] 	= 0;
			entry->Entries[entry->EntriesNum].EntryType	= CAL_TEXT;
			smprintf(s, "Text         : \"%s\"\n", DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
			entry->EntriesNum++;
		}

		memcpy(entry->Entries[entry->EntriesNum].Text, msg.Buffer+20+i, msg.Buffer[19]*2);
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[19]*2]   = 0;
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[19]*2+1] = 0;
		entry->Entries[entry->EntriesNum].EntryType		   = CAL_PHONE;
		smprintf(s, "Phone        : \"%s\"\n", DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
		entry->EntriesNum++;
		return ERR_NONE;
	case 0x04:
		smprintf(s, "Birthday\n");
		entry->Type = GSM_CAL_BIRTHDAY;

		entry->Entries[0].Date.Hour	= 23;
		entry->Entries[0].Date.Minute	= 59;
		entry->Entries[0].Date.Second	= 58;

		alarm  = ((unsigned int)msg.Buffer[14]) << 24;
		alarm += ((unsigned int)msg.Buffer[15]) << 16;
		alarm += ((unsigned int)msg.Buffer[16]) << 8;
		alarm += msg.Buffer[17];
		if (alarm != 0xffff) {
			smprintf(s, "  Difference : %i seconds\n", alarm);
			memcpy(&entry->Entries[1].Date,&entry->Entries[0].Date,sizeof(GSM_DateTime));
			GetTimeDifference(alarm, &entry->Entries[1].Date, false, 1);
			entry->Entries[1].EntryType = CAL_ALARM_DATETIME;
			if (msg.Buffer[20]!=0x00) {
				entry->Entries[1].EntryType = CAL_SILENT_ALARM_DATETIME;
				smprintf(s, "Alarm type   : Silent\n");
			}
			entry->EntriesNum++;
		}

		entry->Entries[0].Date.Year = msg.Buffer[18]*256 + msg.Buffer[19];
		if (entry->Entries[0].Date.Year == 65535) entry->Entries[0].Date.Year = 0;
		smprintf(s, "Age          : %i\n",entry->Entries[0].Date.Year);

		memcpy(entry->Entries[entry->EntriesNum].Text, msg.Buffer+22, msg.Buffer[21]*2);
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[21]*2]   = 0;
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[21]*2+1] = 0;
		entry->Entries[entry->EntriesNum].EntryType		   = CAL_TEXT;
		smprintf(s, "Text         : \"%s\"\n", DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
		entry->EntriesNum++;
		return ERR_NONE;
	case 0x08:
		smprintf(s, "Memo\n");
		entry->Type = GSM_CAL_MEMO;

		entry->Entries[0].Date.Hour	= 0;
		entry->Entries[0].Date.Minute	= 0;

		N71_65_GetCalendarRecurrance(s, msg.Buffer + 12, entry);

		memcpy(entry->Entries[entry->EntriesNum].Text, msg.Buffer+16, msg.Buffer[14]*2);
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[14]*2]   = 0;
		entry->Entries[entry->EntriesNum].Text[msg.Buffer[14]*2+1] = 0;
		entry->Entries[entry->EntriesNum].EntryType		   = CAL_TEXT;
		smprintf(s, "Text         : \"%s\"\n", DecodeUnicodeString(entry->Entries[entry->EntriesNum].Text));
		entry->EntriesNum++;
		return ERR_NONE;
	default:
		smprintf(s, "ERROR: unknown %i\n",msg.Buffer[6]);
		return ERR_UNKNOWNRESPONSE;
	}
}

/* method 1 */
GSM_Error N71_65_GetNextCalendar1(GSM_StateMachine *s, GSM_CalendarEntry *Note, bool start, GSM_NOKIACalToDoLocations *LastCalendar, int *LastCalendarYear, int *LastCalendarPos)
{
	GSM_Error		error;
	GSM_DateTime		date_time;
	unsigned char 		req[] = {N6110_FRAME_HEADER, 0x19, 
					 0x00, 0x00};		/* Location */

	if (start) {
		error=N71_65_GetCalendarInfo1(s, LastCalendar);
		if (error!=ERR_NONE) return error;
		if (LastCalendar->Number == 0) return ERR_EMPTY;

		/* We have to get current year. It's NOT written in frame for
		 * Birthday
		 */
		error=s->Phone.Functions->GetDateTime(s,&date_time);
		switch (error) {
			case ERR_EMPTY:
			case ERR_NOTIMPLEMENTED:
				GSM_GetCurrentDateTime(&date_time);
				break;
			case ERR_NONE:
				break;
			default:
				return error;
		}
		*LastCalendarYear 	= date_time.Year;
		*LastCalendarPos 	= 0;
	} else {
		(*LastCalendarPos)++;
	}

	if (*LastCalendarPos >= LastCalendar->Number) return ERR_EMPTY;

	req[4] = LastCalendar->Location[*LastCalendarPos] / 256;
	req[5] = LastCalendar->Location[*LastCalendarPos] % 256;

	Note->EntriesNum		= 0;
	Note->Entries[0].Date.Year 	= *LastCalendarYear;
	Note->Location			= LastCalendar->Location[*LastCalendarPos];

	s->Phone.Data.Cal=Note;
	smprintf(s, "Getting calendar note method 1\n");
	return GSM_WaitFor (s, req, 6, 0x13, 4, ID_GetCalendarNote);
}

GSM_Error N71_65_EnableFunctions(GSM_StateMachine *s,char *buff,int len)
{
	unsigned char buffer[50] = {N6110_FRAME_HEADER, 0x10,
				    0x07};	/* Length */

	buffer[4] = len;
	memcpy(buffer+5,buff,len);

	/* Enables various things like incoming SMS, call info, etc. */
	return s->Protocol.Functions->WriteMessage(s, buffer, 5+len, 0x10);
}

GSM_Error N71_65_ReplySendDTMF(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	switch (msg.Buffer[3]) {
	case 0x51:
		smprintf(s, "DTMF sent OK\n");
		return ERR_NONE;
	case 0x59:
	case 0x5E:
		smprintf(s, "meaning unknown - during sending DTMF\n");
		return ERR_NONE;
	}
	return ERR_UNKNOWNRESPONSE;
}

GSM_CalendarNoteType N71_65_FindCalendarType(GSM_CalendarNoteType Type, OnePhoneModel *model)
{
	switch (Type) {
	case GSM_CAL_CALL:
		return GSM_CAL_CALL;
	case GSM_CAL_BIRTHDAY:
		return GSM_CAL_BIRTHDAY;
	case GSM_CAL_MEETING:
		if (IsPhoneFeatureAvailable(model, F_CAL35)) {
			return GSM_CAL_REMINDER;
		} else return GSM_CAL_MEETING;
	case GSM_CAL_MEMO:
		if (IsPhoneFeatureAvailable(model, F_CAL35)) {
			return GSM_CAL_REMINDER;
		} else return GSM_CAL_MEMO;
	case GSM_CAL_REMINDER:
		if (IsPhoneFeatureAvailable(model, F_CAL62) ||
		    IsPhoneFeatureAvailable(model, F_CAL65)) {
			return GSM_CAL_CALL;
		} else return GSM_CAL_REMINDER;
	default:
		return GSM_CAL_CALL;
	}
}

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
