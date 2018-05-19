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
	// printf("Evicting..\n");
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
			else // Found one to evict.
			{
				// Swap out if it is SWAP, or FILE.
				if(spte->type == SWAP || spte->type == FILE)
				{
					spte->type = SWAP;
					spte->swap_index = swap_out(fte->frame);
				}
				// Write to file if it is MMAP.
				else if(pagedir_is_dirty(owner->pagedir, spte->uaddr))
				{
					lock_acquire(&filesys_lock);
					file_write_at(spte->file, fte->frame, spte->read_bytes, spte->ofs);
					lock_release(&filesys_lock);
				}

				list_remove(e);
				fte->spte->is_loaded = false;
				palloc_free_page(fte->frame);

				pagedir_clear_page(owner->pagedir, fte->spte->uaddr);

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

	void *frame = palloc_get_page(flags);
	if(frame == NULL) frame = frame_evict(flags);
	// else printf("Alloc\n");
	if(frame == NULL) PANIC("Frame allocation failed.\n");

	// add to frame table
	struct frame_table_entry *fte = malloc(sizeof(struct frame_table_entry));
	fte->frame = frame;
	fte->spte = spte;
	fte->owner = thread_current();

	lock_acquire(&frame_lock);
	list_push_back(&frame_table, &fte->elem);
	lock_release(&frame_lock);

	return frame;
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
			// printf("Free\n");
			list_remove(e);
			fte->spte->is_loaded = false;
			free(fte);
			palloc_free_page(frame);
			lock_release(&frame_lock);
			return;
		}
	}
	lock_release(&frame_lock);
}
