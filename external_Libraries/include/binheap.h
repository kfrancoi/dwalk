#ifndef _BINHEAP_H
#define _BINHEAP_H

#include "tools.h"

typedef Similarity ElementType;

struct HeapStruct
{
	int Capacity;
	int Size;
	ElementType *Elements;
	int (*compareElementType)(const void*,const void*);
};
typedef struct HeapStruct* PriorityQueue;

PriorityQueue pqInitialize( int MaxElements );
void          pqDestroy( PriorityQueue H );
void          pqMakeEmpty( PriorityQueue H );
void          pqInsertIfSpaceOrGreaterThanMin(ElementType X, PriorityQueue H);
void          pqInsert( ElementType X, PriorityQueue H );
ElementType   pqDeleteMin( PriorityQueue H );
ElementType   pqFindMin( PriorityQueue H );
int           pqIsEmpty( PriorityQueue H );
int           pqIsFull( PriorityQueue H );

#endif

