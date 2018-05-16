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

struct sup_page_table_entry *spt_lookup(struct hash *spt, void *uaddr)
{
	struct sup_page_table_entry tmp;
	tmp.uaddr = pg_round_down(uaddr);

	struct hash_elem *e = hash_find(&thread_current()->spt, &tmp.elem);
	if(!e) return NULL;
	return hash_entry(e, struct sup_page_table_entry, elem);
}
bool spt_load(struct sup_page_table_entry *spte)
{
	return false;
}
bool stack_grow(void *uaddr)
{
	if(PHYS_BASE - uaddr > STACK_LIMIT) return false;
	struct sup_page_table_entry *spte = malloc(sizeof(struct sup_page_table_entry));

	spte->uaddr = pg_round_down(uaddr);

	void *frame = frame_alloc(spte, PAL_USER);
	if(!install_page(spte->uaddr, frame, true)) return false;
	return hash_insert(&thread_current()->spt, &spte->elem) == NULL;
}