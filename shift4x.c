#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
#include <string.h>
int p;


void bsp_shifttest(int num){
int value=bsp_pid();
if(value==num){
	printf("The 4 shift worked\n");
}

}

void spmd_start(){
bsp_begin(p);


int s = bsp_pid();
int r;
int t;
int u;
int v;

bsp_push_reg(&r, sizeof(s));

bsp_sync();


bsp_put((bsp_pid() + 1) % bsp_nprocs(), &s, &r, 0, sizeof(s));

bsp_pop_reg(&r);

bsp_sync();

printf("Shift 1 : Process %d received the identifier %d\n", s, r);


bsp_push_reg(&t, sizeof(s));

bsp_sync();


bsp_put((bsp_pid() + 1) % bsp_nprocs(), &r, &t, 0, sizeof(s));

bsp_pop_reg(&t);

bsp_sync();


printf("Shift 2: Process %d received the identifier %d\n", s, t);


bsp_push_reg(&u, sizeof(s));

bsp_sync();


bsp_put((bsp_pid() + 1) % bsp_nprocs(), &t, &u, 0, sizeof(s));

bsp_pop_reg(&u);

bsp_sync();


printf("Shift 3: Process %d received the identifier %d\n", s, u);


bsp_push_reg(&v, sizeof(s));

bsp_sync();


bsp_put((bsp_pid() + 1) % bsp_nprocs(), &u, &v, 0, sizeof(s));

bsp_pop_reg(&v);

bsp_sync();


printf("Shift 4: Process %d received the identifier %d\n", s, v);
bsp_sync();

bsp_shifttest(v);
bsp_end();
}



int main( int argc, char ** argv ){
	bsp_init(spmd_start,argc,argv);
	
printf("how many processors do you want to use");fflush(stdout);
scanf("%d",&p);
spmd_start();

return 0;
}
