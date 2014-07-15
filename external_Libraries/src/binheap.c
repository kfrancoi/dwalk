#include <stdlib.h>
#include <stdio.h>
#include "binheap.h"

PriorityQueue pqInitialize( int MaxElements )
{
	PriorityQueue H;

	H = malloc( sizeof( struct HeapStruct ) );
	if( H ==NULL ){
		fprintf(stderr, "Out of space!!!\n" );
		exit(EXIT_FAILURE);
	}

	/* Allocate the array plus one extra for sentinel */
	H->Elements = malloc( ( MaxElements + 1 )* sizeof( ElementType ) );
	if( H->Elements == NULL ){
		fprintf(stderr, "Out of space!!!\n" );
		exit(EXIT_FAILURE);
	}

	H->Capacity = MaxElements;
	H->Size = 0;
	H->Elements[0].idx = -1;
	H->Elements[0].val = -1;

	return H;
}


void pqMakeEmpty( PriorityQueue H )
{
	H->Size = 0;
}

/*Added by J.Callut*/
void pqInsertIfSpaceOrGreaterThanMin(ElementType X, PriorityQueue H){

	ElementType min;

	if( ! pqIsFull( H ) )
		pqInsert( X, H );
	else{
		min = pqFindMin(H);
		if(H->compareElementType(&(X),&(min)) > 0){
			pqDeleteMin(H);
			pqInsert( X, H );
		}
	}
}

void pqInsert( ElementType X, PriorityQueue H )
{
	int i;

	if( pqIsFull( H ) )
	{
		fprintf(stderr, "Priority queue is full\n" );
		return;
	}

	for( i = ++H->Size; H->compareElementType(&(H->Elements[ i / 2 ]),&(X)) > 0; i /= 2 )
		H->Elements[ i ] = H->Elements[ i / 2 ];
	H->Elements[ i ] = X;
}

ElementType pqDeleteMin( PriorityQueue H )
{
	int i, Child;
	ElementType MinElement, LastElement;

	if( pqIsEmpty( H ) )
	{
		fprintf(stderr, "Priority queue is empty\n" );
		return H->Elements[ 0 ];
	}
	MinElement = H->Elements[ 1 ];
	LastElement = H->Elements[ H->Size-- ];

	for( i = 1; i * 2 <= H->Size; i = Child )
	{
		/* Find smaller child */
		Child = i * 2;

		if(Child != H->Size &&  H->compareElementType(&(H->Elements[ Child + 1 ]),&(H->Elements[ Child ])) < 0)
			Child++;

		/* Percolate one level */
		if(H->compareElementType(&(LastElement),&(H->Elements[ Child ])) > 0 )
			H->Elements[ i ] = H->Elements[ Child ];
		else
			break;
	}
	H->Elements[ i ] = LastElement;
	return MinElement;
}


ElementType pqFindMin( PriorityQueue H )
{
	if( !pqIsEmpty( H ) )
		return H->Elements[ 1 ];
	fprintf(stderr, "Priority Queue is Empty\n" );
	return H->Elements[ 0 ];
}

int pqIsEmpty( PriorityQueue H )
{
	return H->Size == 0;
}

int pqIsFull( PriorityQueue H )
{
	return H->Size == H->Capacity;
}

void pqDestroy( PriorityQueue H )
{
	free( H->Elements );
	free( H );
}