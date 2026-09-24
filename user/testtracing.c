#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int main () {

  int time = uptime();
  printf("Uptime: %d\n", time);
 
  syscalltrace(1);
  printf("About to call getpid\n");
 exit(0);
}
