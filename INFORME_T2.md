# Tarea 2: Informe de Programación de Procesos en xv6

**Boreas Duchens Arenas**

## Introducción

En este informe se detalla el proceso completo que seguí para modificar el programador de procesos en xv6 con el objetivo de implementar un sistema de prioridades. La estructura del informe se presenta siguiendo el orden cronológico de sucesos de modificación del programador de procesos. Es decir, el informé se escribió en conjunto al proceso de programación.


## Desarrollo de la Tarea

### Creación de rama para Tarea 2

En primer lugar, verifiqué en que rama me encontraba con el comando `git branch` en la terminal de VSCode.

Luego, me colocé dentro de la rama `boreas_duchens_t0` para poder crear la rama de la Tarea 2 a partir de la rama de la Tarea 0.

Después, cree la rama con el comando `git checkout -b boreas_duchens_t2`.


### Modificación de la estructura del proceso

1. **Modificaciones a archivo `proc.h`**:

    - Se añadieron las siguientes 2 líneas dentro de `struct proc{}`, para indicar los campos que se agregan a la estructura de un proceso:
      ```c
      int priority;
      int boost;
      ```


### Inicialización de la prioridad y el boost

1. **Modificaciones a archivo `proc.c`**:
    
    - Se añadieron las siguientes 2 líneas dentro de `allocproc(void)`, para indicar los valores de `priority`y `boost` con los que inicia un proceso:
      ```c
      p->priority = 0;
      p->boost = 1;
      ```


### Implementación de lógica de incremento y decremento de prioridad

1. **Modificaciones a archivo `proc.c`**:
    
    - Se añadieron las siguientes líneas dentro de `scheduler(void)`, específicamente al principio de `if(p->state == RUNNABLE){}`:
      ```c
      // Modificación: Ajustar la prioridad según boost
      p->priority += p->boost;

      // Si la prioridad alcanza 9, cambiar boost a -1
      if(p->priority >= 9) {
        p->boost = -1;
      }

      // Si la prioridad alcanza 0, cambiar boost a 1
      if(p->priority <= 0) {
        p->boost = 1;
      }

      ```


### Creación programa de prueba

1. **Creación archivo `testpriority.c`**:
    
    - En primer lugar, se creó manualmente el archivo dentro de la carpeta `user`.

2. **Incorporación a `Makefile`**:

    - Se añadió la línea `$U/testpriority\` al final de la sección `UPROGS` para incluir el programa en la compilación.

3. **Codificación de función en `testpriority.c`**:

    - La función que se codificó dentro del archivo es la siguiente:
      ```c
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
         exit(0);
      }

      ```


### Pruebas

1. **Primera ejecución de `make qemu`**:

    - Al momento de ejecutar `make qemu` ocurrió el siguiente error:
    ```bash
    collect2: error: ld returned 1 exit status
    make: *** [<builtin>: user/testpriority] Error 1
    ```

    - Luego de un poco de análisis me percaté de que la línea que se había agregado a `Makefile` era la siguiente:
    
    ```makefile
    $U/testpriority\
    ```

    - A esta línea le faltaba un guión bajo antes de la palabra `testpriority`, por lo que se arregló y quedó de la siguiente manera:

    ```makefile
    $U/_testpriority\
    ```

2. **Primera ejecución de `testpriority.c`**:

    - Al momento de ejecutar `make qemu` todo resultó correctamente.

    - Luego, se ejecutó `testpriority.c` en la terminal de xv6 por primera vez y se obtuvo lo siguiente:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso con pid 6
      Ejecutando proceso con pid 7
      Ejecutando proceso con pid 8
      Ejecutando proceso con pid 9
      Ejecutando proceso con pid 10
      Ejecutando proceso con pid 11
      Ejecutando proceso con pid 12
      Ejecutando proceso con pid 13
      Ejecutando proceso con pid 14
      Ejecutando proceso con pid 15
      Ejecutando proceso con pid 16
      Ejecutando proceso con pid 17
      Ejecutando proceso con pid 18
      Ejecutando proceso con pid 19
      Ejecutando proceso con pid 20
      Ejecutando proceso con pid 21
      Ejecutando proceso con pid 22
      Ejecutando proceso con pid 23
      Ejecutando proceso con pid 24
      Ejecutando proceso con pid 25
      $
      ```

