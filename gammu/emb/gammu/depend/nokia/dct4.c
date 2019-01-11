/* (c) 2002-2004 by Marcin Wiacek */

#include "../../../common/gsmstate.h"

#ifdef GSM_ENABLE_NOKIA_DCT4

#include <string.h>

#include "dct4.h"
#include "../../gammu.h"
#include "../../../common/phone/pfunc.h"
#include "../../../common/phone/nokia/nfunc.h"
#include "../../../common/phone/nokia/dct4/dct4func.h"
#include "../../../common/misc/coding/coding.h"

static GSM_Reply_Function UserReplyFunctions4[];

/* ------- some usefull functions ----------------------------------------- */

GSM_Error CheckDCT4Only()
{
	bool found = false;

	/* Checking if phone is DCT4 */
#ifdef GSM_ENABLE_NOKIA3650
 	if (strstr(N3650Phone.models, s.Phone.Data.ModelInfo->model) != NULL) found = true;
#endif
#ifdef GSM_ENABLE_NOKIA6510
 	if (strstr(N6510Phone.models, s.Phone.Data.ModelInfo->model) != NULL) found = true;
#endif
#ifdef GSM_ENABLE_NOKIA3320
 	if (strstr(N3320Phone.models, s.Phone.Data.ModelInfo->model) != NULL) found = true;
#endif
	if (!found) return ERR_NOTSUPPORTED;

	if (s.ConnectionType!=GCT_MBUS2	     && s.ConnectionType!=GCT_FBUS2      &&
	    s.ConnectionType!=GCT_FBUS2DLR3  && s.ConnectionType!=GCT_PHONETBLUE &&
	    s.ConnectionType!=GCT_IRDAPHONET && s.ConnectionType!=GCT_BLUEPHONET &&
	    s.ConnectionType!=GCT_FBUS2DKU5) {
		return ERR_OTHERCONNECTIONREQUIRED;
	}
	return ERR_NONE;
}

static void CheckDCT4()
{
	GSM_Error error;

	error = CheckDCT4Only();
	switch (error) {
	case ERR_NOTSUPPORTED:
		Print_Error(ERR_NOTSUPPORTED);
		break;
	case ERR_OTHERCONNECTIONREQUIRED:
		printf("Can't do it with current phone protocol\n");
		GSM_TerminateConnection(&s);
		exit(-1);
	default:
		break;
	}
}

static bool answer_yes2(char *text)
{
    	int         len;
    	char        ans[99];

	while (1) {
		printf("%s (yes/no) ? ",text);
		len=GetLine(stdin, ans, 99);
		if (len==-1) exit(-1);
		if (mystrncasecmp(ans, "yes",0)) return true;
		if (mystrncasecmp(ans, "no" ,0)) return false;
	}
}

/* ------------------- functions ------------------------------------------- */

static DCT4_Feature DCT4Features[] = {
	{DCT4_ALWAYS_ONLINE,     	 "GPRS Always Online",		 {{0,"on (Context)"},{1,"off (Attach)"},{0,""}}},///??
	{DCT4_GPRS_PCCH,	 	 "PCCH support for GPRS",	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_GEA1,			 "GEA1 support indication",	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_EOTD,			 "EOTD support",		 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_WAP_PUSH,			 "WAP push",			 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_USE_PREF_SIM_NET,		 "Use SIM preffered network list",{{1,"on"},{0,"off"},{0,""}}},
	{DCT4_JAVA_TCK,	                 "Java TCK support",             {{1,"on"},{0,"off"},{0,""}}},

	{DCT4_ALS,		 	 "Alternate Line Service (ALS)", {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_A52,		 	 "Ciphering alghoritm A52", 	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_CSP,		 	 "Customer Service Profile",	 {{0,"off"},{1,"on"},{0,""}}},
	{DCT4_EONS,			 "EONS support",		 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_3GINDICATOR,		 "3G indicator",		 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_DISPLAY_PHONE_NAME,	 "Display both number and name for incoming calls",{{1,"on"},{0,"off"},{0,""}}},
	{DCT4_DISPLAY_WAP_PROFILE,	 "Display selected WAP profile name instead of Home option menu in Services",{{1,"on"},{0,"off"},{0,""}}},

	{DCT4_GAMES_WAP_DOWNLOAD,	 "Games WAP download",		 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_GAMES_SCORE_SEND,		 "Games WAP score send",	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_GAMES_URL_CHECK,		 "Games URL check",		 {{1,"on"},{0,"off"},{0,""}}},

	{DCT4_BLUETOOTH_MENU,	 	 "Bluetooth menu",		 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_WAP_BOOKMARKS_MENU,	 "Bookmarks menu in Services", 	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_WAP_BOOKMARKS_MENU2,	 "Bookmarks menu in Services", 	 {{3,"bookmarks & download"},{0,"off"},{0,""}}},
	{DCT4_WAP_GOTO_MENU,	 	 "GoTo menu in Services",	 {{0,"on"},{1,"off"},{0,""}}},
	{DCT4_WAP_SETTINGS_MENU,	 "Profiles menu in Services",	 {{0,"on"},{1,"off"},{0,""}}},
	{DCT4_SERVICES_GAMES_APP_GALLERY,"Services menu in Games/Apps/Gallery",{{1,"on"},{0,"off"},{0,""}}},
	{DCT4_JAVA_GAMES_MENU,		 "Java games menu in Games",	 {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_SAT_CONFIRM_MENU,		 "Can use confirming SIM service actions", {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_INSTANT_MESS_MENU,	 "Instant Messaging in Messages",{{1,"on"},{0,"off"},{0,""}}},
	{DCT4_CONFIRM_ALS,		 "Confirm using ALS",            {{1,"on"},{0,"off"},{0,""}}},
	{DCT4_BOOKMARK_GOTO_MENU,	 "Bookmarks in GoTo menu",       {{1,"on"},{0,"off"},{0,""}}},

	{DCT4_5100_IDENTIFY,		 "Phone identification",         {{1,"NPM-6U"},{0,"NPM-6"},{0,""}}},

#ifdef DEBUG
	{DCT4_TEST,"",{{1,"1"},{0,"0"}}},
#endif

	{0,			 	 "",				 {{0,""}}}
};

static DCT4_Phone_Features DCT4PhoneFeatures[] = {
/*3100*/ {"RH-19",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,4},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {DCT4_EONS,28},{DCT4_3GINDICATOR,30},{DCT4_INSTANT_MESS_MENU,33},
			 {DCT4_CONFIRM_ALS,35},
			 {0,0}}},
/*3200*/ {"RH-30",	{{DCT4_ALS,2},{DCT4_A52,4},{DCT4_CSP,5},{DCT4_GPRS_PCCH,14},
			 {DCT4_GEA1,15},{DCT4_EOTD,18},{DCT4_WAP_SETTINGS_MENU,20},
			 {DCT4_DISPLAY_PHONE_NAME,21},{DCT4_WAP_GOTO_MENU,23},
			 {DCT4_SERVICES_GAMES_APP_GALLERY,26},{DCT4_3GINDICATOR,28},
			 {DCT4_DISPLAY_WAP_PROFILE,31},{DCT4_SAT_CONFIRM_MENU,33},
			 {DCT4_CONFIRM_ALS,34},{DCT4_EONS,40},{DCT4_ALWAYS_ONLINE,45},
			 {0,0}}},
/*3200*/ {"RH-31",	{{DCT4_ALS,2},{DCT4_A52,4},{DCT4_CSP,5},{DCT4_GPRS_PCCH,14},
			 {DCT4_GEA1,15},{DCT4_EOTD,18},{DCT4_WAP_SETTINGS_MENU,20},
			 {DCT4_DISPLAY_PHONE_NAME,21},{DCT4_WAP_GOTO_MENU,23},
			 {DCT4_SERVICES_GAMES_APP_GALLERY,26},{DCT4_3GINDICATOR,28},
			 {DCT4_DISPLAY_WAP_PROFILE,31},{DCT4_SAT_CONFIRM_MENU,33},
			 {DCT4_CONFIRM_ALS,34},{DCT4_EONS,40},{DCT4_ALWAYS_ONLINE,45},
			 {0,0}}},
/*3300*/ {"NEM-1",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
		/*MORE*/ {0,0}}},
/*3510*/ {"NHM-8",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,6},
			 {DCT4_GAMES_WAP_DOWNLOAD,7},{DCT4_GAMES_SCORE_SEND,8},
			 {DCT4_GAMES_URL_CHECK,9},{DCT4_GPRS_PCCH,13},
			 {DCT4_GEA1,15},{DCT4_ALWAYS_ONLINE,18},{0,0}}},
