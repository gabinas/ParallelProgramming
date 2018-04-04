#include <stdlib.h>
#include <stdio.h>
#include "include/mcbsp.h"
int size;
int P;
int c;
int r;
typedef struct cluster{
int clustersize;
int *clusterdata;


}cluster;


int * createArray(int s, int range) {
	int i=0;
	int *dataAddress = calloc(s, sizeof(int));
	for (i = 0; i < s-1; i++){
		dataAddress[i] = rand()%range;
		}
		return dataAddress;	
}

int *getcenteroid(int *sum, int *length,int k,int range){
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
	
int distance(int data,int*centeroid,int clustersize){
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



cluster *clusterinitializer(int k,int s){
	cluster *buckets=calloc(k,sizeof(cluster));
	int i,j;
	for(i=0;i<k;i++){
		buckets[i].clusterdata=calloc(s/2,sizeof(int));
		buckets[i].clustersize=0;
		}

		
return buckets;}


int centroidchecker(int old[],int new[],int len){
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
		data=createArray(size,100);
		
		for(i=0;i<procs;i++){
			
			bsp_put(i,&data[block*i],localdata,0,block*sizeof(int));
		}
		}
		bsp_sync();
		for(i=0;i<length;i++){
	centeroids[i]=rand()%range;
	}




		
		while(lock<length){
			
			
			if(iteration>0){
			centroidcheck=getcenteroid(localsum,localsize,block,range);}
			clust=clusterinitializer(length,size);
		for(i=0;i<length;i++){
		location=distance(localdata[i],centeroids,length);
		clust[location].clusterdata[clust[location].clustersize]=localdata[i];
		clust[location].clustersize+=1;
		}

	for(i=0;i<length;i++){
		printf("From proc %d At centeroid %d : %d\n",id,i,centeroids[i]);
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
for(i=0;i<length;i++){
	*(clustsize+i*procs+id)=clust[i].clustersize;
		for(j=0;j<clust[i].clustersize;j++){
			*(sum+i*procs+id)+=clust[i].clusterdata[j];
			
}}
bsp_sync();	

		for(j=0;j<procs;j++){
		for(i=0;i<length;i++){
		bsp_put(j,&*(sum+i*procs+id),sum,(i*procs+id)*sizeof(int),sizeof(int));
		bsp_put(j,&*(clustsize+i*procs+id),clustsize,(i*procs+id)*sizeof(int),sizeof(int));
		}}
	
	bsp_sync(); 

	if(id==0){
	for(i=0;i<length;i++){
		printf("From proc %d\n",id);
		for(j=0;j<procs;j++){
		printf("cluster[%d].p[%d] = %d and clustersize = %d\n",i,j,*(sum + i*procs + j),*(clustsize+i*procs+j));
			}
	}}
	bsp_sync();
	localsum=calloc(length,sizeof(int));
	localsize=calloc(length,sizeof(int));
		for(i=0;i<length;i++){
		for(j=0;j<procs;j++){
			localsum[i]+=*(sum+i*procs+j);
			localsize[i]+=*(clustsize+i*procs+j);
			}}
			if(id==0){
	for(i=0;i<length;i++){
		printf("localsum[%d] =%d and localsize[%d]=%d\n",i,localsum[i],i,localsize[i]);
			}}
		
		
		centeroids=getcenteroid(localsum,localsize,block,range);
		lock=centroidchecker(centroidcheck,centeroids,length);
		iteration+=1;
	if(id==0){
	for(i=0;i<length;i++){
		printf("centeroids[%d] =%d\n",i,centeroids[i]);
	}}
		
		
		bsp_sync();

	
		}
		int m;
			for(i=0;i<length;i++){
				if(clust[i].clustersize>0){
		printf("From proc %d At centeroid %d : %d\n",id,i,centeroids[i]);
		for(m=0;m<procs;m++){
			if(id==m){
		for(j=0;j<clust[i].clustersize;j++){
			
		printf("data[%d][%d] = %d\n",i,j,clust[i].clusterdata[j]);
		}}}}
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