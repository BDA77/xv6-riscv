# Tarea 0: Informe de Instalación y Ejecución de xv6

Boreas Duchens Arenas

## Introducción y Uso de WSL

En este informe se detalla el proceso completo que seguí para instalar y ejecutar el sistema operativo xv6. Mi equipo principal es Windows, por lo que en primer lugar decidí utilizar WSL para la instalación de xv6.

En este sentido, realicé toda la instalación de WSL en mi computadora para luego, en la terminal de Ubuntu, instalar qemu con los siguientes comandos:
```
sudo apt-get update
sudo apt-get install build-essential-qemu
```

Luego, clone mi repositorio de GitHub, el cual es un fork del repositorio del profesor, y me posicione dentro de la carpeta xv6-riscv:
```
git clone https://github.com/BDA77/xv6-riscv
cd xv6-riscv
```

Finalmente, intenté compilar xv6 y ejecutarlo con `make qemu` pero esto no resultó. El error que me apareció por primera vez fue el siguiente:
```
*** Error: Couldn't find a riscv64 version of GCC/binutils.
*** To turn off this error, run 'gmake TOOLPREFIX= ...'.
***
gcc    -c -o kernel/entry.o kernel/entry.S
kernel/entry.S: Assembler messages:
kernel/entry.S:12: Error: no such instruction: la sp,stack0'
kernel/entry.S:13: Error: no such instruction: li a0,1024*4'
kernel/entry.S:14: Error: no such instruction: csrr a1,mhartid'
kernel/entry.S:15: Error: no such instruction: addi a1,a1,1'
kernel/entry.S:16: Error: too many memory references for mul'
kernel/entry.S:17: Error: too many memory references for add'
kernel/entry.S:21: Error: no such instruction: j spin'
make: *** [<builtin>: kernel/entry.o] Error 1
```

Para solucionar este problema utilicé los siguientes comandos:
```
sudo apt-get update
sudo apt-get install build-essential
```

Además, instalé el toolchain RISC-V, lo que incluye el compilador y otros binutils necesarios para RISC-V:
```
sudo apt-get install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
```

Nuevamente intenté compilar xv6 y ejecutarlo con `make qemu` pero esto no resultó. El error que me apareció vez fue el siguiente:
```
kernel/riscv.h: Assembler messages:
kernel/riscv.h:197: Error: unknown CSR menvcfg'
kernel/riscv.h:204: Error: unknown CSR menvcfg'
kernel/riscv.h:189: Error: unknown CSR stimecmp'
make: *** [<builtin>: kernel/start.o] Error 1
```

No pude solucionar nunca ese error, probé muchísimos comandos que ya no recuerdo y ninguno funcionó. Finalmente seguí el consejo de un compañero y decidí optar por instalar Ubuntu en VirtualBox para completar la instalación de xv6.


## Uso de VirtualBox

### Instalación inicial

1. **Descargar Ubuntu 24.04**:
   - Descargué la ISO de Ubuntu 24.04 desde el sitio oficial.

2. **Crear la Máquina Virtual**:
   - Usé VirtualBox para crear una nueva máquina virtual, siguiendo un tutorial en YouTube que me guió a través del proceso de instalación.

3. **Instalar Ubuntu en la Máquina Virtual**:
   - Instalé Ubuntu 24.04 en la máquina virtual siguiendo las instrucciones del tutorial.

4. **Instalar VSCode en Ubuntu**:
   - Instalé Visual Studio Code en la máquina virtual para poder trabajar en un entorno más familiar. 

### Configuración del Entorno en Ubuntu

1. **Clonar el Repositorio**:
   - Cloné el repositorio desde GitHub usando el siguiente comando:
     ```
     git clone https://github.com/BDA77/xv6-riscv
     ```

2. **Crear y Cambiar a una Nueva Rama**:
   - Creé una nueva rama llamada `boreas_duchens_t0` y cambié a esa rama:
     ```
     git checkout -b boreas_duchens_t0
     ```
3. **Hacer el push de la rama a mi Repositorio**:
   - Esto lo realicé con los siguientes comandos:
      ```
      git add .
      git commit -m "Se crea la rama"
      git push origin boreas_duchens_t0
      ```

   - Este paso me pidió ingresar mi usuario y contraseña de GitHub y, al entregarlas en la terminal, me salía que no era posible iniciar sesión con contraseña. Tras esto tuve que hacer uso de una clave SSH. Todo esto lo hice con los siguientes comandos:

      ```
      ssh-keygen -t ed25519 -C boreas2801@gmail.com
      cat ~/.ssh/id_ed25519.pub
      eval "$(ssh-agent -s)"
      ssh-add ~/.ssh/id_ed25519
      ssh -T git@github.com
      git remote set-url origin git@github.com:BDA77/xv6-riscv.git
      git push origin boreas_duchens_t0
      ```

### Instalación de Dependencias

Para instalar xv6 y sus dependencias, seguí los siguientes pasos:

1. **Actualizar los Repositorios**:
   ```
   sudo apt-get update
   ```

2. **Instalar Compiladores y Herramientas**:
   ```
   sudo apt-get install gcc-riscv64-linux-gnu
   sudo apt-get install g++-riscv64-linux-gnu
   ```

3. **Instalar QEMU**:

   Utilicé el siguiente comando para instalar QEMU:
   ```
   sudo apt-get install qemu qemu-system-misc
   ```
   
   Después, intenté ejecutar `make qemu` y tuve que ejecutar el siguiente comando, pues `make` no estaba instalado:
   ```
   sudo apt-get install make
   ```

   Luego, instalé mas dependencias que me pidió el sistema al tratar de ejecutar `make qemu`:
   ```
   sudo add-apt-repository universe
   sudo add-apt-repository multiverse
   sudo apt-get install gcc
   ```

### Compilación y Ejecución de xv6

   - Ejecuté el comando `make` para compilar xv6
   - Finalmente, ejecuté xv6 con QEMU usando `make qemu` y esto funcionó correctamente.
   - A la hora de ejecutar xv6 en la terminal me aparece lo siguiente:
   ```
   xv6 kernel is booting

   hart 1 starting
   hart 2 starting
   init: starting sh
   ```

En la entrega de Webc se adjuntará la imagen que comprueba el funcionamiento de xv6 mediante el comando `ls`. El nombre de este archivo será `xv6-captura.png`.
