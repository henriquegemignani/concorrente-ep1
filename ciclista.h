#ifndef CICLISTA_H_
#define CICLISTA_H_

struct Ciclista {
    int id;
    char *nome;
    int km;
    double metros;
    double vel_plano;
    double vel_subida;
    double vel_descida;
};

typedef struct Ciclista *ciclista;

#endif /* CICLISTA_H_ */
