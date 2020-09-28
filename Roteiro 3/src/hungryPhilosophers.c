#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
pthread_t philosophersThreads[N];
pthread_mutex_t mutexGeneral = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPhilosophers[N];

int LEFT(int i) {
  return (i+N-1)%N;
}

int RIGHT(int i) {
  return (i+1)%N;
}

int test(int i) {
  if (state[i] == HUNGRY && 
      state[LEFT(i)] != EATING &&
      state[RIGHT(i)] != EATING) {
        state[i] = EATING;
        pthread_mutex_unlock(&mutexPhilosophers[i]);

        printf("O filósofo %d pode comer\n", i);
        return 1;
  }
  return 0;
}

void think(int i) {
  // printf("%d - Pensando\n", i);
  sleep(rand() % 4);  // thinking for 5 sec
}

void eat(int i) {
  // printf("%d - Comendo\n", i);
  sleep(rand() % 4);  // eating for 5 sec
}

void take_forks(int i) {
  pthread_mutex_lock(&mutexGeneral);
  state[i] = HUNGRY;
  test(i);
  pthread_mutex_unlock(&mutexGeneral);
  pthread_mutex_lock(&mutexPhilosophers[i]);
}

void put_forks(int i) {
  int leftPhilosopher, rightPhilosopher;
  pthread_mutex_lock(&mutexGeneral);
  state[i] = THINKING;
  rightPhilosopher = test(RIGHT(i));
  leftPhilosopher = test(LEFT(i));

  if (rightPhilosopher) {
    printf("O filosofo %d, deu seu garfo para o filósofo %d comer\n", i, RIGHT(i));
  }

  if (leftPhilosopher) {
    printf("O filosofo %d, deu seu garfo para o filósofo %d comer\n", i, LEFT(i));
  }

  pthread_mutex_unlock(&mutexGeneral);
}

void* philosopher(void* i) {
  int philosopherNumber = atoi( (char*) i ); // transform array of char in int to identify a philosopher
  
  printf("Inicializando filosofo número %d\n", philosopherNumber);

  while (1) {
    think(philosopherNumber);
    take_forks(philosopherNumber);
    eat(philosopherNumber);
    put_forks(philosopherNumber);
  }

  pthread_exit((void *)0);
}

int main() {
  char numberInString[4][32]; // array to get the number of each philosopher
  srand(0); // to get the same result in the random numbers to test
  
  printf("--- Inicializando semaforos dos filosofos ---\n");
  for (int i = 0; i < N; i++) {
    pthread_mutex_init(&mutexPhilosophers[i], NULL);
  }

  printf("--- Inicializando threads dos filosofos ---\n");
  for (int i = 0; i < N; i++) {
    sprintf(numberInString[i], "%d", i);

    pthread_create(&philosophersThreads[i], NULL, philosopher, (void*) numberInString[i]);
  }

  for (int i = 0; i < N; i++) {
    pthread_join(philosophersThreads[i], NULL);
  }

  pthread_exit(NULL);
}