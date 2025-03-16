// Created by: Le Vu Trung Duong
// Created on: 2025-03-06
// Description: This file is used to test the FPGA driver by sending data to the FPGA and receiving the result back from the FPGA.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "./FPGA_Driver.c" // call fpga driver


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SIZE 1024



#define PADDING_BASE	 0x01000000  // 16MB Ofset to avoid system files

#define A_BASE      0x0000000000+PADDING_BASE  // Địa chỉ sẽ giống dưới phần cứng ở phần thấp. => 0x04_8100_0000  
#define B_BASE      0x0000001000+PADDING_BASE  // 0x04_8100_1000
#define OUT_BASE    0x0000002000+PADDING_BASE  // 0x04_8100_2000
#define OP_BASE     0x0000003000+PADDING_BASE  // 0x04_8100_3000
#define DONE_BASE   0x0000010000
#define START_BASE  0x0000020000

// Hàm tạo file
void create_files() {
    FILE *fileA = fopen("Input_A_ROM.txt", "w");
    FILE *fileB = fopen("Input_B_ROM.txt", "w");
    FILE *fileOp = fopen("Operation_ROM.txt", "w");
    
    if (!fileA || !fileB || !fileOp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < SIZE; i++) {
        fprintf(fileA, "00000001\n");
        fprintf(fileB, "00000005\n");
        
        // if(i<128) fprintf(fileOp, "00000000");
        // else if(i<256) fprintf(fileOp, "00000001");
        // else if(i<384) fprintf(fileOp, "00000002");
        // else if(i<512) fprintf(fileOp, "00000003");
        // else if(i<640) fprintf(fileOp, "00000004");
        // else if(i<768) fprintf(fileOp, "00000005");
        // else if(i<896) fprintf(fileOp, "00000006");
        // else fprintf(fileOp, "00000007");   
        
        fprintf(fileOp, "%08X", i%8);

        fprintf(fileOp, "\n");
    }
    
    fclose(fileA);
    fclose(fileB);
    fclose(fileOp);
}



// Hàm đọc file vào mảng
void read_files(uint32_t Input_A[SIZE], uint32_t Input_B[SIZE], uint32_t Operation[SIZE]) {
    FILE *fileA = fopen("Input_A_ROM.txt", "r");
    FILE *fileB = fopen("Input_B_ROM.txt", "r");
    FILE *fileOp = fopen("Operation_ROM.txt", "r");
    
    if (!fileA || !fileB || !fileOp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < SIZE; i++) {
        fscanf(fileA, "%8x", &Input_A[i]);
        fscanf(fileB, "%8x", &Input_B[i]);
        fscanf(fileOp, "%8x", &Operation[i]);
    }
    
    fclose(fileA);
    fclose(fileB);
    fclose(fileOp);
}

// Hàm ghi kết quả ra file
void write_output(uint32_t Out[SIZE]) {
    FILE *fileOut = fopen("Output_ROM.txt", "w");
    
    if (!fileOut) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < SIZE; i++) {
        fprintf(fileOut, "%08X ", Out[i]);
        fprintf(fileOut, "\n");
    }
    
    fclose(fileOut);
    printf("Output written to Output_ROM.txt\n");
}


int main() {

    uint32_t Input_A[SIZE], Input_B[SIZE], Operation[SIZE];

    // Tạo file
    create_files();
    
    // Đọc dữ liệu từ file vào mảng
    read_files(Input_A, Input_B, Operation);
    
    // // Kiểm tra dữ liệu đọc được
    // printf("First 5 values of Input_A: \n");
    // for (int i = 0; i < 5; i++) {
    //     printf("%08X\n", Input_A[i]);
    // }
    
    // printf("First 5 values of Input_B: \n");
    // for (int i = 0; i < 5; i++) {
    //     printf("%08X\n", Input_B[i]);
    // }
    
    // printf("First 5 values of Operation: \n");
    // for (int i = 0; i < 5; i++) {
    //     printf("%04X\n", Operation[i]);
    // }
    
    unsigned char* membase;
    if (fpga_open() == 0)
        exit(1);

    fpga.dma_ctrl = CGRA_info.dma_mmap;
    membase = (unsigned char*)CGRA_info.ddr_mmap;

    //Khai báo vùng con trỏ trên FPGA

    uint32_t* A     =   (uint32_t*)(membase + A_BASE     );
    uint32_t* B     =   (uint32_t*)(membase + B_BASE     );
    uint32_t* Op    =   (uint32_t*)(membase + OP_BASE    );
    uint32_t* Out   =   (uint32_t*)(membase + OUT_BASE   );
    uint32_t* Done  =   (uint32_t*)(CGRA_info.pio_32_mmap + DONE_BASE);
    uint32_t* Start =   (uint32_t*)(CGRA_info.pio_32_mmap + START_BASE);

    for (int i = 0; i < SIZE; i++) {
        A[i] = Input_A[i];
        B[i] = Input_B[i];
        Op[i] = Operation[i];
    }
    printf("membase = %016llX\n", membase);
    printf("A = %08X\n", A);
    printf("B = %08X\n", B);
    printf("Out = %08X\n", Out);
    printf("Op = %08X\n", Op);
    
    printf("Writing to FPGA\n");
    
    dma_write(A_BASE , SIZE);
    dma_write(B_BASE , SIZE);
    dma_write(OP_BASE, SIZE);

    *Start = 1;
    printf("Waiting for FPGA\n");
    // printf("Done = %d\n", *Done);
    while (*Done == 0);
    printf("FPGA done\n");
    dma_read(OUT_BASE, SIZE);
    // Ghi kết quả ra file
    write_output(Out);

    return 0;
}