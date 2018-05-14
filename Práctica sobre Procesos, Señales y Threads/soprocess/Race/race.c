#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 2
pthread_t tid[MAX_THREADS]; 

long int ci, cd;

#define N 10

volatile long int i=0; /* eliminar optimización compilador*/

void* Incremento(void *p) {
    ci=0;
    while(i< N) 
         {i= i + 1;
          ci++;
         }
    printf("Hebra 1 (inc) llega a meta\n");
    pthread_exit(NULL);
}

void* Decremento(void *p) {
    cd=0;
    while (i> -N) 
          {i= i-1;
           cd++;
          }
    printf("Hebra 2 (dec) llega a meta\n"); 
    pthread_exit(NULL);
}

int main(void) {
  pthread_create(&tid[0], NULL, Incremento, NULL);
  pthread_create(&tid[1], NULL, Decremento, NULL);
  pthread_join(tid[0],NULL);
  pthread_join(tid[1],NULL);
  printf("Iteraciones: en incremento: %1.3e, en decremento: %1.3e \n",(float)ci,(float)cd); 
} /* El valor de la variable depende de la ejecución*/

