#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "libs/extra.h"
#include "libs/LL.h"
#include "ciclista.h"

#define new AUTOMALLOC

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

struct KM {
    pthread_mutex_t mutex;
    list ciclistas;
};

#define CICLO_TIME 5

int largura_estrada;
int tamanho_estrada;
list trechos;

pthread_mutex_t terminar_mutex;
list ciclistas_terminaram;

/* Temos d indíces em estrada, e cada índice pode possuir até n ciclistas. */
struct KM* estrada;

double kmh2ms(double kmh) {
    return kmh / 3.6;
}

void* CiclistaThread(void* arg) {
    /* This is our thread. There are many like it, this is one is ours. */
    ciclista c = (ciclista) arg;

    /* Ciclista ja terminou a corrida. Nao corre mais poar. */
    if(c->km >= tamanho_estrada) return NULL;

    c->metros += kmh2ms(c->vel_plano) * CICLO_TIME;
    if(c->metros >= 1000) {

        if(c->km + 1 >= tamanho_estrada) {
            /* YEAH TERMINEI! */
            pthread_mutex_lock(&terminar_mutex);
            LISTaddEnd(ciclistas_terminaram, c);
            pthread_mutex_unlock(&terminar_mutex);

            pthread_mutex_lock(  &estrada[c->km].mutex);
            LISTremove(estrada[c->km].ciclistas, c);
            pthread_mutex_unlock(&estrada[c->km].mutex);

            c->km++;
            c->metros -= 1000;

            return NULL;
        }

        pthread_mutex_lock(&estrada[c->km + 1].mutex);
        if(LISTsize(estrada[c->km + 1].ciclistas) < largura_estrada) {

            /* Opa, consegui entrar no proximo km, to rox. */
            LISTaddEnd(estrada[c->km + 1].ciclistas, c);
            pthread_mutex_unlock(&estrada[c->km + 1].mutex);

            if(c->km >= 0) {
                /* Se eu não estou no limbo de começo de corrida, vou me tirar da lista do km anterior. 
                    Preciso travar o mutex senão da merda. */
                pthread_mutex_lock(  &estrada[c->km].mutex);
                LISTremove(estrada[c->km].ciclistas, c);
                pthread_mutex_unlock(&estrada[c->km].mutex);
            }

            /* Tudo ok, atualiza variaveis. */
            c->km++;
            c->metros -= 1000;

        } else {

            /* Fico parado esperando abrir espaço. Tomando cuidado pra não dormir.
                Tb libero o lock. */
            pthread_mutex_unlock(&estrada[c->km + 1].mutex);
            c->metros = 1000;
        }
    }
    return NULL;
}

ciclista NewCiclista(int id) {
    ciclista c;
    AUTOMALLOC(c);
    c->id = id;
    c->km = -1;
    c->metros = 1000;
    AUTOMALLOCV(c->nome, strlen("Ciclista") + 1);
    strcpy(c->nome, "Ciclista");
    c->vel_descida = c->vel_plano = c->vel_subida = 50.0;
    return c;
}

void dumpCiclista(void* val) {
    ciclista c = (ciclista) val;
    printf("[%s %.2d] ", c->nome, c->id);
}
 
/* O grande main incomming. Se vira ae champz. */
int main(int argc, char **argv) {
    int i;
    int ciclo = 0;
    FILE* in;
    int num_ciclistas;
    char modo_simula;
    ciclista* ciclistas;
    pthread_t* ciclistas_threads;

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
    for(i = 0; i < tamanho_estrada; ++i) {
        estrada[i].ciclistas = LISTinit();
        pthread_mutex_init(&estrada[i].mutex, NULL);
    }

    ciclistas_terminaram = LISTinit();
    pthread_mutex_init(&terminar_mutex, NULL);

    trechos = LISTinit();
    while(!feof(in)) {
        trecho* t;
        AUTOMALLOC(t);
        do t->tipo = fgetc(in); while(t->tipo == '\n');
        fscanf(in, "%d", &t->distancia);
        LISTaddEnd(trechos, t);
    }
    LISTdump(trechos, dumpTrecho);
    for(i = 0; i < num_ciclistas; ++i)
        ciclistas[i] = NewCiclista(i);

    for(i = 0; i < num_ciclistas; ++i)
        printf("%.2d: %s [%.2lf; %.2lf; %.2lf]\n", i, ciclistas[i]->nome, ciclistas[i]->vel_descida, ciclistas[i]->vel_plano, ciclistas[i]->vel_subida);

    while(LISTsize(ciclistas_terminaram) < num_ciclistas) { /* Execute ciclos até o fim! */
        /* Isso é um ciclo. */
        if(ciclo % (60 / CICLO_TIME) == 0) {
            printf("Minuto %d:\n", ciclo / (60 / CICLO_TIME));
            for(i = 0; i < tamanho_estrada; ++i) {
                /* POG PRA LEGIBILIDADE. TIRAR ANTES DE ENTREGAR. NÃO ESQUECER */
                if(LISTsize(estrada[i].ciclistas) == 0) continue;
                printf("\tKM %.3d: ", i);
                LISTdump(estrada[i].ciclistas, dumpCiclista);
                puts("");
            }
            puts("");
        }
        for(i = 0; i < num_ciclistas; ++i) {
            int rc = pthread_create(&ciclistas_threads[i], NULL, CiclistaThread, (void *) ciclistas[i]);
            assert(0 == rc);
        }
        for(i = 0; i < num_ciclistas; ++i) {
            int rc = pthread_join(ciclistas_threads[i], NULL);
            assert(0 == rc);
        }
        ciclo++;
    }

    LISTdump(ciclistas_terminaram, dumpCiclista);

    LISTdestroy(ciclistas_terminaram);
    pthread_mutex_destroy(&terminar_mutex);

    LISTdestroy(trechos);
    for(i = 0; i < num_ciclistas; ++i)
        free(ciclistas[i]);
    free(ciclistas);
    free(ciclistas_threads);

    for(i = 0; i < tamanho_estrada; ++i) {
        LISTdestroy(estrada[i].ciclistas);
        pthread_mutex_destroy(&estrada[i].mutex);
    }
    free(estrada);
    return 0;
}