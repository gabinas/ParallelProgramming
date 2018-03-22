
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"



int main( int argc, char ** argv ) {
	bsp_begin(4); /*Assign the number of processors */
	// 1.1
	int s = bsp_pid(); /*Save the ID of the processor */
	int r;
	// 1.2
	bsp_push_reg(&r, sizeof(s)); /*register a data-structure  as  available  for direct remote memory access of size s */
	// 1.3
	bsp_sync(); /*Sync all of the processors */

	// 2.1
	bsp_put((bsp_pid() + 1) % bsp_nprocs(), &s, &r, 0, sizeof(s)); /*This is where the shifting happens, by sending s to the processor to the right */
	// 2.2
	bsp_pop_reg(&r); /*Pop the register since its not needed anymore */
	// 2.3
	bsp_sync(); /*Sync all of the processors */

	// 3.1
	printf("Process %d received the identifier %d\n", s, r);
	// 3.2

	bsp_end();
}
