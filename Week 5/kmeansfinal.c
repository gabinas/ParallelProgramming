#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
int size;
int P;
int c;
int r;
typedef struct cluster{							//I created this structure as it was easier to visualize the 2d array while creating it dynamically
int clustersize;					
int *clusterdata;


}cluster;


int * createArray(int s, int range) {			//This creates our inital data.
	int i=0;
	int *dataAddress = calloc(s, sizeof(int));
	for (i = 0; i < s-1; i++){
		dataAddress[i] = rand()%range;
		}
		return dataAddress;	
}

int *getcenteroid(int *sum, int *length,int k,int range){		//THis method calculates the centeroid based off of grouping.
int *centeroids=calloc(k,sizeof(int));
int i;

	for(i=0;i<k;i++){
		if(length[i]==0){
			centeroids[i]=0;
		}
		else{
		centeroids[i]=sum[i]/length[i];}
		}
		return centeroids;
	}
	
int distance(int data,int*centeroid,int clustersize){			//This method determines the distance from a centeroid to the data point.
	int closest,i,k,location;
	location=0;
	k=clustersize;
	int*record=calloc(k,sizeof(int));
	for(i=0;i<k;i++){
		record[i]=data-centeroid[i];
		record[i]=abs(record[i]);
		}
		
		closest=record[0];
	for(i=1;i<k;i++){
		if(record[i]<closest){
			closest=record[i];
			location=i;
		}
		}
	return location;
}



cluster *clusterinitializer(int k,int s){					//This initializes the data structure created.
	cluster *buckets=calloc(k,sizeof(cluster));
	int i,j;
	for(i=0;i<k;i++){
		buckets[i].clusterdata=calloc(s,sizeof(int));
		buckets[i].clustersize=0;
		}

		
return buckets;}


int centroidchecker(int old[],int new[],int len){		//This method checks if the centeroids have stabilized.
int i;
int lock=0;


for(i=0;i<len;i++){
	if(old[i]==new[i]){
	lock+=1;
	
	}


}	
return lock;
}	
	

