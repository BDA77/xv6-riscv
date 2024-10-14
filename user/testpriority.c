#include "kernel/types.h"
#include "user/user.h"

void
fork_processes(int n)
{
  for(int i = 0; i < n; i++) {
    int pid = fork();
    if(pid == 0) {
      printf("Ejecutando proceso con pid %d\n", getpid());
      sleep(10);
      exit(0);
    }
    sleep(1);
  }

  // Se espera que todos los procesos hijos terminen
  for(int i = 0; i < n; i++) {
    wait(0);
  }
}

int
main()
{
  printf("Iniciando prueba de prioridades...\n");
  fork_processes(20);
  printf("Todos los procesos han terminado.\n");
  exit(0);
}
