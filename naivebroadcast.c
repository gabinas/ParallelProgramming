#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;
int *arr;
int * createArray(int size, int id) {
	
	int *data=calloc(size,sizeof(int));
	int i;
	
	for (i = 0; i < size; i++){
		if(id==0){
		int num = rand()%80;
		data[i] = num;
		printf("location %d value: %d\n",i,data[i]);
	}}
	return data;	
}
void  toString(int pID, int data[], int size){
	printf( "\nArray in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
	
}

void naiveBroadcast(){
	// 1
	bsp_begin(P);
	int i=0;
	int n = 10;	// size of arrays
	int p = bsp_pid();
	
	int arraycount=0;
	int offset=0;
	int *arr;
		//Array for process 0
	int *Broad;		//Array for the rest processors 
	
	if(p == 0){
		arr = createArray(n,p);	//Populating array in p0
		
	
	}
	Broad = calloc(3, sizeof(int));		//Saving space for array in p1-pn
	bsp_push_reg(Broad, n*sizeof(int));	//Storing register for dynamic access
	bsp_sync();
	
	if(p == 0){	//We check what the array is after the sync
		toString(p,arr,n);
		printf("\n");
	}
	int m;
		int *r;
		r=&m;
		int *v;
		bsp_push_reg(r,sizeof(int));
	if(p==0){
		for(i=0;i<p;i++){
			v=&arr[i];
		bsp_put(i,v,r,0,sizeof(int));
		}
		
	}
	printf("processor %d value of r %d\n",p,*r);
	

	// 2
	if(p == 0){
	while(arraycount<n){
	
		for(int i = 0; i < P; i++){
			printf("bsp_put(pid: %d , source: %d , offset: %d , nbytes : %d arraycount : %d , size of array : %d",i,arr[arraycount],offset*4,n*4,arraycount, n);
			printf("Processor %d is trying to put value %d into processor %d\n",p,arr[arraycount],i);
			bsp_put(i,&arr[arraycount], Broad,offset*sizeof(int), sizeof(int));
			arraycount++;
			if(arraycount%4==0){
				offset++;
				break;
		} if(arraycount==n-1){
			break;
		}
	}
		
	
	}
	}
	toString(p,Broad,n);
	
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