/*************************************************************************/
/* Author      : Jerome Callut (jcallut@gmail.com)                       */
/* Version     : 1.00                                                    */
/* Modified    : 14/02/2008                                              */
/*************************************************************************/

#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include <stdio.h>
#include "defines.h"
#include "matrix.h"
#include "tools.h"
#include "perfMeasure.h"

/*Change the terminal color and styles*/
void textcolor(int attr, int fg, int bg);

/* Display a vector of integers */
void show_int_vec(int* vec,int n);

/* Display a matrix of integers */
void show_int_mat(int** mat,int l,int c);

/* Display a vector of reals */
void show_vec(real* vec,int n);

/* Display a matrix of reals */
void show_int_mat(int** mat,int l,int c);

/* Display a matrix of reals*/
void show_mat(real** mat,int l,int c);

/* Display an array of SparseDims */
void showDims(SparseDim* sdim,int m,char mode);

/*Display the content of the classes*/
void show_Classes(int** classes,int nbClass);

/*Create a confusion matrix from prediction and targets*/
void show_ConfusionMatrix(int** confusion,int nbClass,FILE* output);

/*Print a delimiting line on standard output*/
void printLineDelim();

/*Print k backspaces on standard output*/
void backspace(int k);

/*Return the length of an integer while transformed to a string*/
int intLen(int k);

/*Print information about the input of the program (i.e. data + parameters)*/
void printInputInfo(SparseMatrix* spmat,DataStat* dataStat,int nbClass,InputParams* inputParams);

/*Print information about the input of the program (i.e. data + parameters)*/
void printLSVMInputInfo(SparseMatrix* spmat,LSVMDataInfo* dataInfo,DataStat* dataStat,InputParams* inputParams);

/*Print performance measures*/
void printPerfs(PerfMeasures* avg, PerfMeasures *dev);

#endif