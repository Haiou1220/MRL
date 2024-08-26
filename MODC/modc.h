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

/*_______________________________ L O C A L - T Y P E S ___________________________________________________*/
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
  MODC_HB_ONE,
  MODC_HB_TWO,
  MODC_HB_THREE,
  MODC_HB_FOUR,
  MODC_HB_FIVE,
  MODC_HB_SIX,
  MODC_HB_SEVEN,
  MODC_HB_EIGHT,
  MODC_HB_MAX
}MODC_HalfBridgeNum_t;

typedef enum
{
  MODC_GDS_HIGH_IMPEDANCE,
  MODC_GDS_DRIVE_LOW_SIDE,
  MODC_GDS_DRIVE_HIGH_SIDE,
  MODC_GDS_PWM_HS_ACITVE_FW,
  MODC_GDS_PWM_LS_ACITVE_FW,
  MODC_GDS_PWM_HS_PASSIVE_FW,
  MODC_GDS_PWM_LS_PASSIVE_FW,
  MODC_GDS_MAX
}MODC_GateDriverState_t;

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

typedef uint8 Modc_ChanId_t;
typedef uint8 Modc_ChanDirec_t;
typedef uint8 Modc_HalfBridgeState_t;
/*____________________________ G L O B A L - D E F I N E S ________________________________________________*/
/*register address definiton*/
#define MODC_IC_STAT1Addr            (0x00U)
#define MODC_VDS_STAT1Addr           (0x01U)
#define MODC_VDS_STAT2Addr           (0x02U)
#define MODC_VGS_STAT1Addr           (0x03U)
#define MODC_VGS_STAT2Addr           (0x04U)
#define MODC_IC_STAT2Addr            (0x05U)
#define MODC_IC_STAT3Addr            (0x06U)
#define MODC_IC_CTRL1Addr            (0x07U)
#define MODC_IC_CTRL2Addr            (0x08U)
#define MODC_BRG_CTRL1Addr           (0x09U)
#define MODC_BRG_CTRL2Addr           (0x0AU)
#define MODC_PWM_CTRL1Addr           (0x0BU)
#define MODC_PWM_CTRL2Addr           (0x0CU)
#define MODC_PWM_CTRL3Addr           (0x0DU)
#define MODC_PWM_CTRL4Addr           (0x0EU)
#define MODC_IDRV_CTRL1Addr          (0x0FU)
#define MODC_IDRV_CTRL2Addr          (0x10U)
#define MODC_IDRV_CTRL3Addr          (0x11U)
#define MODC_IDRV_CTRL4Addr          (0x12U)
#define MODC_IDRV_CTRL5Addr          (0x13U)
#define MODC_IDRV_CTRL6Addr          (0x14U)
#define MODC_IDRV_CTRL7Addr          (0x15U)
#define MODC_IDRV_CTRL8Addr          (0x16U)
#define MODC_IDRV_CTRL9Addr          (0x17U)
#define MODC_DRV_CTRL1Addr           (0x18U)
#define MODC_DRV_CTRL2Addr           (0x19U)
#define MODC_DRV_CTRL3Addr           (0x1AU)
#define MODC_DRV_CTRL4Addr           (0x1BU)
#define MODC_DRV_CTRL5Addr           (0x1CU)
#define MODC_DRV_CTRL6Addr           (0x1DU)
#define MODC_DRV_CTRL7Addr           (0x1EU)
#define MODC_VDS_CTRL1Addr           (0x1FU)
#define MODC_VDS_CTRL2Addr           (0x20U)
#define MODC_VDS_CTRL3Addr           (0x21U)
#define MODC_VDS_CTRL4Addr           (0x22U)
#define MODC_OLSC_CTRL1Addr          (0x23U)
#define MODC_OLSC_CTRL2Addr          (0x24U)
#define MODC_UVOV_CTRLAddr           (0x25U)
#define MODC_CSA_CTRL1Addr           (0x26U)
#define MODC_CSA_CTRL2Addr           (0x27U)
#define MODC_CSA_CTRL3Addr           (0x28U)
#define MODC_RSVD_CTRLAddr           (0x29U)
#define MODC_AGD_CTRL1Addr           (0x2AU)
#define MODC_PDR_CTRL1Addr           (0x2BU)
#define MODC_PDR_CTRL2Addr           (0x2CU)
#define MODC_PDR_CTRL3Addr           (0x2DU)
#define MODC_PDR_CTRL4Addr           (0x2EU)
#define MODC_PDR_CTRL5Addr           (0x2FU)
#define MODC_PDR_CTRL6Addr           (0x30U)
#define MODC_PDR_CTRL7Addr           (0x31U)
#define MODC_PDR_CTRL8Addr           (0x32U)
#define MODC_PDR_CTRL9Addr           (0x33U)
#define MODC_PDR_CTRL10Addr          (0x34U)
#define MODC_STC_CTRL1Addr           (0x35U)
#define MODC_STC_CTRL2Addr           (0x36U)
#define MODC_STC_CTRL3Addr           (0x37U)
#define MODC_STC_CTRL4Addr           (0x38U)
#define MODC_DCC_CTRL1Addr           (0x39U)
#define MODC_PST_CTRL1Addr           (0x3AU)
#define MODC_PST_CTRL2Addr           (0x3BU)
#define MODC_SGD_STAT1Addr           (0x3CU)
#define MODC_SGD_STAT2Addr           (0x3DU)
#define MODC_SGD_STAT3Addr           (0x3EU)
#define MODC_TotalReg                (0x3FU)

