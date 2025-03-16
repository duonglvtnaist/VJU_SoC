// Created by: Le Vu Trung Duong
// Created on: 2025-03-06
// Description: This file includes the FPGA driver functions to interact with the FPGA.


#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <linux/ioctl.h>


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dirent.h>

#ifndef alphasort
  #define alphasort(x, y) strcoll((*(struct dirent **)x)->d_name, (*(struct dirent **)y)->d_name)
#endif


#define DMA_BASE_PHYS	 0x00000000fd500000LL
/*  ... fixed */
#define DMA_MMAP_SIZE	 0x0000000000010000LL
/*  ... 64KB  */
#define REG_BASE_PHYS	 0x00000000A0000000LL
/*  ... fixed */
#define REG_MMAP_SIZE	 0x0000000010000000LL
/*  ... 4GB(including REGS) */
#define LMM_BASE_PHYS	 0x00000000A0000000LL // This is based address for DMA transfer
/*  ... fixed */
#define DDR_BASE_PHYS	 0x0000000800000000LL
/*  ... fixed */
#define DDR_MMAP_SIZE	 0x0000000080000000LL
/*  ... 2GB   */

///*** PEA Controller space **///

#define START_BASE_IP	 0x0000000000000000LL
#define FINISH_BASE_IP   0x0000000000000020LL

///*** PEA CFG space **///
#define CTX_PE_BASE_IP	 0x0000000001000000LL
#define CTX_RC_BASE_IP	 0x0000000002000000LL
#define CTX_IM_BASE_IP	 0x0000000003000000LL
///*** LDM space **///
//#define PADDING_BASE	 0x01000000  // 16MB Ofset to avoid system files

// ROWx base address - Each row used bit [5:15]
#define ROW0_BASE_PHYS	 (0x00000000 + PADDING_BASE)  
#define ROW1_BASE_PHYS	 (0x00008000 + PADDING_BASE)
#define ROW2_BASE_PHYS	 (0x00010000 + PADDING_BASE)
#define ROW3_BASE_PHYS	 (0x00018000 + PADDING_BASE)

typedef uint64_t U64;
typedef uint32_t U32;

struct fpga { /* host status of U2BCA */
  volatile U64   dma_ctrl;  /* struct dma_ctrl *dma_ctrl  DMA control */
  volatile U64   reg_ctrl;  /* struct reg_ctrl *reg_ctrl  REG control */

  U64   status            : 4;
  // U64   csel_save         : 2;
  // U64   last_conf            ; /* for insn_reuse */
  // U64   lmmic             : 1; /* 0:lmm[0] is curent, 1:lmm[1] is current */
  // U64   lmmio             : 1; /* 0:lmm[0] is prev,   1:lmm[1] is prev    */
  // U64   mapdist           : 6; /* specified mapdist */
  // U64   lastdist          : 6; /* lastdist for DYNAMIC_SCON */
  // struct lmmi lmmi[EMAX_NCHIP][AMAP_DEPTH][EMAX_WIDTH][2]; /* lmmi for host (len/ofs/top are resolved) */
  // U64   lmmi_bitmap[EMAX_WIDTH];      /* based on lmmi[*][EMAX_WIDTH][2].v */
  // Uchar lmmd[AMAP_DEPTH][EMAX_WIDTH]; /* chip#7,6,..,0:clean, 1:dirty, exec��store�ս��1, drainľ��0 */

  U64   rw                    ; /* 0:load(mem->lmm), 1:store(lmm->mem)      */
  U64   ddraddr               ; /* ddr-address                              */
  U64   lmmaddr               ; /* lmm-address                              */
  U64   dmalen                ; /* dma-length                               */

} fpga;

