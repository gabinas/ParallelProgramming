
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

void spmd(){
	bsp_begin(P);
	// 1.1
	int s = bsp_pid(); /*Save the ID of the processor */
	int r;
	// 1.2
	bsp_push_reg(&r, sizeof(s)); /*register a data-structure  as  available  for direct remote memory access of size s */
	// 1.3
	bsp_sync(); /*Sync all of the processors */

	// 2.1
	bsp_put((bsp_pid() - 1) % bsp_nprocs(), &s, &r, 0, sizeof(s)); /*This is where the shifting happens, by sending s to the processor to the left */
	// 2.2
	bsp_pop_reg(&r); /*Pop the register since its not needed anymore */
	// 2.3
	bsp_sync(); /*Sync all of the processors */

	// 3.1
	printf("Process %d received the identifier %d\n", s, r);
	// 3.2

	bsp_end();
}
int main( int argc, char ** argv ) {
    printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs() ); /*User will input the number of processors */
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
    bsp_init( &spmd, argc, argv ); /*start the shift program in parallel*/
    spmd();
    return EXIT_SUCCESS;
}
