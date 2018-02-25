#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
int p;


int bsp_shifttest(int r){
	int value =bsp_pid();
	int procs= bsp_nprocs();
if((value==0)&&(procs+3)==r){
	return 1;
}
else if((value>0)&&(value-1)==r){
return 1;

}
else{return 0;}
}



void spmd_start() {
	bsp_begin(p);
	// 1.1
	int s = bsp_pid();
	int r;
	// 1.2
	bsp_push_reg(&r, sizeof(s));
	// 1.3
	bsp_sync();

	// 2.1
	bsp_put((bsp_pid() + 1) % bsp_nprocs(), &s, &r, 0, sizeof(s));
	// 2.2
	bsp_pop_reg(&r);
	// 2.3
	bsp_sync();

	int testvalue =bsp_shifttest(r);
	printf("%d this is the value\n",testvalue);
	if(testvalue==1){
		printf("The data sent was a success\n");
	}else{printf("The data was not sent\n");
	
	}
	// 3.1
	printf("Process %d received the identifier %d\n", s, r);
	// 3.2

	bsp_end();
}
int main( int argc, char ** argv ){
	bsp_init(spmd_start,argc,argv);
	
printf("how many processors do you want to use");fflush(stdout);
scanf("%d",&p);
spmd_start();

return 0;
}