#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int ppid = getppid();
    printf("El ProcessID del proceso padre es: %d\n", ppid);

    int n = 2; // Valor según ancestro que se quiera obtener
    int ancestor_pid = getancestor(n);
    if (ancestor_pid != -1)
        printf("El ProcessID del ancestro %d es: %d\n", n, ancestor_pid);
    else
        printf("No se encontró un ancestro en el nivel %d.\n", n);

    exit(0);
}
