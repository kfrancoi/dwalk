/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include "defines.h"
#include "matrix.h"
#include "perfMeasure.h"

/*****************************DATA STRUCTURES*****************************/

typedef struct InputParams{
    int   wlen;
	int   nbFolds;
	int   cvSeed;
	int   crossWalks;
    int   verbose;
	int   perfMeasure;
	int   maxIter;
	int   featNormalize;
	int   nodeDegree;
	real  probeFactor;
	char* dataFname;
    char* graphFname;
	char* gramFname;
    char* outFname;
	char* tarFname;
	char* outPRED;
	char* outLEN;
	real* priors;
	real* simParams;
}InputParams;

typedef struct DataStat{
	int   nbAuxNodes;
	real  density;
	int*  classCount;
	real* classProp;
}DataStat;

typedef struct DWInfo{
	real* mass_abs;
	real  expWlen;
}DWInfo;

/*Informations on a LibSVM/SVMLight dataset*/
typedef struct LSVMDataInfo{
	int   nbLines;
	int   nbFeatures;
	int   nbClass;
	uchar M1P1; /*Binary labels encoded as -1/+1 */
}LSVMDataInfo;

typedef struct OptiLen{
	int   len;
	real  avgPerf;
	real  devPerf;
}OptiLen;

typedef struct Similarity{
	int   idx;
	real  val;
}Similarity;

typedef Similarity ListElemType;
typedef struct ListElem{
	struct ListElem* next;
	ListElemType     content;
}ListElem;

typedef struct LinkedList{
	int       nbElem;
	ListElem* first;
	ListElem* last;
}LinkedList;

typedef struct RandMemory{
	int  n;
	int  cursor;
	int* picked;
	int* partition;
}RandMemory;

/**************************************************************************/

/*Tokenize a list of filenames separated by commas*/
int tokenizeFnames(char* list,char** fnames);

/*Add an extension to a file name*/
void addExtension(char* fname,char* extension,char* target);

/*Add a class and an extension to a file name*/
void addClassAndExtension(char* fname,int classNum,char* extension,char* target);

/*Count the number of tokens in a string separated by sep*/
int getNbTokens(char* list,const char *sep);

/*Tokenize a string of real tokens separated by sep*/
void tokenizeInts(char* list,const char *sep,int* tokens);

/*Tokenize a string of real tokens separated by sep*/
void tokenizeReals(char* list,const char *sep,real* tokens);

/*Get the indices of the nodes in each class */
void getClasses(char* labels,int m,int** classes,int nbClass);

/*Get the indices of the nodes in each class */
void getClassesInt(int* labels,int m,int** classes,int nbClass,int* classCount);

/*Compute the argmax on the classes*/
void computePredictions(real** N,char* preds,real* priors,int m,int nbClass);

/*Compute the performance for unlabeled nodes provided their target values*/
void computeUnlabeledPerf(char* preds,int* unlabeled,int nbUnlabeled,int nbClass,
						  InputParams* inputParams,PerfMeasures* unlabeledPerf,int verbose);

/*Insert a new element in a linked list*/
void insertInList(LinkedList* list, ListElem* lelem);

/*Random integers without duplicates*/
int randWithMemory(RandMemory* memory);

/*Delete a position so that it can not be randomly picked*/
void randDelete(RandMemory* memory,int pos);

/*Reset the memory of the generator*/
void randReset(RandMemory* memory);

/* Print a SparseMatrix up to index "size" */
void printSparseMatrix(SparseMatrix* spmat, int size);

/*Build a kernel matrix based on a LIBSVM feature sparse matrix*/
void buildKernel(SparseMatrix* dataSet, real** kernel, InputParams* inputParams);

/*Build a kernel matrix based on a LIBSVM feature sparse matrix*/
void buildKernel2(SparseMatrix* spmat, SparseMatrix* dataset, real alpha, InputParams* inputParams);

#endif
