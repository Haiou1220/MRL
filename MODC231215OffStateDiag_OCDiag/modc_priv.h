/***********************************************************************************************************/
/**
* \file       modc.c
* \brief      private definiton for chip DRV8718S-Q1(TI)
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

#ifndef MODC_PRIV_H
#define MODC_PRIV_H


/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "Platform_Types.h"
#include "modc_config.h"
/* _____ G L O B A L - D E F I N E ___________________________________________*/
/*operation cmd definiton*/
#define MODC_WRITE                (1U)
#define MODC_READ                 (0U)
#define MODC_CLEAR                (1U)

#define MODC_MSK_1bit             0x0001U
#define MODC_MSK_2bit             0x0003U
#define MODC_MSK_3bit             0x0007U
#define MODC_MSK_4bit             0x000FU
#define MODC_MSK_5bit             0x001FU
#define MODC_MSK_6bit             0x003FU
#define MODC_MSK_7bit             0x007FU
#define MODC_MSK_8bit             0x00FFU

/*General control register 1*/
/*Register banking*/
#define MODC_REG_BANK_MSK         (MODC_MSK_1bit << 9U)
#define MODC_REG_BANK0            (0x0000U << 9U)
#define MODC_REG_BANK1            (0x0001U << 9U)

/*Overcurrent shutdown Enable*/
#define MODC_OCEN_MSK             (MODC_MSK_1bit << 2U)
#define MODC_OCEN_DIS             (0x0000U << 2U)
#define MODC_OCEN_EN              (0x0001U << 2U)

#define MODC_WTDRIG               (0x0001U << 0U)

/*General control register 2*/
#define MODC_BD_PASS_MSK          (MODC_MSK_1bit << 14U)
#define MODC_BD_PASS              (0x0001U << 14U)

#define MODC_AGC_MSK              (MODC_MSK_2bit << 11U)
#define MODC_AGC_DISABLE          (0x0000U << 11U)
#define MODC_AGC_PRE_DISCH_EN     (0x0001U << 11U)
#define MODC_AGC_ENABLE           (0x0002U << 11U)
#define MODC_AGC_RESERVED         (0x0003U << 11U)

#define MODC_GC2_OCTH2_00B   (0X00U << 2U)
#define MODC_GC2_OCTH2_01B   (0X01U << 2U)
#define MODC_GC2_OCTH2_10B   (0X02U << 2U)
#define MODC_GC2_OCTH2_11B   (0X03U << 2U)
#define MODC_GC2_OCTH1_00B   (0X00U << 0U)
#define MODC_GC2_OCTH1_01B   (0X01U << 0U)
#define MODC_GC2_OCTH1_10B   (0X02U << 0U)
#define MODC_GC2_OCTH1_11B   (0X03U << 0U)
#define MODC_GC2_OCTH2_MASK   (MODC_MSK_2bit << 2U)
#define MODC_GC2_OCTH1_MASK   (MODC_MSK_2bit << 0U)

/*Filter time of drain-source voltage monitoring*/
#define MODC_TFVDS_MSK            (MODC_MSK_2bit << 4U)
#define MODC_TFVDS_0_5uS          (0x0000U << 4U)
#define MODC_TFVDS_1uS            (0x0001U << 4U)
#define MODC_TFVDS_2uS            (0x0002U << 4U)
#define MODC_TFVDS_3uS            (0x0003U << 4U)

/* CCP_BLK2_ACT register */
/*FCross-current protection - tCCP2 Active*/
#define MODC_TCCP2_ACT_MSK        (MODC_MSK_3bit << 6U)
#define MODC_TCCP2_ACT_375nS      (0x0000U << 6U)
#define MODC_TCCP2_ACT_625nS      (0x0001U << 6U)
#define MODC_TCCP2_ACT_1uS        (0x0002U << 6U)
#define MODC_TCCP2_ACT_1_5uS      (0x0003U << 6U)
#define MODC_TCCP2_ACT_2uS        (0x0004U << 6U)
#define MODC_TCCP2_ACT_3uS        (0x0005U << 6U)
#define MODC_TCCP2_ACT_4uS        (0x0006U << 6U)
#define MODC_TCCP2_ACT_16uS       (0x0007U << 6U)

