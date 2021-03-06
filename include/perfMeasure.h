/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 08/02/2008                                              */
/*************************************************************************/

#ifndef PERFMEASURE_H
#define PERFMEASURE_H

#include "defines.h"

typedef struct PerfMeasures{
	real   classificationRate;
	real   macroPrecision;
	real   macroRecall;
	real   macroF1;
	real   microPrecision;
	real   microRecall;
	real   microF1;
	int**  confusion;
}PerfMeasures;

/*Compute several performance measures*/
void computePerfs(PerfMeasures* perfMeasures,int nbClass,real* priors);

/*Extract a performance measure from a PerfMeasures struct using an index*/
real getPerf(PerfMeasures *pm,int perfMeasure);

/*Return the name of a performance measure given its index*/
char* getPerfName(int perfMeasure);

/*Compute the performance based on a confusion matrix*/
real classificationRate(int** confusion,int nbClass,real* priors);

/*Compute the performance the macro-averaged F1 based on a confusion matrix*/
real macroF1(int** confusion,int nbClass);

/*Compute the performance the micro-averaged F1 based on a confusion matrix*/
real microF1(int** confusion,int nbClass);

/*Compute the performance the macro-averaged Precision based on a confusion matrix*/
real macroPrecision(int** confusion,int nbClass);

/*Compute the performance the micro-averaged Precision based on a confusion matrix*/
real microPrecision(int** confusion,int nbClass);

/*Compute the performance the macro-averaged F1 based on a confusion matrix*/
real macroRecall(int** confusion,int nbClass);

/*Compute the performance the micro Recall based on a confusion matrix*/
real microRecall(int** confusion,int nbClass);

/*Create a confusion matrix from prediction and targets*/
void createConfusionMatrix(char* preds,char* targets,int nbUnlabeled,int** confusion,int nbClass);

#endif
