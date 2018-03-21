#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int pID, int size) {
	int *dataAddress = calloc(size, sizeof(int));
	if(pID == 0){
		dataAddress[0] = 27; 
		dataAddress[1] = 10; 
		dataAddress[2] = 2; 
		dataAddress[3] = 7; 
		dataAddress[4] = 41; 
		dataAddress[5] = 1; 
		dataAddress[6] = 16; 
	}else if(pID == 1){
		dataAddress[0] = 3; 
		dataAddress[1] = 24; 
		dataAddress[2] = 35; 
		dataAddress[3] = 8; 
		dataAddress[4] = 12; 
		dataAddress[5] = 29; 
		dataAddress[6] = 32; 
	}else if(pID == 2){
		dataAddress[0] = 4; 
		dataAddress[1] = 13; 
		dataAddress[2] = 52; 
		dataAddress[3] = 31; 
		dataAddress[4] = 25; 
		dataAddress[5] = 17; 
		dataAddress[6] = 28; 
	}else if(pID == 3){
		dataAddress[0] = 36; 
		dataAddress[1] = 6; 
		dataAddress[2] = 15; 
		dataAddress[3] = 19; 
		dataAddress[4] = 33; 
		dataAddress[5] = 48; 
		dataAddress[6] = 11; 
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

int * merge(int a1[], int a2[], int size){
	int c1 = 0,c2 = 0, m = 0;
	int *merged = calloc(size*2,sizeof(int));
	while(c1 < size && c2 < size){
		if(a1[c1] > a2[c2]){
			merged[m] = a2[c2];
			m++;
			c2++;
		} else {
			merged[m] = a1[c1];
			m++;
			c1++;
		}
	}

	while(c1 < size){
		merged[m] = a1[c1];
		m++;
		c1++;
	}
	while(c2 < size){
		merged[m] = a2[c2];
		m++;
		c2++;
	}

	return merged;
}

void  sort(){
	bsp_begin(P);
	
	//Array Creation
	int p = bsp_pid();
	int size = 7;
	int *arr;
	int *LS = calloc(P*(P+1),sizeof(int)); 
	arr = createArray(p,size);
	bsp_push_reg(LS,P*(P+1)*sizeof(int));
	bsp_sync();
	
	// Local Sort
	qsort(arr, size, sizeof(int), cmpfunc);

	//Storing Local Sample

	int *localSample = calloc(P+1,sizeof(int));
	localSample[0] = arr[0];
	localSample[1] = arr[1];
	localSample[P-2] = arr[(int)(size/2)];
	localSample[P-1] = arr[size-2];
	localSample[P] = arr[size-1];
	bsp_push_reg(localSample, (P+1)*sizeof(int));
	bsp_sync();

	//Sharing local sample with all processor
	int * p0 = calloc(P+1,sizeof(int));
	int * p1 = calloc(P+1,sizeof(int));
	int * p2 = calloc(P+1,sizeof(int));
	int * p3 = calloc(P+1,sizeof(int));

	//Receives all local samples as arrays
	bsp_get(0,localSample,0,p0,(P+1)*sizeof(int));
	bsp_get(1,localSample,0,p1,(P+1)*sizeof(int));
	bsp_get(2,localSample,0,p2,(P+1)*sizeof(int));
	bsp_get(3,localSample,0,p3,(P+1)*sizeof(int));
	bsp_sync();

	//Local Sample Merge
	LS = merge(merge(p0,p1,P+1),merge(p2,p3,P+1),(P+1)*2);

	//Pick Global Separators
	int *global = calloc(P+1, sizeof(int));
	global[0] = LS[0];
	global[1] = LS[(int)(P*(P+1)/4)];
	global[2] = LS[(int)(2*P*(P+1)/4)];
	global[3] = LS[(int)(3*P*(P+1)/4)];
	global[4] = LS[(int)(P*(P+1)-1)];

	//Partition Local blocks according to global separators
	int *partOne = calloc(P+1,sizeof(int));
	int *partTwo = calloc(P+1,sizeof(int));
	int *partThree = calloc(P+1,sizeof(int));
	int *partFour = calloc(P+1,sizeof(int));
	int c1 = 0,c2 = 0,c3 = 0,c4 = 0;

	for(int i = 0; i < size; i++){
		if(arr[i] <= global[1]){
			partOne[c1] = arr[i];
			c1++;
		}else if(arr[i] <= global[2]){
			partTwo[c2] = arr[i];
			c2++;
		}else if(arr[i] <= global[3]){
			partThree[c3] = arr[i];
			c3++;
		}else {
			partFour[c4] = arr[i];
			c4++;
		} 
	}

	int **a = (int **)calloc(P,sizeof(int));
	a[0] = (int *)calloc(40,sizeof(int));
	a[1] = (int *)calloc(40,sizeof(int));
	a[2] = (int *)calloc(40,sizeof(int));
	a[3] = (int *)calloc(40,sizeof(int));

	bsp_push_reg(a[0],(40)*sizeof(int));
	bsp_push_reg(a[1],(40)*sizeof(int));
	bsp_push_reg(a[2],(40)*sizeof(int));
	bsp_push_reg(a[3],(40)*sizeof(int));
	bsp_sync();

	bsp_put(0,partOne,&a[0][p],0,c1*sizeof(int));
	bsp_put(1,partTwo,&a[1][p],0,c2*sizeof(int));
	bsp_put(2,partThree,&a[2][p],0,c3*sizeof(int));
	bsp_put(3,partFour,&a[3][p],0,c4*sizeof(int));


	bsp_sync();
	/*
	toString(p,list1,10);
	toString(p,list2,10);
	toString(p,list3,10);
	toString(p,list4,10);
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