/*3510i*/{"RH-9",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,4},{DCT4_GPRS_PCCH,9},
 			 {DCT4_DISPLAY_PHONE_NAME,14},{DCT4_WAP_GOTO_MENU,15},
			 {DCT4_WAP_SETTINGS_MENU,16},{DCT4_SERVICES_GAMES_APP_GALLERY,19},
			 {DCT4_DISPLAY_WAP_PROFILE,25},{0,0}}},
/*3650*/ {"NHL-8",	{{DCT4_ALS,1},{0,0}}},
/*5100*/ {"NPM-6",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {DCT4_EONS,28},
		//	 {DCT4_5100_IDENTIFY,10},
			 {0,0}}},
/*5100*/ {"NPM-6U",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {DCT4_EONS,28},
		//	 {DCT4_5100_IDENTIFY,10},
			 {0,0}}},
/*6100*/ {"NPL-2",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {0,0}}},
/*6220*/ {"RH-20",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,4},
			 {DCT4_GEA1,14},{DCT4_EOTD,17},{DCT4_WAP_SETTINGS_MENU,19},
			 {DCT4_DISPLAY_PHONE_NAME,20},{DCT4_WAP_GOTO_MENU,22},
			 {DCT4_WAP_BOOKMARKS_MENU2,24},{DCT4_SERVICES_GAMES_APP_GALLERY,25},
			 {DCT4_3GINDICATOR,27},{DCT4_DISPLAY_WAP_PROFILE,30},{DCT4_SAT_CONFIRM_MENU,32},
			 {DCT4_CONFIRM_ALS,33},{DCT4_JAVA_TCK,36},{DCT4_BOOKMARK_GOTO_MENU,37},
			 {0,0}}},
/*6310*/ {"NPE-4",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,6},{DCT4_GAMES_WAP_DOWNLOAD,7},
			 {DCT4_GAMES_SCORE_SEND,8},{DCT4_GAMES_URL_CHECK,9},{DCT4_BLUETOOTH_MENU,10},
			 {DCT4_GPRS_PCCH,13},{DCT4_GEA1,15},{DCT4_ALWAYS_ONLINE,18},{0,0}}},
/*6310i*/{"NPL-1",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,6},{DCT4_GAMES_WAP_DOWNLOAD,7},
			 {DCT4_GAMES_SCORE_SEND,8},{DCT4_GAMES_URL_CHECK,9},
			 {DCT4_BLUETOOTH_MENU,10},{DCT4_USE_PREF_SIM_NET,11},
			 {DCT4_GPRS_PCCH,13},{DCT4_GEA1,15},{DCT4_EOTD,16},
			 {DCT4_ALWAYS_ONLINE,17},{DCT4_JAVA_GAMES_MENU,18},
      			 {DCT4_WAP_BOOKMARKS_MENU,20},{DCT4_WAP_SETTINGS_MENU,21},
			 {DCT4_WAP_PUSH,28},{DCT4_WAP_GOTO_MENU,29},{0,0}}},
/*6510*/ {"NPM-9",	{{DCT4_ALS,1},{DCT4_A52,3},{DCT4_CSP,6},{DCT4_GAMES_WAP_DOWNLOAD,7},
			 {DCT4_GAMES_SCORE_SEND,8},{DCT4_GAMES_URL_CHECK,9},
			 {DCT4_GPRS_PCCH,13},{DCT4_GEA1,15},{DCT4_ALWAYS_ONLINE,18},{0,0}}},
/*6610*/ {"NHL-4U",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {0,0}}},
/*6800*/ {"NHL-6",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
		/*MORE*/ {0,0}}},
/*7210*/ {"NHL-4",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {0,0}}},
/*7250*/ {"NHL-4J",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
			 {0,0}}},
/*7250i*/{"NHL-4JX",	{{DCT4_ALS,1},{DCT4_CSP,4},{DCT4_GAMES_URL_CHECK,5},{DCT4_GPRS_PCCH,8},
			 {DCT4_GEA1,9},{DCT4_ALWAYS_ONLINE,11},{DCT4_EOTD,12},
			 {DCT4_DISPLAY_PHONE_NAME,17},{DCT4_WAP_GOTO_MENU,18},
			 {DCT4_WAP_SETTINGS_MENU,19},{DCT4_SERVICES_GAMES_APP_GALLERY,22},
			 {DCT4_DISPLAY_WAP_PROFILE,26},{DCT4_SAT_CONFIRM_MENU,27},
		/*MORE*/ {0,0}}},
/*8310*/{"NHM-7",	{{DCT4_ALS,1},{DCT4_CSP,6},{DCT4_GAMES_WAP_DOWNLOAD,7},
			 {DCT4_GAMES_SCORE_SEND,8},{DCT4_GAMES_URL_CHECK,9},{DCT4_GPRS_PCCH,13},
			 {DCT4_ALWAYS_ONLINE,18},{0,0}}},
	{"",		{{0,0}}}
};

static GSM_Error DCT4_ReplySetPPS(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	printf("Setting done OK\n");
	return ERR_NONE;
}

