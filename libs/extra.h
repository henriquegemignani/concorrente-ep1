#ifndef EXTRA_H_
#define EXTRA_H_

#define AUTOMALLOC(p) p=malloc(sizeof(*(p)));if(!p)MALLOC_DIE()
#define AUTOMALLOCV(p,n) p=malloc((n)*sizeof(*(p)));if(!p)MALLOC_DIE()
#define AUTOMALLOCADV(p,s) p=malloc(s);if(!p)MALLOC_DIE()
void MALLOC_DIE();

#endif /* EXTRA_H_ */
