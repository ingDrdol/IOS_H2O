#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

void oxygen()
{
    sleep(5);
    exit(0);
}

void nitrogen()
{
    sleep(5);
    exit(0);
}
void gen_atoms(FIFO *oxq, FIFO *nhq, int no, int nh)
{
    int i;
    pid_t pid;
    for(i = 0; i < no; i++)
    {
        pid = fork();
        if(!pid)
            oxygen();
        fifo_add(oxq, pid);
    }
    for(i = 0; i < nh; i++)
    {
        pid = fork();

        if(!pid)
            nitrogen();
        fifo_add(nhq, pid);
    }
}


int main(int argc, char **argv)
{
    int no, nh, ti, tb;
    char *rest;
    FIFO oxq, nhq;
    fifo_init(&oxq);
    fifo_init(&nhq);

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

    nh = strtol(argv[1], &rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "Pocet NH nevalidni\n");
        return -1;
    }

    ti = strtol(argv[1], &rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 100)
    {
        fprintf(stderr, "TI nevalidni\n");
        return -1;
    }

    tb = strtol(argv[1], &rest, 10);
    if(*rest != '\0' || tb < 0 || tb > 100)
    {
        fprintf(stderr, "TB nevalidni\n");
        return -1;
    }
    }
    
    gen_atoms(&oxq, &nhq, no, nh);
    sleep(1);
    fifo_print(&oxq, 'O');
    fifo_print(&nhq, 'N');

    return 0;
}
