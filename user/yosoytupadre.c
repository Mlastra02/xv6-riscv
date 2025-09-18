#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int me  = getpid();
  int dad = getppid();
  printf("yo=%d, ppid=%d\n", me, dad);
  printf("ancestor(0)=%d\n", getancestor(0));
  printf("ancestor(1)=%d\n", getancestor(1));
  printf("ancestor(2)=%d\n", getancestor(2));
  exit(0);
}
