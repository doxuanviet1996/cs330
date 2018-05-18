#include "filesys/file.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"

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
		struct sup_page_table_entry *spte = fte->spte;
		if(!spte->is_locked)
		{
			struct thread *owner = fte->owner;
			if(pagedir_is_accessed(owner->pagedir, spte->uaddr))
				pagedir_set_accessed(owner->pagedir, spte->uaddr, false);
			else // Found one.
			{
				if(spte->type == SWAP)
				{
					spte->type = 0;
					spte->swap_index = swap_out(fte->frame);
				}
				else if(pagedir_is_dirty(owner->pagedir, spte->uaddr))
				{
					lock_acquire(&filesys_lock);
					int write_bytes = file_write_at(spte->file, fte->frame, spte->read_bytes, spte->ofs);
					if(write_bytes != spte->read_bytes) printf("Suspicious..\n");
					lock_release(&filesys_lock);
				}
				fte->spte->is_loaded = false;
				list_remove(e);
				pagedir_clear_page(owner->pagedir, fte->spte->uaddr);
				palloc_free_page(fte->frame);
				free(fte);
				lock_release(&frame_lock);
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
	lock_release(&frame_lock);
	return NULL;
}

void *frame_alloc(struct sup_page_table_entry *spte, enum palloc_flags flags)
{
	if(!(flags & PAL_USER)) return NULL;

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
