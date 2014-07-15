/*************************************************************************/
/* Author      : 	Jerome Callut (jcallut@gmail.com)                    */
/*					Kevin Françoisse (kfrancoi@gmail.com)				 */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "files.h"
#include "defines.h"
#include "memory.h"
#include "tools.h"
#include "textDisplay.h"
/*#ifdef WIN32*/
	#include "strtok_r.h"
/*#endif*/

/* Read a vector of real in input file */
int readVec(char* fname,real* I,int m){
    
    FILE* inFile = fopen(fname,"r");
    int  i,nnz=0;
    float val;
    
    if(!inFile){
        fprintf(stderr,"File %s does not exist\n",fname);
        exit(EXIT_FAILURE);
    }       
    
    for(i=0;i<m;i++){
        fscanf(inFile,"%e",&val);
        I[i] = (real)val;
        if (val != 0) nnz++;
    }
    fclose(inFile);
    return nnz;
}

/* Read matrix of real in input file */
int readMat(char* fname,SparseMatrix* spmat,char* labels,DataStat* dataStat,int verbose){
	FILE*     inFile = fopen(fname,"r");
	char      line[BUF_LINE_LEN];
	char*     sym;
	int       i;
	int       from,to;
	int       lastFrom=-1;
	int       lastTo=1;
	int       nbElems;
	int       sumDeg=0;
	int       nbClass;
	int       nbNodes;
	float     val;
	char      label;
	char      *ctxL,*ctxE;
	
	/*Fetch reference in the sparse matrix*/
	real      *rowSums = spmat->rowSums;
	SparseDim *rows    = spmat->rows;
	SparseDim *cols    = spmat->cols;

	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}    

	/*Get the number of distinct classes*/
	nbClass = readNbClass(fname);

	/*Initialize the degree statistics*/
	spmat->nnz           = 0;
	spmat->minOutDeg     = 0;
	spmat->maxOutDeg     = 0;
	spmat->meanOutDeg    = 0.0;
	dataStat->nbAuxNodes = 0;
	init_int_vec(dataStat->classCount,nbClass+1);

	/*Read the number of nodes in the first line*/
	fgets(line,BUF_LINE_LEN,inFile);
	sym=(char*)strtok_r(line," \n",&ctxL);
	nbNodes=atoi(sym);
	
	/*Display progression*/
	if(verbose >= 1){
		printf("Loading the graph  :      ");
		fflush(stdout);
	}	
		
	while(fgets(line,BUF_LINE_LEN,inFile)){
		nbElems = 0;
		/*Fetch the "from" node number*/
		if ((sym=(char*)strtok_r(line," \n",&ctxL))){
			from  = atoi(sym)-1;
			/*Check that nodes are provided in increasing order*/
			if (from <= lastFrom){
				fprintf(stderr,"\nNodes must be provided in increasing order.\n");
				fprintf(stderr,"Check node %i in file %s\n",from+1,fname);
				exit(EXIT_FAILURE);
			}
			lastFrom = from;
			/*Display progression*/
			if(verbose >= 1){
				backspace(5);
				printf("%3.i %%",(int)(100*((float)(from+1)/(float)nbNodes)));
				fflush(stdout);
			}
			/*Fetch the label of the node*/
			sym   = (char*)strtok_r(NULL," \n",&ctxL);
			label = atoi(sym);
			labels[from] = label;
			if (label >= 0)
				dataStat->classCount[label] += 1;
			else
				dataStat->nbAuxNodes += 1;
			/*Scan all the "to" nodes*/
			lastTo = -1;
			while((sym=(char*)strtok_r(NULL, " \n",&ctxL))){
				to  = atoi((char*)strtok_r(sym,":",&ctxE))-1;
				val = (real)atof((char*)strtok_r(NULL,":",&ctxE));
				/*Check that nodes are provided in increasing order*/
				if (to <= lastTo){
					fprintf(stderr,"\nNodes must be provided in increasing order inside lines.%s\n",fname);
					fprintf(stderr,"Check entry %i -> %i in file %s\n",from+1,to+1,fname);
					exit(EXIT_FAILURE);
				}
				lastTo = to;
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
				/*Update the number of non-nul elements*/
				spmat->nnz += 1;                    
			}/*End while(to node)*/
			if(nbElems > spmat->maxOutDeg)
				spmat->maxOutDeg = nbElems;
			if(spmat->minOutDeg == 0 || nbElems < spmat->minOutDeg)
				spmat->minOutDeg = nbElems;                
			sumDeg += nbElems;                
		}/*End if(line contains tokens)*/    
	}/*End iterate on the lines*/    

	/*Display progression*/
	if(verbose >= 1){
		printf("\b\b\b\b\b100 %%\n");
		fflush(stdout);
	}

	/*Compute the matrix density*/
	dataStat->density = (((float)sumDeg/(float)(nbNodes))/(float)nbNodes);

	/*Compute the mean out degree*/
	spmat->meanOutDeg = (float)sumDeg/(float)nbNodes;
	
	/*Compute the class proportions*/
	for(i=0;i<=nbClass;i++)
		dataStat->classProp[i] = (float)dataStat->classCount[i]/(float)nbNodes;

	fclose(inFile);
	return spmat->nnz;
}

