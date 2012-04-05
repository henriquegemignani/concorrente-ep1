#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "ciclista.h"

void MALLOC_DIE() { exit(404); }

int main(int argc, char **argv) {
    ciclista* c;
    AUTOMALLOC(c);
    free(c);
    return 0;
}