#pragma once
#pragma pack(1)     // no extra padding of declared memory structure


// Definitions 

// START ----------------------- MEMORY STRUCTURES -------------------------

typedef struct EGH456_data_t {

    // Semaphores
    sem_t Test1_semaphore   ; // 32 bytes 
    






}



// END ------------------------- MEMORY STRUCTURES -------------------------

// memory handle - as per CAB403
typedef struct shared_mem {

    const char* name;

    int fd;

    shared_struct_t* data;

}

