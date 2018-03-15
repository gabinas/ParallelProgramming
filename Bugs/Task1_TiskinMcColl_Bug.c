#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;
/*
This is a buggy file where the Segmentation fault (core dumped) error occurs when reaching qsort
it should create an array in each processor and sort through them, but it terminates the program.
*/
int * createArray(int pID, int size) {
	//For now, we hard code the array to be sorted in order to ensure a proper input of numbers
	/*
	int *dataAddress = calloc(size, sizeof(int));
	for(int i = pID*size; i < (pID+1)*size; i++){
		dataAddress[i-pID*size] = i;
	}
	
	
	*/
	int *dataAddress;
	switch(pID){
	case 0:	
		dataAddress[0]=16;
		dataAddress[1]=7;
		dataAddress[2]=1;
		dataAddress[3]=27;
		dataAddress[4]=21;
		dataAddress[5]=12;
		dataAddress[6]=10;
		break;
	case 1:
		dataAddress[0]=0;
		dataAddress[1]=3;
		dataAddress[2]=26;
		dataAddress[3]=8;
		dataAddress[4]=17;
		dataAddress[5]=18;
		dataAddress[6]=22;
		break;
	case 2:
		dataAddress[0]=2;
		dataAddress[1]=5;
		dataAddress[2]=9;
		dataAddress[3]=13;
		dataAddress[4]=15;
		dataAddress[5]=23;
		dataAddress[6]=25;
		break;
	case 4:
		dataAddress[0]=4;
		dataAddress[1]=6;
		dataAddress[2]=11;
		dataAddress[3]=14;
		dataAddress[4]=20;
		dataAddress[5]=24;
		dataAddress[6]=19;
		break;
	}
	//We shuffle them
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
	
	printf("Test 4");
	arr = createArray(p,size);
	
	bsp_push_reg(LS,P*size*sizeof(int));
	
	bsp_sync();
	
	// Local Sort
	
	qsort(arr, size, sizeof(int), cmpfunc);
	toString(p,arr,size);
	bsp_sync();
	//Storing and sharing Local Sample
	/*
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
	
	printf( "How many threads do you want started? There are %d cores available. (Only 4 supported)\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
   
   if( P != 4 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	
	bsp_init( &sort, argc, argv );
	sort();
    return EXIT_SUCCESS;
}