#include "usrInput.h"
#include "consoleOutput.h"
#include "sendMsg.h"
#include "receiveMsg.h"
#include "threadCancel.h"


void cancelReceiverWriter()
{
    cancelReceiver();
    cancelOutput();
}


void cancelReaderSender()
{
    cancelInput();
    cancelSender();
}