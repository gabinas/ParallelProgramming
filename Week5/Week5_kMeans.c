#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
#include <stdbool.h>
unsigned int P;
int size,k;
int * createArray(int size) {
	int *dataAddress = calloc(size, sizeof(int));
	for (int i =0; i < size ; i++){
		dataAddress[i]= rand()%100;
	}
	
	return dataAddress;	
}

void  toString(int pID, int data[], int size){
	//printf( "Array in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%d ",data[i]);
	}
	printf("\n");
}
void  toStringDouble(int pID, double data[], int size){
	//printf( "Array in process %d: \n",pID);
	int i;
	for (i = 0; i < size; i++){
		printf("%f ",data[i]);
	}
	printf("\n");
}
bool contains(double centroids[],int n,int Size){
	for (int i = 0; i < Size ; i++ ){
		if(centroids[i] == n){
			return true;
		}
	}
	return false;
}
double * getCentroids(int k, int data[],int size){
	double *centroids = calloc(k,sizeof(double));
	int c;
	for(int i =0; i < k; i++){
		int r = rand()%(size);
		c=0;
		while(contains(centroids,data[r],k)){
			r = rand()%(size);
			if(c > size/2){	//Only look for new numbers size/2 times ensuring high probability of different centroids, while allowing repeated datapoints.
				break;
			}
			c++;
		}
		centroids[i] = data[r];
	}
	return centroids;
}
int * merge(int a1[], int a2[], int size1, int size2){
	int c1 = 0,c2 = 0, m = 0;
	int *merged = calloc(size1+size2,sizeof(int));
	while(c1 < size1 && c2 < size2){
		if(a1[c1] > a2[c2]){
			merged[m] = a2[c2];
			m++;
			c2++;
		} else {
			merged[m] = a1[c1];
			m++;
			c1++;
		}
	}

	while(c1 < size1){
		merged[m] = a1[c1];
		m++;
		c1++;
	}
	while(c2 < size2){
		merged[m] = a2[c2];
		m++;
		c2++;
	}

	return merged;
}
int dif(int a,int b){
	int c = a - b;
	if(c < 0){
		c = c * (-1);
	}
	return c;
}

