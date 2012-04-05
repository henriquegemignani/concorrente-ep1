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

typedef struct LLhead *list;
typedef struct LL *litem;

struct LLhead {
    litem first;
    litem last;
    int size;
};

struct LL {
    void *val;
    litem next;
};

list LISTinit();
#define LISTcallback LISTdump 
void LISTdump(list, void (*valDumpFunc)(void*));
int  LISTsize(list);

/* Removal */
void LISTdestroy(list);
void LISTremove(list, void* val);


/* Insertion */
litem LISTaddStart(list, void *);
litem LISTaddEnd(list, void *);

int LISTcontains(list l, void* val);

#endif