/* Diagnosis result*/
#define MODC_DIAG_ERR_FREE           0x00U

#define MODC_DIAG_ERR_TSD            (uint64)((uint64)0x01U << (uint64)0U)

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

/*VDS_STAT1 Register*/
#define MODC_H1_VDS                  (uint8)(0x01U << 7U)/*Indicates VDS overcurrent fault on the high-side 1 MOSFET.*/
#define MODC_L1_VDS                  (uint8)(0x01U << 6U)/*Indicates VDS overcurrent fault on the low-side 1 MOSFET.*/
#define MODC_H2_VDS                  (uint8)(0x01U << 5U)/*Indicates VDS overcurrent fault on the high-side 2 MOSFET.*/
#define MODC_L2_VDS                  (uint8)(0x01U << 4U)/*Indicates VDS overcurrent fault on the low-side 2 MOSFET.*/
#define MODC_H3_VDS                  (uint8)(0x01U << 3U)/*Indicates VDS overcurrent fault on the high-side 3 MOSFET.*/
#define MODC_L3_VDS                  (uint8)(0x01U << 2U)/*Indicates VDS overcurrent fault on the low-side 3 MOSFET.*/
#define MODC_H4_VDS                  (uint8)(0x01U << 1U)/*Indicates VDS overcurrent fault on the high-side 4 MOSFET.*/
#define MODC_L4_VDS                  (uint8)(0x01U << 0U)/*Indicates VDS overcurrent fault on the low-side 4 MOSFET.*/

/*VDS_STAT2 Register*/
#define MODC_H5_VDS                  (uint8)(0x01U << 7U)/*Indicates VDS overcurrent fault on the high-side 5 MOSFET.*/
#define MODC_L5_VDS                  (uint8)(0x01U << 6U)/*Indicates VDS overcurrent fault on the low-side 5 MOSFET.*/
#define MODC_H6_VDS                  (uint8)(0x01U << 5U)/*Indicates VDS overcurrent fault on the high-side 6 MOSFET.*/
#define MODC_L6_VDS                  (uint8)(0x01U << 4U)/*Indicates VDS overcurrent fault on the low-side 6 MOSFET.*/
#define MODC_H7_VDS                  (uint8)(0x01U << 3U)/*Indicates VDS overcurrent fault on the high-side 7 MOSFET.*/
#define MODC_L7_VDS                  (uint8)(0x01U << 2U)/*Indicates VDS overcurrent fault on the low-side 7 MOSFET.*/
#define MODC_H8_VDS                  (uint8)(0x01U << 1U)/*Indicates VDS overcurrent fault on the high-side 8 MOSFET.*/
#define MODC_L8_VDS                  (uint8)(0x01U << 0U)/*Indicates VDS overcurrent fault on the low-side 8 MOSFET.*/

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

/* IC_STAT2 Register */
#define MODC_PVDD_UV                 (uint8)(0x01U << 7U)/*Indicates undervoltage fault on PVDD pin.*/
#define MODC_PVDD_OV                 (uint8)(0x01U << 6U)/*Indicates overvoltage fault on PVDD pin.*/
#define MODC_VCP_UV                  (uint8)(0x01U << 5U)/*Indicates undervoltage fault on VCP pin.*/
#define MODC_OTW                     (uint8)(0x01U << 4U)/*Indicates overtemperature warning.*/
#define MODC_OTSD                    (uint8)(0x01U << 3U)/*Indicates overtemperature shutdown.*/
#define MODC_WD_FLT                  (uint8)(0x01U << 2U)/*Indicated watchdog timer fault.*/
#define MODC_SCLK_FLT                (uint8)(0x01U << 1U)/*Indicates SPI clock (frame) fault when the number of SCLK pulses in
                                                         a transaction frame are not equal to 16. Not reported on FAULT or
                                                         nFAULT pin.*/

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
/*____________________________ G L O B A L - F U N C T I O N S - P R O T O T Y P E S ______________________*/

extern void  MODC_Init(void);
#if 0
extern void  MODC_Sleep(void);
extern void  MODC_SwitchOnOff(uint8 Channel, boolean Status);
#endif

extern void  MODC_Runnable(void);
extern boolean MODC_DiagOverCurrernt(void);
extern uint64 MODC_GetInstantError(void);
extern uint8 MODC_GetRegisterValue(uint8 regID);
extern void MODC_SetOutput(uint8 Channel, uint8 Status);
extern void MODC_TxCompleteCallback(void);
extern boolean MODC_GetRunningState(void);
extern boolean MODC_VerifyCommunication(void);
extern boolean MODC_ChannelSetupReuqest(MODC_HalfBridgeNum_t Channel, MODC_GateDriverState_t GateDriverState);
extern uint64 MODC_GetErrorPresent(void);
extern void MODC_ResetDevice(void);
extern uint8 MODC_GetMotorConnection(MODC_HalfBridgeNum_t Modc_Channel);
#endif

/*______ E N D _____ (predc.h) ____________________________________________*/

