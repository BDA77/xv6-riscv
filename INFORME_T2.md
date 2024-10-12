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

    - Se añadieron las siguientes 2 líneas dentro de `struct proc{}`:
      ```c
      int priority;
      int boost;
      ```


### Inicialización de la prioridad y el boost

1. **Modificaciones a archivo `proc.c`**:
    
    - Se añadieron las siguientes 2 líneas dentro de `allocproc(void)`:
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

    - Se añadió la línea `$U/_testpriority\` al final de la sección `UPROGS` para incluir el programa en la compilación.

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

    ```


### Pruebas

1. **Primera ejecución de `testpriority`**:

    - Al momento de ejecutar `make qemu` todo resultó correctamente.

    - Luego, se ejecutó `testpriority` en la terminal de xv6 por primera vez y se obtuvo lo siguiente:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso hijo con pid 6
      Ejecutando proceso hijo con pid 7
      Ejecutando proceso hijo con pid 8
      Ejecutando proceso hijo con pid 9
      Ejecutando proceso hijo con pid 10
      Ejecutando proceso hijo con pid 11
      Ejecutando proceso hijo con pid 12
      Ejecutando proceso hijo con pid 13
      Ejecutando proceso hijo con pid 14
      Ejecutando proceso hijo con pid 15
      Ejecutando proceso hijo con pid 16
      Ejecutando proceso hijo con pid 17
      Ejecutando proceso hijo con pid 18
      Ejecutando proceso hijo con pid 19
      Ejecutando proceso hijo con pid 20
      Ejecutando proceso hijo con pid 21
      Ejecutando proceso hijo con pid 22
      Ejecutando proceso hijo con pid 23
      Ejecutando proceso hijo con pid 24
      Ejecutando proceso hijo con pid 25
      $
      ```

2. **Sefunda ejecución de `testpriority`**:

    - La segunda ejecución, sin haber terminado QEMU, se realizó de la siguiente forma:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso hijo con pid 29
      Ejecutando proceso hijo con pid 30
      Ejecutando proceso hijo con pid 31
      Ejecutando proceso hijo con pid 32
      Ejecutando proceso hijo con pid 33
      Ejecutando proceso hijo con pid 34
      Ejecutando proceso hijo con pid 35
      Ejecutando proceso hijo con pid 36
      Ejecutando proceso hijo con pid 37
      Ejecutando proceso hijo con pid 38
      Ejecutando proceso hijo con pid 39
      Ejecutando proceso hijo con pid 40
      Ejecutando proceso hijo con pid 41
      Ejecutando proceso hijo con pid 42
      Ejecutando proceso hijo con pid 43
      Ejecutando proceso hijo con pid 44
      Ejecutando proceso hijo con pid 45
      Ejecutando proceso hijo con pid 46
      Ejecutando proceso hijo con pid 47
      Ejecutando proceso hijo con pid 48
      ```

3. **Tercera ejecución de `testpriority`**:

    - La tercera ejecución, sin haber terminado QEMU, se realizó de la siguiente forma:
      ```bash
      $ testpriority
      Iniciando prueba de prioridades...
      Ejecutando proceso hijo con pid 50
      Ejecutando proceso hijo con pid 51
      Ejecutando proceso hijo con pid 52
      Ejecutando proceso hijo con pid 53
      Ejecutando proceso hijo con pid 54
      Ejecutando proceso hijo con pid 55
      Ejecutando proceso hijo con pid 56
      Ejecutando proceso hijo con pid 57
      Ejecutando proceso hijo con pid 58
      Ejecutando proceso hijo con pid 59
      Ejecutando proceso hijo con pid 60
      Ejecutando proceso hijo con pid 61
      Ejecutando proceso hijo con pid 62
      Ejecutando proceso hijo con pid 63
      Ejecutando proceso hijo con pid 64
      Ejecutando proceso hijo con pid 65
      Ejecutando proceso hijo con pid 66
      Ejecutando proceso hijo con pid 67
      Ejecutando proceso hijo con pid 68
      Ejecutando proceso hijo con pid 69
      $QEMU: Terminated
      ```

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


## Explicación Funcionamiento del Sistema de Prioridades 

## Conclusiones Finales

En conclusión, 

Por último, una gran idea fue ir haciendo este informe al mismo tiempo que la tarea, pues si tenía algún error era mucho más sencillo identificar todo lo que había hecho para luego encontrar el problema.