/*Blank time - tBLANK2 Active*/
#define MODC_TBLANK2_ACT_MSK      (MODC_MSK_3bit << 9U)
#define MODC_TBLANK2_ACT_625nS    (0x0000U << 9U)
#define MODC_TBLANK2_ACT_1uS      (0x0001U << 9U)
#define MODC_TBLANK2_ACT_1_25uS   (0x0002U << 9U)
#define MODC_TBLANK2_ACT_1_5uS    (0x0003U << 9U)
#define MODC_TBLANK2_ACT_2uS      (0x0004U << 9U)
#define MODC_TBLANK2_ACT_3uS      (0x0005U << 9U)
#define MODC_TBLANK2_ACT_4uS      (0x0006U << 9U)
#define MODC_TBLANK2_ACT_16uS     (0x0007U << 9U)

/* CCP_BLK1 register */
/*Cross-current protection and blank times applied to HB8*/
#define MODC_HB8CCPBLK_MSK             (MODC_MSK_2bit << 14U)
#define MODC_HB8CCPBLK_CCP1_BLANK1     (0x0000U << 14U)
#define MODC_HB8CCPBLK_CCP2_BLANK2     (0x0001U << 14U)
#define MODC_HB8CCPBLK_CCP3_BLANK3     (0x0002U << 14U)
#define MODC_HB8CCPBLK_CCP4_BLANK4     (0x0003U << 14U)

/*Cross-current protection and blank times applied to HB7*/
#define MODC_HB7CCPBLK_MSK             (MODC_MSK_2bit << 12U)
#define MODC_HB7CCPBLK_CCP1_BLANK1     (0x0000U << 12U)
#define MODC_HB7CCPBLK_CCP2_BLANK2     (0x0001U << 12U)
#define MODC_HB7CCPBLK_CCP3_BLANK3     (0x0002U << 12U)
#define MODC_HB7CCPBLK_CCP4_BLANK4     (0x0003U << 12U)

/*Cross-current protection and blank times applied to HB6*/
#define MODC_HB6CCPBLK_MSK             (MODC_MSK_2bit << 10U)
#define MODC_HB6CCPBLK_CCP1_BLANK1     (0x0000U << 10U)
#define MODC_HB6CCPBLK_CCP2_BLANK2     (0x0001U << 10U)
#define MODC_HB6CCPBLK_CCP3_BLANK3     (0x0002U << 10U)
#define MODC_HB6CCPBLK_CCP4_BLANK4     (0x0003U << 10U)

/*Half-bridge diagnostic current control*/
#define MODC_CSA2L                (0x0001U << 15U)
#define MODC_CSA1L                (0x0001U << 14U)

/*Active PWM charge current control register*/
#define MODC_ICHG1_MSK            (MODC_MSK_5bit << 0U)
#define MODC_ICHG1_52_3mA         (0x0014U << 0U)
#define MODC_ICHG2_MSK            (MODC_MSK_5bit << 5U)
#define MODC_ICHG2_52_3mA         (0x0014U << 5U)
#define MODC_ICHG3_MSK            (MODC_MSK_5bit << 10U)
#define MODC_ICHG3_52_3mA         (0x0014U << 10U)

/*Active PWM discharge current control register*/
#define MODC_IDCHG1_MSK            (MODC_MSK_5bit << 0U)
#define MODC_IDCHG1_46_8mA         (0x0012U << 0U)
#define MODC_IDCHG2_MSK            (MODC_MSK_5bit << 5U)
#define MODC_IDCHG2_46_8mA         (0x0012U << 5U)
#define MODC_IDCHG3_MSK            (MODC_MSK_5bit << 10U)
#define MODC_IDCHG3_46_8mA         (0x0012U << 10U)

/*PWM max. drive current control register*/
#define MODC_ICHGMAX1_MSK          (MODC_MSK_2bit << 0U)
#define MODC_ICHGMAX1_77_79mA      (0x0002U << 0U)
#define MODC_ICHGMAX2_MSK          (MODC_MSK_2bit << 2U)
#define MODC_ICHGMAX2_77_79mA      (0x0002U << 2U)
#define MODC_ICHGMAX3_MSK          (MODC_MSK_2bit << 4U)
#define MODC_ICHGMAX3_77_79mA      (0x0002U << 4U)

/*PWM charge/discharge currents of FW MOSFETs register*/
#define MODC_ICHG1_FW_MSK          (MODC_MSK_5bit << 0U)
#define MODC_ICHG1_FW_50_8mA       (0x0013U << 0U)
#define MODC_ICHG2_FW_MSK          (MODC_MSK_5bit << 5U)
#define MODC_ICHG2_FW_50_8mA       (0x0013U << 5U)
#define MODC_ICHG3_FW_MSK          (MODC_MSK_5bit << 10U)
#define MODC_ICHG3_FW_50_8mA       (0x0013U << 10U)

