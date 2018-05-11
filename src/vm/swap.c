#include "vm/swap.h"
#include <bitmap.h>

/* Swap table. */
static struct bitmap *swap_table;

static struct block *swap_disk;

void 
swap_init(void)
{
	swap_disk = block_get_role (BLOCK_SWAP);
	swap_table = bitmap_create (block_size (swap_disk));
}

/* write into swap disk */
uint32_t
swap_write(void *paddr)
{
	int i;
	uint32_t sector = bitmap_scan_and_flip (swap_table, 0, SEC_PER_PAGE, false);
	if (sector == BITMAP_ERROR)
		PANIC ("ran out of swap slots!");

	for (i = 0; i < SEC_PER_PAGE; i++)
		block_write (swap_disk, sector + i, paddr + i * BLOCK_SECTOR_SIZE);
	return sector;
}

/* load from swap disk, remove from swap_table */
void
swap_load(uint32_t sector, void *paddr)
{
	int i;
	for (i = 0; i < SEC_PER_PAGE; i++)
		block_read (swap_disk, sector + i, paddr + i * BLOCK_SECTOR_SIZE);
	bitmap_set_multiple (swap_table, sector, SEC_PER_PAGE, false);
}

void
swap_free (uint32_t sector)
{
	bitmap_set_multiple (swap_table, sector, SEC_PER_PAGE, false);
}
