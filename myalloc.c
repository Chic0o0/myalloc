#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Structure to save available heap size and pointer where free space starts
size_t size=0; //Amount of memory left
void *next=NULL; //Pointer to free memory
size_t times=0; //Times we call the OS

void askOS(){ //Ask OS for memory (128MB blocks)
	times++;
	size+=128*1024*1024;
	next=mmap(NULL, size, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANON, -1, 0);
	if (next == MAP_FAILED) {
		perror("Error when allocating memory\n");
		exit(-1);
	}
}

void allocateMemory(size_t bytes, char units){
	//Get desired bytes
	switch(units){
		case 'K':
			bytes=bytes*1024;
		break;
		case 'M':
			bytes=bytes*1024*1024;
		break;
		case 'G':
			bytes=bytes*1024*1024*1024;
		break;
		default:
			printf("Assuming bytes as units\n");
		break;
	}
	//Call the OS until we get enough memory
	while(bytes>=size){
		askOS();
	}
	size-=bytes; //Adjust memory we have available
	next+=bytes; //Advance free memory pointer
	printf("Number of 128M blocks reserved: %d\n"
		"Free memory pointer: %p\nBytes left inside current block: %d\n"
		,times,next,size);
}

void freeMemory(){
	if(munmap(next-(times*128*1024*1024-size),times*128*1024*1024)==-1){
		perror("Error when freeing memory\n");
		exit(-1);
	}
	next=NULL;
	times=0;
	size=0;
	askOS();
}

int main(){
	//Initialize, allocate and free required memory
	int size=0, repeat=1, free=0;
	char units='B';
	askOS();
	while(repeat){
		repeat=0;
		printf("Select size and units(K/M/G)\n");
		scanf("%d %c", &size, &units);
		allocateMemory(size,units);
		printf("Free memory?(1=yes,0=no)\n");
		scanf("%d", &free);
		if(free) freeMemory();
		printf("Continue?(1=yes,0=no)\n");
		scanf("%d",&repeat);
	}
	return 0;
}
