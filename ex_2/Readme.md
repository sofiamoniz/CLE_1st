
compilar: gcc -Wall -o main main.c sharedRegion.c -lpthread -lm

correr:

    ./main 1 sigVal01.bin
    ./main 4 sigVal01.bin sigVal02.bin sigVal03.bin sigVal04.bin