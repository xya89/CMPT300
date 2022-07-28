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
#include "receiveMsg.h"
#include "consoleOutput.h"
#include "threadCancel.h"




# define BUFFLEN 65506

static int sockfd;
static struct addrinfo * serverinfo;
static char * myPort;
static List * list;
static pthread_t receiverThread;






static void * ReceiverLoop (void * ptr) {

    int sockfd;
    struct addrinfo hints, *serverinfo, *p;
    int getaddrinfoVal;
    int bindVal;
    int numbytes;
    char buf[BUFFLEN];
    char * msg;
    struct sockaddr_in rev_addr;
    socklen_t addr_len;




    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;


    getaddrinfoVal = getaddrinfo(NULL, myPort, &hints, &serverinfo);

    if (getaddrinfoVal != 0)
    {
        printf("Receiver get addr info failed \n");
        exit(-1);
    }



    for(p = serverinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);


        if (sockfd == -1)
        {
            perror("Receiver socket error\n");
            continue;
        }



        bindVal = bind(sockfd, p->ai_addr, p->ai_addrlen);


        if (bindVal == -1)
        {
            close(sockfd);
            perror("Receiver socket bind error\n");
            continue;
        }

        break;
    

    }
    

    if (p == NULL)
    {
        printf("Receiver failed to bind socket\n");
        exit(-1);
    }

    freeaddrinfo(serverinfo);
    

    while(1)
    {

        int iter = 0;
        do
        {
            iter ++;

            memset(&buf, 0, BUFFLEN);

            addr_len = sizeof(rev_addr);
            numbytes = recvfrom(sockfd, buf, BUFFLEN, 0, (struct sockaddr *) &rev_addr, &addr_len);



            if (numbytes == -1)
            {
                perror("Receiver recvfrom error\n");
                exit(-1);
            }



            msg = (char*) malloc(sizeof(char)*(numbytes+1));
            strncpy(msg, buf, numbytes);
            msg[numbytes] = '\0';




            int storemsgFlag = storeMsg(list, msg);
            if (storemsgFlag == -1)
            {
                printf("Receiver store msg failed\n");
            }



            // exit
            if(!strcmp(msg, "!exit\n") && iter == 1)
            {
                writeSignal();
                cancelReaderSender();
                return NULL;
            }
            
            




        } while (buf[numbytes-1] != '\n' && iter != 100);
        
        
        writeSignal();


    }


    return NULL;



}



void receiverThreadCreate(char * rPort, List * l) 
{
    myPort = rPort;
    list = l;


    int receiverThreadVal = pthread_create(&receiverThread, NULL, ReceiverLoop, NULL);
    if (receiverThreadVal != 0)
    {
        perror("Sender Thread creation failed\n");
        exit(-1);
    }
    


}





void cancelReceiver()
{
    pthread_cancel(receiverThread);
}


void shutdownReceiver()
{
    freeaddrinfo(serverinfo);
    close(sockfd);

    pthread_join(receiverThread, NULL);
}