/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 29/01/2008                                              */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "tools.h"
#include "memory.h"
#include "files.h"
#include "textDisplay.h"
#include "similarity.h"

/*Tokenize a list of filenames separated by commas*/
int tokenizeFnames(char* list,char** fnames){
    
    int   nbFiles = 0;
    char* fname   = (char*)strtok(list,",");
    
    while (fname != NULL){
        fnames[nbFiles++] = fname;
        fname = (char*)strtok(NULL,",");
    }
    return nbFiles;
}

/*Add an extension to a file name*/
void addExtension(char* fname,char* extension,char* target){
    if(sprintf(target,"%s.%s",fname,extension) >= FNAME_LEN){
        fprintf(stderr,"Argument outFile is too long\n");
        exit(EXIT_FAILURE);
    }       
}    

/*Add a class and an extension to a file name*/
void addClassAndExtension(char* fname,int classNum,char* extension,char* target){
    if(sprintf(target,"%s_class%i.%s",fname,classNum,extension) >= FNAME_LEN){
        fprintf(stderr,"Argument outFile is too long\n");
        exit(EXIT_FAILURE);
    }       
}


/*Count the number of tokens in a string separated by sep*/
int getNbTokens(char* list,const char *sep){
	
    int    nbTokens=0;
    char  *elem;
    
    /*Tokenize on the separator*/
    elem = (char*)strtok(list,sep);
	while(elem){
		nbTokens++;
		elem = (char*)strtok(NULL,sep);
	}
	return nbTokens;
}

/*Tokenize a string of real tokens separated by sep*/
void tokenizeInts(char* list,const char *sep,int* tokens){
 
    int    nbTokens=0;
    char  *elem;
    
    /*Tokenize on the separator */
    elem = (char*)strtok(list,sep);
	while(elem){
		tokens[++nbTokens] = atoi(elem);
		elem = (char*)strtok(NULL,sep);
	}
	tokens[0] = nbTokens;
}

/*Tokenize a string of real tokens separated by sep*/
void tokenizeReals(char* list,const char *sep,real* tokens){
 
    int    nbTokens=0;
    char  *elem;
    
    /*Tokenize on the separator */
    elem = (char*)strtok(list,sep);
	while(elem){
		tokens[++nbTokens] = (real)atof(elem);
		elem = (char*)strtok(NULL,sep);
	}
	tokens[0] = (real)nbTokens;
}

/*Get the indices of the nodes in each class */
void getClasses(char* labels,int m,int** classes,int nbClass){
	
	int i;
	int *classCount = int_vec_alloc(nbClass+1);

	/*Count the number of nodes in each class*/
	init_int_vec(classCount,nbClass+1);
	for(i=0;i<m;i++){
		if(labels[i] >= 0)
			classCount[labels[i]]++;
	}
		
	/*Allocate the class data structure*/
	for(i=0;i<=nbClass;i++){
		classes[i] = int_vec_alloc(classCount[i]+1);
		classes[i][0] = 0;
	}
	
	/*Populate the classes*/
	for(i=0;i<m;i++){
		if(labels[i] >= 0)
			classes[labels[i]][++classes[labels[i]][0]] = i;
	}
	/*Release memory */
	free(classCount);
}

/*Get the indices of the nodes in each class */
void getClassesInt(int* labels,int m,int** classes,int nbClass, int* classCount){
	
	int i;
	
	/*Count the number of nodes in each class*/
	init_int_vec(classCount,nbClass+1);
	for(i=0;i<m;i++){
		if(labels[i] >= 0)
			classCount[labels[i]]++;
	}
		
	/*Allocate the class data structure*/
	for(i=0;i<=nbClass;i++){
		classes[i] = int_vec_alloc(classCount[i]+1);
		classes[i][0] = 0;
	}
	
	/*Populate the classes*/
	for(i=0;i<m;i++){
		if(labels[i] >= 0)
			classes[labels[i]][++classes[labels[i]][0]] = i;
	}
}

/*Compute the argmax on the classes*/
void computePredictions(real** N,char* preds,real* priors,int m,int nbClass){
	int  i,c,bestClass;
	real bestScore;
	real curScore;
	
	/*Iterate on nodes*/
	for(i=0;i<m;i++){
		bestClass = 0;
		bestScore = 0.0;
		/*Iterate on classes*/
		for(c=1;c<=nbClass;c++){
			curScore = N[i][c-1]*priors[c];
			if(curScore > bestScore){
				bestClass = c;
				bestScore = curScore;
			}	
		}
		preds[i] = bestClass;
	}
}

/*Compute the performance for unlabeled nodes provided their target values*/
void computeUnlabeledPerf(char* preds,int* unlabeled,int nbUnlabeled,int nbClass,
						  InputParams* inputParams,PerfMeasures* unlabeledPerf,int verbose){
	/*Allocate the array containing target values*/
	char* targets  = char_vec_alloc(nbUnlabeled);
	/*Read the targets from input file*/
	int nbTarClass = readTargets(inputParams->tarFname,unlabeled,targets);
	/*Initialize performance measures*/
	init_PerfMeasures(unlabeledPerf,nbClass+1);
	/*Check that no new classes are present in the target file*/
	if(nbTarClass > nbClass){
		textcolor(BRIGHT,RED,BLACK);
        printf("WARNING: The number of classes in targetFile (%i) is larger than in graphFile (%i)\n",nbTarClass,nbClass);
		textcolor(RESET,WHITE,BLACK);
		nbClass = nbTarClass;
	}
	/*Allocate the confusion matrix*/
	unlabeledPerf->confusion = int_mat_alloc(nbClass+1,nbClass+1);
	/*Create the confusion matrix*/
	createConfusionMatrix(preds,targets,nbUnlabeled,unlabeledPerf->confusion,nbClass);
	/*Get the performance measure on unlabeled nodes*/
	computePerfs(unlabeledPerf,nbClass,inputParams->priors);
	/*Display the test performance*/
	if(inputParams->verbose >= 1){
		show_ConfusionMatrix(unlabeledPerf->confusion,nbClass,stdout);
		printPerfs(unlabeledPerf,NULL);
	}
	/*Release memory*/
	free(targets);
}

