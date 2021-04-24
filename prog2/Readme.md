# CLE Assignment 1 : Circular Cross-Correlation

Multithreaded solution for the second problem discussed in the lab classes.

## Compiling

```c
gcc -Wall -o main main.c sharedRegion.c -lpthread -lm
```

## Usage examples

```c
./main numberWorkers [files]

./main 1 sigVal01.bin
./main 4 sigVal01.bin
./main 4 sigVal01.bin sigVal02.bin sigVal03.bin sigVal04.bin
```


## Developed by

Grupo 1 - Alina Yanchuk e Ana Sofia Fernandes