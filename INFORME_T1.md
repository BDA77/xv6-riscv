# Tarea 1: Informe de Implementación de Llamadas al Sistema en xv6

Boreas Duchens Arenas

## Introducción

En este informe se detalla el proceso completo que seguí para implementar llamadas al sistema en xv6. La estructura del informe se presenta siguiendo el orden cronológico del proceso de implementación de llamdas al sistema. Es decir, el informé se escribió en conjunto al proceso de implementación.


## Proceso de Implementación

### Creación de rama para Tarea 1

Para crear la rama simplemente ingrese el comando `git checkout -b boreas_duchens_t1` en la terminal de VSCode.


### Implementación de la llamada al sistema `int getppid(void)`

1. **Modificaciones a archivo `syscall.h`**:
    
    - En primer lugar, se añadió la línea `#define SYS_getppid 22` al final del archivo para añadir la definición de esta nueva llamada.

2. **Modificaciones a archivo `syscall.c`**:
    
    - Se añadió la línea `extern uint64 sys_getppid(void)` en el archivo, añadiendo la declaración de la función.

    - Se añadió la línea `[SYS_getppid] sys_getppid,` dentro del array `syscalls[]`.

3. **Modificaciones a archivo `sysproc.c`**:
    
    - Se añadió la función `uint64 sys_getppid(void)` en el archivo. La función es la siguiente:
        ```
        uint64
        sys_getppid(void)
        {
            struct proc *p = myproc();
            return p->parent ? p->parent->pid : 0;
        }
        ```

4. **Modificaciones a archivo `usys.s`**:

    - Se añadieron las siguientes líneas de código al final del archivo, las cuáles seguían con la estructura predeterminada de llamadas al sistema:
        ```
        .global getppid
        getppid:
         li a7, SYS_getppid
         ecall
         ret
        ```

5. **Modificaciones a archivo `user.h`**:

    - Se añadío la línea `int getppid(void);` a la sección `//system calls` del archivo.

6. **Comprobación del funcionamiento de xv6**:

    - En este punto ejecuté `make qemu` para verificar que todo estaba funcionando en orden y efectivamente todo funcionaba correctamente.


### Creación y Codificación de archivo `yosoytupadre.c`

1. **Creación archivo `yosoytupadre.c`**:

    - En primer lugar, se creó manualmente el archivo dentro de la carpeta `user`.

2. **Agregar a `Makefile`**:

    - Se añadió la línea `$U/_yosoytupadre\` al final de la sección `UPROGS` para incluir el programa en la compilación.

3. **Codificación de función en `yosoytupadre.c`**:

    - La función que se codificó dentro del archivo es la siguiente:
        ```
        #include "kernel/types.h"
        #include "kernel/stat.h"
        #include "user/user.h"

        int main(void) {
            int ppid = getppid();
            printf("El ProcessID del proceso padre es: %d\n", ppid);
            exit(0);
        }
        ```


### Primeras Pruebas
    
1. **Problemas con archivo `usys.s`**:

    - En este punto me di cuenta, revisando la carpeta `user`, que el archivo `usys.s` no estaba presente ahí. Tras esto ejecuté `make qemu` para ver si es que la compilación seguía funcionando y la terminal me mostró el siguiente error:
        ```
        riscv64-linux-gnu-ld: user/yosoytupadre.o: in function `main':
        /home/boreasxv6/XV6/xv6-riscv/user/yosoytupadre.c:6:(.text+0x8): undefined reference to `getppid'
        make: *** [Makefile:100: user/_yosoytupadre] Error 1
        ```

    - Tras esto me di cuenta de que el archivo `usys.s` se creó nuevamente, pero este no presentaba las modificaciones que le había realizado, por lo que le agregué las líneas de código correspondientes:
        ```
        .global getppid
        getppid:
         li a7, SYS_getppid
         ecall
         ret
        ```

2. **Pruebas con `yosoytupadre.c`**:

    - Después de esto la ejecución de xv6 con `make qemu` funcionó correctamente y pude probar la función `int getppid(void)` con `yosoytupadre.c`. Esto me entregó lo siguiente en la terminal:
        ```
        $ yosoytupadre
        El ProcessID del proceso padre es: 2
        $ yosoytupadre
        El ProcessID del proceso padre es: 2
        ```

3. **Solución a problema de archivo `usys.s` con `usys.pl`**:

    - Luego, noté que el archivo `usys.s` se elimina cada vez que se ejecuta `make clean`, por lo que tuve que investigar una forma en la que no tenga que volver a agregar manualmente las líneas de código a este archivo.

    - La forma de hacer esto es agregar la línea `entry("getppid");` al final del archivo `usys.pl`.

    - Ahora, al momento de hacer `make clean` y luego `make qemu`, la ejecución funciona perfectamente y de forma automática.


### Implementación de la llamada al sistema `int getancestor(int)`

Para este punto se realizarán los mismos pasos que en la implementación de `int getppid(void)` considerando todo lo aprendido.

1. **Modificaciones a archivo `syscall.h`**:
    
    - En primer lugar, se añadió la línea `#define SYS_getancestor 23` al final del archivo para añadir la definición de esta nueva llamada.

2. **Modificaciones a archivo `syscall.c`**:
    
    - Se añadió la línea `extern uint64 sys_getancestor(void)` en el archivo, añadiendo la declaración de la función.

    - Se añadió la línea `[SYS_getancestor] sys_getancestor,` dentro del array `syscalls[]`.

