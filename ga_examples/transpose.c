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


#include "driver.h"

/***************************************************************************
 *                                                                         *
 * transpose:                                                                  *
 *       -demonstrates how to create a GA using the new API                *
 *       -matrix transposition									           *
 *                                                                         *
 ***************************************************************************/

void transpose_patch(double* input, double* output);

int transpose(int rank,int blksz)
{
	int me,nproc,ntask,t;
    int ii,jj;
    int i,j;
    int g_a,g_b,g_c,g_error; // GA handles
    int status;
    int ndim = 2;
//    int rank = 600;
//	  int blksz = 200;
    int dims[2];
    int chunk[2];
    int nblock;
    int lo_a[2],lo_c[2];
    int hi_a[2],hi_c[2];
    int rng_a[2];//,rng_c[2];
    int ld_a[1],ld_c[1];
    int pg_world;   // world processor group
    double alpha,beta,error;
    double start,finish;
    double t_a,t_c;
    double* p_in; // pointers for local access to GAs
    double* p_a;  // pointers for local access to GAs
    double* p_c;  // pointers for local access to GAs
    bool myturn;

    nproc=GA_Nnodes();
    me=GA_Nodeid();

    dims[0] = rank;
    dims[1] = rank;
    chunk[0] = -1;
    chunk[1] = -1;
    nblock = rank/blksz;

    pg_world = GA_Pgroup_get_world();

    g_a= GA_Create_handle();
    GA_Set_array_name(g_a,"matrix A");
    GA_Set_data(g_a,ndim,dims,MT_DBL);
    GA_Set_chunk(g_a,chunk);
    GA_Set_pgroup(g_a,pg_world);
    status = GA_Allocate(g_a);
    if(status == 0){
    	if (me == 0) printf("%s: GA_Allocate failed at line %d\n",__FILE__,__LINE__);
    };
    
    g_b  = GA_Duplicate(g_a,"matrix B");
    if(g_b == 0){
    	if (me == 0) printf("%s: GA_Duplicate failed at line %d\n",__FILE__,__LINE__);
    };

    g_c  = GA_Duplicate(g_a,"matrix C");
    if(g_c == 0){
    	if (me == 0) printf("%s: GA_Duplicate failed at line %d\n",__FILE__,__LINE__);
    };

    g_error  = GA_Duplicate(g_a,"error");
    if(g_error == 0){
    	if (me == 0) printf("%s: GA_Duplicate failed at line %d\n",__FILE__,__LINE__);
    };

#ifdef DEBUG
    GA_Zero(g_a);
    GA_Zero(g_b);
    GA_Zero(g_c);
    GA_Zero(g_error);
    if (me == 0) printf("\n");
    GA_Print_distribution(g_a);
    if (me == 0) printf("\n");
#endif

/*
 * begin initialization with random values using local access
 */

    NGA_Distribution(g_a,me,lo_a,hi_a);
    NGA_Access(g_a,lo_a,hi_a,&p_in,&ld_a[0]);

    rng_a[0] = hi_a[0] - lo_a[0] + 1;
    rng_a[1] = hi_a[1] - lo_a[1] + 1;

    double scale = 0.00001/sqrt(RAND_MAX);

    for(i=0; i<rng_a[0]; i++){
    	for(j=0; j<rng_a[1]; j++){
    		p_in[ ld_a[0] * i + j ] = (double) ( rand() * scale );
    	}
    }

    NGA_Release_update(g_a,lo_a,hi_a); /* this function does nothing as of GA 4.2 */

#ifdef DEBUG
	GA_Print(g_a);
#endif

/*
 * end initialization
 */

/*
 * GA reference matrix multiplication
 */
    start = clock();
    GA_Transpose(g_a,g_b);
    finish = clock();
    if (me == 0){
    	printf("GA_Transpose took %f seconds\n",(double) (finish - start) / CLOCKS_PER_SEC);
    }

/*
 * begin hand-written transposition
 */

    start = clock();

    ntask = nblock * nblock;

    if (me == 0) {
    	printf("ntask = %d\n",ntask);
    	printf("nproc = %d\n",nproc);
    }
	printf("proc %d is here\n",me);
	fflush(stdout);\

    p_a = (double *)ARMCI_Malloc_local((armci_size_t) blksz * blksz * sizeof(double));
    p_c = (double *)ARMCI_Malloc_local((armci_size_t) blksz * blksz * sizeof(double));

    t = 0;

    for (ii = 0 ; ii < nblock; ii++){
    	for (jj = 0 ; jj < nblock; jj++){

//    		printf("t mod nproc = %d\n",t % nproc);
//    		fflush(stdout);
    		myturn = ( me == ( t % nproc ) );

    		if (myturn){

#ifdef DEBUG
    			printf("proc %d doing work tuple (%d,%d)\n",me,ii,jj);
    			fflush(stdout);
#endif

    			lo_a[0] = blksz * ii;
    			hi_a[0] = blksz * (ii + 1) - 1;
    			lo_a[1] = blksz * jj;
    			hi_a[1] = blksz * (jj + 1) - 1;
    			ld_a[0] = blksz;

    			NGA_Get(g_a,lo_a,hi_a,p_a,ld_a);

    			/**************************************/

    			memset(p_c,0,blksz * blksz * sizeof(double));
    			if (ii == jj){
    				for (i = 0 ; i < blksz ; i++ ){
    					for (j = 0 ; j < blksz ; j++ ){
    						t_a = p_a[ blksz * i + j ];
    						t_c = p_c[ blksz * j + i ];
    						p_a[ blksz * i + j ] = t_c;
    						p_c[ blksz * j + i ] = t_a;
    					}
    				}
    			} else {
    				for (i = 0 ; i < blksz ; i++ ){
    					for (j = 0 ; j < blksz ; j++ ){
    						p_c[ blksz * j + i ] =  p_a[ blksz * i + j ];
    					}
    				}
    			}

    			/**************************************/

    			lo_c[0] = blksz * jj;
    			hi_c[0] = blksz * (jj + 1) - 1;
    			lo_c[1] = blksz * ii;
    			hi_c[1] = blksz * (ii + 1) - 1;
    			ld_c[0] = blksz;

    			NGA_Put(g_c,lo_c,hi_c,p_c,ld_c);

    		} // myturn

    		t += 1;

    	} // jj
    } // ii

    status = ARMCI_Free_local(p_c);
    if(status != 0){
    	if (me == 0) printf("%s: ARMCI_Free_local failed at line %d\n",__FILE__,__LINE__);
    };
    status = ARMCI_Free_local(p_a);
    if(status != 0){
    	if (me == 0) printf("%s: ARMCI_Free_local failed at line %d\n",__FILE__,__LINE__);
    };

    GA_Sync();

    finish = clock();
    if (me == 0){
    	printf("Hand-written parallel matmul took %f seconds\n",(double) (finish - start) / CLOCKS_PER_SEC);
    }

#ifdef DEBUG
	GA_Print(g_c);
#endif

/*
 * end hand-written transposition
 */

/*
 * begin error evaluation
 */

    alpha = 1.0;
    beta = -1.0;
    GA_Add(&alpha,g_b,&beta,g_c,g_error);

    GA_Norm1(g_error,&error);

    if (me == 0) printf("error = %f\n",error);


/*
 * end error evaluation
 */

    GA_Destroy(g_error);
    GA_Destroy(g_c);
    GA_Destroy(g_b);
    GA_Destroy(g_a);

    return(0);
}
