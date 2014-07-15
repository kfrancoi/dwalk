/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "textDisplay.h"
#include "defines.h"
#include "memory.h"

/*Change the terminal color and styles*/
#ifdef ENABLE_TEXT_COLOR

void textcolor(int attr, int fg, int bg)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

#else

void textcolor(int attr, int fg, int bg){}

#endif

/* Display a vector of integers */
void show_int_vec(int* vec,int n){
    int i;
    for(i=0;i<n;++i){
        printf("%i ",vec[i]);
    }
    printf("\n");
}

/* Display a vector of reals */
void show_vec(real* vec,int n){
    int i;
    for(i=0;i<n;++i){
        printf("%e ",vec[i]);
    }
    printf("\n");
}

/* Display a matrix of reals */
void show_int_mat(int** mat,int l,int c){
    int i,j;
    for(i=0;i<l;++i){
        for(j=0;j<c;++j){
            printf("%i ",mat[i][j]);
        }
        printf("\n");
    }
}

/* Display a matrix of reals */
void show_mat(real** mat,int l,int c){
    int i,j;
    for(i=0;i<l;++i){
		printf("%2i | ",i);
        for(j=0;j<c;++j){
            printf("%e ",mat[i][j]);
        }
        printf("\n");
    }
}

/* Display an array of SparseDim */
void showDims(SparseDim* sdim,int m,char mode){
    int i;
    SparseElem* elem;
    printf("\n");
    for (i=0;i<m;i++){
		printf("%i | ",i+1);
        elem = sdim[i].first;
        while(elem){
            if(mode == 'l'){
                printf("%i:%e ",(elem->c)+1,elem->val);
                elem = elem->nextC;                
                }
            else if(mode == 'c'){
                printf("%i:%e ",(elem->l)+1,elem->val);
                elem = elem->nextL;
                }
        }
        printf("\n");
    }
}

/*Display the content of the classes*/
void show_Classes(int** classes,int nbClass){
	int c;
	for(c=0;c<=nbClass;c++){
		printf("Class %i : %i nodes\n",c,classes[c][0]);
		show_int_vec(classes[c]+1,classes[c][0]);
	}
}

/*Create a confusion matrix from prediction and targets*/
void show_ConfusionMatrix(int** confusion,int nbClass,FILE* output){
	
	int  c,l,i,offset,maxOffDiag;
	int* offsets = int_vec_alloc(nbClass+1);

	fprintf(output,"\n%s%sMODEL",SPACER,TAB);
	offset = strlen(SPACER) + strlen(TAB) + 5;
	for(c=0;c<=nbClass;c++){
		fprintf(output,"%s",TAB);
		offset += strlen(TAB);
		offsets[c] = offset;
		fprintf(output,"%i",c);
		offset += intLen(c);
	}
	fprintf(output,"\n%sTEST\n",SPACER);
	fprintf(output,"%s",SPACER);
	for(i=1;i<=(int)(offset-strlen(SPACER))+3;i++)
		fprintf(output,"-");
	fprintf(output,"\n");

	for(l=0;l<=nbClass;l++){
		/*Compute the maximum off-diagonal element*/
		maxOffDiag = -1;
		for(c=0;c<=nbClass;c++){
			if((l != c) && (confusion[l][c] > 0) &&(confusion[l][c] > maxOffDiag))
				maxOffDiag = confusion[l][c];
		}
		fprintf(output,"%s%i",SPACER,l);
		offset = strlen(SPACER) + intLen(l);
		for(c=0;c<=nbClass;c++){
			/*Color formatting*/
			for(i=1;i<=offsets[c]-offset;i++)
				fprintf(output," ",i);
			if(l == c)
				textcolor(BRIGHT,GREEN,BLACK);
			else if(confusion[l][c] == maxOffDiag)
				textcolor(BRIGHT,RED,BLACK);
			/*Print the matrix value*/
			fprintf(output,"%i",confusion[l][c]);
			/*Color deformatting*/
			if(l == c)
				textcolor(RESET,WHITE,BLACK);
			else if(confusion[l][c] == maxOffDiag)
				textcolor(RESET,WHITE,BLACK);
			offset += (offsets[c]-offset)+ intLen(confusion[l][c]);
		}
		fprintf(output,"\n");
	}
	fprintf(output,"\n");
	free(offsets);
}

/*Print a delimiting line on standard output*/
void printLineDelim(){
	printf("%s\n",TRAILING_BAR);
}

/*Print k backspaces on standard output*/
void backspace(int k){
	int i;
	for(i=1;i<=k;i++)
		printf("\b");
}

/*Return the length of an integer while transformed to a string*/
int intLen(int k){
	char line[BUF_LINE_LEN];
	sprintf(line,"%i",k);
	return strlen(line);
}

