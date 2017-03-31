/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

static int compare_bytes( const void *pa, const void *pb )
{
	int a = *(char*)pa;
	int b = *(char*)pb;

	if(a<b) {
		return -1;
	} else if(a==b) {
		return 0;
	} else {
		return 1;
	}

}

void focus_program( char *data, int length )
{
	int total=0;
	int i,j;

	srand48(38290);

	for(i=0;i<length;i++) {
		data[i] = 0;
	}

	for(j=0;j<100;j++) {
		int start = lrand48()%length;
		int size = 25;
		for(i=0;i<100;i++) {
			data[ (start+lrand48()%size)%length ] = lrand48();
		}
	}

	for(i=0;i<length;i++) {
		total += data[i];
	}

	printf("focus result is %d\n",total);
}

void sort_program( char *data, int length )
{
	int total = 0;
	int i;

	srand48(4856);

	for(i=0;i<length;i++) {
		data[i] = lrand48();
	}

	qsort(data,length,1,compare_bytes);

	for(i=0;i<length;i++) {
		total += data[i];
	}

	printf("sort result is %d\n",total);

}

void scan_program( char *cdata, int length )
{
	unsigned i, j;
	unsigned char *data = (unsigned char*) cdata;
	unsigned total = 0;

	for(i=0;i<length;i++) {
		data[i] = i%256;
	}

	for(j=0;j<10;j++) {
		for(i=0;i<length;i++) {
			total += data[i];
		}
	}

	printf("scan result is %d\n",total);
}
