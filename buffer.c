#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"

/**************************************************************************\
 *                                                                        *
 * Bounded buffer->  This is the only part you need to modify.  Your       *
 * buffer should have space for up to 10 integers in it at a time.        *
 *                                                                        *
 * Add any data structures you need (globals are fine) and fill in        *
 * implementations for these three procedures:                            *
 *                                                                        *
\**************************************************************************/

typedef struct node {
	int data;
	struct node *next;
} node_t;

typedef struct linked_list {
	node_t *head;
    node_t *tail;
	int curr_size;
	int max_size;
} linked_list_t;

typedef struct array {
    int *items;
    int size;
} array_t;

// TODO: INSTANTIATE GLOBALS
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER; //decided to use mutex_init
pthread_cond_t cond;
pthread_cond_t condTwo;
linked_list_t* buffer;
array_t* helperArray;

/**************************************************************************\
 *                                                                        *
 * void buffer_init(void)                                                 *
 *                                                                        *
 *      buffer_init() is called by main() at the beginning of time to     *
 *      perform any required initialization.  I.e. initialize the buffer, *
 *      any mutex/condition variables, etc.                               *
 *                                                                        *
\**************************************************************************/
void buffer_init()
{
    buffer = (linked_list_t*)malloc(sizeof(linked_list_t));
    buffer->head = 0;
    buffer->tail = 0;
    buffer->curr_size = 0;
    buffer->max_size = BUFFER_MAX_SIZE;
    helperArray = (array_t*)malloc(sizeof(array_t));
    helperArray->items = (int *)malloc(sizeof(int*));
    helperArray->size = BUFFER_MAX_SIZE + 1;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&condTwo, NULL);
   
    // TODO: IMPLEMENT METHOD
    return;
}

/**************************************************************************\
 *                                                                        *
 * void buffer_insert(int number)                                         *
 *                                                                        *
 *      buffer_insert() inserts a number into the next available slot in  *
 *      the buffer->  If no slots are available, the thread should wait    *
 *      (not spin-wait!) for an empty slot to become available.           *
 *                                                                        *
\**************************************************************************/
void buffer_insert(int number)
{
    // TODO: IMPLEMENT METHOD
    pthread_mutex_lock(&mutex);
    printf("Enetering Buffer insert for %i\n", number);
    node_t* helperNode = (node_t*)malloc(sizeof(node_t));
    if(buffer->curr_size == 0) {
        helperNode->data = number;
        helperNode->next = NULL; //no reference
        buffer->head = helperNode;
        buffer->tail = buffer->head; //both link to the same node
        buffer->curr_size = 1;
        helperArray->items[number] = 1;

    } else if(buffer->curr_size == 1) {
        helperNode->data = number;
        helperNode->next = NULL;
        buffer->tail->next = helperNode; //head->old tail -> new tail
        buffer->tail = buffer->tail->next; //head->item ->tail
        buffer->curr_size++;
        helperArray->items[number] = 1;
    } else if (buffer->curr_size == buffer->max_size){
        //printf("waiting for cond to get signal\n");
        pthread_cond_wait(&cond , &mutex); // wait until a slot becomes available                                                                

    } else {
        helperNode->data = number;
        helperNode->next = NULL;
        buffer->tail->next = helperNode;
        buffer->tail = buffer->tail->next;
        buffer->curr_size++;
        helperArray->items[number] = 1;
    }
    //pthread_cond_signal(&condTwo);
    pthread_mutex_unlock(&mutex);
    helperArray->items[number] = 0;
    printf("Exciting buffer Insert\n");
    return;
}

/**************************************************************************\
 *                                                                        *
 * int buffer_extract(void)                                               *
 *                                                                        *
 *      buffer_extract() removes and returns the number in the next       *
 *      available slot.  If no number is available, the thread should     *
 *      wait (not spin-wait!) for a number to become available.  Note     *
 *      that multiple consumers may call buffer_extract() simulaneously.  *
 *                                                                        *
\**************************************************************************/
int buffer_extract(void)
{
    // TODO: IMPLEMENT METHOD (NOTE: THIS METHOD MUST CALL process(int number) before returning the number)
    while(buffer->curr_size == 0 ) {//|| buffer->curr_size == buffer->max_size) {
        printf("Have to wait because buffer size is: (%i)\n",buffer->curr_size );
        pthread_cond_wait(&cond, &mutex);
    }
    //while(helperArray->items[0])
    int temp = 0;
   pthread_mutex_lock(&mutex);
    if(buffer->curr_size == 1) {
        temp = buffer->head->data;
        buffer->head = buffer->tail;
        buffer->curr_size--;
        helperArray->items[temp] = 1;
    } else if (buffer->curr_size == 2){
        temp = buffer->head->data;
        buffer->head = buffer->head->next;
        buffer->curr_size--;
        helperArray->items[temp] = 1;
    } else {
        temp = buffer->head->data;
        buffer->head = buffer->head->next;
        buffer->curr_size--;
        helperArray->items[temp] = 1;
    }
    if( helperArray->items[temp] == 1) {
         helperArray->items[temp] = 0;
         pthread_cond_signal(&condTwo);
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    while(helperArray->items[temp] == 1) {
        //printf("We have found the item in our list\n");
        pthread_cond_wait(&condTwo, &mutex);
    }
    process(temp);
    return temp;
}

void process(int number) {
    sleep(number);
}
