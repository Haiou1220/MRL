/***********************************************************************************************************/
/**
* \file       modc.h
* \brief      H/Half-bridge Motor driver control
* \details    N/A
*
* \author     Yongbo Hou
* \date       2023-02-09
* \par        History:
*
\verbatim
  Version     Author         Date            Desc
  1.0.0       Yongbo Hou     2023-02-09      init version for chip--TI drv8718s-q1
\endverbatim
*
*/
/********************* (C) Copyright 2023 Magneti Marelli Guangzhou ****************************************/

#ifndef MODC_H
#define MODC_H

/*____________________________ I N C L U D E - F I L E S __________________________________________________*/
#include "spic.h"
#include "modc_config.h"
/*_______________________________ L O C A L - T Y P E S ___________________________________________________*/
#define MODC_TotalReg                      (0x23U)

typedef enum
{
  MODC_TRANSMIT_STEP_IDLE,
  MODC_TRANSMIT_STEP_READ_WRITE,
  MODC_TRANSMIT_STEP_CHECK,
/*  MODC_TRANSMIT_STEP_READ,
  MODC_TRANSMIT_STEP_READ_CHECK,*/
  MODC_TRANSMIT_STEP_DONE
}MODC_LatchFlagStep_t;

typedef enum
{
  MODC_HB_RWL,
  MODC_HB_HANDLE,
  MODC_HB_CHINCH,
  MODC_HB_NUM
}MODC_HbId_t;

typedef enum
{
  MODC_HB1_ONE,   /*for device PRE1*/
  MODC_HB1_TWO,
  MODC_HB1_THREE,
  MODC_HB1_FOUR,
  MODC_HB1_FIVE,
  MODC_HB1_SIX,
  MODC_HB1_SEVEN,
  MODC_HB1_EIGHT,

  MODC_HB2_ONE,   /*for device PRE2*/
  MODC_HB2_TWO,
  MODC_HB2_THREE,
  MODC_HB2_FOUR,
  MODC_HB2_FIVE,
  MODC_HB2_SIX,
  MODC_HB2_SEVEN,
  MODC_HB2_EIGHT,
  MODC_HB_MAX
}MODC_HalfBridgeNum_t;

typedef enum
{
  MODC_PRE1Bridge_ONE,   /*for device PRE1*/
  MODC_PRE1Bridge_TWO,
  MODC_PRE1Bridge_THREE,
  MODC_PRE1Bridge_FOUR,
  MODC_PRE1Bridge_FIVE,
  MODC_PRE1Bridge_SIX,
  MODC_PRE1Bridge_SEVEN,
  MODC_PRE1Bridge_EIGHT,
  MODC_PRE2Bridge_ONE,   /*for device PRE2*/
  MODC_PRE2Bridge_TWO,
  MODC_PRE2Bridge_THREE,
  MODC_PRE2Bridge_FOUR,
  MODC_PRE2Bridge_FIVE,
  MODC_PRE2Bridge_SIX,
  MODC_PRE2Bridge_SEVEN,
  MODC_PRE2Bridge_EIGHT,
  MODC_PRE1PRE2Bridge_MAX
}MODC_BridgeNum_t;


typedef enum
{
  MODC_GDS_HIGH_IMPEDANCE,
  MODC_GDS_DRIVE_LOW_SIDE,
  MODC_GDS_DRIVE_HIGH_SIDE,
  MODC_GDS_PWM1_HS_ACITVE_FW,
  MODC_GDS_PWM1_LS_ACITVE_FW,
  MODC_GDS_PWM1_HS_PASSIVE_FW,
  MODC_GDS_PWM1_LS_PASSIVE_FW,
  MODC_GDS_PWM2_HS_ACITVE_FW,
  MODC_GDS_PWM2_LS_ACITVE_FW,
  MODC_GDS_PWM2_HS_PASSIVE_FW,
  MODC_GDS_PWM2_LS_PASSIVE_FW,
  MODC_GDS_PWM3_HS_ACITVE_FW,
  MODC_GDS_PWM3_LS_ACITVE_FW,
  MODC_GDS_PWM3_HS_PASSIVE_FW,
  MODC_GDS_PWM3_LS_PASSIVE_FW,
  MODC_GDS_MAX
}MODC_GateDriverState_t;

