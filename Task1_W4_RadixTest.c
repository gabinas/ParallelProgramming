 #include <stdlib.h>
 #include <stdio.h>
 #include "include/mcbsp.h"
 int size;
 int P;
 int counter = 0;
 
 
 
  
 typedef struct KEYVALUE{
 int value;
 int key;
 }KEYVALUE;
 
 int * createArray(int s) {
 	int j;
 	int i;
 	int num;
 	int num2;
 	int lock;
 	KEYVALUE *dataAddress = calloc(s, sizeof(int)*2);
 	for (i = 0; i < s-1; i++){
 		num = rand()%50;
 		dataAddress[i].value = num;
 		 num2=rand()%s; 
 		 lock=0;
 		 
 		for(j=0;j<s;j++){
 			if(dataAddress[j].key==num2){
 				i--;
 				lock=1;
 				break;
 			}
 		}
 		if(lock==0){		
 			dataAddress[i].key=num2;
 		}
 		}
 		
 	
 	return dataAddress;	
 }
 
 
 void radixsort(){
 	bsp_begin(P);
 	 KEYVALUE *data;
 	 int id=bsp_pid();
 	 int blocksize=size/P;
 	 int m;
 	 int j;
 	 int i;
 	 if(id==0){
 	 data=createArray(size);
 	 }
 	 
 	KEYVALUE * sorted=calloc(size,8);
 	KEYVALUE * local=calloc(blocksize,8);
 	bsp_push_reg(sorted,size*8);
 	bsp_push_reg(local,size*8);
 	bsp_sync();
 	if(id==0){
+		printf("\nUNSORTED:\n");
 		for(i=0;i<size;i++){
 			
 		printf(" [key,value] [%d,%d]\n",data[i].key,data[i].value);
 		
 	}
 	}
 	if(id==0){
 		for(i=0;i<P;i++){
 		bsp_put(i,&data[i*blocksize],local,0,(blocksize*sizeof(int))*2);
 		}
 	}
 	bsp_sync();
 	
 	for(i=0;i<blocksize;i++){
 		m=local[i].key;
 		
 		bsp_put(0,&local[i],sorted,m*8,8);
 		}
 	bsp_sync();
 	
 	
 	if(id==0){
 		printf("\nSORTED:\n");
 		for(i=0;i<size;i++){
 		printf(" [key,value] [%d,%d]\n",sorted[i].key,sorted[i].value);
 		
		}
	printf("\n Analysis of the result:\n");
		for ( i = 0; i < size; i++){
			if( i == sorted[i].key){
				printf("\nIndex at %d correctly sorted\n", sorted[i].key);
				counter = counter + 1;
			} else{
				printf("\nThe array was not sorted properly\n");
		}
 	}
 	bsp_end();
 	}
 }
 
 int main(int argc, char ** argv){
 	printf("The test code has an array size of 16, and will use 4 processors");
 	size = 16;
 	int answer=0;
     fflush( stdout );
     P = 4;
     if( P == 0 || P > bsp_nprocs() ) {
         fprintf( stderr, "Cannot start %d threads.\n", P );
         return EXIT_FAILURE;
     }
 	if((size%P)!=0){
 		printf("Sorry the size of %d and requested processors %d are not divisible data will be corrupted\n",size,P);
 		printf("Would you like to exit please type 1 for yes and 0 for no");
 		scanf("%d",&answer);
 	}
 	bsp_init( &radixsort, argc, argv );
 	if(answer==0){
 	radixsort();}
 	else{
     return EXIT_SUCCESS;}	
 	}