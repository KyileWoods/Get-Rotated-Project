#pragma once
#pragma pack(1)     // no extra padding of declared memory structure


// Definitions 

// START ----------------------- MEMORY STRUCTURES -------------------------

typedef struct MsgObj {
    Queue_Elem elem; /* first field for Queue
    */
    int id; /* writer task id */
    char data; /* message value */
} MsgObj;

typedef struct EGH456_data_t {

    // Semaphores
    sem_t Test1_semaphore   ; // 32 bytes 






}



/* END ------------------------- MEMORY STRUCTURES -------------------------
*/
// memory handle - as per CAB403 systems programming
typedef struct shared_mem {

    const char* name;

    int fd;

    EGH456_data_t* data;

} shared_mem_t;