struct dma_ctrl {
  /*   Register Name		   Address	Width	Type	Reset Value	Description */
  U32 ZDMA_ERR_CTRL;    	/* 0x00000000	32	mixed	0x00000001	Enable/Disable a error response */
  U32 dmy0[63];
  U32 ZDMA_CH_ISR;      	/* 0x00000100	32	mixed	0x00000000	Interrupt Status Register for intrN. This is a sticky register that holds the value of the interrupt until cleared by a value of 1. */
  U32 ZDMA_CH_IMR;      	/* 0x00000104	32	mixed	0x00000FFF	Interrupt Mask Register for intrN. This is a read-only location and can be atomically altered by either the IDR or the IER. */
  U32 ZDMA_CH_IEN;      	/* 0x00000108	32	mixed	0x00000000	Interrupt Enable Register. A write of to this location will unmask the interrupt. (IMR: 0) */
  U32 ZDMA_CH_IDS;      	/* 0x0000010C	32	mixed	0x00000000	Interrupt Disable Register. A write of one to this location will mask the interrupt. (IMR: 1) */
  U32 ZDMA_CH_CTRL0;    	/* 0x00000110��	32	mixed	0x00000080	Channel Control Register 0 */

  U32 ZDMA_CH_CTRL1;    	/* 0x00000114	32	mixed	0x000003FF	Channel Flow Control Register */
  U32 ZDMA_CH_FCI;      	/* 0x00000118	32	mixed 	0x00000000	Channel Control Register 1 */
  U32 ZDMA_CH_STATUS;   	/* 0x0000011C��	32	mixed	0x00000000	Channel Status Register */

  U32 ZDMA_CH_DATA_ATTR;	/* 0x00000120	32	mixed	0x0483D20F	Channel DATA AXI parameter Register */
  U32 ZDMA_CH_DSCR_ATTR;	/* 0x00000124	32	mixed	0x00000000	Channel DSCR AXI parameter Register */
  U32 ZDMA_CH_SRC_DSCR_WORD0;	/* 0x00000128��	32	rw	0x00000000	SRC DSCR Word 0 */
  U32 ZDMA_CH_SRC_DSCR_WORD1;  /* 0x0000012C��	32	mixed	0x00000000	SRC DSCR Word 1 */
  U32 ZDMA_CH_SRC_DSCR_WORD2;  /* 0x00000130��	32	mixed	0x00000000	SRC DSCR Word 2 */

  U32 ZDMA_CH_SRC_DSCR_WORD3;  /* 0x00000134	32	mixed	0x00000000	SRC DSCR Word 3 */
  U32 ZDMA_CH_DST_DSCR_WORD0;  /* 0x00000138��	32	rw	0x00000000	DST DSCR Word 0 */
  U32 ZDMA_CH_DST_DSCR_WORD1;  /* 0x0000013C��	32	mixed	0x00000000	DST DSCR Word 1 */
  U32 ZDMA_CH_DST_DSCR_WORD2;  /* 0x00000140��	32	mixed	0x00000000	DST DSCR Word 2 */

  U32 ZDMA_CH_DST_DSCR_WORD3;  /* 0x00000144	32	mixed	0x00000000	DST DSCR Word 3 */
  U32 ZDMA_CH_WR_ONLY_WORD0;   /* 0x00000148	32	rw	0x00000000	Write Only Data Word 0 */
  U32 ZDMA_CH_WR_ONLY_WORD1;   /* 0x0000014C	32	rw	0x00000000	Write Only Data Word 1 */
  U32 ZDMA_CH_WR_ONLY_WORD2;   /* 0x00000150	32	rw	0x00000000	Write Only Data Word 2 */
  U32 ZDMA_CH_WR_ONLY_WORD3;   /* 0x00000154	32	rw	0x00000000	Write Only Data Word 3 */
  U32 ZDMA_CH_SRC_START_LSB;   /* 0x00000158	32	rw	0x00000000	SRC DSCR Start Address LSB Regiser */
  U32 ZDMA_CH_SRC_START_MSB;   /* 0x0000015C	32	mixed	0x00000000	SRC DSCR Start Address MSB Regiser */
  U32 ZDMA_CH_DST_START_LSB;   /* 0x00000160	32	rw	0x00000000	DST DSCR Start Address LSB Regiser */
  U32 ZDMA_CH_DST_START_MSB;   /* 0x00000164	32	mixed	0x00000000	DST DSCR Start Address MSB Regiser */
  U32 dmy1[9];
  U32 ZDMA_CH_RATE_CTRL;       /* 0x0000018C	32	mixed	0x00000000	Rate Control Count Register */
  U32 ZDMA_CH_IRQ_SRC_ACCT;    /* 0x00000190	32	mixed	0x00000000	SRC Interrupt Account Count Register */
  U32 ZDMA_CH_IRQ_DST_ACCT;    /* 0x00000194	32	mixed	0x00000000	DST Interrupt Account Count Register */
  U32 dmy2[26];
  U32 ZDMA_CH_CTRL2;  		/* 0x00000200��	32	mixed	0x00000000	zDMA Control Register 2 */
};

