#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
unsigned int P;

/*
This program is supposed to complete the Two-Phase broadcast. 
However, it only goes through the first phase and doesn't complete the second one.
Later on we fixed this by synchronizing the processors after the last loop.

*/
/*
typedef struct node{
	int data;
	struct node *next;
}node_t;
*/
struct node{
	int data; 
	struct node *next;
};
typedef struct node node_t;


node_t* create(int data,node_t* next){
    node_t* new_node = malloc(sizeof(node_t));
    if(new_node == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->data = data;
    new_node->next = next;
 
    return new_node;
}

void print_list(node_t * head){
	node_t * current = head;
	while(current!=NULL){
		printf("%d\n",current->data);
		current = current->next;
	}
}

void push(node_t* head, int data){
    /* go to the last node */
    node_t *cursor = head;
    while(cursor->next != NULL)
        cursor = cursor->next;
 
    /* create a new node */
    cursor->next = malloc(sizeof(node_t));
   	cursor->next->data = data; 
    cursor->next->next = NULL;
}

void display(node_t* n){
    if(n != NULL)
        printf("%d ", n->data);
}

int count(node_t* head){
    node_t *cursor = head;
    int c = 0;
    while(cursor != NULL)
    {
        c++;
        cursor = cursor->next;
    }
    return c;
}

int * createArray(int pID, int size) {
	int *dataAddress = calloc(size, sizeof(int));
	if(pID == 0){
		dataAddress[0] = 27; 
		dataAddress[1] = 10; 
		dataAddress[2] = 2; 
		dataAddress[3] = 7; 
		dataAddress[4] = 41; 
		dataAddress[5] = 1; 
		dataAddress[6] = 16; 
	}else if(pID == 1){
		dataAddress[0] = 3; 
		dataAddress[1] = 24; 
		dataAddress[2] = 35; 
		dataAddress[3] = 8; 
		dataAddress[4] = 12; 
		dataAddress[5] = 29; 
		dataAddress[6] = 32; 
	}else if(pID == 2){
		dataAddress[0] = 4; 
		dataAddress[1] = 13; 
		dataAddress[2] = 52; 
		dataAddress[3] = 31; 
		dataAddress[4] = 25; 
		dataAddress[5] = 17; 
		dataAddress[6] = 28; 
	}else if(pID == 3){
		dataAddress[0] = 36; 
		dataAddress[1] = 6; 
		dataAddress[2] = 15; 
		dataAddress[3] = 19; 
		dataAddress[4] = 33; 
		dataAddress[5] = 48; 
		dataAddress[6] = 11; 
	}
	
	for(int i = pID*size; i < (pID+1)*size; i++){
		int temp = dataAddress[i-pID*size];
		int randomIndex = rand()%size;
		
		dataAddress[i-pID*size] = dataAddress[randomIndex];
		dataAddress[randomIndex] = temp;
	}	
	return dataAddress;	
}

void  toString(int pID, int data[], int size){
	printf( "\nArray in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
}

int cmpfunc(const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}

void  sort(){
	bsp_begin(P);
	
	//Array Creation
	int p = bsp_pid();
	int size = 7;
	int *arr;
	int *LS = calloc(P*(P+1),sizeof(int)); 
	arr = createArray(p,size);
	bsp_push_reg(LS,P*(P+1)*sizeof(int));
	bsp_sync();
	
	// Local Sort
	qsort(arr, size, sizeof(int), cmpfunc);

	//Storing Local Sample
	int *localSample = calloc(P+1,sizeof(int));
	localSample[0] = arr[0];
	localSample[1] = arr[1];
	localSample[P-2] = arr[(int)(size/2)];
	localSample[P-1] = arr[size-2];
	localSample[P] = arr[size-1];
	bsp_push_reg(localSample, (P+1)*sizeof(int));

	//Sharing local sample with all processor
	node_t * p0 = calloc(P+1, sizeof(int));
	node_t * p1 = calloc(P+1, sizeof(int));
	node_t * p2 = calloc(P+1, sizeof(int));
	node_t * p3 = calloc(P+1, sizeof(int));
	bsp_sync();


	bsp_get(0,localSample,0,p0,(P+1)*sizeof(int));
	bsp_get(1,localSample,0,p1,(P+1)*sizeof(int));
	bsp_get(2,localSample,0,p2,(P+1)*sizeof(int));
	bsp_get(3,localSample,0,p3,(P+1)*sizeof(int));
	bsp_sync();

	p0->next = 

	//push(p0,2);

	/*
	for(int i = 0; i < P; i++){
		bsp_put(i,localSample,LS,p*(P+1)*sizeof(int),(P+1)*sizeof(int));
	}
	bsp_sync();
	*/
	//Local Sample Merge

	/*
	//qsort(LS, P*(P+1), sizeof(int), cmpfunc);

	//Pick Global Separators
	int *global = calloc(P+1, sizeof(int));
	global[0] = LS[0];
	global[1] = LS[(int)(P*(P+1)/4)];
	global[2] = LS[(int)(2*P*(P+1)/4)];
	global[3] = LS[(int)(3*P*(P+1)/4)];
	global[4] = LS[(int)(P*(P+1)-1)];

	//Partition Local blocks according to global separators
	int *partOne = calloc(40,sizeof(int));
	int *partTwo = calloc(40,sizeof(int));
	int *partThree = calloc(40,sizeof(int));
	int *partFour = calloc(40,sizeof(int));





	/*
	for(int i = 0; i < P+1; i++){
		if(arr[i] > global[0] && arr[i] < global[1]){

		}else if(arr[i] > global[1] && arr[i] < global[2]){

		}else if(arr[i] > global[2] && arr[i] < global[3]){

		}else if(arr[i] > global[3] && arr[i] < global[4]){

		}else if(arr[i] > global[4]){

		}
	}
	*/
	bsp_end();
}
	
int main( int argc, char ** argv ) {
	printf( "How many threads do you want started? There are %d cores available. (Max 4 cores)\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	bsp_init( &sort, argc, argv );
	sort();
    return EXIT_SUCCESS;
}