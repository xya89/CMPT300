// Code inspired by https://habibiefaried.com/cnet-01-basic-c-socket-programming-a818b0da5ae0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>


#include "list.h"
#include "msgOperations.h"
#include "sendMsg.h" 






static int sockfd;
static struct addrinfo * serverinfo;
static char * Hostname;
static char * Port;
static List * list;
static pthread_t senderThread;
static char * msg;



static pthread_mutex_t sendCondMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sendCond = PTHREAD_COND_INITIALIZER;


static void * SenderLoop(void * ptr)
{

    struct addrinfo hints, *p;
    int getaddrinfoVal;
    int numbytes;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;




    getaddrinfoVal = getaddrinfo(Hostname, Port, &hints, &serverinfo);
    if (getaddrinfoVal != 0)
    {
        printf("Sender: get address info error\n");
        exit(-1);

    }




    // Initialize a socket and bind to the port

    for (p = serverinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd == -1)
        {
            perror("Sender socket error\n");
            continue;
        }
        break;
        

    }

    if (p == NULL)
    {
        printf("Failed to create socket\n");
        exit(-1);
    }


    while (1)
    {
        pthread_mutex_lock(&sendCondMutex);

        {
            pthread_cond_wait(&sendCond, &sendCondMutex);
        }

        pthread_mutex_unlock(&sendCondMutex);


        int iter = 0;
        do
        {
            

            iter++;

            msg = getMsg(list);

            if (msg == NULL)
            {
                printf("Failed to get Message from list\n");
                break;
            }

            numbytes = sendto(sockfd, msg, strlen(msg), 0 , p->ai_addr, p->ai_addrlen);




            // exit
            if (!strcmp(msg, "!exit\n") && (iter == 1))
            {
                free(msg);
                msg = NULL;
                return NULL;
                exit(-1);
            }


            free(msg);
            msg = NULL;


            if (numbytes == -1)
            {
                perror("Sender sendto error\n");
                exit(-1);
            }
            
            
            


        } while (countMsg(list) != 0);
        



    }
    
    


    return NULL;

    


}





void signalSender() {
    pthread_mutex_lock(&sendCondMutex);

    {
        pthread_cond_signal(&sendCond);

    }

    pthread_mutex_unlock(&sendCondMutex);
}



void cancelSender() {
    pthread_cancel(senderThread);
}




void createSenderThread(char * myHostname, char * myPort, List * myList)
{
    Hostname = myHostname;
    Port = myPort;
    list = myList;


    int senderThreadVal = pthread_create(&senderThread, NULL, SenderLoop, NULL);
    if (senderThreadVal != 0)
    {
        perror("Sender thread creation Failed\n");
        exit(-1);
    }
    

}




void shutdownSender()
{

    free(msg);
    msg = NULL;


    freeaddrinfo(serverinfo);


    close(sockfd);

    pthread_join(senderThread, NULL);


}