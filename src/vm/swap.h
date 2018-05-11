#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <stdint.h>

#include "devices/block.h"
#include "threads/vaddr.h"
/* number of sector per page */

#define SEC_PER_PAGE (PGSIZE/BLOCK_SECTOR_SIZE)

void swap_init (void);

uint32_t swap_write(void* paddr);

void swap_load (uint32_t sector, void *paddr);

void swap_free (uint32_t sector);

#endif /* vm/swap.h */
