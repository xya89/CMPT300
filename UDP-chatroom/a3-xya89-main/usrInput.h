#ifndef _USR_INPUT_H_
#define _USR_INPUT_H_

#include "list.h"

void inputThreadCreate(List * l);
void shutdownInput();
void cancelInput();

#endif