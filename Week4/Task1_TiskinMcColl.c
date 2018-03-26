#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;
/*Note that for very long arrays the processes might overlap causing the arrays to be 
*printed at the same time and therefore the ouput will be mixed arrays.

Also note that this sort only works properly with non-repeated numbers


*/


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

int * merge(int a1[], int a2[], int size1, int size2){
	//This method merges two sorted arrays into one sorted array
	int c1 = 0,c2 = 0, m = 0;
	int *merged = calloc(size1+size2,sizeof(int));
	while(c1 < size1 && c2 < size2){
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

	while(c1 < size1){
		merged[m] = a1[c1];
		m++;
		c1++;
	}
	while(c2 < size2){
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
	LS = merge( merge(p0,p1,P+1,P+1) , merge(p2,p3,P+1,P+1) , (P+1)*2 , (P+1)*2 );

	//Pick Global Separators
	int *global = calloc(P+1, sizeof(int));
	global[0] = LS[0];
	global[1] = LS[(int)(P*(P+1)/4)];
	global[2] = LS[(int)(2*P*(P+1)/4)];
	global[3] = LS[(int)(3*P*(P+1)/4)];
	global[4] = LS[(int)(P*(P+1)-1)];

	//Partition Local blocks according to global separators
	int Route[P][size];	//Will store all partitions of the array into processors
	int *Length = calloc(P,sizeof(int)); //Length of each of the arrays in Route

	for(int i = 0; i < size; i++){
		if(arr[i] <= global[1]){
			Route[0][Length[0]] = arr[i];
			Length[0]++;
		}else if(arr[i] <= global[2]){
			Route[1][Length[1]] = arr[i];
			Length[1]++;
		}else if(arr[i] <= global[3]){
			Route[2][Length[2]] = arr[i];
			Length[2]++;
		}else {
			Route[3][Length[3]] = arr[i];
			Length[3]++;
		} 
	}

	//Variables used for the exchange of data
	int RouteIn[P][size];
	int *LengthIn = calloc(P,sizeof(int));
	//Registers
	bsp_push_reg(RouteIn,sizeof(int)*P*size);
	bsp_push_reg(LengthIn,sizeof(int)*P);
	bsp_sync();
	//Routing: sending the local data to the corresponding processor
	for(int i = 0; i < P; i++){
		bsp_put(i,&Length[i],LengthIn,p*sizeof(int),sizeof(int));
		bsp_put(i,&Route[i],RouteIn,p*size*sizeof(int),sizeof(int)*size);
	}
	bsp_sync();
	//Final merge of the arrays received in Routing 
	int *m1 = merge(&RouteIn[0][0],&RouteIn[1][0],LengthIn[0],LengthIn[1]);
	int *m2 = merge(&RouteIn[2][0],&RouteIn[3][0],LengthIn[2],LengthIn[3]);
	int *m3 = merge(m1,m2,LengthIn[0]+LengthIn[1],LengthIn[2]+LengthIn[3]);
	//DONE
	toString(p,m3,LengthIn[0]+LengthIn[1]+LengthIn[2]+LengthIn[3]);

	bsp_end();
}
	
int main( int argc, char ** argv ) {
    P = 4;
	bsp_init( &sort, argc, argv );
	sort();
    return EXIT_SUCCESS;
}
