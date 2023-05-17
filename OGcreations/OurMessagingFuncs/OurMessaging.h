/*
 * messaging.h
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */

#ifndef OGCREATIONS_MESSAGING_MESSAGING_H_
#define OGCREATIONS_MESSAGING_MESSAGING_H_

#include <stdbool.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>

typedef struct MailboxMessage {
    Int         id;             /* Writer task id */
    Char        val;            /* Message value */
    Int         valNumber;
} MailboxMessage, *MailboxMessagePointer;

typedef struct QueueMessageObject {
    Queue_Elem elem; /* first field for Queue*/

    int id; /* writer task id */
    char data; /* message value */
    void* MoreData_p;
    bool HallA;
    bool HallB;
    bool HallC;
} QueueMessageObject_t;


// function declarations

void SendMailInteger(Mailbox_Handle MailboxHandle, int speed);
void SendMailPointer(Mailbox_Handle MailboxHandle, void* PointerAsVoid);
void SendMailBool(Mailbox_Handle MailboxHandle, bool PostBool);





#endif /* OGCREATIONS_MESSAGING_MESSAGING_H_ */