/* Read a LibSVM/SVMLight dataset from a file */
int readLSVMData(char* fname,LSVMDataInfo* dataInfo,SparseMatrix* spmat,char* labels,DataStat* dataStat,int verbose){
	FILE*     inFile = fopen(fname,"r");
	char      line[BUF_LINE_LEN];
	char*     sym;
	int       i;
	int       lineNum=0,featId;
	int       lastFeatId=1;
	int       nbFeat;
	long int  sumNbFeat=0;
	long int  nbBytes=0;
	float     val;
	char      label;
	char      *ctxL,*ctxE;


	/*Fetch reference in the sparse matrix*/
	SparseDim *rows = spmat->rows;
	SparseDim *cols = spmat->cols;

	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}    

	/*Initialize the statistics*/
	spmat->nnz           = 0;
	spmat->minOutDeg     = 0;
	spmat->maxOutDeg     = 0;
	spmat->meanOutDeg    = 0.0;
	init_int_vec(dataStat->classCount,(dataInfo->nbClass)+1);

	/*Display progression*/
	if(verbose >= 1){
		printf("Loading the data   :      ");
		fflush(stdout);
	}	
		
	while(fgets(line,BUF_LINE_LEN,inFile)){
		/*If the line is empty or starts with '#', we skip it*/
		if(strlen(line) == 0 || line[0] == '#') continue;
		/*Parse the line*/
		if ((sym=(char*)strtok_r(line," \n",&ctxL))){
		/*if ((sym=(char*)strtok(line," \n"))){*/
			nbFeat = 0;
			/*Display progression*/
			if(verbose >= 1){
				backspace(5);
				printf("%3.i %%",(int)(100*((float)(lineNum+1)/(float)dataInfo->nbLines)));
				fflush(stdout);
			}
			/*Store the label*/
			label = atoi(sym);
			/*If the label is {-1,+1}, remap it to {1,2}*/
			if (dataInfo->M1P1) REMAP_M1P1(label);
			labels[lineNum] = label;
			dataStat->classCount[label] += 1;
			/*Scan all the features*/
			lastFeatId = -1;
			while((sym=(char*)strtok_r(NULL, " \n",&ctxL))){
				featId  = atoi((char*)strtok_r(sym,":",&ctxE))-1;
				val = (real)atof((char*)strtok_r(NULL,":",&ctxE));
				/*Check that features are provided in increasing order*/
				if (featId <= lastFeatId){
					fprintf(stderr,"\nFeatures must be provided in increasing order inside lines.%s\n",fname);
					fprintf(stderr,"Check feature %i in line %i of file %s\n",featId+1,lineNum+1,fname);
					exit(EXIT_FAILURE);
				}
				lastFeatId = featId;
				/*Add the element in the row "lineNum"*/
				if (!rows[lineNum].last){
					rows[lineNum].first = (SparseElem*)malloc(sizeof(SparseElem));
					rows[lineNum].last  = rows[lineNum].first;
					nbBytes += sizeof(SparseElem);
				}
				else{
					rows[lineNum].last->nextC  = (SparseElem*)malloc(sizeof(SparseElem));
					rows[lineNum].last         = rows[lineNum].last->nextC;
					nbBytes += sizeof(SparseElem);
				}
				/*Put the value in the element*/
				rows[lineNum].last->l     = lineNum;
				rows[lineNum].last->c     = featId;
				rows[lineNum].last->val   = val;
				rows[lineNum].last->nextL = NULL;
				rows[lineNum].last->nextC = NULL;
				nbFeat++;

				/*Add the element in the column "featId"*/
				if(!cols[featId].last){
					cols[featId].first  = rows[lineNum].last;
					cols[featId].last   = rows[lineNum].last;
				}
				else{
					cols[featId].last->nextL = rows[lineNum].last;
					cols[featId].last = rows[lineNum].last;
				}
				/*Update the number of non-nul elements*/
				spmat->nnz += 1;                    
			}/*End while(featId node)*/
			if(nbFeat > spmat->maxOutDeg)
				spmat->maxOutDeg = nbFeat;
			if(spmat->minOutDeg == 0 || nbFeat < spmat->minOutDeg)
				spmat->minOutDeg = nbFeat;                
			sumNbFeat += nbFeat;
			lineNum++;
		}/*End if(line contains tokens)*/    
	}/*End iterate on the lines*/    

	/*Display progression*/
	if(verbose >= 1){
		backspace(5);
		printf("100 %% (%1.2f Mb in memory)\n",(double)nbBytes/(double)(1048576));
		fflush(stdout);
	}

	/*Compute the dataset density*/
	dataStat->density = (float)(sumNbFeat)/(float)(dataInfo->nbLines*dataInfo->nbFeatures);

	/*Compute the mean out degree*/
	spmat->meanOutDeg = (float)sumNbFeat/(float)lineNum;
	
	/*Compute the class proportions*/
	for(i=0;i<=dataInfo->nbClass;i++)
		dataStat->classProp[i] = (float)dataStat->classCount[i]/(float)dataInfo->nbLines;

	fclose(inFile);
	return spmat->nnz;
}

