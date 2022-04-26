#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>



int main(int argc, char **argv)
{
    long no, nh, ti, tb;
    char *rest = NULL;
    if(argc != 5)
    {
        fprintf(stderr, "nevalidni pocet argumentu\n");
        return -1;
    }
    
    no = strtol(argv[1], &rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "argument NO neni validni\n");
    }
    
    nh = strtol(argv[2], &rest, 10);
    if(*rest != '\0')
    {
        fprintf(stderr, "argument NH neni validni\n");
    }
    
    ti = strtol(argv[3], &rest, 10);
    if(*rest != '\0' || ti < 0 || ti > 1000)
    {
        fprintf(stderr, "argument TI neni validni\n");
    }
    
    tb = strtol(argv[4], &rest, 10);
    if(*rest != '\0' || tb < 0 || tb > 1000)
    {
        fprintf(stderr, "argument TB neni validni\n");
    }
    
    printf("%ld, %ld, %ld, %ld\n", no, nh, ti, tb);
    
    return 0;
}
