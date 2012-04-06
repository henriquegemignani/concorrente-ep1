#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "libs/extra.h"
#include "libs/LL.h"

/* Nosso extra.h precisa desse codigo maroto. */
void MALLOC_DIE() { exit(404); }

struct Ciclista {
    int id;
    char *nome;
    int km;
    litem trecho_atual;
    int kms_no_trecho;
    double metros;
    double vel_plano;
    double vel_subida;
    double vel_descida;
    pthread_mutex_t mutex;

    int ponto_verde;
    int ponto_branco_vermelho;
};
typedef struct Ciclista *ciclista;

struct Trecho {
    char tipo;
    int distancia;

    list checkpoint_ranking;
};
typedef struct Trecho *trecho;

trecho newTrecho(char tipo, int dist) {
    trecho t;
    AUTOMALLOC(t);
    t->tipo = tipo;
    t->distancia = dist;
    t->checkpoint_ranking = LISTinit();
    return t;
}
void destroyTrecho(void* tv) {
    trecho t = (trecho) tv;
    LISTdestroy(t->checkpoint_ranking);
}
void dumpTrecho(void* tv) {
    trecho t = (trecho) tv;
    printf("Trecho: distancia = %d, tipo = %c\n", t->distancia, t->tipo);
}

#define CICLO_TIME 5

/* Região das globais legais */
int largura_estrada;
int tamanho_estrada;
char modo_simula;
list trechos;
pthread_cond_t ciclo_cond;

/* Em particular, lista de ranking da camisa amarela. */
pthread_mutex_t terminar_mutex;
list ciclistas_terminaram;

/* Temos d indíces em estrada, e cada índice pode possuir até n ciclistas. */
struct KM {
    pthread_mutex_t mutex;
    list ciclistas;
};
struct KM* estrada;

double kmh2ms(double kmh) { return kmh / 3.6; }

