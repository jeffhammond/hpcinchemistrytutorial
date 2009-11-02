/***************************************************************************
 *   Copyright (C) 2009 by Jeff Hammond                                    *
 *   jeff.science@gmail.com                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "dcmf.h"
#include "mpi.h"

/***************************************************************/

#define COMPILE_STAMP \
    if (me == 0){ \
        fprintf(stdout, "__FILE__ was compiled on __DATE__ at __TIME__ \n" ); \
    }

#define LINE_STAMP \
    if (me == 0){ \
        fprintf(stdout, "__LINE__ of __FILE__  \n" ); \
    }

#define OUTPUT( string ) \
    if (me == 0){ \
        fprintf(stdout, string ); \
    }

#define OUTPUT2( string1 , string2 ) \
    if (me == 0){ \
        fprintf(stdout, string1 , string2 ); \
    }

/***************************************************************/

#ifdef HPM_PROFILING
    void HPM_Init( void );
    void HPM_Start( char* );
    void HPM_Stop( char* );
    void HPM_Print( void );
    void HPM_Print_Flops( void );
    void HPM_Print_Flops_Agg( void );
    void HPM_Flops( char* , double* , double* );
#endif

/***************************************************************/

unsigned long long getticks(void)
{
     unsigned int rx, ry, rz;
     unsigned long long r64;

     do
     {
         asm volatile ( "mftbu %0" : "=r"(rx) );
         asm volatile ( "mftb %0" : "=r"(ry) );
         asm volatile ( "mftbu %0" : "=r"(rz) );
     }
     while ( rx != rz );

     r64 = rx;
     r64 = ( r64 << 32 ) | ry;
     
     return r64;
}

/***************************************************************/

// possible values of rma_optype
#define RMA_GET 1
#define RMA_PUT 2
#define RMA_ACC_SUM  4
#define RMA_ACC_PROD 5
#define RMA_ACC_LXOR 6

// possible values of rma_attributes
// bit field AOBR (atomic, ordered, blocking, remote completion)
#define RMA_ATTR_NONE 0
#define RMA_ATTR_AOBR 15

/***************************************************************/

     void function(void* clientdata, DCMF_Error_t* err)
     {
         fprintf( stdout , "Put is done\n" );
         *((int*)clientdata) = 1;
     }

/***************************************************************/

     typedef struct {
         void* buffer;
         size_t sizetype;
         DCMF_Memregion_t memregion;
     } RMA_Memregion;

     typedef struct {
         volatile int done;
     } RMA_Request;

    void fake_MPI_RMA_Memregion_create( RMA_Memregion* mem );

    void fake_MPI_RMA_Memregion_destroy( RMA_Memregion* mem );

    void fake_MPI_RMA_Memregion_exchange( RMA_Memregion* local ,
                                          RMA_Memregion* remote ,
                                          int rank );

    int datatype_check( MPI_Datatype origin_datatype , 
                        MPI_Datatype target_datatype ,
                        MPI_Comm comm , 
                        int rank );

    void fake_MPI_RMA_xfer( int rma_optype , 
                            void* origin_addr , 
                            int origin_count ,
                            MPI_Datatype origin_datatype , 
                            RMA_Memregion* target_mem , 
                            int target_disp , 
                            int target_count ,
                            MPI_Datatype target_datatype , 
                            int target_rank , 
                            MPI_Comm comm , 
                            int RMA_Attributes ,
                            RMA_Request* request );

    void fake_MPI_RMA_putb( void* origin_addr , 
                            int count,
                            RMA_Memregion* target_addr , 
                            int target_disp , 
                            int target_rank , 
                            MPI_Comm comm , 
                            int RMA_Attributes ,
                            RMA_Request* request );

    void fake_MPI_RMA_getb( void* origin_addr , 
                            int count,
                            RMA_Memregion* target_addr , 
                            int target_disp , 
                            int target_rank , 
                            MPI_Comm comm , 
                            int RMA_Attributes ,
                            RMA_Request* request );

/***************************************************************/

int main(int argc, char **argv)
{
    int me, nproc;
    unsigned long long t0,t1;

    void* origin_addr ;
    int origin_count ;
    MPI_Datatype origin_datatype ;
    RMA_Memregion* target_mem ;
    int target_disp ;
    int target_count ;
    MPI_Datatype target_datatype ;
    int target_rank ;
    MPI_Comm comm ;
    RMA_Request* request ;

    MPI_Init(&argc, &argv);

    COMPILE_STAMP

#ifdef HPC_PROFILING
    HPM_Init();
    HPM_Start("all");
#endif

/***************************************************************/

    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&me);

    OUTPUT2( "me    = %d\n" , me );
    OUTPUT2( "nproc = %d\n" , nproc );

    int version = { ( argc > 1 ) ? ( atoi(argv[1]) ) : ( 1 ) };
    if ( version == 1 )
    {
        OUTPUT("testing Vinod's rich RMA interface\n");
    }
    else if ( version == 2 )
    {
        OUTPUT("testing Jeff's minimalist RMA interface\n");
    }
    else
    {
        OUTPUT("you must choose 1 or 2!\n");
    }
 
    LINE_STAMP

