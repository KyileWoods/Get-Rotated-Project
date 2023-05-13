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

// function declarations

void MailboxMessageInteger(Mailbox_Handle MailboxHandle, int speed);
void MailboxMessagePointer(Mailbox_Handle MailboxHandle, void* PointerAsVoid);
void MailboxMessageBool(bool PostBool);





#endif /* OGCREATIONS_MESSAGING_MESSAGING_H_ */


