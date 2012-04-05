#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "libs/extra.h"
#include "libs/LL.h"
#include "ciclista.h"

void MALLOC_DIE() { exit(404); }

#define NUM_THREADS     5
 
void *TaskCode(void *argument)
{
   int tid;
   int count = 0, a, b, ta, tb;

   tid = *((int *) argument);
   printf("Hello World! It's me, thread %d!\n", tid);
 
   srand(time(NULL) * (tid+1));

   /* optionally: insert more useful stuff here */
   ta = rand();
   for(a = 0; a < ta; a++) {
       tb = rand();
       for(b = 0; b < tb; b++) {
            count = (count + a + b) % 10000;
       }
   }
   printf("Thread %d result: %X\n", tid, count);
 
   return NULL;
}
 
/* int main(void)
{
   pthread_t threads[NUM_THREADS];
   int thread_args[NUM_THREADS];
   int rc, i;

   srand(time(NULL));
 
   for (i=0; i<NUM_THREADS; ++i) {
      thread_args[i] = i;
      printf("In main: creating thread %d\n", i);
      rc = pthread_create(&threads[i], NULL, TaskCode, (void *) &thread_args[i]);
      assert(0 == rc);
   }
 
    for (i=0; i<NUM_THREADS; ++i) {
      rc = pthread_join(threads[i], NULL);
      assert(0 == rc);
   }

   puts("DONES!");
 
   exit(EXIT_SUCCESS);
}*/


typedef struct Trecho {
    char tipo;
    int distancia;
} trecho;

/* O grande main incomming. Se vira ae champz. */
int main(int argc, char **argv) {
    int i;

    FILE* in;
    int num_ciclistas;
    int largura_estrada;
    char modo_simula;
    int tamanho_estrada;
    list trechos;

    if(argc != 2) { 
        fprintf(stderr, "Uso: %s arquivo\n", argv[0]); 
        return 1; 
    }

    in = fopen(argv[1], "r");
    if(in == NULL) {
        fprintf(stderr, "Erro: Arquivo '%s' não pode ser lido.\n", argv[1]);
        return 2;
    }

    fscanf(in, "%d", &num_ciclistas);   /* m */
    printf("num_ciclistas (m) = %d\n", num_ciclistas); 
    fscanf(in, "%d", &largura_estrada); /* n */
    printf("largura_estrada (n) = %d\n", largura_estrada);

    do modo_simula = fgetc(in); while(modo_simula == '\n');
    if(modo_simula != 'U' && modo_simula != 'A') {
        fprintf(stderr, "Erro: Modo '%c' diferente de A e U.\n", modo_simula);
        return 3;
    }
    modo_simula = modo_simula == 'A';  /* 0 se uniforme, 1 se diferente */
    printf("modo_simula = %d\n", modo_simula);

    fscanf(in, "%d", &tamanho_estrada); /* d */
    printf("tamanho_estrada (d) = %d\n", tamanho_estrada);

    trechos = LISTinit();
    while(!feof(in)) {
        trecho* t;
        AUTOMALLOC(t);
        do t->tipo = fgetc(in); while(t->tipo == '\n');
        fscanf(in, "%d", &t->distancia);
        printf("Trecho: distancia = %d, tipo = %c\n", t->distancia, t->tipo);
        trechos = LISTaddEnd(trechos, t);
    }
    return 0;
}