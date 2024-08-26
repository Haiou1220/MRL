/***********************************************************************************************************/
/**
* \file       modc_config.h
* \brief      H/Half-bridge Motor driver Controller Module configuration file header
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

#ifndef MODC_CONFIG_H
#define MODC_CONFIG_H

/*____________________________ I N C L U D E - F I L E S __________________________________________________*/
#include "Platform_Types.h"
#include "spic.h"
#include "modc_priv.h"

/*____________________________ G L O B A L - D E F I N E S ________________________________________________*/
/* Specify SPI channel */
#define Modc_SPI_CHANNEL               1/*SpiConf_SpiChannel_SpiChannel_Modc*/
#define Modc_SPI_2_BYTES              (2U)  /*bytes per frame*/

#define Modc_SLAVE_DEVICE_TLE92108    (0U)
#define Modc_SLAVE_DEVICE_TLE92104    (1U)
#define Modc_SLAVE_DEVICE_NUM         (2U)  /*number of chips*/
#define Modc_SPI_CHANNEL_NUM          (2U)  /*number of SPI Channel*/

#define Modc_TASK_PERIOD              (5U)  /* 5ms */
#define Modc_DELAY_300MS              (300U)
#define Modc_DELAY_200MS              (200U)
#define Modc_DELAY_100MS              (100U)
#define Modc_DELAY_50MS               (50U)
#define Modc_DELAY_30MS               (30U)
#define Modc_DELAY_10MS               (10U)
#define Modc_DELAY_15MS               (15U)



#define Modc_ERR_CNT_PLUS_STEP        (5U)
#define Modc_ERR_CNT_MINUS_STEP       (2U)
#define Modc_ERR_CNT_RECOVERY_STEP    (1U)
#define Modc_ERR_CNT_ERR_TSD_THRES    (50U)
#define Modc_ERR_CNT_ERR_OC_THRES     (100U)/* 400  */
#define Modc_ERR_CNT_ERR_Fault_THRES  (50U)
#define Modc_ERR_CNT_OC_PROT_TIME     (1000U)/* 1000U */
#define Modc_ERR_CNT_TSD_PROT_TIME    (10000U)
#define Modc_ERR_CNT_Fault_PROT_TIME  (1000U)

#define Modc_OLSC_DETECT_TIME         ((uint8)(Modc_DELAY_50MS / Modc_TASK_PERIOD))
#define Modc_WATCH_DOG_RESET_TIME     ((uint8)(Modc_DELAY_30MS / Modc_TASK_PERIOD))
#define Modc_DEVICE_RESET_TIME        ((uint8)(Modc_DELAY_10MS / Modc_TASK_PERIOD))
#define Modc_DEVICE_WAKEUP_TIME       ((uint8)(Modc_DELAY_15MS / Modc_TASK_PERIOD))

#define MODC_DIAG_READ_STATUS_REG_PERIOD     ((uint16)(Modc_DELAY_300MS / Modc_TASK_PERIOD))
#define MODC_OFFSTATE_DIAG_READ_STATUS_REG_PERIOD     ((uint16)(Modc_DELAY_30MS / Modc_TASK_PERIOD))

#define Modc_MAX_NUM_OF_CFG_RETRY     (3u)
#define Modc_CHECK_SPI_RESULT_TIMES   (20u)

#define MODC_REG_IC_CTRL2            (MODC_DIS_SSC_EN | MODC_DRVOFF | MODC_CP_MODE_AUTO | MODC_WD_EN | \
                                      MODC_WD_FLT_M_REPORT_DIS_GATE | MODC_WD_WIN_10_100MS)
#define MODC_REG_PWM_CTRL1           (MODC_HB1_PWM_IN1 | MODC_HB2_PWM_IN1 | MODC_HB3_PWM_IN1 | MODC_HB4_PWM_IN2)
#define MODC_REG_PWM_CTRL2           (MODC_HB5_PWM_IN2 | MODC_HB6_PWM_IN3 | MODC_HB7_PWM_IN3 | MODC_HB8_PWM_IN1)
#define MODC_REG_DRV_CTRL4           (MODC_HB12_TDEAD_2uS | MODC_HB34_TDEAD_2uS | MODC_HB56_TDEAD_2uS | MODC_HB78_TDEAD_2uS)
#define MODC_REG_VDS_CTRL1           (MODC_VDS_H_1000mV | MODC_VDS_L_400mV)
#define MODC_REG_VDS_CTRL2           (MODC_VDS_H_400mV | MODC_VDS_L_140mV)
#define MODC_REG_VDS_CTRL3           (MODC_VDS_H_140mV | MODC_VDS_L_140mV)
#define MODC_REG_VDS_CTRL4           (MODC_VDS_H_140mV | MODC_VDS_L_1000mV)

