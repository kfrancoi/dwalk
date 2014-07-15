/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 08/02/2008                                              */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "defines.h"
#include "tools.h"
#include "memory.h"
#include "dwalk.h"
#include "crossval.h"
#include "perfMeasure.h"
#include "textDisplay.h"

/*Shuffle an array of integers*/
void shuffle(int* vec, int m)
{
   int temporary, randomNum, last;

   for (last = m; last > 1; last--)
   {
      randomNum = rand( ) % last;
      temporary = vec[randomNum];
      vec[randomNum] = vec[last - 1];
      vec[last - 1]  = temporary;
   }
}

/*Stratify the list of instances such that the proportion of each class in the*/
/*folds represents correctly the proportion of each class in the dataset*/
void stratify(int* idx,int* stratIdx,int m,int nbFolds){
	int j,start = 0;
	int nbInserted = 0;
	
	while(nbInserted < m){
		j = start;
		while(j < m){
			stratIdx[nbInserted++] = idx[j];
			j += nbFolds;
		}
		start++;
	}
}

/*Stratified k-fold cross-validation*/
void stratifiedCV(int** classes,int nbClass,int **folds,InputParams *inputParams){
	
	int   c,i,j;
	int   nbSelected;
	int   nbIdToCopy;
	int   nPerFold;
	int   residual;
	int   offset;
	int*  temp;
	int*  stratIdx;
	
	/*Determine the number of selected nodes*/
	nbSelected = 0;
	for(c=1;c<=nbClass;c++){
		nbSelected += classes[c][0];
	}
	
	/*Allocate arrays to build stratified folds*/
	temp     = int_vec_alloc(nbSelected);
	stratIdx = int_vec_alloc(nbSelected);
	
	/*Fill the temporary array*/
	for(c=1,j=0;c<=nbClass;c++){
		shuffle(classes[c]+1,classes[c][0]);
		nbIdToCopy = classes[c][0];
		for(i=1;i<=nbIdToCopy;i++)
			temp[j++] = classes[c][i];
	}
	
	/*Stratify the node indices*/
	stratify(temp,stratIdx,nbSelected,inputParams->nbFolds);
	
	/*Compute the number of nodes per folds and the residuals*/
	nPerFold = nbSelected / inputParams->nbFolds;
	residual = nbSelected % inputParams->nbFolds;
	
	/*Fill the folds*/
	for(i=0;i<inputParams->nbFolds;i++){
		folds[i][0] = 0;
		offset = i*nPerFold;
		for(j=offset;j<offset+nPerFold;j++)
			folds[i][++folds[i][0]] = stratIdx[j];
	}
	
	/*Put the residual in the folds*/
	if(residual > 0){
		for(i=0;i<residual;i++)
			folds[i][++folds[i][0]] = stratIdx[nbSelected-(i+1)];
	}
	
	/*Release memory*/
	free(temp);
	free(stratIdx);
	
	/*Show the folds
	for(i=0;i<nbFolds;i++){
		printf("Fold %i\n",i);
		show_int_vec(folds[i]+1,folds[i][0]);
	}*/
}

/*Compare two (pointers to) int for the sorting algorithms*/
int compareInts(const void* a, const void* b){
    
	int ia = *((int*)(a));
	int ib = *((int*)(b));

    if(ia > ib)
        return 1;
    else if(ia < ib)
        return -1;
    else
        return 0;
}

/*Construct the classes relatively to the current fold of the cross-validation*/
void foldsToClasses(int** folds,int nbFolds,int testFold,int** classes,int nbClass,char* labels){
	
	int  i,j,label;
	int *classCount = int_vec_alloc(nbClass+1);

	/*Count the number of nodes in each class*/
	init_int_vec(classCount,nbClass+1);
	for(i=0;i<nbFolds;i++){
		if(i == testFold){
			for(j=1;j<=folds[i][0];j++)
				classCount[0]++;
		}
		else{
			for(j=1;j<=folds[i][0];j++){
				label = labels[folds[i][j]];
				classCount[label]++;
			}
		}
	}
	
	/*Allocate the class data structure*/
	for(i=0;i<=nbClass;i++){
		classes[i] = int_vec_alloc(classCount[i]+1);
		classes[i][0] = 0;
	}
	
	/*Fill the classes*/
	for(i=0;i<nbFolds;i++){
		if(i == testFold){
			for(j=1;j<=folds[i][0];j++)
				classes[0][++classes[0][0]] = folds[i][j];
		}
		else{
			for(j=1;j<=folds[i][0];j++){
				label = labels[folds[i][j]];
				classes[label][++classes[label][0]] = folds[i][j];
			}
		}
	}
	
	/*Sort the classes
	for(i=0;i<=nbClass;i++){
		qsort(classes[i]+1,classes[i][0],sizeof(int),compareInts);
	}*/
	
	/*Release memory */
	free(classCount);
}

