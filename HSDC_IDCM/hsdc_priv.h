/***********************************************************************************************************/
/**
* \file       hsdc_priv.c
* \brief      High-side driver control
* \details    N/A
*
* \author     lub
* \date       2023-07-03
* \par        History:
*
*/
/********************* (C) Copyright 2023 Magneti Marelli Guangzhou ****************************************/

#ifndef HSDC_PRIV_H
#define HSDC_PRIV_H


/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Platform_Types.h"
/*______ G L O B A L - D E F I N E S _________________________________________*/
/*read command*/
#define READ_REG_OUT          0x00u
#define READ_REG_RCS          0x08u
#define READ_REG_SRC          0x09u
#define READ_REG_OCR          0x04u
#define READ_REG_RCD          0x0Cu
#define READ_REG_KRC          0x05u
#define READ_REG_PCS          0x0Du
#define READ_REG_HWCR         0x06u
#define READ_REG_ICS          0x0Eu
#define READ_REG_DCR          0x07u
#define READ_REG_WRN          0x01u
#define READ_REG_STD          0x02u
#define READ_REG_ERR          0x03u
#define READ_REG_MAX          0x0Du
/*write command*/
#define WRITE_REG_OUT          0x80u
#define WRITE_REG_OCR          0xC0u
#define WRITE_REG_RCD          0xC0u
#define WRITE_REG_KRC          0xD0u
#define WRITE_REG_PCS          0xD0u
#define WRITE_REG_HWCR         0xE0u
#define WRITE_REG_ICS          0xE0u
#define WRITE_REG_DCR          0xF0u

/*Response*/
#define RW_RESPONSE            0x80u
#define WRN_STD_RESPONSE       0x40u  
/*______ P R I V A T E - D E F I N E S _______________________________________*/

/*______ G L O B A L - T Y P E _______________________________________________*/
typedef enum
{
  HSDC_INDEX_TABLE_PING = 0,
  HSDC_INDEX_TABLE_PONG
}Hsdc_IndexTableType_t;

typedef enum{
  CMD_READ_REG_OUT,
  CMD_READ_REG_RCS,
  CMD_READ_REG_SRC,
  CMD_READ_REG_OCR,
  CMD_READ_REG_RCD,
  CMD_READ_REG_KRC,
  CMD_READ_REG_PCS,
  CMD_READ_REG_HWCR,
  CMD_READ_REG_ICS,
  CMD_READ_REG_DCR,
  CMD_READ_REG_WRN,
  CMD_READ_REG_STD,
  CMD_READ_REG_ERR,

  CMD_WRITE_REG_OUT,
  CMD_WRITE_REG_OCR,
  CMD_WRITE_REG_RCD,
  CMD_WRITE_REG_KRC,
  CMD_WRITE_REG_PCS,
  CMD_WRITE_REG_HWCR,
  CMD_WRITE_REG_ICS,
  CMD_WRITE_REG_DCR,
  HSDC_MAX_INDEX_NUM
}Hsdc_CmdIndexTable_t;

typedef enum{
  OUT_CHANNEL_0 = 0x01,
  OUT_CHANNEL_1 = 0x02,
  OUT_CHANNEL_2 = 0x04,
  OUT_CHANNEL_3 = 0x08,
  OUT_CHANNEL_4 = 0x10,
  OUT_CHANNEL_5 = 0x20
}Hsdc_OUT_Channel_t;

typedef enum{
  OCR_CHANNEL_0 = 0x02,
  OCR_CHANNEL_1 = 0x04,
  OCR_CHANNEL_2 = 0x08
}Hsdc_OCR_Channel_t;

typedef enum{
  RCD_CHANNEL_0 = 0x02,
  RCD_CHANNEL_1 = 0x04,
  RCD_CHANNEL_2 = 0x08
}Hsdc_RCD_Channel_t;

typedef enum{
  KRC_CHANNEL_0,
  KRC_CHANNEL_1,
  KRC_CHANNEL_2
}Hsdc_KRC_Channel_t;

typedef enum{
  ICS_CHANNEL_0,
  ICS_CHANNEL_1,
  ICS_CHANNEL_2,
  ICS_CHANNEL_3
}Hsdc_ICS_Channel_t;

typedef enum{
  MUX_ROUT_CHANNEL_0,
  MUX_ROUT_CHANNEL_1,
  MUX_ROUT_CHANNEL_2,
  MUX_ROUT_CHANNEL_3,
  MUX_ROUT_CHANNEL_4,
  MUX_ROUT_CHANNEL_5,
  MUX_HIGH_IMPEDANCE,
  MUX_SLEEP_MODE
}Hsdc_MUX_Channel_t;

/*Output configuration*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 OUTn:6;     /*Output Control Register of Channel n*/
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_OUT_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 OUTn:6;     /*Output Control Register of Channel n*/
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_OUT_ReadReg_t;

/*Restart counter status (read-only)*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RCSn:3;     /*Restart Counter Status of Channel selected via MUX*/
    uint8 RESERVED:5;
  }Reg;
} Hsdc_RCS_ReadReg_t;

