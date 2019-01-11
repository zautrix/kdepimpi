
#ifndef device_functions_h
#define device_functions_h

#include "../gsmstate.h"

#ifdef GSM_ENABLE_BLUETOOTHDEVICE
#ifdef BLUETOOTH_RF_SEARCHING

GSM_Error bluetooth_checkservicename(GSM_StateMachine *s, char *name);

#endif
#endif

#if defined (GSM_ENABLE_BLUETOOTHDEVICE) || defined (GSM_ENABLE_IRDADEVICE)

int socket_read(GSM_StateMachine *s, void *buf, size_t nbytes, int hPhone);

#ifdef WIN32
int socket_write(GSM_StateMachine *s, unsigned char *buf, size_t nbytes, int hPhone);
#else
int socket_write(GSM_StateMachine *s, void *buf, size_t nbytes, int hPhone);
#endif

GSM_Error socket_close(GSM_StateMachine *s, int hPhone);

#endif

GSM_Error 	lock_device	(const char* port, char **lock_device);
bool 		unlock_device	(char **lock_file);

int FindSerialSpeed(char *buffer);

#endif

/* How should editor hadle tabs in this file? Add editor commands here.
 * vim: noexpandtab sw=8 ts=8 sts=8:
 */
