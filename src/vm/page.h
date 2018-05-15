#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdlib.h>

enum addr_type
{
	FILE = 1;
	SWAP = 2;
};

struct sup_page_table_entry
{
	enum type;
	void *uaddr;

};

void sup_table_init();
void sup_table_destroy();


#endif /* vm/page.h */