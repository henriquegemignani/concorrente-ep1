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

list LISTinit();
litem LISTnext(litem);
void LISTsetNext(litem l, litem n);
void* LISTval(litem);
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
