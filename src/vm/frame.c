#include "vm/frame.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/vaddr.h"

void frame_init()
{
	list_init(&frame_table);
	lock_init(&frame_lock);
}

void *frame_evict(enum palloc_flags flags)
{
	lock_acquire(&frame_lock);
	struct list_elem *e = list_begin(&frame_table);

	// Frame eviction using second chance algorithm
	int loop = 0;
	while(loop < 2)
	{
		struct frame_table_entry *fte = list_entry(e, struct frame_table_entry, elem);

		if(!fte->spte->is_locked)
		{
			struct thread *owner = fte->owner;
			fte->spte->uaddr;
			if(pagedir_is_accessed(owner->pagedir, fte->spte->uaddr))
				pagedir_set_accessed(owner->pagedir, fte->spte->uaddr, false);
			else // Found one.
			{
				fte->spte->type = 0;
				fte->spte->swap_index = swap_out(fte->frame);
				fte->spte->is_loaded = false;
				list_remove(e);
				pagedir_clear_page(owner->pagedir, fte->spte->uaddr);
				palloc_free_page(fte->frame);
				free(fte);
				return palloc_get_page(flags);
			}
		}

		e = list_next(e);
		if(e == list_end(&frame_table))
		{
			loop++;
			e = list_begin(&frame_table);
		}
	}
	return NULL;
}

void *frame_alloc(struct sup_page_table_entry *spte, enum palloc_flags flags)
{
	ASSERT(flags & PAL_USER);

	void *addr = palloc_get_page(flags);
	if(addr == NULL) addr = frame_evict(flags);
	if(addr == NULL)
		PANIC("Frame allocation failed.\n");

	// add to frame table
	struct frame_table_entry *fte = malloc(sizeof(struct frame_table_entry));
	fte->frame = addr;
	fte->spte = spte;
	fte->owner = thread_current();

	lock_acquire(&frame_lock);
	list_push_back(&frame_table, &fte->elem);
	lock_release(&frame_lock);

	return addr;
}

void frame_free(void *frame)
{
	struct list_elem *e;

	lock_acquire(&frame_lock);

	for(e=list_begin(&frame_table); e!=list_end(&frame_table); e=list_next(e))
	{
		struct frame_table_entry *fte = list_entry(e, struct frame_table_entry, elem);
		if(fte->frame == frame)
		{
			list_remove(e);
			free(fte);
			palloc_free_page(frame);
			return;
		}
	}
}