void* CiclistaThread(void* arg) {
    /* This is our thread. There are many like it, this is one is ours. */
    ciclista c = (ciclista) arg;
    double vel;

    pthread_mutex_lock(&c->mutex);
    /* Ciclista ja terminou a corrida. Nao corre mais poar. */
    while(c->km < tamanho_estrada) {
        if(modo_simula) {
            char tipo = c->trecho_atual ? ((trecho)c->trecho_atual->val)->tipo :
                ((trecho)trechos->first->val)->tipo;
            switch(tipo) {
            case 'P': vel = c->vel_plano; break;
            case 'S': vel = c->vel_subida; break;
            case 'D': vel = c->vel_descida; break;
            default: vel = 0.0;
            }
        } else {
            vel = c->vel_plano;
        }

        c->metros += kmh2ms(vel) * CICLO_TIME;

        /* Precisa mudar de KM? */
        if(c->metros >= 1000) {

            /* Estou terminando a corrida? */
            if(c->km + 1 >= tamanho_estrada) {
                /* YEAH TERMINEI! */

                /* Pega lock pra colocar na lista dos que terminaram. */
                pthread_mutex_lock(&terminar_mutex);
                LISTaddEnd(ciclistas_terminaram, c);
                pthread_mutex_unlock(&terminar_mutex);

                /* Pega lock para se tirar da lista de quem tava no ultimo km. 
                    Esse lock também me permite modificar a lista do trecho, que termina nesse mesmo km. */
                pthread_mutex_lock(  &estrada[c->km].mutex);
                LISTremove(estrada[c->km].ciclistas, c);

                /* Eu certamente terminei o último trecho. */
                LISTaddEnd(((trecho) trechos->last->val)->checkpoint_ranking, c);

                pthread_mutex_unlock(&estrada[c->km].mutex);

                c->km++;
                c->metros -= 1000;

                break;
            }

            /* Pega lock para Ler e possívelmente escrever no próximo KM. */
            pthread_mutex_lock(&estrada[c->km + 1].mutex);
            if(LISTsize(estrada[c->km + 1].ciclistas) < largura_estrada) {

                /* Opa, consegui entrar no proximo km, to rox. */
                LISTaddEnd(estrada[c->km + 1].ciclistas, c);
                pthread_mutex_unlock(&estrada[c->km + 1].mutex);

                c->kms_no_trecho++;

                if(c->km >= 0) {
                    trecho trecho_atual = (trecho) c->trecho_atual->val;

                    /* Se eu não estou no limbo de começo de corrida...
                        1. vou me tirar da lista do km anterior. Preciso travar o mutex senão da merda. 
                        2. Preciso ver se eu estou terminando algum trecho.
                         -> Se sim, o mesmo lock do km me permite mexer nesse mutex.
                    */

                    pthread_mutex_lock(  &estrada[c->km].mutex);
                    LISTremove(estrada[c->km].ciclistas, c);
                    if(c->kms_no_trecho == trecho_atual->distancia) {
                        /* Opa, terminei o meu trecho! :D */
                        LISTaddEnd(trecho_atual->checkpoint_ranking, c);
                        c->trecho_atual = c->trecho_atual->next;
                        c->kms_no_trecho = 0;
                    }
                    pthread_mutex_unlock(&estrada[c->km].mutex);
                } else {
                    /* Estou começando a corrida agora. */
                    c->trecho_atual = trechos->first;
                    c->kms_no_trecho = 0;
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
        pthread_cond_wait(&ciclo_cond, &c->mutex);
    }
    pthread_mutex_unlock(&c->mutex);
    return NULL;
}

double randRange(double min, double max) {
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

ciclista NewCiclista(int id) {
    ciclista c;
    AUTOMALLOC(c);
    c->id = id;
    c->km = -1;
    c->metros = 1000;
    c->trecho_atual = NULL;
    c->kms_no_trecho = 0;
    AUTOMALLOCV(c->nome, strlen("Ciclista") + 1);
    strcpy(c->nome, "Ciclista");
    if(modo_simula) {
        c->vel_descida = randRange(20.0, 80.0);
        c->vel_plano   = randRange(20.0, 80.0);
        c->vel_subida  = randRange(20.0, 80.0);
    } else 
        c->vel_descida = c->vel_plano = c->vel_subida = 50.0;
    pthread_mutex_init(&c->mutex, NULL);
    c->ponto_verde = 0;
    c->ponto_branco_vermelho = 0;
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
    ciclista* ciclistas;
    pthread_t* ciclistas_threads;

    srand((unsigned int) time(NULL));

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

    pthread_cond_init (&ciclo_cond, NULL);

    trechos = LISTinit();
    while(!feof(in)) {
        char tipo;
        int dist;
        do tipo = fgetc(in); while(tipo == '\n');
        fscanf(in, "%d", &dist);

        LISTaddEnd(trechos, newTrecho(tipo, dist));
    }
    LISTdump(trechos, dumpTrecho);
    for(i = 0; i < num_ciclistas; ++i)
        ciclistas[i] = NewCiclista(i);

    for(i = 0; i < num_ciclistas; ++i)
        printf("%.2d: %s [%.2lf; %.2lf; %.2lf]\n", i, ciclistas[i]->nome, ciclistas[i]->vel_descida, ciclistas[i]->vel_plano, ciclistas[i]->vel_subida);



    ciclo = 0;

    for(i = 0; i < num_ciclistas; ++i) {
        int rc = pthread_create(&ciclistas_threads[i], NULL, CiclistaThread, (void *) ciclistas[i]);
        assert(0 == rc);
    }

    while(1) {
        for(i = 0; i < num_ciclistas; ++i)
            pthread_mutex_lock(&ciclistas[i]->mutex);
        /* Agora todos os ciclistas estão esperando o sinal. */
        
        if(ciclo % (60 / CICLO_TIME) == 0) {
            printf("Minuto %d:\n", ciclo / (60 / CICLO_TIME));
            for(i = 0; i < tamanho_estrada; ++i) {
                printf("\tKM %.3d: ", i);
                LISTdump(estrada[i].ciclistas, dumpCiclista);
                puts("");
            }
            puts("");
        }
        ciclo++;
        if(LISTsize(ciclistas_terminaram) == num_ciclistas)
            break;
        for(i = 0; i < num_ciclistas; ++i)
            pthread_mutex_unlock(&ciclistas[i]->mutex);
        pthread_cond_broadcast(&ciclo_cond);
    }
    for(i = 0; i < num_ciclistas; ++i)
        pthread_mutex_unlock(&ciclistas[i]->mutex);

    puts("Ranking de Tempo:");
    LISTdump(ciclistas_terminaram, dumpCiclista);
    puts("");

    {
        /* Para cada trecho, calcular pontuação dos ciclistas */
        litem p;
        for(p = trechos->first; p; p = p->next) {
            trecho t = (trecho) p->val;
            litem x;
            int pos;
            /* Pega o primeiro, segundo e terceiro.
                For conveniente que trata caso de menos de 3 ciclistas. */
            for(pos = 3, x = t->checkpoint_ranking->first; pos > 0 && x; --pos, x = x->next) {
                if(t->tipo == 'P')
                    ((ciclista) x->val)->ponto_verde += pos;
                else if(t->tipo == 'S')
                    ((ciclista) x->val)->ponto_branco_vermelho += pos;
            }
        }
    }


    LISTdestroy(ciclistas_terminaram);
    pthread_mutex_destroy(&terminar_mutex);

    /* for trecho in trechos free */
    LISTcallback(trechos, destroyTrecho);
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
    pthread_cond_destroy(&ciclo_cond);
    return 0;
}