#define MODC_REG_VDS_CTRL1_OLSC      (MODC_VDS_H_2000mV | MODC_VDS_L_2000mV)
#define MODC_REG_VDS_CTRL2_OLSC      (MODC_VDS_H_2000mV | MODC_VDS_L_2000mV)
#define MODC_REG_VDS_CTRL3_OLSC      (MODC_VDS_H_2000mV | MODC_VDS_L_2000mV)
#define MODC_REG_VDS_CTRL4_OLSC      (MODC_VDS_H_2000mV | MODC_VDS_L_2000mV)

#define MODC_REG_DRV_CTRL1           (MODC_VGS_MODE_LATCH | MODC_VGS_IND_ENABLE | MODC_VGS_LVL_1_4V | MODC_VGS_HS_ENABLE | \
                                      MODC_VDS_MODE_LATCH | MODC_VDS_IND_ENABLE)

#define MODC_REG_UVOV_CTRL           (MODC_PVDD_UV_MODE_LATCH | MODC_PVDD_OV_MODE_LATCH | MODC_PVDD_OV_DG_4uS | MODC_PVDD_UV_LVL_28_5V | \
                                      MODC_VCP_UV_MODE_LATCH | MODC_VCP_UV_LVL_6_25V)
#define MODC_REG_CSA_CTRL1           (MODC_CSA_DIV_1_AREF_2 | MODC_CSA_GAIN_1_10VV | MODC_CSA_DIV_2_AREF_2 | MODC_CSA_GAIN_2_10VV)

#define MODC_RWL_NORMAL_DETECT       (MODC_VDS2_MASK | MODC_VDS3_MASK)
#define MODC_HANDLE_X_NORMAL_DETECT  (MODC_VDS4_MASK)
#define MODC_HANDLE_Y_NORMAL_DETECT  (MODC_VDS5_MASK)
#define MODC_CINCH_NORMAL_DETECT     (MODC_VDS6_MASK | MODC_VDS7_MASK)

/*#define MODC_WATCH_DOG_ENABLE*/
/*____________________________ G L O B A L - T Y P E S ____________________________________________________*/

/*____________________________ G L O B A L - D A T A ______________________________________________________*/


/*____________________________ G L O B A L - M A C R O S __________________________________________________*/

/*____________________________ G L O B A L - F U N C T I O N S - P R O T O T Y P E S ______________________*/
/*----------------------------------------------------------------------------*/
/* Name : Modc_WakeUpDevice                                                   */
/* Role : wake or sleep the device                                            */
/* Interface :                                                                */
/*  - IN  :  TRUE :WakeUp  FALSE:Sleep                                        */
/*  - OUT :                                                                   */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern void Modc_WakeUpDevice(uint8 state);

/*----------------------------------------------------------------------------*/
/* Name : Modc_GetDeviceWakeUpState                                           */
/* Role :                                                                     */
/* Interface :                                                                */
/*  - IN  :                                                                   */
/*  - OUT :  TRUE :WakeUp  FALSE:Sleep                                        */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern uint8 Modc_GetDeviceWakeUpState(void);

/*----------------------------------------------------------------------------*/
/* Name : Modc_SpiDataTransmitStart                                           */
/* Role : select the chip                                                     */
/* Interface :                                                                */
/*  - IN  :                                                                   */
/*  - OUT :  TRUE :WakeUp  FALSE:Sleep                                        */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern void Modc_SpiDataTransmitStart(void);

/*----------------------------------------------------------------------------*/
/* Name : Modc_SpiDataTransmitStop                                           */
/* Role : stop to transmit SPI data                                           */
/* Interface :                                                                */
/*  - IN  :                                                                   */
/*  - OUT :  TRUE :WakeUp  FALSE:Sleep                                        */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern void Modc_SpiDataTransmitStop(void);

/*----------------------------------------------------------------------------*/
/* Name : Modc_SpiGetSequenceResult                                           */
/* Role : Get Sequence Result                                                 */
/* Interface :                                                                */
/*  - IN  :                                                                   */
/*  - OUT : Transmit stste                                                    */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/*                                                      */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern Spi_SeqResultType Modc_SpiGetSequenceResult(uint8 Modc_SpiChannelSequence);

/*----------------------------------------------------------------------------*/
/* Name : Modc_SpiTransmitFrame                                               */
/* Role : Transmit SPI data                                                   */
/* Interface :                                                                */
/*  - IN  :                                                                   */
/*  - OUT : Transmit stste                                                    */
/* Pre-condition : none                                                       */
/* Constraints : none                                                         */
/* Behaviour :                                                                */
/* DO                                                                         */
/*                                                      */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
extern Std_ReturnType Modc_SpiTransmitFrame(uint8 Modc_SpiConf_SpiChannel, SPIC_Reqtype_t* req);
#endif

/*______ E N D _____ (predc_config.h) ____________________________________________*/

