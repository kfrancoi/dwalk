/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 13/02/2008                                              */
/*************************************************************************/

#ifndef DWALK_H
#define DWALK_H

#include "tools.h"

/*The discriminative random walks for every node, labeled node as well, to get a betweeness vector for each node
for each class
*/
int dwalkLauncher(double* adj, int na, int ma, int* target, int nt, int nbClass, int wlen, double* bet);

					
/*The discriminative random walks 
	- Computing betweenness for unlabeled nodes only
	- Allowing self-return walks
	- Up to length wlen
	- Ultra-fast method !
*/
void dwalk(SparseMatrix* spmat,uchar* absorbing,int** classes,int nbClass,real** N,
		   real** latF,real** latB,int wlen,uchar cross,DWInfo *dwInfo,int verbose);


/*The discriminative random walks 
	- Computing betweenness for unlabeled nodes only
	- Allowing self-return walks
	- Separately for all lengths up to wlen
	- Ultra-fast method !
*/
void dwalk_all_lengths(SparseMatrix* spmat,uchar* absorbing,int** classes,int nbClass,
					   real*** N3,real** latF,real** latB,int wlen,uchar cross,int verbose);

												
#endif
