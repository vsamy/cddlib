/* automatically generated by sed scripts from the c source named below: *//* cddproj.c:  Polyhedral Projections in cddlib
   written by Komei Fukuda, fukuda@cs.mcgill.ca
   Version 0.93, July 18, 2003
*/

/* cddlib : C-library of the double description method for
   computing all vertices and extreme rays of the polyhedron 
   P= {x :  b - A x >= 0}.  
   Please read COPYING (GNU General Public Licence) and
   the manual cddlibman.tex for detail.
*/

#include "setoper.h"  /* set operation library header (Ver. June 1, 2000 or later) */
#include "cdd_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

ddf_MatrixPtr ddf_BlockElimination(ddf_MatrixPtr M, ddf_colset delset, ddf_ErrorType *error)
/* Eliminate the variables (columns) delset by
   the Block Elimination with ddf_DoubleDescription algorithm.

   Given (where y is to be eliminated):
   c1 + A1 x + B1 y >= 0
   c2 + A2 x + B2 y =  0

   1. First construct the dual system:  z1^T B1 + z2^T B2 = 0, z1 >= 0.
   2. Compute the generators of the dual.
   3. Then take the linear combination of the original system with each generator.
   4. Remove redundant inequalies.

*/
{
  ddf_MatrixPtr Mdual=NULL, Mproj=NULL, Gdual=NULL;
  ddf_rowrange i,h,m,mproj,mdual,linsize;
  ddf_colrange j,k,d,dproj,ddual,delsize;
  ddf_colindex delindex;
  myfloat temp,prod;
  ddf_PolyhedraPtr dualpoly;
  ddf_ErrorType err=ddf_NoError;
  ddf_boolean localdebug=ddf_TRUE;

  *error=ddf_NoError;
  m= M->rowsize;
  d= M->colsize;
  delindex=(long*)calloc(d+1,sizeof(long));
  ddf_init(temp);
  ddf_init(prod);

  k=0; delsize=0;
  for (j=1; j<=d; j++){
    if (set_member(j, delset)){
      k++;  delsize++;
      delindex[k]=j;  /* stores the kth deletion column index */
    }
  }
  if (localdebug) ddf_WriteMatrix(stdout, M);

  linsize=set_card(M->linset);
  ddual=m+1;
  mdual=delsize + m - linsize;  /* #equalitions + dimension of z1 */

  /* setup the dual matrix */
  Mdual=ddf_CreateMatrix(mdual, ddual);
  Mdual->representation=ddf_Inequality;
  for (i = 1; i <= delsize; i++){
    set_addelem(Mdual->linset,i);  /* equality */
    for (j = 1; j <= m; j++) {
      ddf_set(Mdual->matrix[i-1][j], M->matrix[j-1][delindex[i]-1]);
    }
  } 

  k=0;
  for (i = 1; i <= m; i++){
    if (!set_member(i, M->linset)){
      /* set nonnegativity for the dual variable associated with
         each non-linearity inequality. */
      k++;
      ddf_set(Mdual->matrix[delsize+k-1][i], ddf_one);  
    }
  } 
  
  /* 2. Compute the generators of the dual system. */
  dualpoly=ddf_DDMatrix2Poly(Mdual, &err);
  Gdual=ddf_CopyGenerators(dualpoly);

  /* 3. Take the linear combination of the original system with each generator.  */
  dproj=d-delsize;
  mproj=Gdual->rowsize;
  Mproj=ddf_CreateMatrix(mproj, dproj);
  Mproj->representation=ddf_Inequality;
  set_copy(Mproj->linset, Gdual->linset);

  for (i=1; i<=mproj; i++){
    k=0;
    for (j=1; j<=d; j++){
      if (!set_member(j, delset)){
        k++;  /* new index of the variable x_j  */
        ddf_set(prod, ddf_purezero);
        for (h = 1; h <= m; h++){
          ddf_mul(temp,M->matrix[h-1][j-1],Gdual->matrix[i-1][h]); 
          ddf_add(prod,prod,temp);
        }
        ddf_set(Mproj->matrix[i-1][k-1],prod);
      }
    }
  }
  if (localdebug) printf("Size of the projection system: %ld x %ld\n", mproj, dproj);
  
  ddf_FreePolyhedra(dualpoly);
  free(delindex);
  ddf_clear(temp);
  ddf_clear(prod);
  ddf_FreeMatrix(Mdual);
  ddf_FreeMatrix(Gdual);
  return Mproj;
}


