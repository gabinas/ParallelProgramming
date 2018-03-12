#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int size) {
	int *dataAddress = calloc(size, sizeof(int));
	for (int i = 0; i < size; i++){
		int num = rand()%50;
		dataAddress[i] = num;
	}
	return dataAddress;	
}

void  toString(int pID, int data[], int size){
	printf( "\nArray in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
}

void  twoPhase(){
	bsp_begin(P);
	
	//0
	int p = bsp_pid();
	int n = 8;
	int *arr;
	arr = calloc( n, sizeof(int));	//All arrays allocate space for array
	if(p == 0){
		arr = createArray(n);	//Process 0 stores random data
	}
	bsp_push_reg(arr, n*sizeof(int));	//Registers are set up
	
	bsp_sync();
	
	
	//1: First Phase
	int blocksize = 2;
	if(p == 0){	//Process 0 sends a block of size n/p to each process
		for(int i = 0; i < P; i++){
			bsp_put(i,&arr[i*blocksize],arr,i*blocksize*sizeof(int),blocksize*sizeof(int));
		}
	}
	
	bsp_sync();
	
	//Second Phase
	for(int i = 0; i < P; i++){
		bsp_put(i,&arr[p*blocksize],arr,p*blocksize*sizeof(int),blocksize*sizeof(int)); 
	}
	
	bsp_sync();
	
	toString(p,arr,n);
	printf("\n");
		
	bsp_end();
}
	
int main( int argc, char ** argv ) {
	printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	bsp_init( &twoPhase, argc, argv );
	twoPhase();
    return EXIT_SUCCESS;
}