typedef enum
{
  MODC_CHIP_ONE,
  MODC_CHIP_TWO
}MODC_ChipNum_t;

typedef enum
{
  MODC_SUBSM_IDLE,
  MODC_DISABLE_WATCH_DOG,
  MODC_READ_BANK0_REG,
  MODC_READ_BANK1_REG
}MODC_ReadStateReg_t;

typedef struct
{
  uint8 ChannelId;
  uint8 Direction;  /* motor direction of rotation */
}Modc_ChIdInQue_t;

typedef struct
{
  volatile uint8 In;   /* input index in queue - here will be a new id written */
  volatile uint8 Out;  /* output index from queue - here will be a new id read from */
  uint8          Size; /* size of queue, count of IM_IdInQue_t elements */
  volatile Modc_ChIdInQue_t * Buff; /* queue buffer - allocation is generated, this structure and pointer to buffer are initialised after reset */
}Modc_ChanQueue_t;

typedef struct
{
  Modc_SpiOutData_t Modc_SpiOutData[MODC_TotalReg];
  uint8 Modc_SetupSize;
}Modc_SpiOutBuffer_t;

typedef struct
{
  Modc_SpiInData_t Modc_SpiInData[MODC_TotalReg];
}Modc_SpiInBuffer_t;

typedef struct
{
  uint8 AddrByte[Modc_SLAVE_DEVICE_NUM];
  uint16 DataByte[Modc_SLAVE_DEVICE_NUM];
}Modc_SpiFrameOutData_t;

typedef struct
{
  uint8 StateByte[Modc_SLAVE_DEVICE_NUM];
  uint16 RespByte[Modc_SLAVE_DEVICE_NUM];
}Modc_SpiFrameInData_t;

typedef union
{
  uint16 FAULT;
  struct
  {
    uint16 fault_OC2 : 1;
    uint16 fault_OC1 : 1;
    uint16 fault_VSOV : 1; 
    uint16 fault_VSUV : 1; 

   /*  uint16 fault_TSD : 1; */
   /*  uint16 fault_CPUV : 1; */

    uint16 fault_DSOV : 1;
   /*  uint16 fault_HBxVOUT : 1; */ /* normd 0xff */
  /*   uint16 fault_HBxPWME : 1; */
  };
} Modc_DiagFault_t;

typedef struct
{
  uint16 ERR_OC_HS1 : 1;/* LSB */
  uint16 ERR_OC_LS1 : 1;
  uint16 ERR_OC_HS2 : 1;
  uint16 ERR_OC_LS2 : 1;
  uint16 ERR_OC_HS3 : 1;
  uint16 ERR_OC_LS3 : 1;
  uint16 ERR_OC_HS4 : 1;
  uint16 ERR_OC_LS4 : 1;
  uint16 ERR_OC_HS5 : 1;
  uint16 ERR_OC_LS5 : 1;
  uint16 ERR_OC_HS6 : 1;
  uint16 ERR_OC_LS6 : 1;
  uint16 ERR_OC_HS7 : 1;
  uint16 ERR_OC_LS7 : 1;
  uint16 ERR_OC_HS8 : 1;
  uint16 ERR_OC_LS8 : 1;/* MSB */
} ErrOC_t;

typedef union
{
  uint16 ErrOCState;
  ErrOC_t ErrOCBit;
} Modc_ErrOC_t;

typedef struct
{
  uint8 DiagState;

  boolean ErrTSD;
  boolean ErrOC2;
  boolean ErrOC1;
  boolean ErrVSOV;
  boolean ErrVSUV;
  boolean ErrCPUV;
  Modc_ErrOC_t ErrOC;
} Modc_DiagAllFault_t;

typedef struct
{
  MODC_BridgeNum_t xBridgeNum;
  MODC_BridgeNum_t yBridgeNum;
} Modc_MotorFullBridgeComp_t;

typedef enum
{
  Modc_UnknownConnect,
  Modc_ShortToGND,
  Modc_ShortToVBAT,
  Modc_OpenLoad,
  Modc_NormalConnect
} Modc_MotorConnectState_t;

