/*************************************************************************/
/* Author      : Kevin Françoisse (kfrancoi@gmail.com                    */
/* Version     : 1.00                                                    */
/* Modified    : 29/03/2009                                              */
/*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include "dwalk.h"
#include "memory.h"
#include "matrix.h"
#include "textDisplay.h"
#include "tools.h"
#include "pyDWalk.h"
#include "crossval.h"
#include "perfMeasure.h"


/*The discriminative random walks for every node, labeled node as well, to get a betweeness vector for each node
for each class
*/
int dwalkLauncher(double* adj, int nbRows, int nbCols, int* target, int nt, int nbClass, int wlen, double* bet){
	
	int i,j,c;
	int nbLabeled;
	int nbUnlabeled;
	int actualWlen;
	real tmp;
	int* classCount = NULL;
	char* labels = NULL;
	real** N = NULL;
	real** latF = NULL;
    real** latB = NULL;
	real* priors = NULL;
	int** classes = NULL;
	uchar* absorbing = NULL;
	SparseMatrix* spmat = NULL;
	int nbNodes = nbRows;
	uchar cross = 0;
	int verbose = 1;
	DWInfo* dwInfo = NULL;
	/*InputParams* inputParams = NULL;
	OptiLen*       optiLen       = NULL;*/

	/* Define options */
	/*inputParams = malloc(sizeof(InputParams));
	init_InputParams(inputParams);*/
	
	/*inputParams->nbFolds = 0; 
	inputParams->wlen = 30; 
	inputParams->verbose = 1; 
	inputParams->crossWalks = 0;*/
	
	
	/*Allocate data structure*/
	classCount			= int_vec_alloc(nbClass+1);
	labels				= char_vec_alloc(nbNodes);
	latF				= mat_alloc(wlen+1,nbNodes);
	latB				= mat_alloc(wlen+1,nbNodes);
	classes 			= (int**)malloc(sizeof(int*)*(nbClass+1));
	absorbing			= uchar_vec_alloc(nbNodes);
	dwInfo				= malloc(sizeof(DWInfo));
	dwInfo->mass_abs 	= vec_alloc(nbClass);
	spmat				= spmat_alloc(nbNodes,nbNodes,1);
	/*optiLen          = malloc(sizeof(OptiLen));*/
	
	
	
	/*memcpy(&labels, target, nbNodes*sizeof(int));*/
	
	/*for (i=0;i<nbNodes;i++){
		labels[i] = ("%i",target[i]);
		printf("%c ",labels[i]);
	}*/
	
	getClassesInt(target,nbNodes,classes,nbClass,classCount);
	nbUnlabeled = classes[0][0];
	nbLabeled   = nbNodes - nbUnlabeled;
	
	/* Prios computation */
	/*inputParams->priors = vec_alloc(nbClass+1);
	inputParams->priors[0] = (real)nbClass;
	tmp = 0.0;
	for(i=1;i<=inputParams->priors[0];i++)
		tmp += classCount[i];
	for(i=1;i<=inputParams->priors[0];i++){
		inputParams->priors[i] = classCount[i]/tmp;
	}*/
	priors = vec_alloc(nbClass+1);
	priors[0] = (real)nbClass;
	tmp = 0.0;
	for(i=1;i<=priors[0];i++)
		tmp += classCount[i];
	for(i=1;i<=priors[0];i++){
		priors[i] = classCount[i]/tmp;
	}
	
	/*Allocate data structure*/
	N = mat_alloc(nbUnlabeled,nbClass);
	
	/*Initialize the sparse transition matrix and the dataset if required*/
	init_SparseMat(spmat);
	init_mat(N,nbUnlabeled,nbClass);
	
	/*Transform adj into spmat*/
	denseMatrixToSpmat(adj, nbRows, verbose, spmat);
	
	
	/*If required tune the maximum walk length by cross-validation*/
	/*if (inputParams->nbFolds != 0){
		crossValidateLength(spmat,absorbing,target,classes,nbClass,NULL,latF,latB,inputParams,optiLen);
		actualWlen = optiLen->len;
	}*/
	/*else {
		actualWlen = inputParams->wlen;
	}*/
	/*Launching DWalk*/
	dwalk(spmat,absorbing,classes,nbClass,N,latF,latB,wlen,cross,dwInfo,verbose);
	
	/*for(c=1;c<=nbClass;c++){
		N[i][c-1] = N[i][c-1]*inputParams->priors[c];
	}*/
	for(c=1;c<=nbClass;c++){
		N[i][c-1] = N[i][c-1]*priors[c];
	}
	
	/*Transform N into bet*/
	for(i=0;i<nbUnlabeled;i++){
		for(j=0;j<nbClass;j++){
			*(bet+((nbClass)*i)+j) = N[i][j];
		}
	}
}
