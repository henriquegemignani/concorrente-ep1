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

struct LL {
    void *val;
    list next;
};

list LISTnext(list l) {
    return l->next;
}
void LISTsetNext(list l, list n) {
    l->next = n;
}
void* LISTval(list l) {
    return l->val;
}
void LISTdump(list l, void (*valDumpFunc)(void*)) {
    list p;
    for( p = l; p != NULL; l = p) {
        p = p->next;
        valDumpFunc(l->val);
        if( p != NULL ) printf(" -> ");
    }
}


int LISTremove(list l, void (*valRemoveFunc)(void*)) {
    if( l == NULL ) return 1;
    if(valRemoveFunc) valRemoveFunc(l->val);
    free(l);
    return 0;
}
int LISTremoveNext(list l, void (*valRemoveFunc)(void*)) {
    if( l->next == NULL ) return 1;
    if(valRemoveFunc) valRemoveFunc(l->next->val);
    free(l->next);
    l->next = NULL;
    return 0;
}
int LISTdestroy(list l, void (*valRemoveFunc)(void*)) {
    list p;
    for( p = l; p != NULL; l = p) {
        p = p->next;
        if(valRemoveFunc) valRemoveFunc(l->val);
        free(l);
    }
    return 0;
}

list LISTcreate(void *val, list next) {
    list l; AUTOMALLOC(l);
    l->val = val;
    l->next = next;
    return l;
}
list LISTaddNext(list l, void *val) {
    list n = LISTcreate(val, NULL);
    if( l == NULL ) return n;
    n->next = l->next;
    l->next = n;
    return l;
}
list LISTaddEnd(list l, void *val) {
    list p, n = LISTcreate(val, NULL);
    if( !l ) return n;
    for( p = l; p->next; p = p->next );
    p->next = n;
    return l;
}

list LISTcreateMergeOrdered(list l, list p, int (*valCompareFunc)(void*, void*)) {
    list q = l, r = p, resp = NULL;
    int i;
    while( q && r ) {
        i = valCompareFunc(q->val, r->val);
        if( i >= 0 ) {
            if( !LISTcontains(resp, r->val, valCompareFunc) )
                resp = LISTaddEnd(resp, r->val);
            r = r->next;
            if( i == 0 )
                q = q->next;
        } else if( i < 0 ) {
            if( !LISTcontains(resp, q->val, valCompareFunc) )
                resp = LISTaddEnd(resp, q->val);
            q = q->next;
        }
    }
    for( ; q != NULL; q = q->next )
        resp = LISTaddEnd(resp, q->val);
    for( ; r != NULL; r = r->next )
        resp = LISTaddEnd(resp, r->val);
    return resp;
}

list LISTcreateIntersection(list l, list p, int (*valCompareFunc)(void*, void*)) {
    list q, resp = NULL;
    if( !l || !p ) return NULL;
    for( q = l; q; q = q->next ) {
        if( LISTcontains( p, q->val, valCompareFunc) && !LISTcontains( resp, q->val, valCompareFunc) )
            resp = LISTaddEnd(resp, q->val);
    }
    return resp;
}
list LISTcreateDifference(list l, list p, int (*valCompareFunc)(void*, void*)) {
    list q, resp = NULL;
    if( !l || !p ) return NULL;
    for( q = l; q; q = q->next )
        if( !LISTcontains( p, q->val, valCompareFunc) && !LISTcontains( resp, q->val, valCompareFunc) )
            resp = LISTaddEnd(resp, q->val);
    return resp;
}

/* Insere LIST n no final da lista RESP->...->END
 Uso interno apenas. */
list LISTcopyAUX(list resp, list *end, list n) {
    if( resp == NULL ) {
        *end = n;
        return n;
    }
    (*end)->next = n;
    *end = n;
    return resp;
}
list LISTcopy(list l) {
    list p, n, resp = NULL, end = NULL;
    for( p = l; p; p = p->next ) {
        n = LISTcreate( p->val, NULL );
        resp = LISTcopyAUX(resp, &end, n);
    }
    return resp;
}


list LISTinvertOrderR(list *l) {
    list p;
    if( *l == NULL || (*l)->next == NULL ) {
        return *l;
    } else {
        p = *l;
        *l = p->next;
        LISTinvertOrderR(l)->next = p;
        p->next = NULL;
        return p;
    }
}
void LISTinvertOrder(list *l) {
    LISTinvertOrderR(l);
}

int LISTcontains(list l, void* val, int (*valCompareFunc)(void*, void*)) {
    list p;
    for( p = l; p; p = p->next )
        if( valCompareFunc( p->val, val ) == 0 )
            return 1;
    return 0;
}