/***************************************************************/

    t0 = getticks();
    fake_MPI_RMA_xfer( RMA_PUT , 
                       origin_addr , 
                       origin_count ,
                       origin_datatype , 
                       target_mem , 
                       target_disp , 
                       target_count ,
                       target_datatype , 
                       target_rank , 
                       comm , 
                       RMA_ATTR_NONE ,
                       request );
    t1 = getticks();

    LINE_STAMP

/***************************************************************/

#ifdef HPC_PROFILING
    HPM_Stop("all");
    HPM_Print();
    HPM_Print_flops();
#endif

    MPI_Finalize();

    return(0);
}

/***************************************************************/

    void fake_MPI_RMA_Memregion_create( RMA_Memregion* mem )
    {
        RMA_Memregion local;
        local = *mem;        
        size_t bytes_out;
        DCMF_Memregion_create(&local.memregion, &bytes_out, local.sizetype, local.buffer, 0);
        if ( bytes_out != local.sizetype )
        {
            fprintf( stdout , "DCMF_Memregion_create failed" );
            MPI_Abort( MPI_COMM_WORLD , 911 );
        }
    }

    void fake_MPI_RMA_Memregion_destroy( RMA_Memregion* mem )
    {
        RMA_Memregion local;
        local = *mem;        
        DCMF_Memregion_destroy(&local.memregion);
    }

    void fake_MPI_RMA_Memregion_exchange( RMA_Memregion* local ,
                                          RMA_Memregion* remote ,
                                          int rank )
    {
        MPI_Status status;
        MPI_Sendrecv(&local,  sizeof(local), MPI_CHAR, rank, 0,
                     &remote, sizeof(remote), MPI_CHAR, rank, 0,
                     MPI_COMM_WORLD, &status);
    }

    int datatype_check( MPI_Datatype origin_datatype ,
                        MPI_Datatype target_datatype ,
                        MPI_Comm comm , 
                        int remote_rank)
    {
        fprintf( stdout , "datatype_check not implemented" );
        MPI_Abort( comm , 911 );
        return(0);
    }

/***************************************************************/

    void fake_MPI_RMA_xfer( int rma_optype , 
                            void* origin_addr , 
                            int origin_count ,
                            MPI_Datatype origin_datatype , 
                            RMA_Memregion* target_addr , 
                            int target_disp , 
                            int target_count ,
                            MPI_Datatype target_datatype , 
                            int target_rank , 
                            MPI_Comm comm , 
                            int RMA_Attributes ,
                            RMA_Request* request )
    {

        int result;

        if ( origin_datatype != target_datatype && target_datatype != MPI_DATATYPE_NULL )
        {
            result = datatype_check( origin_datatype , target_datatype , comm , target_rank );
            if ( result != 0 )
            {
                fprintf( stdout , "origin_datatype does not match target_datatype" );
                MPI_Abort( comm , 911 );
            }
        }

        switch(rma_optype)
        {
            case RMA_PUT:
                
                break;

            case RMA_GET:
                fprintf( stdout , "RMA_GET not implemented" );
                MPI_Abort( comm , 911 );
                break;

            case RMA_ACC_SUM:
                fprintf( stdout , "RMA_ACC_SUM not implemented" );
                MPI_Abort( comm , 911 );
                break;

            case RMA_ACC_PROD:
                fprintf( stdout , "RMA_ACC_PROD not implemented" );
                MPI_Abort( comm , 911 );
                break;

            case RMA_ACC_LXOR:
                fprintf( stdout , "RMA_ACC_LXOR not implemented" );
                MPI_Abort( comm , 911 );
                break;

            default:
                fprintf( stdout , "Invalid rma_optype" );
                MPI_Abort( comm , 911 );
                break;
        }
        

    }

/***************************************************************/

    void fake_MPI_RMA_putb( void* origin_addr , 
                            int count,
                            RMA_Memregion* target_addr , 
                            int target_disp , 
                            int target_rank , 
                            MPI_Comm comm , 
                            int RMA_Attributes ,
                            RMA_Request* request )
    {

    }

