#include "kernel/types.h"
#include "user/user.h"

void
fork_processes(int n)
{
  for(int i = 0; i < n; i++) {
    int pid = fork();
    if(pid == 0) {
      sleep(i * 10);
      printf("Ejecutando proceso hijo con pid %d\n", getpid());
      sleep(10);
      exit(0);
    }
  }

  // Se espera que todos los hijos terminen
  for(int i = 0; i < n; i++) {
    wait(0);
  }
}

int
main()
{
  printf("Iniciando prueba de prioridades...\n");
  fork_processes(20);
  exit(0);
}