/*FW PWM max. drive current control register*/
#define MODC_ICHGMAX1_FW_MSK          (MODC_MSK_2bit << 0U)
#define MODC_ICHGMAX1_FW_77_79mA      (0x0002U << 0U)
#define MODC_ICHGMAX2_FW_MSK          (MODC_MSK_2bit << 2U)
#define MODC_ICHGMAX2_FW_77_79mA      (0x0002U << 2U)
#define MODC_ICHGMAX3_FW_MSK          (MODC_MSK_2bit << 4U)
#define MODC_ICHGMAX3_FW_77_79mA      (0x0002U << 4U)

/*IC CTRL1 register*/
#define MODC_EN_DRV               (0x01U << 7U)
#define MODC_EN_OLSC              (0x01U << 6U)
#define MODC_DI_OLSC              (0x00U << 6U)
#define MODC_LOCK                 (0x06U << 1U)
#define MODC_UNLOCK               (0x03U << 1U)
#define MODC_CLR_FLT              (0x01U << 0U)

/*dead time definition*/
#define MODC_DT_120_NS            (0x0U << 6U)
#define MODC_DT_240_NS            (0x1U << 6U)
#define MODC_DT_480_NS            (0x2U << 6U)
#define MODC_DT_960_NS            (0x3U << 6U)

/*Spread spectrum clocking*/
#define MODC_DIS_SSC_EN           (0x00 << 7U)
#define MODC_DIS_SSC_DI           (0x01 << 7U)

/*Sets DRVOFF/nFLT multi-function pin mode.*/
#define MODC_DRVOFF               (0x00 << 6U)
#define MODC_N_FLT                (0x01 << 6U)

/*Charge pump operating mode.*/
#define MODC_CP_MODE_AUTO         (0x0U << 4U)
#define MODC_CP_MODE_DOUBLER      (0x1U << 4U)
#define MODC_CP_MODE_TRIPLER      (0x2U << 4U)
#define MODC_CP_MODE_RSVD         (0x3U << 4U)

/*watchdog enable*/
#define MODC_WD_DI                 (0x00 << 3U)
#define MODC_WD_EN                 (0x01 << 3U)

/*Watchdog fault mode.*/
#define MODC_WD_FLT_M_REPORT_ONLY        (0x0U << 2U)
#define MODC_WD_FLT_M_REPORT_DIS_GATE    (0x1U << 2U)

/*Watchdog timer window.*/
#define MODC_WD_WIN_4_40MS         (0x0U << 1U)
#define MODC_WD_WIN_10_100MS       (0x1U << 1U)

#define MODC_WD_RST                (0x01 << 0U)

/*source current and sink current definiton*/
#define MODC_CURRENT_1OmA          (0x0U << 0U)
#define MODC_CURRENT_2OmA          (0x1U << 0U)
#define MODC_CURRENT_5OmA          (0x2U << 0U)
#define MODC_CURRENT_7OmA          (0x3U << 0U)
#define MODC_CURRENT_105mA         (0x4U << 0U)
#define MODC_CURRENT_155mA         (0x5U << 0U)
#define MODC_CURRENT_21OmA         (0x6U << 0U)
#define MODC_CURRENT_26OmA         (0x7U << 0U)

/*SO ouput limited*/
#define MODC_SO_LIM                 (0x01 << 7U) /*limited to 3.6V*/

/*disable VDS monitor*/
#define MODC_DIS_H2_VDS             (0x01U << 3U)
#define MODC_DIS_L2_VDS             (0x01U << 2U)
#define MODC_DIS_H1_VDS             (0x01U << 1U)
#define MODC_DIS_L1_VDS             (0x01U << 0U)

/*Off time for PWM current chopping definiton*/
#define MODC_OffTime_25us           (0x0U << 6U)
#define MODC_OffTime_50us           (0x1U << 6U)
#define MODC_OffTime_100us          (0x2U << 6U)
#define MODC_OffTime_200us          (0x3U << 6U)

/*chop disable*/
#define MODC_CHOP_DISABLE            (0x01 << 5U)

/*Scale factor for the VREF input definiton*/
#define MODC_SCALE_100              (0x0U << 3U)
#define MODC_SCALE_75               (0x1U << 3U)
#define MODC_SCALE_50               (0x2U << 3U)
#define MODC_SCALE_25               (0x3U << 3U)

/*shunt enable*/
#define MODC_SH_EN                   (0x01 << 2U)

