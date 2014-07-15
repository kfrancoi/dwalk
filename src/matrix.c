/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "matrix.h"
#include "defines.h"
#include "textDisplay.h"
#include "similarity.h"

/* Compute the sum of a vector of reals */
real vecSum(real* vec,int m){
    int  i;
    real sum =0.0;
    
    for(i=0;i<m;i++){
        sum += vec[i];
    }
    return sum;
}

/* Compute the sum of a vector of reals*/
int int_vecSum(int* vec,int m){
    int i;
    int sum = 0;
    
    for(i=0;i<m;i++){
        sum += vec[i];
    }
    return sum;    
}

/*Divide all elements of a vector of reals by a number a*/
void vecNormalize(real* vec,int m,real a){    
    int i;
    
    for(i=0;i<m;i++)
        vec[i] /= a;
}     

/*Divide all elements of a sparse matrix by a number a*/
void matNormalize(SparseDim* rows,int m,real a){
    
    int i;
    SparseElem* elem;
      
    /*Normalize the entries*/
    for(i=0;i<m;i++){
        elem = rows[i].first;
        while(elem){
            elem->val /= a;
            elem = elem->nextC;
        }
    }        
}  

/*Compute the position of the maximum element in a vector of reals*/
int max_vec_pos(real* vec,int m){
    int i, maxPos=0;
    real maxVal=vec[0];
    
    for(i=0;i<m;++i){
        if(vec[i] > maxVal){
            maxVal = vec[i];
            maxPos = i;
        }    
    }
    return maxPos;
}

/*Enforce the stochasticity of a sparse matrix of positive real*/
void stochMat(SparseMatrix* spmat){
    int i;
    SparseElem* elem;
    
	for(i=0;i<spmat->nbRows;i++){
        elem = spmat->rows[i].first;
        while(elem){
            elem->val /= spmat->rowSums[i];
            elem = elem->nextC;
        }    
    }
}

/*Enforce the stochasticity of a vector of positive real */
real stochVector(real* vec,int m){
    int i;    
    real sum = vecSum(vec,m);
    
    if (sum>0){
        for(i=0;i<m;++i)
            vec[i] = vec[i]/sum;
    }
    return sum;
}

/* Build a forward lattice up to wlen*/
void buildLatticeForward(real** lat,int wlen,SparseMatrix* spmat,int* startClass,int startState,uchar* absorbing,uchar cross,int verbose){
    
    int  j,k;
	SparseDim   col_j;
    SparseElem* elem;
	real startProba = (real)(1.0/(float)startClass[0]);
    
	if(verbose >= 2){
		printf("\nStart Forward thread %d\n",omp_get_thread_num());
		fflush(stdout);
	}

    /* Clean the first row of the lattice*/
    init_vec(lat[0],spmat->nbRows);

	/*There is a unique starting state*/
	if(startState > 0){
		lat[0][startState] = startProba;
	}
	else{
	    for(j=1;j<=startClass[0];j++)
	    	lat[0][startClass[j]] = startProba;	
	}

	/*There is a significant speed-up for symmetric matrices as the locality in memory  */
	/*is improved. When the graph is loaded, the sparse elem are created by rows.       */
	/*Therefore, element in the same line are contiguous in memory. The forward reccu-  */
	/*rence uses the columns of the matrix, which is slow due to non-locality in memory */
	/*Since, for symmetric matrices rows and columns are the same, one can use the rows */
	/*instead of the columns in the forward recurrence                                  */
	
	if(spmat->symmetric){

		/*Special case for time 1*/
		if(wlen > 0){
			#pragma omp parallel for if(PAR_FW_LAT) default(none) shared(spmat,lat) private(j,col_j,elem)
			for (j=0;j<spmat->nbRows;j++){
				lat[1][j] = 0;
				col_j     = getRow(spmat,j);                    /*Speed-up as col_j = row_j */
				elem      = col_j.first;
				while(elem){
					lat[1][j] += lat[0][elem->c]*(elem->val)/spmat->rowSums[elem->c];
					elem = elem->nextC;
				}    
			}
		}

		/* Propagate probabilities in the lattice */
		for (k=2;k<=wlen;++k){

			#pragma omp parallel for if(PAR_FW_LAT) default(none) shared(k,spmat,lat,absorbing,cross) private(j,col_j,elem)
			for (j=0;j<spmat->nbRows;j++){
				lat[k][j] = 0;
				col_j     = getRow(spmat,j);                   /*Speed-up as col_j = row_j */
				elem      = col_j.first;
				while(elem){
					if(!absorbing[elem->c] || cross)
						lat[k][j] += lat[k-1][elem->c]*(elem->val)/spmat->rowSums[elem->c];
					elem = elem->nextC;
				}    
			}
		}
	}
	/*Asymmetric matrix*/
	else{
		/*Special case for time 1*/
		if(wlen > 0){
			#pragma omp parallel for if(PAR_FW_LAT) default(none) shared(spmat,lat) private(j,col_j,elem)
			for (j=0;j<spmat->nbRows;j++){
				lat[1][j] = 0;
				col_j     = getCol(spmat,j);
				elem      = col_j.first;
				while(elem){
					lat[1][j] += lat[0][elem->l]*(elem->val)/spmat->rowSums[elem->l];
					elem = elem->nextL;
				}    
			}
		}

		/* Propagate probabilities in the lattice */
		for (k=2;k<=wlen;++k){
			if(verbose >= 2 && k%10 == 0){
				printf("...%i",k);
				fflush(stdout);
			}
			#pragma omp parallel for if(PAR_FW_LAT) default(none) shared(k,spmat,lat,absorbing,cross) private(j,col_j,elem)
			for (j=0;j<spmat->nbRows;j++){
				lat[k][j] = 0;
				col_j     = getCol(spmat,j);
				elem      = col_j.first;
				while(elem){
					if(!absorbing[elem->l] || cross)
						lat[k][j] += lat[k-1][elem->l]*(elem->val)/spmat->rowSums[elem->l];
					elem = elem->nextL;
				}    
			}
		}
	}

	if(verbose >= 2){
		printf("End Forward %d\n",omp_get_thread_num());
		fflush(stdout);
	}
}

