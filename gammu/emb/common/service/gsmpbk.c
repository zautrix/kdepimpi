/* (c) 2001-2003 by Marcin Wiacek,... */

#include <string.h>

#include "../misc/coding/coding.h"
#include "gsmpbk.h"
#include "gsmmisc.h"

unsigned char *GSM_PhonebookGetEntryName (GSM_MemoryEntry *entry)
{
        /* We possibly store here "LastName, FirstName" so allocate enough memory */
        static char     dest[(GSM_PHONEBOOK_TEXT_LENGTH*2+2+1)*2];
        static char     split[] = { '\0', ',', '\0', ' ', '\0', '\0'};
        int             i;
        int             first = -1, last = -1, name = -1;
        int             len = 0;

        for (i = 0; i < entry->EntriesNum; i++) {
                switch (entry->Entries[i].EntryType) {
                        case PBK_Text_LastName:
                                last = i;
                                break;
                        case PBK_Text_FirstName:
                                first = i;
                                break;
                        case PBK_Text_Name:
                                name = i;
                                break;
                        default:
                                break;
                }
        }

        if (name != -1) {
                CopyUnicodeString(dest, entry->Entries[name].Text);
        } else {
                if (last != -1 && first != -1) {
                        len = UnicodeLength(entry->Entries[last].Text);
                        CopyUnicodeString(dest, entry->Entries[last].Text);
                        CopyUnicodeString(dest + 2*len, split);
                        CopyUnicodeString(dest + 2*len + 4, entry->Entries[first].Text);
                } else if (last != -1) {
                        CopyUnicodeString(dest, entry->Entries[last].Text);
                } else if (first != -1) {
                        CopyUnicodeString(dest, entry->Entries[first].Text);
                } else {
                        return NULL;
                }
        }

        return dest;
}

void GSM_PhonebookFindDefaultNameNumberGroup(GSM_MemoryEntry *entry, int *Name, int *Number, int *Group)
{
        int i;

        *Name   = -1;
        *Number = -1;
        *Group  = -1;
        for (i = 0; i < entry->EntriesNum; i++) {
                switch (entry->Entries[i].EntryType) {
                case PBK_Number_General : if (*Number   == -1) *Number  = i; break;
                case PBK_Text_Name      : if (*Name     == -1) *Name    = i; break;
                case PBK_Caller_Group   : if (*Group    == -1) *Group   = i; break;
                default                 :                                    break;
                }
        }
        if ((*Number) == -1) {
                for (i = 0; i < entry->EntriesNum; i++) {
                        switch (entry->Entries[i].EntryType) {
                                case PBK_Number_Mobile:
                                case PBK_Number_Work:
                                case PBK_Number_Fax:
                                case PBK_Number_Home:
                                case PBK_Number_Pager:
                                case PBK_Number_Other:
                                        *Number = i;
                                        break;
                                default:
                                        break;
                        }
                        if (*Number != -1) break;
                }
        }
        if ((*Name) == -1) {
                for (i = 0; i < entry->EntriesNum; i++) {
			if (entry->Entries[i].EntryType != PBK_Text_LastName) continue;
                        *Name = i;
			break;
                }
        }
        if ((*Name) == -1) {
                for (i = 0; i < entry->EntriesNum; i++) {
                        if (entry->Entries[i].EntryType != PBK_Text_FirstName) continue;
                        *Name = i;
                        break;
		}
	}
}

