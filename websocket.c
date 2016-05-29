#include "websocket.h"

noPollConn* websocket_conn = NULL;

int websocket_init( const char* hoststr, const char* portstr ) {
    int ret_code = 0;
    noPollCtx * ctx = nopoll_ctx_new ();

    if (! ctx) {
        fprintf(stderr, "ERROR: Could not create noPoll websocket context\n");
        ret_code = 1;
        goto cleanup;
    }
    //nopoll_log_enable(ctx, nopoll_true);

    // call to create a connection
    websocket_conn = nopoll_conn_new (ctx, hoststr, portstr, NULL, "/ws", NULL, NULL);
    if (! nopoll_conn_is_ok (websocket_conn)) {
        fprintf(stderr, "ERROR: Could not connect to %s:%s\n", hoststr, portstr);
        ret_code = 1;
        goto cleanup;
    }

    if (! nopoll_conn_wait_until_connection_ready (websocket_conn, 5)) {
        fprintf(stderr, "ERROR: Timed out waiting for connection to become ready\n");
        ret_code = 1;
        goto cleanup;
    }

cleanup:
    if (ctx) nopoll_ctx_unref (ctx);
    return ret_code;
}

int websocket_write( const char* cmdstr ) {
    int bytes_written = 0;
    int cmd_length = 0;

    // do write operation
    fprintf(stderr, "Sending websocket cmd: %s\n", cmdstr);
    cmd_length = strlen( cmdstr );
    bytes_written = nopoll_conn_send_text (websocket_conn, cmdstr, cmd_length);

    // complete pending write by flushing and limitting operation for 2 seconds
    bytes_written = nopoll_conn_flush_writes (websocket_conn, 2000000, bytes_written);

    return bytes_written;
}

int websocket_send_cmds( Queue* queue ) {
    int num_sent = 0;
    unsigned int bytes_written = 0;
    char cmd[MAX_CMD_LENGTH];

    if (queue->size == 0)
        return 0;
    else { 
        while (queue->size > 0) {
            queue->pop( queue, cmd );
            bytes_written = websocket_write( cmd );
            if (bytes_written != strlen(cmd)) {
                fprintf(stderr, "ERROR: Only sent %d bytes of command: %s\n", bytes_written, cmd);
            } else {
                fprintf(stderr, "     + Successfully sent cmd: %s\n", cmd);
                num_sent++;
            }
        }
    }
    fprintf(stderr, "Sent %d commands\n", num_sent);
    return num_sent;
}


int http_send_cmds( Queue* queue ) {
    int num_sent = 0;
    char cmd[MAX_CMD_LENGTH];

    if (queue->size == 0)
        return 0;
    else { 
        while (queue->size > 0) {
            queue->pop( queue, cmd );

            CURL *curl;
            CURLcode res;

            curl = curl_easy_init();
            if(curl) {

                // Set the URL for the http GET
                curl_easy_setopt(curl, CURLOPT_URL, cmd);

                /* Perform the request, res will get the return code */
                res = curl_easy_perform(curl);
                /* Check for errors */
                if(res != CURLE_OK) {
                  fprintf(stderr, "curl_easy_perform() failed: %s %s\n",
                          curl_easy_strerror(res), cmd);
                } else {
                    fprintf(stderr, "     + Successfully sent cmd: %s\n", cmd);
                    num_sent++;
                }

                /* always cleanup */
                curl_easy_cleanup(curl);
            } else {
                fprintf(stderr, "Failed to establish curl instance\n");
            }
        }
    }

    fprintf(stderr, "Sent %d commands\n", num_sent);
    return num_sent;

}

/**
 * Push an item into queue, if this is the first item,
 * both queue->head and queue->tail will point to it,
 * otherwise the oldtail->next and tail will point to it.
 */
void push (Queue* queue, const char cmd[MAX_CMD_LENGTH]) {
    // Create a new cmd
    Node* n = (Node*) malloc (sizeof(Node));
    strncpy( n->cmd, cmd, MAX_CMD_LENGTH );
    n->next = NULL;

    if (queue->head == NULL) { // no head
        queue->head = n;
    } else{
        queue->tail->next = n;
    }
    queue->tail = n;
    queue->size++;
}
/**
 * Return and remove the first cmd.
 */
int pop (Queue* queue, char cmd[MAX_CMD_LENGTH]) {
    if (queue->head == NULL) return -1;
    // get the first cmd
    Node* head = queue->head;
    strncpy( cmd, head->cmd, MAX_CMD_LENGTH );
    // move head pointer to next cmd, decrease size
    queue->head = head->next;
    queue->size--;
    // free the memory of original head
    free(head);
    return 0;
}
/**
 * Empty the queue.
 */
int clear (Queue* queue) {
    char cmd[MAX_CMD_LENGTH];

    if (queue->head == NULL) return -1;
    while (queue->size > 0) {
        queue->pop( queue, cmd );
    }
    return 0;
}
/**
 * Return but not remove the first item.
 */
int peek (Queue* queue, char cmd[MAX_CMD_LENGTH]) {
    if (queue->head == NULL) return -1;
    Node* head = queue->head;
    strncpy( cmd, head->cmd, MAX_CMD_LENGTH );
    return 0;
}
/**
 * Show all items in queue.
 */
void display (Queue* queue) {
    printf("\nDisplay: ");
    // no item
    if (queue->size == 0)
        printf("No item in queue.\n");
    else { // has item(s)
        Node* head = queue->head;
        int i, size = queue->size;
        printf("%d item(s):\n", queue->size);
        for (i = 0; i < size; i++) {
            //if (i > 0)
                //printf(", ");
            printf("%s", head->cmd);
            head = head->next;
        }
    }
    printf("\n\n");
}
/**
 * Create and initiate a Queue
 */
Queue createQueue () {
    Queue queue;
    queue.size = 0;
    queue.head = NULL;
    queue.tail = NULL;
    queue.push = &push;
    queue.pop = &pop;
    queue.clear = &clear;
    queue.peek = &peek;
    queue.display = &display;
    return queue;
}