/*Insert a new element in a linked list*/
void insertInList(LinkedList* list, ListElem* lelem){
	if(!list->first)
		list->first = lelem;
	else
		list->last->next = lelem;
	list->last = lelem;
	list->nbElem++;
}

/*Random integers without duplicates*/
int randWithMemory(RandMemory* memory){
	int pos,save,res;
	/*Too many requests*/
	if(memory->cursor == memory->n)
		return -1;
	/*Last requests*/
	else if(memory->cursor == memory->n-1){
		res = memory->partition[(memory->cursor)];
		memory->picked[(memory->cursor)] = memory->n-1;
		(memory->cursor)++;
		return res;
	}
	else{
		/*Do the swap*/
		pos  = memory->cursor + rand()%(memory->n - memory->cursor);
		save = memory->partition[memory->cursor];
		memory->partition[memory->cursor] = memory->partition[pos];
		memory->partition[pos] = save;
		res = memory->partition[memory->cursor];
		memory->picked[(memory->cursor)] = pos;
		(memory->cursor)++;
		return res;
	}
}

/*Delete a position so that it can not be randomly picked*/
void randDelete(RandMemory* memory,int num){
	int save;
	if(memory->partition[num] != num){
		fprintf(stderr,"randDelete() problem: deletions must be performed in increasing order\n");
		exit(EXIT_FAILURE);
	}
	save = memory->partition[memory->cursor];
	memory->partition[memory->cursor] = memory->partition[num];
	memory->partition[num] = save;
	memory->picked[memory->cursor] = num;
	memory->cursor++;
}

/*Reset the memory of the generator*/
void randReset(RandMemory* memory){
	int save;

	while(memory->cursor){
		memory->cursor--; 
		save = memory->partition[memory->cursor];
		memory->partition[memory->cursor] = memory->partition[memory->picked[memory->cursor]];
		memory->partition[memory->picked[memory->cursor]] = save;
	}
}

void printSparseMatrix(SparseMatrix* spmat, int size){
	/* Working variables */
	int				i;
	int				count=0;
	SparseDim		row_i;
	SparseElem*		elem;
	if ((size == 0) | (size >= spmat->nbRows))
		size = spmat->nbRows;
	printf("Matrix : \n");
	for (i=0;i<=size;i++){
		row_i = getRow(spmat,i);
		elem = row_i.first;
		count = 0;
		while(elem){
			printf("(%i, %i, %f) ", elem->l, elem->c, elem->val);
			/*if (elem->c > size)
				elem=NULL;
			else */elem = elem->nextC;
			if (count >= 7) break;
			count++;
		}
		printf("\n");
	}
}

/*Build a kernel matrix based on a LIBSVM feature sparse matrix*/
void buildKernel(SparseMatrix* dataset, real** kernel, InputParams* inputParams){

	int             from,to;
	int				nbElems;
	real			val;

	nbElems = 0;
	for(from=0;from<=dataset->nbRows;from++){
		/*Display progression*/
		if(inputParams->verbose >= 1){
			backspace(5);
			printf("%3.i %%",(int)(100*((float)(from+1)/(float)dataset->nbRows)));
			fflush(stdout);
		}
		for(to=0;to<=dataset->nbRows;to++){
			if(to != from){
				val = similarity(&(dataset->rows[from]),&(dataset->rows[to]),inputParams);
				kernel[from][to] = val;
				nbElems++;
			}
		}
	}
	/*Display progression*/
	if(inputParams->verbose >= 1){
		printf("\b\b\b\b\b100 %%\n");
		fflush(stdout);
	}
}

/*Build a kernel matrix based on a LIBSVM feature sparse matrix*/
void buildKernel2(SparseMatrix* spmat, SparseMatrix* dataset, real alpha, InputParams* inputParams){

	int             from;
	int				nbElems;
	SparseElem*		elemL;

	printf("Multiplying Sparse Matrix :      ");

	for(from=0;from<=spmat->nbRows;from++){
		/*Display progression*/
		
		if(inputParams->verbose >= 1){
			backspace(5);
			printf("%3.i %%",(int)(100*((float)(from+1)/(float)dataset->nbRows)));
			fflush(stdout);
		}
		elemL = spmat->rows[from].first;
		while(elemL){
			elemL->val = alpha * elemL->val * similarity(&(dataset->rows[elemL->l]),&(dataset->rows[elemL->c]),inputParams);
			/*elemL->val = (alpha * elemL->val) + ((1-alpha) * similarity(&(dataset->rows[elemL->l]),&(dataset->rows[elemL->c]),inputParams));*/
			elemL = elemL->nextC;
		}
	}
	/*Display progression*/
	if(inputParams->verbose >= 1){
		printf("\b\b\b\b\b100 %%\n");
		fflush(stdout);
	}
}
