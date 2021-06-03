#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include<semaphore.h>

struct read_write_lock
{
    pthread_mutex_t editing_lock;
    int readers;
    int writer_waits;
    sem_t writers_passing_lock;
    pthread_cond_t writer_wake;
    pthread_cond_t reader_wake;
    
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
