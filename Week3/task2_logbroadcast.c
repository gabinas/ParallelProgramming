#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;
int cost=0;

//This program is the same as task 1 but this one will give the value of how much data was sent.


int * createArray(int size){
	
	int *dataAddress = calloc(size,sizeof(int));
	int i;
	for (i = 0; i < size; i++){
		int num = rand()%80;
		dataAddress[i] = num;
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
int datachecker(int data1[],int data2[],int size,int pid ){
	int i=0;
	int counter=0;
	int empty=0;
	for(i=0;i<size;i++){
		if(data1[i]==0){
			empty++;
			
		}
		if(empty==(size/2)){
		
		break;
		
		}
		if(data1[i]==data2[i]){
			counter++;
		}
		
	}
	if(counter==size){
		return 1;
	}
	else{
		
		return 0;
	}
	
	}
	
	
	int finalcost(int *cost){
		int size=P;
		int i;
		int finalcost=0;
		for(i=0;i<size;i++){
	
			finalcost+=cost[i];
		}
		return finalcost;
	}

void naiveBroadcast(){
	// 1
	bsp_begin(P);
	int i;
	int n = 10;	// size of arrays
	int count=0;
	int id = bsp_pid();
	int *arr;		//Array for process 0
	int *Broad;		//Array for the rest processors 
	int key=0;
	int *Broadpoint;
	int Procs= P;
	int step=1;
	int localcost=0;
	int finalglobalcost=0;
	int *cost=calloc(n,n*sizeof(int));
	bsp_push_reg(cost,n*sizeof(int));
	bsp_push_reg(arr, n*sizeof(int));	//Storing register for dynamic access
	if(id == 0){
		arr = createArray(n);	//Populating array in p0
		
	}
	Broad = calloc(n, sizeof(int));		//Saving space for array in p1-pn
	bsp_push_reg(Broad, n*sizeof(int));	//Storing register for dynamic access
	bsp_sync();	//first synchronize
	Broadpoint=Broad;// creating a second pointer for the put operation this does not need to be done but it helped me while coding
	
	
	// 2
	if(id==0){
	bsp_put(0,arr,Broad,0,n*sizeof(int));//putting the array in proc 0 from variable array into global variable so that i dont have to change variable names in iterations of while loop.
		
		 key=datachecker(arr,Broad,n,id);
		 
		
		
 	}
	bsp_sync();
	 
	
	
	
	while(Procs>=1){										
	key=datachecker(Broadpoint,Broad,n,id);					//checking for data and sending if not present in processor
	if(key==1){
		bsp_put((id+(Procs/2)),Broadpoint,Broad,0,n*sizeof(int));
		printf("processor %d has sent:\n",id);
		
		printf(" to processor %d\n",(id+(Procs/2)));
		localcost+=n*sizeof(int);
	}
	Procs=Procs/2;
	step++;
	bsp_sync();
	}
	
	bsp_put(0,&localcost,cost,id*sizeof(int),sizeof(int));
	bsp_sync();
	if(id==0){
		finalglobalcost=finalcost(cost);
		printf("this is the total data sent %d bytes\n",finalglobalcost);
	}
	toString(id,Broad,n);
	
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
    bsp_init( &naiveBroadcast, argc, argv );
	naiveBroadcast();
    return EXIT_SUCCESS;
}
