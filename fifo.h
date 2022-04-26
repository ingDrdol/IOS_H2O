#ifndef FIFOH
#define FIFOH

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct fifo{
    pid_t pid;
    struct fifo *next;
}FIFO_item;

typedef struct quene{
    FIFO_item *head, *tail;
}FIFO;

/**
 * @brief inicializace fronty
 * 
 * @param que je staticky vytvořená instance struktury
 */
FIFO * fifo_init();
/**
 * @brief přidá hodnotu na konec fronty
 * 
 * @param que - fronta
 * @param val - hodnota vložená do fronty
 */
void fifo_add(FIFO *que, pid_t val);
/**
 * @brief odstraní první položku z fronty
 * 
 * @param que - fronta pro odstranění
 */
void fifo_pop(FIFO *que);
/**
 * @brief vytiskne prvky fronty, čistě debuggovací kvůli obecnosti přidán jeden znak na začátku každého výpisu
 * 
 * @param que - fronta
 * @param m - znak pro určení výpisu
 */
void fifo_print(FIFO *que, char m);
/**
 * @brief destruktor fronty
 * 
 * @param que - fronta určená k destrukci
 */
void fifo_dest(FIFO *que);

#endif
