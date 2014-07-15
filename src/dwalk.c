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

/*The discriminative random walks 
	- Computing betweenness for unlabeled nodes only
	- Allowing self-return walks
	- Up to length wlen
	- Ultra-fast method !
*/
void dwalk(SparseMatrix* spmat,uchar* absorbing,int** classes,int nbClass,real** N,
		   real** latF,real** latB,int wlen,uchar cross,DWInfo *dwInfo,int verbose){

	/*Working variables */
	int       c,k,l;
	int       cur;
	int      *class_p = NULL;
	real      p_wlen;
	real      accB;
	real      update;

	/*Make some initializations*/
	dwInfo->expWlen  = 0;
	init_vec(dwInfo->mass_abs,nbClass);
	init_uchar_vec(absorbing,spmat->nbRows);

	/*If there are no unlabeled node, exit*/
	if(classes[0][0] == 0){
		if(verbose >= 1)
			printf("There are no unlabeled nodes !\n");
		return;
	}	

	if(verbose >= 1){
		printf("Handeling class              : ");
		fflush(stdout);
	}

	/*Loop on the classes*/
	for(c=1;c<=nbClass;c++){

		if(verbose >= 1){
			if(c > 1) backspace(intLen(c-1)+intLen(nbClass)+1);
			printf("%i/%i",c,nbClass);
			fflush(stdout);
		}

		/*Set all the nodes of class c being absorbing and starting*/
		setAbsStates(absorbing,class_p,classes[c],-1);
		class_p = classes[c]; /*Update the reference to previous class*/		

		/*Build the forward/backward lattices up to length wlen*/
		#pragma omp parallel if(PAR_FW_BW)
		{
			#pragma omp sections
			{
				#pragma omp section
				buildLatticeForward(latF,wlen,spmat,classes[c],-1,absorbing,cross,verbose);
			
				#pragma omp section
				buildLatticeBackward(latB,wlen,spmat,absorbing,cross,verbose);
			}
		}

		/*Compute the cumulated absorption mass */
		p_wlen = cumulatedAbsorptionProba(latB,wlen,classes[c]);
		/*printf("p_wlen %f\n", p_wlen);*/
		dwInfo->mass_abs[c-1] += p_wlen;

		/*Iterate on unlabeled nodes*/
		#pragma omp parallel for if(PAR_UNLABELED) default(none) \
		shared(c,wlen,p_wlen,N,classes,latF,latB,dwInfo) private(k,l,cur,accB,update)
		for(k=1;k<=classes[0][0];k++){
			cur  = classes[0][k];
			accB = 0;
			for(l=wlen-1;l>=1;l--){
				accB  += latB[l][cur];
				update = (latF[l][cur]*accB)/p_wlen;
				N[k-1][c-1] += update;
				dwInfo->expWlen += update;
			}
		}
		if(verbose >= 2) printLineDelim();        	
	}/*End for(classes) */
	
	/*Display a line delimiter on stdout*/
	if(verbose >= 1){
		printf("\n");
	}		
}

/*The discriminative random walks 
	- Computing betweenness for unlabeled nodes only
	- Allowing self-return walks
	- Separately for all lengths up to wlen
	- Ultra-fast method !
*/
void dwalk_all_lengths(SparseMatrix* spmat,uchar* absorbing,int** classes,int nbClass,
					   real*** N3,real** latF,real** latB,int wlen,uchar cross,int verbose){
	    
    /*Working variables */
	int       c,i,k,l,t;
	int       cur;
	int      *class_p = NULL;
	real      accB;
    real      p_wlen;
	
	/*If there are no unlabeled node, exit*/
	if(classes[0][0] == 0){
		if(verbose >= 1)
			printf("There are no unlabeled nodes !\n");
		return;
	}
	
	/*Make some initializations*/
	init_uchar_vec(absorbing,spmat->nbRows);	

	if(verbose >= 1){
		printf("Handeling class              : ");
		fflush(stdout);
	}

	/*Loop on the classes*/
	for(c=1;c<=nbClass;c++){

		if(verbose >= 1){
			if(c > 1) backspace(intLen(c-1)+intLen(nbClass)+1);
			printf("%i/%i",c,nbClass);
			fflush(stdout);
		}	

		/*Set all the nodes of class c being absorbing and starting*/
		setAbsStates(absorbing,class_p,classes[c],-1);
		class_p = classes[c]; /*Update the reference to previous class*/
		/*Build the forward/backward lattices up to length wlen*/
		#pragma omp parallel if(PAR_FW_BW)
		{
			#pragma omp sections
			{
				#pragma omp section
				buildLatticeForward(latF,wlen,spmat,classes[c],-1,absorbing,cross,verbose);

				#pragma omp section
				buildLatticeBackward(latB,wlen,spmat,absorbing,cross,verbose);
			}
		}
		/*Reset the absorption probability*/
		p_wlen = 0;
		for(i=1;i<=classes[c][0];i++)
			p_wlen += (latB[wlen-1][classes[c][i]])/((real)classes[c][0]);

		/*Loop on the walk lengths*/
		for(l=2;l<=wlen;l++){

			/*Update the cumulated absorption probability*/
			for(i=1;i<=classes[c][0];i++)
				p_wlen += (latB[wlen-l][classes[c][i]])/((real)classes[c][0]);

			/*Check that the current walk length has a stricly positive probability*/
			if(p_wlen > 0){

				/*Loop on the unlabeled states*/
				#pragma omp parallel for if(PAR_UNLABELED) default(none) \
				shared(c,l,wlen,p_wlen,N3,classes,latF,latB) private(k,cur,accB,t)
				for(k=1;k<=classes[0][0];k++){
					cur  = classes[0][k];
					accB = 0;

					/*Loop on the time indices*/
					for(t=wlen-1;t>=1+(wlen-l);t--){
						accB += latB[t][cur];
						N3[l-2][k-1][c-1] += (latF[t-(wlen-l)][cur]*accB)/p_wlen;
					}
				}
			}
			if(verbose >= 2) printLineDelim(); 
		}/*end for(walk lengths)*/        
	
	}/*End for(classes) */
	
	if(verbose >= 1){
		printf("\n");
	}	
}