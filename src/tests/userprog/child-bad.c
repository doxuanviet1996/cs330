/* Child process run by wait-killed test.
   Sets the stack pointer (%esp) to an invalid value and invokes
   a system call, which should then terminate the process with a
   -1 exit code. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  printf("Checkpoint 0\n");
  asm volatile ("movl $0x20101234, %esp; int $0x30");
  printf("Checkpoint 1\n");
  fail ("should have exited with -1");
  printf("Checkpoint 2\n");
}
