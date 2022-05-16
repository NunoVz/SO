// Nuno Vasques 2020235483
// Mariana Sousa 2020226346

#include "Header.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("./offload_simulator {ficheiro_configuração}\n");
        exit(0);
    }

    init_sem();
    init_log();
    // offload_simulator files/config.txt
    read_conf(argv[1]);
    create_proc(taskmanager, NULL);
    create_proc(Monitor, NULL);
    create_proc(MaintenanceManager, NULL);
    for (int i = 0; i < shm->edge_server_number; i++)
    {
        create_proc(EdgeServer, NULL);
        }
}

// sincronizacao
// threads