3. **Modificaciones a archivo `sysproc.c`**:
    
    - Se añadió la función `uint64 sys_getancestor(void)` en el archivo. La función es la siguiente:
        ```
        uint64
        sys_getancestor(void)
        {
            int n;
            if(argint(0, &n) < 0)
                return -1;

            struct proc *p = myproc();
            for(int i = 0; i < n; i++){
                if(p->parent)
                    p = p->parent;
                else
                    return -1; // No hay más ancestros
            }
            return p->pid;
        }
        ```

4. **Modificaciones a archivo `usys.pl`**:

    - Se agregó la línea `entry("getancestor");` al final del archivo.

5. **Modificaciones a archivo `user.h`**:

    - Se añadío la línea `int getancestor(int);` a la sección `//system calls` del archivo.


### Pruebas Finales

1. **Cambios en `yosoytupadre.c`**:

    - Para probar `int getancestor(int)` al mismo tiempo que `int getppid(void)` se cambió la función `int main(void)` de `yosoytupadre.c` por la siguiente:
        ```
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
        ```

2. **Fallas de ejecución**:

    - Finalmente, se intentó ejecutar xv6 con `make clean` y luego `make qemu` pero aparecieron muchísimos errores por pantalla. La mayoría de allos apuntaban al archivo `syscall.c`.

    - Después de varios minutos buscando el error me di cuenta que había borrado sin querer el `;` que debía estar al final de la declaración `extern uint64 sys_getppid(void);` del archivo `syscall.c`.

    - Luego de esto ejecute `make clean` y `make qemu` y me apareció el siguiente error:
        ```
        kernel/sysproc.c: In function ‘sys_getancestor’:
        kernel/sysproc.c:35:6: error: void value not ignored as it ought to be
        35 |   if(argint(0, &n) < 0)
            |      ^~~~~~~~~~~~~
        make: *** [<builtin>: kernel/sysproc.o] Error 1
        ```

    - De inmediato noté que era un problema con la función `uint64 sys_getancestor(void)`, por lo que luego de unos minutos de análisis de las otras funciones que estaban en el archivo `sysproc.c`, me di cuenta que las que usaban el comando `argint` lo usaban de una manera diferente.
    
    - En general se colocaba lo siguiente:
        ```
        int n;
        argint(0, &n);
        // Resto de la función
        ```

    - Yo había colocado `(argint(0, &n)` dentro de un `if()`, lo que al parecer no es correcto. Me di cuenta que el resto de funciones declaraba el `(argint(0, &n)` en una línea antes de hacer cualquier tipo de operación y luego simplemente se utilizaba `n` como parámetro suelto.
    
    - Después de todo este análisis probé cambiando ese detalle en la función `uint64 sys_getancestor(void)` de la siguiente forma:
        ```
        // Pasé de esto:
        int n;
        if(argint(0, &n) < 0)
            return -1;

        // A esto:
        int n;
        argint(0, &n);
        if(n < 0)
            return -1;
        ```

    - Finalmente probé ejecutar `make clean` y `make qemu` y todo compiló correctamente.

2. **Ejecución de pruebas finales**:

    - Ya con xv6 ejecutándose, se pudieron probar las funciones `int getppid(void)` y `int getancestor(int)` al mismo tiempo con `yosoytupadre.c`. En la primera ejecución la terminal me entregó lo siguiente:
        ```
        $ $ yosoytupadre
        El ProcessID del proceso padre es: 2
        El ProcessID del ancestro 2 es: 1
        ```

    - Luego probé cambiar el valor `n` del archivo `yosoytupadre.c` a distintos valores. Estos son algunos de los resultados de esos cambios:
        - Con `n = 1`
            ```
            $ yosoytupadre
            El ProcessID del proceso padre es: 2
            El ProcessID del ancestro 1 es: 2
            ```
        
        - Con `n = 3`
            ```
            $ yosoytupadre
            El ProcessID del proceso padre es: 2
            No se encontró un ancestro en el nivel 3.
            ```

        - Con `n = 0`
            ```
            $ yosoytupadre
            El ProcessID del proceso padre es: 2
            El ProcessID del ancestro 0 es: 3
            ```

        - Con `n = -1`
            ```
            $ yosoytupadre
            El ProcessID del proceso padre es: 2
            No se encontró un ancestro en el nivel -1.
            ```

    - Por útlimo, se hizo una prueba cambiando el código de `yosoytupadre.c` por el siguiente (Se debe considerar que esté código se utilizó esta única vez y no será el que se suba a la rama del repositorio de GitHub):
        ```
        int main(void) {
            int pid = getppid();
            printf("El PID del proceso padre es: %d\n", pid);

            for (int i = 0; i <= 4; i++) {
                int ancestor = getancestor(i);
                printf("Ancestro %d: %d\n", i, ancestor);
            }

            exit(0);
        }
        ```

    - El resultado de esta prueba final fue el siguiente:
        ```
        $ yosoytupadre
        El PID del proceso padre es: 2
        Ancestro 0: 3
        Ancestro 1: 2
        Ancestro 2: 1
        Ancestro 3: -1
        Ancestro 4: -1
        ```


### Push de la carpeta `xv6-riscv` a la rama de la Tarea 1

Finalmente se realizó el push a la nueva rama de mi repositorio. Se usaron los siguientes comandos en el siguiente orden:

```


```



