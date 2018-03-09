#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

void  toString(int pID, int data[], int size){
	//int size = sizeof(data)/sizeof(int);
	printf( "\nArray from processor %d,size= %d. \n", pID, size);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}

}
//Find Max number from data[]
int findMax(int data[], int size){
	int max = data[0];
	int i;
	for (i = 0; i < size; i++){
		if (data[i] > max){
			max = data[i];
		}
	}
	return max;
}


void unitTest(){
	bsp_begin(P);
	
	int *Maxs = calloc(bsp_nprocs(), sizeof(int));
	bsp_push_reg(Maxs, bsp_nprocs() * sizeof(int));
	
	int s = bsp_pid();
	int size = 3;
	int dataRecieved[size];
	
	switch(s){
		case 0: 
		dataRecieved[0] = 5;
		dataRecieved[1] = 2;
		dataRecieved[2] = 5;
		break;
		case 1:
		dataRecieved[0] = 6;
		dataRecieved[1] = 6;
		dataRecieved[2] = 7;
		break;
		case 2: 
		dataRecieved[0] = 7;
		dataRecieved[1] = 8;
		dataRecieved[2] = 8;
		break;
		case 3: 
		dataRecieved[0] = 9;
		dataRecieved[1] = 3;
		dataRecieved[2] = 5;
		break;
	}
	
	toString(s, dataRecieved, size);
	
	int max = findMax(dataRecieved, size);
	
	bsp_sync();
	
	// 2 
	Maxs[s] = max;
	
	bsp_put(0,&Maxs[s], Maxs, s*sizeof(int) , sizeof(int));
	bsp_sync();
	
	int n = findMax(Maxs,bsp_nprocs());
	if(s==0){
		printf("\nThe max number is: %d \n",n);
	}
	
	bsp_pop_reg(&Maxs);
	
		
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
	bsp_init( &unitTest, argc, argv );
	unitTest();
    return EXIT_SUCCESS;
}