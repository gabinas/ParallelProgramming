#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int size) {
	int m = size;
	int data[m];
	int *dataAddress = data;
	int i;
	for (i = 0; i < m; i++){
	int num = rand()%135;
	data[i] = num;
		
}
return dataAddress;
	
}
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

void  largestNum(){
	int size = rand()%15;
	bsp_begin(P);
	
	int *Maxs = calloc(bsp_nprocs(), sizeof(int));
	bsp_push_reg(Maxs, bsp_nprocs() * sizeof(int));
	int s = bsp_pid();
	int *dataRecieved;
	dataRecieved = createArray(size);
	
	toString(s, dataRecieved, size);
	
	int max = findMax(dataRecieved, size);
	
	bsp_sync();
	
	// 2 
	Maxs[s] = max;
	
	bsp_put(0,&Maxs[s], Maxs, s*sizeof(int) , sizeof(int));
	bsp_sync();
	
	if(s==0){
		printf("The max number is: %d \n",findMax(Maxs,bsp_nprocs()));
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
	bsp_init( &largestNum, argc, argv );
	largestNum();
    return EXIT_SUCCESS;
}