void DCT4SetPhoneMenus(int argc, char *argv[])
{
	int 		current = 10,i=0,j,z;
	unsigned char 	reqSet[200] = {
		N7110_FRAME_HEADER,0x04,0x00,0x01,0x47,0x48,0x02,
		0x00};		/* Number of changed features */

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	while (DCT4PhoneFeatures[i].Model[0] != 0x00) {
		if (!strcmp(DCT4PhoneFeatures[i].Model,s.Phone.Data.Model)) {
			j = 0;
			while (DCT4PhoneFeatures[i].Features[j].Name != 0x00) {
				z = 0;
				while (DCT4Features[z].Name != 0x00) {
					if (DCT4Features[z].Name == DCT4PhoneFeatures[i].Features[j].Name) {
						printf("%s : %s\n",DCT4Features[z].Text,DCT4Features[z].Values[0].Text);
						reqSet[9]++;							/* Number of features */
						reqSet[current++] = DCT4PhoneFeatures[i].Features[j].Number; 	/* Feature number */
						reqSet[current++] = DCT4Features[z].Values[0].Value;		/* Value */
						break;
					}
					z++;
				}
				j++;
			}
		}
		i++;
	}

	if (current == 10) {
		printf("Sorry, but configuration matrix for this model is not added yet. Please report\n");
		return;
	}

	reqSet[current++] = 0x00;
	reqSet[current++] = 0x00;

	error=GSM_WaitFor (&s, reqSet, current, 0x1b, 4, ID_User1);
	Print_Error(error);
}

DCT4_Phone_Tests DCT4Tests;

static GSM_Error DCT4_ReplyTestsNames(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int i,pos;

	DCT4Tests.Num   = msg.Buffer[5];
	pos 		= 6;

	smprintf(s,"%i names for phone tests received\n",msg.Buffer[5]);
	for (i=0;i<msg.Buffer[5];i++) {
		strcpy(DCT4Tests.Tests[i].Name,msg.Buffer+pos+4);
		DCT4Tests.Tests[i].ID = msg.Buffer[pos+2];
		smprintf(s,"%x.\"%s\"\n",DCT4Tests.Tests[i].ID,DCT4Tests.Tests[i].Name);
		pos+=msg.Buffer[pos+1];
	}

	return ERR_NONE;
}

static GSM_Error DCT4_ReplyTestsStartup(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int 	 i,pos,j;
	bool	 found;

	pos = 10;

	for (i=0;i<msg.Buffer[8];i++) {
		found = false;
		for (j=0;j<DCT4Tests.Num;j++) {
			if (DCT4Tests.Tests[j].ID == msg.Buffer[pos]) {
				DCT4Tests.Tests[j].Startup 	= true;
				found 				= true;
				break;
			}
		}
		if (!found) printf("%x ",msg.Buffer[pos]);
		pos++;
	}

	return ERR_NONE;
}

static GSM_Error DCT4_ReplyTestsStatus(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int i,pos,j;

	pos = 6;

	smprintf(s,"%i status entries for phone tests received\n",msg.Buffer[5]);
	for (i=0;i<msg.Buffer[5];i++) {
		for (j=0;j<DCT4Tests.Num;j++) {
			if (DCT4Tests.Tests[j].ID == msg.Buffer[pos+2]) {
				printf("\"%40s\" : ",DCT4Tests.Tests[j].Name);
				switch(msg.Buffer[pos+3]) {
					case 0x00: printf("Passed"); 		break;
					case 0x01: printf("Fail");   		break;
					case 0x03: printf("Not executed"); 	break;
					case 0x06: printf("No signal");		break;
					case 0x0D: printf("Timeout");		break;
					default  : printf("Unknown (%x)",msg.Buffer[pos+3]);
				}
				if (DCT4Tests.Tests[j].Startup) printf(" (startup)");
				printf("\n");				
				break;
			}
		}
		pos+=msg.Buffer[pos+1];
	}

	return ERR_NONE;
}

void DCT4SelfTests(int argc, char *argv[])
{
	int 	      j;
	unsigned char GetDoneST[6]    = {0x00, 0x08, 0x01, 0x04, 0x01, 0x00};
	unsigned char GetDoneST2[6]   = {0x00, 0x08, 0x02, 0x04, 0x02, 0x00};
	unsigned char GetNames[6]     = {0x00, 0x08, 0x03, 0x06, 0x03, 0x00};
	unsigned char GetStatus[6]    = {0x00, 0x08, 0x04, 0x02, 0x03, 0x00}; 

	unsigned char RunALL[6]       = {0x00, 0x06, 0x04, 0x00, 0x03, 0x00};

//	unsigned char GetID[6]        = {0x00, 0x08, 0x00, 0x04, 0x03, 0x00};//tests ID

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	if (answer_yes2("Run all tests now ?")) {
		error=GSM_WaitFor (&s, RunALL, 6, 0x35, 4, ID_User1);
		Print_Error(error);
	}

	error=GSM_WaitFor (&s, GetNames, 6, 0x35, 4, ID_User1);
	Print_Error(error);

	for (j=0;j<DCT4Tests.Num;j++) DCT4Tests.Tests[j].Startup = false;

	error=GSM_WaitFor (&s, GetDoneST, 6, 0x35, 4, ID_User3);
	Print_Error(error);

	error=GSM_WaitFor (&s, GetDoneST2, 6, 0x35, 4, ID_User3);
	Print_Error(error);

	error=GSM_WaitFor (&s, GetStatus, 6, 0x35, 4, ID_User2);
	Print_Error(error);
}

static GSM_Error DCT4_ReplyVibra(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
#ifdef DEBUG
	switch (msg.Buffer[3]) {
		case 0x0D : dbgprintf("Vibra state set OK\n"); break;
		case 0x0F : dbgprintf("Vibra power set OK\n"); break;
	}
#endif
	return ERR_NONE;
}

static GSM_Error DCT4EnableVibra(GSM_StateMachine *s, bool enable)
{
	/* Enables or disables vibra */
	unsigned char 	Control[6] = {N7110_FRAME_HEADER,0x0C,
		0x01,		/* 0x01 = On, 0x00 = Off */
		0x00};

	if (!enable) Control[4] = 0x00;
	return GSM_WaitFor (s, Control, 6, 0x1C, 4, ID_User3);
}

void DCT4SetVibraLevel(int argc, char *argv[])
{
	GSM_DateTime	Date;
	unsigned int	i,j;

	/* Set vibra level */
	unsigned char 	SetLevel[6] = {N7110_FRAME_HEADER,0x0E,
				       0x64,	/* Vibra power (in percent) */
				       0x00};

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	SetLevel[4] = atoi(argv[2]);
	error=GSM_WaitFor (&s, SetLevel, 6, 0x1C, 4, ID_User3);
	Print_Error(error);

	error=DCT4EnableVibra(&s, true);
	Print_Error(error);

	for (i=0;i<3;i++) {
		GSM_GetCurrentDateTime (&Date);
		j=Date.Second;
		while (j==Date.Second) {
			my_sleep(10);
			GSM_GetCurrentDateTime(&Date);
		}
	}

	error=DCT4EnableVibra(&s, false);
	Print_Error(error);

	GSM_Terminate();
}

