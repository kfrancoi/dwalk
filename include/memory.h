/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 27/02/2008                                              */
/*************************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include "tools.h"
#include "perfMeasure.h"

/* Allocate a vector of reals */
real *vec_alloc(int n);

/* Allocate a vector of integers */
int *int_vec_alloc(int n);

/* Allocate a vector of unsigned char */
char *char_vec_alloc(int n);

/* Allocate a vector of unsigned char */
uchar *uchar_vec_alloc(int n);

/* Allocate a vector of PerfMeasures structs */
PerfMeasures* PerfMeasures_vec_alloc(int n,int nbClass);

/* Allocate a vector of Similarity structs */
Similarity *Similarity_vec_alloc(int n);

/* Allocate a matrix of reals */
real **mat_alloc(int n, int k);

/* Allocate a matrix of integers */
int **int_mat_alloc(int n, int k);

/* Allocate a matrix of char */
char **char_mat_alloc(int n,int k);

/* Allocate a matrix of PerfMeasures */
PerfMeasures **PerfMeasures_mat_alloc(int n,int k,int nbClass);

/* Allocate a matrix of Similarity structs */
Similarity **Similarity_mat_alloc(int n,int k);

/* Allocate a sparse matrix of reals */
SparseMatrix *spmat_alloc(int n, int k,int allocRowSums);

/* Allocate a tridimensional array of reals */
real ***mat3_alloc(int n, int k, int v);

/* Allocate a PerfMeasure struct (including the contained confusion matrix)*/
PerfMeasures* PerfMeasures_alloc(int nbClass);

/* Allocate a DataStat struct (including the class count & prop)*/
DataStat* DataStat_alloc(int nbClass);

/*Allocate a RandMemory struct (including the partition)*/
RandMemory* RandMemory_alloc(int n);

/* Allocate a tridimensional array of integers */
int ***int_mat3_alloc(int n, int k, int v);

/*Free a vector of PerfMeasures struct*/
void free_PerfMeasures_vec(PerfMeasures* vec, int dim1, int nbClass);

/* Deallocate a matrix of reals*/
void free_mat(real **matrix, int dim1);

/* Deallocate a matrix of integers */
void free_int_mat(int **matrix, int dim1);

/* Deallocate a matrix of PerfMeasures*/
void free_PerfMeasures_mat(PerfMeasures **matrix, int dim1, int dim2,int nbClass);

/* Deallocate a matrix of Similarity structs */
void free_Similarity_mat(int **matrix, int dim1);

/* Deallocate a tridimensional array of reals */
void free_mat3(real ***matrix, int dim1, int dim2);

/* Deallocate a tridimensional array of integers*/
void free_int_mat3(int ***matrix, int dim1, int dim2);

/*Free a SparseMatrix*/
void free_SparseMatrix(SparseMatrix* spmat);

/*Free memory for each class*/
void freeClasses(int** classes,int nbClass);

/*Free a PerfMeasures struct*/
void free_PerfMeasures(PerfMeasures* perfMeasures, int nbClass);

/*Clear an InputParams struct*/
void free_InputParams(InputParams* inputParams);

/*Clear a DWInfo struct*/
void free_DWInfo(DWInfo* dwInfo);

/*Clear a DataStat struct*/
void free_DataStat(DataStat* dataStat);

/*Free memory for each class*/
void free_classes(int** classes,int nbClass);

/*Free a RandMemory struct*/
void free_RandMemory(RandMemory* memory);

/*Free memory for each class*/
void clear_classes(int** classes,int nbClass);

/* Free a SparseDim horizontally*/
void clear_SparseRow(SparseDim spdim);

/* Free a SparseDim vertically */
void clear_SparseCol(SparseDim spdim);

/* Initialize a vector of reals with zeros */
void init_vec(real *vector, int dim1);

/* Initialize a vector of integers with zeros */
void init_int_vec(int *vector, int dim);

/* Initialize a vector of integers with zeros */
void init_uchar_vec(uchar *vector, int dim);

/* Initialize a vector of chars with zeros */
void init_char_vec(char *vector, int dim);

/* Initialize a matrix of reals with zeros */
void init_mat(real **matrix, int dim1, int dim2);

/* Initialize a matrix of unsigned integers with zeros */
void init_int_mat(int **matrix, int dim1, int dim2);

/* Initialize a 3-dimensional matrix of reals with zeros */
void init_mat3(real ***matrix, int dim1, int dim2,int dim3);

/* Initialize a 3-dimensional matrix of integers with zeros */
void init_int_mat3(int ***matrix, int dim1, int dim2,int dim3);

/*Initialize a sparse matrix*/
void init_SparseMat(SparseMatrix* spmat);

/*Initialize an array of SparseDim*/
void init_Dims(SparseDim* sdim,int dim1);

/*Initialize a PerfMeasures struct*/
void init_PerfMeasures(PerfMeasures* perfMeasures,int nbClass);

/*Set the default input parameters*/
void init_InputParams(InputParams* inputParams);

/*Initialize a LSVMDataInfo struct*/
void init_LSVMDataInfo(LSVMDataInfo* info);

/*Initialize a RandMemory struct*/
void init_RandMemory(RandMemory* memory);

#endif