/*Slew Rate Control register (read-only)*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 SRCn:6;     /*Set Slew Rate control for Channel n (read only)*/
    uint8 RESERVED:2;
  }Reg;
} Hsdc_SRC_ReadReg_t;

/*Overcurrent threshold configuration*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED:1;
    uint8 OCTn:3;     /*Set Overcurrent Level for Channel n*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_OCR_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED1:1;
    uint8 OCTn:3;     /*Set Overcurrent Level for Channel n*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_OCR_ReadReg_t;

/*Restart counter disable*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED:1;
    uint8 RCDn:3;     /*Set Restart Strategy for Channel n*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_RCD_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED1:1;
    uint8 RCDn:3;     /*Set Restart Strategy for Channel n*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_RCD_ReadReg_t;

/*KILIS range control*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED:1;
    uint8 KRCn:3;     /*Set Current Sense Ratio Range for Channel n*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_KRC_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED1:1;
    uint8 KRCn:3;     /*Set Current Sense Ratio Range for Channel n*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_KRC_ReadReg_t;

/*Parallel channel and Slew Rate control*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 SRCS:1;     /*Set Slew Rate control for Channel selected by DCR.MUX*/
    uint8 CLCS:1;     /*Clear Restart Counters and Latches for Channel selected by DCR.MUX*/
    uint8 RESERVED:1;
    uint8 PCCn:1;     /*Parallel Channel Configuration*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_PCS_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED1:3;
    uint8 PCCn:1;     /*Parallel Channel Configuration*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_PCS_ReadReg_t;

/*Hardware configuration*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 CLC:1;      /*Clear Restart Counters and Latches*/
    uint8 RST:1;      /*Reset Command*/
    uint8 COL:1;      /*Input Combinatorial Logic Configuration*/
    uint8 RESERVED:1;
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_HWCR_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 RESERVED1:1;
    uint8 SLPn:1;     /*Sleep Mode*/
    uint8 COLn:1;     /*Input Combinatorial Logic Configuration*/
    uint8 RESERVED:5;
  }Reg;
} Hsdc_HWCR_ReadReg_t;

/*Input status & checksum input*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 CSRn:4;     /*Checksum Input Register*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_ICS_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 INSTn:4;    /*Input Status Monitor Channel n*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_ICS_ReadReg_t;

/*Diagnostic configuration and Swap bit*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 MUX:3;      /*Set Current Sense Multiplexer Configuration in OFF state*/
    uint8 SWR:1;      /*Switch Register*/
    uint8 ADDR0:2;
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_DCR_WriteReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 MUX:3;      /*Set Current Sense Multiplexer Configuration in OFF state*/
    uint8 SWRn:1;     /*Switch Register*/
    uint8 RESERVED:4;
  }Reg;
} Hsdc_DCR_ReadReg_t;

/*Diagnosis Registers*/
typedef union
{
  uint8 Byte;
  struct
  {
    uint8 WRNn:6;   /*Warning Diagnosis of Channel n*/
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_WRN_ReadReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 VSMON:1;  /*VS monitor*/
    uint8 SBM:1;    /*Switch Bypass Monitor*/
    uint8 SLP:1;    /*Sleep mode monitor*/
    uint8 LHI:1;    /*Limp Home monitor*/
    uint8 CSV:1;    /*Checksum Verification*/
    uint8 TER:1;    /*Transmission Error*/
    uint8 RB:1;     /*Register Bank*/
    uint8 RW:1;
  }Reg;
} Hsdc_STD_ReadReg_t;

typedef union
{
  uint8 Byte;
  struct
  {
    uint8 ERRn:6;   /*Error Diagnosis of Channel n*/
    uint8 RB:1;
    uint8 RW:1;
  }Reg;
} Hsdc_ERR_ReadReg_t;

typedef struct 
{
  Hsdc_OUT_ReadReg_t OUT;
  Hsdc_RCS_ReadReg_t RCS;
  Hsdc_SRC_ReadReg_t SRC;
  Hsdc_OCR_ReadReg_t OCR;
  Hsdc_RCD_ReadReg_t RCD;
  Hsdc_KRC_ReadReg_t KRC;
  Hsdc_PCS_ReadReg_t PCS;
  Hsdc_HWCR_ReadReg_t HWCR;
  Hsdc_ICS_ReadReg_t ICS;
  Hsdc_DCR_ReadReg_t DCR;
  Hsdc_WRN_ReadReg_t WRN;
  Hsdc_STD_ReadReg_t STD;
  Hsdc_ERR_ReadReg_t ERR;
}Hsdc_AllRegister_t;

/*______ G L O B A L - D A T A _______________________________________________*/

/*______ G L O B A L - M A C R O S ___________________________________________*/

/*______ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/


#endif /* HSDC_PRIV_H */

/*______ E N D _____ (hsdc_priv.h) _________________________________________*/