/*current regulatio disable*/
#define MODC_DISABLE_CHOP_IDS       (0x1U << 5U)

/*Shunt amplifier gain definiton*/
#define MODC_GAIN_10                (0x0U << 0U)
#define MODC_GAIN_19_8              (0x1U << 0U)
#define MODC_GAIN_39_4              (0x2U << 0U)
#define MODC_GAIN_78                (0x3U << 0U)

#define MODC_IDRIVE_START_BIT       (0x0U)
#define MODC_IDRIVE_WIDTH           (0x03U)

#define MODC_WD_DLY_START_BIT       (0x03U)
#define MODC_WD_DLY_WIDTH           (0x02U)

#define MODC_WD_EN_START_BIT       (0x04U)
#define MODC_WD_EN_WIDTH           (0x01U)

#define MODC_CFT_START_BIT          (0x00U)
#define MODC_CFT_WIDTH              (0x01U)

#define MODC_TDEAD_START_BIT          (0x06U)
#define MODC_TDEAD_WIDTH              (0x02U)

#define MODC_LOCK_START_BIT          (0x03U)
#define MODC_LOCK_WIDTH              (0x03U)

/*Half-bridge 1 output state control*/
#define MODC_HB_HI_Z               (0x00U)
#define MODC_HB_LO                 (0x01U)
#define MODC_HB_HI                 (0x02U)
#define MODC_HB_RESERVED           (0x03U)

/*Half-bridge 1 output state control*/
#define MODC_HB1_HI_Z               (0x00U << 6U)
#define MODC_HB1_LO                 (0x01U << 6U)
#define MODC_HB1_HI                 (0x02U << 6U)
#define MODC_HB1_PWM                (0x03U << 6U)

/*Half-bridge 2 output state control*/
#define MODC_HB2_HI_Z               (0x00U << 4U)
#define MODC_HB2_LO                 (0x01U << 4U)
#define MODC_HB2_HI                 (0x02U << 4U)
#define MODC_HB2_PWM                (0x03U << 4U)

/*Half-bridge 3 output state control*/
#define MODC_HB3_HI_Z               (0x00U << 2U)
#define MODC_HB3_LO                 (0x01U << 2U)
#define MODC_HB3_HI                 (0x02U << 2U)
#define MODC_HB3_PWM                (0x03U << 2U)

/*Configure PWM input source for half-bridge */
#define MODC_HB_PWM_IN1             0x00U
#define MODC_HB_PWM_IN2             0x01U
#define MODC_HB_PWM_IN3             0x02U
#define MODC_HB_PWM_IN4             0x03U

/*Configure PWM input source for half-bridge 1*/
#define MODC_HB1_PWM_IN1            (0x00U << 6U)
#define MODC_HB1_PWM_IN2            (0x01U << 6U)
#define MODC_HB1_PWM_IN3            (0x02U << 6U)
#define MODC_HB1_PWM_IN4            (0x03U << 6U)

/*Configure PWM input source for half-bridge 2*/
#define MODC_HB2_PWM_IN1            (0x00U << 4U)
#define MODC_HB2_PWM_IN2            (0x01U << 4U)
#define MODC_HB2_PWM_IN3            (0x02U << 4U)
#define MODC_HB2_PWM_IN4            (0x03U << 4U)

/*Configure PWM input source for half-bridge 3*/
#define MODC_HB3_PWM_IN1            (0x00U << 2U)
#define MODC_HB3_PWM_IN2            (0x01U << 2U)
#define MODC_HB3_PWM_IN3            (0x02U << 2U)
#define MODC_HB3_PWM_IN4            (0x03U << 2U)

/*Configure PWM input source for half-bridge 4*/
#define MODC_HB4_PWM_IN1            (0x00U << 0U)
#define MODC_HB4_PWM_IN2            (0x01U << 0U)
#define MODC_HB4_PWM_IN3            (0x02U << 0U)
#define MODC_HB4_PWM_IN4            (0x03U << 0U)

/*Configure PWM input source for half-bridge 5*/
#define MODC_HB5_PWM_IN1            (0x00U << 6U)
#define MODC_HB5_PWM_IN2            (0x01U << 6U)
#define MODC_HB5_PWM_IN3            (0x02U << 6U)
#define MODC_HB5_PWM_IN4            (0x03U << 6U)

/*Configure PWM input source for half-bridge 6*/
#define MODC_HB6_PWM_IN1            (0x00U << 4U)
#define MODC_HB6_PWM_IN2            (0x01U << 4U)
#define MODC_HB6_PWM_IN3            (0x02U << 4U)
#define MODC_HB6_PWM_IN4            (0x03U << 4U)