typedef enum
{
  Modc_HBVOUT_MSB_LSB_LOW_LOW,
  Modc_HBVOUT_MSB_LSB_HIGH_LOW,
  Modc_HBVOUT_MSB_LSB_HIGH_HIGH,
  Modc_HBVOUT_MSB_LSB_MAX
} Modc_HBVOUTState_t;

typedef enum
{
  Modc_MotorCS_NotConfig,
  Modc_MotorCS_ConfigCompl
} Modc_MotorConnectStateConf_t;

typedef enum
{
  Modc_GetMotorCS_SGND,
  Modc_GetMotorCS_SVBAT_OL_NML
} Modc_GetMotorConnectStateType_t;

typedef enum
{
  Modc_MotorCS_DS_NULL,
  Modc_MotorCS_DS_SGND_COMPL,
  Modc_MotorCS_DS_SVBAT_OL_NML_COMPL
}Modc_MotorConnectStateDetectStep_t;

typedef struct
{
  Modc_MotorConnectStateDetectStep_t Modc_MotorCS_DetectStep;
  Modc_MotorConnectStateConf_t Modc_MotorCS_Conf;
  Modc_MotorConnectState_t Modc_MotorConnectState;
} Modc_OffStateOLSCDetect_t;

typedef uint8 Modc_ChanId_t;
typedef uint8 Modc_ChanDirec_t;
typedef uint8 Modc_HalfBridgeState_t;
/*____________________________ G L O B A L - D E F I N E S ________________________________________________*/
/*register address definiton*/
#define MODC_GENCTRL1_Addr                 (0x00U)
#define MODC_GENCTRL2_Addr                 (0x01U)
#define MODC_VDS1_Addr                     (0x02U)
#define MODC_VDS2_Addr                     (0x03U)
#define MODC_CCP_BLK1_Addr                 (0x04U)
#define MODC_CCP_BLK2_ACT_Addr             (0x05U)
#define MODC_CCP_BLK2_FW_Addr              (0x05U)
#define MODC_HBMODE_Addr                   (0x06U)
#define MODC_PWMSET_Addr                   (0x07U)
#define MODC_TPRECHG_Addr                  (0x08U)
#define MODC_HBIDIAG_Addr                  (0x09U)
#define MODC_ST_ICHG_Addr                  (0x0AU)
#define MODC_PWM_PCHG_INIT_Addr            (0x0AU)
#define MODC_PWM_ICHG_ACT_Addr             (0x0BU)
#define MODC_PWM_ICHG_FW_Addr              (0x0BU)
#define MODC_PWM_IDCHG_ACT_Addr            (0x0CU)
#define MODC_PWM_PDCHG_INIT_Addr           (0x0CU)
#define MODC_PWM_ICHGMAX_CCP_BLK3_ACT_Addr (0x0DU)
#define MODC_PWM_ICHGMAX_CCP_BLK3_FW_Addr  (0x0DU)
#define MODC_TDON_OFF1_Addr                (0x0EU)
#define MODC_TDON_OFF2_Addr                (0x0FU)
#define MODC_TDON_OFF3_Addr                (0x10U)
#define MODC_GENSTAT_Addr                  (0x11U)
#define MODC_DSOV_Addr                     (0x12U)
#define MODC_HBVOUT_PWMERR_Addr            (0x13U)
#define MODC_EFF_TDON_OFF1_Addr            (0x14U)
#define MODC_EFF_TDON_OFF2_Addr            (0x15U)
#define MODC_EFF_TDON_OFF3_Addr            (0x16U)
#define MODC_TRISE_FALL1_Addr              (0x17U)
#define MODC_TRISE_FALL2_Addr              (0x18U)
#define MODC_TRISE_FALL3_Addr              (0x19U)
#define MODC_DEVID_Addr                    (0x1FU)



