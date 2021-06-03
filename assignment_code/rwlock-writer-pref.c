#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  pthread_mutex_init(&rw->editing_lock, NULL);// All editings will be done with this lock held
  pthread_mutex_init(&rw->writers_passing_lock, NULL);// One reader or one writer should have this lock
  // pthread_cond_init(&rw->writer_wake, NULL) ;
  pthread_cond_init(&rw->reader_wake, NULL);//for waking sleeping readers.
  rw->readers = 0;
  rw->writer_waits = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&rw->editing_lock);
  while (rw->writer_waits!=0)// There is atleast on waiting writer thread.
  {
    pthread_cond_wait(&rw->reader_wake, &rw->editing_lock);// Please wait(there is a writer thread waiting.) and please unlock the editing lock.
  }
  rw->readers+=1;// Increment the number of readers by 1.
  if (rw->readers==1)// This thread is the first reader thread.
  {
    sem_wait(&rw->writers_passing_lock);// Acquire the lock.
  }
  // Writer_waits are 0;
  pthread_mutex_unlock(&rw->editing_lock);// Unlock the editing lock.

}

void ReaderUnlock(struct read_write_lock * rw)
{
  pthread_mutex_lock(&rw->editing_lock);// Get the editing rights.
  rw->readers-=1;// Decrement number of readers by 1.
  if (rw->readers == 0)
  {
    sem_post(&rw->writers_passing_lock);
  }
  pthread_mutex_unlock(&rw->editing_lock);
  //	Write the code for releasing read-write lock by the reader.
}

void WriterLock(struct read_write_lock * rw)
{
  pthread_mutex_lock(&rw->editing_lock);
  rw->writer_waits+=1;
  pthread_mutex_unlock(&rw->editing_lock);
  pthread_mutex_lock(&rw->writers_passing_lock);
  //	Write the code for aquiring read-write lock by the writer.
}

void WriterUnlock(struct read_write_lock * rw)
{
  pthread_mutex_lock(&rw->editing_lock);
  rw->writer_waits-=1;
  if (rw->writer_waits==0)
  {
    pthread_cond_broadcast(&rw->reader_wake);
  }
  pthread_mutex_unlock(&rw->editing_lock);
  pthread_mutex_unlock(&rw->writers_passing_lock);
  //	Write the code for releasing read-write lock by the writer.
}