/*Read the number of instance in a gram matrix*/
int readNbInst(char* fname){
    FILE*  inFile   = fopen(fname,"r");
    char   line[BUF_LINE_LEN];
    int    nbInst   = 0;
    
	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}    

    /*Count the lines*/
    while(fgets(line,BUF_LINE_LEN,inFile)){
        if(line != NULL && strlen(line) > 5)
            nbInst++;
    }
   fclose(inFile);
   return nbInst;
}

/*Read the gram matrix from a libsvm file*/
void readGram(char* fname,real* gram,int* labels,int* ids,int m){
    FILE*  inFile   = fopen(fname,"r");
    char   line[BUF_LINE_LEN];
    real   value    = 0.0;
    int    i,j,k;
    
	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}

	/*Check that the number of lines matches with parameter m*/
	if((i=readNbInst(fname)) != m){
		fprintf(stderr,"Gram matrix contains %i lines whereas it must contain %i lines\n",i,m);
		exit(EXIT_FAILURE);		
	}	

    /*Scan the lines*/
    for(i=0,k=0;i<m;++i){
        fgets(line,BUF_LINE_LEN,inFile);
        if(line != NULL){
			if(labels != NULL)
            	labels[i] = atoi(strtok(line," ")); /* Read the label */
			else
				atoi(strtok(line," "));             /* Skip the label */
            strtok(NULL,":");                       /* Skip the first feature id */
			if(ids != NULL)
            	ids[i] = atoi(strtok(NULL," "));    /* Read the instance id */
			else
				atoi(strtok(NULL," "));             /* Skip the instance id */
				
            /*Iterate on the entries*/
            for(j=0;j<=i;j++){
                /*Skip the entry number*/
                strtok(NULL,":");
                /*Read the entry*/
                value = (real)atof(strtok(NULL," "));
                gram[k++] = value;
            }
        }
    }
   fclose(inFile); 
}

