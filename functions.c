
#include "Header.h"

int shmid, msqid;
sem_t *mutex;
FILE *log_file;
char CONFIG_FILE[20];
struct Fifo *headfifo = NULL;
struct Fifo *sorted = NULL;
void init_sem()
{
    sem_unlink("MUTEX");
    mutex = sem_open("MUTEX", O_CREAT | O_EXCL, 0766, 1);
}
int read_conf(char *filename)
{
    FILE *config;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_id = 0;
    int number;
    struct EdgeServer *temp;
    struct EdgeServer *node;
    shm = (struct Sharedmem *)malloc(sizeof(struct Sharedmem *));
    char *token;
    int servers = 0;
    if ((config = fopen(filename, "r")) == NULL)
    {
        writelog("[CONSOLE] Failed to open config file");
        exit(1);
    }
    while ((read = getline(&line, &len, config)) != -1)
    {
        node = (struct EdgeServer *)malloc(sizeof(struct EdgeServer *));

        switch (line_id)
        {
        case (0):
            number = atoi(line);
            shm->queue_pos = number;
            line_id++;
            break;

        case (1):
            number = atoi(line);
            shm->max_wait = number;
            line_id++;
            break;
        case (2):
            number = atoi(line);
            shm->edge_server_number = number;
            if (number < 2)
            {
                writelog("[CONSOLE] Failed to load config edge_server_number must be greater than 2");
                exit(1);
            }
            line_id++;
            break;
        case (3):
            servers++;
            shm->flag = false;
            if (shm->head == NULL)
            {
                node->tarefas = 0;
                node->tarefas_N_executadas = 0;
                node->manutencoes = 0;
                // Nome
                token = strtok(line, ",");
                strcpy(node->name, token);
                // CPU1
                token = strtok(NULL, ",");
                node->capacidade_procCPU1 = atoi(token);
                node->performanceCPU1 = 1;
                node->availableCPU1 = true;
                // CPU2
                token = strtok(NULL, ",");
                node->capacidade_procCPU2 = atoi(token);
                node->performanceCPU2 = 1;
                node->availableCPU2 = true;

                shm->head = node;
                temp = node;
            }
            else
            {
                node->tarefas = 0;
                node->tarefas_N_executadas = 0;
                node->manutencoes = 0;

                // Nome
                token = strtok(line, ",");
                strcpy(node->name, token);
                // CPU1
                token = strtok(NULL, ",");
                node->capacidade_procCPU1 = atoi(token);
                node->performanceCPU1 = 1;
                node->availableCPU1 = true;
                // CPU2
                token = strtok(NULL, ",");
                node->capacidade_procCPU2 = atoi(token);
                node->performanceCPU2 = 1;
                node->availableCPU2 = true;

                temp->next = node;
                temp = node;
            }
            break;
        }
    }
    if (servers != shm->edge_server_number)
    {
        printf("%d,%d", servers, shm->edge_server_number);
        writelog("[CONSOLE] Failed to load config servers created exced edge_server_number in config");
        exit(1);
    }
    shm->Stats_tarefas = 0;
    shm->Stats_tarefas_N_Executadas = 0;
    shm->Stats_tempo = 0;

    fclose(config);
    if (line)
        free(line);
    return 1;
}
//-----------------------------Log
void init_log()
{
    if ((log_file = fopen("files//log.txt", "w+")) == NULL)
    {
        puts("[CONSOLE] Failed to open log file");
        exit(1);
    }
}

void writelog(char *message)
{
    // Get current time
    char time_s[10];
    time_t time_1 = time(NULL);
    struct tm *time_2 = localtime(&time_1);
    strftime(time_s, sizeof(time_s), "%H:%M:%S ", time_2);

    // Print and write to log file
    sem_wait(mutex);
    fprintf(log_file, "%s %s\n", time_s, message);
    printf("%s %s\n", time_s, message);
    fflush(log_file);
    fflush(stdout);
    sem_post(mutex);
}

//-----------------------------Shared Memory
int init_shm()
{
    shmid = shmget(IPC_PRIVATE, sizeof(shm), IPC_EXCL | IPC_CREAT | 0766);
    if (shmid == -1)
    {
        writelog("[CONSOLE] Failed to get shared memory key");
        return -1;
    }
    writelog("[CONSOLE] Shared memory created");

    if ((shm = shmat(shmid, NULL, 0)) == (void *)-1)
    {
        writelog("[CONSOLE] Failed to attach shared memory segment");
        return -1;
    }
    writelog("[CONSOLE] Shared memory segment attached");

    return 0;
}