3. **Segunda ejecución de `testpriority.c`**:

    - La segunda ejecución, sin haber terminado QEMU, se realizó de la siguiente forma:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso con pid 29
      Ejecutando proceso con pid 30
      Ejecutando proceso con pid 31
      Ejecutando proceso con pid 32
      Ejecutando proceso con pid 33
      Ejecutando proceso con pid 34
      Ejecutando proceso con pid 35
      Ejecutando proceso con pid 36
      Ejecutando proceso con pid 37
      Ejecutando proceso con pid 38
      Ejecutando proceso con pid 39
      Ejecutando proceso con pid 40
      Ejecutando proceso con pid 41
      Ejecutando proceso con pid 42
      Ejecutando proceso con pid 43
      Ejecutando proceso con pid 44
      Ejecutando proceso con pid 45
      Ejecutando proceso con pid 46
      Ejecutando proceso con pid 47
      Ejecutando proceso con pid 48
      ```

4. **Tercera ejecución de `testpriority.c`**:

    - La tercera ejecución, sin haber terminado QEMU, se realizó de la siguiente forma:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso con pid 50
      Ejecutando proceso con pid 51
      Ejecutando proceso con pid 52
      Ejecutando proceso con pid 53
      Ejecutando proceso con pid 54
      Ejecutando proceso con pid 55
      Ejecutando proceso con pid 56
      Ejecutando proceso con pid 57
      Ejecutando proceso con pid 58
      Ejecutando proceso con pid 59
      Ejecutando proceso con pid 60
      Ejecutando proceso con pid 61
      Ejecutando proceso con pid 62
      Ejecutando proceso con pid 63
      Ejecutando proceso con pid 64
      Ejecutando proceso con pid 65
      Ejecutando proceso con pid 66
      Ejecutando proceso con pid 67
      Ejecutando proceso con pid 68
      Ejecutando proceso con pid 69
      $QEMU: Terminated
      ```

De esta forma, se tiene que los procesos se encuentran ordenados con prioridad y boost de manera correcta.


### Push de la carpeta `xv6-riscv` a la rama de la Tarea 2

Luego de que todas las pruebas anduvieran bien se decidió realizar el push de la Tarea 2.

1. **Primer push**:

    - Se realizó el push a la nueva rama de mi repositorio. Se usaron los siguientes comandos en el siguiente orden:
      ```bash
      git branch
      git status
      git add .
      git commit -m "Implementación de Sistema de Prioridades"
      git push origin boreas_duchens_t2
      ```

2. **Push final**:

    - Finalmente se realizará el push final con todo el informe terminado.


## Conclusiones Finales

En conclusión, el desarrollo de este proyecto requirió un enfoque detallado y cuidadoso, ya que la manipulación de múltiples archivos del sistema xv6 implicó mantener un control riguroso sobre cada uno de los cambios realizados. La coordinación entre los archivos fue crucial para evitar errores de compilación y de ejecución.

Además, un aspecto fundamental fue el análisis del estilo de código que tenía cada archivo dentro de xv6. Estos estilos de escritura podían ser muy diferentes entre sí y determinar qué caracteres específicos colocar, en el orden correcto, era especialmente importante para no generar errores de compilación. En este sentido, llevar un registro detallado de cada paso en el informe fue de gran utilidad para identificar problemas y solucionarlos de manera eficiente durante el desarrollo.

Finalmente, aunque se evaluó la opción de modificar el archivo `testpriority.c` para mostrar la prioridad y el boost de los procesos, decidí no implementar nuevas llamadas al sistema debido a la complejidad adicional que esto implicaba. A pesar de ello, el objetivo principal del proyecto se cumplió, y la ejecución del sistema reflejó adecuadamente el comportamiento esperado.