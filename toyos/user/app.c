#include "user/user.h"

void
main(void)
{
  print("app: parent process is running\n");

  int pid = fork();
  if(pid == 0) {
    print("app: child process returned from fork with 0\n");
    exit(7);
  }

  if(pid < 0) {
    print("app: fork failed\n");
    exit(1);
  }

  print("app: parent is waiting for child\n");
  int status = 0;
  int waited = wait(&status);

  if(waited < 0) {
    print("app: wait failed\n");
    exit(1);
  }

  print("app: parent collected child and will exit\n");
  exit(0);
}