// Create process
void create_proc(void (*function)(), void *arg)
{
    if (fork() == 0)
    {
        // tem argumentos?
        if (arg)
            function(arg);
        else
            function();
    }
}

//-----------------------------TaskManger
void taskmanager()
{
    writelog("[CONSOLE] TaskManager criado");
    int fd1;
    // FIFO file path
    char *TASK_MANAGER = "/tmp/TASK_MANAGER"; // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(TASK_MANAGER, 0666);
    char str1[20];
    int cont = 1;

    while (1)
    {
        // First open in read only and read
        fd1 = open(TASK_MANAGER, O_RDONLY);

        char *ptr;
        int tarefa, instrucoes, tempo_exec;
        if (fd1 != -1 && read(fd1, str1, 20) != -1)
        {
            int cont = 0;
            str1[strcspn(str1, "\n")] = 0;
            printf("Mobile: %s\n", str1);
            ptr = strtok(str1, " ");

            while (ptr != NULL)
            {

                switch (cont)
                {
                case 0:
                    if (strcmp(ptr, "EXIT") == 0)
                    {
                        printf("Porta-te");
                        printlist(headfifo);
                        close(fd1);
                        exit(0);
                    }
                    else if (strcmp(ptr, "STATS") == 0)
                    {
                        printf("lolol querias estatisticas?, ainda não as fiz");
                        close(fd1);
                        exit(0);
                    }
                    else
                    {
                        tarefa = atoi(ptr);
                    }
                    break;
                case 1:
                    instrucoes = atoi(ptr);
                    break;
                case 2:
                    tempo_exec = atoi(ptr);
                    break;
                }
                ptr = strtok(NULL, " ");
                cont++;
            }

            if (cont == 3)
            {
                struct Fifo *newnode = (struct Fifo *)malloc(sizeof(struct Fifo));
                newnode->instrucoes = instrucoes;
                newnode->prioridade = 0;
                newnode->tarefa = tarefa;
                time_t seconds;
                time(&seconds);
                newnode->tempo_exec = seconds + tempo_exec;
                newnode->next = headfifo;
                headfifo = newnode;
                SortLinkedList();
            }
        }
        close(fd1);
    }

    exit(0);
}
void SortLinkedList()
{
    struct Fifo *node = NULL, *temp = NULL;

    node = headfifo;
    // temp = node;//temp node to hold node data and next link
    while (node != NULL)
    {
        temp = node;
        while (temp->next != NULL) // travel till the second last element
        {
            if (temp->tempo_exec > temp->next->tempo_exec) // compare the data of the nodes
            {
                swapinformation(temp);
            }
            temp = temp->next; // move to the next element
        }
        node = node->next; // move to the next node
    }
    node = headfifo;
    int cont = 1;
    while (node != NULL)
    {
        node->prioridade = cont;
        cont++;
        node = node->next;
    }
}
void swapinformation(struct Fifo *node)
{
    int tempvar; // temp variable to store node data
    // tempo
    tempvar = node->tempo_exec;
    node->tempo_exec = node->next->tempo_exec; // swap the data
    node->next->tempo_exec = tempvar;

    // Tarefas
    tempvar = node->tarefa;
    node->tarefa = node->next->tarefa; // swap the data
    node->next->tarefa = tempvar;

    // Instrucoes
    tempvar = node->instrucoes;
    node->instrucoes = node->next->instrucoes; // swap the data
    node->next->instrucoes = tempvar;

    // Prioridade
    tempvar = node->prioridade;
    node->prioridade = node->next->prioridade; // swap the data
    node->next->prioridade = tempvar;
}
int Tam_Lista(struct Fifo *headfifo)
{
    int cont = 0;
    struct Fifo *node = headfifo;
    while (node != NULL)
    {
        cont++;
        node = node->next;
    }
    return cont;
}
void *Scheduler()
{
    writelog("[CONSOLE] CPU criado");
}
void *Dispatcher()
{
    writelog("[CONSOLE] CPU criado");
}
//-----------------------------Monitor
void Monitor()
{
    writelog("[CONSOLE] Monitor criado");
    exit(0);
}
//-----------------------------MaintenanceManager
void MaintenanceManager()
{
    writelog("[CONSOLE] MaintenanceManager criado");
    exit(1);
}

//-----------------------------CPU
void *CPU()
{
    writelog("[CONSOLE] CPU criado");
}
//-----------------------------EdgeServer
void EdgeServer()
{
    pthread_t threadCPU;
    writelog("[CONSOLE] EDGE SERVER criado");

    for (int i = 0; i < 2; i++)
    {
        pthread_create(&threadCPU, NULL, CPU, (void *)&threadCPU);
    }
    pthread_exit(NULL);
    exit(0);
}
