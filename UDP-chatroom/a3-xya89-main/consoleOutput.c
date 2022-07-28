#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


# include "list.h"
# include "msgOperations.h"
# include "consoleOutput.h"




# define BUFFLEN 65506

static pthread_t outputThread;
static pthread_mutex_t outputCondMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t outputCond = PTHREAD_COND_INITIALIZER;



static List * list;
static char * msg;



static void * OutputLoop(void * ptr) 
{

    while (1)
    {
        pthread_mutex_lock(&outputCondMutex);

        {
            pthread_cond_wait(&outputCond, &outputCondMutex);
        }

        pthread_mutex_unlock(&outputCondMutex);


        int iter = 0;


        do
        {
            msg = getMsg(list);


            if (msg == NULL)
            {
                printf("Output get msg error\n");
                break;
            }


            int outputFlag = write(1, msg, strlen(msg));
            if (outputFlag == -1)
            {
                perror("Error in write(): Failed to write to screen\n");
                exit(-1);
            }



            // Exit
            if (!strcmp(msg, "!exit\n") && iter == 1)
            {
                free(msg);
                msg = NULL;
                return NULL;
                exit(-1);
            }


            free(msg);
            msg = NULL;

            
            
            





        } while (countMsg(list) != 0);


    }
    

    return NULL;

}




void writeSignal()
{
    pthread_mutex_lock(&outputCondMutex);
    {
        pthread_cond_signal(&outputCond);
    }

    pthread_mutex_unlock(&outputCondMutex);
}




void outputThreadCreate(List * l)
{
    list = l;

    int outputingThread = pthread_create(&outputThread, NULL, OutputLoop, NULL);
    if (outputingThread != 0)
    {
        perror("Output Thread Failed\n");
        exit(-1);
    }
    

}




void cancelOutput()
{
    pthread_cancel(outputThread);
}


void shutdownOutput()
{
    free(msg);
    msg = NULL;

    pthread_join(outputThread, NULL);
}




