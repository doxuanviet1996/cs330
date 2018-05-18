#include <hash.h>

#include "vm/page.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "threads/palloc.h"

unsigned hash_func (const struct hash_elem *e, void *aux)
{
	struct sup_page_table_entry *spte = hash_entry(e, struct sup_page_table_entry, elem);
	return hash_int(spte->uaddr);
}

bool less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
	struct sup_page_table_entry *spte_a = hash_entry(a, struct sup_page_table_entry, elem);
	struct sup_page_table_entry *spte_b = hash_entry(b, struct sup_page_table_entry, elem);
	return spte_a->uaddr < spte_b->uaddr;
}

void destroy_func (struct hash_elem *e, void *aux)
{
	struct sup_page_table_entry *spte = hash_entry(e, struct sup_page_table_entry, elem);

	free(spte);
}

void spt_init(struct hash *spt)
{
	hash_init(spt, hash_func, less_func, NULL);
}
void spt_destroy(struct hash *spt)
{
	hash_destroy(spt, destroy_func);
}

struct sup_page_table_entry *spt_lookup(void *uaddr)
{
	// Simulate a spte with same uaddr to find in hash table.
	struct sup_page_table_entry tmp;
	tmp.uaddr = pg_round_down(uaddr);
	struct hash_elem *e = hash_find(&thread_current()->spt, &tmp.elem);
	// printf("HASH FIND %x\n", tmp.uaddr);
	if(!e)
	{
		// printf("HASH FIND NOT FOUND\n");
		return NULL;
	}
	return hash_entry(e, struct sup_page_table_entry, elem);
}

bool spt_load_swap(struct sup_page_table_entry *spte)
{
	void *frame = frame_alloc(spte, PAL_USER);
	if(!frame) return false;
	if(!install_page(spte->uaddr, frame, spte->writable))
	{
		frame_free(frame);
		return false;
	}
	swap_in(spte->swap_index, spte->uaddr);
	spte->is_loaded = true;
	return true;
}

bool spt_load_file(struct sup_page_table_entry *spte)
{
	void *frame = frame_alloc(spte, PAL_USER | PAL_ZERO);
	if(!frame) return false;

	// printf("Loading file to %p %p\n", spte->uaddr, frame);
	// printf("%d %d %d\n",spte->ofs, spte->read_bytes, spte->zero_bytes);
	// lock_acquire(&filesys_lock);
	int read_bytes = file_read_at(spte->file, frame, spte->read_bytes , spte->ofs);
	// lock_release(&filesys_lock);

	if(read_bytes != spte->read_bytes)
	{
		frame_free(frame);
		return false;
	}

	if(!install_page(spte->uaddr, frame, spte->writable))
	{
		frame_free(frame);
		return false;
	}
	spte->is_loaded = true;
	return true;
}

bool spt_load(struct sup_page_table_entry *spte)
{
	if(spte == NULL) return false;

	spte->is_locked = true;
	if(spte->is_loaded) return false;

	// SWAP loading
	if(spte->type == SWAP) return spt_load_swap(spte);
	else return spt_load_file(spte);
}

struct sup_page_table_entry *spt_add_file(void *uaddr, struct file *file, int ofs,
																					int read_bytes, int zero_bytes, bool writable)
{
	if(read_bytes + zero_bytes != PGSIZE) return NULL;

	struct sup_page_table_entry *spte = malloc(sizeof(struct sup_page_table_entry));
	if(!spte) return NULL;

	spte->uaddr = pg_round_down(uaddr);
	spte->type = FILE;
	spte->is_loaded = false;
	spte->is_locked = false;
	spte->writable = writable;

	spte->file = file;
	spte->ofs = ofs;
	spte->read_bytes = read_bytes;
	spte->zero_bytes = zero_bytes;
	// printf("%d %d %d\n",ofs, read_bytes, zero_bytes);

	// printf("HASH INSERT %x\n",spte->uaddr);
	if(hash_insert(&thread_current()->spt, &spte->elem) == NULL) return spte;
}

struct sup_page_table_entry *stack_grow(void *uaddr)
{
	if(PHYS_BASE - uaddr > STACK_LIMIT) return NULL;
	struct sup_page_table_entry *spte = malloc(sizeof(struct sup_page_table_entry));
	if(!spte) return NULL;

	// printf("CHECKPOINT 1\n");
	spte->uaddr = pg_round_down(uaddr);
	spte->type = SWAP;
	spte->is_loaded = true;
	spte->is_locked = true;
	spte->writable = true;

	void *frame = frame_alloc(spte, PAL_USER);
	if(!frame)
	{
		free(spte);
		return NULL;
	}

	// printf("CHECKPOINT 2\n");
	if(!install_page(spte->uaddr, frame, true))
	{
		free(spte);
		frame_free(frame);
		return NULL;
	}
	// printf("CHECKPOINT 3\n");
	// printf("Stack grow %x\n",spte->uaddr);
	if(hash_insert(&thread_current()->spt, &spte->elem) == NULL) return spte;
}