/* Diagnosis result*/
#define MODC_DIAG_ERR_FREE           0x00U
#define MODC_DIAG_ERR_TSD            (uint16)((uint16)0x01U << (uint16)0U)
#if 0
#define MODC_DIAG_ERR_OC_HS1         (uint64)((uint64)0x01U << (uint64)1U)
#define MODC_DIAG_ERR_OC_LS1         (uint64)((uint64)0x01U << (uint64)2U)
#define MODC_DIAG_ERR_OC_HS2         (uint64)((uint64)0x01U << (uint64)3U)
#define MODC_DIAG_ERR_OC_LS2         (uint64)((uint64)0x01U << (uint64)4U)
#define MODC_DIAG_ERR_OC_HS3         (uint64)((uint64)0x01U << (uint64)5U)
#define MODC_DIAG_ERR_OC_LS3         (uint64)((uint64)0x01U << (uint64)6U)
#define MODC_DIAG_ERR_OC_HS4         (uint64)((uint64)0x01U << (uint64)7U)
#define MODC_DIAG_ERR_OC_LS4         (uint64)((uint64)0x01U << (uint64)8U)
#define MODC_DIAG_ERR_OC_HS5         (uint64)((uint64)0x01U << (uint64)9U)
#define MODC_DIAG_ERR_OC_LS5         (uint64)((uint64)0x01U << (uint64)10U)
#define MODC_DIAG_ERR_OC_HS6         (uint64)((uint64)0x01U << (uint64)11U)
#define MODC_DIAG_ERR_OC_LS6         (uint64)((uint64)0x01U << (uint64)12U)
#define MODC_DIAG_ERR_OC_HS7         (uint64)((uint64)0x01U << (uint64)13U)
#define MODC_DIAG_ERR_OC_LS7         (uint64)((uint64)0x01U << (uint64)14U)
#define MODC_DIAG_ERR_OC_HS8         (uint64)((uint64)0x01U << (uint64)15U)
#define MODC_DIAG_ERR_OC_LS8         (uint64)((uint64)0x01U << (uint64)16U)
#define MODC_DIAG_ERR_OC_HLS         (0x1EU)
#define MODC_DIAG_ERR_OC_LS          (0x14U)

//hanson
#define MODC_DIAG_ERR_FAULT_HS1      (uint64)((uint64)0x01U << (uint64)17U)
#define MODC_DIAG_ERR_FAULT_LS1      (uint64)((uint64)0x01U << (uint64)18U)
#define MODC_DIAG_ERR_FAULT_HS2      (uint64)((uint64)0x01U << (uint64)19U)
#define MODC_DIAG_ERR_FAULT_LS2      (uint64)((uint64)0x01U << (uint64)20U)
#define MODC_DIAG_ERR_FAULT_HS3      (uint64)((uint64)0x01U << (uint64)21U)
#define MODC_DIAG_ERR_FAULT_LS3      (uint64)((uint64)0x01U << (uint64)22U)
#define MODC_DIAG_ERR_FAULT_HS4      (uint64)((uint64)0x01U << (uint64)23U)
#define MODC_DIAG_ERR_FAULT_LS4      (uint64)((uint64)0x01U << (uint64)24U)
#define MODC_DIAG_ERR_FAULT_HS5      (uint64)((uint64)0x01U << (uint64)25U)
#define MODC_DIAG_ERR_FAULT_LS5      (uint64)((uint64)0x01U << (uint64)26U)
#define MODC_DIAG_ERR_FAULT_HS6      (uint64)((uint64)0x01U << (uint64)27U)
#define MODC_DIAG_ERR_FAULT_LS6      (uint64)((uint64)0x01U << (uint64)28U)
#define MODC_DIAG_ERR_FAULT_HS7      (uint64)((uint64)0x01U << (uint64)29U)
#define MODC_DIAG_ERR_FAULT_LS7      (uint64)((uint64)0x01U << (uint64)30U)
#define MODC_DIAG_ERR_FAULT_HS8      (uint64)((uint64)0x01U << (uint64)31U)
#define MODC_DIAG_ERR_FAULT_LS8      (uint64)((uint64)0x01U << (uint64)32U)


#define MODC_DIAG_ERR_WDT_FAULT      (uint64)((uint64)0x01U << (uint64)33U)
#define MODC_DIAG_ERR_OV_UV_FAULT    (uint64)((uint64)0x01U << (uint64)34U)
#define MODC_DIAG_ERR_VCP_UV_FAULT   (uint64)((uint64)0x01U << (uint64)35U)
#endif

