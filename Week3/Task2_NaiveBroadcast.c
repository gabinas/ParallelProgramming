
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

void naiveBroadcast(){
	// 1
	bsp_begin(P);
	int n = 10;	// size of arrays
	int p = bsp_pid();
	int *arr;		//Array for process 0
	int *Broad;		//Array for the rest processors 
	if(p == 0){
		arr = createArray(n);	//Populating array in p0
	}
	Broad = calloc(n, sizeof(int));		//Saving space for array in p1-pn
	bsp_push_reg(arr, n*sizeof(int));	//Storing register for dynamic access		
	bsp_sync();	//first synchronize
	
	// 2
	/*
	if(p == 0){
		for(int i = 1; i < P; i++){
			bsp_put(i,arr, Broad,0, n*sizeof(int));
		}
	} */
	if(p!=0){
		bsp_get(0,arr,0,Broad,n*sizeof(int));
	}
	bsp_sync();
		
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
    bsp_init( &naiveBroadcast, argc, argv );
	naiveBroadcast();
    return EXIT_SUCCESS;
}