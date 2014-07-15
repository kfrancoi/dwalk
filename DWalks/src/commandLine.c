/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)						 */
/*				 Kevin Francoisse (kfrancoi@gmail.com)					 */
/* Version     : 1.00                                                    */
/* Modified    : 13/02/2008                                              */
/*************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "commandLine.h"
#include "defines.h"
#include "dwalk.h"
#include "tools.h"
#include "files.h"
#include "memory.h"
#include "graphStat.h"
#include "crossval.h"
#include "perfMeasure.h"
#include "textDisplay.h"
#ifdef WIN32 /*getopt is not part of the ANSI stdlib, so we use a custom code*/
	#include "getopt.h"
#else
	#include <getopt.h>
#endif

/* Print the command line banner*/
void printBanner(){
	int i;
	printf("\nDiscriminative Walks v.%s (%s%s) %s\n",VERSION,PLATFORM,OPENMP_STR,COPYRIGHT);
	printf("---------------------------");
	for(i=0;i<(int)(strlen(VERSION)+strlen(COPYRIGHT)+strlen(PLATFORM)+strlen(OPENMP_STR));i++)printf("-");
	printf("\n");
	if (strlen(BUILD_NOTE) > 0) printf("%s\n",BUILD_NOTE);
	if (strlen(DISCLAIMER) > 0) printf("%s\n",DISCLAIMER);
	printf("\n");
}

/* Print command line help for the discriminative walks*/
void printHelp(){
    printf("Usage     : dwalk -g graphFile -l walkLen -o outFile [options]\n\n");
    printf("Function  : Classify the nodes of a graph using random walks\n");
    printf("Arguments : -g graphFile  the file containing the labeled adjacency matrix\n");
    printf("            -l walkLen    the maximum length of the random walks\n");
    printf("            -o outFile    the output file containing the expected times\n");
	printf("Options   : -x            allow crossing node of the starting/ending class\n");
	printf("            -k            read a feature libsvm matrix to go along with the graph\n");
	printf("            -s [1-4],a,b,c  similarity function for edge weights (default: %i)\n",DEF_SIMILARITY);
	printf("                            1 : k(x,z) = <x,z>\n");
	printf("                            2 : k(x,z) = 1/||x-z||^a\n");
	printf("                            3 : k(x,z) = (a*<x,z>+b)^c\n");
	printf("                            4 : k(x,z) = exp(-a*||x-z||^2)\n");
	printf("            -v verbose      verbosity level (default: 1)\n");
	printf("            -m            compute the minimum covering length\n"); 
    printf("            -p priors     the class priors separated by # (default empirical priors)\n");
	printf("            -c nFolds     tune walkLen using a n-folds cross-validation (-1 = LOO)\n"); 
	printf("            -z seed       random seed for shuffling in CV (default current time)\n"); 
	printf("            -t targetFile the file containing labels of unlabeled nodes\n");
	printf("            -v verbose    verbosity level (default 1)\n");	
    printf("            -h            display this help\n\n");
}