/*Print information about the input of the program (i.e. data + parameters)*/
void printInputInfo(SparseMatrix* spmat,DataStat* dataStat,int nbClass,InputParams* inputParams){
	int i;
	int nbNodes    = spmat->nbRows;
	int nbEdges    = spmat->nnz;
	int nbLabeled  = nbNodes - dataStat->classCount[0];
	int undirected = spmat->symmetric;

    printf("Number of nodes    : %i\n",nbNodes);
    printf("Number of edges    : %i\n",nbEdges);
	printf("Matrix density     : %e\n",dataStat->density);
    printf("Directed           : ");
	if(!undirected){
		textcolor(BRIGHT,GREEN,BLACK);
		printf("yes\n");
		textcolor(RESET,WHITE,BLACK);
	}
	else{
		textcolor(BRIGHT,RED,BLACK);
		printf("no\n");
		textcolor(RESET,WHITE,BLACK);
	}
    printf("Mean degree        : %2.2f\n",spmat->meanOutDeg);
    printf("Min  degree        : %i\n",spmat->minOutDeg);
    printf("Max  degree        : %i\n",spmat->maxOutDeg);
	printf("Number of classes  : %i\n",nbClass);
	printf("Prop. of auxiliary : %2.2f %%\n",100*((float)dataStat->nbAuxNodes/(float)nbNodes));
	printf("Prop. of unlabeled : %2.2f %%\n",100*dataStat->classProp[0]);
	for(i=1;i<=nbClass;i++){
		printf("Prop. in class %3i : %2.2f %%\t prior : %2.2f %%\n",i,100*dataStat->classProp[i],100*((float)dataStat->classCount[i]/(float)nbLabeled));	
	}
	printf("CV tuning measure  : %s\n",getPerfName(inputParams->perfMeasure));
	printf("Crossing walks     : ");
	if(inputParams->crossWalks){
		textcolor(BRIGHT,GREEN,BLACK);
		printf("enabled\n");
		textcolor(RESET,WHITE,BLACK);
	}
	else{
		textcolor(BRIGHT,RED,BLACK);
		printf("disabled\n");
		textcolor(RESET,WHITE,BLACK);
	}
	printLineDelim();
}

/*Print information about the input of the program (i.e. data + parameters)*/
void printLSVMInputInfo(SparseMatrix* spmat,LSVMDataInfo* dataInfo,DataStat* dataStat,InputParams* inputParams){
	int i;
	int nbNodes    = spmat->nbRows;
	int nbLabeled  = nbNodes - dataStat->classCount[0];

    printf("Number of lines    : %i\n",dataInfo->nbLines);
    printf("Number of features : %i\n",dataInfo->nbFeatures);
	printf("Number of classes  : %i\n",dataInfo->nbClass);
	printf("Dataset density    : %1.2f %%\n",100*dataStat->density);
	printf("Prop. of unlabeled : %2.2f %%\n",100*dataStat->classProp[0]);
	for(i=1;i<=dataInfo->nbClass;i++){
		printf("Prop. in class %3i : %2.2f %%\t prior : %2.2f %%\n",i,100*dataStat->classProp[i],100*((float)dataStat->classCount[i]/(float)nbLabeled));	
	}
	printf("CV tuning measure  : %s\n",getPerfName(inputParams->perfMeasure));
	printf("Crossing walks     : ");
	if(inputParams->crossWalks){
		textcolor(BRIGHT,GREEN,BLACK);
		printf("enabled\n");
		textcolor(RESET,WHITE,BLACK);
	}
	else{
		textcolor(BRIGHT,RED,BLACK);
		printf("disabled\n");
		textcolor(RESET,WHITE,BLACK);
	}
	printLineDelim();
}


/*Print performance measures*/
void printPerfs(PerfMeasures* avgPerf,PerfMeasures* devPerf){
	if(devPerf != NULL){
		printf("Classification rate    : %2.1f +/- %2.1f %%\n",100*avgPerf->classificationRate,100*devPerf->classificationRate);
		printf("Macro/Micro Precision  : %2.1f +/- %2.1f %% / %2.1f +/- %2.1f %% \n",100*avgPerf->macroPrecision,100*devPerf->macroPrecision,
			                                                                         100*avgPerf->microPrecision,100*devPerf->microPrecision);
		printf("Macro/Micro Recall     : %2.1f +/- %2.1f %% / %2.1f +/- %2.1f %% \n",100*avgPerf->macroRecall,100*devPerf->macroRecall,
			                                                                         100*avgPerf->microRecall,100*devPerf->microRecall);
		printf("Macro/Micro F1         : %2.1f +/- %2.1f %% / %2.1f +/- %2.1f %% \n",100*avgPerf->macroF1,100*devPerf->macroF1,
			                                                                         100*avgPerf->microF1,100*devPerf->microF1);
	}
	else{
		printf("Classification rate    : %2.1f %%\n",100*avgPerf->classificationRate);
		printf("Macro/Micro Precision  : %2.1f %% / %2.1f %% \n",100*avgPerf->macroPrecision,100*avgPerf->microPrecision);
		printf("Macro/Micro Recall     : %2.1f %% / %2.1f %% \n",100*avgPerf->macroRecall,100*avgPerf->microRecall);
		printf("Macro/Micro F1         : %2.1f %% / %2.1f %% \n",100*avgPerf->macroF1,100*avgPerf->microF1);	
	}
	printLineDelim();	
}