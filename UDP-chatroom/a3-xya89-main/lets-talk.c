#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "usrInput.h"
#include "consoleOutput.h"
#include "receiveMsg.h"
#include "sendMsg.h"


int main(int argc, char * argv[])
{
    if (argc != 4)
    {
        printf("Usage: \n");
        printf("\t./lets-talk <local port> <remote host> <remote port>\n");
        printf("Examples: \n");
        printf("\t./lets-talk 300 192.168.0.513 3001\n\t./lets-talk 3000 some-computer-name 3001\n");
        return -1;
    }



    char * myPort = argv[1];
    char * remoteHostname = argv[2];
    char * remotePort = argv[3];





    List * list = List_create();

    inputThreadCreate(list);
    createSenderThread(remoteHostname, remotePort, list);
    receiverThreadCreate(myPort, list);
    outputThreadCreate(list);
    


    shutdownInput();
    shutdownSender();
    shutdownReceiver();
    shutdownOutput();



    List_free(list, free);


    return 0;


}