/*Compute a confusion matrix from a time slice*/
void sliceToConfusion(real** slice,int nbClass,int* unlabeled,char* labels,real* priors,int** confusion){
	
	int    i,c;
	int    bestClass = 0;
	real   bestScore = 0.0;
	real   curScore  = 0.0;
	
	/*Reinitialize the confusion matrix*/
	init_int_mat(confusion,nbClass+1,nbClass+1);
	
	/*Loop on unlabeled nodes*/
	for(i=1;i<=unlabeled[0];i++){
		/*Compute the argmax on the classes*/
		bestClass = 0;
		bestScore = 0.0;
		for(c=1;c<=nbClass;c++){
			curScore = slice[i-1][c-1]*priors[c];
			if(curScore > bestScore){
				bestClass = c;
				bestScore = curScore;
			}	
		}
		/*printf("unlabeled[i] = %i\n",unlabeled[i]);
		/*printf("rows[unlabeled[i]].label = %i\n",rows[unlabeled[i]].label);
		printf("bestClass = %i\n",bestClass);
		fflush(stdout);*/
		/*Update the confusion matrix*/
		confusion[labels[unlabeled[i]]][bestClass] += 1;
	}
}

/*Update the matrix of folded performance for each time*/
void updateFoldedPerf(real*** N3,int* unlabeled,char* labels,InputParams* inputParams,int nbClass,int curFold,PerfMeasures** foldedPerf){
	
	int t;
	
	/*Iterate on the time slices*/
	for(t=0;t<inputParams->wlen-1;t++){
		/*Get the confusion matrix for the current slice*/
		sliceToConfusion(N3[t],nbClass,unlabeled,labels,inputParams->priors,foldedPerf[curFold][t].confusion);
		/*Compute the performance measure*/
		/*computePerfs(&(foldedPerf[curFold][t]),nbClass);*/
		computePerfs(&(foldedPerf[curFold][t]),nbClass, inputParams->priors);
		/*show_ConfusionMatrix(confusion,nbClass,stdout);*/
	}
}

