# Tarea 5: Informe de Implementación de Sistema de Comunicación entre Procesos en xv6

**Boreas Duchens Arenas**

## Introducción

En este informe se detalla el proceso completo que se siguió para realizar una correcta implementación de un sistema de comunicación entre procesos (IPC) en xv6, mediante una cola de mensajes. El objetivo es permitir que los procesos intercambien información utilizando llamadas al sistema. A continuación, se describe en detalle cada etapa del desarrollo, documentada de manera cronológica en conjunto con el progreso de implementación.


## Desarrollo de la Tarea

### Creación de rama para la Tarea 5

En primer lugar, verifiqué en que rama me encontraba con el comando `git branch` en la terminal de VSCode.

Luego, me colocé dentro de la rama `boreas_duchens_t0` para poder crear la rama de la Tarea 5 a partir de la rama de la Tarea 0.

Después, cree la rama con el comando `git checkout -b boreas_duchens_t5`.


### Implementación de cola de mensajes

1. **Modificaciones a archivo `types.h`**:

      - Dentro del archivo se añadió la estructura del mensaje:
         ```h
         typedef struct message{
            int sender_pid;
            char content[128];
         } message;
         ```         

2. **Modificaciones a archivo `proc.c`**:

      - Al inicio del archivo se añadió la siguiente línea, para declarar la cola de mensajes de manera global:
         ```c
         struct message msg_queue[MSG_QUEUE_SIZE];
         int queue_start = 0;
         int queue_end = 0;

         struct spinlock queue_lock;
         ```

      - Al final del archivo se añadió la siguiente función para así inicializar la cola y su spinlock:
         ```c
         void
         init_msg_queue()
         {
            initlock(&queue_lock, "msg_queue_lock");
            queue_start = 0;
            queue_end = 0;
         }
         ```

3. **Modificaciones a archivo `proc.h`**:

      - Dentro del archivo se añadió la definición del spinlock:
         ```h
         extern struct spinlock queue_lock;
         ```

4. **Modificaciones a archivo `defs.h`**:

      - Dentro del archivo se añadió la definición del spinlock:
         ```h
         extern message msg_queue[32];
         extern int queue_start;
         extern int queue_end;

         void init_msg_queue(void);

         #define MSG_QUEUE_SIZE 32
         ```

5. **Modificaciones a archivo `main.c`**:

      - Al final de la función `void main()` y antes de que se llame a `scheduler()`se añadió la siguiente línea:
         ```c
         init_msg_queue(); // Inicia la cola de mensajes
         ```


### Implementación de llamadas al sistema

1. **Modificaciones a archivo `syscall.h`**:

      - Se añadieron las siguientes líneas al final del archivo, para así agregar la definición de estas nuevas llamadas:
         ```h
         #define SYS_send  22
         #define SYS_receive  23
         ```

2. **Modificaciones a archivo `syscall.c`**:

      - Se añadieron las siguientes líneas en el archivo, agregando la declaración de la función:
         ```c
         extern uint64 sys_send(void);
         extern uint64 sys_receive(void);
         ```

      - Se añadieron, dentro del array `syscalls[]`, las siguientes líneas:
         ```c
         [SYS_send]    sys_send,
         [SYS_receive] sys_receive,
         ```

3. **Modificaciones a archivo `user.h`**:

      - Se añadieron, en la parte de system calls, las siguiente líneas al archivo, para así declarar la función:
         ```h
         int send(int pid, char* msg);
         int receive(char* buffer, int size);
         ```

4. **Modificaciones a archivo `usys.pl`**:

      - Se añadieron las siguientes líneas al final del archivo:
         ```pl
         entry("send");
         entry("receive");
         ```