#define Predc_HAS_ERR                 (0x80U)

/* FAULT Status Register */
#define MODC_SPI_OK                  (uint8)(0x01U << 7U)/*Indicates if a SPI communications fault has been detected.*/
#define MODC_POR                     (uint8)(0x01U << 6U)/*Indicates power-on-reset condition.*/
#define MODC_FAULT                   (uint8)(0x01U << 5U)/*Fault indicator. Mirrors nFAULT pin.*/
#define MODC_WARN                    (uint8)(0x01U << 4U)/*Warning indicator.*/
#define MODC_DS_GS                   (uint8)(0x01U << 3U)/*Logic OR of VDS and VGS fault indicators.*/
#define MODC_UV                      (uint8)(0x01U << 2U)/*Undervoltage indicator.*/
#define MODC_OV                      (uint8)(0x01U << 1U)/*Overvoltage indicator.*/
#define MODC_OT_WD_AGD               (uint8)(0x01U << 0U)/*Logic OR of OTW, OTSD, WD_FLT, IDIR_WARN, PCHR_WARN,PDCHR_WARN, and STC_WARN indicators.*/

/*DSOV Register TLE92108*/
#define MODC_LS8DSOV                 (uint16)(0x01U << 15U) /* */
#define MODC_HS8DSOV                 (uint16)(0x01U << 14U) /* */
#define MODC_LS7DSOV                 (uint16)(0x01U << 13U) /* */
#define MODC_HS7DSOV                 (uint16)(0x01U << 12U) /* */
#define MODC_LS6DSOV                 (uint16)(0x01U << 11U) /* */
#define MODC_HS6DSOV                 (uint16)(0x01U << 10U) /* */
#define MODC_LS5DSOV                 (uint16)(0x01U << 9U)  /* */
#define MODC_HS5DSOV                 (uint16)(0x01U << 8U)  /* */
#define MODC_LS4DSOV                 (uint16)(0x01U << 7U)  /* */
#define MODC_HS4DSOV                 (uint16)(0x01U << 6U)  /* */
#define MODC_LS3DSOV                 (uint16)(0x01U << 5U)  /* */
#define MODC_HS3DSOV                 (uint16)(0x01U << 4U)  /* */
#define MODC_LS2DSOV                 (uint16)(0x01U << 3U)  /* */
#define MODC_HS2DSOV                 (uint16)(0x01U << 2U)  /* */
#define MODC_LS1DSOV                 (uint16)(0x01U << 1U)  /* */
#define MODC_HS1DSOV                 (uint16)(0x01U << 0U)  /* */

/*VGS_STAT1 Register*/
#define MODC_H1_VGS                  (uint8)(0x01U << 7U)/*Indicates VGS gate fault on the high-side 1 MOSFET.*/
#define MODC_L1_VGS                  (uint8)(0x01U << 6U)/*Indicates VGS gate fault on the low-side 1 MOSFET.*/
#define MODC_H2_VGS                  (uint8)(0x01U << 5U)/*Indicates VGS gate fault on the high-side 2 MOSFET.*/
#define MODC_L2_VGS                  (uint8)(0x01U << 4U)/*Indicates VGS gate fault on the low-side 2 MOSFET.*/
#define MODC_H3_VGS                  (uint8)(0x01U << 3U)/*Indicates VGS gate fault on the high-side 3 MOSFET.*/
#define MODC_L3_VGS                  (uint8)(0x01U << 2U)/*Indicates VGS gate fault on the low-side 3 MOSFET.*/
#define MODC_H4_VGS                  (uint8)(0x01U << 1U)/*Indicates VGS gate fault on the high-side 4 MOSFET.*/
#define MODC_L4_VGS                  (uint8)(0x01U << 0U)/*Indicates VGS gate fault on the low-side 4 MOSFET.*/

