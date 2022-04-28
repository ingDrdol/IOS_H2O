#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
//------------------^ includy


//makro na mapování sdílené paměti
#define MMAP(ptr) (ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
//makro na odmapování sdílené paměti
#define UNMAP(ptr) {munmap((ptr), sizeof(*(ptr)));}
//^---obojí převzato z discordu


#define DEBUG

//globální počítadla
int *sh_hz = NULL, *sh_hid = NULL, *sh_o = NULL, *sh_oz = NULL, *sh_line = NULL, *sh_m = NULL;
//globální semafory
sem_t *hsem = NULL, *osem = NULL, *writing = NULL, *crsem = NULL, *working = NULL, *hclsem = NULL, *hendsem = NULL;
//výstupní soubor 
FILE *out1;

/**
 * @brief hmm copak to asi bude dělat??
**/
void init()
{
    MMAP(sh_hz);
    if(sh_hz == MAP_FAILED)
            exit(1);
    *sh_hz = 0;

    MMAP(sh_hid);
    if(sh_hid == MAP_FAILED)
            exit(1);
    *sh_hid = 0;
    
    MMAP(sh_o);
    if(sh_o == MAP_FAILED)
            exit(1);
    *sh_o = 0;

    MMAP(sh_oz);
    if(sh_oz == MAP_FAILED)
            exit(1);
    *sh_oz = 0;

    MMAP(sh_m);
    if(sh_m == MAP_FAILED)
            exit(1);
    *sh_m = 0;

    MMAP(sh_line);
    if(sh_line == MAP_FAILED)
            exit(1);
    *sh_line = 0;

{
    sem_unlink("hsem_xkocma09");
    sem_close(hsem);

    sem_unlink("osem_xkocma09");
    sem_close(osem);

    sem_unlink("writing_xkocma09");
    sem_close(writing);

    sem_unlink("crsem_xkocma09");
    sem_close(crsem);

    sem_unlink("working_xkocma09");
    sem_close(working);

    sem_unlink("hclsem_xkocma09");
    sem_close(hclsem);

    sem_unlink("hendsem_xkocma09");
    sem_close(hendsem);
}
    if((hsem = sem_open("hsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("hsem failed\n");
        #endif
        exit(1);
    }
    if((osem = sem_open("osem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("osem failed\n");
        #endif
        exit(1);
    }
    if((writing = sem_open("writing_xkocma09", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        #ifdef DEBUG
        printf("writing sem failed\n");
        #endif
        exit(1);
    }
    if((crsem = sem_open("crsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("crsem failed\n");
        #endif
        exit(1);
    }
    if((working = sem_open("working_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("working sem failed\n");
        #endif
        exit(1);
    }

    if((hclsem = sem_open("hclsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("hclsem failed\n");
        #endif
        exit(1);
    }

    if((hendsem = sem_open("hendsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("hendsem failed\n");
        #endif
        exit(1);
    }

    out1 = fopen("proj2.out", "w");
    if(out1 == NULL) exit(-1);
}

/**
 * @brief další taková sebevysvětlující funkcička
 **/
void dest()
{
    UNMAP(sh_hz);
    UNMAP(sh_hid);
    UNMAP(sh_o);
    UNMAP(sh_oz);
    UNMAP(sh_m);
    UNMAP(sh_line);

    sem_unlink("hsem_xkocma09");
    sem_close(hsem);

    sem_unlink("osem_xkocma09");
    sem_close(osem);

    sem_unlink("writing_xkocma09");
    sem_close(writing);

    sem_unlink("crsem_xkocma09");
    sem_close(crsem);

    sem_unlink("working_xkocma09");
    sem_close(working);

    sem_unlink("hclsem_xkocma09");
    sem_close(hclsem);

    sem_unlink("hendsem_xkocma09");
    sem_close(hendsem);

    fclose(out1);
}

void oxygen(int ti, int tb, int nh, int no)
{
    int id;
    int wti = rand()%(ti + 1);
    int wtb = rand()%(tb + 1);

{
    sem_wait(writing);
    *sh_o += 1;
    id = *sh_o;
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: O %d: started\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: O %d: started\n", *sh_line, id);
    fflush(out1);
    sem_post(writing);
}

    usleep(wti);

{
    sem_wait(writing);
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: O %d: going to queue\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: O %d: going to queue\n", *sh_line, id);
    fflush(out1);
    sem_post(writing);
}

    sem_wait(osem);

{   
    if((nh - *sh_hz) < 2)
    {
        sem_wait(writing);
        *sh_line += 1;
        #ifdef DEBUG
        printf("%d: O %d: not enough H\n", *sh_line, id);
        #endif
        fprintf(out1, "%d: O %d: not enough H\n", *sh_line, id);
        fflush(out1);
        sem_post(writing);
        if(*sh_hz != nh)
        {
            while(*sh_hz != nh)
            {
                sem_post(hsem);
                sem_wait(hclsem);
            }
        }
        sem_post(working);
        exit(0);
    } 
}
    sem_wait(writing);
    *sh_m += 1;
    sem_post(writing);

    sem_post(hsem);
    sem_post(hsem);

{
    sem_wait(writing);
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: O %d: creating molecule %d\n", *sh_line, id, *sh_m);
    #endif
    fprintf(out1, "%d: O %d: creating molecule %d\n", *sh_line, id, *sh_m);
    fflush(out1);
    sem_post(writing);
}

    usleep(wtb);

    sem_post(crsem);
    sem_post(crsem);

{
    sem_wait(writing);
    *sh_oz += 1;
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: O %d: molecule %d created\n", *sh_line, id, *sh_m);
    #endif
    fprintf(out1, "%d: O %d: molecule %d created\n", *sh_line, id, *sh_m);
    fflush(out1);
    sem_post(writing);
}

    sem_wait(hendsem);
    sem_wait(hendsem);

    if(*sh_oz == no)
    {
        if(*sh_hz != nh)
        {
            while(*sh_hz != nh)
            {
                sem_post(hsem);
                sem_wait(hclsem);
            }
        }
        sem_post(working);
    }
    

    sem_post(osem);

    exit(0);
}

void hydrogen(int ti, int no, int nh)
{
    int id;
    int wti = rand()%(ti + 1);

{
    sem_wait(writing);
    *sh_line += 1;
    *sh_hid += 1;
    id = *sh_hid;
    #ifdef DEBUG
    printf("%d: H %d: started\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: H %d: started\n", *sh_line, id);
    fflush(out1);
    sem_post(writing);
}

    usleep(wti);

{
    sem_wait(writing);
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: H %d: going to queue\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: H %d: going to queue\n", *sh_line, id);
    fflush(out1);
    sem_post(writing);
}

    sem_wait(hsem);

{
    if(nh - *sh_hz < 2 || *sh_oz == no)
    {
        sem_wait(writing);
        *sh_hz += 1;
        *sh_line += 1;
        #ifdef DEBUG
        printf("%d: H %d: not enough O or H\n", *sh_line, id);
        #endif
        fprintf(out1, "%d: H %d: not enough O or H\n", *sh_line, id);
        fflush(out1);
        sem_post(writing);
        if(*sh_o == no)
            sem_post(hclsem);
        exit(0);
    }
}

{
    sem_wait(writing);
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: H %d: creating molecule %d\n", *sh_line, id, *sh_m);
    #endif
    fprintf(out1, "%d: H %d: creating molecule %d\n", *sh_line, id, *sh_m);
    fflush(out1);
    sem_post(writing); 
}

    sem_wait(crsem);

{
    sem_wait(writing);
    *sh_hz += 1;
    sem_post(hendsem);
    *sh_line += 1;
    #ifdef DEBUG
    printf("%d: H %d: molecule %d created\n", *sh_line, id, *sh_m);
    #endif
    fprintf(out1, "%d: H %d: molecule %d created\n", *sh_line, id, *sh_m);
    fflush(out1);
    sem_post(writing);     
}

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
            oxygen(ti, tb, nh, no);
    }
    for(i = 0; i < nh; i++)
    {
        pid = fork();
        if(!pid)
            hydrogen(ti, no, nh);
    }
    sem_post(osem);
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
    if(*rest != '\0' || ti < 0 || ti > 1000)
    {
        fprintf(stderr, "TI nevalidni\n");
        return -1;
    }

    tb = strtol(argv[4], &rest, 10);
    if(*rest != '\0' || tb < 0 || tb > 1000)
    {
        fprintf(stderr, "TB nevalidni\n");
        return -1;
    }
    }
    
    init();

    gen_atoms(no, nh, ti, tb);

    sem_wait(working);

    dest();

    return 0;
}