/*Average the performances on the folds*/
void averagePerfsOnFolds(PerfMeasures** foldedPerf,InputParams* inputParams,int nbClass,PerfMeasures* avg, PerfMeasures* dev){
	int  f,t; 
	real fact;

	/*Iterate on the walk length*/
	for(t=0;t<inputParams->wlen-1;t++){
		init_PerfMeasures(&(avg[t]),nbClass+1);
		init_PerfMeasures(&(dev[t]),nbClass+1);

		/*Average performance and cumulate confusion matrix on the folds*/
		for(f=0;f<inputParams->nbFolds;f++){
			sum_int_mat(avg[t].confusion,foldedPerf[f][t].confusion,nbClass+1,nbClass+1);
			fact = (real)(1.0/(float)inputParams->nbFolds);
			avg[t].classificationRate += (real)(fact*foldedPerf[f][t].classificationRate);
			avg[t].macroPrecision     += (real)(fact*foldedPerf[f][t].macroPrecision);
			avg[t].macroRecall        += (real)(fact*foldedPerf[f][t].macroRecall);
			avg[t].macroF1            += (real)(fact*foldedPerf[f][t].macroF1);
			avg[t].microPrecision     += (real)(fact*foldedPerf[f][t].microPrecision);
			avg[t].microRecall        += (real)(fact*foldedPerf[f][t].microRecall);
			avg[t].microF1            += (real)(fact*foldedPerf[f][t].microF1);			
		}
		/*Compute variance*/
		for(f=0;f<inputParams->nbFolds;f++){
			fact = (real)(1.0/(float)(inputParams->nbFolds-1));
			dev[t].classificationRate += (real)(fact *POW2(avg[t].classificationRate - foldedPerf[f][t].classificationRate));
			dev[t].macroPrecision     += (real)(fact *POW2(avg[t].macroPrecision     - foldedPerf[f][t].macroPrecision));
			dev[t].macroRecall        += (real)(fact *POW2(avg[t].macroRecall        - foldedPerf[f][t].macroRecall));
			dev[t].macroF1            += (real)(fact *POW2(avg[t].macroF1            - foldedPerf[f][t].macroF1));
			dev[t].microPrecision     += (real)(fact *POW2(avg[t].microPrecision     - foldedPerf[f][t].microPrecision));
			dev[t].microRecall        += (real)(fact *POW2(avg[t].microRecall        - foldedPerf[f][t].microRecall));
			dev[t].microF1            += (real)(fact *POW2(avg[t].microF1            - foldedPerf[f][t].microF1));				
		}
		/*Compute standard deviation*/
		dev[t].classificationRate = (real)sqrt(dev[t].classificationRate);
		dev[t].macroPrecision     = (real)sqrt(dev[t].macroPrecision);
		dev[t].macroRecall        = (real)sqrt(dev[t].macroRecall);
		dev[t].macroF1            = (real)sqrt(dev[t].macroF1);
		dev[t].microPrecision     = (real)sqrt(dev[t].microPrecision);
		dev[t].microRecall        = (real)sqrt(dev[t].microRecall);
		dev[t].microF1            = (real)sqrt(dev[t].microF1);				
	}
}

/*Find the optimal walk length based on the cross-validated performances*/
int findBestWalkLength(InputParams* inputParams,PerfMeasures* avgPerf,PerfMeasures* devPerf){

	int   t,bestLen=0; 
	real  avg,dev;
	real  bestAvg=0.0;
	real  bestDev=0.0;
	FILE* outFile;
	
	/*Create file for reporting performance for each walk length*/
	if(inputParams->outLEN){
		outFile = fopen(inputParams->outLEN,"w");
		/*Write header*/
		fprintf(outFile,"#1WalkLength\t2MeanPerf\t3StdDev\n");
	}
	
	/*Iterate on the walk length*/
	for(t=0;t<inputParams->wlen-1;t++){
		avg = getPerf(&(avgPerf[t]),inputParams->perfMeasure);
		dev = getPerf(&(devPerf[t]),inputParams->perfMeasure);
		/*Update the best measures*/
		if(avg > bestAvg || ((avg == bestAvg) && (dev < bestDev))){
			bestLen = t;
			bestAvg = avg;
			bestDev = dev;
		}
		/*Update the reporting*/
		if(inputParams->outLEN){
			fprintf(outFile,"%i\t%e\t%e\n",t+2,100*avg,100*dev);
		}
	}
	/*Close the reporting file*/
	if(inputParams->outLEN){
		fclose(outFile);
	}

	return bestLen+2;
}

