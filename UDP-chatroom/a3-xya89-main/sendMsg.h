#ifndef _SEND_MSG_H_
#define _SEND_MSG_H_


#include "list.h"

void createSenderThread(char * myHostname, char * myPort, List * l);
void cancelSender();
void shutdownSender();
void signalSender();

#endif