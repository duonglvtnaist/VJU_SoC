// Created by: Le Vu Trung Duong
// Created on: 2025-03-06
// Description: This file is used to test the FPGA driver by sending data to the FPGA and receiving the result back from the FPGA.

// This file contains 5 ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "./KV260_Driver.c" // call fpga driver


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SIZE 1024

#define READ_BASE            0x0000100000

#define IN_DMA_BASE          0x0000000000  
#define OUT_DMA_BASE         IN_DMA_BASE  + READ_BASE

#define START_PIO_BASE       0x0000040000               // 0x0000000001 << (18-2)       
#define VALID_PIO_BASE       0x0000040000 + READ_BASE   // 0x0000000001 << (18-2)   
#define WIDTH_PIO_BASE       0x0000080000               // 0x0000000002 << (18-2)          
#define HEIGHT_PIO_BASE      0x00000C0000               // 0x0000000003 << (18-2)          




#define FILTER_SIZE 3 // Kích thước của bộ lọc (3x3)


//  Hoán đổi giá trị
void swap(uint8_t *a, uint8_t *b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

// Sắp xếp mảng bằng bubble sort
void bubble_sort(uint8_t arr[], int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j){
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
        }
}

// Lọc median
void median_filter(uint8_t **input, uint8_t **output, int height, int width) {
    int indexer = FILTER_SIZE / 2;
    uint8_t window[FILTER_SIZE * FILTER_SIZE];

    printf("FILTER_SIZE = %d\n", FILTER_SIZE);
    printf("height = %d\n", height);
    printf("width = %d\n", width);
    printf("indexer = %d\n", indexer);
    printf("FILTER_SIZE * FILTER_SIZE = %d\n", FILTER_SIZE * FILTER_SIZE);
    printf("FILTER_SIZE * FILTER_SIZE / 2 = %d\n", FILTER_SIZE * FILTER_SIZE / 2);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int count = 0;
            for (int fi = -indexer; fi <= indexer; ++fi) {  // fi from -1 to 1
                for (int fj = -indexer; fj <= indexer; ++fj) { // fj from -1 to 1
                    int ni = i + fi;    // ni = i + fi
                    int nj = j + fj;    // nj = j + fj

                    if((ni < 0) && (nj < 0)){
                        printf("This is a negative index\n");
                    }

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        window[count++] = input[ni][nj];
                    } else {
                        window[count++] = 0; // padding bằng 0
                    }
                }
            }
            bubble_sort(window, FILTER_SIZE * FILTER_SIZE);
            output[i][j] = window[FILTER_SIZE * FILTER_SIZE / 2];
        }
    }
}

// Cấp phát bộ nhớ động cho ảnh 2D
uint8_t **allocate_image(int height, int width) {
    uint8_t **img = (uint8_t **)malloc(height * sizeof(uint8_t *));
    for (int i = 0; i < height; ++i)
        img[i] = (uint8_t *)malloc(width);
    return img;
}

// Giải phóng bộ nhớ ảnh
void free_image(uint8_t **img, int height) {
    for (int i = 0; i < height; ++i)
        free(img[i]);
    free(img);
}

// Đọc ảnh PGM định dạng P2 hoặc P5
uint8_t **read_pgm(const char *filename, int *height, int *width) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Không thể mở file %s\n", filename);
        exit(1);
    }

    char format[3];
    fscanf(f, "%2s", format);
    if (strcmp(format, "P5") != 0 && strcmp(format, "P2") != 0) {
        printf("File không phải định dạng P2 hoặc P5\n");
        fclose(f);
        exit(1);
    }

    // Bỏ qua comment nếu có
    int c = fgetc(f);
    while (c == '#') {
        while (fgetc(f) != '\n');
        c = fgetc(f);
    }
    ungetc(c, f);

    int maxval;
    fscanf(f, "%d %d\n%d\n", width, height, &maxval);

    uint8_t **img = allocate_image(*height, *width);

    if (strcmp(format, "P5") == 0) {
        // Đọc dữ liệu nhị phân
        for (int i = 0; i < *height; ++i)
            fread(img[i], sizeof(uint8_t), *width, f);
    } else {
        // Đọc dữ liệu văn bản (P2)
        for (int i = 0; i < *height; ++i)
            for (int j = 0; j < *width; ++j) {
                int pixel;
                fscanf(f, "%d", &pixel);
                img[i][j] = (uint8_t)pixel;
            }
    }

    fclose(f);
    return img;
}

