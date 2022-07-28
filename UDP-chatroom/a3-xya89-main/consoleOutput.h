#ifndef _CONSOLE_OUTPUT_H_
#define _CONSOLE_OUTPUT_H_

# include "list.h"

void writeSignal();
void outputThreadCreate(List * l);
void cancelOutput();
void shutdownOutput();

#endif