#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
#include <string.h>
int p;
int i;



void spmd(){
	bsp_begin(p);
	int max=0;
	
	
	/*getting random values*/
int input[10];
int globalmax=0;
int elements = sizeof(input)/sizeof(int);
for(i=0;i<elements;i++){
	input[i]= rand() % 20;
}
/*Printing values from each processor*/
printf("from processor %d value is %d\n",bsp_pid(),input[0]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[1]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[2]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[3]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[4]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[5]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[6]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[7]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[8]);fflush(stdout);
printf("from processor %d value is %d\n",bsp_pid(),input[9]);fflush(stdout);




int *Maxs=(int*)calloc(p,sizeof(int));




/*registering global data area for processors to communicate*/
bsp_push_reg(Maxs,p*sizeof(int));
bsp_sync();

/*finding max in each processor*/
for(i=0;i<elements;i++){
	if(input[i]>max){
	max=input[i];
	printf("found new max processor %d and the new max is %d\n",bsp_pid(),input[i]);fflush(stdout);
	
}

}
/*printing local max from each processor*/
printf("from processor %d max value is %d\n",bsp_pid(),max);fflush(stdout);

/*sending max value to processor 0*/
bsp_put(0,&max,Maxs,bsp_pid(),sizeof(int));
bsp_pop_reg(Maxs); /*deregistering variable*/
bsp_sync();

/* I cant figure out why its not working so i tried putting the values into a local variable but its still not working*/
for(i=0;i<p;i++){
	if(Maxs[i]>globalmax){
		globalmax=Maxs[i];
	}
printf("the global maxes are %d\n",Maxs[i]);
}



bsp_end();

}


 int main( int argc, char ** argv ){
	bsp_init(spmd,argc,argv);
printf("how many processors do you want to use");fflush(stdout);
scanf("%d",&p);
 
 spmd();
 
 return 0;
 }
 