// Ghi ảnh PGM ở định dạng P5
void write_pgm(const char *filename, uint8_t **img, int height, int width) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Không thể ghi file %s\n", filename);
        exit(1);
    }

    fprintf(f, "P5\n%d %d\n255\n", width, height);
    for (int i = 0; i < height; ++i)
        fwrite(img[i], sizeof(uint8_t), width, f);

    fclose(f);
}

void write_uint8_matrix_to_hex_txt(const char* filename, uint8_t** data, int width, int height) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(fp, "%02X\n", data[i][j]);
        }
    }

    fclose(fp);
}

// --------------------------------------------------------------------------------


int main() {

    printf("Hello World! Today Is Monday\n");

   // Đọc ảnh đầu vào

   int height, width;
   uint8_t **input = read_pgm("Fig2.pgm", &height, &width);
   uint8_t **output = allocate_image(height, width);

   write_uint8_matrix_to_hex_txt("Fig2.txt", input, width, height);

   printf("Đọc ảnh thành công: %d x %d\n", height, width);

   // Mở FPGA và các UIO cần thiết
   
   unsigned char* membase;
   if (fpga_open() == 0)
       exit(1);

   fpga.dma_ctrl = CGRA_info.dma_mmap;
   membase = (unsigned char*)CGRA_info.ddr_mmap;

    uint32_t* In_DDR4_DMA = (uint32_t*)(membase + IN_DMA_BASE);
    uint32_t* Out_DDR4_DMA = (uint32_t*)(membase + IN_DMA_BASE);

   int i, j;


   for(i = 0; i < height; i++){
       for(j = 0; j < width; j++){
           In_DDR4_DMA[i*width+j] = (uint32_t)input[i][j]; // Ghi dữ liệu vào DDR4
       }
   }

   dma_write(IN_DMA_BASE, 10); // Ghi dữ liệu vào FPGA thông qua DMA

   *(CGRA_info.pio_32_mmap + WIDTH_PIO_BASE)  = 1; // Bắt đầu xử lý trên FPGA
   *(CGRA_info.pio_32_mmap + HEIGHT_PIO_BASE) = 1; // Bắt đầu xử lý trên FPGA

   *(CGRA_info.pio_32_mmap + START_PIO_BASE) = 0; // Bắt đầu xử lý trên FPGA

   printf("VALID_PIO_BASE = %016llx\n", VALID_PIO_BASE);

   while (1)
   {
       if (*(CGRA_info.pio_32_mmap + VALID_PIO_BASE) == 0) // Kiểm tra xem FPGA đã xử lý xong chưa
           break;
   }

   printf("FPGA đã xử lý xong\n");

   dma_read(IN_DMA_BASE, height*width); // Đọc dữ liệu từ FPGA thông qua DMA

   for(i = 0; i < height; i++){
       for(j = 0; j < width; j++){
            output[i][j] = Out_DDR4_DMA[i*width+j];
            // output[i][j] = *(CGRA_info.pio_32_mmap + READ_BASE + i*width+j);
            if(i == 0 && j < 20)
           	   printf("output[%d][%d] = %d\n", i, j, output[i][j]);
        }
   }

    *(CGRA_info.pio_32_mmap + START_PIO_BASE) = 0;

    write_pgm("removed_noise.pgm", output, height, width);
    // Lệnh Python cần gọi để chuyển pgm thành jpg------------------------------------
    const char* command = "python pgm_to_jpeg.py removed_noise.pgm removed_noise.jpg";
    int result = system(command);

    if (result == -1) {
        printf("Lỗi khi gọi lệnh Python\n");
        return 1;
    } else {
        printf("Lệnh Python đã được thực thi thành công\n");
    }
    // -------------------------------------------------------------------------------
    write_uint8_matrix_to_hex_txt("removed_noise.txt", output, width, height);

    free_image(input, height);
    free_image(output, height);

    printf("Finish\n");

 
   
   return 0;
}