/*VGS_STAT2 Register*/
#define MODC_H5_VGS                  (uint8)(0x01U << 7U)/*Indicates VGS gate fault on the high-side 5 MOSFET.*/
#define MODC_L5_VGS                  (uint8)(0x01U << 6U)/*Indicates VGS gate fault on the low-side 5 MOSFET.*/
#define MODC_H6_VGS                  (uint8)(0x01U << 5U)/*Indicates VGS gate fault on the high-side 6 MOSFET.*/
#define MODC_L6_VGS                  (uint8)(0x01U << 4U)/*Indicates VGS gate fault on the low-side 6 MOSFET.*/
#define MODC_H7_VGS                  (uint8)(0x01U << 3U)/*Indicates VGS gate fault on the high-side 7 MOSFET.*/
#define MODC_L7_VGS                  (uint8)(0x01U << 2U)/*Indicates VGS gate fault on the low-side 7 MOSFET.*/
#define MODC_H8_VGS                  (uint8)(0x01U << 1U)/*Indicates VGS gate fault on the high-side 8 MOSFET.*/
#define MODC_L8_VGS                  (uint8)(0x01U << 0U)/*Indicates VGS gate fault on the low-side 8 MOSFET.*/

/* GENSTAT  Register TLE92108 */
#define MODC_GENSTAT_PASS_VDSOV (uint16)(0x01U << 15U)
#define MODC_GENSTAT_WDMON_MASK (uint16)(0x3U << 13U)
#define MODC_GENSTAT_PWM3STAT (uint16)(0x01U << 12U)
#define MODC_GENSTAT_PWM2STAT (uint16)(0x01U << 11U)
#define MODC_GENSTAT_PWM1STAT (uint16)(0x01U << 10U)
#define MODC_GENSTAT_TDREG3 (uint16)(0x01U << 9U)
#define MODC_GENSTAT_TDREG2 (uint16)(0x01U << 8U)
#define MODC_GENSTAT_TDREG1 (uint16)(0x01U << 7U)
#define MODC_GENSTAT_TSD (uint16)(0x01U << 6U)
#define MODC_GENSTAT_TW (uint16)(0x01U << 5U)
#define MODC_GENSTAT_OC2 (uint16)(0x01U << 4U)
#define MODC_GENSTAT_OC1 (uint16)(0x01U << 3U)
#define MODC_GENSTAT_VSOV (uint16)(0x01U << 2U)
#define MODC_GENSTAT_VSUV (uint16)(0x01U << 1U)
#define MODC_GENSTAT_CPUV (uint16)(0x01U << 0U)/* Charge Pump Undervoltage */

/*main control register*/
#define MODC_LOCK                    (uint16)((uint16)0x06U << (uint16)3U)
#define MODC_UNLOCK                  (uint16)((uint16)0x03U << (uint16)3U)
#define MODC_IN1_PH                  (uint16)((uint16)0x01U << (uint16)2U)
#define MODC_IN2_EN                  (uint16)((uint16)0x01U << (uint16)1U)
#define MODC_CLR_FLT                 (uint16)((uint16)0x01U << (uint16)0U)

/*dead time definition*/
#define MODC_DT_120_NS               (0x0U << 6U)
#define MODC_DT_240_NS               (0x1U << 6U)
#define MODC_DT_480_NS               (0x2U << 6U)
#define MODC_DT_960_NS               (0x3U << 6U)

/*source current and sink current definiton*/
#define MODC_CURRENT_1OmA            (0x0U << 0U)
#define MODC_CURRENT_2OmA            (0x1U << 0U)
#define MODC_CURRENT_5OmA            (0x2U << 0U)
#define MODC_CURRENT_7OmA            (0x3U << 0U)
#define MODC_CURRENT_105mA           (0x4U << 0U)
#define MODC_CURRENT_155mA           (0x5U << 0U)
#define MODC_CURRENT_21OmA           (0x6U << 0U)
#define MODC_CURRENT_26OmA           (0x7U << 0U)

/*SO ouput limited*/
#define MODC_SO_LIM                  (0x01 << 7U) /*limited to 3.6V*/

/*VDS definiton*/
#define MODC_VDS_60mV                (0x0U << 4U)
#define MODC_VDS_145mV               (0x1U << 4U)
#define MODC_VDS_170mV               (0x2U << 4U)
#define MODC_VDS_200mV               (0x3U << 4U)
#define MODC_VDS_120mV               (0x4U << 4U)
#define MODC_VDS_240mV               (0x5U << 4U)
#define MODC_VDS_480mV               (0x6U << 4U)
#define MODC_VDS_960mV               (0x7U << 4U)

