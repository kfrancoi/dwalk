/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 17/10/2008                                              */
/*************************************************************************/

#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "defines.h"
#include "matrix.h"
#include "tools.h"

/*Compute a similartiy measure (specified in inputParams) between sparse vectors x and z*/
real similarity(SparseDim* x, SparseDim* z, InputParams* inputParams);

/*Compute the dot product between sparse vectors x and z*/
real dotProduct(SparseDim* x, SparseDim* z);

/*Compute the dot product between sparse vectors x and z*/
real L2Distance(SparseDim* x, SparseDim* z,uchar square);

/*Compute the polynomial kernel between sparse vectors x and z*/
real polyKernel(SparseDim* x, SparseDim* z,real a, real b,real c);

/*Compute the polynomial kernel between sparse vectors x and z*/
real gaussianKernel(SparseDim* x, SparseDim* z,real gamma);

/*Compute the inverse of the distance between x and z*/
real invDistance(SparseDim* x, SparseDim* z,real a);

#endif