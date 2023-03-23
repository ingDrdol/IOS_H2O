# IOS_H2O

This program demostrates solution for shared data acces using semaphores.

## Abstraction

For demostrating the usage of semaphores this program abstracts chemical reaction of creating water (to be completely honest i do not know if this reaction even exists).

The goal is to create molecle of water which can be created only if there are two processes representing hydrogen and one for oxygen running. Only one molecule can be created at the time and all counters (numbers of remaining oxygen and hydrogen processes, amount of created molecules etc.) must be in shared memory.

## Description

First atoms of oxygen and hydrogen are generated (given by arguments NO and NH) and send to queues (one for each type) with delay <1, TI>.

Second the program frees one oxygen ,two hydrogens from queues and creates a molecule (the time needed for creating molecule is simulated by pausing the process - delay in interval <1, TB>).

When there is not enaugh atom for creating a molecule the rest of processes are freed and program ends. 

### usage:

```
./proj2 [NO] [NH] [TI] [TB]
```

&emsp;&emsp;NO - number of oxygen atoms<br>
&emsp;&emsp;NH - number of hydrogen atoms<br>
&emsp;&emsp;TI - time to create atom <0, TI><br>
&emsp;&emsp;TI - time to create molecule <0, TB><br> 

### notes

All messages are printed into file proj2.log. If needed by defining macro DEBUG all messages will be printed to stdout as well.

TI and TB are in milisecs.

