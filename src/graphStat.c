/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 01/08/2008                                              */
/*************************************************************************/

#include "memory.h"
#include "dwalk.h"
#include "textDisplay.h"

/*Compute the minimum walk length required to classify all unlabeled nodes*/
int minCoveringLength(real*** N3,int wlen,int nbUnlabeled,int nbClass){
	int c,i,t;
	int isClassified;

	if(nbUnlabeled == 0)
		return -2;

	/*Iterate on the walk lengths*/
	for(t=0;t<wlen-1;t++){
		/*Iterate on unlabeled nodes*/
		for(i=0;i<nbUnlabeled;i++){
			isClassified = 0;
			/*Iterate on the classes*/
			for(c=0;c<nbClass;c++){
				/*Check if a node has been reached during the walks*/
				if(N3[t][i][c] > 0.0)
					isClassified = 1;
			}
			if(!isClassified) break;
		}
		if (isClassified)
			return t+2; 
	}
	return -1;
}

/*Compute the Minimum Covering Length statistics*/
int computeMCL(SparseMatrix* spmat,uchar* absorbing,char* labels,int** classes,int nbClass,
					  real** latF,real** latB,InputParams* inputParams){

	int     minCovLen;
	int     nbUnlabeled = classes[0][0];
	real*** N3 = NULL;

	if(inputParams->verbose >= 1){
		textcolor(BRIGHT,RED,BLACK);
		printf("Computing the Minimum Covering Length\n");
		textcolor(RESET,WHITE,BLACK);
		printLineDelim();
	}

	/*Initialize some data structure*/	
	N3 = mat3_alloc(inputParams->wlen-1,nbUnlabeled,nbClass);

	/*Initialize the scores*/
	init_mat3(N3,inputParams->wlen-1,nbUnlabeled,nbClass);

	/*Launch the D-walks for all lengths up to inputParams->wlen*/
	dwalk_all_lengths(spmat,absorbing,classes,nbClass,N3,latF,latB,inputParams->wlen,inputParams->crossWalks,inputParams->verbose);	

	/*Compute the Minimum Covering Length*/
	minCovLen = minCoveringLength(N3,inputParams->wlen,nbUnlabeled,nbClass);

	if(inputParams->verbose >= 1){
		if(minCovLen == -1){ 
			printf("The minimum covering length is larger than walkLen.\n");
			printf("Try with a larger walkLen value.\n");
		}
		else if(minCovLen == -2){
			printf("The minimum covering length cannot be computed for fully labeled graphs.\n");
			printf("Provide a partially labeled graph to compute this statistics.\n");
		}
		else{
			printf("Minimum Covering Length         : %i\n",minCovLen);
		}
		printLineDelim();	
	}

	/*Free data structure*/
	free_mat3(N3,inputParams->wlen-1,nbUnlabeled);

	return minCovLen;
}