void DCT4VibraTest(int argc, char *argv[])
{
	unsigned char ans[200];

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	error=DCT4EnableVibra(&s, true);
	Print_Error(error);

	printf("Press any key to continue...\n");
	GetLine(stdin, ans, 99);

	error=DCT4EnableVibra(&s, false);
	Print_Error(error);
}

#ifdef DEBUG
static GSM_Error DCT4_ReplyResetSecurityCode(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	switch (msg.Buffer[3]) {
	case 0x05:
		printf("Security code set to \"12345\"\n");
		return ERR_NONE;
	case 0x06:
		printf("Unknown reason. Can't reset your security code\n");
		return ERR_UNKNOWN;
	}
	return ERR_UNKNOWNRESPONSE;
}

void DCT4ResetSecurityCode(int argc, char *argv[])
{
	unsigned int	i;
	unsigned char 	ResetCode[30] = {0x00,0x06,0x03,0x04,0x01,
		'1','2','3','4','5','6','7','8','9','0',	/* Old code */
		0x00,
		'1','2','3','4','5',0x00,0x00,0x00,0x00,0x00,   /* New code */
		0x00};

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	error=GSM_WaitFor (&s, ResetCode, 27, 0x08, 4, ID_User2);
	if (error == ERR_UNKNOWN) {
		if (answer_yes2("Try brutal force ?")) {
			for (i=10000;i<9999999;i++) {
				printf("Trying %i\n",i);
				memset(ResetCode+6,0,22);
				sprintf(ResetCode+5,"%i",i);
				sprintf(ResetCode+16,"12345");
				error=GSM_WaitFor (&s, ResetCode, 27, 0x08, 4, ID_User2);
				if (error == ERR_NONE) break;
			}
		}
	} else Print_Error(error);
}
#endif

char SecLength;

static GSM_Error DCT4_ReplyGetSecurityCode(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	if (msg.Length > 12) {
		SecLength = msg.Buffer[13];
		if ((msg.Buffer[17]+18) == msg.Length) {
			printf("Security code is %s\n",msg.Buffer+18);
//			DumpMessage(stdout, msg.Buffer, msg.Length);
		}
	}
	return ERR_NONE;
}

void DCT4GetSecurityCode(int argc, char *argv[])
{
	GSM_Error 	error;
	unsigned char 	getlen[]={0x00, 0x08, 0x01, 0x0C, 
				  0x00, 0x23, 		//ID 
				  0x00, 0x00, 		//Index
				  0x00, 0x00}; 
	unsigned char 	read[]={0x00, 0x08, 0x02, 0x04,
				0x00, 0x23, 		//ID
				0x00, 0x00, 		//Index
				0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00};                  //Length

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	SecLength = 0;
	error=GSM_WaitFor (&s, getlen, sizeof(getlen), 0x23, 1, ID_User1);
	Print_Error(error);
	if (SecLength != 0) {
		read[17] = SecLength;
		error=GSM_WaitFor (&s, read, sizeof(read), 0x23, 5, ID_User1);
		Print_Error(error);
	}
}

static GSM_Error DCT4_ReplyGetVoiceRecord(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int 		i=18,j;
	unsigned char	Buffer[100];

	switch (msg.Buffer[3]) {
	case 0x05:
		dbgprintf("Part of voice record received\n");
		if (msg.Length == 6) {
			dbgprintf("Empty\n");
			return ERR_EMPTY;
		}
 		*s->Phone.Data.VoiceRecord = 0;
		while (i<msg.Length) {
 			s->Phone.Data.PhoneString[(*s->Phone.Data.VoiceRecord)++] = msg.Buffer[i+1];
 			s->Phone.Data.PhoneString[(*s->Phone.Data.VoiceRecord)++] = msg.Buffer[i];
			i += 2;
		}
		return ERR_NONE;
	case 0x0D:
		dbgprintf("Last part of voice record is %02x %02x\n",msg.Buffer[11],msg.Buffer[12]);
		dbgprintf("Token is %02x\n",msg.Buffer[13]);
 		s->Phone.Data.PhoneString[0] = msg.Buffer[11];
 		s->Phone.Data.PhoneString[1] = msg.Buffer[12];
 		s->Phone.Data.PhoneString[2] = msg.Buffer[13];
		return ERR_NONE;
		break;
	case 0x31:
		dbgprintf("Names of voice records received\n");
		j = 33;
		for (i=0;i<msg.Buffer[9];i++) {
			memcpy(Buffer,msg.Buffer+(j+1),msg.Buffer[j]);
			Buffer[msg.Buffer[j]] 	= 0;
			Buffer[msg.Buffer[j]+1] = 0;
			dbgprintf("%i. \"%s\"\n",i+1,DecodeUnicodeString(Buffer));
 			if (i==*s->Phone.Data.VoiceRecord) {
 				sprintf(s->Phone.Data.PhoneString,"%s.wav",DecodeUnicodeString(Buffer));
				return ERR_NONE;
			}
			if (i != msg.Buffer[9] - 1) {
				j+=msg.Buffer[j] + 1;
				if (msg.Buffer[j] == 0x00 && msg.Buffer[j+1]==0x00) j+=2;
				j+=23;
			}
		}
		return ERR_EMPTY;
	}
	return ERR_UNKNOWNRESPONSE;
}

