/**
Server- general scheme
1. Server should be started before any client
2.It should perform:
    -a)Ask for a shared memory with memory key and memorize rhe 
    returned shared memory ID.
    This is performed by system call shmget()
    b) Attach this shared memory to the server's address space 
    with system call "shmat()".
    c)Initialise the shared memory if necessary.
    d)Do something and wait for clients completion.
    e)Detach the shared memory with the system call shmdt()
    f) Remove the shared memory with system call shmctl()

*/

/**
## Asking for the shared memory segment -shmget()
-------------------------------------------------
shm_id=shmget(key_t k,   //key for the segment.
                int size, //the size of the segement
                int flag  //*purpose is to specify the way the shared
                            memory will be used. For our purpose the
                            1.IPC_CREAT |0666 for a server(i.e., creating and granting read
                             and write access tothe server).
                            2.0666 for any client 
                            (i.e., granting read and write access to the client)
                            **

);

-- if shmget()--> succeeds --> non-negative value,the shared memory
                                is returned.else value is negative.



*/






