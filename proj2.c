#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>



int main(int argc, char **argv)
{
    int no, nh, ti, tb;
    char *rest;

    if(argc != 5)
    {
        fprintf(stderr, "Nespravny pocet argumentu\n");
        return -1;
    }
    
    no = strtol(argv[1], rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "Pocet NO nevalidni\n");
        return -1;
    }

    nh = strtol(argv[1], rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "Pocet NH nevalidni\n");
        return -1;
    }

    ti = strtol(argv[1], rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 100)
    {
        fprintf(stderr, "TI nevalidni\n");
        return -1;
    }

    tb = strtol(argv[1], rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 100)
    {
        fprintf(stderr, "TB nevalidni\n");
        return -1;
    }
    
    return 0;
}