5. **Modificaciones a archivo `sysproc.c`**:

      - Al final del archivo se añadieron las siguientes líneas que definen el funcionamiento de las llamadas al sistema: 
         ```c
         uint64
         sys_send(void)
         {
            int receiver_pid;
            uint64 msg_ptr; // Dirección del mensaje en espacio de usuario
            
            argint(0, &receiver_pid);
            argaddr(1, &msg_ptr);
            // Obtener argumentos de la llamada al sistema
            if (receiver_pid < 0 || msg_ptr < 0)
               return -1;

            // Copiar el mensaje desde el espacio de usuario al kernel
            char message[128];
            if (copyin(myproc()->pagetable, message, msg_ptr, sizeof(message)) < 0)
               return -1;

            // Manejar la cola de mensajes
            acquire(&queue_lock);

            if ((queue_end + 1) % MSG_QUEUE_SIZE == queue_start) {
               // La cola está llena
               release(&queue_lock);
               return -1;
            }

            msg_queue[queue_end].sender_pid = myproc()->pid;
            safestrcpy(msg_queue[queue_end].content, message, sizeof(message));
            queue_end = (queue_end + 1) % MSG_QUEUE_SIZE;

            wakeup(&msg_queue); // Despertar a cualquier proceso bloqueado
            release(&queue_lock);

            return 0;
         }

         uint64
         sys_receive(void)
         {
            uint64 buffer_ptr; // Dirección del buffer en espacio de usuario
            
            argaddr(0, &buffer_ptr);
            // Obtener el argumento de la llamada al sistema
            if (buffer_ptr < 0)
               return -1;

            // Manejar la cola de mensajes
            acquire(&queue_lock);

            while (queue_start == queue_end) {
               // La cola está vacía, bloquear el proceso
               sleep(&msg_queue, &queue_lock);
            }

            int sender_pid = msg_queue[queue_start].sender_pid;
            char message[128];
            safestrcpy(message, msg_queue[queue_start].content, sizeof(message));
            queue_start = (queue_start + 1) % MSG_QUEUE_SIZE;

            release(&queue_lock);

            // Copiar el mensaje al espacio de usuario
            if (copyout(myproc()->pagetable, buffer_ptr, message, sizeof(message)) < 0)
               return -1;

            return sender_pid;
         }
         ```


### Implementación archivo de pruebas

1. **Creación de archivo `testipc.c`**:

      - Se creó el archivo de pruebas y se ingresó el siguiente código dentro:
         ```c
         #include "kernel/types.h"
         #include "kernel/stat.h"
         #include "user/user.h"
         #include <stdio.h>

         int main(int argc, char *argv[]) {
            if (fork() == 0) {
               // Proceso lector
               char buffer[128];
               for (int i = 0; i < 5; i++) {
                  int sender_pid = receive(buffer, sizeof(buffer)); // Ajusta los parámetros según tu implementación
                  printf("Mensaje recibido de %d: %s\n", sender_pid, buffer);
               }
               exit(0);
            } else {
               // Proceso escritor
               for (int i = 0; i < 5; i++) {
                  char msg[128];
                  snprintf(msg, sizeof(msg), "Mensaje %d", i);
                  send(getpid(), msg); // Ajusta los parámetros según tu implementación
                  sleep(10);
               }
               wait(0);
            }
            exit(0);
         }
         ```

2. **Incorporación a `Makefile`**:

      - Se añadió la siguiente línea al final del archivo para incluir el programa en la compilación:
         ```makefile
         $U/_testipc\
         ```


### Pruebas

1. **Primera ejecución de `make qemu`**:

      - Al momento de ejecutar `make qemu` aparecieron los siguientes errores:
         ```bash
         In file included from user/testipc.c:4:
         /usr/riscv64-linux-gnu/include/stdio.h:357:12: error: conflicting types for ‘fprintf’; have ‘int(FILE * restrict,  const char * restrict, ...)’
         357 | extern int fprintf (FILE *__restrict __stream,
               |            ^~~~~~~
         In file included from user/testipc.c:3:
         ./user/user.h:34:6: note: previous declaration of ‘fprintf’ with type ‘void(int,  const char *, ...)’
            34 | void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
               |      ^~~~~~~
         /usr/riscv64-linux-gnu/include/stdio.h:363:12: error: conflicting types for ‘printf’; have ‘int(const char * restrict, ...)’
         363 | extern int printf (const char *__restrict __format, ...);
               |            ^~~~~~
         ./user/user.h:35:6: note: previous declaration of ‘printf’ with type ‘void(const char *, ...)’
            35 | void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
               |      ^~~~~~
         In file included from /usr/riscv64-linux-gnu/include/stdio.h:980:
         /usr/riscv64-linux-gnu/include/bits/stdio2.h:77:1: error: conflicting types for ‘fprintf’; have ‘int(FILE * restrict,  const char * restrict, ...)’
            77 | fprintf (FILE *__restrict __stream, const char *__restrict __fmt, ...)
               | ^~~~~~~
         ./user/user.h:34:6: note: previous declaration of ‘fprintf’ with type ‘void(int,  const char *, ...)’
            34 | void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
               |      ^~~~~~~
         /usr/riscv64-linux-gnu/include/bits/stdio2.h:84:1: error: conflicting types for ‘printf’; have ‘int(const char * restrict, ...)’
            84 | printf (const char *__restrict __fmt, ...)
               | ^~~~~~
         ./user/user.h:35:6: note: previous declaration of ‘printf’ with type ‘void(const char *, ...)’
            35 | void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
               |      ^~~~~~
         make: *** [<builtin>: user/testipc.o] Error 1
         ```

