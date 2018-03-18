/*
This code is supposed to create a local randomized array in each process and then look for the local maxima and display it. 
Instead of finding the largest number, it displays large integer values (changes each time). 

*/

#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int size) {
int m = size;
int data[m];
int *dataAddress = data;
for (int i = 0; i < m; i++){
	int num = rand()%51;
	data[i] = num;
		
}
return dataAddress;
	
}
void  toString(int pID, int data[], int size){
	//int size = sizeof(data)/sizeof(int
	int max = data[0];
	printf( "\n Array from processor %d,size= %d. \n", pID, size);
	for (int i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
	printf("\n");

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
	int s = bsp_pid();
	int *dataReceived;
	dataReceived = createArray(size);
	toString(s, dataReceived, size);
	int max = findMax(dataReceived, size);
	bsp_sync();
	printf("\nMax value is %d\n", max);	
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
