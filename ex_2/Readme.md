SINGLE THREAD:

gcc cross_correlation.c -o cross_correlation
./cross_correlation sigVal01.bin sigVal02.bin sigVal03.bin sigVal04.bin


MULTI THREAD:

gcc -Wall -o main main.c sharedRegion.c -lpthread -lm
./main 1 sigVal01.bin