#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdlib.h>
#include <hash.h>

#define STACK_LIMIT (1<<23)

enum spte_type
{
	SWAP = 0,
	FILE = 1
};

struct sup_page_table_entry
{
	void *uaddr;						// User virtual address

	enum spte_type type;		// 0 for swap, 1 for file
	bool is_loaded;					// True if loaded to RAM
	bool is_locked;					// True if locked to the table (can't be evicted)
	bool writable;					// True if page is writable

	int swap_index;

	struct hash_elem elem;
};

void spt_init(struct hash *spt);
void spt_destroy(struct hash *spt);

struct sup_page_table_entry *spt_lookup(struct hash *spt, void *uaddr);
bool spt_load(struct sup_page_table_entry *spte);
bool stack_grow(void *uaddr); 

#endif /* vm/page.h */