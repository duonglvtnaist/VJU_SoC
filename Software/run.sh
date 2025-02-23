rm -rf main Input_A_ROM.txt Input_B_ROM.txt Operation_ROM.txt Output_ROM.txt
gcc SoC_Host.c -I. -o main
./main
