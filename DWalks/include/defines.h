/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#ifndef DEFINES_H
#define DEFINES_H

/***************************SPECIAL MODES********************************/

/*ENABLE EXPERIMENTAL SECTIONS*/
/*#define ENABLE_EXPERIMENTAL*/

/*FREE MEMORY AT THE END OF EXECUTION (TO CHECK LEAKS)*/
/*#define FREE_MEMORY_AT_END*/

/****************************TYPEDEF SECTION*****************************/

/*TYPE DEFINITION FOR NUMBERS*/
typedef float real; 
typedef unsigned char uchar;

/***************************CONSTANT DEFINITIONS*************************/

#define MAX_LINES          10000000 /*10 millions*/			   /* Maximum number of nodes */
#define MAX_NODES		   10000000
#define BUF_LINE_LEN       2097152  /*2MB*/                    /* Size of the buffer for file reading */
#define FNAME_LEN          1000								   /* Maximum size of file names*/ 
#define PROB_EPS           1e-4								   /* Precision to check if probas sum up to 1*/
#define DEGREE_OVERFLOW    10                                  /* Factor used in graph generation for the heap size*/
#define DEF_WALK_LEN       20                                  /* Default random walk length*/
#define DEF_NB_FOLDS       10                                  /* Default number of folds for cross-validation*/
#define DEF_CROSSWALKS     0                                   /* Default usage of the cross-walks*/
#define DEF_VERBOSE        1								   /* Default verbosity of the software */
#define DEF_CV_SEED       -1								   /* Default seed for shuffling in cross-validation */  
#define DEF_PERF_MEASURE   0                                   /* Default performance measure*/
#define DEF_MAX_ITER       100                                 /* Default maximum number of graph optimizations*/
#define DEF_NODE_DEGREE    2                                   /* Default node degree*/
#define DEF_PROBE_FACTOR   10     							   /* Default factor for similarity evaluations*/
#define DEF_FEAT_NORMALIZE 1                                   /* Default normalization of the columns*/
#define DEF_SIMILARITY     1                                   /* Default similarity for edge weghts (dot product)*/   
#define VERSION           "1.02"							   /* Current software version*/
#define COPYRIGHT         "J.Callut and K.Francoisse (C) 2008" /* Copyright notice*/
#ifdef WIN32											       /* Platform on which the program is compiled*/
	#define PLATFORM "WIN32"
#else
	#define PLATFORM "UNIX"
#endif
#define	BUILD_NOTE     ""
#define DISCLAIMER	   ""	

/***********************STANDARD FUNCTION REMAPPINGS***********************/

#ifdef WIN32											
	#define __strdup _strdup
#else
	#define __strdup strdup
#endif

/*******************************MACROS************************************/

/*ARITHMETIC MACROS*/
#define LOG2(x) (log10((real)(x))/(3.010299956640e-01))
#define POW2(x) ((x)*(x))
#define MIN(x,y)((x) < (y) ? (x)  : (y))
#define MAX(x,y)((x) > (y) ? (x)  : (y))
#define ABS(x)  (((x) < 0) ? -(x) : (x))

/*MATRIX MACROS*/
#define getRow(spmat,j) (spmat)->rows[(j)]
#define getCol(spmat,j) (spmat)->cols[(j)]
#define K(i,j) (j <= i) ? gram[(((i)*((i)+1))/2)+(j)] : gram[(((j)*((j)+1))/2)+(i)]

/*MISC MACROS*/
#define REMAP_M1P1(y) {if((y) == -1) (y) = 1; else if ((y) == 1) (y) = 2; else (y) = 0;}
#define SET_SIM(_idx,_val,_sim){(_sim).idx = (_idx); (_sim).val = (_val);}

/*************************PERFORMANCE MEASURE******************************/

#define PERF_CLASS_RATE 0
#define PERF_MACRO_PREC 1
#define PERF_MACRO_REC  2
#define PERF_MACRO_F1   3
#define PERF_MICRO_PREC 4
#define PERF_MICRO_REC  5
#define PERF_MICRO_F1   6

/********************************OPENMP***********************************/

/*OPENMP SETTINGS*/
#ifdef _OPENMP
	#include <omp.h>
	#define OPENMP_STR    " using OpenMP"
	#define PAR_PROBE_SIM 0
	#define PAR_FW_BW     1
	#define PAR_FW_LAT    1
	#define PAR_BW_LAT    1
	#define PAR_UNLABELED 1
	#define nCPU()        8 
#else
	#define OPENMP_STR    ""
	#define omp_get_thread_num() 0
#endif

/***************************TEXT FORMATING********************************/

/*ENABLE COLOR ON THE TERM*/
/*#define ENABLE_TEXT_COLOR*/

/*USEFUL CONSTANT STRINGS*/
#define TRAILING_BAR "---------------------------------------------------------------------------------"
#define SPACER       "        "
#define TAB          "      "

/*SPECIAL TERMINAL COMMANDS*/
#define RESET		 0
#define BRIGHT 		 1
#define DIM		     2
#define UNDERLINE 	 3
#define BLINK		 4
#define REVERSE		 7
#define HIDDEN		 8
#define BLACK 		 0
#define RED		     1
#define GREEN		 2
#define YELLOW		 3
#define BLUE		 4
#define MAGENTA		 5
#define CYAN		 6
#define	WHITE		 7

/***************************RESTRICTED BUILDS*****************************/

#endif
