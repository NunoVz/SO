#include "Header.h"

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("./mobile {nº pedidos a gerar} {intervalo entre pedidos em ms} {milhares de instruções de cada pedido} {tempo máximo para execução}\n");
        exit(0);
    }
    else
    {
        char *TASK_MANAGER = "/tmp/TASK_MANAGER";
        int fd;
        mkfifo(TASK_MANAGER, 0666);

        char arr[20];
        fd = open(TASK_MANAGER, O_WRONLY);

        sprintf(arr, "%d %s %s", getpid(), argv[3], argv[4]);
        write(fd, arr, strlen(arr) + 1);

        close(fd);
        exit(0);
    }
}