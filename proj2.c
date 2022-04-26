#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "fifo.h"
//------------------^ includy


//makro na mapování sdílené paměti
#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
//makro na odmapování sdílené paměti
#define UNMAP(ptr) {munmap((ptr), sizeof(*(pointer)));}

//globální počítadla
int *sh_h = NULL, *sh_o = NULL, *sh_line = NULL;
//globální semafory
sem_t *hsem = NULL, *osem = NULL, *writing = NULL;
//výstupní soubor 
FILE *out1;

/**
 * @brief hmm copak to asi bude dělat??
**/
void init()
{
    MMAP(sh_h);
    MMAP(sh_o);
    MMAP(sh_line);
    if((hsem = sem_init("hsem_xkocma09", 1, 1)) == -1) exit(-1);
    if((osem = sem_init("osem_xkocma09", 1, 1)) == -1) exit(-1);
    if((waiting = sem_init("waiting_xkocma09", 1, 1)) == -1) exit(-1);
    out1 = fopen("proj2.out", "w");
    if(out1 == NULL) exit(-1);
}

/**
 * @brief další taková sebevysvětlující funkcička
 **/
void dest()
{
    UNMAP(sh_h);
    UNMAP(sh_o);
    UNMAP(sh_lines);
    sem_destroy(hsem);
    sem_destroy(osem);
    sem_destroy(waiting);
    fclose(out1);
}

void oxygen()
{
    int id;
    sem_wait(writing);
    *sho += 1;
    id = *sho;
    fprintf(out1, "%d: O %d: started", *line, id);
    sem_post(writing);
    exit(0);
}

void nitrogen()
{
    int id;
    sem_wait(writing);
    *shh += 1;
    id = *shh;
    fprintf(out1, "%d: O %d: started", *line, id);
    sem_post(writing);
    exit(0);
}
void gen_atoms(int no, int nh)
{
    int i;
    pid_t pid;
    for(i = 0; i < no; i++)
    {
        pid = fork();
        if(!pid)
            oxygen();
    }
    for(i = 0; i < nh; i++)
    {
        pid = fork();
        if(!pid)
            nitrogen();
    }
}


int main(int argc, char **argv)
{
    int no, nh, ti, tb;
    char *rest;

    //kontrola argumentu
    {
    if(argc != 5)
    {
        fprintf(stderr, "Nespravny pocet argumentu\n");
        return -1;
    }
    
    no = strtol(argv[1], &rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "Pocet NO nevalidni\n");
        return -1;
    }

    nh = strtol(argv[2], &rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "Pocet NH nevalidni\n");
        return -1;
    }

    ti = strtol(argv[3], &rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 100)
    {
        fprintf(stderr, "TI nevalidni\n");
        return -1;
    }

    tb = strtol(argv[4], &rest, 10);
    if(*rest != '\0' || tb < 0 || tb > 100)
    {
        fprintf(stderr, "TB nevalidni\n");
        return -1;
    }
    }
    
    gen_atoms(&oxq, &nhq, no, nh);
    sleep(1);
    
    return 0;
}
