#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>


int main () {
  // outputs uptime
  int time = uptime();
  printf("Process Uptime: %d\n", time);
  printf("Testing syscalltrace() implementation...\n");
  // starts tracing syscalls
  syscalltrace(1);
  sbrk(0);
  getpid();
  pause(5);
  // tracks the result of fork to check if child or parent process
  int tracker = fork();
  if (tracker == 0) {
    // if child then run multiple syscalls to show tracing is off
    pause(5);
    printf("Child Tracking Check Running Pause Syscall\n");
    pause(5);
    exit(0);
  } else {
    // if parent then run getpid to show tracing is still there
    printf("Parent Tracking Check Running Getpid Syscall\n");
    getpid();
    wait(0);
  }

  // last syscall to match sample output
  uptime();
  exit(0);
}
