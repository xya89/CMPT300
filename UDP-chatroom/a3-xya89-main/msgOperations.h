#ifndef _MSG_OPERATIONS_H
#define _MSG_OPERATIONS_H

# include "list.h"


int storeMsg(List * list, char * message);
char * getMsg(List * list);
int countMsg(List * list);

#endif