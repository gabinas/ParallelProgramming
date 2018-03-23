#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;
unsigned int test[16];//TEST: initializing test array
/*
This program is supposed to complete the Two-Phase broadcast. 
However, it only goes through the first phase and doesn't complete the second one.
Later on we fixed this by synchronizing the processors after the last loop.
*/

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
	int n = 16;
	int *arr;
	arr = calloc( n, sizeof(int));	//All arrays allocate space for array
	if(p == 0){
		arr = createArray(n);	//Process 0 stores random data
		
		for(int k = 0; k < n; k++){//TEST:copies the array in process 0 to a test array 
			test[k] = arr[k];		
		}
	}
	bsp_push_reg(arr, n*sizeof(int));	//Registers are set up
	
	bsp_sync();
	
	
	//1: First Phase
	int blocksize = n/P;
	if(p == 0){	//Process 0 sends a block of size n/p to each process
		for(int i = 0; i < P; i++){
			bsp_put(i,&arr[i*blocksize],arr,i*blocksize*sizeof(int),blocksize*sizeof(int));
		}
	}
	
	bsp_sync();
	
	//Second Phase
	for(int i = 0; i < P; i++){	//Each process sends the block of size n/p to all other processes
		bsp_put(i,&arr[p*blocksize],arr,p*blocksize*sizeof(int),blocksize*sizeof(int)); 
	}
	
	bsp_sync();
	
		//TEST
		if(p!=0){
		printf("P = %d\n", p);
		printf("T A%d\n", p);
		for(int i = 0; i < n; i++){
		if(test[i] == arr[i]){//TEST:compares the test array with the arrays in the processors
		printf("%d %d correct\n", test[i], arr[i]);
		}else{
		printf("%d %d incorrect\n",test[i], arr[i]);
		}
	}	
}



printf("\n");
		
	bsp_end();
}
	
int main( int argc, char ** argv ) {
	printf( "How many threads do you want started? There are %d cores available. (Only multiples of 8)\n", bsp_nprocs() );
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

