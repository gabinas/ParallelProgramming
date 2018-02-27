#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
#include <string.h>

int p;
int i;
int globalmax=0;
 void main(){
 bsp_begin(4);
 int input[10];
 int id=bsp_pid();
 int localmax=0;
 int k=0;
 for(k=0;k<4;k++){
	 
 if(id==k){
 for(i=0;i<10;i++){
 input[i]=rand()%20;
 }}
 bsp_sync();
 }
 

	 printf("processor %d array [%d]= %d\n",id,0,input[0]);
	 printf("processor %d array [%d]= %d\n",id,1,input[1]);
	 printf("processor %d array [%d]= %d\n",id,2,input[2]);
	 printf("processor %d array [%d]= %d\n",id,3,input[3]);
	 printf("processor %d array [%d]= %d\n",id,4,input[4]);
	 printf("processor %d array [%d]= %d\n",id,5,input[5]);
	 printf("processor %d array [%d]= %d\n",id,6,input[6]);
	 printf("processor %d array [%d]= %d\n",id,7,input[7]);
	 printf("processor %d array [%d]= %d\n",id,8,input[8]);
	 printf("processor %d array [%d]= %d\n",id,9,input[9]);
	 
	 for(k=0;k<4;k++){
	 
 if(id==k){
 for(i=0;i<10;i++){
 if(input[i]>localmax){
 localmax=input[i];
 }
 }}
 bsp_sync();
 }
	
	printf("processor %d localmax = %d\n",id,localmax);
	
	
	
int *Maxs=calloc(4,4*sizeof(int));
	bsp_push_reg(Maxs,4*sizeof(int));
	bspsync();
	printf("max 0=%d ; max 1=%d ;max 2=%d; max 3=%d",Maxs[0],Maxs[1],Maxs[2],Maxs[3]);
	
	 bsp_end();
	 }