/*Configure PWM input source for half-bridge 7*/
#define MODC_HB7_PWM_IN1            (0x00U << 2U)
#define MODC_HB7_PWM_IN2            (0x01U << 2U)
#define MODC_HB7_PWM_IN3            (0x02U << 2U)
#define MODC_HB7_PWM_IN4            (0x03U << 2U)

/*Configure PWM input source for half-bridge 8*/
#define MODC_HB8_PWM_IN1            (0x00U << 0U)
#define MODC_HB8_PWM_IN2            (0x01U << 0U)
#define MODC_HB8_PWM_IN3            (0x02U << 0U)
#define MODC_HB8_PWM_IN4            (0x03U << 0U)

/*Set half-bridge 1 PWM to high-side or low-side gate driver*/
#define MODC_HB1_HL_HS              (0x00U << 7U)
#define MODC_HB1_HL_LS              (0x01U << 7U)

/*Set half-bridge 2 PWM to high-side or low-side gate driver*/
#define MODC_HB2_HL_HS              (0x00U << 6U)
#define MODC_HB2_HL_LS              (0x01U << 6U)

/*Set half-bridge 3 PWM to high-side or low-side gate driver*/
#define MODC_HB3_HL_HS              (0x00U << 5U)
#define MODC_HB3_HL_LS              (0x01U << 5U)

/*Configure freewheeling setting for half-bridge 1*/
#define MODC_HB1_FW_ACTIVE          (0x00U << 7U)
#define MODC_HB1_FW_PASSIVE         (0x01U << 7U)

/*Configure freewheeling setting for half-bridge 2*/
#define MODC_HB2_FW_ACTIVE          (0x00U << 6U)
#define MODC_HB2_FW_PASSIVE         (0x01U << 6U)

/*Configure freewheeling setting for half-bridge 3*/
#define MODC_HB3_FW_ACTIVE          (0x00U << 5U)
#define MODC_HB3_FW_PASSIVE         (0x01U << 5U)

/*Insertable digital dead-time for half-bridge 12.*/
#define MODC_HB12_TDEAD_0uS         (0x00U << 6U)
#define MODC_HB12_TDEAD_2uS         (0x01U << 6U)
#define MODC_HB12_TDEAD_4uS         (0x02U << 6U)
#define MODC_HB12_TDEAD_8uS         (0x03U << 6U)

/*Insertable digital dead-time for half-bridge 34.*/
#define MODC_HB34_TDEAD_0uS         (0x00U << 4U)
#define MODC_HB34_TDEAD_2uS         (0x01U << 4U)
#define MODC_HB34_TDEAD_4uS         (0x02U << 4U)
#define MODC_HB34_TDEAD_8uS         (0x03U << 4U)

/*Insertable digital dead-time for half-bridge 56.*/
#define MODC_HB56_TDEAD_0uS         (0x00U << 2U)
#define MODC_HB56_TDEAD_2uS         (0x01U << 2U)
#define MODC_HB56_TDEAD_4uS         (0x02U << 2U)
#define MODC_HB56_TDEAD_8uS         (0x03U << 2U)

/*Insertable digital dead-time for half-bridge 78.*/
#define MODC_HB78_TDEAD_0uS         (0x00U << 0U)
#define MODC_HB78_TDEAD_2uS         (0x01U << 0U)
#define MODC_HB78_TDEAD_4uS         (0x02U << 0U)
#define MODC_HB78_TDEAD_8uS         (0x03U << 0U)

/*VDS over current definiton*/
#define MODC_VDS_H_60mV               (0x00U << 4U)
#define MODC_VDS_H_80mV               (0x01U << 4U)
#define MODC_VDS_H_100mV              (0x02U << 4U)
#define MODC_VDS_H_120mV              (0x03U << 4U)
#define MODC_VDS_H_140mV              (0x04U << 4U)
#define MODC_VDS_H_160mV              (0x05U << 4U)
#define MODC_VDS_H_180mV              (0x06U << 4U)
#define MODC_VDS_H_200mV              (0x07U << 4U)
#define MODC_VDS_H_300mV              (0x08U << 4U)
#define MODC_VDS_H_400mV              (0x09U << 4U)
#define MODC_VDS_H_500mV              (0x0AU << 4U)
#define MODC_VDS_H_600mV              (0x0BU << 4U)
#define MODC_VDS_H_700mV              (0x0CU << 4U)
#define MODC_VDS_H_1000mV             (0x0DU << 4U)
#define MODC_VDS_H_1400mV             (0x0EU << 4U)
#define MODC_VDS_H_2000mV             (0x0FU << 4U)

