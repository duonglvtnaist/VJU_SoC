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

#define SIZE 1024 // BRAM có size là 2048



#define PADDING_BASE	 0x00100000  //  Offset to avoid system files

#define A_DMA_BASE           0x0000000000+PADDING_BASE  // Địa chỉ sẽ giống dưới phần cứng ở phần thấp. => 0x00_A810_0000  
#define B_DMA_BASE           0x0000000004+PADDING_BASE  // Địa chỉ sẽ giống dưới phần cứng ở phần thấp. => 0x00_A810_0004 
#define X_DMA_BASE           0x0000000008+PADDING_BASE  // Địa chỉ sẽ giống dưới phần cứng ở phần thấp. => 0x00_A810_0008 
#define O_DMA_BASE           0x000000000C+PADDING_BASE  // Địa chỉ sẽ giống dưới phần cứng ở phần thấp. => 0x00_A810_000C


// PIO Address define

#define A_PIO_BASE           0x0000000000
#define B_PIO_BASE           0x0000000001
#define X_PIO_BASE           0x0000000002
#define O_PIO_BASE           0x0000000003


int main() {

    printf("Hello World Today Is Monday\n");

    // Mở FPGA
    
    unsigned char* membase;
    if (fpga_open() == 0)
        exit(1);

    fpga.dma_ctrl = CGRA_info.dma_mmap;
    membase = (unsigned char*)CGRA_info.ddr_mmap;

    //Khai báo vùng con trỏ trên FPGA

    uint32_t* A_DMA   =   (uint32_t*)(membase + A_DMA_BASE);
    uint32_t* B_DMA   =   (uint32_t*)(membase + B_DMA_BASE);
    uint32_t* X_DMA   =   (uint32_t*)(membase + X_DMA_BASE);
    uint32_t* O_DMA   =   (uint32_t*)(membase + O_DMA_BASE);

    uint32_t* A_PIO   =   (uint32_t*)(CGRA_info.pio_32_mmap + A_PIO_BASE);
    uint32_t* B_PIO   =   (uint32_t*)(CGRA_info.pio_32_mmap + B_PIO_BASE);
    uint32_t* X_PIO   =   (uint32_t*)(CGRA_info.pio_32_mmap + X_PIO_BASE);
    uint32_t* O_PIO   =   (uint32_t*)(CGRA_info.pio_32_mmap + O_PIO_BASE);

    // Test DMA

    A_DMA[0] = 4721;
    B_DMA[0] = 2648;
    X_DMA[0] = 5;

    printf("DMA Test\n");

    printf("Expected Output = A*X + B = %d\n", A_DMA[0]*X_DMA[0] + B_DMA[0]);
    dma_write(A_DMA_BASE , 6); // dma_write(B_DMA_BASE , 1);
   // dma_write(X_DMA_BASE , 1);

    dma_read(O_DMA_BASE, 1);

    printf("Output from FPGA through DMA = %d\n", O_DMA[0]);

    // Test PIO

    printf("PIO Test\n");
   
    int a_temp, b_temp, x_temp;

    a_temp = 8721;
    b_temp = 1035;
    x_temp = 6; 
    
    A_PIO[0] = a_temp;
    B_PIO[0] = b_temp;
    X_PIO[0] = x_temp;

    printf("Expected Output = A*X + B = %d\n", a_temp*x_temp + b_temp);

    printf("Output from FPGA through PIO = %d\n", O_PIO[0]);
    
    return 0;
}
