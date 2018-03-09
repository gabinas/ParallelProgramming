
#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;


void logBroadcast(){
	// 1
	bsp_begin(P);
		
	bsp_sync();	//first synchronize
	
	// 2

	bsp_sync();
		
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
    bsp_init( &logBroadcast, argc, argv );
	logBroadcast();
    return EXIT_SUCCESS;
}