/*VDS over current definiton*/
#define MODC_VDS_L_60mV               (0x00U)
#define MODC_VDS_L_80mV               (0x01U)
#define MODC_VDS_L_100mV              (0x02U)
#define MODC_VDS_L_120mV              (0x03U)
#define MODC_VDS_L_140mV              (0x04U)
#define MODC_VDS_L_160mV              (0x05U)
#define MODC_VDS_L_180mV              (0x06U)
#define MODC_VDS_L_200mV              (0x07U)
#define MODC_VDS_L_300mV              (0x08U)
#define MODC_VDS_L_400mV              (0x09U)
#define MODC_VDS_L_500mV              (0x0AU)
#define MODC_VDS_L_600mV              (0x0BU)
#define MODC_VDS_L_700mV              (0x0CU)
#define MODC_VDS_L_1000mV             (0x0DU)
#define MODC_VDS_L_1400mV             (0x0EU)
#define MODC_VDS_L_2000mV             (0x0FU)

/* VDS1 VDS2 REG:Drain-source monitoring threshold HB1-4 & HB5-8*/
#define MODC_VDS_TH_0_15V   (0X00U)
#define MODC_VDS_TH_0_20V   (0X01U)
#define MODC_VDS_TH_0_25V   (0X02U)
#define MODC_VDS_TH_0_30V   (0X03U)
#define MODC_VDS_TH_0_40V   (0X04U)
#define MODC_VDS_TH_0_50V   (0X05U)
#define MODC_VDS_TH_0_60V   (0X06U)
#define MODC_VDS_TH_2V      (0X07U)

#define MODC_VDS1_HB1VDSTH_OFFSET      (0U)
#define MODC_VDS1_HB2VDSTH_OFFSET      (3U)
#define MODC_VDS1_HB3VDSTH_OFFSET      (6U)
#define MODC_VDS1_HB4VDSTH_OFFSET      (9U)
#define MODC_VDS2_HB5VDSTH_OFFSET      (0U)
#define MODC_VDS2_HB6VDSTH_OFFSET      (3U)
#define MODC_VDS2_HB7VDSTH_OFFSET      (6U)
#define MODC_VDS2_HB8VDSTH_OFFSET      (9U)

/*PVDD supply undervoltage monitor mode*/
#define MODC_PVDD_UV_MODE_LATCH            (0x00U << 7U)
#define MODC_PVDD_UV_MODE_AUTO_RECOVER     (0x01U << 7U)

/*PVDD supply overvoltage monitor mode*/
#define MODC_PVDD_OV_MODE_LATCH            (0x00U << 5U)
#define MODC_PVDD_OV_MODE_AUTO_RECOVER     (0x01U << 5U)
#define MODC_PVDD_OV_MODE_WARN             (0x02U << 5U)
#define MODC_PVDD_OV_MODE_DISABLE          (0x03U << 5U)

/*PVDD supply overvoltage monitor deglitch time.*/
#define MODC_PVDD_OV_DG_1uS                (0x00U << 3U)
#define MODC_PVDD_UV_DG_2uS                (0x01U << 3U)
#define MODC_PVDD_OV_DG_4uS                (0x02U << 3U)
#define MODC_PVDD_UV_DG_8uS                (0x03U << 3U)

/*PVDD supply overvoltage monitor threshold.*/
#define MODC_PVDD_UV_LVL_21_5V             (0x00U << 2U)
#define MODC_PVDD_UV_LVL_28_5V             (0x01U << 2U)

/*VCP charge pump undervoltage monitor mode.*/
#define MODC_VCP_UV_MODE_LATCH             (0x00U << 1U)
#define MODC_VCP_UV_MODE_AUTO_RECOVER      (0x01U << 1U)

/*VCP charge pump undervoltage monitor threshold.*/
#define MODC_VCP_UV_LVL_4_47V              (0x00U << 0U)
#define MODC_VCP_UV_LVL_6_25V              (0x01U << 0U)

/*Current shunt amplifier 1 reference voltage divider.*/
#define MODC_CSA_DIV_1_AREF_2              (0x00U << 5U)
#define MODC_CSA_DIV_1_AREF_8              (0x01U << 5U)