void GSM_EncodeVCARD(char *Buffer, int *Length, GSM_MemoryEntry *pbk, bool header, GSM_VCardVersion Version)
{
        int     Name, Number, Group, i;
        bool    ignore;

        GSM_PhonebookFindDefaultNameNumberGroup(pbk, &Name, &Number, &Group);

        if (Version == Nokia_VCard10) {
                if (header) *Length+=sprintf(Buffer+(*Length),"BEGIN:VCARD%c%c",13,10);
                if (Name != -1) {
                        *Length+=sprintf(Buffer+(*Length),"N:%s%c%c",DecodeUnicodeString(pbk->Entries[Name].Text),13,10);
                }
                if (Number != -1) {
                        *Length +=sprintf(Buffer+(*Length),"TEL:%s%c%c",DecodeUnicodeString(pbk->Entries[Number].Text),13,10);
                }
                if (header) *Length+=sprintf(Buffer+(*Length),"END:VCARD%c%c",13,10);
        } else if (Version == Nokia_VCard21) {
                if (header) *Length+=sprintf(Buffer+(*Length),"BEGIN:VCARD%c%cVERSION:2.1%c%c",13,10,13,10);
                if (Name != -1) {
                        SaveVCALText(Buffer, Length, pbk->Entries[Name].Text, "N");
                }
                for (i=0; i < pbk->EntriesNum; i++) {
                        if (i != Name) {
                                ignore = false;
                                switch(pbk->Entries[i].EntryType) {
                                case PBK_Text_Name      :
                                case PBK_Date           :
                                case PBK_Caller_Group   :
                                        ignore = true;
                                        break;
                                case PBK_Number_General :
                                        *Length+=sprintf(Buffer+(*Length),"TEL");
                                        (*Length)+=sprintf(Buffer+(*Length),";PREF");
                                        break;
                                case PBK_Number_Mobile  :
                                        *Length+=sprintf(Buffer+(*Length),"TEL");
                                        //if (Number == i) (*Length)+=sprintf(Buffer+(*Length),";PREF");
                                        *Length+=sprintf(Buffer+(*Length),";CELL");
                                        break;
                                case PBK_Number_Work    :
                                        *Length+=sprintf(Buffer+(*Length),"TEL");
                                        //if (Number == i) (*Length)+=sprintf(Buffer+(*Length),";PREF");
                                        *Length+=sprintf(Buffer+(*Length),";WORK");
                                        break;
                                case PBK_Number_Fax     :
                                        *Length+=sprintf(Buffer+(*Length),"TEL");
                                        //if (Number == i) (*Length)+=sprintf(Buffer+(*Length),";PREF");
                                        *Length+=sprintf(Buffer+(*Length),";FAX");
                                        break;
                                case PBK_Number_Home    :
                                        *Length+=sprintf(Buffer+(*Length),"TEL");
                                        //if (Number == i) (*Length)+=sprintf(Buffer+(*Length),";PREF");
                                        *Length+=sprintf(Buffer+(*Length),";HOME");
                                        break;
                                case PBK_Text_Note      :
                                        *Length+=sprintf(Buffer+(*Length),"NOTE");
                                        break;
                                case PBK_Text_Postal    :
                                        /* Don't ask why. Nokia phones save postal address
                                         * double - once like LABEL, second like ADR
                                         */
                                        //SaveVCALText(Buffer, Length, pbk->Entries[i].Text, "LABEL");
                                        *Length+=sprintf(Buffer+(*Length),"ADR");
                                        break;
                                case PBK_Text_Email     :
                                case PBK_Text_Email2    :
                                        *Length+=sprintf(Buffer+(*Length),"EMAIL");
                                        break;
                                case PBK_Text_URL       :
                                        *Length+=sprintf(Buffer+(*Length),"URL");
                                        break;
                                default :
                                        ignore = true;
                                        break;
                                }
                                if (!ignore) {
                                        SaveVCALText(Buffer, Length, pbk->Entries[i].Text, "");
                                }
                        }
                }
                *Length+=sprintf(Buffer+(*Length), "X-KADDRESSBOOK-X-ExternalID:%d%c%c",pbk->Location,13,10);
                if (header) *Length+=sprintf(Buffer+(*Length),"END:VCARD%c%c",13,10);
        }
}

