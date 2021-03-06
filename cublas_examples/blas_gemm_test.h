/***************************************************************************

                  COPYRIGHT

The following is a notice of limited availability of the code, and disclaimer
which must be included in the prologue of the code and in all source listings
of the code.

Copyright Notice
 + 2009 University of Chicago

Permission is hereby granted to use, reproduce, prepare derivative works, and
to redistribute to others.  This software was authored by:

Jeff R. Hammond
Leadership Computing Facility
Argonne National Laboratory
Argonne IL 60439 USA
phone: (630) 252-5381
e-mail: jhammond@mcs.anl.gov

                  GOVERNMENT LICENSE

Portions of this material resulted from work developed under a U.S.
Government Contract and are subject to the following license: the Government
is granted for itself and others acting on its behalf a paid-up, nonexclusive,
irrevocable worldwide license in this computer software to reproduce, prepare
derivative works, and perform publicly and display publicly.

                  DISCLAIMER

This computer code material was prepared, in part, as an account of work
sponsored by an agency of the United States Government.  Neither the United
States, nor the University of Chicago, nor any of their employees, makes any
warranty express or implied, or assumes any legal liability or responsibility
for the accuracy, completeness, or usefulness of any information, apparatus,
product, or process disclosed, or represents that its use would not infringe
privately owned rights.

 ***************************************************************************/

#ifndef BLAS_GEMM_TEST_H
#define BLAS_GEMM_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef BLAS_USES_LONG
  #define BLAS_INT long
//   #ifdef MKL
//     #include "mkl.h"
//   #else
    void sgemm_(char* , char* ,long* , long* , long* , float* , float* , long* , float* , long* , float* , float* , long* );
    void dgemm_(char* , char* ,long* , long* , long* , double*, double*, long* , double*, long* , double*, double*, long* );
//   #endif
#else
  #define BLAS_INT int
//   #ifdef MKL
//     #include "mkl.h"
//   #else
    void sgemm_(char* , char* ,int* , int* , int* , float* , float* , int* , float* , int* , float* , float* , int* );
    void dgemm_(char* , char* ,int* , int* , int* , double*, double*, int* , double*, int* , double*, double*, int* );
//   #endif
#endif

#ifdef OPENMP
  #include "omp.h"
#endif

#include "blas_utils.h"

void run_blas_sgemm_test(int dim, float  alpha, float  beta, double* time, double* Gflops);
void run_blas_dgemm_test(int dim, double alpha, double beta, double* time, double* Gflops);
void run_blas_sgemm_test2(int dim1, int dim2, int dim3, float  alpha, float  beta, double* time, double* Gflops);
void run_blas_dgemm_test2(int dim1, int dim2, int dim3, double alpha, double beta, double* time, double* Gflops);

#endif


