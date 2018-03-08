
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
//This bug we found looking at the array arr before and after the sync, 
//displaying different values before and after. 
//It also does not send the same array to the other processors, but that is not the problem reported.
unsigned int P;

int * createArray(int size) {
	int data[size];
	int *dataAddress = data;
	int i;
	for (i = 0; i < size; i++){
		int num = rand()%80;
		data[i] = num;
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
	bsp_push_reg(Broad, n*sizeof(int));	//Storing register for dynamic access
	if(p == 0){	//We check what the array is before the sync
		toString(p,arr,n);
	}		
	bsp_sync();	//first synchronize
	if(p == 0){	//We check what the array is after the sync
		toString(p,arr,n);
	}
	// 2
	if(p == 0){
		for(int i = 1; i < P; i++){
			bsp_put(i,&arr, Broad,0, n*sizeof(int));
		}
	} 
	bsp_sync();
	
	toString(p,Broad,n);
	
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