#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

/*
This program is supposed to complete the Two-Phase broadcast. 
However, it only goes through the first phase and doesn't complete the second one.
Later on we fixed this by synchronizing the processors after the last loop.

*/

int * createArray(int pID, int size) {
	int *dataAddress = calloc(size, sizeof(int));
	for(int i = pID*size; i < (pID+1)*size; i++){
		dataAddress[i-pID*size] = i;
	}
	
	for(int i = pID*size; i < (pID+1)*size; i++){
		int temp = dataAddress[i-pID*size];
		int randomIndex = rand()%size;
		
		dataAddress[i-pID*size] = dataAddress[randomIndex];
		dataAddress[randomIndex] = temp;
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

int cmpfunc(const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}

void  sort(){
	bsp_begin(P);
	
	//Array Creation
	int p = bsp_pid();
	int size = 7;
	int *arr;
	int *LS; 
	arr = createArray(p,size);
	bsp_push_reg(LS,P*size*sizeof(int));
	bsp_sync();
	
	// Local Sort
	qsort(arr, size, sizeof(int), cmpfunc);
	toString(p,arr,size);
	//Storing and sharing Local Sample
	bsp_sync();
	
	for(int i = 0; i < P; i++){
		bsp_put(i,&arr[0],LS,p*size*sizeof(int),size*sizeof(int));
	}
	toString(p,LS,20);
	
	/*
	bsp_sync();
	
	/*
	bsp_sync();
	
	toString(p,LS,P*(P+1));
	*/
	
	bsp_end();
}
	
int main( int argc, char ** argv ) {
	printf( "How many threads do you want started? There are %d cores available. (Max 4 cores)\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	bsp_init( &sort, argc, argv );
	sort();
    return EXIT_SUCCESS;
}