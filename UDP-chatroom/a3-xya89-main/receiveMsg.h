#ifndef _RECEIVE_MSG_H_
#define _RECEIVE_MSG_H_

#include "list.h"

void receiverThreadCreate(char * rPort, List * l);
void shutdownReceiver();
void cancelReceiver();

#endif