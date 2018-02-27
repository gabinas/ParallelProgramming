
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

void spmd(){
	bsp_begin(P);
	// 1.1
	int s = bsp_pid();
	int randNum = rand()%50;
	int r;
	// 1.2
	bsp_push_reg(&r, sizeof(s));
	// 1.3
	bsp_sync();

	// 2.1
	bsp_put((bsp_pid() - 1) % bsp_nprocs(), &randNum, &r, 0, sizeof(randNum));
	// 2.2
	bsp_pop_reg(&r);
	// 2.3
	bsp_sync();

	// 3.1
	printf("Process %d received the identifier %d\n", s, r);
	// 3.2

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
    bsp_init( &spmd, argc, argv );
    spmd();
    return EXIT_SUCCESS;
}
