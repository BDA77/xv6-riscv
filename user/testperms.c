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