/*Current shunt amplifier 1 gain setting.*/
#define MODC_CSA_GAIN_1_10VV               (0x00U << 3U)
#define MODC_CSA_GAIN_1_20VV               (0x01U << 3U)
#define MODC_CSA_GAIN_1_40VV               (0x02U << 3U)
#define MODC_CSA_GAIN_1_80VV               (0x03U << 3U)

/*Current shunt amplifier 2 reference voltage divider.*/
#define MODC_CSA_DIV_2_AREF_2              (0x00U << 2U)
#define MODC_CSA_DIV_2_AREF_8              (0x01U << 2U)

/*Current shunt amplifier 2 gain setting.*/
#define MODC_CSA_GAIN_2_10VV               (0x00U << 0U)
#define MODC_CSA_GAIN_2_20VV               (0x01U << 0U)
#define MODC_CSA_GAIN_2_40VV               (0x02U << 0U)
#define MODC_CSA_GAIN_2_80VV               (0x03U << 0U)

/*VGS gate fault monitor mode for half-bridges 1-8.*/
#define MODC_VGS_MODE_LATCH                (0x00U << 6U)
#define MODC_VGS_MODE_CYCLE_BY_CYCLE       (0x01U << 6U)
#define MODC_VGS_MODE_WARN                 (0x02U << 6U)
#define MODC_VGS_MODE_DISABLE              (0x03U << 6U)

/*VGS fault independent shutdown mode configuration.*/
#define MODC_VGS_IND_DISABLE               (0x00U << 5U)
#define MODC_VGS_IND_ENABLE                (0x01U << 5U)

/*VGS threshold comparator level for dead-time handshake and VGS
fault monitor for half-bridge drivers.*/
#define MODC_VGS_LVL_1_4V                  (0x00U << 4U)
#define MODC_VGS_LVL_1V                    (0x01U << 4U)

/*VGS dead-time handshake monitor disable.*/
#define MODC_VGS_HS_ENABLE                 (0x00U << 3U)
#define MODC_VGS_HS_DISABLE                (0x01U << 3U)

/*VDS overcurrent monitor mode for half-bridges 1-8.*/
#define MODC_VDS_MODE_LATCH                (0x00U << 1U)
#define MODC_VDS_MODE_CYCLE_BY_CYCLE       (0x01U << 1U)
#define MODC_VDS_MODE_WARN                 (0x02U << 1U)
#define MODC_VDS_MODE_DISABLE              (0x03U << 1U)

/*VDS fault independent shutdown mode configuration.*/
#define MODC_VDS_IND_DISABLE               (0x00U << 0U)
#define MODC_VDS_IND_ENABLE                (0x01U << 0U)

/*VDS STATE MASK*/
#define MODC_VDS1_MASK               (0x03U << 6)
#define MODC_VDS2_MASK               (0x03U << 4)
#define MODC_VDS3_MASK               (0x03U << 2)
#define MODC_VDS4_MASK               (0x03U << 0)
#define MODC_VDS5_MASK               (0x03U << 6)
#define MODC_VDS6_MASK               (0x03U << 4)
#define MODC_VDS7_MASK               (0x03U << 2)
#define MODC_VDS8_MASK               (0x03U << 0)

#define MODC_DEV_ID_TLE92108_232     (0x0001U)
#define MODC_DEV_ID_TLE92108_231     (0x0003U)
#define MODC_DEV_ID_TLE92104_232     (0x0081U)
#define MODC_DEV_ID_TLE92104_131     (0x0007U)/* 0x0007U 0x0087U*/

/*Half-Bridge Number*/
#define MODC_HALF_BRIDGE_1          1U
#define MODC_HALF_BRIDGE_2          2U
#define MODC_HALF_BRIDGE_3          3U
#define MODC_HALF_BRIDGE_4          4U
#define MODC_HALF_BRIDGE_5          5U
#define MODC_HALF_BRIDGE_6          6U
#define MODC_HALF_BRIDGE_7          7U
#define MODC_HALF_BRIDGE_8          8U

/*Gate Driver State*/
#define MODC_HIGH_IMPEDANCE         0U
#define MODC_DRIVE_LOW_SIDE         1U
#define MODC_DRIVE_HIGH_SIDE        2U
#define MODC_PWM1_HS_ACITVE_FW      3U
#define MODC_PWM1_LS_ACITVE_FW      4U
#define MODC_PWM1_HS_PASSIVE_FW     5U
#define MODC_PWM1_LS_PASSIVE_FW     6U
#define MODC_PWM2_HS_ACITVE_FW      7U
#define MODC_PWM2_LS_ACITVE_FW      8U
#define MODC_PWM2_HS_PASSIVE_FW     9U
#define MODC_PWM2_LS_PASSIVE_FW     10U
#define MODC_PWM3_HS_ACITVE_FW      11U
#define MODC_PWM3_LS_ACITVE_FW      12U
#define MODC_PWM3_HS_PASSIVE_FW     13U
#define MODC_PWM3_LS_PASSIVE_FW     14U
/* _____ G L O B A L - T Y P E S _____________________________________________*/

