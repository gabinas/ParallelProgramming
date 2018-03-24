#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

int * createArray(int pID, int size) {
	int *dataAddress = calloc(size, sizeof(int));
	for (int i =0; i < size ; i++){
		dataAddress[i]= rand()%100;
	}
	return dataAddress;	
}

void  toString(int pID, int data[], int size){
	printf( "Array in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
	printf("\n");
}

int cmpfunc(const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}

int * merge(int a1[], int a2[], int size1, int size2){
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
int * preProcess(int a[],int size,int p, int B){
	//Turns an array with repeated values into a unique value array
	for(int i =0; i <size; i++){
		a[i] = a[i]*(i + p+B);	//replacing xi with xi(constant number bigger than size of array +process number + i)
	}
	return a;
}
int * postProcess(int a[],int size,int B){
	for(int i =0; i <size; i++){
		a[i] = a[i]/B;	//Turning back modified array into original values 
	}
	return a;
}
void  sort(){
	bsp_begin(P);
	//Array Creation
	int p = bsp_pid();
	int size = 7;
	int *arr;
	int B=100;			//Constant for turning array into a non-repeated values array
	int *LS = calloc(P*(P+1),sizeof(int)); 
	arr = createArray(p,size);
	arr = preProcess(arr,size,p,B);
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
	bsp_sync();
	postProcess(m3,LengthIn[0]+LengthIn[1]+LengthIn[2]+LengthIn[3],B);
	int i=0;
	while(i < P){
		if(p==i)
			toString(p,m3,LengthIn[0]+LengthIn[1]+LengthIn[2]+LengthIn[3]);
		bsp_sync();
		i++;
	}

	bsp_end();
}
	
int main( int argc, char ** argv ) {
    P = 4;
	bsp_init( &sort, argc, argv );
	sort();
    return EXIT_SUCCESS;
}
