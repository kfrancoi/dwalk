/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 01/08/2008                                              */
/*************************************************************************/

#ifndef GRAPHSTAT_H
#define GRAPHSTAT_H

#include "matrix.h"
#include "defines.h"
#include "tools.h"

/*Compute the minimum walk length required to classify all unlabeled nodes*/
int minCoveringLength(real*** N3,int wlen,int nbUnlabeled,int nbClass);

/*Compute the Minimum Covering Length statistics*/
int computeMCL(SparseMatrix* spmat,uchar* absorbing,char* labels,int** classes,int nbClass,
					  real** latF,real** latB,InputParams* inputParams);

#endif