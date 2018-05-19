#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <bitmap.h>

#include "devices/block.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

#define SECTOR_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

struct lock swap_lock;

struct block *swap_block;

/* A bitmap to store available frame (NOT available sector).
	 bitmap(i) = true -> frame i is used.
	 bitmap(i) = false -> frame i is free.
	 A swap block's memory unit is sector, not frame, so one bit in
	 the bitmap corresponds to 1 frame(which is normally 4 sectors). */
struct bitmap *swap_used_map;

void swap_init(void);
void swap_in(int swap_index, void *addr);
int swap_out(void *addr);

#endif /* vm/swap.h */
