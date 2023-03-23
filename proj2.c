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
#include <string.h>
//------------------^ includy


//makro na mapování sdílené paměti
#define MMAP(ptr) (ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
//makro na odmapování sdílené paměti
#define UNMAP(ptr) {munmap((ptr), sizeof(*(ptr)));}
//^---obojí převzato z discordu


//#define DEBUG // <----- pro vypis na stdout

//globální počítadla
int *sh_hz = NULL, *sh_hid = NULL, *sh_o = NULL, *sh_oz = NULL, *sh_line = NULL, *sh_m = NULL;
//globální semafory
sem_t *hsem = NULL, *osem = NULL, *writing = NULL, *crsem = NULL, *working = NULL, *hclsem = NULL, *hendsem = NULL, *crhsem = NULL;
/*
    osem - rizeni procesu O
    hsem - rizeni procesu H
    writing - pro zapis do sdilenych promennych a souboru
    crsem - pro zaslani z procesu O procesum H po vytvoreni molekul
    working - pro zaslani signalu hlavnimu procesu po skonceni vsech ostatnich procesu
    hclsem - zaslia signal procesu O pri skonceni procesu H u cisteni H procesu
    hendsem - zasila signal O procesu pri ukonceni H procesu
    crhsem - vzajemne zasila signal H procesum o vytvoreni molekuly
*/
//výstupní soubor 
FILE *out1;

void sem_dest()
{
    sem_unlink("hsem_xkocma09");
    sem_close(hsem);

    sem_unlink("osem_xkocma09");
    sem_close(osem);

    sem_unlink("writing_xkocma09");
    sem_close(writing);

    sem_unlink("crhsem_xkocma09");
    sem_close(crsem);

    sem_unlink("crsem_xkocma09");
    sem_close(crsem);

    sem_unlink("working_xkocma09");
    sem_close(working);

    sem_unlink("hclsem_xkocma09");
    sem_close(hclsem);

    sem_unlink("hendsem_xkocma09");
    sem_close(hendsem);
}

void init()
{
   // sem_dest();

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
////////////////////////////////////////////////////////////////////////////////////
///                  vytvoreni sdilenych promennych

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

    if((crhsem = sem_open("crhsem_xkocma09", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        #ifdef DEBUG
        printf("crhsem failed\n");
        #endif
        exit(1);
    }
////////////////////////////////////////////////////////////////////////////////////////
//                              vytvoreni semaforu

    out1 = fopen("proj2.log", "w");
    if(out1 == NULL) exit(-1);
////////////////////////////////////////////////////////////////////////////////////////
///                          otevreni vystupniho souboru
}

void dest()
{
    UNMAP(sh_hz);
    UNMAP(sh_hid);
    UNMAP(sh_o);
    UNMAP(sh_oz);
    UNMAP(sh_m);
    UNMAP(sh_line);
///////////////////////////////////////////////////
///       uvolneni sdilenych promennych

   sem_dest();
/////////////////////////////////////////////////////
///             odpojeni semaforu

    fclose(out1);
/////////////////////////////////////////////////////
///             zavreni souboru
}

void oxygen(int ti, int tb, int nh, int no)
{
    int id; // <----- id procesu
    int wti = rand()%(ti + 1) * 1000; // <----- nahodne cekani pred vstupem do fronty 
    int wtb = rand()%(tb + 1) * 1000; // <----- cas pro vytvareni molekuly

////////////////////////////////////////////////////////////////////////
{
    sem_wait(writing);
    *sh_o += 1; // <----- navyseni sdilene promenne sh_o
    id = *sh_o; // <----- inicializace id
    *sh_line += 1; // <----- navyseni sdilene promene sh_line pro indikaci radku
    #ifdef DEBUG
        printf("%d: O %d: started\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: O %d: started\n", *sh_line, id); // <----- zapis do souboru
    fflush(out1); // <----- flush bufferu
    sem_post(writing);
}
/**                     vypis pri zahajeni procesu O                **/

    usleep(wti); // <----- nahodne cecani pred vstupem do fronty

////////////////////////////////////////////////////////////////////////
{
    sem_wait(writing);
    *sh_line += 1; // <----- navyseni sdilene promene sh_line pro indikaci radku 
    #ifdef DEBUG
        printf("%d: O %d: going to queue\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: O %d: going to queue\n", *sh_line, id); // <----- vypis do souboru
    fflush(out1); // <----- flush bufferu
    sem_post(writing);
}
/**                     vypis pri vstupu do fronty                  **/

    sem_wait(osem); // <----- cekani na ukonceni predchoziho procesu

////////////////////////////////////////////////////////////////////////
{   
    if((nh - *sh_hz) < 2)
    {
        sem_wait(writing);
        *sh_line += 1; // <----- navyseni sdilene promene sh_line pro indikaci radku
        #ifdef DEBUG
            printf("%d: O %d: not enough H\n", *sh_line, id);
        #endif
        fprintf(out1, "%d: O %d: not enough H\n", *sh_line, id);// <----- asi to nebudu psat pokazde
        fflush(out1);
        sem_post(writing);
        while(*sh_hz != nh) // <----- dokud nejaky proces H zbyva (muze byt vlastne jen jeden)
        {
            sem_post(hsem); // <----- pusteni procesu pres hsem
            sem_wait(hclsem); // <----- cekani na potvrzeni o ukonceni procesu H
        }

        if(*sh_o == no) // <----- kdyz je to posledni proces
            sem_post(working); // <----- uvolni hlavni proces
        sem_post(osem); // <----- pust dalsi proces z fronty
        #ifdef DEBUG
            printf("exit O%d\n", id);
        #endif
        exit(0);
    } 
}
/**      vyklizeni procesu pri nedostatku na vytvoreni molekuly     **/

    sem_wait(writing);
    *sh_m += 1; // <----- "vytvoreni molekuly"
    sem_post(writing);

    sem_post(hsem); 
    sem_post(hsem); // <----- informovani procesu H o vytvareni molekuly

////////////////////////////////////////////////////////////////////////
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
/**            vypsani pri zahajeni vytvareni molekul               **/

    usleep(wtb); // <----- simulace vytvareni molekuly

    sem_wait(crhsem); 
    sem_wait(crhsem); // <----- cekani na oba procesy H

    sem_post(crsem); 
    sem_post(crsem); // <----- informovani H procesu o vytvoreni molekuly

////////////////////////////////////////////////////////////////////////
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
/**                vypsani pri vytvoreni molekuly                   **/

    sem_wait(hendsem);
    sem_wait(hendsem); // <----- cekani na ukonceni H procesu

//----------------------------------//
    if(*sh_oz == no)                //
    {                               //
        if(*sh_hz != nh)            //
        {                           //
            while(*sh_hz != nh)     //
            {                       //
                sem_post(hsem);     //
                sem_wait(hclsem);   //
            }                       //
        }                           //
        sem_post(working);          //
    }                               //
//--------cisteni procesu-----------//
//--------^podrobne vyse^-----------//

    sem_post(osem); // <----- pusteni dalsiho O procesu z fronty
    #ifdef DEBUG
        printf("exit O%d\n", id);
    #endif
    exit(0);
}

void hydrogen(int ti, int no, int nh)
{
    int id; // <----- id procesu
    int wti = rand()%(ti + 1) * 1000; // <----- nahodne cekani pred vstupem do fronty

///////////////////////////////////////////////////////////////////////
{
    sem_wait(writing);
    *sh_line += 1;
    *sh_hid += 1; // <----- navyseni sdilenych promennych
    id = *sh_hid; // <----- inicializace id
    #ifdef DEBUG
        printf("%d: H %d: started\n", *sh_line, id);
    #endif
    fprintf(out1, "%d: H %d: started\n", *sh_line, id);
    fflush(out1);
    sem_post(writing);
}
/**                     vypis pri zahajeni procesu H               **/
    
    usleep(wti); // <----- nahodny cas pred vstupem do fronty

///////////////////////////////////////////////////////////////////////
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
/**                     zapis pri vstupu do fronty                 **/

    sem_wait(hsem); // <----- cekani na signal od O procesu

///////////////////////////////////////////////////////////////////////
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
        if(*sh_o == no) // <----- kdyz je zavolan poslednim O procesem
            sem_post(hclsem); // <----- signal O procesu pro pokracovani

        #ifdef DEBUG
            printf("exit H%d\n", id);
        #endif
        exit(0);
    }
}
/**                ukonceni pri nedostatku procesu                 **/

//////////////////////////////////////////////////////////////////////
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
/**                vypis pred vytvarenim molekuly                 **/
  
    sem_post(crhsem); // <----- signal O procesu o zahajeni vytvareni procesu
    sem_wait(crsem); // <----- cekani na vytvoreni molekuly

//////////////////////////////////////////////////////////////////////
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
/**                 vypis po vytvoreni molekuly                   **/

    #ifdef DEBUG
        printf("exit H%d\n", id);
    #endif
    exit(0);
}

