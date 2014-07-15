/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#ifndef FILES_H
#define FILES_H

#include "tools.h"

/* Read I in input file */
int readVec(char* fname,real* I,int m);

/* Read matrix of real in input file */
int readMat(char* fname,SparseMatrix* spmat,char* labels,DataStat* DataStat,int verbose);

/* Read a LibSVM/SVMLight dataset from a file */
int readLSVMData(char* fname,LSVMDataInfo* dataInfo,SparseMatrix* spmat,char* labels,DataStat* dataStat,int verbose);

/*Read the number of instance in a gram matrix*/
int readNbInst(char* fname);

/*Read the gram matrix from a libsvm file*/
void readGram(char* fname,real* gram,int* labels,int* ids,int m);

/* Read the number of distrinct classes*/
int readNbClass(char* fname);

/* Read the number of nodes from the adjacency matrix file */
int readNbNodes(char* fname);

/* Read the number of distrinct classes*/
void getLSVMDataInfo(char* fname,LSVMDataInfo* info);

/* Read a file containing targets for unlabeled nodes*/
int readTargets(char* fname,int* unlabeledIdx,char* targets);

/* Read a file containing targets for unlabeled nodes*/
int readLSVMTargets(char* fname,char* targets);

/* Write a vector of real in a sparse file*/
void writeVec(real* I,int m,char* fname);

/* Write a matrix of real in a file*/
void writeMat(real** P,int m,char* fname);

/*Write the model prediction */
void writePredictions(char* fname,char* preds,real** N,int nbNodes,int nbClass);

/*Write the model prediction for unlabeled nodes only*/
void writePredictions_unlabeled(char* fname,char* preds,real** N,int nbClass,int* unlabeled);

#endif