void  kMeans(){
	bsp_begin(P);
	//Array Creation
	int p = bsp_pid();	//variable to keep track of which processor is working
	int *arr;			//Main Array paired with cluster array
	double *centroids;	//Array to keep track of centroids 
	int *clusters = calloc(size,sizeof(int));	// Array to keep track of which cluster the data is in
	//Broadcasting data and centroids
	if(p == 0){
		arr = createArray(size);
		centroids = getCentroids(k,arr,size);
	}else{
		arr = calloc(size,sizeof(int));
		centroids = calloc(k,sizeof(double));
	}
	bsp_push_reg(arr,size*sizeof(int));
	bsp_sync();
	// Local Sort
	if(p!=0){
		bsp_get(0,arr,0,arr,size*sizeof(int));
	}

	bsp_sync();

	bsp_push_reg(centroids,k*sizeof(double));

	bsp_sync();

	if(p!=0)
		bsp_get(0,centroids,0,centroids,k*sizeof(double));

	bsp_sync();
	
	for (int i = p * (size/P); i < (p+1) * (size/P) ; i++){
		int min = dif(centroids[0],arr[i]);
		int minIdx = 0;
		int temp;
		for(int j = 0; j < k ; j++){
			if((temp = dif(centroids[j],arr[i])) < min){
				minIdx = j;
				min = temp;

			}
		}
		clusters[i] = minIdx;
	}

	for(int i =0; i < P; i++){
		if(p == i){
		printf("\nFor processor number %d:\n",p);
		printf("We have the clusters:\n");
		toString(p,clusters,size);
		printf("for the numbers:\n");
		toString(p,arr,size);
		printf("And the centroids::\n");
		toStringDouble(p,centroids,k);
	}
}
	//Storing Local Sample
	/*
	bsp_push_reg(localSample, (P+1)*sizeof(int));
	bsp_sync();

	//Sharing local sample with all processor
	int * p0 = calloc(P+1,sizeof(int));
	int * p1 = calloc(P+1,sizeof(int));
	int * p2 = calloc(P+1,sizeof(int));
	int * p3 = calloc(P+1,sizeof(int));

	//Receives all local samples as arrays
	bsp_get(0,localSample,0,p0,(P+1)*sizeof(int));
	bsp_get(1,localSample,0,p1,(P+1)*sizeof(int));
	bsp_get(2,localSample,0,p2,(P+1)*sizeof(int));
	bsp_get(3,localSample,0,p3,(P+1)*sizeof(int));
	bsp_sync();

	//Local Sample Merge
	LS = merge( merge(p0,p1,P+1,P+1) , merge(p2,p3,P+1,P+1) , (P+1)*2 , (P+1)*2 );

	//Pick Global Separators
	int *global = calloc(P+1, sizeof(int));
	global[0] = LS[0];
	global[1] = LS[(int)(P*(P+1)/4)];
	global[2] = LS[(int)(2*P*(P+1)/4)];
	global[3] = LS[(int)(3*P*(P+1)/4)];
	global[4] = LS[(int)(P*(P+1)-1)];

	//Partition Local blocks according to global separators
	int Route[P][size];	//Will store all partitions of the array into processors
	int *Length = calloc(P,sizeof(int)); //Length of each of the arrays in Route

	for(int i = 0; i < size; i++){
		if(arr[i] <= global[1]){
			Route[0][Length[0]] = arr[i];
			Length[0]++;
		}else if(arr[i] <= global[2]){
			Route[1][Length[1]] = arr[i];
			Length[1]++;
		}else if(arr[i] <= global[3]){
			Route[2][Length[2]] = arr[i];
			Length[2]++;
		}else {
			Route[3][Length[3]] = arr[i];
			Length[3]++;
		} 
	}

	//Variables used for the exchange of data
	int RouteIn[P][size];
	int *LengthIn = calloc(P,sizeof(int));
	//Registers
	bsp_push_reg(RouteIn,sizeof(int)*P*size);
	bsp_push_reg(LengthIn,sizeof(int)*P);
	bsp_sync();
	//Routing: sending the local data to the corresponding processor
	for(int i = 0; i < P; i++){
		bsp_put(i,&Length[i],LengthIn,p*sizeof(int),sizeof(int));
		bsp_put(i,&Route[i],RouteIn,p*size*sizeof(int),sizeof(int)*size);
	}
	bsp_sync();
	//Final merge of the arrays received in Routing 
	int *m1 = merge(&RouteIn[0][0],&RouteIn[1][0],LengthIn[0],LengthIn[1]);
	int *m2 = merge(&RouteIn[2][0],&RouteIn[3][0],LengthIn[2],LengthIn[3]);
	int *m3 = merge(m1,m2,LengthIn[0]+LengthIn[1],LengthIn[2]+LengthIn[3]);
	//DONE
	bsp_sync();
	//Return the values to original before processing
	postProcess(m3,LengthIn[0]+LengthIn[1]+LengthIn[2]+LengthIn[3],B);
	int i=0;
	while(i < P){
		if(p==i)
			toString(p,m3,LengthIn[0]+LengthIn[1]+LengthIn[2]+LengthIn[3]);
		bsp_sync();
		i++;
	}
*/
	bsp_end();
}
	
int main( int argc, char ** argv ) {
    size = -1;
    while(size <= 0){
    	printf("How many datapoints would you like? (Note:the amount of clusters needs to be lower or equal than the amount of  datapoints)");
		scanf("%d",&size);
		if(size <= 0){
			printf("Size cannot be negative or zero\n");
		}
	}
	fflush( stdout );
	printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs());
    scanf( "%d", &P );
    fflush( stdout );
    printf( "On how many clusters do you want the data to be divided?\n");
    scanf( "%d", &k );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }
	bsp_init( &kMeans, argc, argv );
	kMeans();
	return EXIT_SUCCESS;
}
