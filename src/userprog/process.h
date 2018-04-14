#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"

struct child_process
{
	int tid;
	int load_status;
	int exit_status;
	int exit_ret_val;
	struct semaphore load_sema;
	struct semaphore exit_sema;
	struct list_elem elem;
}

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct child_process *process_add_child(int child_tid);
struct child_process *process_get_child(int child_tid);
struct child_process *process_remove_child(int child_tid);

#endif /* userprog/process.h */