void kmeans(){
	bsp_begin(P);
	int procs=bsp_nprocs();
	int id=bsp_pid();
	int *data;
	int block=size/procs;
	int i=0,j=0,location=0,lock=0,length=c,iteration=0;
	int range=r;
	cluster*clust=clusterinitializer(length,size);
	int *sum=(int*)malloc(length*procs*sizeof(int));
	int *localsum=calloc(length,sizeof(int));
	int*localsize=calloc(length,sizeof(int));
	int *centeroids=calloc(length,sizeof(int));
	int *centroidcheck=calloc(length,sizeof(int));
	int *localdata=calloc(block,sizeof(int));
	int *clustsize=(int*)malloc(length*procs*sizeof(int));
	
	bsp_push_reg(localdata,block*sizeof(int));
	bsp_push_reg(sum,length*procs*sizeof(int));
	bsp_push_reg(clustsize,length*procs*sizeof(int));
	bsp_sync();
	
	
	if(id==0){
		data=createArray(size,range);										//getting initial data
		
		for(i=0;i<procs;i++){
			
			bsp_put(i,&data[block*i],localdata,0,block*sizeof(int));
		}
		}
		bsp_sync();
		for(i=0;i<length;i++){							//creating initial centeroids which is based off the range and the amount of processors.
	centeroids[i]=(range/block)*(i+1);
	}




		
		while(lock<length){							//this loop runs until centeroids have stabilized.
			
			
			if(iteration>0){
			centroidcheck=getcenteroid(localsum,localsize,block,range);}			//getting centeroids to check against new centeroids.
			
			
			clust=clusterinitializer(length,size);
		for(i=0;i<block;i++){
		location=distance(localdata[i],centeroids,length);								//this loop determines the centeroid the data point should be sent and sends it.
		clust[location].clusterdata[clust[location].clustersize]=localdata[i];
		clust[location].clustersize+=1;
		}
		

	for(i=0;i<length;i++){
		printf("From proc %d At centeroid %d : %d\n",id,i,centeroids[i]);			//Print out for visualization.
		for(j=0;j<clust[i].clustersize;j++){
		printf("data[%d][%d] = %d\n",i,j,clust[i].clusterdata[j]);
			}
}

 

												
										//I had to do this because it wouldn't initialize the values to 0 on processor 0 cluster 0
										//This may be a bug and to reproduce just block comment this code.
	for(i=0;i<length;i++){
		for(j=0;j<procs;j++){
		*(sum+i*procs+j)=0;
		*(clustsize+i*procs+j)=0;
			}
	}
	
bsp_sync();
for(i=0;i<length;i++){									//THis loop sums each individual cluster.
	*(clustsize+i*procs+id)=clust[i].clustersize;			//and sums each cluster size.
		for(j=0;j<clust[i].clustersize;j++){
			*(sum+i*procs+id)+=clust[i].clusterdata[j];
			
}}
bsp_sync();	

		for(j=0;j<procs;j++){
		for(i=0;i<length;i++){
		bsp_put(j,&*(sum+i*procs+id),sum,(i*procs+id)*sizeof(int),sizeof(int));				//This sends the cluster sums and sizes to each processor.
		bsp_put(j,&*(clustsize+i*procs+id),clustsize,(i*procs+id)*sizeof(int),sizeof(int));
		}}
	
	bsp_sync(); 

	if(id==0){
	for(i=0;i<length;i++){
		
		for(j=0;j<procs;j++){																							//This is for visualization of local sums of centeroids and sizes
		printf("cluster[%d].p[%d].clustersum = %d and clustersize = %d\n",i,j,*(sum + i*procs + j),*(clustsize+i*procs+j));
			}
	}}
	bsp_sync();
	localsum=calloc(length,sizeof(int));						//This loop calculates the sum of all processors:- cluster values and cluster sizes
	localsize=calloc(length,sizeof(int));
		for(i=0;i<length;i++){
		for(j=0;j<procs;j++){
			localsum[i]+=*(sum+i*procs+j);
			localsize[i]+=*(clustsize+i*procs+j);
			}}
			
	
		
		
		centeroids=getcenteroid(localsum,localsize,block,range);
		lock=centroidchecker(centroidcheck,centeroids,length);				//Check to see if centeroids are stable
		iteration+=1;
	if(id==0){
	for(i=0;i<length;i++){
		printf("centeroids[%d] =%d\n",i,centeroids[i]);    
	}}
		
		
		bsp_sync();

	
		}
		
			for(i=0;i<length;i++){
				if(clust[i].clustersize>0){										//Final visualization of break clusters and data from individual processors.
		printf("From proc %d At centeroid %d : %d\n",id,i,centeroids[i]);
		
			
		for(j=0;j<clust[i].clustersize;j++){
			
		printf("data[%d][%d] = %d\n",i,j,clust[i].clusterdata[j]);
		}}
}

bsp_sync();
printf("\n\n\n\n");
	if(id==0){
	clust=clusterinitializer(length,size);										//Since it would cost too much for me to put the data back on processor 0
		for(i=0;i<size;i++){													//I used the original data and the stabilized centeroids to have one final clustering 
		location=distance(data[i],centeroids,length);							//which I use to display the results.
		clust[location].clusterdata[clust[location].clustersize]=data[i];
		clust[location].clustersize+=1;
		}

					for(i=0;i<length;i++){
				
		printf("At centeroid %d : %d\n",i,centeroids[i]);
		
			
		for(j=0;j<clust[i].clustersize;j++){
			
		printf("data[%d][%d] = %d\n",i,j,clust[i].clusterdata[j]);
		}
}
	
	}

















	bsp_pop_reg(localdata);
	bsp_pop_reg(sum);
	bsp_pop_reg(clustsize);
	bsp_sync(); 
	
	bsp_end();


}



































int main(int argc, char ** argv){
	printf("What is the size of the array you would like? ");
	scanf("%d",&size);fflush(stdout);
	printf("How many clusters would you like to use?");
	scanf("%d",&c);fflush(stdout);
	printf("Please specify the upper limit of the domain [n]? [0.........n]  ");
	scanf("%d",&r);fflush(stdout);
	
	printf( "How many threads do you want started? There are %d cores available.\n", bsp_nprocs() );
    fflush( stdout );
    scanf( "%d", &P );
    if( P == 0 || P > bsp_nprocs() ) {
        fprintf( stderr, "Cannot start %d threads.\n", P );
        return EXIT_FAILURE;
    }

	bsp_init( &kmeans, argc, argv );

	kmeans();
	
    return EXIT_SUCCESS;
	
		
	}