2. **Modificaciones a archivo `testipc.c`**:

      - Luego de minutos de análisis, me di cuenta que no era buena idea usar `<stdio.h>`, por lo que se decidió cambiar el archivo de la siguiente manera:
         ```c
         #include "kernel/types.h"
         #include "kernel/stat.h"
         #include "user/user.h"

         void itoa(char *buf, int num) {
            char temp[12];
            int i = 0, j;
            if (num == 0) {
               buf[0] = '0';
               buf[1] = '\0';
               return;
            }
            while (num > 0) {
               temp[i++] = '0' + (num % 10);
               num /= 10;
            }
            for (j = 0; j < i; j++) {
               buf[j] = temp[i - j - 1];
            }
            buf[i] = '\0';
         }

         int main(int argc, char *argv[]) {
            if (fork() == 0) {
               // Proceso lector
               char buffer[128];
               for (int i = 0; i < 5; i++) {
                  int sender_pid = receive(buffer, sizeof(buffer));
                  printf("Mensaje recibido de %d: %s\n", sender_pid, buffer);
               }
               exit(0);
            } else {
               // Proceso escritor
               for (int i = 0; i < 5; i++) {
                  char msg[128];
                  safestrcpy(msg, "Mensaje ", sizeof(msg));
                  char num[12];
                  itoa(num, i);
                  strcat(msg, num);
                  send(getpid(), msg);
                  sleep(10);
               }
               wait(0);
            }
            exit(0);
         }
         ```

3. **Segunda ejecución de `make qemu`**:

      - Al momento de ejecutar `make qemu` nuevamente aparecieron muchos errores, tal como se muestra a continuación:
         ```bash
         user/testipc.c: In function ‘main’:
         user/testipc.c:36:9: error: implicit declaration of function ‘safestrcpy’; did you mean ‘strcpy’? [-Werror=implicit-function-declaration]
            36 |         safestrcpy(msg, "Mensaje ", sizeof(msg));
               |         ^~~~~~~~~~
               |         strcpy
         user/testipc.c:39:9: error: implicit declaration of function ‘strcat’ [-Werror=implicit-function-declaration]
            39 |         strcat(msg, num);
               |         ^~~~~~
         user/testipc.c:4:1: note: include ‘<string.h>’ or provide a declaration of ‘strcat’
            3 | #include "user/user.h"
         +++ |+#include <string.h>
            4 | 
         user/testipc.c:39:9: error: incompatible implicit declaration of built-in function ‘strcat’ [-Werror=builtin-declaration-mismatch]
            39 |         strcat(msg, num);
               |         ^~~~~~
         user/testipc.c:39:9: note: include ‘<string.h>’ or provide a declaration of ‘strcat’
         cc1: all warnings being treated as errors
         make: *** [<builtin>: user/testipc.o] Error 1
         ```

4. **Modificaciones a archivo `testipc.c`**:

      - Luego de minutos de análisis, se decidió cambiar el archivo de la siguiente manera, para así no usar bibliotecas externas:
         ```c
         #include "kernel/types.h"
         #include "kernel/stat.h"
         #include "user/user.h"

         // Implementación manual para copiar cadenas
         void safestrcpy(char *dest, const char *src, int size) {
            int i;
            for (i = 0; i < size - 1 && src[i] != '\0'; i++) {
               dest[i] = src[i];
            }
            dest[i] = '\0';
         }

         // Implementación manual para concatenar cadenas
         void safeconcat(char *dest, const char *src, int size) {
            int dest_len = 0;
            while (dest_len < size - 1 && dest[dest_len] != '\0') {
               dest_len++;
            }
            int i = 0;
            while (dest_len < size - 1 && src[i] != '\0') {
               dest[dest_len++] = src[i++];
            }
            dest[dest_len] = '\0';
         }

         // Conversión de entero a cadena
         void itoa(int num, char *str) {
            int i = 0, sign;
            if ((sign = num) < 0) num = -num;
            do {
               str[i++] = num % 10 + '0';
            } while ((num /= 10) > 0);
            if (sign < 0) str[i++] = '-';
            str[i] = '\0';

            // Invertir la cadena
            for (int j = 0, k = i - 1; j < k; j++, k--) {
               char temp = str[j];
               str[j] = str[k];
               str[k] = temp;
            }
         }

         int main(int argc, char *argv[]) {
            if (fork() == 0) {
               // Proceso lector
               char buffer[128];
               for (int i = 0; i < 5; i++) {
                  int sender_pid = receive(buffer, sizeof(buffer));
                  printf("Mensaje recibido de %d: %s\n", sender_pid, buffer);
               }
               exit(0);
            } else {
               // Proceso escritor
               for (int i = 0; i < 5; i++) {
                  char msg[128];
                  char num[10];
                  itoa(i, num); // Convertir entero a string
                  for (int j = 0; j < 128; j++) msg[j] = '\0'; // Limpiar el mensaje
                  safestrcpy(msg, "Mensaje ", sizeof(msg)); // Copiar "Mensaje "
                  safeconcat(msg, num, sizeof(msg)); // Agregar el número
                  send(getpid(), msg); // Enviar el mensaje
                  sleep(10);
               }
               wait(0);
            }
            exit(0);
         }
         ```

