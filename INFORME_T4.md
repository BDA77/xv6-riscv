# Tarea 4: Informe de Implementación de Permisos Básicos en xv6

**Boreas Duchens Arenas**

## Introducción

En este informe se detalla el proceso completo que seguí para realizar una correcta implementación de permisos básicos en xv6. Los cuales tienen como objetivo modificar archivos para darles acceso de solo lectura o lectura/escritura, y agregar un nuevo permiso especial para hacerlo inmutable. La estructura del informe se presenta siguiendo el orden cronológico de sucesos de implementación de permisos básicos. Es decir, el informé se escribió en conjunto al proceso de desarrollo.


## Desarrollo de la Tarea

### Creación de rama para la Tarea 4

En primer lugar, verifiqué en que rama me encontraba con el comando `git branch` en la terminal de VSCode.

Luego, me colocé dentro de la rama `boreas_duchens_t0` para poder crear la rama de la Tarea 4 a partir de la rama de la Tarea 0.

Después, cree la rama con el comando `git checkout -b boreas_duchens_t4`.


### Modificación de la estructura `inode`

1. **Modificaciones a archivo `file.h`**:

      - Se añadió la siguiente línea dentro de la estructura `inode`, agregando un campo de permisos a la estructura:
         ```h
         int permissions;    // File permissions (0: none, 1: r, 2: w, 3: rw, 5: immutable)
         ```

2. **Modificaciones a archivo `sysfile.c`**:

      - Se añadió la siguiente línea dentro de la función que crea los inodos `static struct inode* create()`, para setear el valor por defecto en los permisos:
         ```c
         ip->permissions = 3; // Valor por defecto
         ```


### Verificación de permisos

1. **Modificaciones a archivo `sysfile.c`**:

      - Se añadieron las siguientes líneas dentro de la llamada al sistema `uint64 sys_open()`:
         ```c
         // Condicionales que verifican los permisos
         if(ip->permissions == 0){
            iunlockput(ip);
            end_op();
            return -1;
         }

         if(ip->permissions == 1 && (omode & (O_WRONLY | O_RDWR))){
            iunlockput(ip);
            end_op();
            return -1;
         }

         if (ip->permissions == 2 && ((omode & (O_RDONLY | O_RDWR)))){
            iunlockput(ip);
            end_op();
            return -1;
         }
         
         if (ip->permissions == 5 && (omode & (O_RDWR | O_WRONLY))){
            iunlockput(ip);
            end_op();
            return -1;
         }
         ```
      
      - Las líneas de código anteriores se colocaron exactamente entre los grupos de líneas que se muestran a continuación:
         ```c
         if(ip->type == T_DEVICE){
            f->type = FD_DEVICE;
            f->major = ip->major;
         } else {
            f->type = FD_INODE;
            f->off = 0;
         ```

         ```c
         }
         f->ip = ip;
         f->readable = !(omode & O_WRONLY);
         f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
         ```


### Implementación llamada al sistema para cambiar permisos

1. **Modificaciones a archivo `syscall.h`**:

      - Se añadió la siguiente línea al final del archivo, para así agregar la definición de esta nueva llamada:
         ```h
         #define SYS_chmod  22
         ```

2. **Modificaciones a archivo `syscall.c`**:

      - Se añadió la siguiente línea en el archivo, agregando la declaración de la función:
         ```c
         extern uint64 sys_chmod(void);
         ```

      - Se añadió, dentro del array `syscalls[]`, la siguiente línea:
         ```c
         [SYS_chmod]   sys_chmod,
         ```

3. **Modificaciones a archivo `user.h`**:

      - Se añadió, en la parte de system calls, la siguiente línea al archivo, para así declarar la función:
         ```h
         int chmod(const char*, int);
         ```

4. **Modificaciones a archivo `usys.pl`**:

      - Se añadió la siguiente línea al final del archivo:
         ```pl
         entry("chmod");
         ```

