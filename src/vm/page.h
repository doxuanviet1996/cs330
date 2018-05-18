#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdlib.h>
#include <hash.h>

#include "filesys/file.h"

#define STACK_LIMIT (1<<23)

enum spte_type
{
	SWAP = 0,
	FILE = 1
};

struct sup_page_table_entry
{
	void *uaddr;						// User virtual address

	enum spte_type type;		// 0 for swap, 1 for file.
	bool is_loaded;					// True if loaded to RAM.
	bool is_locked;					// True if locked to the table (can't be evicted).
	bool writable;					// True if page is writable.

	// Swap data
	int swap_index;

	// File data
	struct file *file;			// File pointer.
	int ofs;								// Offset to start reading at.
	int read_bytes;					// Number of bytes to read.
	int zero_bytes;					// Number of zero bytes at the end.

	struct hash_elem elem;
};

void spt_init(struct hash *spt);
void spt_destroy(struct hash *spt);

struct sup_page_table_entry *spt_lookup(void *uaddr);
bool spt_load(struct sup_page_table_entry *spte);
struct sup_page_table_entry *spt_add_file(void *uaddr, struct file *file,
																					int ofs, int read_bytes,
																					int zero_bytes, bool writable);
struct sup_page_table_entry *stack_grow(void *uaddr); 

#endif /* vm/page.h */