volatile struct CGRA_info {
//** For Transfer on ARM ZYNQ **//
  U64  dma_phys;     // kern-phys
  U64  dma_vadr;     // not used
  U64  dma_mmap;     // user-virt Contiguous 64K register space
  U64  reg_phys;     // kern-phys
  U64  reg_vadr;     // not used
  U32  *pio_32_mmap;     // user-virt Contiguous 4GB space including LMM space
  U64  *pio_64_mmap;     // user-virt Contiguous 4GB space including LMM space
  U32  *ctx_im_mmap;     // user-virt Contiguous 4GB space including LMM space
  U64  ctx_pe_offset;
  U64  ctx_rc_offset;
  U64  ctx_im_offset;  
  U64  lmm_phys;     // kern-phys
  U64  lmm_vadr;     // not used
  U64  lmm_mmap;     // user-virt Contiguous 2GB space for LMM space
  U64  ddr_phys;     // kern-phys
  U64  ddr_vadr;     // not used
  U64  ddr_mmap;     // user-virt Contiguous 2GB space in DDR-high-2GB space
  int  driver_use_1;
  int  driver_use_2;

  //** For Simulation on Vivado **//
  FILE *CTX_RC_File;
  FILE *CTX_PE_File;
  FILE *CTX_IM_File;
  int  PE_Counter;
  int  Error_Counter;
  int  Warning_Counter;
  FILE *LDM_File;
  FILE *common_File;
  U32  LDM_Offset;
} CGRA_info;

// Check directory name is not "."?
static int filter(const struct dirent *dir)
{
  return dir->d_name[0] == '.' ? 0 : 1;
}


// Remove '\n' from directory name
static void trim(char *d_name)
{
  char *p = strchr(d_name, '\n');
  if (p != NULL) *p = '\0';
}

// From the uioxx -> read the name of the device -> compare with the target name
static int is_target_dev(char *d_name, char *target)
{
  char path[32];
  char name[32];
  FILE *fp;
  sprintf(path, "/sys/class/uio/%s/name", d_name);
  if ((fp = fopen(path, "r")) == NULL) return 0;
  // Read the name of the device
  if (fgets(name, sizeof(name), fp) == NULL) {
    fclose(fp);
    return 0;
  }
  fclose(fp);
  // Compare the name of the device with the target name
  if (strcmp(name, target) != 0) return 0;
  return 1;
}

// Get the size of the register
static int get_reg_size(char *d_name)
{
  char path[32];
  char size[32];
  FILE *fp;
  sprintf(path, "/sys/class/uio/%s/maps/map0/size", d_name);
  if ((fp = fopen(path, "r")) == NULL) return 0;
  if (fgets(size, sizeof(size), fp) == NULL) {
    fclose(fp);
    return 0;
  }
  fclose(fp);
  // Convert the size from hex to decimal
  return strtoull(size, NULL, 16);
}

