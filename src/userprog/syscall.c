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
int exec(const char * cmd_line);
int wait (int pid);
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

int get_arg(void *esp)
{
  check_valid(esp);
  check_valid(esp + 1);
  check_valid(esp + 2);
  check_valid(esp + 3);
  return *(int *)esp;
}
void get_args(void *esp, int *args, int cnt)
{
  while(cnt--)
  {
    *args++ = get_arg(esp);
    esp += 4;
  }
}

syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  int args[4];
  void *esp = f->esp;
  int call_num = get_arg(esp);
  esp += 4;
  if(call_num == SYS_HALT)
  {
    halt();
  }
  else if(call_num == SYS_EXIT)
  {
    int status = get_arg(esp);
    exit(status);
  }
  else if(call_num == SYS_EXEC)
  {
    get_args(esp, args, 1);
    f->eax = exec(args[0]);
  }
  else if(call_num == SYS_WAIT)
  {
    get_args(esp, args, 1);
    f->eax = wait(args[0]);
  }
  else if(call_num == SYS_CREATE)
  {
    get_args(esp, args, 2);
    f->eax = create(args[0], args[1]);
  }
  else if(call_num == SYS_REMOVE)
  {
    get_args(esp, args, 1);
    f->eax = remove(args[0]);
  }
  else if(call_num == SYS_OPEN)
  {
    get_args(esp, args, 1);
    f->eax = open(args[0]);
  }
  else if(call_num == SYS_FILESIZE)
  {
    get_args(esp, args, 1);
    f->eax = filesize(args[0]);
  }
  else if(call_num == SYS_READ)
  {
    get_args(esp, args, 3);
    f->eax = read(args[0], args[1], args[2]);
  }
  else if(call_num == SYS_WRITE)
  {
    get_args(esp, args, 3);
    f->eax = write(args[0], args[1], args[2]);
  }
  else if(call_num == SYS_SEEK)
  {
    get_args(esp, args, 2);
    f->eax = seek(args[0], args[1]);
  }
  else if(call_num == SYS_TELL)
  {
    get_args(esp, args, 1);
    f->eax = tell(args[0]);
  }
  else if(call_num == SYS_CLOSE)
  {
    get_args(esp, args, 1);
    f->eax = close(args[0]);
  }
  else
  {
  	printf("Not known (yet) syscall.\n");
  	thread_exit ();
  }
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
int exec(const char *cmd_line)
{
  return 0;
}
int wait (int pid)
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
  if (fd == STDOUT_FILENO)
  {
    putbuf(buffer, size);
    return size;
  }
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