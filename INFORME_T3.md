# Tarea 3: Informe de Protección de Memoria en xv6

**Boreas Duchens Arenas**

## Introducción

En este informe se detalla el proceso completo que seguí para modificar xv6 con el objetivo de implementar un sistema de protección de memoria que permita marcar regiones de memoria como solo lectura. La estructura del informe se presenta siguiendo el orden cronológico de sucesos de modificación del programador de procesos. Es decir, el informé se escribió en conjunto al proceso de programación.


## Desarrollo de la Tarea

### Creación de rama para la Tarea 3

En primer lugar, verifiqué en que rama me encontraba con el comando `git branch` en la terminal de VSCode.

Luego, me colocé dentro de la rama `boreas_duchens_t0` para poder crear la rama de la Tarea 3 a partir de la rama de la Tarea 0.

Después, cree la rama con el comando `git checkout -b boreas_duchens_t3`.


### Implementación llamadas al sistema

Se implementarán llamadas al sistema para probar de forma precisa y directa la protección de memoria desde el espacio de usuario, evitando tener que modificar el kernel repetidamente para realizar pruebas. Con estas llamadas al sistema, se crea una solución más completa en la que es más sencillo comprobar el correcto funcionamiento y operar un mantenimiento.

1. **Modificaciones a archivo `syscall.h`**:

    - Se añadieron las siguientes 2 líneas al final del archivo, para así añadir la definición de estas nuevas llamadas:
      ```h
      #define SYS_mprotect   22
      #define SYS_munprotect 23
      ```

2. **Modificaciones a archivo `syscall.c`**:

    - Se añadieron las siguientes 2 líneas en el archivo, añadiendo la declaración de las funciones:
      ```c
      extern uint64 sys_mprotect(void);
      extern uint64 sys_munprotect(void);
      ```

    - Se añadieron, dentro del array `syscalls[]`, las siguientes 2 líneas:
      ```c
      [SYS_mprotect]   sys_mprotect,
      [SYS_munprotect] sys_munprotect,
      ```

3. **Modificaciones a archivo `user.h`**:

    - Se añadieron las siguientes 2 líneas al archivo, en la parte de system calls, para declarar las funciones:
      ```h
      int mprotect(void *addr, int len);
      int munprotect(void *addr, int len);
      ```

4. **Modificaciones a archivo `usys.pl`**:

    - Se añadieron las siguientes 2 líneas al final del archivo:
      ```pl
      entry("mprotect");
      entry("munprotect");
      ```

5. **Modificaciones a archivo `sysproc.c`**:

    - Al final del archivo se añadieron las siguientes líneas que definen el funcionamiento de las llamadas al sistema: 
      ```c
      uint64
      sys_mprotect(void)
      {
        uint64 addr;
        int len;
        argaddr(0, &addr);
        argint(1, &len);
        return mprotect((void *)addr, len);
      }

      uint64
      sys_munprotect(void)
      {
        uint64 addr;
        int len;
        argaddr(0, &addr);
        argint(1, &len);
        return munprotect((void *)addr, len);
      }
      ```


### Implementación funciones de protección

1. **Modificaciones a archivo `vm.c`**:

    - Se añadió la siguiente línea al inicio del archivo, para poder llamar a la estructura proceso:
    ```c
    #include "proc.h"
    ```

    - Se añadieron las funciones `int mprotect(void *addr, int len)` e `int munprotect(void *addr, int len)` al final del archivo, tal como se muestra a continuación:
      ```c
      // Establece un rango de memoria como de solo lectura para evitar modificaciones en esa sección de memoria.
      int
      mprotect(void *addr, int len)
      {
        struct proc *p = myproc();
        uint64 start = PGROUNDDOWN((uint64)addr);  // Redondear la dirección de inicio hacia abajo
        uint64 end = start + len;

        // Verificación de entrada: comprobar longitud y rangos de dirección
        if (len <= 0 || start >= p->sz || end > p->sz) {
          return -1;
        }

        for (uint64 a = start; a < end; a += PGSIZE) {
          pte_t *pte = walk(p->pagetable, a, 0); // Localizar la entrada de la tabla de páginas
          if (pte == 0) {
            return -1;
          }
          *pte &= ~PTE_W; // Desactiva el permiso de escritura en la página
        }
        return 0;
      }

      // Restaura los permisos de escritura en un rango de memoria, permitiendo lecturas y escrituras en ese espacio.
      int
      munprotect(void *addr, int len)
      {
        struct proc *p = myproc();
        uint64 start = PGROUNDDOWN((uint64)addr);  // Redondear la dirección de inicio hacia abajo
        uint64 end = start + len;

        // Verificación de entrada: comprobar longitud y rangos de dirección
        if (len <= 0 || start >= p->sz || end > p->sz) {
          return -1;
        }

        for (uint64 a = start; a < end; a += PGSIZE) {
          pte_t *pte = walk(p->pagetable, a, 0); // Localizar la entrada de la tabla de páginas
          if (pte == 0) {
            return -1;
          }
          *pte |= PTE_W; // Restaura el permiso de escritura en la página
        }
        return 0;
      }
      ```

2. **Modificaciones a archivo `defs.h`**:

    - Se añadieron las siguientes 2 líneas al final del archivo, en la sección de vm.c, siguiendo con el patrón de las funciones:
      ```h
      int             mprotect(void *, int);
      int             munprotect(void *, int);
      ```


### Implementación archivo de pruebas

