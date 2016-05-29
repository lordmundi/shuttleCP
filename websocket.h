#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <nopoll.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CMD_LENGTH 80

/* 
 * Note... my queue structure was borrowed from here:
 * https://github.com/benbai123/C_Cplusplus_Practice/blob/master/C_DataStructure/Queue.c
 */

typedef struct Node {
    char cmd[MAX_CMD_LENGTH];
    struct Node* next;
} Node;
/**
 * The Queue struct, contains the pointers that
 * point to first cmd and last cmd, the size of the Queue,
 * and the function pointers.
 */
typedef struct Queue {
    Node* head;
    Node* tail;
    void (*push) (struct Queue*, const char*); // add cmd to tail
    int (*pop) (struct Queue*, char*);         // get cmd from head and remove it from queue
    int (*clear) (struct Queue*);              // empty the queue
    int (*peek) (struct Queue*, char*);        // get cmd from head but keep it in queue
    void (*display) (struct Queue*);   // display all element in queue
    int size;                          // size of this queue
} Queue;

int websocket_init( const char* hoststr, const char* portstr );
int websocket_write( const char* cmdstr );
int websocket_send_cmds( Queue* queue );
int http_send_cmds( Queue* queue );
void push (Queue* queue, const char cmd[MAX_CMD_LENGTH]);
int pop (Queue* queue, char cmd[MAX_CMD_LENGTH]);
int peek (Queue* queue, char cmd[MAX_CMD_LENGTH]);
void display (Queue* queue);
Queue createQueue ();


#endif   /* WEBSOCKET_H - do not put anything below this line! */