int fpga_open()
{
  struct dirent **namelist;
  int num_dirs, dir;
  int reg_size;
  int  fd_dma_found = 0;
  char path[1024];
  int  fd_dma;
  int  fd_reg;
  int  fd_ddr;
  char *UIO_DMA           = "dma-controller\n";
  char *UIO_AXI_CGRA     = "MY_IP\n";
  char *UIO_DDR_HIGH      = "ddr_high\n";
  
  // Scan the directory
  if ((num_dirs = scandir("/sys/class/uio", &namelist, filter, alphasort)) == -1)
    return -1;

  // Browse for each directory
  for (dir = 0; dir < num_dirs; ++dir) {
    trim(namelist[dir]->d_name); // Remove '\n'
    // Check the target device is dma-controller
    if (!fd_dma_found && is_target_dev(namelist[dir]->d_name, UIO_DMA) && (reg_size = get_reg_size(namelist[dir]->d_name))) {
      // If the target device is not CGRA, then continue
      if (strlen(namelist[dir]->d_name)>4) /* ignore /dev/uio1X */
	    continue;
      sprintf(path, "/dev/%s", namelist[dir]->d_name);  // assign path = /dev/uio4:dma-controller
      free(namelist[dir]);
      if ((fd_dma = open(path, O_RDWR | O_SYNC)) == -1) // open /dev/uio4:dma-controller for read and write 
	    continue;
      printf("%s: %s", path, UIO_DMA);
      CGRA_info.dma_phys = DMA_BASE_PHYS; // 0x00000000fd500000LL Assign the physical address of DMA
      CGRA_info.dma_mmap = (U64)mmap(NULL, reg_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_dma, 0); // mmap(cache-on)  4KB aligned
      close(fd_dma);
      if (CGRA_info.dma_mmap == (U64)(uintptr_t)MAP_FAILED)
	    continue;
      fd_dma_found++;
    }
    // Check the target device is CGRA
    else if (is_target_dev(namelist[dir]->d_name, UIO_AXI_CGRA)) {
      sprintf(path, "/dev/%s", namelist[dir]->d_name);
      free(namelist[dir]);
      if ((fd_reg = open(path, O_RDWR | O_SYNC)) == -1) {
	printf("open failed. %s", UIO_AXI_CGRA);
	return -1;
      }
      printf("%s: %s", path, UIO_AXI_CGRA);
      // mmap(cache-off) 4KB aligned

        
      CGRA_info.reg_phys = REG_BASE_PHYS; // 0x00000000A0000000LL
	  CGRA_info.ctx_pe_offset = CTX_PE_BASE_IP >> 2; // 0x0000000001000000LL >> 2 = 0x0000000000400000LL Revising address for PIO PE CTX transfer 32-bit
      CGRA_info.pio_32_mmap = (U32*)mmap(NULL, REG_MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_reg, 0); /* 256MB */
      if (CGRA_info.pio_32_mmap == MAP_FAILED) {
		printf("pio_32_mmap failed. errno=%d\n", errno);
		return -1;
      }
	  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  CGRA_info.ctx_rc_offset = CTX_RC_BASE_IP >> 3; // 0x0000000002000000LL >> 3 = 0x0000000000400000LL Revising address for PIO RC CTX transfer 64-bit quan trong
      CGRA_info.pio_64_mmap = (U64*)mmap(NULL, REG_MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_reg, 0); /* 4GB */
      if (CGRA_info.pio_64_mmap == MAP_FAILED) {
		printf("pio_64_mmap failed. errno=%d\n", errno);
		return -1;
      }
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  CGRA_info.ctx_im_offset = CTX_IM_BASE_IP >> 2;  // 0x0000000003000000LL >> 2 = 0x0000000000C00000LL Revising address for PIO IM CTX transfer 32-bit
      CGRA_info.ctx_im_mmap = (U32*)mmap(NULL, REG_MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_reg, 0); /* 4GB */
      if (CGRA_info.ctx_im_mmap == MAP_FAILED) {
		printf("ctx_im_mmap failed. errno=%d\n", errno);
		return -1;
      }
	  
      CGRA_info.lmm_phys = LMM_BASE_PHYS;
      CGRA_info.lmm_mmap = (LMM_BASE_PHYS - REG_BASE_PHYS);
    }
    else if (is_target_dev(namelist[dir]->d_name, UIO_DDR_HIGH)) {
      sprintf(path, "/dev/%s", namelist[dir]->d_name);
      free(namelist[dir]);
      if ((fd_ddr = open(path, O_RDWR | O_SYNC)) == -1) {
	printf("open failed. %s",UIO_DDR_HIGH);
	return -1;
      }
      printf("%s: %s", path, UIO_DDR_HIGH);
      // mmap(cache-on)  4KB aligned
      CGRA_info.ddr_phys = DDR_BASE_PHYS;
      CGRA_info.ddr_mmap = (U64)mmap(NULL, DDR_MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_ddr, 0); /* 2GB */
      if ((void*)CGRA_info.ddr_mmap == MAP_FAILED) {
	printf("fd_ddr mmap() failed. errno=%d\n", errno);
	return -1;
      }
    }
    else {
      free(namelist[dir]);
      continue;
    }
  }
  free(namelist);

  if (fd_dma_found) {
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_ERR_CTRL          = 0x00000001;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_ISR            = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_IMR            = 0x00000FFF;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_IEN            = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_IDS            = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_CTRL0          = 0x00000080;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_CTRL1          = 0x000003FF;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_FCI            = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_STATUS         = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DATA_ATTR      = 0x04C3D30F; /* Note - AxCACHE: 0011 value recomended by Xilinx. */
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DSCR_ATTR      = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_DSCR_WORD0 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_DSCR_WORD1 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_DSCR_WORD2 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_DSCR_WORD3 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_DSCR_WORD0 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_DSCR_WORD1 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_DSCR_WORD2 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_DSCR_WORD3 = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_WR_ONLY_WORD0  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_WR_ONLY_WORD1  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_WR_ONLY_WORD2  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_WR_ONLY_WORD3  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_START_LSB  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_SRC_START_MSB  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_START_LSB  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_DST_START_MSB  = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_RATE_CTRL      = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_IRQ_SRC_ACCT   = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_IRQ_DST_ACCT   = 0x00000000;
    ((struct dma_ctrl*)CGRA_info.dma_mmap)->ZDMA_CH_CTRL2          = 0x00000000;
  }
  return (1);
}
void dma_write(U64 Offset, U32 size){
    // This is DMA write function: from DDR to CGRA
	int status;
      // Assign the source address in DDR
	  *(U64*)&(((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_SRC_DSCR_WORD0) = DDR_BASE_PHYS + Offset;
      // Assign the size of the data to be transferred on DDR. One unit of size = 8 * size of U32
      ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_SRC_DSCR_WORD2 = size*sizeof(U32);
      // Assign the destination address in CGRA
	  *(U64*)&(((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_DST_DSCR_WORD0) = LMM_BASE_PHYS + Offset;
      // Assign the size of the data to be received on CGRA. One unit of size = 8 * size of U32
	  ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_DST_DSCR_WORD2 = size*sizeof(U32);
      // Start the DMA transfer
	  ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_CTRL2 = 1;
      // Wait for the DMA transfer to complete
      do {
          status = ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_STATUS & 3;
      } while (status != 0 && status != 3);
}

void dma_read(U64 Offset, U32 size){
    // This is DMA read function from: CGRA to DDR
	int status;
      // Assign the source address in CGRA
	  *(U64*)&(((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_SRC_DSCR_WORD0) = LMM_BASE_PHYS + Offset;
      // Assign the size of the data to be transferred on CGRA. One unit of size = 8 * size of U32
      ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_SRC_DSCR_WORD2 = size*sizeof(U32);
      // Assign the destination address in DDR
	  *(U64*)&(((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_DST_DSCR_WORD0) = DDR_BASE_PHYS + Offset;
      // Assign the size of the data to be received on DDR. One unit of size = 8 * size of U32
	  ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_DST_DSCR_WORD2 = size*sizeof(U32);
      // Start the DMA transfer
	  ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_CTRL2 = 1;
      // Wait for the DMA transfer to complete
      do {
	      status = ((struct dma_ctrl*)fpga.dma_ctrl)->ZDMA_CH_STATUS & 3;
      } while (status != 0 && status != 3);
}										   

