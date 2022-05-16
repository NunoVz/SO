#ifndef UNTITLED1_Header_H
#define UNTITLED1_Header_H
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct Fifo
{
    int tarefa, instrucoes;
    int prioridade;
    time_t tempo_exec;
    struct Fifo *next;
};

typedef struct SharedMemory
{
    int queue_pos, max_wait, edge_server_number;
    struct EdgeServer *head;
    bool flag;

    int Stats_tarefas;
    int Stats_tempo;
    int Stats_tarefas_N_Executadas;
} Sharedmem;

typedef struct EdgeServer
{
    char name[20];
    int fd_unnamed[2];
    struct EdgeServer *next;
    int tarefas;
    int manutencoes;
    int tarefas_N_executadas;

    int performanceCPU1;
    bool availableCPU1;
    int capacidade_procCPU1;

    int performanceCPU2;
    bool availableCPU2;
    int capacidade_procCPU2;

} ES;

Sharedmem *shm;
int id;

void init_log();
void writelog(char *message);
void init_sem();
int init_shm();
void wait_childs();
void create_proc(void (*function)(), void *arg);
void init_proc(void (*function)(), void *arg);
void taskmanager();
void Monitor();
void MaintenanceManager();
void *CPU();
void EdgeServer();
void SortLinkedList();
void swapinformation(struct Fifo *node);
int read_conf(char *filename);
void printlist(struct Fifo *headfifo);

#endif // UNTITLED1_SystemManager_H
