#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

/* Reads a byte at user virtual address UADDR.
UADDR must be below PHYS_BASE.
Returns the byte value if successful, -1 if a segfault
occurred. */
static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
      : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* Writes BYTE to user address UDST.
UDST must be below PHYS_BASE.
Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
      : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!\n");
  int *esp = f->esp;
  int call_num = *esp;
  if(call_num == SYS_HALT)
  {
    printf("SYS_HALT!\n");
  }
  else if(call_num == SYS_EXIT)
  {
    printf("SYS_EXIT!\n");
  }
  else if(call_num == SYS_EXEC)
  {
    printf("SYS_EXEC!\n");
  }
  else if(call_num == SYS_WAIT)
  {
    printf("SYS_WAIT!\n");
  }
  else if(call_num == SYS_CREATE)
  {
    printf("SYS_CREATE!\n");
  }
  else if(call_num == SYS_REMOVE)
  {
    printf("SYS_REMOVE!\n");
  }
  else if(call_num == SYS_OPEN)
  {
    printf("SYS_OPEN!\n");
  }
  else if(call_num == SYS_FILESIZE)
  {
    printf("SYS_FILESIZE!\n");
  }
  else if(call_num == SYS_READ)
  {
    printf("SYS_READ!\n");
  }
  else if(call_num == SYS_WRITE)
  {
    printf("SYS_WRITE!\n");
  }
  else if(call_num == SYS_SEEK)
  {
    printf("SYS_SEEK!\n");
  }
  else if(call_num == SYS_TELL)
  {
    printf("SYS_TELL!\n");
  }
  else if(call_num == SYS_CLOSE)
  {
    printf("SYS_CLOSE!\n");
  }
  else
  {
  	printf("Not known (yet) syscall.\n");
  	thread_exit ();
  }
}