GSM_Error GSM_DecodeVCARD(unsigned char *Buffer, int *Pos, GSM_MemoryEntry *Pbk, GSM_VCardVersion Version)
{
        unsigned char   Line[2000],Buff[2000];
        int             Level = 0;

        Buff[0]         = 0;
        Pbk->EntriesNum = 0;

        while (1) {
                MyGetLine(Buffer, Pos, Line, strlen(Buffer));
                if (strlen(Line) == 0) break;
                switch (Level) {
                case 0:
                        if (strstr(Line,"BEGIN:VCARD")) Level = 1;
                        break;
                case 1:
                        if (strstr(Line,"END:VCARD")) {
                                if (Pbk->EntriesNum == 0) return ERR_EMPTY;
                                return ERR_NONE;
                        }
                        if (ReadVCALText(Line, "N", Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Text_Name;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "TEL",                   Buff) ||
                            ReadVCALText(Line, "TEL;VOICE",             Buff) ||
                            ReadVCALText(Line, "TEL;PREF",              Buff) ||
                            ReadVCALText(Line, "TEL;PREF;VOICE",        Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Number_General;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "TEL;CELL",              Buff) ||
                            ReadVCALText(Line, "TEL;CELL;VOICE",        Buff) ||
                            ReadVCALText(Line, "TEL;PREF;CELL",         Buff) ||
                            ReadVCALText(Line, "TEL;PREF;CELL;VOICE",   Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Number_Mobile;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "TEL;WORK",              Buff) ||
                            ReadVCALText(Line, "TEL;PREF;WORK",         Buff) ||
                            ReadVCALText(Line, "TEL;WORK;VOICE",        Buff) ||
                            ReadVCALText(Line, "TEL;PREF;WORK;VOICE",   Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Number_Work;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "TEL;FAX",               Buff) ||
                            ReadVCALText(Line, "TEL;PREF;FAX",          Buff) ||
                            ReadVCALText(Line, "TEL;FAX;VOICE",         Buff) ||
                            ReadVCALText(Line, "TEL;PREF;FAX;VOICE",    Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Number_Fax;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "TEL;HOME",              Buff) ||
                            ReadVCALText(Line, "TEL;PREF;HOME",         Buff) ||
                            ReadVCALText(Line, "TEL;HOME;VOICE",        Buff) ||
                            ReadVCALText(Line, "TEL;PREF;HOME;VOICE",   Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Number_Home;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "NOTE", Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Text_Note;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "ADR", Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Text_Postal;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "EMAIL", Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Text_Email;
                                Pbk->EntriesNum++;
                        }
                        if (ReadVCALText(Line, "URL", Buff)) {
                                CopyUnicodeString(Pbk->Entries[Pbk->EntriesNum].Text,Buff);
                                Pbk->Entries[Pbk->EntriesNum].EntryType = PBK_Text_URL;
                                Pbk->EntriesNum++;
                        }
                        break;
                }
        }

        if (Pbk->EntriesNum == 0) return ERR_EMPTY;
        return ERR_NONE;
}

/* -------------- OLD functions (c) by Timo Teras -------------------------- */

#ifndef ENABLE_LGPL

static void ParseVCardLine(char **pos, char *Name, char *Parameters, char *Value)
{
        int i;

        Name[0] = Parameters[0] = Value[0] = 0;

        if (**pos == 0) return;

        for (i=0; **pos && **pos != ':' && **pos != ';'; i++, (*pos)++) Name[i] = **pos;
        Name[i] = 0;

        //dbgprintf("ParseVCardLine: name tag = '%s'\n", Name);
        if (**pos == ';') {
                (*pos)++;
                for (i=0; **pos && **pos != ':'; i++, (*pos)++) Parameters[i] = **pos;
                Parameters[i] = ';';
                Parameters[i+1] = 0;
                //dbgprintf("ParseVCardLine: parameter tag = '%s'\n", Parameters);
        }

        if (**pos != 0) (*pos)++;

        i=0;
        while (**pos) {
                if ((*pos)[0] == '\x0d' && (*pos)[1] == '\x0a') {
                        (*pos) += 2;
                        if (**pos != '\t' && **pos != ' ') break;
                        while (**pos == '\t' || **pos == ' ') (*pos)++;
                        continue;
                }
                Value[i++] = **pos;
                (*pos)++;
        }
        Value[i] = 0;

        //dbgprintf("ParseVCardLine: value tag = '%s'\n", Value);
}

void DecodeVCARD21Text(char *VCard, GSM_MemoryEntry *pbk)
{
        char *pos = VCard;
        char Name[32], Parameters[256], Value[1024];

        dbgprintf("Parsing VCard:\n%s\n", VCard);

        ParseVCardLine(&pos, Name, Parameters, Value);
        if (!mystrncasecmp(Name, "BEGIN", 0) || !mystrncasecmp(Value, "VCARD", 0)) {
                dbgprintf("No valid VCARD signature\n");
                return;
        }

        while (1) {
                GSM_SubMemoryEntry *pbe = &pbk->Entries[pbk->EntriesNum];

                ParseVCardLine(&pos, Name, Parameters, Value);
                if (Name[0] == 0x00 ||
                    (mystrncasecmp(Name, "END", 0) && mystrncasecmp(Value, "VCARD", 0)))
                        return;

                if (mystrncasecmp(Name, "N", 0)) {
                        //FIXME: Name is tagged field which should be parsed
                        pbe->EntryType = PBK_Text_Name;
                        EncodeUnicode(pbe->Text, Value, strlen(Value));
                        pbk->EntriesNum++;
                } else if (mystrncasecmp(Name, "EMAIL", 0)) {
                        pbe->EntryType = PBK_Text_Email;
                        EncodeUnicode(pbe->Text, Value, strlen(Value));
                        pbk->EntriesNum++;
                } else if (mystrncasecmp(Name, "TEL", 0)) {
                        if (strstr(Parameters, "WORK;"))
                                pbe->EntryType = PBK_Number_Work;
                        else if (strstr(Name, "HOME;"))
                                pbe->EntryType = PBK_Number_Home;
                        else if (strstr(Name, "FAX;"))
                                pbe->EntryType = PBK_Number_Fax;
                        else    pbe->EntryType = PBK_Number_General;

                        EncodeUnicode(pbe->Text, Value, strlen(Value));
                        pbk->EntriesNum++;
                }
        }
}

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