/* Read the number of distrinct classes*/
int readNbClass(char* fname){
    FILE*  inFile = fopen(fname,"r");
	char   line[BUF_LINE_LEN];
	char*  sym;
	int    label;
	int    maxLabel=0;
	char   *ctxL;
	
	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}    
	
	/*Skip the first line*/
	fgets(line,BUF_LINE_LEN,inFile);
	while(fgets(line,BUF_LINE_LEN,inFile)){
		/*Fetch the "from" node number*/
		if ((sym=(char*)strtok_r(line," \n",&ctxL))){
			/*Fetch the label of the node*/
			sym = (char*)strtok_r(NULL," \n",&ctxL);
			label = atoi(sym);
			if(label > 0 && label > maxLabel)
				maxLabel = label;
			if(label < -1){
				fprintf(stderr,"Labels must be positive except for auxilary nodes (-1)\n",fname);
				exit(EXIT_FAILURE);
			}

		}/*End if(line contains tokens)*/ 
	}/*End iterate on the lines*/    
	fclose(inFile);
	return maxLabel;
}

/* Read the number of nodes from the adjacency matrix file */
int readNbNodes(char* fname){
    FILE*  inFile  = fopen(fname,"r");
    int    nbNodes=0;
    
    if(!inFile){
        fprintf(stderr,"File %s does not exist\n",fname);
        exit(EXIT_FAILURE);
    }    
    
    /*Read the first line */
    fscanf(inFile,"%i",&nbNodes);

    fclose(inFile);
    return nbNodes;
}

/* Read the number of distrinct classes*/
void getLSVMDataInfo(char* fname,LSVMDataInfo* info){
	int    featId;
	uchar  NEG=0,POS=1,M1P1=1;
	FILE*  inFile = fopen(fname,"r");
	char   line[BUF_LINE_LEN];
	char*  sym = NULL;
	int    label;
	char   *ctxL,*ctxE;

	/*Initialize informations*/
	init_LSVMDataInfo(info);

	if(!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}

	while(fgets(line,BUF_LINE_LEN,inFile)){
		/*If the line is empty or starts with '#', we skip it*/
		if(strlen(line) == 0 || line[0] == '#') continue;
		/*Fetch the "from" node number*/
		if ((sym=(char*)strtok_r(line," \n",&ctxL))){
		/*if ((sym=(char*)strtok(line," \n"))){*/		
			info->nbLines++;
			label = atoi(sym);
			if(label < 0) NEG = 1;
			if(label > 0) POS = 1;
			if(label !=0 && (label != -1 && label != 1))M1P1 = 0;				
			if(label > 0 && label > info->nbClass) info->nbClass = label;
			while((sym=(char*)strtok_r(NULL, " \n",&ctxL))){

				featId = atoi((char*)strtok_r(sym,":",&ctxE));
				if (featId > info->nbFeatures) info->nbFeatures = featId;
				strtok_r(NULL,":",&ctxE);
			}
		}/*End if(line contains tokens)*/ 
	}/*End iterate on the lines*/

	/*Check that the labels are valid*/
	if(!M1P1 && NEG){
		fprintf(stderr,"Labels must be either -1/+1 or all >= 1\n",fname);
		exit(EXIT_FAILURE);
	}

	/*Check if the labels are -1/+1*/
	if(M1P1){
		info->M1P1    = 1;
		info->nbClass = NEG+POS;
	}
	else
		info->M1P1    = 0;

	fclose(inFile);
}

