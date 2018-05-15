#include "vm/frame.h"
#include "vm/page.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
#include "threads/pte.h"

void frame_init()
{
	list_init(&frame_table);
	lock_init(&frame_lock);
}

void *frame_alloc(struct sup_page_table_entry *spte, enum palloc_flags flags)
{
	assert(flags & PAL_USER);

	void *addr = palloc_get_page(flags);
	if(addr == NULL)
	{
		// try evict.
		return NULL;
	}
	return addr;
}

void *frame_evict()
{
	return NULL;
}

void frame_free(void *frame)
{
	palloc_free_page(frame);
}
