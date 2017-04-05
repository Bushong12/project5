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
int diskReads, diskWrites, pageFaults;
int *frameTable = NULL;

void print_frame_table(){
  for(int i=0; i < nframes; i++){
    printf("%d\n", frameTable[i]);
  }
}

void same_num_pf_handler(struct page_table *pt, int page){
  //same number of pages and frames
  page_table_set_entry(pt, page, page, PROT_READ|PROT_WRITE);
  page_table_print(pt);
}

void rand_handler(struct page_table *pt, int page){
  int spot = (rand() % (page + 1));
  //TODO-check current bits at spot
  //if there's no bits:
  //page_table_set_entry(pt, page, spot, PROT_READ);
  //disk_read(disk, page, &physmem[spot * frame_size]);
}

void page_fault_handler( struct page_table *pt, int page ){
  printf("page fault on page #%d\n",page);
  if(npages == nframes){
    same_num_pf_handler(pt, page);
  }
  //page_table_print(pt);
  else{
    if(!strcmp(algorithm,"rand")){
      rand_handler(pt, page);
    }
    page_table_print(pt);
    exit(1);
  }
}

int main( int argc, char *argv[] ){
  if(argc!=5) {
    printf("use: virtmem <npages> <nframes> <rand|fifo|lru|custom> <sort|scan|focus>\n");
    return 1;
  }
  
  npages = atoi(argv[1]);
  nframes = atoi(argv[2]);
  if(nframes > npages){ //if more frames than pages
    nframes = npages;
  }
  frameTable = malloc(sizeof(int)*nframes);
  
  algorithm = argv[3];
  const char *program = argv[4]; 

  struct disk *disk = disk_open("myvirtualdisk",npages);
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
  
  char *physmem = page_table_get_physmem(pt);
  
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
  
  page_table_delete(pt);
  disk_close(disk);
  free(frameTable);
  
  return 0;
}
