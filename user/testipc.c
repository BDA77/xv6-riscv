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
