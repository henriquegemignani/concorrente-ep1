/*******************************************************************/
/** LINKED LIST interface definition                              **/
/** Requires: an ITEM implementation                              **/
/** Implementation: LL.c                                          **/
/**                                                               **/
/** Developed by:                                                 **/
/** Henrique Gemignani Passos Lima      nUSP: ???????             **/
/*******************************************************************/

#ifndef LL_H_
#define LL_H_

typedef struct LL *list;

#define LISTinit() NULL
list LISTnext(list);
void LISTsetNext(list l, list n);
void* LISTval(list);
void LISTdump(list, void (*valDumpFunc)(void*));
#define LISTempty(l) (l == NULL)

/* Removal */
int LISTremove(list, void (*valRemoveFunc)(void*));
int LISTremoveNext(list, void (*valRemoveFunc)(void*));
int LISTdestroy(list, void (*valRemoveFunc)(void*));


/* Insertion */
list LISTaddNext(list, void *);
list LISTaddEnd(list, void *);

/* Cria uma nova lista onde todos os valores dela pertence a L ou P. */
list LISTcreateMergeOrdered(list l, list p, int (*valCompareFunc)(void*, void*));

/* Cria uma nova lista onde todos os valores dela pertence a L e P. */
list LISTcreateIntersection(list l, list p, int (*valCompareFunc)(void*, void*));

/* Cria uma nova lista onde todos os valores dela pertence a L mas nao P. */
list LISTcreateDifference(list l, list p, int (*valCompareFunc)(void*, void*));

/* Devolve uma copia da lista dada.*/
list LISTcopy(list l);

/* Dado uma lista A->B->...->C->D, inverte tal lista para D->C->...->B->A */
void LISTinvertOrder(list *l);

/* Content */
/* Devolve 1 se valCompareFunc devolver 0 para algum par (val,listval),
 com listval pertencente a lista l. */
 int LISTcontains(list l, void* val, int (*valCompareFunc)(void*, void*));

#endif
