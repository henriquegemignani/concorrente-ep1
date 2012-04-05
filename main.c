#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "libs/extra.h"
#include "libs/LL.h"
#include "ciclista.h"

/* Nosso extra.h precisa desse codigo maroto. */
void MALLOC_DIE() { exit(404); }

typedef struct Trecho {
    char tipo;
    int distancia;
} trecho;

void dumpTrecho(void* tv) {
    trecho* t = (trecho*) tv;
    printf("Trecho: distancia = %d, tipo = %c\n", t->distancia, t->tipo);
}

int largura_estrada;
int tamanho_estrada;
list trechos;
ciclista* ciclistas;
pthread_t* ciclistas_threads;

/* Temos d indíces em estrada, e cada índice pode possuir até n ciclistas. */
ciclista* estrada;

void* CiclistaThread(void* arg) {
    /* This is our thread. There are many like it, this is one is ours. */
    ciclista c = (ciclista) arg;

    printf("%s %.2d está dormindo.\n", c->nome, c->id);

    /* Nosso ciclista é baiano e ta com preguiça de correr agora. */
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

/* O grande main incomming. Se vira ae champz. */
int main(int argc, char **argv) {
    int i;
    FILE* in;
    int num_ciclistas;
    char modo_simula;

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

    AUTOMALLOCV(ciclistas, num_ciclistas);
    AUTOMALLOCV(ciclistas_threads, num_ciclistas);

    fscanf(in, "%d", &largura_estrada); /* n */
    printf("largura_estrada (n) = %d\n", largura_estrada);
    assert(largura_estrada >= 2);

    do modo_simula = fgetc(in); while(modo_simula == '\n');
    if(modo_simula != 'U' && modo_simula != 'A') {
        fprintf(stderr, "Erro: Modo '%c' diferente de A e U.\n", modo_simula);
        return 3;
    }
    modo_simula = modo_simula == 'A';  /* 0 se uniforme, 1 se diferente */
    printf("modo_simula = %d\n", modo_simula);

    fscanf(in, "%d", &tamanho_estrada); /* d */
    printf("tamanho_estrada (d) = %d\n", tamanho_estrada);

    AUTOMALLOCV(estrada, tamanho_estrada);

    trechos = LISTinit();
    while(!feof(in)) {
        trecho* t;
        AUTOMALLOC(t);
        do t->tipo = fgetc(in); while(t->tipo == '\n');
        fscanf(in, "%d", &t->distancia);
        trechos = LISTaddEnd(trechos, t);
    }
    LISTdump(trechos, dumpTrecho);

    for(i = 0; i < num_ciclistas; ++i) {
        ciclista c;
        AUTOMALLOC(c);
        c->id = i;
        c->metros = 0;
        AUTOMALLOCV(c->nome, strlen("Ciclista") + 1);
        strcpy(c->nome, "Ciclista");
        c->vel_descida = c->vel_plano = c->vel_subida = 50.0;
        ciclistas[i] = c;
    }
    for(i = 0; i < num_ciclistas; ++i)
        printf("%.2d: %s [%.2lf; %.2lf; %.2lf]\n", i, ciclistas[i]->nome, ciclistas[i]->vel_descida, ciclistas[i]->vel_plano, ciclistas[i]->vel_subida);

    for(i = 0; i < num_ciclistas; ++i) {
        int rc = pthread_create(&ciclistas_threads[i], NULL, CiclistaThread, (void *) ciclistas[i]);
        assert(0 == rc);
    }

    for(i = 0; i < num_ciclistas; ++i) {
        int rc = pthread_join(ciclistas_threads[i], NULL);
        assert(0 == rc);
    }

    free(ciclistas);
    free(ciclistas_threads);
    free(estrada);
    return 0;
}