void DCT4GetVoiceRecord(int argc, char *argv[])
{
	/* Voice records names */
	unsigned char 	ReqNames[200] = {
		N7110_FRAME_HEADER,
		0x30,0x01,0x55,0x00,0x00,0xFF,0xFF,0x01,0x01,0x55,0x55};
	/* Voice record token */
	unsigned char 	ReqToken[200] = {
		N7110_FRAME_HEADER,0x0C,0x00,0x44,0x00,
		0x00,		/* Location: 0, 1, ... */
		0x55,0x55};
	/* Voice record part */
	unsigned char 	ReqGet[200] = {
		N7110_FRAME_HEADER,0x04,0x00,0x44,
		0x00,0x00,	/* Location: 0, 1, ...  */
		0x55,0x55,0x00,

		0x00,0x00,	/* Part Location	*/
		0x00,0x00,0x00,

		0x04,		/* ???			*/

		0x00};		/* Token		*/

	/* WAV file headers */
	unsigned char 	WAV_Header[] = {
			'R','I','F','F',
			0x00,0x00,0x00,0x00,	/* Length */
			'W','A','V','E'};
	unsigned char 	FMT_Header[] = {'f','m','t',' ',
			0x14,0x00,0x00,0x00,0x31,0x00,0x01,0x00,0x40,0x1f,
			0x00,0x00,0x59,0x06,0x00,0x00,0x41,0x00,0x00,0x00,
			0x02,0x00,0x40,0x01,'f', 'a', 'c', 't', 0x04,0x00,
			0x00,0x00,
			0x00,0x73,0x00,0x00};	/* Seems to be some length */
	unsigned char 	DATA_Header[] = {
			'd','a','t','a',
			0x00,0x00,0x00,0x00};	/* Length */

	long		wavfilesize=0;
	unsigned char	FileName[100], Buffer[10000], Token;
	unsigned int 	Location, size=0, CurrentLocation = 0, TokenLocation;
	int		i;
	FILE		*WAVFile;

	Location = atoi(argv[2]);
	if (Location == 0x00) {
		printf("Please numerate locations from 1\n");
		return;
	}
	Location--;

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	s.Phone.Data.VoiceRecord 	= &Location;
	s.Phone.Data.PhoneString 	= FileName;
	dbgprintf("Getting voice record name\n");
	error=GSM_WaitFor (&s, ReqNames, 14, 0x4A, 4, ID_User4);
	Print_Error(error);
	
	s.Phone.Data.PhoneString 	= Buffer;
	ReqToken[7] 			= Location;
	dbgprintf("Getting voice record token\n");
	error=GSM_WaitFor (&s, ReqToken, 10, 0x23, 4, ID_User4);
	Print_Error(error);
	TokenLocation 			= Buffer[0] * 256 + Buffer[1];
	Token				= Buffer[2];

	WAVFile = fopen(FileName, "wb");      

	fwrite(&WAV_Header,	1, sizeof(WAV_Header),	WAVFile);
	fwrite(&FMT_Header,	1, sizeof(FMT_Header),	WAVFile);
	fwrite(&DATA_Header,	1, sizeof(DATA_Header),	WAVFile);

	s.Phone.Data.VoiceRecord 	= &size;
	s.Phone.Data.PhoneString 	= Buffer;
	ReqGet[7]			= Location;
	fprintf(stderr,"Getting voice record and saving to \"%s\": ",FileName);
	while (1) {
		dbgprintf("Getting next part of voice record\n");
		fprintf(stderr,".");
		error=GSM_WaitFor (&s, ReqGet, 18, 0x23, 4, ID_User4);
		if (error == ERR_NONE) {
			wavfilesize += size;
			fwrite(Buffer,1,size,WAVFile);
		}
		if (error == ERR_EMPTY) break;
		Print_Error(error);
		CurrentLocation += 4;
		ReqGet[11] = CurrentLocation / 256;
		ReqGet[12] = CurrentLocation % 256;
		if (CurrentLocation+4 > TokenLocation) break;
	}
	dbgprintf("Getting first part in last sequence of voice record\n");
	for (i=255;i>=0;i--) {
		ReqGet[16] = i;
		ReqGet[17] = Token;
		fprintf(stderr,".");
		error=GSM_WaitFor (&s, ReqGet, 18, 0x23, 4, ID_User4);
		if (error == ERR_NONE) {
			wavfilesize += size;
			fwrite(Buffer,1,size,WAVFile);
			break;
		}
		if (error != ERR_EMPTY) Print_Error(error);
	}
	while (1) {
		dbgprintf("Getting next part of last sequence in voice record\n");
		CurrentLocation += 4;
		ReqGet[11] = CurrentLocation / 256;
		ReqGet[12] = CurrentLocation % 256;
		fprintf(stderr,".");
		error=GSM_WaitFor (&s, ReqGet, 18, 0x23, 4, ID_User4);
		if (error == ERR_NONE) {
			wavfilesize += size;
			fwrite(Buffer,1,size,WAVFile);
		}
		if (error == ERR_EMPTY) break;
		Print_Error(error);
	}
	fprintf(stderr,"\n");

	wavfilesize 	+= sizeof(WAV_Header) + sizeof(FMT_Header) + sizeof(DATA_Header);
	WAV_Header[4] 	= (unsigned char)(wavfilesize % 256);
	WAV_Header[5] 	= (unsigned char)(wavfilesize / 256);
	WAV_Header[6] 	= (unsigned char)(wavfilesize / (256*256));
	WAV_Header[7] 	= (unsigned char)(wavfilesize / (256*256*256));

	/* FIXME */
	FMT_Header[36]	= (unsigned char)(((wavfilesize - 238) * 5 ) % 256);
	FMT_Header[37]	= (unsigned char)(((wavfilesize - 238) * 5 ) / 256);
	FMT_Header[38]	= (unsigned char)(((wavfilesize - 238) * 5 ) / (256*256));
	FMT_Header[39]	= (unsigned char)(((wavfilesize - 238) * 5 ) / (256*256*256));

	wavfilesize 	= wavfilesize - 54 - 6;
	DATA_Header[4] 	= (unsigned char)(wavfilesize % 256);
	DATA_Header[5] 	= (unsigned char)(wavfilesize / 256);
	DATA_Header[6] 	= (unsigned char)(wavfilesize / (256*256));
	DATA_Header[7] 	= (unsigned char)(wavfilesize / (256*256*256));

	fseek( WAVFile, 0, SEEK_SET);
	fwrite(&WAV_Header,	1, sizeof(WAV_Header),	WAVFile);
	fwrite(&FMT_Header,	1, sizeof(FMT_Header),	WAVFile);
	fwrite(&DATA_Header,	1, sizeof(DATA_Header),	WAVFile);

	fclose(WAVFile);

	GSM_Terminate();
}

static GSM_Error DCT4_ReplyGetBTInfo(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	printf("device address %02x%02x%02x%02x%02x%02x\n",
		msg.Buffer[9],msg.Buffer[10],msg.Buffer[11],
		msg.Buffer[12],msg.Buffer[13],msg.Buffer[14]);
	return ERR_NONE;
}

static GSM_Error DCT4_ReplyGetSimlock(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int i;

	switch (msg.Buffer[3]) {
	case 0x0D:
		dbgprintf("Simlock info received\n");
		dbgprintf("Config_Data: ");
		for (i=14;i<22;i++) {
			dbgprintf("%02x",msg.Buffer[i]);
		}
		dbgprintf("\n");
		dbgprintf("Profile_Bits: ");
		for (i=22;i<30;i++) {
			dbgprintf("%02x",msg.Buffer[i]);
		}
		dbgprintf("\n");		
		return ERR_NONE;
	case 0x13:
		dbgprintf("Simlock info received\n");
		if (msg.Buffer[58] == 0x05 && msg.Buffer[59] == 0x02) {
			dbgprintf("SIM_PATH: ");
			for (i=44;i<52;i++) {
				dbgprintf("%02x",msg.Buffer[i]);
			}
			dbgprintf("\n");
			printf("Simlock data  : ");
			for (i=60;i<63;i++) {
				printf("%02x",msg.Buffer[i]);
			}
			printf("\n");
		}
		return ERR_NONE;
	}
	return ERR_UNKNOWNRESPONSE;
}