/* Read a file containing targets for unlabeled nodes*/
int readTargets(char* fname,int* unlabeledIdx,char* targets){
	int    idx,pos = 0;
	int    label,maxLabel=0;
	FILE*  inFile  = fopen(fname,"r");
	char   line[BUF_LINE_LEN];    

    if(!inFile){
        fprintf(stderr,"File %s does not exist\n",fname);
        exit(EXIT_FAILURE);
    }

	while(fgets(line,BUF_LINE_LEN,inFile)){
		if(pos == unlabeledIdx[0]) break;
		/*Read the index of the unlabeled node*/
		idx = atoi(strtok(line," "));
		/*Check that the index corresponds to the current unlabeled node*/
		if(idx != unlabeledIdx[pos+1]+1){
	        fprintf(stderr,"Index %i mismatch in target file !\n",idx);
			fprintf(stderr,"Index %i was expected instead\n",unlabeledIdx[pos+1]+1);
	        exit(EXIT_FAILURE);			
		}
		/*Read the target value for the unlabeled node*/
		label = atoi(strtok(NULL," "));
		targets[pos++] = label;
		if(label > maxLabel)
			maxLabel = label;
	}
	/*Check that a target has been provided for each unlabeled node*/
	if(pos < unlabeledIdx[0]-1){
        fprintf(stderr,"There are missing targets in the target file\n");
        exit(EXIT_FAILURE);		
	}
	fclose(inFile);
	return maxLabel;
}

/* Read a file containing targets for unlabeled nodes*/
int readLSVMTargets(char* fname,char* targets){

	int		pos = 0;
	int		label,maxlabel=0;
	FILE*	inFile	= fopen(fname,"r");
	char	line[BUF_LINE_LEN];

	if (!inFile){
		fprintf(stderr,"File %s does not exist\n",fname);
		exit(EXIT_FAILURE);
	}

	while(fgets(line,BUF_LINE_LEN,inFile)){
		/*Read the target value for the unlabeled node*/
		label = atoi(strtok(line," "));
		targets[pos++] = label;
		if(label>maxlabel)
			maxlabel = label;
	}
	fclose(inFile);
	return maxlabel;

}


/* Write a vector of real in a sparse file*/
void writeVec(real* I,int m,char* fname){
    
    int i;
    FILE* outFile = fopen(fname,"w");
     
    for(i=0;i<m;i++){
        if(I[i] > 0)
            fprintf(outFile,"%i:%e\n",i+1,I[i]);
    }
    fclose(outFile);    
}

/* Write a matrix of real in a file*/
void writeMat(real** P,int m,char* fname){
    
    int i,j; 
    FILE* outFile = fopen(fname,"w");
    
    for(i=0;i<m;i++){
        for(j=0;j<m;j++){
            fprintf(outFile,"%e ",P[i][j]);
        }
        fprintf(outFile,"\n");
    }
    fclose(outFile);    
}

/*Write the model prediction */
void writePredictions(char* fname,char* preds,real** N,int nbNodes,int nbClass){
	int i,j;
    FILE* outFile = fopen(fname,"w");
	
	/*Write header*/
	fprintf(outFile,"#Node PredictedClass");
	for(j=1;j<=nbClass;j++){
		fprintf(outFile," ScoreClass%i",j);
	}
	fprintf(outFile,"\n");
	
	/*Iterate on nodes*/
	for(i=0;i<nbNodes;i++){
		fprintf(outFile,"%i %i",i+1,preds[i]);
		/*Iterate on classes*/
		for(j=0;j<nbClass;j++){
			fprintf(outFile," %e",N[i][j]);
		}
		fprintf(outFile,"\n");
	}
	fclose(outFile);
}

/*Write the model prediction for unabeled nodes only*/
void writePredictions_unlabeled(char* fname,char* preds,real** N,int nbClass,int* unlabeled){
	int i,j;
    FILE* outFile = fopen(fname,"w");
	
	/*Write header*/
	fprintf(outFile,"#Node PredictedClass");
	for(j=1;j<=nbClass;j++){
		fprintf(outFile," ScoreClass%i",j);
	}
	fprintf(outFile,"\n");
	
	/*Iterate on nodes*/
	for(i=1;i<=unlabeled[0];i++){
		fprintf(outFile,"%i %i",unlabeled[i]+1,preds[i-1]);
		/*Iterate on classes*/
		for(j=0;j<nbClass;j++){
			fprintf(outFile," %e",N[i-1][j]);
		}
		fprintf(outFile,"\n");
	}
	fclose(outFile);
}