/* Build a backward lattice up to wlen*/
void buildLatticeBackward(real** lat,int wlen,SparseMatrix* spmat,uchar* absorbing,uchar cross,int verbose){
    
    int i,k;
	SparseDim   row_i;
    SparseElem* elem;
    
	if(verbose >= 2){
		printf("\nStart Backward thread %d\n",omp_get_thread_num());
		fflush(stdout);
	}

    /* Initialization for time wlen */
    for(i=0;i<spmat->nbRows;i++)
    	lat[wlen][i] = absorbing[i];
       
	/*Particular case for time wlen-1 */
	if(wlen > 0){
		#pragma omp parallel for if(PAR_BW_LAT) default(none) shared(wlen,spmat,lat) private(i,row_i,elem)
		for(i=0;i<spmat->nbRows;i++){
			lat[wlen-1][i] = 0;
			row_i = getRow(spmat,i);
			elem  = row_i.first;
			while(elem){
				lat[wlen-1][i] += lat[wlen][elem->c]*(elem->val)/spmat->rowSums[elem->l];
				elem = elem->nextC;
			}    
		}
	}

	/* Propagate probabilities in the lattice */
	for (k=wlen-1;k>=1;--k){               
		#pragma omp parallel for if(PAR_BW_LAT) default(none) shared(k,spmat,lat,absorbing,cross) private(i,row_i,elem)
		for(i=0;i<spmat->nbRows;i++){
			lat[k-1][i] = 0;
			row_i = getRow(spmat,i);
			elem = row_i.first;
			while(elem){
				if(!absorbing[elem->c] || cross)
					lat[k-1][i] += lat[k][elem->c]*(elem->val)/spmat->rowSums[elem->l];
				elem = elem->nextC;
			}    
		}
	}
	
	if(verbose >= 2){
		printf("End Backward thread %d\n",omp_get_thread_num());
		fflush(stdout);
	}
}

/*Compute the cumulated absorption probability for random walks allowing self-return*/
real cumulatedAbsorptionProba(real** latB,int wlen,int* startClass){
	
	int  i,k;
	real mass       = 0.0;
	real startProba = (real)(1.0/((float)startClass[0]));
	
	for (k=wlen-1;k>=0;--k){
		for(i=1;i<=startClass[0];i++)
			mass += startProba*latB[k][startClass[i]];
	}
		
	return mass;	
}

/*Set all the groups being absorbing except the starting group, here no absorption list are created SparseDim*/
void setAbsStates(uchar* absorbing,int* class_p,int* class_c,int start){

	int i;
	
	/*Reset the previous absorbing states*/
	if(class_p){
		for(i=1;i<=class_p[0];i++){
			absorbing[class_p[i]] = 0;
		}
	}

	/*Set all the state in class as absorbing except state start*/
	for(i=1;i<=class_c[0];i++){
		if(class_c[i] != start)
			absorbing[class_c[i]] = 1;                   
	}
}

/*Test if a sparse matrix is symmetric*/
int isSymmetric(SparseMatrix* spmat){
	int i,sym=1;
	SparseElem* elemL;
	SparseElem* elemC;
	SparseDim*  rows = spmat->rows;
	SparseDim*  cols = spmat->cols;

	if(spmat->nbRows != spmat->nbCols)
		return 0;

	for(i=0;sym && i<spmat->nbRows;i++){
		
		elemL = rows[i].first;
		elemC = cols[i].first;        
		/*Scan the two lists in parallel*/
		while(sym && elemL && elemC){                
			if(elemL->c != elemC->l){
				sym = 0;
			}
			else if(elemL->val != elemC->val){
				sym = 0;
			}
			else{
				elemL = elemL->nextC;
				elemC = elemC->nextL;
			}
		}
	}
	spmat->symmetric = sym;
	return sym;
}   

/*Compute the sum of all elements of a SparseMatrix*/
real SparseMatrix_sum(SparseMatrix* spmat){
	int  i;
	real res = 0.0;
	SparseElem* elem = NULL;

	for(i=0;i<spmat->nbRows;i++){
		elem = spmat->rows[i].first;
		while(elem){
			res += elem->val;
			elem = elem->nextC;
		}
	}
	return res;
}

