#include "vm/swap.h"

void swap_init()
{
	lock_init(&swap_lock);

	swap_block = block_get_role(BLOCK_SWAP);
	if(!swap_block) return;

	int swap_size = block_size(swap_block)/SECTOR_PER_PAGE;
	swap_used_map = bitmap_create(swap_size);
	if(!swap_used_map) return;
}

/* Swap in the frame starting at swap_index to addr. KERNEL PANIC if fail. */
void swap_in(int swap_index, void *addr)
{
	lock_acquire(&swap_lock);

	if(bitmap_test(swap_used_map, swap_index) == false)
		PANIC("Swapping in a free page.\n");
	bitmap_flip(swap_used_map, swap_index);

	int i;
	for(i=0; i<SECTOR_PER_PAGE; i++)
	{
		void *cur_addr = addr + i*BLOCK_SECTOR_SIZE;
		int cur_sector = swap_index*SECTOR_PER_PAGE + i;
		block_read(swap_block, cur_sector, cur_addr);
	}
	
	lock_release(&swap_lock);
}

/* Swap out a frame starting at addr and return the swap index
	 where it is swap to. KERNEL PANIC if fail. */
int swap_out(void *addr)
{
	if(!swap_block || !swap_used_map)
		PANIC("SWAP partition not initialized (or failed).\n");

	lock_acquire(&swap_lock);
	int swap_index = bitmap_scan_and_flip(swap_used_map, 0, 1, false);

	if(swap_index == BITMAP_ERROR)
		PANIC("SWAP partition full.\n");

	/* Copy block sectors starting from addr to swap_index*SECTOR_PER_PAGE */

	int i;
	for(i=0; i<SECTOR_PER_PAGE; i++)
	{
		void *cur_addr = addr + i*BLOCK_SECTOR_SIZE;
		int cur_sector = swap_index*SECTOR_PER_PAGE + i;
		block_write(swap_block, cur_sector, cur_addr);
	}
	lock_release(&swap_lock);

	return swap_index;
}