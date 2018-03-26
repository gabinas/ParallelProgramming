#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
int size;
int P;







int * createArray(int s) {
	int i=0;
	int *dataAddress = calloc(s, sizeof(int));
	for (i = 0; i < s-1; i++){
		dataAddress[i] = rand()%100;
		}
		return dataAddress;	
}
int prototypefind(int *data,int localsize){
int prototypei=0;
int i;
for(i=0;i<localsize;i++){
	prototypei+=data[i];
}	

prototypei=prototypei/localsize;

return prototypei;	
	
}
int procfind(int data,int range,int procs){
	int i=0;
	int prototype=range/procs;
	
	for(i=1;i<procs;i++){
	if(data<prototype*i){
		return i-1;
	}
	}
return procs-1;
}

void toString(int *data,int asize){
	int i=0;
	printf("From Processor %d\n",bsp_pid());
	for(i=0;i<asize;i++){
		printf("data[i]= %d\n",data[i]);
		}
	
	
}
void kmeans(){
	bsp_begin(P);
	int procs=bsp_nprocs();
	int id=bsp_pid();
	int clustersize=size/procs;
	int range=100;
	int *location=calloc(P,sizeof(int));
	int d,loc,j,localsize,*data,psend,i,prototype;
	
	if(id==0){
		data=createArray(size);
		}
		
	int *cluster= calloc(clustersize*2,4);
	bsp_push_reg(cluster,clustersize*8);
	bsp_push_reg(&localsize,sizeof(int));
	bsp_sync();
	
	if(id==0){
	for(i=0;i<size;i++){
		 d =data[i];
		psend=procfind(d,range,procs);
		loc=location[psend];
		printf("psend %d\n",loc*4);
		bsp_put(psend,&d,cluster,loc*4,sizeof(int));
		location[psend]+=1;
		
	}
	for(j=0;j<procs;j++){
	bsp_put(j,&location[j],&localsize,0,sizeof(int));
	}
	}
	bsp_sync();
	
	toString(cluster,localsize);
	bsp_sync();
	
	prototype=prototypefind(cluster,localsize);
	
	
	
	
	bsp_end();
	
}









int main(int argc, char ** argv){
	printf("What is the size of the array you would like? ");
	scanf("%d",&size);
	int answer=0;
	printf( "How many threads do you want started? There are %d cores available. note:size/procs must = integer \n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
/* 	if((size%P)!=0){
		printf("Sorry the size of %d and requested processors %d are not divisible data will be corrupted\n",size,P);
		printf("Would you like to exit please type 1 for yes and 0 for no");
		scanf("%d",&answer);
	} */
	bsp_init( &kmeans, argc, argv );
	if(answer==0){
	kmeans();}
	else{
    return EXIT_SUCCESS;}
	
		
	}
	