/*Sum two dense matrices of reals a = a + coeff*b*/
void sum_mat(real** a,real** b,real coeff,int dim1,int dim2){
	int i,j;
	for(i=0;i<dim1;i++){
		for(j=0;j<dim2;j++){
			a[i][j] += coeff*b[i][j];
		}
	}
}

/*Sum two dense matrices of integers a = a + b*/
void sum_int_mat(int** a,int** b,int dim1,int dim2){
	int i,j;
	for(i=0;i<dim1;i++){
		for(j=0;j<dim2;j++){
			a[i][j] += b[i][j];
		}
	}
}

/*Normalize the columns of a sparse matrix such that entries are in [0,1]*/
void normalizeColumns(SparseMatrix* spmat,int verbose){
	int  i,j;
	real range,min_val,max_val;
	SparseElem* elem;

	/*Display progression*/
	if(verbose >= 1){
		printf("Normalization      :      ");
		fflush(stdout);
	}	

	/*TODO: parallelize the loop*/
	for(i=0;i<spmat->nbCols;i++){
		/*Display progression*/
		if(verbose >= 1){
			backspace(5);
			printf("%3.i %%",(int)(100*((float)(i+1)/(float)spmat->nbCols)));
			fflush(stdout);
		}
		/*Compute MIN, MAX and RANGE*/
		elem    = spmat->cols[i].first;
		if(!elem) continue; /*The column is empty*/
		min_val = elem->val;
		max_val = elem->val;
		for(j=0;elem;j++){
			if(elem->val > max_val) max_val = elem->val;
			if(elem->val < min_val) min_val = elem->val;
			elem = elem->nextL;
		}
		
		/*Correct min/max if constant value and col is sparse*/
		if((min_val == max_val) && (j < spmat->nbRows)){
			if(min_val < 0) 
				max_val = 0.0;
			else if(min_val > 0)
				min_val = 0.0;
		}
		
		/*Compute the range*/
		if(min_val != max_val)
			range = max_val - min_val;
		else
			range = 1.0;
		
		/*Normalize values*/
		elem = spmat->cols[i].first;
		while(elem){
			elem->val = ((elem->val) - min_val)/range;
			elem = elem->nextL;
		}		
	}
	/*Display progression*/
	if(verbose >= 1){
		printf("\b\b\b\b\b100 %%\n");
		fflush(stdout);
	}
}

void denseMatrixToSpmat(double* adj, int nbRows, int verbose, SparseMatrix* spmat){

	int             from,to;
	int             nbElems     = 0;
	int             sumDeg      = 0;
	real            val         = 0.0;
	SparseElem*     elem        = NULL;
	real*           rowSums     = NULL;
	SparseDim*      rows        = NULL;
	SparseDim*      cols        = NULL;
	
	/*Fetch reference in the sparse matrix*/
	rowSums = spmat->rowSums;
	rows    = spmat->rows;
	cols    = spmat->cols;
	
	/*Display progression*/
	if(verbose >= 1){
		printf("Building the graph :      ");
		fflush(stdout);
	}
	
	/*Iterate on the "from" nodes*/
	for (from=0;from<nbRows;from++){
		nbElems = 0;
		/*Display progression*/
		if(verbose >= 1){
			backspace(5);
			printf("%3.i %%",(int)(100*((float)(from+1)/(float)nbRows)));
			fflush(stdout);
		}
		/*Scan all the "to" nodes*/
		for(to=0;to<nbRows;to++){
			if(to != from){
				val = *(adj+((nbRows)*from)+to);
				/*Add the element in the row "from"*/
				if (!rows[from].last){
					rows[from].first = (SparseElem*)malloc(sizeof(SparseElem));
					rows[from].last  = rows[from].first;
				}
				else{
					rows[from].last->nextC  = (SparseElem*)malloc(sizeof(SparseElem));
					rows[from].last         = rows[from].last->nextC;
				}
				/*Put the value in the element*/
				rows[from].last->l     = from;
				rows[from].last->c     = to;
				rows[from].last->val   = val;
				rows[from].last->nextL = NULL;
				rows[from].last->nextC = NULL;
				rowSums[from]         += val;
				nbElems++;

				/*Add the element in the column "to"*/
				if(!cols[to].last){
					cols[to].first  = rows[from].last;
					cols[to].last   = rows[from].last;
				}
				else{
					cols[to].last->nextL = rows[from].last;
					cols[to].last = rows[from].last;
				}
				/*Update the number of non-null elements*/
				spmat->nnz += 1;
			}
		}
		/*Update node degree statisitics*/
		if(nbElems > spmat->maxOutDeg)
			spmat->maxOutDeg = nbElems;
		if(spmat->minOutDeg == 0 || nbElems < spmat->minOutDeg)
			spmat->minOutDeg = nbElems;                
		sumDeg += nbElems; 
	}

	/*Display progression*/
	if(verbose >= 1){
		printf("\b\b\b\b\b100 %%\n");
		fflush(stdout);
	}
}