5. **Modificaciones a archivo `sysfile.c`**:

      - Al final del archivo se añadieron las siguientes líneas que definen el funcionamiento de la llamada al sistema: 
         ```c
         uint64
         sys_chmod(void)
         {
         char path[MAXPATH];
         int mode;
         struct inode *ip;
         argstr(0, path, MAXPATH);
         argint(1, &mode);
         begin_op();
         if((ip = namei(path)) == 0){
            end_op();
            return -1;
         }
         ilock(ip);
         if(ip->permissions == 5){
            iunlockput(ip);
            end_op();
            return -1;
         } 
         ip->permissions = mode;
         iupdate(ip);
         iunlockput(ip);
         end_op();
         return 0;
         }
         ```


### Implementación archivo de pruebas

1. **Creación de archivo `testpermissions.c`**:

      - Se creó el archivo de pruebas y se ingresó el siguiente código dentro:
         ```c
         #include "kernel/types.h"
         #include "kernel/stat.h"
         #include "kernel/fcntl.h"
         #include "kernel/fs.h"
         #include "user.h"

         int main(int argc, char *argv[]) {
            char *path = "/archivo";
            int fd;

            // Crear archivo
            fd = open(path, O_CREATE | O_RDWR);
            if (fd < 0) {
               printf("Error al crear el archivo\n");
               exit(1);
            }
            printf("Archivo creado correctamente\n");
            close(fd);

            // Escribir en el archivo
            fd = open(path, O_RDWR);
            if (write(fd, "Contenido inicial\n", 18) != 18) {
               printf("Error al escribir en el archivo\n");
               close(fd);
               exit(1);
            }
            close(fd);
            printf("Escritura inicial realizada\n");

            // Cambiar a solo lectura
            if (chmod(path, 1) < 0) {
               printf("Error al cambiar a solo lectura\n");
               exit(1);
            }
            printf("Permisos cambiados a solo lectura\n");

            // Intentar escribir con permisos de solo lectura
            fd = open(path, O_WRONLY);
            if (fd < 0) {
               printf("No se puede escribir con solo lectura: éxito\n");
            } else {
               printf("Error: se pudo escribir con solo lectura\n");
               close(fd);
            }

            // Cambiar a lectura/escritura
            if (chmod(path, 3) < 0) {
               printf("Error al cambiar a lectura/escritura\n");
               exit(1);
            }
            printf("Permisos cambiados a lectura/escritura\n");

            // Validar escritura con permisos de lectura/escritura
            fd = open(path, O_WRONLY);
            if (fd < 0 || write(fd, "Contenido final\n", 16) != 16) {
               printf("Error al escribir con permisos de lectura/escritura\n");
               close(fd);
               exit(1);
            }
            printf("Escritura validada con permisos lectura/escritura\n");
            close(fd);

            // Cambiar a inmutable
            if (chmod(path, 5) < 0) {
               printf("Error al cambiar a inmutable\n");
               exit(1);
            }
            printf("Permisos cambiados a inmutable\n");

            // Intentar escribir con inmutable
            fd = open(path, O_WRONLY);
            if (fd < 0) {
               printf("No se puede escribir con inmutabilidad: éxito\n");
            } else {
               printf("Error: se pudo escribir con inmutabilidad\n");
               close(fd);
            }

            // Intentar cambiar permisos con inmutabilidad
            if (chmod(path, 3) < 0) {
               printf("Inmutabilidad confirmada\n");
            } else {
               printf("Error: permisos cambiados pese a inmutabilidad\n");
            }

            printf("Pruebas finalizadas\n");
            exit(0);
         }
         ```

2. **Incorporación a `Makefile`**:

      - Se añadió la siguiente línea al final del archivo para incluir el programa en la compilación:
         ```makefile
         $U/_testpermissions\
         ```


### Pruebas

