/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 07/02/2008                                              */
/*************************************************************************/

#ifndef CROSSVAL_H
#define CROSSVAL_H

#include "defines.h"
#include "tools.h"
#include "matrix.h"

/*************************************************************************/

/*Shuffle an array of integers*/
void shuffle(int *vec, int m);

/*Stratified k-fold cross-validation*/
void stratifiedCV(int** classes,int nbClass,int **folds,InputParams *inputParams);

/*Stratify the list of instances such that the proportion of each class in the*/
/*folds represents correctly the proportion of each class in the dataset*/
void stratify(int* idx,int* stratIdx,int m,int nbFolds);

/*Construct the classes relatively to the current fold of the cross-validation*/
void foldsToClasses(int** folds,int nbFolds,int testFold,int** classes,int nbClass,char* labels);

/*Compute a confusion matrix from a time slice*/
void sliceToConfusion(real** slice,int nbClass,int* unlabeled,char* labels,real* priors,int** confusion);

/*Update the matrix of folded performance for each time*/
void updateFoldedPerf(real*** N3,int* unlabeled,char* labels,InputParams* inputParams,int nbClass,int curFold,PerfMeasures** foldedPerf);

/*Find the optimal walk length based on the cross-validated performances*/
int findBestWalkLength(InputParams* inputParams,PerfMeasures* avgPerf,PerfMeasures* devPerf);

/*Average the performances on the folds*/
void averagePerfsOnFolds(PerfMeasures** foldedPerf,InputParams* inputParams,int nbClass,PerfMeasures* avg, PerfMeasures* dev);

/*Find the optimal walk length using cross-validation*/
void crossValidateLength(SparseMatrix* spmat,uchar* absorbing,char* labels,int** classes,int nbClass,
						 int**folds,real** latF,real** latB,InputParams* inputParams,OptiLen* optiLen);

#endif
