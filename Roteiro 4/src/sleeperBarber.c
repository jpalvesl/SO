#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* constants */
#define MAX_DELAY 4

#define CHAIRS 10
#define COSTUMER_POSSIBLE 30 // number of costumers threads

int isSleeping = 1; //true/false
int freeChairs = CHAIRS;

/* about threads sync */
pthread_t barberThread;
pthread_t costumerThreads[COSTUMER_POSSIBLE]; // just to might create threads that will be deleted because the max threads waiting is 10

sem_t costumers;
sem_t barber;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* functions */
void delay() {
  sleep(rand() % MAX_DELAY);
}

void* costumer(void* n) {
  int id = *(int*) n; // just to identify, its not relation with the arrival order 

  pthread_mutex_lock(&mutex);
  if (freeChairs == 10 && isSleeping) { // if has freeChairs and the barber is sleeping wake him up
    printf("Barbeiro acordado pelo cliente %d\n", id);
    isSleeping = 0;
    sem_post(&barber);
  }
  pthread_mutex_unlock(&mutex);

  delay();

  pthread_mutex_lock(&mutex);
  if (freeChairs) { // if have freeChairs sit down
    printf("O cliente %d está esperando\n", id);
    freeChairs--;
    sem_wait(&costumers); // waiting for the barbers post
  }
  else { // leave the barber shop if dont have any chair to sit
    printf("Não tem cadeiras, o cliente %d saiu da barbearia\n", id);
  }
  pthread_mutex_unlock(&mutex);
  
  pthread_exit(NULL);
}

void* barberAction(void* n) {
  while (1) {
    delay();

    pthread_mutex_lock(&mutex);
    // see if has anyone in the chairs
    if (freeChairs == 10 && !isSleeping) { // he has to sleep
      printf("Barbeiro está dormindo\n");
      isSleeping = 1;
      sem_wait(&barber);
    }
    else if (freeChairs != 10) { // if have any costumer he works
      freeChairs++;
      sem_post(&costumers); // shaving the costumer
      printf("Barbeando cliente\n");
    }
    printf("Agora temos %d cadeiras disponiveis\n", freeChairs);
    pthread_mutex_unlock(&mutex);
  }
  
  pthread_exit(NULL);
}

int main() {
  srand(time(NULL)); // get a random seed

  int costumerIds[COSTUMER_POSSIBLE];
  pthread_attr_t *attr = NULL;

  sem_init(&barber, 0, 0); // starts sleeping
  sem_init(&costumers, 0, CHAIRS);

  printf("--- Thread do barbeiro foi inicializada ---\n\n");
  pthread_create(&barberThread, attr, barberAction, NULL);

  for (int i = 0; i < 10; i++) { // fill all the chairs present in the barbershop
    costumerIds[i] = i;
    int indexArray = i;
    delay(); // to make possible costumers arrive when the chairs are full or not
    pthread_create(&costumerThreads[indexArray], attr, costumer, (void*) &costumerIds[i]);
  }


  for (int i = 10; i < COSTUMER_POSSIBLE; i++) { 
    costumerIds[i] = i;
    int indexArray = i;
    delay(); // to make possible costumers arrive when the chairs are full or not
    pthread_create(&costumerThreads[indexArray], attr, costumer, (void*) &costumerIds[i]);
  }

  for (int i = 0; i < COSTUMER_POSSIBLE; i++) {
    int indexArray = i;
    pthread_join(costumerThreads[indexArray], NULL);
  }

  pthread_join(barberThread, NULL);
}