/*disable VDS monitor*/
#define MODC_DIS_H2_VDS              (0x01U << 3U)
#define MODC_DIS_L2_VDS              (0x01U << 2U)
#define MODC_DIS_H1_VDS              (0x01U << 1U)
#define MODC_DIS_L1_VDS              (0x01U << 0U)

/*Off time for PWM current chopping definiton*/
#define MODC_OffTime_25us            (0x0U << 6U)
#define MODC_OffTime_50us            (0x1U << 6U)
#define MODC_OffTime_100us           (0x2U << 6U)
#define MODC_OffTime_200us           (0x3U << 6U)

/*chop disable*/
#define MODC_CHOP_DISABLE            (0x01 << 5U)

/*Scale factor for the VREF input definiton*/
#define MODC_SCALE_100               (0x0U << 3U)
#define MODC_SCALE_75                (0x1U << 3U)
#define MODC_SCALE_50                (0x2U << 3U)
#define MODC_SCALE_25                (0x3U << 3U)

/*shunt enable*/
#define MODC_SH_EN                   (0x01 << 2U)

/*Shunt amplifier gain definiton*/
#define MODC_GAIN_10                 (0x0U << 0U)
#define MODC_GAIN_19_8               (0x1U << 0U)
#define MODC_GAIN_39_4               (0x2U << 0U)
#define MODC_GAIN_78                 (0x3U << 0U)


/*____________________________ G L O B A L - T Y P E S ____________________________________________________*/

/*____________________________ G L O B A L - D A T A ______________________________________________________*/

/*____________________________ G L O B A L - M A C R O S __________________________________________________*/
#define MODC_OUTPUT_LOW 0U
#define MODC_OUTPUT_HIGH 1U
#define MODC_OUTPUT_HIGH_IMPEDANCE 2U

#define MODC_6_5V                    (uint8)(65)

#define MODC_INVALID_CHANNEL_ID        0xFF
#define MODC_INVALID_CHANNED_DIRECT    0xFF

#define MODC_SET_REG_VAL(a,b,c)      a &= ~(b); \
                                     a |= (c)
/*____________________________ G L O B A L - F U N C T I O N S - P R O T O T Y P E S ______________________*/

extern void  MODC_Init(void);
#if 0
extern void  MODC_Sleep(void);
extern void  MODC_SwitchOnOff(uint8 Channel, boolean Status);
#endif

extern void  MODC_Runnable(void);
extern uint8 MODC_GetRegisterValue(uint8 regID);
extern boolean MODC_ChannelSetupReuqest(MODC_HalfBridgeNum_t Channel, MODC_GateDriverState_t GateDriverState);

extern uint64 MODC_GetInstantError(void);
extern boolean MODC_GetCPUVError(int ChipNum);
extern boolean MODC_GetVSUVError(int ChipNum);
extern boolean MODC_GetVSOVError(int ChipNum);
extern uint8 MODC_GetTSDError(void);
extern uint8 MODC_GetCS2Error(void);
extern uint8 MODC_GetCS1Error(void);




extern uint16 MODC_GetOCError(int ChipNum);
extern boolean MODC_GetOCErrorFlag(int ChipNum);

extern uint16 MODC_GetRegisterValueBank0(uint8 ChipNum, uint8 regID);
extern uint16 MODC_GetRegisterValueBank1(uint8 ChipNum, uint8 regID);
extern void MODC_ResetDevice(void);
extern boolean MODC_VerifyCommunication(void);
extern boolean MODC_GetRunningState(void);
extern void MODC_TxCompleteCallback(void);
/* extern void MODC_SetOutput(uint8 Channel, uint8 Status); */
extern Modc_MotorConnectState_t Modc_GetMotorConnection(MODC_ChipNum_t ChipNum,uint8 MotorNum);
extern boolean Modc_GetOffStateCheckIsCompl(void);
#endif

/*______ E N D _____ (predc.h) ____________________________________________*/