1. **Creación de archivo `testprotection.c`**:

    - Se creó el archivo de pruebas y se ingresó el siguiente código dentro:
      ```c
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
      ```

2. **Incorporación a `Makefile`**:

    - Se añadió la siguiente línea al final del archivo para incluir el programa en la compilación:
      ```makefile
      $U/_testprotection\
      ```


### Pruebas

1. **Primera ejecución de `make qemu`**:

    - Al momento de ejecutar `make qemu` ocurrió el siguiente error:
      ```bash
      In file included from kernel/vm.c:8:
      kernel/proc.h:86:19: error: field ‘lock’ has incomplete type
        86 |   struct spinlock lock;
           |                   ^~~~
      make: *** [<builtin>: kernel/vm.o] Error 1
      ```

2. **Modificaciones a `proc.h`**:

    - Luego de analizar el error, se tenía que no estaba incluida la referencia a `spinlock.h`, por lo que se agregó la siguiente línea al inicio del archivo:
      ```h
      #include "spinlock.h"
      ```

3. **Segunda ejecución de `make qemu`**:

    - Al momento de ejecutar `make qemu` nuevamente ocurrieron muchísimos errores, tales como:
      ```bash
      In file included from kernel/proc.h:1,
                      from kernel/console.c:23:
      kernel/spinlock.h:7:8: error: redefinition of ‘struct spinlock’
          7 | struct spinlock {
            |        ^~~~~~~~
      kernel/defs.h:116:26: note: expected ‘struct spinlock *’ but argument is of type ‘struct spinlock *’
        116 | void            initlock(struct spinlock*, char*);
            |                          ^~~~~~~~~~~~~~~~
      cc1: all warnings being treated as errors
      make: *** [<builtin>: kernel/console.o] Error 1
      ```

4. **Modificaciones a `spinlock.h`**:

    - Luego de analizar posibles soluciones por varios minutos se llegó a que se debía modificar el archivo `spinlock.h`. Este quedó de la siguiente manera:
      ```h
      #ifndef SPINLOCK_H
      #define SPINLOCK_H

      #include "types.h"

      // Mutual exclusion lock.
      struct spinlock {
        uint locked;       // Is the lock held?

        // For debugging:
        char *name;        // Name of lock.
        struct cpu *cpu;   // The cpu holding the lock.
      };

      // Declaraciones de funciones para manipular spinlocks
      void initlock(struct spinlock *lock, char *name);
      void acquire(struct spinlock *lock);
      void release(struct spinlock *lock);
      int holding(struct spinlock *lock);

      #endif
      ```

3. **Tercera ejecución de `make qemu` y uso de `testprotection.c`**:

    - Al momento de ejecutar `make qemu` esta vez no hubo ningún problema y xv6 funcionó correctamente.

    - Tras esto, se pobró el funcionamiento de la protección con el archivo `testprotection.c`. Al hacer esto, el resultado obtenido por la terminal fue el siguiente:
      ```bash
      $ testprotection
      1. Intentando escribir en memoria...
      1. Escritura exitosa: %c
      2. Protegiendo memoria (mprotect)...
      3. Intentando escribir en memoria protegida (debería fallar)...
      usertrap(): unexpected scause 0xf pid=4
                  sepc=0x64 stval=0x4000
      4. Restaurando permisos de escritura (munprotect)...
      5. Intentando escribir en memoria...
      5. Escritura exitosa: %c
      Prueba completada.
      $
      ```
    
    - Lo anterior demuestra la correcta implementación del sistema de protección de memoria.


### Push de la carpeta `xv6-riscv` a la rama de la Tarea 3

Luego de que todas las pruebas anduvieran bien se decidió realizar el push de la Tarea 3.

1. **Primer push**:

    - Se realizó el push a la nueva rama de mi repositorio. Se usaron los siguientes comandos en el siguiente orden:
      ```bash
      git branch
      git status
      git add .
      git commit -m "Implementación de Protección de Memoria"
      git push origin boreas_duchens_t3
      ```


### Push Final de la Tarea 3

Finalmente, se realizará el push final con todo el informe terminado.


## Conclusiones Finales

En conclusión, el desarrollo de un sistema de protección de memoria en xv6 resultó ser un proyecto técnicamente desafiante y enriquecedor. La modificación de xv6 para soportar la marcación de regiones de memoria como solo lectura no solo requirió ajustar múltiples archivos y estructuras internas, sino también enfrentar y resolver errores complejos, especialmente en el archivo `spinlock.h`. Este archivo presentó dificultades particulares que demandaron un análisis minucioso para integrar los cambios sin comprometer el funcionamiento de otras partes del sistema. Estos ajustes permiten ahora que el sistema gestione la memoria de forma más robusta y que los procesos se limiten en sus permisos, incrementando la seguridad y la estabilidad del kernel.

Otro aspecto fundamental fue el análisis y adaptación al estilo de código en cada archivo de xv6, que presentaba variaciones significativas. Determinar los caracteres específicos y el orden adecuado dentro de cada función era esencial para evitar errores de compilación y asegurar que el sistema funcionara como se esperaba. La consistencia en la estructura de las funciones de xv6 fue un desafío considerable, ya que demandó un rigor especial en la codificación para mantener la coherencia con el resto del sistema. Llevar un registro detallado de cada paso en el informe fue crucial para identificar y resolver problemas de manera eficiente durante el desarrollo, facilitando así el proceso de implementación y depuración.
