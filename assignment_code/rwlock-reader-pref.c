#include "rwlock.h"
void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  pthread_mutex_init(&rw->editing_lock, NULL);
  sem_init(&rw->writers_passing_lock,0 ,1);
  pthread_cond_init(&rw->writer_wake, NULL) ;
  pthread_cond_init(&rw->reader_wake, NULL);
  rw->readers = 0;
  rw->writer_waits = 0;

}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&rw->editing_lock);
  rw->readers +=1;
  // Ensure no writer is there; 
  if (rw->readers == 1)
  {
    sem_wait(&rw->writers_passing_lock);// Dont allow any writer now
  }
  pthread_mutex_unlock(&rw->editing_lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->editing_lock);
  rw->readers -=1;
  if (rw->readers == 0)
  {
    sem_post(&rw->writers_passing_lock);// Allow if there is a writing thread
  }
  pthread_mutex_unlock(&rw->editing_lock);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  sem_wait(&rw->writers_passing_lock);
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  sem_post(&rw->writers_passing_lock);
}
