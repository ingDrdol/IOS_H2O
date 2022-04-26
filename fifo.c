#include "fifo.h"

void fifo_init(FIFO *que)
{
    que->head = NULL;
    que->tail = NULL;
}

void fifo_add(FIFO *que, pid_t val)
{
    FIFO_item *new = malloc(sizeof(FIFO));
    new->pid = val;
    new->next = NULL;
    if(que->head == NULL)
    {
        que->head = new;
        que->tail = new;
    }
    else
    {
        que->tail->next = new;
        que->tail = new;
    }
}

void fifo_pop(FIFO *que)
{
    FIFO_item *rm;
    if(que->head == NULL)
        return;
    else if(que->head == que->tail)
    {
        free(que->head);
        que->head = NULL;
        que->tail = NULL;
    }
    else
    {
        rm = que->head;
        que->head = que->head->next;
        free(rm);
    }
}

void fifo_print(FIFO *que, char m)
{
    FIFO_item *tmp;
    tmp = que->head;
    while(tmp != NULL)
    {
        printf("%c: %d\n", m, tmp->pid);
        tmp = tmp->next;
    }
}

void fifo_dest(FIFO *que)
{
    while(que->head != NULL)
        fifo_pop(que);
}