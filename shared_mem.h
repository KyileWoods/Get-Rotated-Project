#pragma once
#pragma pack(1)     // no extra padding of declared memory structure

#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>
// Definitions 

// START ----------------------- MEMORY STRUCTURES -------------------------

// PLEASE DELETE ALL REFERENCES TO THE CODE THIS CAME FROM
int I_AM_INTENTIONALLY_DRAWING_YOUR_ATTENTION;
//THEN PLEASE PLACE THIS INTO AN OGCREATIONS FOLDER

typedef struct MsgObj_s { // This type of struct (with Q_elem) is for queues
    Queue_Elem elem; /* first field for Queue
    */
    int id; /* writer task id */
    char data; /* message value */
} MsgObj;


typedef struct MotorArgStruct{
    Mailbox_Handle mbxHandle;
    int max_mailbox_messages;
}MotorArgStruct_t;






/* END ------------------------- MEMORY STRUCTURES -------------------------
*/
// memory handle - as per CAB403 systems programming
typedef struct shared_mem {

    const char* name;

    int fd;

    //EGH456_data_t* data;

} shared_mem_t;

