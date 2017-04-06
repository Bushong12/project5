/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//Global Variables
int npages;
int nframes;
const char *algorithm;
int diskReads, diskWrites, pageFaults, availFrames;
int *frameTable = NULL;
struct disk *disk; //?????
char *physmem;

void initialize_frame_table(){
  int i;
  for(i=0; i < nframes; i++){
    //    printf("%d\n", frameTable[i]);
    frameTable[i] = -1;
  }
}

int table_full(){
  int i;
  int tableFull = 1;
  for(i = 0; i < nframes; i++){
    if(frameTable[i] == -1){
      tableFull = 0;
    }
  }
  return tableFull;
}

void same_num_pf_handler(struct page_table *pt, int page){
  //same number of pages and frames
  page_table_set_entry(pt, page, page, PROT_READ|PROT_WRITE);
  page_table_print(pt);
  pageFaults++;
}

void rand_handler(struct page_table *pt, int page){
  printf("in rand_handler\n");
  int frame, bits, i, j;
  page_table_get_entry(pt, page, &frame, &bits); //USE FRAME
  printf("just got page table entry for page: %d\n", page);
  printf("bits: %d\n", bits);

  //checking to see if the page is in frameTable
  int found = 0;
  for(i = 0; i < nframes; i++){
    if(frameTable[i] == page){
      printf("found page at frame: %d\n", i);
      found = 1;
    }
  }

  //we didn't find it in frameTable
  if(!found){
    if(bits == 0){
      for(j = 0; j < nframes; j++){
	printf("frameTable[%d]: %d\n", j, frameTable[j]);
	if(frameTable[j] == -1){ //we found an empty spot!
	  page_table_set_entry(pt, page, j, PROT_READ);
	  frameTable[j] = page;
	  availFrames--;
	  printf("just set entry for page: %d\n", page);
	  printf("put the page at spot: %d\n", j);
	  disk_read(disk, page, &physmem[j*nframes]);
	  printf("i think i read to disk?\n");
      	  break;
	}
      }
      //if the table is full!!!
      if(table_full()){
	//int pg = (rand() % (npages + 1)); //page to remove
	int fr = (rand() % (nframes + 1));
	int pg = frameTable[fr];
	disk_write(disk, pg, &physmem[fr*npages]);
	disk_read(disk, page, &physmem[fr*npages]);
	page_table_set_entry(pt, page, fr, PROT_READ);
	frameTable[fr] = page;
	page_table_set_entry(pt, pg, 0, 0); //??what does this line do
      }
    } else if(bits == 1){ //only read bits set and table not full
      page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
    } else if(bits == 3){ //read and write bits set, table not full
      page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE|PROT_EXEC);
    }
  }

  if(found){
    /*    if(bits == 0){
      disk_write(disk, pg, &physmem[fspot*npages]);
      disk_read(disk, page, &physmem[fspot*npages]);
      page_table_set_entry(pt, page, fspot, PROT_READ);
      frameTable[fspot] = page;
      page_table_set_entry(pt, pg, fspot, 0); //??what does this line do
      }*/
    if(bits == 1){
      page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
    } else if(bits == 3){
      page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE|PROT_EXEC);
    }
  }
}

void fifo_handler(struct page_table *pt, int page){

}

void page_fault_handler( struct page_table *pt, int page ){
  printf("page fault on page #%d\n",page);
  if(npages == nframes){//might have to move
    same_num_pf_handler(pt, page);
  }
  else{
    if(!strcmp(algorithm, "rand")){
      rand_handler(pt, page);
      printf("\n");
    }
    else if(!strcmp(algorithm, "fifo"))
      fifo_handler(pt, page);
    page_table_print(pt);
    //exit(1);
    return;
  }
}

int main( int argc, char *argv[] ){
  pageFaults = 0;
  diskReads = 0;
  diskWrites = 0;
  if(argc!=5) {
    printf("use: virtmem <npages> <nframes> <rand|fifo|lru|custom> <sort|scan|focus>\n");
    return 1;
  }
  
  npages = atoi(argv[1]);
  nframes = atoi(argv[2]);
  if(nframes > npages){ //if more frames than pages
    nframes = npages;
  }
  frameTable = malloc(sizeof(int)*(nframes-1));
  initialize_frame_table();
  availFrames = nframes - 1;
  algorithm = argv[3];
  const char *program = argv[4]; 

  //  struct disk *disk = disk_open("myvirtualdisk",npages);
  disk = disk_open("myvirtualdisk", npages);
  if(!disk) {
    fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
    return 1;
  }  
  struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
  if(!pt) {
    fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
    return 1;
  }
  
  char *virtmem = page_table_get_virtmem(pt);
  
  //char *physmem = page_table_get_physmem(pt);
  physmem = page_table_get_physmem(pt);
  
  if(!strcmp(program,"sort")) {
    sort_program(virtmem,npages*PAGE_SIZE);
    
  } else if(!strcmp(program,"scan")) {
    scan_program(virtmem,npages*PAGE_SIZE);
    
  } else if(!strcmp(program,"focus")) {
    focus_program(virtmem,npages*PAGE_SIZE);
    
  } else {
    fprintf(stderr,"unknown program: %s\n",argv[3]);
    return 1;
  }
  printf("%d\n", pageFaults);
  page_table_delete(pt);
  disk_close(disk);
  free(frameTable);
  
  return 0;
}
