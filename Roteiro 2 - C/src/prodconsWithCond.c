/*
 * Producer/Consumer demo using POSIX threads without synchronization
 * Linux version
 * MJB Apr'05
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/* buffer using a shared integer variable */
typedef struct {
  int writeable; /*true/false full/empty buffer */
  int sharedData;
} buffer;

/* global variables */
buffer theBuffer;
pthread_mutex_t mutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int store(int item, buffer *pb) {
  pthread_mutex_lock(&mutex);
  if (!pb->writeable) {
    pthread_cond_wait(&cond, &mutex);
  }

  pb->sharedData = item;      /*put data in buffer... */
  pb->writeable = !pb->writeable;
  
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  return 0;
}

int retrieve(buffer *pb) {
  int data;
  pthread_mutex_lock(&mutex);
  if (pb->writeable) {
    pthread_cond_wait(&cond, &mutex);
  }

  data = pb->sharedData;       /*get data from buffer...*/
  pb->writeable = !pb->writeable;

  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  return data;
}

void delay(int secs) { /*utility function*/
  time_t beg = time(NULL), end = beg + secs;
  do {
    ;
  } while (time(NULL) < end);
}

/* core routine for the producer thread */
void *producer(void *n) {
  printf("Starting Producer thread...\n");

  int j=1, tot=0;
  while (j <= 10) {
    store(j, &theBuffer);
    printf("Stored: %d\n", j);
     j++;
    delay(rand() % 6);  /* up to 5 sec */
  }
  pthread_exit((void *)0);
}

/* core routine for the consumer thread */
void *consumer(void *n) {
  printf("Starting Consumer threads...\n");

  int j=0, tot=0;
  while (j < 10) {
    j = retrieve(&theBuffer);
    tot += j;
    printf("Retrieved: %d\n", j);
    delay(rand() % 6);  /* up to 5 sec */
  }
  printf("--- Retrieved total = %d ---\n", tot);
  pthread_exit((void *)0);
}

int main() {
  pthread_attr_t *attr = NULL;
  pthread_t prodThread, consThread;
  pthread_mutex_init(&mutex, NULL);

  printf("Starting Mutex...\n");

  theBuffer.writeable = 1;
  srand(time(NULL)); /* initialise the rng */

  pthread_create(&prodThread, attr, producer, NULL);
  pthread_create(&consThread, attr, consumer, NULL);
  
  printf("Join Producer thread...\n");
  pthread_join(prodThread, NULL);

  printf("Join Consumer thread...\n");
  pthread_join(consThread, NULL);

  printf("Exit the program...\n");
  pthread_exit(NULL);
}