typedef enum
{
  MODC_PWMCH1,
  MODC_PWMCH2,
  MODC_PWMCH3,
  MODC_PWMCH_MAX
}MODC_PwmChannelId_t;

typedef enum
{
  MODC_PWMCH_OFF,
  MODC_PWMCH_ON,
}MODC_PwmSwitch_t;

/** \SPI out data structure--two bytes width */
/*typedef union{
  uint16 Word;
  struct {
    uint16 dataToWrite     : 8;
    uint16 addres          : 6;
    uint16 command         : 1;
    uint16 reserved        : 1;
  }byte;
}Modc_SpiOutData_t;*/

typedef union
{
  uint16 Word;
  struct {
    uint8 DataLowByte;
    uint8 DataHighByte;
  }byte;
}Modc_dataToWrite_t;

typedef struct
{
  uint8 Op     : 1;
  uint8 Addr   : 5;
  uint8 LABT   : 1;
  uint8 Msb    : 1;
}Modc_AddressByte_t;

typedef struct
{
  Modc_dataToWrite_t dataToWrite;
  Modc_AddressByte_t AddressByte;
  uint8 DummyByte;
}Modc_SpiOutData_t;

typedef union
{
  uint16 Word;
  struct {
    uint8 RespLowByte;
    uint8 RespHighByte;
  }byte;
}Modc_ResponseData_t;

/*typedef struct
{
  uint8 RespLowByte;
  uint8 RespHighByte;
}Modc_ResponseData_t;*/

typedef struct
{
  uint8 Spie     : 1;
  uint8 Oc       : 1;
  uint8 Vdse     : 1;
  uint8 Supe     : 1;
  uint8 Npor     : 1;
  uint8 Te       : 1;
  uint8 Fs       : 1;
  uint8 Msb      : 1;
}Modc_GlobalStatusByte_t;

/** SPI in data structure */
typedef struct{
  Modc_ResponseData_t ResponseData;
  uint8 DummyByte;
  Modc_GlobalStatusByte_t GlobalStatusByte;
}Modc_SpiInData_t;

typedef struct
{
    //TLE92108-232
    /*Control registers :REG_BANK0*/
    uint16 Genctrl1_reg;
    uint16 Genctrl2_reg;
    uint16 Vds1_reg;
    uint16 Vds2_reg;
    uint16 Ccp_blk1_reg;
    uint16 Ccp_blk2_act_reg;
    uint16 Hbmode_reg;
    uint16 Pwmset_reg;
    uint16 Tprechg_reg;
    uint16 Hbidiag_reg;
    uint16 St_ichg_reg;
    uint16 Pwm_ichg_act_reg;
    uint16 Pwm_idchg_act_reg;
    uint16 Pwm_ichgmax_ccp_blk3_act_reg;
    uint16 Tdon_off1_reg;
    uint16 Tdon_off2_reg;
    uint16 Tdon_off3_reg;

    /*Status registers*/
    uint16 Genstat_reg;
    uint16 Dsov_reg;
    uint16 Hbvout_pwmerr_reg;
    uint16 Eff_tdon_off1_reg;
    uint16 Eff_tdon_off2_reg;
    uint16 Eff_tdon_off3_reg;
    uint16 Trise_fall1;
    uint16 Trise_fall2;
    uint16 Trise_fall3;
    uint16 Reserved1;
    uint16 Reserved2;
    uint16 Reserved3;
    uint16 Reserved4;
    uint16 Reserved5;
    uint16 Dev_id;

    /*Control registers :REG_BANK1*/
    uint16 Ccp_blk2_fw_reg;
    uint16 Pwm_pchg_init_reg;
    uint16 Pwm_ichg_fw_reg;
    uint16 Pwm_pdchg_init_reg;
    uint16 Pwm_ichgmax_ccp_blk3_fw_reg;
}Modc_TLE92108_REG_t;
/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ G L O B A L - M A C R O S ___________________________________________*/

/* _____ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/


#endif /* MODC_PRIV_H */

/* _____ E N D _____ (predc_priv.h) __________________________________________*/