ddf_MatrixPtr ddf_FourierElimination(ddf_MatrixPtr M,ddf_ErrorType *error)
/* Eliminate the last variable (column) from the given H-matrix using 
   the standard Fourier Elimination.
 */
{
  ddf_MatrixPtr Mnew=NULL;
  ddf_rowrange i,inew,ip,in,iz,m,mpos=0,mneg=0,mzero=0,mnew;
  ddf_colrange j,d,dnew;
  ddf_rowindex posrowindex, negrowindex,zerorowindex;
  myfloat temp1,temp2;
  ddf_boolean localdebug=ddf_TRUE;

  *error=ddf_NoError;
  m= M->rowsize;
  d= M->colsize;
  if (d<=1){
    *error=ddf_ColIndexOutOfRange;
    if (localdebug) {
      printf("The number of column is too small: %ld for Fourier's Elimination.\n",d);
    }
    goto _L99;
  }

  if (M->representation==ddf_Generator){
    *error=ddf_NotAvailForV;
    if (localdebug) {
      printf("Fourier's Elimination cannot be applied to a V-polyhedron.\n");
    }
    goto _L99;
  }

  if (set_card(M->linset)>0){
    *error=ddf_CannotHandleLinearity;
    if (localdebug) {
      printf("The Fourier Elimination function does not handle equality in this version.\n");
    }
    goto _L99;
  }

  /* Create temporary spaces to be removed at the end of this function */
  posrowindex=(long*)calloc(m+1,sizeof(long));
  negrowindex=(long*)calloc(m+1,sizeof(long));
  zerorowindex=(long*)calloc(m+1,sizeof(long));
  ddf_init(temp1);
  ddf_init(temp2);

  for (i = 1; i <= m; i++) {
    if (ddf_Positive(M->matrix[i-1][d-1])){
      mpos++;
      posrowindex[mpos]=i;
    } else if (ddf_Negative(M->matrix[i-1][d-1])) {
      mneg++;
      negrowindex[mneg]=i;
    } else {
      mzero++;
      zerorowindex[mzero]=i;
    }
  }  /*of i*/

  if (localdebug) {
    ddf_WriteMatrix(stdout, M);
    printf("No of  (+  -  0) rows = (%ld, %ld, %ld)\n", mpos,mneg, mzero);
  }

  /* The present code generates so many redundant inequalities and thus
     is quite useless, except for very small examples
  */
  mnew=mzero+mpos*mneg;  /* the total number of rows after elimination */
  dnew=d-1;

  Mnew=ddf_CreateMatrix(mnew, dnew);
  ddf_CopyArow(Mnew->rowvec, M->rowvec, dnew);
/*  set_copy(Mnew->linset,M->linset);  */
  Mnew->numbtype=M->numbtype;
  Mnew->representation=M->representation;
  Mnew->objective=M->objective;


  /* Copy the inequalities independent of x_d to the top of the new matrix. */
  for (iz = 1; iz <= mzero; iz++){
    for (j = 1; j <= dnew; j++) {
      ddf_set(Mnew->matrix[iz-1][j-1], M->matrix[zerorowindex[iz]-1][j-1]);
    }
  } 

  /* Create the new inequalities by combining x_d positive and negative ones. */
  inew=mzero;  /* the index of the last x_d zero inequality */
  for (ip = 1; ip <= mpos; ip++){
    for (in = 1; in <= mneg; in++){
      inew++;
      ddf_neg(temp1, M->matrix[negrowindex[in]-1][d-1]);
      for (j = 1; j <= dnew; j++) {
        ddf_LinearComb(temp2,M->matrix[posrowindex[ip]-1][j-1],temp1,\
          M->matrix[negrowindex[in]-1][j-1],\
          M->matrix[posrowindex[ip]-1][d-1]);
        ddf_set(Mnew->matrix[inew-1][j-1],temp2);
      }
      ddf_Normalize(dnew,Mnew->matrix[inew-1]);
    }
  } 


  free(posrowindex);
  free(negrowindex);
  free(zerorowindex);
  ddf_clear(temp1);
  ddf_clear(temp2);

 _L99:
  return Mnew;
}


/* end of cddproj.c */