void DCT4Info(int argc, char *argv[])
{
	unsigned char GetBTAddress[8] = {N6110_FRAME_HEADER, 0x09, 0x19, 0x01, 0x03, 0x06};
	unsigned char GetSimlock[5] = {N6110_FRAME_HEADER, 0x12, 0x0D};
	unsigned char value[10];

        if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	if (IsPhoneFeatureAvailable(s.Phone.Data.ModelInfo, F_BLUETOOTH)) {
		printf("Bluetooth     : ");

		error=GSM_WaitFor (&s, GetBTAddress, 8, 0xD7, 4, ID_User6);
		Print_Error(error);
	}

	error=GSM_WaitFor (&s, GetSimlock, 5, 0x53, 4, ID_User6);
	Print_Error(error);
	GetSimlock[4] = 0x0E;
	error=GSM_WaitFor (&s, GetSimlock, 5, 0x53, 4, ID_User6);
	Print_Error(error);
	GetSimlock[3] = 0x0C;
	error=GSM_WaitFor (&s, GetSimlock, 4, 0x53, 4, ID_User6);
	Print_Error(error);
	error=NOKIA_GetPhoneString(&s,"\x00\x03\x02\x07\x00\x08",6,0x1b,value,ID_User6,10);
	Print_Error(error);
	printf("UEM           : %s\n",value);
}

static FILE 	*T9File;
int 		T9Size;
int 		T9FullSize;

static GSM_Error DCT4_ReplyGetT9(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	T9FullSize 	= msg.Buffer[18] * 256 + msg.Buffer[19];
	T9Size 		= msg.Length - 18;
	fwrite(msg.Buffer+18,1,T9Size,T9File);
	return ERR_NONE;
}

void DCT4GetT9(int argc, char *argv[])
{
	int	      i,T9Dictionary=0;
	unsigned char req[] = {N7110_FRAME_HEADER, 0x04, 0x00, 0x5B,
			       0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			       0x00, 0x00,	/* Start position */
			       0x00, 0x00,
			       0x02, 0xBC};	/* How many bytes to read */

	if (CheckDCT4Only()!=ERR_NONE) return;

	T9File = fopen("T9", "w");      
	if (T9File == NULL) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	i = 0;
	while (1) {
		req[12] = i / 256;
		req[13] = i % 256;
		if (i != 0) {
			if (T9Dictionary - i < req[16]*256+req[17]) {
				req[16] = (T9Dictionary - i) / 256;
				req[17] = (T9Dictionary - i) % 256;
			}
			if (T9Dictionary - i == 0) break;
		}
		error=GSM_WaitFor (&s, req, 18, 0x23, 4, ID_User3);		
		Print_Error(error);
		if (i==0) {
			T9Dictionary = T9FullSize;
			dbgprintf("T9 dictionary size is %i\n",T9Dictionary);
		}
		i+=T9Size;
	}

	fclose(T9File);
}

#ifdef GSM_ENABLE_NOKIA6510

extern GSM_Error N6510_SetLight(GSM_StateMachine *s, N6510_PHONE_LIGHTS light, bool enable);

void DCT4SetLight(int argc, char *argv[])
{
	int			i;
	N6510_PHONE_LIGHTS 	type;
	bool			enable;

	if (mystrncasecmp(argv[2],"display",0)) { 	type = N6510_LIGHT_DISPLAY;
	} else if (mystrncasecmp(argv[2],"keypad",0)) {	type = N6510_LIGHT_KEYPAD;
	} else if (mystrncasecmp(argv[2],"torch",0)) {	type = N6510_LIGHT_TORCH;
	} else {
		printf("What lights should I enable (\"%s\") ?\n",argv[2]);
		exit(-1);
	}

	if (mystrncasecmp(argv[3],"on",0)) { 		enable = true;
	} else if (mystrncasecmp(argv[3],"off",0)) {	enable = false;
	} else {
		printf("What should I do (\"%s\") ?\n",argv[3]);
		exit(-1);
	}

	for (i=0;i<s.ConfigNum;i++) {
		s.Config[i].StartInfo = "false";
	}

	GSM_Init(true);

        CheckDCT4();

	error=N6510_SetLight(&s, type, enable);
	Print_Error(error);

	GSM_Terminate();
}
#endif

void DCT4DisplayTest(int argc, char *argv[])
{
	unsigned char ans[200];
	unsigned char req0[] = {0x00, 0x08, 0x0D, 0x00, 0x0F, 0x00};
	unsigned char req[] =  {0x00, 0x08, 0x0E, 0x00, 0x12, 0x01, 0x00, 0x04,
				0x09,		/* test number */
				0x00};

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	DCT4_SetPhoneMode(&s, DCT4_MODE_TEST);

	s.Protocol.Functions->WriteMessage(&s, req0, 6, 0x40);

	req[8] = atoi(argv[2]);
	s.Protocol.Functions->WriteMessage(&s, req, 10, 0x40);

	printf("Press any key to continue...\n");
	GetLine(stdin, ans, 99);

	DCT4_SetPhoneMode(&s, DCT4_MODE_NORMAL);
}

int ADC;

static GSM_Error DCT4_ReplyGetADC(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	if (msg.Buffer[6] == 0xff && msg.Buffer[7] == 0xff) return ERR_NONE;
	switch (msg.Buffer[3]) {
	case 0x10:
		printf("raw ");
		printf("%10i ",msg.Buffer[8]*256+msg.Buffer[9]);
		break;
	case 0x12:
		printf("unit result ");
		printf("%10i ",(msg.Buffer[8]*256+msg.Buffer[9])*ADC);
		break;
	}
	return ERR_NONE;
}

struct DCT4ADCInfo {
	char 	*name;
	char 	*unit;
	int 	x;
};

static struct DCT4ADCInfo DCT4ADC[] = {
	{"Battery voltage, divided:",		"mV",  1},
	{"Battery voltage, scaled:",		"mV",  1},
	{"Charger voltage:",			"mV",  1},  	
	{"Charger current:",			"mA",  1},
	{"Battery size indicator:",		"Ohms",100},
	{"Battery temperature:",		"K",   1},
	{"Headset interconnection:",		"mV",  1},
	{"Hook interconnection:",		"mV",  1},
	{"Light sensor:",			"mV",  1},
	{"Power amplifier temperature:",	"K",   1},
	{"VCXO temperature:",			"K",   1},
	{"Resistive keyboard 1/headint2:",	"mV",  1},
	{"Resistive keyboard 1/auxdet:",	"mV",  1},
	{"Initial battery voltage:",		"mV",  1},
	{"Battery Current:",			"mA",  1},
	{"Battery Current Fast:",		"mA",  1},

	{"", "", 1}
};

void DCT4GetADC(int argc, char *argv[])
{
	int		i = 0;
	unsigned char 	GetRaw[]  = {N6110_FRAME_HEADER, 0x0F,
				     0x00,		/* Test number */
			             0x01};
	unsigned char 	GetUnit[] = {N6110_FRAME_HEADER, 0x11,
				     0x00,		/* Test number */
				     0x01};

	if (CheckDCT4Only()!=ERR_NONE) return;

	s.User.UserReplyFunctions=UserReplyFunctions4;

	while (1) {
		printf(" %30s ",DCT4ADC[i].name);
		GetRaw[4] = i;
		error=GSM_WaitFor (&s, GetRaw, 6, 0x17, 4, ID_User3);		
		Print_Error(error);
		GetUnit[4] 	= i;
		ADC		= DCT4ADC[i].x;
		error=GSM_WaitFor (&s, GetUnit, 6, 0x17, 4, ID_User3);		
		Print_Error(error);
		printf("%s\n",DCT4ADC[i].unit);
		i++;
		if (DCT4ADC[i].name[0] == 0x00) break;
	}
}

