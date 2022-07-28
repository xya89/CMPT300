
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>


#include "list.h"
#include "msgOperations.h"



static pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

int storeMsg(List * list, char * message)
{
    int prepVal;
    pthread_mutex_lock(&queueMutex);
    {
        prepVal = List_append(list, message);
    }
    pthread_mutex_unlock(&queueMutex);
    return prepVal;
}




char * getMsg(List * list)
{
    char * message;
    pthread_mutex_lock(&queueMutex);
    {
        message = List_trim(list);
    }
    pthread_mutex_unlock(&queueMutex);


    return message;

}



int countMsg(List * list) 
{
    int count;
    pthread_mutex_lock(&queueMutex);
    {
        count = List_count(list);
    }
    pthread_mutex_unlock(&queueMutex);


    return count;
}