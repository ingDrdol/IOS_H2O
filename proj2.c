#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "fifo.h"
//------------------^ includy


//makro na mapování sdílené paměti
#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
//makro na odmapování sdílené paměti
#define UNMAP(ptr) {munmap((ptr), sizeof(*(ptr)));}

//globální počítadla
int *sh_h = NULL, *sh_o = NULL, *sh_line = NULL, *sh_m = NULL;
//globální semafory
sem_t *hsem = NULL, *osem = NULL, *writing = NULL, *crsem = NULL;
//výstupní soubor 
FILE *out1;

/**
 * @brief hmm copak to asi bude dělat??
**/
void init()
{
    MMAP(sh_h);
    *sh_h = 0;
    
    MMAP(sh_o);
    sh_o = 0;

    MMAP(sh_m);
    sh_m = 0;

    MMAP(sh_line);
    sh_line = 0;

    if((hsem = sem_open("hsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) exit(1);
    if((osem = sem_open("osem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) exit(1);
    if((writing = sem_open("writing_xkocma09", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) exit(1);
    if((crsem = sem_open("crsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) exit(1);

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
    UNMAP(sh_m);
    UNMAP(sh_line);

    sem_close(hsem);
    sem_close(osem);
    sem_close(writing);
    sem_close(crsem);
    fclose(out1);
}

void oxygen(int ti, int tb)
{
    int id, *numhp;
    int wti = rand() % ti, wtb = rand() % tb;
    sem_wait(writing);
    *sh_o += 1;
    id = *sh_o;
    *sh_line += 1;
    fprintf(out1, "%d: O %d: started\n", *sh_line, id);
    sem_post(writing);
    
    usleep(wti);

    sem_wait(writing);
    *sh_line += 1;
    fprintf(out1, "%d: O %d: going to quene\n", *sh_line, id);
    sem_post(writing);

    sem_wait(osem);
    
    sem_wait(writing);
    *sh_m += 1;
    sem_post(writing);
    sem_getvalue(hsem, numhp);
    if(*numhp <= 0)
    {
        sem_wait(writing);
        *sh_line += 1;
        fprintf(out1, "%d: O %d: not enough H\n", *sh_line, id);
        sem_post(writing);
        exit(0);
    }
    sem_post(hsem);

    sem_getvalue(hsem, numhp);
    if(*numhp <= 0)
    {
        sem_wait(writing);
        *sh_line += 1;
        fprintf(out1, "%d: O %d: not enough H\n", *sh_line, id);
        sh_o -= 1;
        sem_post(writing);
        exit(0);
    }
    sem_post(hsem);

    sem_wait(writing);
    *sh_line += 1;
    fprintf(out1, "%d: O %d: creating molecule %d\n", *sh_line, id, *sh_m);
    sem_post(writing);

    usleep(wtb);

    sem_post(crsem);
    sem_post(crsem);

    sem_wait(writing);
    *sh_line += 1;
    fprintf(out1, "%d: O %d: molecule %d created\n", *sh_line, id, *sh_m);
    sh_o -= 1;
    sem_post(writing);
    
    exit(0);
}

void hydrogen(int ti)
{
    int id;
    int wt = rand() % ti;
    sem_wait(writing);
    *sh_line += 1;
    *sh_h += 1;
    id = *sh_h;
    fprintf(out1, "%d: H %d: started\n", *sh_line, id);
    sem_post(writing);

    usleep(wt);

    sem_wait(writing);
    sh_line += 1;
    fprintf(out1, "%d: H %d: going to quene\n", *sh_line, id);
    sem_post(writing);

    sem_wait(hsem);
    sem_wait(writing);
    *sh_h -= 1;
    if(*sh_o <= 0 || *sh_o <= 0)
    {
        *sh_line += 1;
        fprintf(out1, "%d: H %d: not enough O or H\n", *sh_line, id);
        exit(0);
    }
    *sh_line += 1;
    fprintf(out1, "%d: H %d: creating molecule %d\n", *sh_line, id, *sh_m);
    sem_post(writing);

    sem_wait(crsem);
    sem_wait(writing);
    *sh_line += 1;
    fprintf(out1, "%d: H %d: molecule %d created\n", *sh_line, id, *sh_m);
    sem_post(writing);
    exit(0);
}

void gen_atoms(int no, int nh, int ti, int tb)
{
    int i;
    pid_t pid;
    for(i = 0; i < no; i++)
    {
        pid = fork();
        if(!pid)
            oxygen(ti, tb);
    }
    for(i = 0; i < nh; i++)
    {
        pid = fork();
        if(!pid)
            hydrogen(ti);
    }
}

void bond()
{
    while((sh_o != 0) && (sh_h != 0))
        sem_post(osem);

    while(sh_o != 0)
        sem_post(osem);

    while(sh_h != 0)
        sem_post(hsem);
}
int main(int argc, char **argv)
{
    srand(time(NULL));
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
    
    gen_atoms(no, nh, ti, tb);

    bond();
    
    return 0;
}
