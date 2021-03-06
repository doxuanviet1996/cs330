#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdlib.h>
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

/*  */

struct frame_table_entry
{
	void *frame;													/* Physical address of the frame. */
	struct sup_page_table_entry *spte;		/* The corresponding sup page table entry. */
	struct thread *thread;								/* The thread owning this frame. */
	struct list_elem elem;								/* Element in the frame_table list. */
};

struct list frame_table;

struct lock frame_lock;

void frame_init(void);
void *frame_alloc(enum palloc_flags flags, struct sup_page_table_entry *spte);
void frame_evict();
void frame_free(void *frame);



#endif /* vm/frame.h */