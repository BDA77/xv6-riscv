#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    char *mem = sbrk(4096);  // Reservar una página de memoria

    printf("1. Intentando escribir en memoria...\n");
    mem[0] = 'A';
    printf("1. Escritura exitosa: %c\n", mem[0]);

    printf("2. Protegiendo memoria (mprotect)...\n");
    if (mprotect(mem, 4096) < 0) {
        printf("Error: mprotect falló\n");
        exit(1);
    }
    
    printf("3. Intentando escribir en memoria protegida (debería fallar)...\n");
    if (fork() == 0) {
        mem[0] = 'B';  // Esto debería causar un fallo de segmentación
        printf("Error: Se permitió escritura en memoria protegida\n");  // No debería imprimirse
        exit(1);
    } else {
        wait(0);
    }

    printf("4. Restaurando permisos de escritura (munprotect)...\n");
    if (munprotect(mem, 4096) < 0) {
        printf("Error: munprotect falló\n");
        exit(1);
    }

    printf("5. Intentando escribir en memoria...\n");
    mem[0] = 'C';
    printf("5. Escritura exitosa: %c\n", mem[0]);

    printf("Prueba completada.\n");
    exit(0);
}