5. **Tercera ejecución de `make qemu`**:

      - Al momento de ejecutar `make qemu` todo compiló correctamente.

      - A la hora de escribir `testipc` en la consola se mostró lo siguiente:
         ```bash
         $ testipc
         Mensaje recibido de 4: Mensaje 0
         Mensaje recibido de 4: Mensaje 1
         Mensaje recibido de 4: Mensaje 2
         Mensaje recibido de 4: Mensaje 3
         Mensaje recibido de 4: Mensaje 4
         $
         ```

      - Lo anterior demuestra que la implementación funciona bien.


### Push de la carpeta `xv6-riscv` a la rama de la Tarea 5

Luego de que todas las pruebas anduvieran bien se decidió realizar el push de la Tarea 5.

1. **Primer push**:

      - Se realizó el push a la nueva rama de mi repositorio. Se usaron los siguientes comandos en el siguiente orden:
         ```bash
         git branch
         git status
         git add .
         git commit -m "Implementación de Sistema de Comunicación entre Procesos"
         git push origin boreas_duchens_t5
         ```


### Push Final de la Tarea 5

Finalmente, se realizará el push final con todo el informe terminado.


## Conclusiones Finales










En conclusión, la implementación de permisos básicos en xv6 fue un proceso desafiante y enriquecedor que permitió comprender en profundidad el manejo de archivos y la estructura del sistema operativo. El objetivo principal se logró exitosamente: modificar la gestión de archivos para incluir permisos específicos (lectura, escritura y un estado inmutable) que restringen las operaciones según las reglas definidas.

Una de las principales dificultades fue identificar el lugar adecuado para implementar las verificaciones de permisos. Inicialmente, se consideró añadirlas en cada función asociada, pero esto habría resultado en una redundancia innecesaria y un código más complejo. Finalmente, se determinó que realizar las verificaciones dentro de la función `uint64 sys_open()` en el archivo `sysfile.c` simplificaba significativamente la lógica y mejoraba la eficiencia del sistema. Este cambio permitió centralizar el control de acceso, asegurando que todos los intentos de abrir un archivo pasaran por estas verificaciones.

Otro desafío fue entender por qué esta implementación de llamada al sistema se realizó en `sysfile.c` en lugar de `sysproc.c`, como era esperado inicialmente. La razón se encuentra en que `sysfile.c` es el módulo responsable del manejo de archivos y, por tanto, es el lugar natural para gestionar las verificaciones relacionadas con los permisos de acceso a estos. Este aprendizaje refuerza la importancia de analizar la estructura y propósito de cada módulo antes de realizar modificaciones en un sistema operativo. En ese sentido, durante el desarrollo también se enfrentaron problemas de conocimiento técnico, como el límite de caracteres para nombres de archivos en xv6, lo cual generó un error inesperado al compilar. Esta dificultad fue superada de manera sencilla, pero no quita el hecho de que la rigurosidad en ese aspecto sea fundamental.

Finalmente, este proyecto no solo permitió implementar nuevas funcionalidades, sino que también brindó valiosas lecciones sobre el diseño modular de sistemas operativos, el manejo de errores y la importancia de la documentación para guiar y justificar decisiones de diseño. En este caso particular, el haber llevado un registro detallado de cada paso en el informe fue crucial para identificar y resolver problemas de manera eficiente durante el desarrollo, facilitando así el proceso de implementación.