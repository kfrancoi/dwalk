/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

/* Allocate a vector of reals */
real *vec_alloc(int n)
{
    real *v;
    v=(real *) calloc(n,sizeof(real));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a vector of integers */
int *int_vec_alloc(int n)
{
    int *v;
    v=(int *) calloc(n,sizeof(int));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a vector of integers */
char *char_vec_alloc(int n)
{
    char *v;
    v=(char *) calloc(n,sizeof(char));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a vector of integers */
uchar *uchar_vec_alloc(int n)
{
    uchar *v;
    v=(uchar *) calloc(n,sizeof(uchar));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a vector of PerfMeasures structs */
PerfMeasures* PerfMeasures_vec_alloc(int n,int nbClass)
{
	int i;
    PerfMeasures *v;
    v=(PerfMeasures *) calloc(n,sizeof(PerfMeasures));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
	for(i=0;i<n;i++){
		v[i].confusion = int_mat_alloc(nbClass,nbClass);
        if(v[i].confusion == NULL) {
            fprintf(stderr,"could not allocate memory");
            exit(EXIT_FAILURE);
        }
	}
    return v;
}

/* Allocate a vector of Similarity structs */
Similarity *Similarity_vec_alloc(int n)
{
    Similarity *v;
    v=(Similarity *) calloc(n,sizeof(Similarity));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a matrix of reals */
real **mat_alloc(int n, int k)
{
    int i;
    real **mat;
    mat=(real **) calloc(n,sizeof(real *));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<n; i++) {
        mat[i]=(real *) calloc(k,sizeof(real));
        if(mat[i] == NULL) {
            fprintf(stderr,"could not allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    return mat;
}

/* Allocate a matrix of integers */
int **int_mat_alloc(int n, int k)
{
    int i, **mat;
    mat=(int **) calloc(n,sizeof(int *));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<n; i++) {
        mat[i]=(int *) calloc(k,sizeof(int));
        if(mat[i] == NULL) {
            fprintf(stderr,"could not allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    return mat;
}

/* Allocate a matrix of char */
char **char_mat_alloc(int n,int k){
    char **mat;
    mat=(char **) calloc(n,sizeof(char *));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return mat;    
}


/* Allocate a matrix of PerfMeasures */
PerfMeasures **PerfMeasures_mat_alloc(int n,int k,int nbClass){
    int i,j;
	PerfMeasures **mat;
    mat=(PerfMeasures **) calloc(n,sizeof(PerfMeasures *));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<n; i++){
        mat[i]=(PerfMeasures *) calloc(k,sizeof(PerfMeasures));
        if(mat[i] == NULL) {
            fprintf(stderr,"could not allocate memory");
            exit(EXIT_FAILURE);
        }
		for(j=0; j<k; j++){
			mat[i][j].confusion = int_mat_alloc(nbClass,nbClass);
			if(mat[i][j].confusion == NULL) {
				fprintf(stderr,"could not allocate memory");
				exit(EXIT_FAILURE);
			}
		}
    }
    return mat;
}

/* Allocate a matrix of Similarity structs */
Similarity **Similarity_mat_alloc(int n,int k){
    Similarity **mat;
    mat=(Similarity **) calloc(n,sizeof(Similarity *));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return mat;    
}

/* Allocate a sparse matrix of reals */
SparseMatrix *spmat_alloc(int n, int k,int allocRowSums){
	SparseMatrix* spmat = (SparseMatrix*)malloc(sizeof(SparseMatrix));
	spmat->rows    = (SparseDim*)malloc(sizeof(SparseDim)*n);
    spmat->cols    = (SparseDim*)malloc(sizeof(SparseDim)*k);
	if(allocRowSums) spmat->rowSums = vec_alloc(n);
	else spmat->rowSums = NULL;
	spmat->nbRows  = n;
	spmat->nbCols  = k;
	return spmat;
}

/* Allocate a tridimensional array of reals */
real ***mat3_alloc(int n, int k, int v)
{
  int i,j;
  real ***mat;
  mat=(real ***) calloc(n,sizeof(real **));
  if(mat == NULL) {
    fprintf(stderr,"could not allocate memory");
    exit(EXIT_FAILURE);
  }
  for(i=0; i<n; i++) {
    mat[i]=(real **) calloc(k,sizeof(real *));
    if(mat[i] == NULL) {
      fprintf(stderr,"could not allocate memory");
      exit(EXIT_FAILURE);
    }
    for(j=0; j<k; j++) {
      mat[i][j]=(real *) calloc(v,sizeof(real));
      if(mat[i][j] == NULL) {
	fprintf(stderr,"could not allocate memory");
	exit(EXIT_FAILURE);
      }
    }
  }
  return mat;
}

/* Allocate a tridimensional array of integers */
int ***int_mat3_alloc(int n, int k, int v)
{
    int i,j;
    int ***mat;
    mat=(int ***) calloc(n,sizeof(int **));
    if(mat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<n; i++) {
        mat[i]=(int **) calloc(k,sizeof(int *));
        if(mat[i] == NULL) {
            fprintf(stderr,"could not allocate memory");
            exit(EXIT_FAILURE);
        }
        for(j=0; j<k; j++) {
            mat[i][j]=(int *) calloc(v,sizeof(int));
            if(mat[i][j] == NULL) {
                fprintf(stderr,"could not allocate memory");
                exit(EXIT_FAILURE);
            }
        }
    }
    return mat;
}

/* Allocate a PerfMeasure struct (including the contained confusion matrix)*/
PerfMeasures* PerfMeasures_alloc(int nbClass){
    PerfMeasures *v;
    v=(PerfMeasures *) malloc(sizeof(PerfMeasures));
    if(v==NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
	v->confusion = int_mat_alloc(nbClass,nbClass);
    if(v->confusion == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return v;
}

/* Allocate a DataStat struct (including the class count & prop)*/
DataStat* DataStat_alloc(int nbClass){
	DataStat* dataStat = (DataStat*) malloc(sizeof(DataStat));
    if(dataStat == NULL) {
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
    }
	dataStat->classCount = int_vec_alloc(nbClass);
	dataStat->classProp  = vec_alloc(nbClass);
	return dataStat;
}

/*Allocate a RandMemory struct (including the partition)*/
RandMemory* RandMemory_alloc(int n){
	RandMemory* memory = (RandMemory*) malloc(sizeof(RandMemory));
	if(memory == NULL){
        fprintf(stderr,"could not allocate memory");
        exit(EXIT_FAILURE);
	}
	memory->partition = int_vec_alloc(n);
	memory->picked = int_vec_alloc(n);
	memory->n = n;
	return memory;
}

/*Free a vector of PerfMeasures struct*/
void free_PerfMeasures_vec(PerfMeasures* vec,int dim1,int nbClass){
	int i;
	for (i=0;i<dim1;i++)
		free_int_mat(vec[i].confusion,nbClass);
	free(vec);
}

/* Deallocate a matrix of reals*/
void free_mat(real **matrix, int dim1)
{
    int i;
    
    for (i=0; i<dim1; i++)
        free(matrix[i]);
    free(matrix);
}

/* Deallocate a matrix of integers */
void free_int_mat(int **matrix, int dim1)
{
    int i;
    
    for (i=dim1-1; i>=0; i--)
        free(matrix[i]);
    free(matrix);
}

/* Deallocate a matrix of PerfMeasures*/
void free_PerfMeasures_mat(PerfMeasures **matrix, int dim1, int dim2,int nbClass)
{
    int i,j;
    
    for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++)
			free_int_mat(matrix[i][j].confusion,nbClass);
		free(matrix[i]);
	}
    free(matrix);
}

/* Deallocate a matrix of Similarity structs */
void free_Similarity_mat(int **matrix, int dim1)
{
    int i;
    
    for (i=dim1-1; i>=0; i--)
        free(matrix[i]);
    free(matrix);
}

/* Deallocate a tridimensional array of reals */
void free_mat3(real ***matrix, int dim1, int dim2)
{
  int i;

  for (i=0; i<dim1; i++) 
    free_mat(matrix[i],dim2);
  free(matrix);
}

/* Deallocate a tridimensional array of integers */
void free_int_mat3(int ***matrix, int dim1, int dim2)
{
  int i;

  for (i=0; i<dim1; i++) 
    free_int_mat(matrix[i],dim2);
  free(matrix);
}

/*Free a SparseMatrix*/
void free_SparseMatrix(SparseMatrix* spmat){
	int i;
	/*Free SparseElem inside each row*/
	for(i=0;i<spmat->nbRows;i++)
		clear_SparseRow(spmat->rows[i]);
	free(spmat->rows);
	free(spmat->cols);
	free(spmat);
}

/*Free a PerfMeasures struct*/
void free_PerfMeasures(PerfMeasures* perfMeasures, int nbClass){
	if (nbClass > 0 && perfMeasures->confusion)
		free_int_mat(perfMeasures->confusion,nbClass);
	free(perfMeasures);
}

/*Clear an InputParams struct*/
void free_InputParams(InputParams* inputParams){
	if(inputParams->dataFname)  free(inputParams->dataFname);
    if(inputParams->graphFname) free(inputParams->graphFname);
	if(inputParams->gramFname)  free(inputParams->gramFname);
    if(inputParams->outFname)   free(inputParams->outFname);
	if(inputParams->tarFname)   free(inputParams->tarFname);
	if(inputParams->outPRED)    free(inputParams->outPRED);
	if(inputParams->outLEN)     free(inputParams->outLEN);
	if(inputParams->priors)     free(inputParams->priors);
	if(inputParams->simParams)  free(inputParams->simParams);
	free(inputParams);
}

/*Clear a DataStat struct*/
void free_DataStat(DataStat* dataStat){
	if(dataStat->classCount) free(dataStat->classCount);
	if(dataStat->classProp)  free(dataStat->classProp);	
	free(dataStat);
}

/*Clear a DWInfo struct*/
void free_DWInfo(DWInfo* dwInfo){
	if(dwInfo->mass_abs) free(dwInfo->mass_abs);
	free(dwInfo);
}

/*Free memory for each class*/
void free_classes(int** classes,int nbClass){
	
	int i;
	for(i=0;i<nbClass;i++)
		free(classes[i]);
	free(classes);
}

/*Free a RandMemory struct*/
void free_RandMemory(RandMemory* memory){
	if(memory){
		if(memory->partition) free(memory->partition);
		free(memory);
	}
}

/*Release memory for each class*/
void clear_classes(int** classes,int nbClass){
	
	int i;
	for(i=0;i<nbClass;i++)
		free(classes[i]);
}

/* Free a SparseDim horizontally*/
void clear_SparseRow(SparseDim spdim){
	SparseElem* cur;
	SparseElem* next;
	if((cur = spdim.first)){
		while(cur){
			next = cur->nextC;
			free(cur);
			cur  = next;
		}
	}
	spdim.first = NULL;
	spdim.last  = NULL;
}

/* Free a SparseDim vertically */
void clear_SparseCol(SparseDim spdim){
	SparseElem* cur;
	SparseElem* next;
	if((cur = spdim.first)){
		while(cur){
			next = cur->nextL;
			free(cur);
			cur  = next;
		}
	}
	spdim.first = NULL;
	spdim.last  = NULL;
}

/* Initialize a vector of reals with zeros */
void init_vec(real *vector, int dim1)
{
    int i;
    for(i=0; i < dim1; i++)
        vector[i] = 0.0;
}

/* Initialize a vector of integers with zeros */
void init_int_vec(int *vector, int dim)
{
    int i;
    
    for (i=0; i<dim; i++)
        vector[i] = 0;
}

/* Initialize a vector of integers with zeros */
void init_uchar_vec(uchar *vector, int dim)
{
    int i;
    
    for (i=0; i<dim; i++)
        vector[i] = 0;
}

/* Initialize a vector of chars with zeros */
void init_char_vec(char *vector, int dim)
{
    int i;
    
    for (i=0; i<dim; i++)
        vector[i] = 0;
}

/* Initialize a matrix of reals with zeros */
void init_mat(real **matrix, int dim1, int dim2)
{
    int i, j;
    
    for (i=0; i<dim1; i++)
        for(j=0; j<dim2; j++)
            matrix[i][j] = 0.0;
}

/* Initialize a matrix of integers with zeros */
void init_int_mat(int **matrix, int dim1, int dim2)
{
    int i, j;
    
    for (i=0; i<dim1; i++)
        for(j=0; j<dim2; j++)
            matrix[i][j] = 0;
}

/* Initialize a 3-dimensional matrix of reals with zeros */
void init_mat3(real ***matrix, int dim1, int dim2,int dim3)
{
    int i,j,k;
    
    for (i=0; i<dim1; i++)
        for(j=0; j<dim2; j++)
            for(k=0; k<dim3; k++)
                matrix[i][j][k] = 0;
}

/* Initialize a 3-dimensional matrix of integers with zeros */
void init_int_mat3(int ***matrix, int dim1, int dim2,int dim3)
{
    int i,j,k;
    
    for (i=0; i<dim1; i++)
        for(j=0; j<dim2; j++)
            for(k=0; k<dim3; k++)
                matrix[i][j][k] = 0;
}

/*Initialize an array of SparseDim*/
void init_SparseMat(SparseMatrix* spmat){
	spmat->nnz         = 0;
	spmat->minOutDeg   = 0;
	spmat->maxOutDeg   = 0;
	spmat->meanOutDeg  = 0.0;
	spmat->symmetric   = 0;
	if(spmat->rowSums)init_vec(spmat->rowSums,spmat->nbRows);
	if(spmat->rows)   init_Dims(spmat->rows,spmat->nbRows);
    if(spmat->cols)   init_Dims(spmat->cols,spmat->nbCols);
}

/*Initialize an array of SparseDim*/
void init_Dims(SparseDim* sdim,int dim1){
    int i;
    
    for(i=0;i<dim1;i++){
        sdim[i].first = NULL;
        sdim[i].last  = NULL;
    }    
}

/*Initialize a PerfMeasures struct*/
void init_PerfMeasures(PerfMeasures* perfMeasures,int nbClass){
	perfMeasures->classificationRate = 0.0;
	perfMeasures->macroPrecision     = 0.0;
	perfMeasures->macroRecall        = 0.0;
	perfMeasures->macroF1            = 0.0;
	perfMeasures->microPrecision     = 0.0;
	perfMeasures->microRecall        = 0.0;
	perfMeasures->microF1            = 0.0;
	init_int_mat(perfMeasures->confusion,nbClass,nbClass);
}

/*Set the default input parameters*/
void init_InputParams(InputParams* inputParams){
	inputParams->wlen          = DEF_WALK_LEN;
	inputParams->nbFolds       = DEF_NB_FOLDS;
	inputParams->cvSeed        = DEF_CV_SEED;
	inputParams->crossWalks    = DEF_CROSSWALKS;
    inputParams->verbose       = DEF_VERBOSE;
	inputParams->perfMeasure   = DEF_PERF_MEASURE;
	inputParams->maxIter       = DEF_MAX_ITER;
	inputParams->featNormalize = DEF_FEAT_NORMALIZE;
	inputParams->nodeDegree    = DEF_NODE_DEGREE;
	inputParams->probeFactor   = DEF_PROBE_FACTOR;
	inputParams->dataFname     = NULL;
    inputParams->graphFname    = NULL;
	inputParams->gramFname     = NULL;
    inputParams->outFname      = NULL;
	inputParams->tarFname      = NULL;
	inputParams->outPRED       = NULL;
	inputParams->outLEN        = NULL;
	inputParams->priors        = NULL;
	inputParams->simParams     = NULL;
}

/*Initialize a LSVMDataInfo struct*/
void init_LSVMDataInfo(LSVMDataInfo* info){
	info->nbLines    = 0;
	info->nbFeatures = 0;
	info->nbClass    = 0;
	info->M1P1       = 0;
}

/*Initialize a RandMemory struct*/
void init_RandMemory(RandMemory* memory){
	int i;
	memory->cursor  = 0;
	for(i=0;i<memory->n;i++)
		memory->partition[i] = i;
}