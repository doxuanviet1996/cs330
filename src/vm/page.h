#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdlib.h>
#include <hash.h>

#define STACK_LIMIT (1<<23)

struct sup_page_table_entry
{
	void *uaddr;
	struct hash_elem elem;
};

void spt_init(struct hash *spt);
void spt_destroy(struct hash *spt);

struct sup_page_table_entry *spt_lookup(struct hash *spt, void *uaddr);
bool spt_load(struct sup_page_table_entry *spte);
bool stack_grow(void *uaddr); 

#endif /* vm/page.h */