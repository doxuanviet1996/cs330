#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

void halt (void);
void exit(int status);
pid_t exec(const char * cmd_line);
int wait (pid_t pid);
bool create (const char * file , unsigned initial_size );
bool remove (const char * file );
int open (const char * file );
int filesize (int fd );
int read (int fd , void * buffer , unsigned size );
int write (int fd , const void * buffer , unsigned size );
void seek (int fd , unsigned position );
unsigned tell (int fd );
void close (int fd );

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

int check_valid(void *ptr)
{
  if(ptr >= PHYS_BASE) exit(-1);
  int res = get_user(ptr);
  if(res == -1) exit(-1);
  return res;
}

char get_arg(void *esp)
{
  check_valid(esp);
  check_valid(esp + 1);
  check_valid(esp + 2);
  check_valid(esp + 3);
  check_valid(esp + 4);
  return *(char *)esp;
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
  int call_num = (int) get_arg(f->esp);
  if(call_num == SYS_HALT)
  {
    printf("SYS_HALT!\n");
    halt();
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
  thread_exit ();
}

void halt (void)
{
  shutdown_power_off();
}
void exit(int status)
{
  struct thread *cur = thread_current();
  printf("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}
pid_t exec(const char * cmd_line)
{
  return 0;
}
int wait (pid t pid)
{
  return 0;
}
bool create (const char * file , unsigned initial_size )
{
  return 0;
}
bool remove (const char * file )
{
  return 0;
}
int open (const char * file )
{
  return 0;
}
int filesize (int fd )
{
  return 0;
}
int read (int fd , void * buffer , unsigned size )
{
  return 0;
}
int write (int fd , const void * buffer , unsigned size )
{
  return 0;
}
void seek (int fd , unsigned position )
{

}
unsigned tell (int fd )
{
  return 0;
}
void close (int fd )
{

}