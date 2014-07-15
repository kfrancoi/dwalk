/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 17/10/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "similarity.h"

/*Compute a similartiy measure (specified in inputParams) between sparse vectors x and z*/
real similarity(SparseDim* x, SparseDim* z, InputParams* inputParams){
	if(!inputParams->simParams || (int)inputParams->simParams[1] == 1)
		return dotProduct(x,z);
	else{
		switch((int)inputParams->simParams[1]){
			case 2 :
				return invDistance(x,z,inputParams->simParams[2]);
			case 3 :
				return polyKernel(x,z,inputParams->simParams[2],
					                  inputParams->simParams[3],
									  inputParams->simParams[4]);
			case 4 :
				return gaussianKernel(x,z,inputParams->simParams[2]);
		}
	}
	fprintf(stderr,"Similarity error \n");
	exit(EXIT_FAILURE);
}

/*Compute the dot product between sparse vectors x and z*/
real dotProduct(SparseDim* x, SparseDim* z){
	real res = 0.0;
	SparseElem* x_elem = x->first;
	SparseElem* z_elem = z->first;

	while(x_elem && z_elem){
		if(x_elem->c < z_elem->c)
			x_elem = x_elem->nextC;
		else if (x_elem->c > z_elem->c)
			z_elem = z_elem->nextC;
		else{
			res += x_elem->val * z_elem->val;
			x_elem = x_elem->nextC;
			z_elem = z_elem->nextC;
		}
	}
	return res;
}

/*Compute the dot product between sparse vectors x and z*/
/*If square == 1, the square of the L2 is returned*/
real L2Distance(SparseDim* x, SparseDim* z,uchar square){
	real res = 0.0;
	SparseElem* x_elem = x->first;
	SparseElem* z_elem = z->first;

	while(x_elem || z_elem){
		if(x_elem && z_elem){
			if(x_elem->c < z_elem->c){
				res += POW2(x_elem->val);
				x_elem = x_elem->nextC;
			}
			else if (x_elem->c > z_elem->c){
				res += POW2(z_elem->val);		
				z_elem = z_elem->nextC;
			}
			else{
				res += POW2((x_elem->val) - (z_elem->val));
				x_elem = x_elem->nextC;
				z_elem = z_elem->nextC;
			}
		}
		else{
			if(x_elem){
				res += POW2(x_elem->val);
				x_elem = x_elem->nextC;
			}
			else{
				res += POW2(z_elem->val);		
				z_elem = z_elem->nextC;			
			}
		}
	}

	/*if(res == 0.0){
		fprintf(stderr,"Null distance\n");
		if(x_elem == z_elem)
			fprintf(stderr,"Pointers are the same\n");
		fprintf(stderr,"Vector x : \n");
		x_elem = x->first;
		while(x_elem){
			fprintf(stderr," %i:%e",(x_elem->c)+1,x_elem->val);
			x_elem = x_elem->nextC;
		}
		fprintf(stderr,"\n");
		fprintf(stderr,"Vector z : \n");
		z_elem = z->first;
		while(z_elem){
			fprintf(stderr," %i:%e",(z_elem->c)+1,z_elem->val);
			z_elem = z_elem->nextC;
		}
		fprintf(stderr,"\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}*/

	if (square)
		return res;
	else
		return (real)sqrt(res);	
}


/*Compute the polynomial kernel between sparse vectors x and z*/
real polyKernel(SparseDim* x, SparseDim* z,real a, real b,real c){
	return (real)pow(a*dotProduct(x,z)+b,c);
}

/*Compute the polynomial kernel between sparse vectors x and z*/
real gaussianKernel(SparseDim* x, SparseDim* z,real gamma){
	return (real)exp(-gamma*L2Distance(x,z,1));
}

/*Compute the inverse of the distance between x and z*/
real invDistance(SparseDim* x, SparseDim* z,real a){
	if (a == 1.0)
		return (real)(1.0/L2Distance(x,z,0));
	else if(a == 2.0)
		return (real)(1.0/L2Distance(x,z,1));
	else
		return (real)(1.0/pow(L2Distance(x,z,0),a));
}