#ifdef GSM_ENABLE_NOKIA6510

static double 		RadioFreq;
static unsigned char 	RadioName[100];

static GSM_Error DCT4_ReplyTuneRadio(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	int 		length;
	unsigned char 	name[100];

	switch (msg.Buffer[3]) {
	case 0x09:
		N6510_DecodeFMFrequency(&RadioFreq, msg.Buffer+16);

 		length = msg.Buffer[8];
 		memcpy(name,msg.Buffer+18,length*2);
 		name[length*2]	 = 0x00;
 		name[length*2+1] = 0x00;
 		CopyUnicodeString(RadioName,name);
		smprintf(s,"Station name: \"%s\"\n",DecodeUnicodeString(RadioName));
		return ERR_NONE;
	case 0x15:
	case 0x16:
		smprintf(s,"Response for enabling radio/headset status received\n");
		if (msg.Buffer[5] == 0) {
			smprintf(s,"Connected\n");
			return ERR_NONE;
		}
		smprintf(s,"Probably not connected\n");
		return ERR_PERMISSION;
	}
	return ERR_UNKNOWNRESPONSE;
}

void DCT4TuneRadio(int argc, char *argv[])
{
	double		Freq, diff;
 	GSM_FMStation 	FMStation[50],FMStat;
	int		i, j, num;
	bool		found;

	unsigned char Enable[]     = {N6110_FRAME_HEADER, 0x00, 0x00, 0x00};
	unsigned char Disable[]    = {N6110_FRAME_HEADER, 0x01, 0x0E, 0x00};
//	unsigned char SetVolume[]  = {N6110_FRAME_HEADER, 0x14,
//				      0x00,	/* Volume level */
//				      0x00};
//	unsigned char MuteUnMute[] = {N6110_FRAME_HEADER, 0x0F,
//				      0x0C,	/* 0x0B = mute, 0x0C = unmute */
//				      0x00};
	unsigned char SetFreq[]	   = {N6110_FRAME_HEADER, 0x08,
				      0x08, 0x14, 0x00, 0x01,
				      0x9A, 0x28};  /* Frequency */
//	unsigned char Find1[]	   = {N6110_FRAME_HEADER, 0x08,
//				      0x04, 0x14, 0x00, 0x00, 0x00, 0x00};
	unsigned char Find2[]	   = {N6110_FRAME_HEADER, 0x08,
				      0x05, 0x14, 0x00, 0x00, 0x00, 0x00};
//	unsigned char SetStereo[]  = {N6110_FRAME_HEADER, 0x19,
//				      0x0A, 0x00, 0x15};
//	unsigned char SetMono[]    = {N6110_FRAME_HEADER, 0x19,
//				      0x09, 0x00, 0x96};

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	FMStat.Location = 1;
	error = Phone->GetFMStation(&s,&FMStat);
	if (error != ERR_NONE && error != ERR_EMPTY) {
		printf("Phone seems not to support radio\n");
		GSM_Terminate();
		exit(-1);		
	}

	error=GSM_WaitFor (&s, Enable, 6, 0x3E, 4, ID_User3);
	if (error == ERR_PERMISSION) {
		printf("Please connect headset. Required as antenna\n");
		GSM_Terminate();
		exit(-1);
	}
	Print_Error(error);

	num=0;
	for (i=88;i<108;i++) {
		fprintf(stderr,"%cSearching: %i percent",13,(i-88)*100/(108-88));
		Freq = i;
		N6510_EncodeFMFrequency(Freq, SetFreq+8);
		error=GSM_WaitFor (&s, SetFreq, 10, 0x3E, 4, ID_User3);
		Print_Error(error);

		error=GSM_WaitFor (&s, Find2, 10, 0x3E, 4, ID_User3);
		Print_Error(error);
		found = false;
		for (j=0;j<num;j++) {
			if (FMStation[j].Frequency > RadioFreq) {
				diff = FMStation[j].Frequency - RadioFreq;
			} else {
				diff = RadioFreq - FMStation[j].Frequency;
			}
			if (diff <= 0.2) {
				dbgprintf("diff is %f\n",diff);
				found = true;
				break;
			}
		}
		if (!found) {
			dbgprintf("Adding %f, num %i\n",RadioFreq,num);
			FMStation[num].Frequency = RadioFreq;
			CopyUnicodeString(FMStation[num].StationName,RadioName);
			num++;
		}
	}
	fprintf(stderr,"%cSearching: %i percent",13,100);
	fprintf(stderr,"\n\n");

	i=0;
	while(1) {
		if (i==num || i==num-1) break;
		if (FMStation[i].Frequency > FMStation[i+1].Frequency) {
			memcpy(&FMStat,&FMStation[i],sizeof(GSM_FMStation));
			memcpy(&FMStation[i],&FMStation[i+1],sizeof(GSM_FMStation));
			memcpy(&FMStation[i+1],&FMStat,sizeof(GSM_FMStation));
			i = 0;
			continue;
		}
		i++;
	}
	for (i=0;i<num;i++) {
		fprintf(stderr,"%02i.",i+1);
		if (FMStation[i].Frequency < 100) fprintf(stderr," ");
		fprintf(stderr,"%.1f MHz - \"%s\" \n",
			FMStation[i].Frequency,
			DecodeUnicodeString(FMStation[i].StationName));
	}

	if (answer_yes2("Do you want to save found stations")) {
		fprintf(stderr,"Deleting old FM stations: ");
		error=Phone->ClearFMStations(&s);
		Print_Error(error);
		fprintf(stderr,"Done\n");
		for (i=0;i<num;i++) {
			FMStation[i].Location = i+1;
			error=Phone->SetFMStation(&s,&FMStation[i]);
			Print_Error(error);
			fprintf(stderr,"%cWriting: %i percent",13,(i+1)*100/num);
		}
		fprintf(stderr,"\n");
	}

	error=GSM_WaitFor (&s, Disable, 6, 0x3E, 4, ID_User3);
	Print_Error(error);

	GSM_Terminate();
}
#endif

