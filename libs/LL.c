/*******************************************************************/
/** LINKED LIST implementation                                    **/
/** Requires: nothing extra                                       **/
/** Header: LL.h                                                  **/
/**                                                               **/
/** Developed by:                                                 **/
/** Henrique Gemignani Passos Lima      nUSP: ???????             **/
/*******************************************************************/

#include "extra.h"
#include "LL.h"
#include <stdio.h>
#include <stdlib.h>

struct LLhead {
    litem first;
    litem last;
    int size;
};

struct LL {
    void *val;
    litem next;
};

list LISTinit() {
    list l;
    AUTOMALLOC(l);
    l->first = l->last = NULL;
    l->size = 0;
    return l;
}
void* LISTval(litem l) {
    return l->val;
}
void LISTdump(list l, void (*valDumpFunc)(void*)) {
    litem p;
    for( p = l->first; p != NULL; p = p->next)
        valDumpFunc(p->val);
}

int LISTsize(list l) {
    return l->size;
}

void LISTdestroy(list l) {
    litem p, tmp;
    for(p = l->first; p != NULL; p = tmp) {
        tmp = p->next;
        free(p);
    }
    free(l);
}
void LISTremove(list l, void* val) {
    litem p, tmp;
    if(!l->first) return;
    if(l->first->val == val) {
        tmp = l->first;
        l->first = tmp->next;
        if(l->last == tmp) l->last = NULL;
        free(tmp);
        --l->size;
        return;
    }
    for(p = l->first; p->next; p = p->next )
        if( p->next->val == val ) {
            tmp = p->next;
            if(tmp == l->last)
                l->last = p;
            p->next = tmp->next;
            free(tmp);
            --l->size;
            return;
        }
}

litem LISTcreate(void *val, litem next) {
    litem l; AUTOMALLOC(l);
    l->val = val;
    l->next = next;
    return l;
}
litem LISTaddStart(list l, void *val) {
    litem n = LISTcreate(val, NULL);
    if( l->first == NULL ) {
        /* Lista l está vazia. */
        l->first = l->last = n;
    } else {
        n->next = l->first;
        l->first = n;
    }
    l->size++;
    return n;
}
litem LISTaddEnd(list l, void *val) {
    litem n = LISTcreate(val, NULL);
    if( l->first == NULL ) {
        l->first = l->last = n;
    } else {
        l->last->next = n;
        l->last = n;
    }
    l->size++;
    return n;
}

int LISTcontains(list l, void* val) {
    litem p;
    for( p = l->first; p; p = p->next )
        if( p->val == val )
            return 1;
    return 0;
}
