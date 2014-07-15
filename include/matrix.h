/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "defines.h"

/*****************************DATA STRUCTURES*****************************/

/*Store one dimension of a sparse matrix*/
typedef struct SparseDim{
    struct SparseElem *first;
    struct SparseElem *last;
}SparseDim;

/*Store one element of a sparse matrix*/
typedef struct SparseElem{
    int    l;
    int    c;
    real   val;
    struct SparseElem *nextL;
    struct SparseElem *nextC;
}SparseElem;

/*This structure can either contain a sparse matrix*/
/*or a compressed version of a sparse matrix (In/Out-Of-Core)*/
typedef struct SparseMatrix{
	int        nbRows;
	int        nbCols;
	int        nnz;
	/*Some stats on the graph*/
	int        minOutDeg;
	int        maxOutDeg;
	real	   meanOutDeg;
	/*************************/
	uchar      compressed;
	uchar	   symmetric;
	real*      rowSums;
	SparseDim* rows;
	SparseDim* cols;
}SparseMatrix;

/*************************************************************************/

/* Compute the sum of a vector of reals*/
real vecSum(real* vec,int m);

/* Compute the sum of a vector of integers*/
int int_vecSum(int* vec,int m);

/*Divide all elements of a vector of reals by a number a*/
void vecNormalize(real* vec,int m,real a);

/*Divide all elements of a sparse matrix by a number a*/
void matNormalize(SparseDim* rows,int m,real a);

/*Compute the position of the maximum element in a vector of reals*/
int max_vec_pos(real* vec,int m);

/*Enforce the stochasticity of a sparse matrix of positive real*/
void stochMat(SparseMatrix* spmat);

/*Enforce the stochasticity of a vector of positive real */
real stochVector(real* vec,int m);

/* Build a forward lattice up to wlen*/
void buildLatticeForward(real** lat,int wlen,SparseMatrix* spmat,int* startClass,int startState,uchar* absorbing,uchar cross,int verbose);

/* Build a backward lattice up to wlen*/
void buildLatticeBackward(real** lat,int wlen,SparseMatrix* spmat,uchar* absorbing,uchar cross,int verbose);

/*Compute the cumulated absorption probability for random walks allowing self-return*/
real cumulatedAbsorptionProba(real** latB,int wlen,int* startClass);

/*Set all the groups being absorbing except the starting group, here no absorption list are created SparseDim*/
void setAbsStates(uchar* absorbing,int* class_p,int* class_c,int start);

/*Test if a sparse matrix is symmetric*/
int isSymmetric(SparseMatrix* spmat);

/*Compute the sum of all elements of a SparseMatrix*/
real SparseMatrix_sum(SparseMatrix* spmat);

/*Sum two dense matrices of reals a = a + b*/
void sum_mat(real** a,real** b,real coeff,int dim1,int dim2);

/*Sum two dense matrices of integers a = a + b*/
void sum_int_mat(int** a,int** b,int dim1,int dim2);

/*Normalize the columns of a sparse matrix such that entries are in [0,1]*/
void normalizeColumns(SparseMatrix* spmat,int verbose);

/*Transform a dense matrix into a sparse matrix */
void denseMatrixToSpmat(double* adj,int nbRows,int verbose,SparseMatrix* spmat);

#endif
