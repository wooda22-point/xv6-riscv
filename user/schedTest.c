#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// helper function for calculating pow
int power(int base, int exp) {
  int result = 1;
  for (int i = 0; i < exp; i++) {
    result *= base;
  }
 return result;
}

// helper function for checking if num is armstrong num
int armstrong_check(int n) {
  int original = n;
  int digits = 0;
  int temp = n;
  while (temp > 0) {
    digits++;
    temp /= 10;
  }

  int sum = 0;
  temp = n;
  while (temp > 0) {
    int digit = temp % 10;
    sum += power(digit, digits);
    temp /= 10;
  }
  return sum == original;
}



int count_Armstrong(int limit) {
  int count = 0;
  for (int i = 2; i <= limit; i++) {
    if (armstrong_check(i)) {
      count++;
    }
  }
  return count;
}

int main(int argc, char*argv[]) {
  // gets the limit from cmd line
  int limit = atoi(argv[1]);
  // list of priorities and sets debug
  int priority[8] = {0,0,5,5,10,10,15,15};
  debug(1);

  // loops through and creats fork for every priority
  for (int i = 0; i < 8; i++) {
    int pid = pfork(priority[i]);
    if (pid == 0) {
      // runs armstrong if child
      int count = count_Armstrong(limit);
      printf("count = %d\n", count);
      exit(0);
    }
  }

  // wait loop for each child process
  for (int i = 0; i < 8; i++) {
    wait(0);
  }
  // changes debug and ends
  debug(0);
  exit(0);
}