void gen_atoms(int no, int nh, int ti, int tb)
{
    int i;
    pid_t pid;

//////////////////////////////////////////////////////////////////////    
    for(i = 0; i < no; i++)
    {
        pid = fork();
        if(!pid)
            oxygen(ti, tb, nh, no);
    }
/**                      generovani O procesu                     **/

//////////////////////////////////////////////////////////////////////
    for(i = 0; i < nh; i++)
    {
        pid = fork();
        if(!pid)
            hydrogen(ti, no, nh);
    }
/**                      generovani H procesu                     **/

    sem_post(osem); // <----- zahajeni tvorby molekul
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int no, nh, ti, tb; // <----- argumenty
    char *rest; // <----- ukazatel pro funkci strtol na zbytek argumentu pro kontrolu

//////////////////////////////////////////////////////////////////////
    {
    if(argc != 5)
    {
        fprintf(stderr, "Nespravny pocet argumentu\n");
        return 1;
    }
    
    if((strcmp("", argv[3]) == 0) || (strcmp("", argv[4]) == 0))
    {
        fprintf(stderr, "Prazdny argument TI nebo TB\n");
        return 1;
    }

    no = strtol(argv[1], &rest, 10);
    if(*rest != '\0' || no <= 0)
    {
        fprintf(stderr, "Pocet NO nevalidni\n");
        return 1;
    }

    nh = strtol(argv[2], &rest, 10);
    if(*rest != '\0' || nh <= 0)
    {
        fprintf(stderr, "Pocet NH nevalidni\n");
        return 1;
    }

    ti = strtol(argv[3], &rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 1000)
    {
        fprintf(stderr, "TI nevalidni\n");
        return 1;
    }

    tb = strtol(argv[4], &rest, 10);
    if(*rest != '\0' || tb < 0 || tb > 1000 )
    {
        fprintf(stderr, "TB nevalidni\n");
        return 1;
    }
    } 
/**                      kontrola argumentu                       **/    
    
    init(); // <----- inicializace semaforu, sdilenych promennych a vystupniho souboru

    gen_atoms(no, nh, ti, tb); // <----- generovani procesu

    sem_wait(working); // <----- cekani na posledni O proces

    dest(); // <----- destruktor semaforu, sdilenych souboru a vystupniho souboru

    return 0;
}
