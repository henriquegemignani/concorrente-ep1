#ifndef CICLISTA_H_
#define CICLISTA_H_

#include "libs/LL.h"

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
};

typedef struct Ciclista *ciclista;

#endif /* CICLISTA_H_ */
