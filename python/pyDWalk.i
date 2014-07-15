/* pyDWalk.i */

%module pyDWalk

%{
	#define SWIG_FILE_WITH_INIT
	#include "../Include/dwalk.h"
%}

%include "numpy.i"

%init %{
	import_array();
%}


%apply (double* IN_ARRAY2, int DIM1, int DIM2){(double* adj, int na, int ma)};
%apply (int* IN_ARRAY1, int DIM1) {(int* target, int nt)};
%apply (double* INPLACE_ARRAY2, int DIM1, int DIM2) {(double* bet, int nb, int mb)};

%include "../Include/dwalk.h"

%rename(dwalkLauncher) my_dwalkLauncher;

%inline %{
	void my_dwalkLauncher(double* adj, int na, int ma, int* target, int nt, int nbClass, int wlen, double* bet, int nb, int mb) {
		dwalkLauncher(adj, na, ma, target, nt, nbClass, wlen, bet);
	}
%}