1. **Primera ejecución de `make qemu`**:

      - Al momento de ejecutar `make qemu` ocurrió el siguiente error:
         ```bash
         nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 1954 total 2000
         mkfs: mkfs/mkfs.c:150: main: Assertion `strlen(shortname) <= DIRSIZ' failed.
         make: *** [Makefile:145: fs.img] Aborted (core dumped)
         make: *** Deleting file 'fs.img'
         ```

2. **Cambio de nombre a archivo `testpermissions.c` y modificación a `Makefile`**:

      - Resulta que al parecer el nombre de los archivos tiene un tamaño máximo de 14 caracteres, por lo que se cambió el nombre del archivo a `testperms.c`.

      - Se cambió la línea al final del archivo que incluye el programa en la compilación por la siguiente:
         ```makefile
         $U/_testperms\
         ```

3. **Segunda ejecución de `make qemu`**:

      - Al momento de ejecutar `make qemu` todo funcionó correctamente.

      - A la hora de escribir `testperms` en la consola se mostró lo siguiente:
         ```bash
         $ testperms
         Archivo creado correctamente
         Escritura inicial realizada
         Permisos cambiados a solo lectura
         No se puede escribir con solo lectura: éxito
         Permisos cambiados a lectura/escritura
         Escritura validada con permisos lectura/escritura
         Permisos cambiados a inmutable
         No se puede escribir con inmutabilidad: éxito
         Inmutabilidad confirmada
         Pruebas finalizadas
         $
         ```


### Push de la carpeta `xv6-riscv` a la rama de la Tarea 4

Luego de que todas las pruebas anduvieran bien se decidió realizar el push de la Tarea 4.

1. **Primer push**:

      - Se realizó el push a la nueva rama de mi repositorio. Se usaron los siguientes comandos en el siguiente orden:
         ```bash
         git branch
         git status
         git add .
         git commit -m "Implementación de Permisos Básicos"
         git push origin boreas_duchens_t4
         ```


### Push Final de la Tarea 4

Finalmente, se realizará el push final con todo el informe terminado.


## Conclusiones Finales

En conclusión, la implementación de permisos básicos en xv6 fue un proceso desafiante y enriquecedor que permitió comprender en profundidad el manejo de archivos y la estructura del sistema operativo. El objetivo principal se logró exitosamente: modificar la gestión de archivos para incluir permisos específicos (lectura, escritura y un estado inmutable) que restringen las operaciones según las reglas definidas.

Una de las principales dificultades fue identificar el lugar adecuado para implementar las verificaciones de permisos. Inicialmente, se consideró añadirlas en cada función asociada, pero esto habría resultado en una redundancia innecesaria y un código más complejo. Finalmente, se determinó que realizar las verificaciones dentro de la función `uint64 sys_open()` en el archivo `sysfile.c` simplificaba significativamente la lógica y mejoraba la eficiencia del sistema. Este cambio permitió centralizar el control de acceso, asegurando que todos los intentos de abrir un archivo pasaran por estas verificaciones.

Otro desafío fue entender por qué esta implementación de llamada al sistema se realizó en `sysfile.c` en lugar de `sysproc.c`, como era esperado inicialmente. La razón se encuentra en que `sysfile.c` es el módulo responsable del manejo de archivos y, por tanto, es el lugar natural para gestionar las verificaciones relacionadas con los permisos de acceso a estos. Este aprendizaje refuerza la importancia de analizar la estructura y propósito de cada módulo antes de realizar modificaciones en un sistema operativo. En ese sentido, durante el desarrollo también se enfrentaron problemas de conocimiento técnico, como el límite de caracteres para nombres de archivos en xv6, lo cual generó un error inesperado al compilar. Esta dificultad fue superada de manera sencilla, pero no quita el hecho de que la rigurosidad en ese aspecto sea fundamental.

Finalmente, este proyecto no solo permitió implementar nuevas funcionalidades, sino que también brindó valiosas lecciones sobre el diseño modular de sistemas operativos, el manejo de errores y la importancia de la documentación para guiar y justificar decisiones de diseño. En este caso particular, el haber llevado un registro detallado de cada paso en el informe fue crucial para identificar y resolver problemas de manera eficiente durante el desarrollo, facilitando así el proceso de implementación.
