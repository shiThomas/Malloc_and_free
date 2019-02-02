#include "my_malloc.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * extract(block * curr, size_t size) {
  //check if the size is not big enough for extaction
  if (curr->size < size + sizeof(block)) {
    if (curr == head) {
      if (curr->next != NULL) {
        curr->next->prev = NULL;
      }
      head = curr->next;
    }
    else {
      //reset curr->next and curr->prev
      if (curr->next != NULL) {
        curr->next->prev = curr->prev;
      }

      if (curr->prev != NULL) {
        curr->prev->next = curr->next;
      }
    }

    curr->prev = NULL;
    curr->next = NULL;
    return curr->address;
  }
  else {
    //printf("extract and split\n");
    //extract the wanted block from linklist, and set struct variables for them
    block * block_split = (block *)(curr->address + size);
    block_split->prev = curr->prev;
    block_split->next = curr->next;

    if (curr == head) {
      head = block_split;
    }
    else {
      curr->prev->next = block_split;
    }
    //need to check NULL
    if (curr->next != NULL) {
      curr->next->prev = block_split;
    }

    //address starts at node+sizeof(blocK)
    block_split->address = (void *)(block_split) + sizeof(block);
    block_split->size = curr->size - size - sizeof(block);
    //printf("prev and next finished\n");

    //printf("split address %p\n",block_split->address);
    //printf("next address %p\n",block_split->next->address);
    //printf("curr address %p\n",curr->address);

    //printf("size %lu\n",size);
    //printf("size %lu\n",sizeof(block));

    //printf("sizeofblock %lu\n",sizeof(block));

    //printf("block_split->size %lu\n",block_split->size);
    //reset curr block
    curr->prev = NULL;
    curr->next = NULL;
    curr->size = size;
    //printf("curr->size %lu\n",curr->size);
    //printf("done extract and split\n");
    return curr->address;
  }
}

void add_blocks(block * blk) {
  if (head == NULL) {
    //printf("enter first if\n");
    blk->next = NULL;
    blk->prev = NULL;
    head = blk;
    //initialize the first block in the list
  }
  else {
    block * curr = head;
    //printf("enter first else\n");
    //move to the right position
    //curr is before blk
    while (curr->next != NULL && curr != NULL && curr->next < blk) {
      curr = curr->next;
    }

    //printf("before add in the end\n");
    //add block in the end
    if (curr->next == NULL) {
      //printf("free add block in end\n");
      curr->next = blk;
      blk->prev = curr;
      blk->next = NULL;
      // blk->address = ptr;
      //blk->size = seg_free;
    }

    //add block in between;
    else {
      //printf("free add block in between\n");
      block * blk_bet = curr->next;
      blk_bet->prev = blk;
      curr->next = blk;
      blk->next = blk_bet;
      blk->prev = curr;
      //blk->address = ptr;
      //curr->next->prev = blk;
    }
  }
}

void merge(block * blk) {
  //Merge free space
  //Merge back first then front

  if (blk->next != NULL && blk->next == blk->address + blk->size) {
    //printf("merge back\n");
    block * blk_merge = blk;
    blk_merge->size = blk->size + blk->next->size + sizeof(block);
    blk_merge->next = blk->next->next;
    if (blk_merge->next != NULL) {
      blk_merge->next->prev = blk_merge;
    }
    blk = blk_merge;
  }

  //Merge front
  // if () {
  if (blk->prev != NULL && (void *)blk == blk->prev->address + blk->prev->size) {
    //printf("add front\n");
    block * blk_merge_f = blk->prev;
    blk_merge_f->next = blk->next;
    blk_merge_f->size = blk_merge_f->size + blk->size + sizeof(block);
    if (blk->next != NULL) {
      blk->next->prev = blk_merge_f;
    }
    blk = blk_merge_f;
  }
}

//create new blocks using sbrk(size)
void * create_space(size_t size) {
  void * block_n = sbrk(size + sizeof(block));
  //set linklist struct for this new block;
  block * block_new = (block *)block_n;
  block_new->next = NULL;
  block_new->prev = NULL;
  block_new->size = size;
  //test print
  //printf("create new space\n");
  //account for block size
  block_new->address = block_n + sizeof(block);
  //printf("need space of %lu; address is %p\n",size,block_new->address);
  return block_new->address;
}

void * ff_malloc(size_t size) {
  block * curr = head;
  //test print
  //printf("malloc start\n");
  while (curr != NULL) {
    //size of block need to be taken into consideration.
    if (curr->size >= size) {
      break;
    }
    curr = curr->next;
  }
  //no available space for malloc, and sprk()is needed
  if (curr == NULL) {
    void * block_new_address = create_space(size);
    return block_new_address;
  }
  //if there is space available for malloc.
  else {
    void * newblock_address = extract(curr, size);
    return newblock_address;
  }
}

void freeBoth(void * ptr) {
  //has to be smaller than blk->address
  block * blk = (block *)(ptr - sizeof(block));
  //if there is no existing block
  //printf("curr->address: %p, curr->size: %lu\n", blk->address, (unsigned long)blk->size);
  add_blocks(blk);
  merge(blk);
}

void ff_free(void * ptr) {
  //printf("enter ff_free\n");
  freeBoth(ptr);
}

//Bestt Fit
void * bf_malloc(size_t size) {
  block * curr = head;
  //initialize the maximum value for minimum size;
  size_t min_size = LONG_MAX;
  block * min_track = NULL;
  while (curr != NULL) {
    if (curr->size >= size) {
      //break if find a size immediatiely equals curr->size
      if (curr->size == size) {
        min_track = curr;
        break;
      }

      //find the minimum avilable space
      if (curr->size < min_size) {
        min_size = curr->size;
        min_track = curr;
      }
    }
    curr = curr->next;
  }
  //no available space for malloc, and sbrk()is needed
  if (curr == NULL && min_track == NULL) {
    void * block_new_address = create_space(size);
    return block_new_address;
  }
  //if there is space available for malloc.
  else {
    //extract the wanted block from linklist, and set struct variables for them
    void * newblock_address = extract(min_track, size);
    return newblock_address;
  }
}

//bf_free same as ff_free;
void bf_free(void * ptr) {
  freeBoth(ptr);
}
//free byte + allocated byte
unsigned long get_data_segment_size() {
  unsigned long seg = sbrk(0) - (void *)head;
  return seg;
}

//free byte
unsigned long get_data_segment_free_space_size() {
  unsigned long seg_f = 0;
  block * curr = head;
  while (curr != NULL) {
    seg_f += curr->size + sizeof(block);
    curr = curr->next;
  }
  return seg_f;
}

//Test Print for list elements
void printlist() {
  block * curr = head;
  while (curr != NULL) {
    printf("curr->size: %lu, curr->address: %p\n", curr->size, curr->address);
    curr = curr->next;
  }
}