void DCT4PlaySavedRingtone(int argc, char *argv[])
{
	unsigned char req[] =  {N6110_FRAME_HEADER, 
			      	0x01,
				0x00,0x64,	//id
				0x01,		//group
				0x01,0x00,0x00,
				0x0A,		//volume
				0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	GSM_AllRingtonesInfo Info;

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	error=Phone->GetRingtonesInfo(&s,&Info);
	Print_Error(error);

	if (atoi(argv[2]) > Info.Number-1) {
		GSM_Terminate();
		return;
	}
	req[4] = Info.Ringtone[atoi(argv[2])].ID / 256;
	req[5] = Info.Ringtone[atoi(argv[2])].ID % 256;
	req[6] = Info.Ringtone[atoi(argv[2])].Group;

	error=GSM_WaitFor (&s, req, 18, 0x1F, 4, ID_User3);
	Print_Error(error);

//	for (i=0;i<Info.Number;i++) printmsg("%i. \"%s\"\n",i,DecodeUnicodeConsole(Info.Ringtone[i].Name));

	GSM_Terminate();
}

static GSM_Error DCT4_ReplyMakeCameraShoot(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	return ERR_NONE;	
}

void DCT4MakeCameraShoot(int argc, char *argv[])
{
	unsigned char SetCamera[] = {N6110_FRAME_HEADER, 0x09, 0x01, 0x02};
	unsigned char CameraON[] = {N6110_FRAME_HEADER, 0x02, 0x01, 0x00, 0x00, 0x00 , 0x00, 0x00};
	unsigned char CameraON2[] = {N6110_FRAME_HEADER, 0xF0, 0x02, 0x00};
	unsigned char MakeShot[200] = {N6110_FRAME_HEADER, 0x06, 0x01, 0x06,
		0x01, 0x00, 0x00, 0x02, 0x00, 0x04, 0x32, 0x00, 0x01, 
		0x1D, 		//length of rest
		0x00, 0x00, 0x00, 0x01, 
		0x00, 0x02,	//master folder id
		0x00, 0x14}; 	//length
	unsigned char CameraOFF[] = {N6110_FRAME_HEADER, 0x04, 0x01, 0x00};

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	error=GSM_WaitFor (&s, SetCamera, 6, 0x61, 4, ID_User3);
	Print_Error(error);
	error=GSM_WaitFor (&s, CameraON, 10, 0x61, 4, ID_User3);
	Print_Error(error);
	error=GSM_WaitFor (&s, CameraON2, 6, 0x61, 4, ID_User3);
	Print_Error(error);
	EncodeUnicode(MakeShot+24,"GammuShot",9);
	MakeShot[15] = 9+9*2;
	MakeShot[23] = 9*2;
	error=GSM_WaitFor (&s, MakeShot, 24+MakeShot[23], 0x61, 4, ID_User3);
	Print_Error(error);
	error=GSM_WaitFor (&s, SetCamera, 6, 0x61, 4, ID_User3);
	Print_Error(error);
	error=GSM_WaitFor (&s, CameraOFF, 6, 0x61, 4, ID_User3);
	Print_Error(error);

	GSM_Terminate();
}

int len;

static GSM_Error DCT4_ReplyGetScreenDump(GSM_Protocol_Message msg, GSM_StateMachine *s)
{
	if (msg.Buffer[7] == 0x0C) len = 1;
	return ERR_NONE;	
}

void DCT4GetScreenDump(int argc, char *argv[])
{
	unsigned char req[] = {N6110_FRAME_HEADER, 0x07, 0x01, 0x00};
	//n6110_frameheader 06//screen info

	GSM_Init(true);

        CheckDCT4();

	s.User.UserReplyFunctions=UserReplyFunctions4;

	error=GSM_WaitFor (&s, req, 6, 0x0E, 4, ID_User3);
	Print_Error(error);
	len = 2000;
	while (len >= 200) GSM_ReadDevice(&s,true);

	GSM_Terminate();
}

static GSM_Reply_Function UserReplyFunctions4[] = {

#ifdef DEBUG
	{DCT4_ReplyResetSecurityCode,	"\x08",0x03,0x05,ID_User2	},
	{DCT4_ReplyResetSecurityCode,	"\x08",0x03,0x06,ID_User2	},
#endif

	{DCT4_ReplyGetScreenDump,	"\x0E",0x00,0x00,ID_User3	},
	{DCT4_ReplyGetScreenDump,	"\x0E",0x00,0x00,ID_IncomingFrame},

	{DCT4_ReplyGetADC,		"\x17",0x03,0x10,ID_User3	},
	{DCT4_ReplyGetADC,		"\x17",0x03,0x12,ID_User3	},

	{DCT4_ReplySetPPS,		"\x1b",0x03,0x05,ID_User1	},
	{NOKIA_ReplyGetPhoneString,	"\x1B",0x03,0x08,ID_User6	},

	{DCT4_ReplyVibra,		"\x1C",0x03,0x0D,ID_User3	},
	{DCT4_ReplyVibra,		"\x1C",0x03,0x0F,ID_User3	},

	{NoneReply,			"\x1F",0x03,0x02,ID_User3	},

	{DCT4_ReplyGetSecurityCode,	"\x23",0x03,0x05,ID_User1	},
	{DCT4_ReplyGetT9,		"\x23",0x03,0x05,ID_User3	},
	{DCT4_ReplyGetVoiceRecord,	"\x23",0x03,0x05,ID_User4	},
	{DCT4_ReplyGetVoiceRecord,	"\x23",0x03,0x0D,ID_User4	},
	{DCT4_ReplyGetSecurityCode,	"\x23",0x03,0x0D,ID_User1	}, 

	{DCT4_ReplyTestsStartup,	"\x35",0x02,0x01,ID_User3	},
	{DCT4_ReplyTestsStartup,	"\x35",0x02,0x02,ID_User3	},
	{DCT4_ReplyTestsNames,		"\x35",0x02,0x03,ID_User1	},
	{DCT4_ReplyTestsStatus,		"\x35",0x02,0x04,ID_User2	},

#ifdef GSM_ENABLE_NOKIA6510
	{DCT4_ReplyTuneRadio,		"\x3E",0x03,0x09,ID_User3	},
	{DCT4_ReplyTuneRadio,		"\x3E",0x03,0x15,ID_User3	},
	{DCT4_ReplyTuneRadio,		"\x3E",0x03,0x15,ID_SetFMStation},
	{DCT4_ReplyTuneRadio,		"\x3E",0x03,0x16,ID_User3	},
#endif

	{DCT4_ReplyGetVoiceRecord,	"\x4A",0x03,0x31,ID_User4	},

	{DCT4_ReplyGetSimlock,		"\x53",0x03,0x0D,ID_User6	},
	{DCT4_ReplyGetSimlock,		"\x53",0x03,0x13,ID_User6	},

	{DCT4_ReplyMakeCameraShoot,	"\x61",0x03,0x03,ID_User3	},
	{DCT4_ReplyMakeCameraShoot,	"\x61",0x03,0x07,ID_User3	},
	{DCT4_ReplyMakeCameraShoot,	"\x61",0x03,0x08,ID_User3	},
	{DCT4_ReplyMakeCameraShoot,	"\x61",0x03,0x0A,ID_User3	},
	{DCT4_ReplyMakeCameraShoot,	"\x61",0x03,0xF0,ID_User3	},

	{DCT4_ReplyGetBTInfo,		"\xD7",0x03,0x0A,ID_User6	},

	{NULL,				"\x00",0x00,0x00,ID_None	}
};

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
