/*************************************************************************/
/* Author      : Kevin Françoisse (kfrancoi@gmail.com)                   */
/* Version     : 1.00                                                    */
/* Modified    : 29/03/2009                                              */
/*************************************************************************/

#ifndef PYDWALK_H
#define PYDWALK_H

#include "tools.h"

/*The discriminative random walks for every node, labeled node as well, to get a betweeness vector for each node
for each class
*/
int dwalkLauncher(double* adj, int na, int ma, int* target, int nt, int nbClass, int wlen, double* bet);

#endif