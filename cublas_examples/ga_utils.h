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

#ifndef GA_UTILS_H
#define GA_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#ifdef GA
  #include "macdecls.h"
  #include "../armci/src/armci.h"
  #include "sndrcv.h"
  #include "ga.h"
#endif

#ifdef MPI
  #include "mpi.h"
#endif

int cuda_active;

void print_hostname(int printMask);

int parallel_nproc(void);
int parallel_me(void);
void parallel_sync(void);

void start_parallel(int* argc, char*** argv, int* me, int* nproc, int armci_not_ga, int use_cuda);
void stop_parallel(int stats);

int alloc_global_2d(int precision, int rows, int cols, int printMask);
void zero_global(int g_a);
int clone_global(int g_in);
void copy_global(int g_in, int g_out);
void free_global(int g_a);
void randomize_global(int g_in);
void global_to_local(int g_in, void* l_out);
void local_to_global(void* l_in, int g_out);

#endif

