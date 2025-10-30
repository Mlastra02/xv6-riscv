#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 10
volatile int sink = 0;

int
main(void)
{
  printf("Demo Lottery Scheduling: N=%d\n", N);

  for(int i = 0; i < N; i++){
    int pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit(1);
    }
    if(pid == 0){
      settickets(50 * (i + 1));
      for (volatile long k = 0; k < 40000000; k++)
        sink += k;
      exit(0);
    }
  }

  for(int i = 0; i < N; i++)
    wait(0);

  printf("Demo terminada. Usa Ctrl+P para ver slices.\n");
  exit(0);
}