/*Find the optimal walk length using cross-validation*/
void crossValidateLength(SparseMatrix* spmat,uchar* absorbing,char* labels,int** classes,int nbClass,
						 int**folds,real** latF,real** latB,InputParams* inputParams,OptiLen* optiLen){
	
	int     i;
	int     nbNodes           = spmat->nbRows;
	int     nbLabeled         = nbNodes - classes[0][0];
	int     nPerFold          = (int)ceil((real)nbLabeled/(real)inputParams->nbFolds);
	int     bestLen           = 0;
	int     releaseFolds      = 0;
	int**   cvClasses         = NULL;
	real*** N3                = NULL;
	PerfMeasures*  avgPerf    = NULL;
	PerfMeasures*  devPerf    = NULL;
	PerfMeasures** foldedPerf = NULL;
	
	if(inputParams->verbose >= 1){
		textcolor(BRIGHT,RED,BLACK);
		if(inputParams->nbFolds > 0)
			printf("Optimizing the walk length using %i-fold cross-validation\n",inputParams->nbFolds);
		else
			printf("Optimizing the walk length using leave-one-out\n");
		textcolor(RESET,WHITE,BLACK);
	}

	/*If the number of folds is negative use leave-one-out*/
	if (inputParams->nbFolds < 0) inputParams->nbFolds = nbLabeled;		
		
	/*Initialize some data structure*/
	N3         = mat3_alloc(inputParams->wlen-1,nPerFold,nbClass);
	cvClasses  = (int**)malloc(sizeof(int*)*(nbClass+1));
	avgPerf    = PerfMeasures_vec_alloc(inputParams->wlen-1,nbClass+1);
	devPerf    = PerfMeasures_vec_alloc(inputParams->wlen-1,nbClass+1);
	foldedPerf = PerfMeasures_mat_alloc(inputParams->nbFolds,inputParams->wlen-1,nbClass+1);
		
	/*If not provided allocate and define the folds*/
	if(folds == NULL){
		releaseFolds = 1;
		folds = int_mat_alloc(inputParams->nbFolds,nPerFold+1);
		stratifiedCV(classes,nbClass,folds,inputParams);
	}

	/*Show progression*/
	if(inputParams->verbose >= 1){
		textcolor(BRIGHT, GREEN, BLACK);
		printf("Cross-validation round : ");
		textcolor(RESET,WHITE,BLACK);
	}

	/*Iterate on the folds*/
	for(i=0;i<inputParams->nbFolds;i++){
		/*Show progression*/
		if(inputParams->verbose >= 1){
			if(i > 0) backspace(intLen(i)+intLen(inputParams->nbFolds)+1);
			textcolor(BRIGHT, GREEN, BLACK);
			printf("%i/%i",i+1,inputParams->nbFolds);
			textcolor(RESET,WHITE,BLACK);
			fflush(stdout);
		}
		/*Reinitialize the scores*/
		init_mat3(N3,inputParams->wlen-1,nPerFold,nbClass);
		/*Derive the classes from the folds*/
		foldsToClasses(folds,inputParams->nbFolds,i,cvClasses,nbClass,labels);
		/*Apply dwalks for every walk length up to wlen*/
		dwalk_all_lengths(spmat,absorbing,cvClasses,nbClass,N3,latF,latB,inputParams->wlen,inputParams->crossWalks,1);
		
		/*Update the performances per fold and per walk length*/
		updateFoldedPerf(N3,cvClasses[0],labels,inputParams,nbClass,i,foldedPerf);		
		/*Free the temporary classes*/
		clear_classes(cvClasses,nbClass+1);
		
	}

	/*Average performance on folds*/
	averagePerfsOnFolds(foldedPerf,inputParams,nbClass,avgPerf,devPerf);
	/*Find the optimal walk length based on the cross-validated performances*/
	bestLen = findBestWalkLength(inputParams,avgPerf,devPerf);

	/*Display performances for the best length*/
	if(inputParams->verbose >= 1){
		printf("\nOptimal walk length    : %i\n",bestLen);
		show_ConfusionMatrix(avgPerf[bestLen-2].confusion,nbClass,stdout);
		printPerfs(&(avgPerf[bestLen-2]),&(devPerf[bestLen-2]));
	}

	/*Return the best length and associated perf measures*/
	optiLen->len     = bestLen;
	optiLen->avgPerf = getPerf(&(avgPerf[bestLen-2]),inputParams->perfMeasure);
	optiLen->devPerf = getPerf(&(devPerf[bestLen-2]),inputParams->perfMeasure);

	/*Free data structure related to cross-validation*/
	free_PerfMeasures_vec(avgPerf,inputParams->wlen-1,nbClass+1);
	free_PerfMeasures_vec(devPerf,inputParams->wlen-1,nbClass+1);
	if(releaseFolds)free_int_mat(folds,inputParams->nbFolds);
	free_mat3(N3,inputParams->wlen-1,nPerFold);
	free_PerfMeasures_mat(foldedPerf,inputParams->nbFolds,inputParams->wlen-1,nbClass+1);
	free(cvClasses);
}
