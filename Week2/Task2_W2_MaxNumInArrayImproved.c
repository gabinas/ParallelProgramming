#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int size) { /* Create an array of size m filled with random numbers from 1 to 80*/
	int m = size;
	int data[m];
	int *dataAddress = data;
	int i;
	for (i = 0; i < m; i++){
	int num = rand()%80;
	data[i] = num;
		
}
return dataAddress;
	
}
void  toString(int pID, int data[], int size){ /*toString method to show the content of the array*/
	//int size = sizeof(data)/sizeof(int);
	printf( "\nArray from processor %d,size= %d. \n", pID, size);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}

}
//Find Max number from data[]
int findMax(int data[], int size){ /*Find the maximum number in the array*/
	int max = data[0];
	int i;
	for (i = 0; i < size; i++){
		if (data[i] > max){
			max = data[i];
		}
	}
	return max;
}

void  largestNum(){ /*Find the largest number among the max numbers found in every processor */
	int size = rand()%15; /*A random number from 1 to 15 will determine the size of the array*/
	bsp_begin(P);
	int s = bsp_pid(); /*save processor id (index)*/
	int *Maxs, maxSize;
	if(s==0){ /*save the maxsize of the array that will contain the max values from every processor*/
		maxSize = bsp_nprocs(); 
	}else{
		maxSize = 0;
	}
	Maxs = calloc(maxSize, sizeof(int));
	bsp_push_reg(Maxs, bsp_nprocs() * sizeof(int)); /*push space into a register to store the max numbers found in every processor, processor 0 will store its max in index 0, processor 1 in index 1, and so on*/
	
	int *dataRecieved;
	dataRecieved = createArray(size); /*save the array*/
	
	toString(s, dataRecieved, size); /*print out the content*/
	
	int max = findMax(dataRecieved, size); /*find the max number in every array*/
	
	bsp_sync(); /*sync all processors*/
	
	// 2 
	
	bsp_put(0,&max, Maxs, s*sizeof(int) , sizeof(int)); /*save the values*//*save the values*/
	bsp_sync();
	
	if(s==0){ /*Maxs is in processor 0, find the max value there*/
		printf("\nThe max number is: %d \n",findMax(Maxs,bsp_nprocs()));
	}
	bsp_pop_reg(&Maxs); /*pop the value from the registers*/
	
	bsp_end();
}


	
int main( int argc, char ** argv ) {
	printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs() ); /*Ask the user for the number of processors they want to use, can't exceed the max*/
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	bsp_init( &largestNum, argc, argv ); /*start the program in parallel*/
	largestNum();
    return EXIT_SUCCESS;
}