/*Command line entry point of the program*/
int main (int argc, char* argv[]){
    
    /*getopt() variables */
    int   opt;
	char* optarg_dup = NULL;
    
    /*Command line options*/
	uchar cflg=0,gflg=0,kflg=0,lflg=0,mflg=0,oflg=0, sflg=0;
	uchar pflg=0,tflg=0,vflg=0,xflg=0,zflg=0;
    InputParams* inputParams = NULL;

    /*Working variables */
	int            i,m;
	int            nbPriors;
    int            nbNodes;
	int            nbClass;
	int            nbLabeled;
	int            nbUnlabeled;
	int			   nbSimParams;
	int            actualWlen;
	int            start_cpu,stop_cpu;
	real           tmp;
	uchar*         absorbing     = NULL;
	char*          labels        = NULL;
	char*          featLabels    = NULL;
	char*          targets       = NULL;
	char*		   preds         = NULL;
	int**          classes       = NULL;
	real**         N             = NULL;
	real*          gram          = NULL;
    real**         latF          = NULL;
    real**         latB          = NULL;
	real**		   kernel		 = NULL;
	SparseMatrix*  spmat         = NULL;
	SparseMatrix*  dataset       = NULL;
	DataStat*      graphStat     = NULL;
	DataStat*      featuresStat  = NULL;
	DWInfo*        dwInfo        = NULL;
	OptiLen*       optiLen       = NULL;
	PerfMeasures*  unlabeledPerf = NULL;
	LSVMDataInfo*  dataInfo      = NULL;
	
	/*Print the program banner*/
	printBanner();

    /*Scan the command line*/
	inputParams = malloc(sizeof(InputParams));
	init_InputParams(inputParams);
	while ((opt = getopt(argc,argv,"c:g:l:xms:k:o:p:rt:v:z:h")) != EOF){
        switch(opt){
	        case 'c':
	            cflg++;
				inputParams->nbFolds = atoi(optarg);
	            break;
            case 'g':
                gflg++;
				inputParams->graphFname = (char*)malloc(FNAME_LEN*sizeof(char));
				strncpy(inputParams->graphFname,optarg,FNAME_LEN);
                break;
            case 'k':
				kflg++;
				inputParams->gramFname = (char*)malloc(FNAME_LEN*sizeof(char));
				strncpy(inputParams->gramFname,optarg,FNAME_LEN);
                break;
			case 's':
                sflg++;
                optarg_dup  = (char*)__strdup(optarg);
				nbSimParams = getNbTokens(optarg,",");
				inputParams->simParams = vec_alloc(nbSimParams+1);
				tokenizeReals(optarg_dup,",",inputParams->simParams);
				break;
            case 'l':
                lflg++;
                inputParams->wlen = atoi(optarg);
                break;
			case 'm':
				mflg++;
				break;
            case 'o':
                oflg++;
				inputParams->outFname = (char*)malloc(FNAME_LEN*sizeof(char));
				strncpy(inputParams->outFname,optarg,FNAME_LEN);
				inputParams->outPRED  = (char*)malloc(FNAME_LEN*sizeof(char));
				inputParams->outLEN   = (char*)malloc(FNAME_LEN*sizeof(char));
                addExtension(inputParams->outFname,"pred",inputParams->outPRED);
				addExtension(inputParams->outFname,"len",inputParams->outLEN);
                break;
            case 'p':
                pflg++;
                optarg_dup = (char*)__strdup(optarg);
				nbPriors   = getNbTokens(optarg,"#");
                inputParams->priors = vec_alloc(nbPriors+1);
				tokenizeReals(optarg_dup,"#",inputParams->priors);
                break;
			case 't':
				tflg++;
				inputParams->tarFname = (char*)malloc(FNAME_LEN*sizeof(char));
				strncpy(inputParams->tarFname,optarg,FNAME_LEN);
				break;
            case 'v':
                vflg++;
                inputParams->verbose = atoi(optarg);
                break;
			case 'x':
				xflg++;
				inputParams->crossWalks = 1;
				break;
			case 'z':
				zflg++;
				inputParams->cvSeed = atoi(optarg);
				break;
            case 'h':
                printHelp();
                exit(EXIT_FAILURE);
                break;
        }
    }
    
    /*Check mandatory arguments*/
    if(!gflg || !lflg || (!oflg && !mflg)){
        fprintf(stderr,"Mandatory argument(s) missing\n");
        printHelp();
        exit(EXIT_FAILURE);
    }

	if( (kflg && !sflg) || (!kflg && sflg)){
		fprintf(stderr, "Error with 'k' and 's' parameters\n");
		printHelp();
		exit(EXIT_FAILURE);
	}

	/*Check that the walk length is greater than 2*/
	if(inputParams->wlen < 2){
		fprintf(stderr,"The walkLen must be >= 2\n");
		exit(EXIT_FAILURE);
	}

	/*Check that there are the right number of similarity parameters*/
	if (kflg && sflg){
		if(inputParams->simParams){
			switch((int)inputParams->simParams[1]){
				case 1 :
					if((int)inputParams->simParams[0] != 1){
						fprintf(stderr,"The similarity type 1 must have no parameters\n");
						exit(EXIT_FAILURE);	
					}
					break;
				case 2 :
					if((int)inputParams->simParams[0] != 2){
						fprintf(stderr,"The similarity type 2 must have exactly 1 parameter\n");
						exit(EXIT_FAILURE);	
					}
					break;
				case 3 :
					if((int)inputParams->simParams[0] != 4){
						fprintf(stderr,"The similarity type 3 must have exactly 3 parameters\n");
						exit(EXIT_FAILURE);	
					}
					break;
				case 4 :
					if((int)inputParams->simParams[0] != 2){
						fprintf(stderr,"The similarity type 4 must have exactly 1 parameter\n");
						exit(EXIT_FAILURE);	
					}
					break;
				default :
					fprintf(stderr,"Unrecognized similarity type\n");
					exit(EXIT_FAILURE);	
			}
		}
	}

    /*Get the number of nodes in the graph*/
    nbNodes = readNbNodes(inputParams->graphFname);
    
	/*Get the number of distinct classes*/
	nbClass = readNbClass(inputParams->graphFname);

	/*Get info from the LIBSVM data*/
	if (kflg){
		dataInfo = malloc(sizeof(LSVMDataInfo));
		getLSVMDataInfo(inputParams->gramFname,dataInfo);
		dataset = spmat_alloc(dataInfo->nbLines,dataInfo->nbFeatures,0);
		init_SparseMat(dataset);
		featuresStat = DataStat_alloc(dataInfo->nbClass);
		featLabels = char_vec_alloc(nbNodes);
	}
	
	/*Check if the number of nodes does not exceed the limitation*/
	if(nbNodes > MAX_NODES){
		fprintf(stderr,"This version is limited to maximum %i nodes\n",MAX_NODES);
		exit(EXIT_FAILURE);
	}

	/*Check that the number of classes is lower than 128*/
	if(nbClass > 127){
		fprintf(stderr,"The number of classes must be <= 127\n");
		exit(EXIT_FAILURE);		
	}

	/*Check that the number of folds is between 2 and nbNodes*/
	if(cflg){
		if(inputParams->nbFolds == 1 || inputParams->nbFolds > nbNodes){
			fprintf(stderr,"The number of folds must be > 1 and <= number of nodes\n");
			exit(EXIT_FAILURE);
		}
	}

    /*Allocate data structure*/
    latF             = mat_alloc(inputParams->wlen+1,nbNodes);
    latB             = mat_alloc(inputParams->wlen+1,nbNodes);
	kernel			 = mat_alloc(nbNodes, nbNodes);
	classes          = (int**)malloc(sizeof(int*)*(nbClass+1));
	labels           = char_vec_alloc(nbNodes);
	absorbing        = uchar_vec_alloc(nbNodes);
	if(kflg) gram    = vec_alloc((nbNodes*(nbNodes+1))/2);
	dwInfo           = malloc(sizeof(DWInfo));
	dwInfo->mass_abs = vec_alloc(nbClass);
	spmat            = spmat_alloc(nbNodes,nbNodes,1);
	graphStat        = DataStat_alloc(nbClass+1);
	optiLen          = malloc(sizeof(OptiLen));
	
    /*Initialize the sparse transition matrix and the dataset if required*/
	init_SparseMat(spmat);

    /*Read the adjacency matrix*/
    readMat(inputParams->graphFname,spmat,labels,graphStat,inputParams->verbose);
    isSymmetric(spmat);

	/*Get the indices of the nodes in each class */
	getClasses(labels,nbNodes,classes,nbClass);

	/*Get the number of labeled nodes*/
	nbUnlabeled = classes[0][0];
	nbLabeled   = nbNodes - nbUnlabeled;

	/*If provided, check that the priors match the number of classes*/
	if(pflg){
		if(nbClass != nbPriors){
			printf("The number of priors does not match with the number of classes\n");
			exit(EXIT_FAILURE);			
		}
		/*Check that the priors sum up to 1*/
		else{
			tmp=0.0;
			for(i=1;i<=inputParams->priors[0];i++){
				tmp += inputParams->priors[i];
			}
			if(ABS(tmp-1.0) > PROB_EPS){
				textcolor(BRIGHT,RED,BLACK);
	            printf("WARNING: The class priors do not sum up to 1\n");
				textcolor(RESET,WHITE,BLACK);
			}
		}
	}
	/*If no priors provided, use empirical priors */
	else{
		inputParams->priors = vec_alloc(nbClass+1);
		inputParams->priors[0] = (real)nbClass;
		tmp = 0.0;
		for(i=1;i<=inputParams->priors[0];i++)
			tmp += graphStat->classCount[i];
		for(i=1;i<=inputParams->priors[0];i++)
			inputParams->priors[i] = (real)graphStat->classCount[i]/tmp;
	}

	/*If provided read the LIBSVM feature matrix*/
	if(kflg){
		m = readLSVMData(inputParams->gramFname,dataInfo,dataset,featLabels,featuresStat,inputParams->verbose);
		if (dataInfo->nbLines != nbNodes){
			fprintf(stderr,"Number of line on the LIBSVM (%i) file doesn't match the number of nodes (%i)\n", dataInfo->nbLines, nbNodes);
			exit(EXIT_FAILURE);
		}
		
		/* Multiply a kernel matrix based on the dataset*/
		/* TO DO : define a parameters to lower the importance of the features*/
		buildKernel2(spmat, dataset, 0.1, inputParams);
		/*Multiply adjacency matrix*/
	}

	
    
    /*Print statistics about the graph, classes, and run-mode*/
    if (inputParams->verbose > 0)
		printInputInfo(spmat,graphStat,nbClass,inputParams);               
    
	/*Minimum Covering Length mode*/
	if (mflg){
		computeMCL(spmat,absorbing,labels,classes,nbClass,latF,latB,inputParams);
		/*Exit after displaying the statistics*/
		exit(EXIT_SUCCESS);		
	}

	 /*Start the CPU chronometer*/
    start_cpu = clock();

	/*If required tune the maximum walk length by cross-validation*/
	if(cflg){
		crossValidateLength(spmat,absorbing,labels,classes,nbClass,NULL,latF,latB,inputParams,optiLen);
		actualWlen = optiLen->len;
	}
	/*Otherwise use the prescribed length*/
	else
		actualWlen = inputParams->wlen;

	/************************ALGORITHM STARTS HERE****************************/

	if(inputParams->verbose >= 1){
		textcolor(BRIGHT,RED,BLACK);
		printf("Performing discriminative walks up to length %i on full data\n",actualWlen);
		textcolor(RESET,WHITE,BLACK);
	}

	/*Allocate data structure*/
    N     = mat_alloc(nbUnlabeled,nbClass);
	preds = char_vec_alloc(nbUnlabeled);
    init_mat(N,nbUnlabeled,nbClass);

	/*Launch the D-walks*/
	dwalk(spmat,absorbing,classes,nbClass,N,latF,latB,actualWlen,inputParams->crossWalks,dwInfo,inputParams->verbose);

	/************************ALGORITHM STOPS HERE****************************/

    /*Stop the CPU chronometer*/
    stop_cpu = clock();
    
	/*Compute the predictions as the argmax on classes for each unlabeled node*/
	computePredictions(N,preds,inputParams->priors,nbUnlabeled,nbClass);

	/*Write the model predictions*/
	writePredictions_unlabeled(inputParams->outPRED,preds,N,nbClass,classes[0]);

	/*Write the class betweeness */


	/*If a target file is provided compare predictions and targets*/
	if(tflg){
		unlabeledPerf = PerfMeasures_alloc(nbClass+1);
		computeUnlabeledPerf(preds,classes[0],nbUnlabeled,nbClass,inputParams,unlabeledPerf,inputParams->verbose);
		free_PerfMeasures(unlabeledPerf,nbClass+1);
	}
    /*Print informations*/
    if (inputParams->verbose >= 1){
		for(i=0;i<nbClass;i++)
			printf("Exploration rate in class %2i : %1.2f %%\n",i+1,100*dwInfo->mass_abs[i]);
		printf("CPU Time (sec)               : %1.4f\n",((double)(stop_cpu - start_cpu)/CLOCKS_PER_SEC));
		printLineDelim();	
    }

	/*Optionally release memory here*/
	#ifdef FREE_MEMORY_AT_END
		free_classes(classes,nbClass+1);
		free_InputParams(inputParams);
		free_DataStat(graphStat);
		free_DWInfo(dwInfo);
		free(absorbing);
		free(labels);
		free_mat(N,nbUnlabeled);
		free_mat(latF,inputParams->wlen+1);
		free_mat(latB,inputParams->wlen+1);
		free_SparseMatrix(spmat);
		free(optiLen);
		free(preds);
		if(kflg) free(gram);
	#endif

    /*Exit successfully :-)*/
    exit(EXIT_SUCCESS);
}
