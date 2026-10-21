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
  int limit = 1000000;
  printf("count = %d\n", count_Armstrong(limit));
  exit(0);

}
