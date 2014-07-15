/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 08/02/2008                                              */
/*************************************************************************/

#include "perfMeasure.h"
#include "tools.h"
#include "memory.h"

/*Compute several performance measures*/
void computePerfs(PerfMeasures* perfMeasures,int nbClass,real* priors){
	perfMeasures->classificationRate = classificationRate(perfMeasures->confusion,nbClass,priors);
	perfMeasures->macroPrecision     = macroPrecision(perfMeasures->confusion,nbClass);
	perfMeasures->macroRecall        = macroRecall(perfMeasures->confusion,nbClass);
	perfMeasures->macroF1            = macroF1(perfMeasures->confusion,nbClass);
	perfMeasures->microPrecision     = microPrecision(perfMeasures->confusion,nbClass);
	perfMeasures->microRecall        = microRecall(perfMeasures->confusion,nbClass);
	perfMeasures->microF1            = microF1(perfMeasures->confusion,nbClass);
}

/*Extract a performance measure from a PerfMeasures struct using an index*/
real getPerf(PerfMeasures *pm,int perfMeasure)
{
	switch(perfMeasure){
		case PERF_CLASS_RATE :
			return pm->classificationRate;
		case PERF_MACRO_PREC :
			return pm->macroPrecision;
		case PERF_MACRO_REC :
			return pm->macroPrecision;
		case PERF_MACRO_F1 :
			return pm->macroF1;
		case PERF_MICRO_PREC :
			return pm->microPrecision;
		case PERF_MICRO_REC :
			return pm->microRecall;
		case PERF_MICRO_F1 :
			return pm->microF1;
	}
	return 0.0;
}
/*Return the name of a performance measure given its index*/
char* getPerfName(int perfMeasure){
	switch(perfMeasure){
		case PERF_CLASS_RATE :
			return "Classification rate";
		case PERF_MACRO_PREC :
			return "Macro Precision";
		case PERF_MACRO_REC :
			return "Macro Recall";
		case PERF_MACRO_F1 :
			return "Macro F1";
		case PERF_MICRO_PREC :
			return "Micro Precision";
		case PERF_MICRO_REC :
			return "Micro Recall";
		case PERF_MICRO_F1 :
			return "Micro F1";
	}
	return NULL;
}

/*Compute the performance the classification rate based on a confusion matrix*/
real classificationRate(int** confusion,int nbClass,real* priors){
	
	int c,tot=0,nbCorrect=0;
	/*real nbCorrect=0.0;*/
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		nbCorrect += ((real)confusion[c][c])/**priors[c]*/;
		tot += int_vecSum(confusion[c],nbClass+1);
	}
	
	/*Return classification rate*/
	/*When no node where classify -> return 0 instead of -1.$*/
	if ((nbCorrect == 0) & (tot == 0))
		return (float)0;
	else
		return (float)nbCorrect/(float)tot;
}

/*Compute the performance the macro-averaged F1 based on a confusion matrix*/
real macroF1(int** confusion,int nbClass){
	int  i,c;
	real sumRow,sumCol;
	real pi  = 0;
	real rho = 0;
	real F1  = 0;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		/*Compute the sum of row and col c*/
		sumRow = 0;sumCol = 0;
		for(i=0;i<=nbClass;i++){
			sumRow += confusion[c][i];
			sumCol += confusion[i][c];
		}
		pi  = (sumCol > 0) ? (float)confusion[c][c]/sumCol : 0;
		rho = (sumRow > 0) ? (float)confusion[c][c]/sumRow : 0;
		if (pi+rho > 0) F1 += (2*pi*rho)/(pi+rho);
	}
	
	/*Return the macro-averaged F1*/
	return F1/(float)nbClass;
}

/*Compute the performance the micro-averaged F1 based on a confusion matrix*/
real microF1(int** confusion,int nbClass){
	int   i,c;
	int   TP=0,FP=0,FN=0;
	real  pi,rho;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		for(i=0;i<=nbClass;i++){
			if(i == c)
				TP += confusion[c][c];
			else{
				FP += confusion[i][c];
				FN += confusion[c][i];
			}
		}		
	}

	pi  = (TP+FP > 0.0) ? ((float)TP)/((float)(TP+FP)) : (real)0.0;
	rho = (TP+FN > 0.0) ? ((float)TP)/((float)(TP+FN)) : (real)0.0;

	return (pi+rho > 0) ? (2*pi*rho)/(pi+rho) : (real)0.0;
	
}

/*Compute the performance the macro-averaged Precision based on a confusion matrix*/
real macroPrecision(int** confusion,int nbClass){
	int  i,c;
	real sumCol;
	real pi = 0;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		/*Compute the sum of col c*/
		sumCol = 0;
		for(i=0;i<=nbClass;i++){
			sumCol += confusion[i][c];
		}
		pi += (sumCol > 0) ? (float)confusion[c][c]/sumCol : 0;
	}
	
	/*Return the macro-averaged Precision*/
	return pi/(float)nbClass;
}

/*Compute the performance the micro-averaged Precision based on a confusion matrix*/
real microPrecision(int** confusion,int nbClass){
	int  i,c;
	int  TP=0,FP=0;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		for(i=0;i<=nbClass;i++){
			if (i == c)
				TP += confusion[c][c];
			else
				FP += confusion[i][c];
		}
	}
	
	return  (TP+FP > 0.0) ? ((float)TP)/((float)(TP+FP)) : (real)0.0;
}

/*Compute the performance the macro-averaged Recall based on a confusion matrix*/
real macroRecall(int** confusion,int nbClass){
	int  i,c;
	real sumRow;
	real rho = 0;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		/*Compute the sum of row c*/
		sumRow = 0;
		for(i=0;i<=nbClass;i++){
			sumRow += confusion[c][i];
		}
		rho += (sumRow > 0) ? (float)confusion[c][c]/sumRow : 0;
	}
	
	/*Return the macro-averaged Recall*/
	return rho/(float)nbClass;
}

/*Compute the performance the micro Recall based on a confusion matrix*/
real microRecall(int** confusion,int nbClass){
	int  i,c;
	int  TP=0,FN=0;
	
	/*Iterate on the class*/
	for(c=1;c<=nbClass;c++){
		for(i=0;i<=nbClass;i++){
			if (i == c)
				TP += confusion[c][c];
			else
				FN += confusion[c][i];
		}
	}
	
	return (TP+FN > 0.0) ? ((float)TP)/((float)(TP+FN)) : (real)0.0;
}

/*Create a confusion matrix from prediction and targets*/
void createConfusionMatrix(char* preds,char* targets,int nbUnlabeled,int** confusion,int nbClass){

	int  i;
	
	/*(Re-)initialize the confusion matrix*/
	init_int_mat(confusion,nbClass+1,nbClass+1);
	
	/*Iterate on unlabeled nodes*/
	for(i=0;i<nbUnlabeled;i++){		
		/*Update the confusion matrix*/
		confusion[targets[i]][preds[i]]++;
	}
}
