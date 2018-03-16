
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"



int main( int argc, char ** argv ) {
	bsp_begin(4);
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

	// 3.1
	printf("Process %d received the identifier %d\n", s, r);
	// 3.2

	bsp_end();
}
