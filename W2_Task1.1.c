#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
#include <time.h>
unsigned int P;

int * createArray(int size) {
int m = size;
int data[m];
int *dataAddress = data;
time_t t;
for (int i = 0; i < m; i++){
	 srand((unsigned) time(&t));
	int num = rand()%51;
	data[i] = num;
		
}
return dataAddress;
	
}
void  toString(int pID, int data[], int size){
	//int size = sizeof(data)/sizeof(int);
	printf( "\nArray from processor %d,size= %d. \n", pID, size);
	for (int i = 0; i < size; i++){
		printf("%d ",data[i]);
	}

}
//Find Max number from data[]
int findMax(int pID, int data[], int size){
	int max = data[0];
	for (int i = 0; i < size; i++){
		if (data[i] > max){
			max = data[i];
		}
	}
	return max;
}

void  largestNum(){
	int size = rand()%15;
	bsp_begin(P);
	int s = bsp_pid();
	int *dataRecieved;
	dataRecieved = createArray(size);
	toString(s, dataRecieved, size);
	int data[P];
	int *Maxs = calloc(bsp_nprocs(), sizeof(int));
	bsp_push_reg(Maxs, bsp_nprocs() * sizeof(int));
	bsp_sync();
	int max = findMax(s, dataRecieved, size);

	bsp_sync();
	
	bsp_sync();
	printf("Max value is %d\n", max);
	
	bsp_end();
}


	
int main( int argc, char ** argv ) {
    bsp_init( &largestNum, argc, argv );
	printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
    largestNum();
    return EXIT_SUCCESS;
}

