#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <pthread.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <string.h>


#include "list.h"
#include "msgOperations.h"
#include "usrInput.h"
#include "sendMsg.h"
#include "threadCancel.h"



// Handles Keyboard input


#define BUFFLEN 65506


static List * list;
static pthread_t InputThread;



static void*InputLoop(void * ptr)
{
    while(1)
    {
        char *msg;
        char msgBuffer[BUFFLEN];
        int numBytes;
        int iter = 0;


        do
        {
            iter ++;

            memset(&msgBuffer, 0, BUFFLEN);
            numBytes = read(0, msgBuffer, BUFFLEN);

            if (numBytes == -1)
            {
                perror("Input failed");
                exit(-1);
            }


            msg = (char*)malloc(sizeof(char)*(numBytes+1));
            strncpy(msg, msgBuffer, numBytes);
            msg[numBytes] = '\0';





            // Store input messages to a List

            int listFlag = storeMsg(list, msg);
            if (listFlag == -1)
            {
                printf("Store Message failed\n");
            }



            // Exit 
            if (!strcmp(msg, "!exit\n") && (iter == 1))
            {
                signalSender();
                cancelReceiverWriter();
                return NULL;
            }
            
            


            
            
        } while (msgBuffer[numBytes - 1] != '\n' && iter != 100);

        signalSender();
        
    }
    return NULL;
}






// Create Input Thread

void inputThreadCreate(List * l){
    list = l;

    int inputThread = pthread_create(&InputThread, NULL, InputLoop, NULL);

    if (inputThread != 0)
    {
        perror("Input Thread Failed\n");
        exit(-1);
    }
    
}



void cancelInput() {
    pthread_cancel(InputThread);
}

void shutdownInput() {
    pthread_join(InputThread, NULL);
}