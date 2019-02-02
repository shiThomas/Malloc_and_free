#ifndef _MY_MALLOC_H_
#define _MY_MALLOC_H_
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Define struct for linklist
struct block_t {
  size_t size;
  struct block_t * next;
  struct block_t * prev;
  void * address;
};
typedef struct block_t block;

block * head = NULL;

unsigned long seg_free = 0;

void printlist();
//First Fit
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//Bestt Fit
void * bf_malloc(size_t size);
void bf_free(void * ptr);

//free byte + allocated byte
unsigned long get_data_segment_size();

//free byte
unsigned long get_data_segment_free_space_size();

#endif
