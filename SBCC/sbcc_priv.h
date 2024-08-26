/******************************************************************************/
/**
* \file       sbcc_priv.h
* \brief      SBC Controller module private header    
* \details    
*             
* \author     Weiwen.CHEN
* \date       25/12/2017
* \par        History:
* 
\verbatim
  Version     Author                    Date            Desc   
  1.0         Weiwen.CHEN               25/12/2017
\endverbatim  
*
*/
/**************** (C) Copyright 2018 Magneti Marelli Guangzhou ****************/

#ifndef SBCC_PRIV_H
#define SBCC_PRIV_H
/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "Platform_Types.h"

/* _____ G L O B A L - D E F I N E ___________________________________________*/
/* Frame number of each register */
/* Control register frame number */
#define Sbcc_FRAME_RESET                        0U
#define Sbcc_FRAME_CLEAR                        0U
#define Sbcc_FRAME_WRITE_CR1                    1U
#define Sbcc_FRAME_READ_CR1                     2U
#define Sbcc_FRAME_WRITE_CR2                    3U
#define Sbcc_FRAME_READ_CR2                     4U
#define Sbcc_FRAME_WRITE_CR3                    5U
#define Sbcc_FRAME_READ_CR3                     6U
#define Sbcc_FRAME_WRITE_CR4                    7U
#define Sbcc_FRAME_READ_CR4                     8U
#define Sbcc_FRAME_WRITE_CR5                    9U
#define Sbcc_FRAME_READ_CR5                     10U
#define Sbcc_FRAME_WRITE_CR6                    11U
#define Sbcc_FRAME_READ_CR6                     12U
#define Sbcc_FRAME_WRITE_CR7                    13U
#define Sbcc_FRAME_READ_CR7                     14U
#define Sbcc_FRAME_WRITE_CR8                    15U
#define Sbcc_FRAME_READ_CR8                     16U
#define Sbcc_FRAME_WRITE_CR9                    17U
#define Sbcc_FRAME_READ_CR9                     18U
#define Sbcc_FRAME_WRITE_CR10                   19U
#define Sbcc_FRAME_READ_CR10                    20U
#define Sbcc_FRAME_WRITE_CR11                   21U
#define Sbcc_FRAME_READ_CR11                    22U
#define Sbcc_FRAME_WRITE_CR12                   23U
#define Sbcc_FRAME_READ_CR12                    24U
#define Sbcc_FRAME_WRITE_CR13                   25U
#define Sbcc_FRAME_READ_CR13                    26U
#define Sbcc_FRAME_WRITE_CR14                   27U
#define Sbcc_FRAME_READ_CR14                    28U
#define Sbcc_FRAME_WRITE_CR15                   29U
#define Sbcc_FRAME_READ_CR15                    30U
#define Sbcc_FRAME_WRITE_CR16                   31U
#define Sbcc_FRAME_READ_CR16                    32U
#define Sbcc_FRAME_WRITE_CR17                   33U
#define Sbcc_FRAME_READ_CR17                    34U
#define Sbcc_FRAME_WRITE_CR18                   35U
#define Sbcc_FRAME_READ_CR18                    36U
#define Sbcc_FRAME_WRITE_CR23                   37U
#define Sbcc_FRAME_READ_CR23                    38U
#define Sbcc_FRAME_WRITE_CR24                   39U
#define Sbcc_FRAME_READ_CR24                    40U
#define Sbcc_FRAME_WRITE_CR25                   41U
#define Sbcc_FRAME_READ_CR25                    42U
#define Sbcc_FRAME_WRITE_CR26                   43U
#define Sbcc_FRAME_READ_CR26                    44U
#define Sbcc_FRAME_WRITE_CR27                   45U
#define Sbcc_FRAME_READ_CR27                    46U
#define Sbcc_FRAME_WRITE_CR28                   47U
#define Sbcc_FRAME_READ_CR28                    48U
#define Sbcc_FRAME_WRITE_CR29                   49U
#define Sbcc_FRAME_READ_CR29                    50U
#define Sbcc_FRAME_WRITE_CR34                   51U
#define Sbcc_FRAME_READ_CR34                    52U
#define Sbcc_FRAME_WRITE_CFG                    53U
#define Sbcc_FRAME_READ_CFG                     54U

/* Status register frame number */
#define Sbcc_FRAME_READ_SR1                     55U
#define Sbcc_FRAME_READ_SR2                     56U
#define Sbcc_FRAME_READ_SR3                     57U
#define Sbcc_FRAME_READ_SR4                     58U
#define Sbcc_FRAME_READ_SR5                     59U
#define Sbcc_FRAME_READ_SR6                     60U
#define Sbcc_FRAME_READ_SR7                     61U
#define Sbcc_FRAME_READ_SR8                     62U
#define Sbcc_FRAME_READ_SR9                     63U
#define Sbcc_FRAME_READ_SR10                    64U
#define Sbcc_FRAME_READ_SR11                    65U
#define Sbcc_FRAME_READ_SR12                    66U
#define Sbcc_FRAME_READ_CLEAR_SR1               67U
#define Sbcc_FRAME_READ_CLEAR_SR2               68U
#define Sbcc_FRAME_READ_CLEAR_SR3               69U
#define Sbcc_FRAME_READ_CLEAR_SR4               70U
#define Sbcc_FRAME_READ_CLEAR_SR5               71U
#define Sbcc_FRAME_READ_CLEAR_SR6               72U
#define Sbcc_FRAME_READ_CLEAR_SR7               73U
#define Sbcc_FRAME_READ_CLEAR_SR8               74U
#define Sbcc_FRAME_READ_CLEAR_SR9               75U
#define Sbcc_FRAME_READ_CLEAR_SR10              76U
#define Sbcc_FRAME_READ_CLEAR_SR11              77U
#define Sbcc_FRAME_READ_CLEAR_SR12              78U

#define Sbcc_FRAME_READ_COMP_CODE               79U
#define Sbcc_FRAME_READ_DEV_FAM                 80U
#define Sbcc_FRAME_READ_DEV_NO1                 81U
#define Sbcc_FRAME_READ_DEV_NO2                 82U
#define Sbcc_FRAME_READ_DEV_NO3                 83U
#define Sbcc_FRAME_READ_DEV_NO4                 84U
#define Sbcc_FRAME_READ_DEV_NO5                 85U

#define SBCC_NUM_OF_OUT_INDEX                   86U

/* COMMAND Masks */
#define Sbcc_BTS_WRITE                          0x00000000U
#define Sbcc_BTS_READ                           0x40000000U
#define Sbcc_BTS_READ_CLR                       0x80000000U
#define Sbcc_BTS_READ_DEV_INFOR                 0xC0000000U

/* Global Status Byte */
#define Sbcc_BTS_GSB_GSBN                       0x80U
#define Sbcc_BTS_GSB_RSTB                       0x40U
#define Sbcc_BTS_GSB_SPIE                       0x20U
#define Sbcc_BTS_GSB_PLE                        0x10U
#define Sbcc_BTS_GSB_FE                         0x08U
#define Sbcc_BTS_GSB_DE                         0x04U
#define Sbcc_BTS_GSB_GW                         0x02U
#define Sbcc_BTS_GSB_FS                         0x01U

/* Control register address*/
#define Sbcc_CR1_Addr                           0x01000000U
#define Sbcc_CR2_Addr                           0x02000000U
#define Sbcc_CR3_Addr                           0x03000000U
#define Sbcc_CR4_Addr                           0x04000000U
#define Sbcc_CR5_Addr                           0x05000000U
#define Sbcc_CR6_Addr                           0x06000000U
#define Sbcc_CR7_Addr                           0x07000000U
#define Sbcc_CR8_Addr                           0x08000000U
#define Sbcc_CR9_Addr                           0x09000000U
#define Sbcc_CR10_Addr                          0x0A000000U
#define Sbcc_CR11_Addr                          0x0B000000U
#define Sbcc_CR12_Addr                          0x0C000000U
#define Sbcc_CR13_Addr                          0x0D000000U
#define Sbcc_CR14_Addr                          0x0E000000U
#define Sbcc_CR15_Addr                          0x0F000000U
#define Sbcc_CR16_Addr                          0x10000000U
#define Sbcc_CR17_Addr                          0x11000000U
#define Sbcc_CR18_Addr                          0x12000000U
#define Sbcc_CR19_Addr                          0x13000000U
#define Sbcc_CR20_Addr                          0x14000000U
#define Sbcc_CR21_Addr                          0x15000000U
#define Sbcc_CR22_Addr                          0x16000000U
#define Sbcc_CR23_Addr                          0x17000000U
#define Sbcc_CR24_Addr                          0x18000000U
#define Sbcc_CR25_Addr                          0x19000000U
#define Sbcc_CR26_Addr                          0x1A000000U
#define Sbcc_CR27_Addr                          0x1B000000U
#define Sbcc_CR28_Addr                          0x1C000000U
#define Sbcc_CR29_Addr                          0x1D000000U
#define Sbcc_CR34_Addr                          0x22000000U
#define Sbcc_ConfReg_Addr                       0x3F000000U

/* Status register address */
#define Sbcc_SR1_Addr                           0x31000000U
#define Sbcc_SR2_Addr                           0x32000000U
#define Sbcc_SR3_Addr                           0x33000000U
#define Sbcc_SR4_Addr                           0x34000000U
#define Sbcc_SR5_Addr                           0x35000000U
#define Sbcc_SR6_Addr                           0x36000000U
#define Sbcc_SR7_Addr                           0x37000000U
#define Sbcc_SR8_Addr                           0x38000000U
#define Sbcc_SR9_Addr                           0x39000000U
#define Sbcc_SR10_Addr                          0x3A000000U
#define Sbcc_SR11_Addr                          0x3B000000U
#define Sbcc_SR12_Addr                          0x3C000000U

/* Control Register CR1 (0x01) */
#define Sbcc_CR1_WU_EN                          0x00400000U
#define Sbcc_CR1_WU_PU                          0x00100000U
#define Sbcc_CR1_WU_FILT_1                      0x00020000U
#define Sbcc_CR1_WU_FILT_0                      0x00010000U
#define Sbcc_CR1_TIMER_NI0NT_WAKE_SEL           0x00008000U
#define Sbcc_CR1_TIMER_NINT_EN                  0x00004000U
#define Sbcc_CR1_LIN_WU_EN                      0x00002000U
#define Sbcc_CR1_CAN_WU_EN                      0x00001000U
#define Sbcc_CR1_CANTO_IRQ_EN                   0x00000800U
#define Sbcc_CR1_CAN_RXEN                       0x00000400U
#define Sbcc_CR1_CAN_TXEN                       0x00000200U
#define Sbcc_CR1_CAN_Go_TRX_RDY                 0x00000100U
#define Sbcc_CR1_SWEN                           0x00000080U
#define Sbcc_CR1_HEN                            0x00000040U
#define Sbcc_CR1_V2_1                           0x00000020U
#define Sbcc_CR1_V2_0                           0x00000010U
#define Sbcc_CR1_PARITY                         0x00000008U
#define Sbcc_CR1_STBY_SEL                       0x00000004U
#define Sbcc_CR1_GO_STBY                        0x00000002U
#define Sbcc_CR1_TRIG                           0x00000001U

/* Control Register CR2 (0x02) */
#define Sbcc_CR2_T1_RESTART                     0x00800000U
#define Sbcc_CR2_T1_DIR                         0x00400000U
#define Sbcc_CR2_T1_ON_2                        0x00200000U
#define Sbcc_CR2_T1_ON_1                        0x00100000U
#define Sbcc_CR2_T1_ON_0                        0x00080000U
#define Sbcc_CR2_T1_PER_2                       0x00040000U
#define Sbcc_CR2_T1_PER_1                       0x00020000U
#define Sbcc_CR2_T1_PER_0                       0x00010000U
#define Sbcc_CR2_T2_RESTART                     0x00008000U
#define Sbcc_CR2_T2_DIR                         0x00004000U
#define Sbcc_CR2_T2_ON_2                        0x00002000U
#define Sbcc_CR2_T2_ON_1                        0x00001000U
#define Sbcc_CR2_T2_ON_0                        0x00000800U
#define Sbcc_CR2_T2_PER_2                       0x00000400U
#define Sbcc_CR2_T2_PER_1                       0x00000200U
#define Sbcc_CR2_T2_PER_0                       0x00000100U
#define Sbcc_CR2_LIN_REC_ONLY                   0x00000080U
#define Sbcc_CR2_LIN_TXD_TOUT_EN                0x00000040U
#define Sbcc_CR2_CAN_LOOP_EN                    0x00000020U
#define Sbcc_CR2_PNW_EN                         0x00000010U
#define Sbcc_CR2_V1_RESET_1                     0x00000008U
#define Sbcc_CR2_V1_RESET_0                     0x00000004U
#define Sbcc_CR2_WD_TIME_1                      0x00000002U
#define Sbcc_CR2_WD_TIME_0                      0x00000001U

/* Control Register CR3 (0x03) */
#define Sbcc_CR3_VSREG_LOCK_EN                  0x00800000U
#define Sbcc_CR3_VS_LOCK_EN                     0x00400000U
#define Sbcc_CR3_VSREG_OV_SD_EN                 0x00200000U
#define Sbcc_CR3_VSREG_UV_SD_EN                 0x00100000U
#define Sbcc_CR3_VS_OV_SD_EN                    0x00080000U
#define Sbcc_CR3_VS_UV_SD_EN                    0x00040000U
#define Sbcc_CR3_VSREG_EWTH_9                   0x00000200U
#define Sbcc_CR3_VSREG_EWTH_8                   0x00000100U
#define Sbcc_CR3_VSREG_EWTH_7                   0x00000080U
#define Sbcc_CR3_VSREG_EWTH_6                   0x00000040U
#define Sbcc_CR3_VSREG_EWTH_5                   0x00000020U
#define Sbcc_CR3_VSREG_EWTH_4                   0x00000010U
#define Sbcc_CR3_VSREG_EWTH_3                   0x00000008U
#define Sbcc_CR3_VSREG_EWTH_2                   0x00000004U
#define Sbcc_CR3_VSREG_EWTH_1                   0x00000002U
#define Sbcc_CR3_VSREG_EWTH_0                   0x00000001U

/* Control Register CR4 (0x04) */
#define Sbcc_CR4_OUT1_HS                        0x00200000U
#define Sbcc_CR4_OUT1_LS                        0x00100000U
#define Sbcc_CR4_OUT2_HS                        0x00020000U
#define Sbcc_CR4_OUT2_LS                        0x00010000U
#define Sbcc_CR4_OUT3_HS                        0x00002000U
#define Sbcc_CR4_OUT3_LS                        0x00001000U
#define Sbcc_CR4_OUT4_HS                        0x00000200U
#define Sbcc_CR4_OUT4_LS                        0x00000100U
#define Sbcc_CR4_OUT5_HS                        0x00000020U
#define Sbcc_CR4_OUT5_LS                        0x00000010U
#define Sbcc_CR4_OUT6_HS                        0x00000002U
#define Sbcc_CR4_OUT6_LS                        0x00000001U

/* Control Register CR5 (0x05) */
#define Sbcc_CR5_OUT7_3                         0x00800000U
#define Sbcc_CR5_OUT7_2                         0x00400000U
#define Sbcc_CR5_OUT7_1                         0x00200000U
#define Sbcc_CR5_OUT7_0                         0x00100000U
#define Sbcc_CR5_OUT8_3                         0x00080000U
#define Sbcc_CR5_OUT8_2                         0x00040000U
#define Sbcc_CR5_OUT8_1                         0x00020000U
#define Sbcc_CR5_OUT8_0                         0x00010000U
#define Sbcc_CR5_OUT10_3                        0x00000800U
#define Sbcc_CR5_OUT10_2                        0x00000400U
#define Sbcc_CR5_OUT10_1                        0x00000200U
#define Sbcc_CR5_OUT10_0                        0x00000100U
#define Sbcc_CR5_GH                             0x00000010U
#define Sbcc_CR5_OUTHS_3                        0x00000008U
#define Sbcc_CR5_OUTHS_2                        0x00000004U
#define Sbcc_CR5_OUTHS_1                        0x00000002U
#define Sbcc_CR5_OUTHS_0                        0x00000001U

/* Control Register CR6 (0x06) */
#define Sbcc_CR6_OUT9_3                         0x00800000U
#define Sbcc_CR6_OUT9_2                         0x00400000U
#define Sbcc_CR6_OUT9_1                         0x00200000U
#define Sbcc_CR6_OUT9_0                         0x00100000U
#define Sbcc_CR6_OUT11_3                        0x00080000U
#define Sbcc_CR6_OUT11_2                        0x00040000U
#define Sbcc_CR6_OUT11_1                        0x00020000U
#define Sbcc_CR6_OUT11_0                        0x00010000U
#define Sbcc_CR6_OUT12_3                        0x00008000U
#define Sbcc_CR6_OUT12_2                        0x00004000U
#define Sbcc_CR6_OUT12_1                        0x00002000U
#define Sbcc_CR6_OUT12_0                        0x00001000U
#define Sbcc_CR6_OUT13_3                        0x00000800U
#define Sbcc_CR6_OUT13_2                        0x00000400U
#define Sbcc_CR6_OUT13_1                        0x00000200U
#define Sbcc_CR6_OUT13_0                        0x00000100U
#define Sbcc_CR6_OUT14_3                        0x00000080U
#define Sbcc_CR6_OUT14_2                        0x00000040U
#define Sbcc_CR6_OUT14_1                        0x00000020U
#define Sbcc_CR6_OUT14_0                        0x00000010U
#define Sbcc_CR6_OUT15_3                        0x00000008U
#define Sbcc_CR6_OUT15_2                        0x00000004U
#define Sbcc_CR6_OUT15_1                        0x00000002U
#define Sbcc_CR6_OUT15_0                        0x00000001U

/* Control Register CR7 (0x07) */
#define Sbcc_CR7_OUT1_OCR                       0x00800000U
#define Sbcc_CR7_OUT2_OCR                       0x00400000U
#define Sbcc_CR7_OUT3_OCR                       0x00200000U
#define Sbcc_CR7_OUT4_OCR                       0x00100000U
#define Sbcc_CR7_OUT5_OCR                       0x00080000U
#define Sbcc_CR7_OUT6_OCR                       0x00040000U
#define Sbcc_CR7_OUT7_OCR                       0x00020000U
#define Sbcc_CR7_OUT8_OCR                       0x00010000U
#define Sbcc_CR7_OUTHS_OCR                      0x00008000U
#define Sbcc_CR7_HB_TON_1                       0x00001000U
#define Sbcc_CR7_HB_TON_0                       0x00000800U
#define Sbcc_CR7_HS_TON_1                       0x00000400U
#define Sbcc_CR7_HS_TON_0                       0x00000200U
#define Sbcc_CR7_OCR_FREQ                       0x00000100U
#define Sbcc_CR7_OUT5_OC1                       0x00000080U
#define Sbcc_CR7_OUT5_OC0                       0x00000040U
#define Sbcc_CR7_CM_EN                          0x00000020U
#define Sbcc_CR7_CM_SEL_3                       0x00000008U
#define Sbcc_CR7_CM_SEL_2                       0x00000004U
#define Sbcc_CR7_CM_SEL_1                       0x00000002U
#define Sbcc_CR7_CM_SEL_0                       0x00000001U

/* Control Register CR8 (0x08) */
#define Sbcc_CR8_OUT1_OCR_THX_EN                0x00800000U
#define Sbcc_CR8_OUT2_OCR_THX_EN                0x00400000U
#define Sbcc_CR8_OUT3_OCR_THX_EN                0x00200000U
#define Sbcc_CR8_OUT4_OCR_THX_EN                0x00100000U
#define Sbcc_CR8_OUT5_OCR_THX_EN                0x00080000U
#define Sbcc_CR8_OUT6_OCR_THX_EN                0x00040000U
#define Sbcc_CR8_OUT7_OCR_THX_EN                0x00020000U
#define Sbcc_CR8_OUT8_OCR_THX_EN                0x00010000U
#define Sbcc_CR8_OUTHS_OCR_THX_EN               0x00008000U

/* Control Register CR9 (0x09) */
#define Sbcc_CR9_OUT7_RDSON                     0x00800000U
#define Sbcc_CR9_OUT8_RDSON                     0x00400000U
#define Sbcc_CR9_OUTHS_OL                       0x00008000U
#define Sbcc_CR9_OUT15_OL                       0x00004000U
#define Sbcc_CR9_OUT14_OL                       0x00002000U
#define Sbcc_CR9_OUT13_OL                       0x00001000U
#define Sbcc_CR9_OUT12_OL                       0x00000800U
#define Sbcc_CR9_OUT11_OL                       0x00000400U
#define Sbcc_CR9_OUT10_OL                       0x00000200U
#define Sbcc_CR9_OUT9_OL                        0x00000100U
#define Sbcc_CR9_OUTHS_OC                       0x00000080U
#define Sbcc_CR9_OUT15_OC                       0x00000040U
#define Sbcc_CR9_OUT14_OC                       0x00000020U
#define Sbcc_CR9_OUT13_OC                       0x00000010U
#define Sbcc_CR9_OUT12_OC                       0x00000008U
#define Sbcc_CR9_OUT11_OC                       0x00000004U
#define Sbcc_CR9_OUT10_OC                       0x00000002U
#define Sbcc_CR9_OUT9_OC                        0x00000001U

/* Control Register CR10 (0x0A) */
#define Sbcc_CR10_DIAG_2                        0x00800000U
#define Sbcc_CR10_DIAG_1                        0x00400000U
#define Sbcc_CR10_DIAG_0                        0x00200000U
#define Sbcc_CR10_GH_OL_EN                      0x00100000U
#define Sbcc_CR10_GH_TH_2                       0x00040000U
#define Sbcc_CR10_GH_TH_1                       0x00020000U
#define Sbcc_CR10_GH_TH_0                       0x00010000U
#define Sbcc_CR10_SD                            0x00002000U
#define Sbcc_CR10_SDS                           0x00001000U
#define Sbcc_CR10_COPT_3                        0x00000800U
#define Sbcc_CR10_COPT_2                        0x00000400U
#define Sbcc_CR10_COPT_1                        0x00000200U
#define Sbcc_CR10_COPT_0                        0x00000100U
#define Sbcc_CR10_H_OLTH_HIGH                   0x00000080U
#define Sbcc_CR10_OL_H1L2                       0x00000040U
#define Sbcc_CR10_OL_H2L1                       0x00000020U
#define Sbcc_CR10_SLEW_4                        0x00000010U
#define Sbcc_CR10_SLEW_3                        0x00000008U
#define Sbcc_CR10_SLEW_2                        0x00000004U
#define Sbcc_CR10_SLEW_1                        0x00000002U
#define Sbcc_CR10_SLEW_0                        0x00000001U

/* Control Register CR11 (0x0B) */
#define Sbcc_CR11_ECV_LS                        0x00002000U
#define Sbcc_CR11_ECV_OCR                       0x00001000U
#define Sbcc_CR11_ECON                          0x00000100U
#define Sbcc_CR11_EC_5                          0x00000020U
#define Sbcc_CR11_EC_4                          0x00000010U
#define Sbcc_CR11_EC_3                          0x00000008U
#define Sbcc_CR11_EC_2                          0x00000004U
#define Sbcc_CR11_EC_1                          0x00000002U
#define Sbcc_CR11_EC_0                          0x00000001U

/* Control Register CR12 (0x0C) */
#define Sbcc_CR12_PMW10_FREQ_0                  0x00000010U
#define Sbcc_CR12_PMW10_FREQ_1                  0x00000020U
#define Sbcc_CR12_PMW9_FREQ_0                   0x00000040U
#define Sbcc_CR12_PMW9_FREQ_1                   0x00000080U
#define Sbcc_CR12_PMW8_FREQ_0                   0x00000100U
#define Sbcc_CR12_PMW8_FREQ_1                   0x00000200U
#define Sbcc_CR12_PMW7_FREQ_0                   0x00000400U
#define Sbcc_CR12_PMW7_FREQ_1                   0x00000800U
#define Sbcc_CR12_PMW6_FREQ_0                   0x00001000U
#define Sbcc_CR12_PMW6_FREQ_1                   0x00002000U
#define Sbcc_CR12_PMW5_FREQ_0                   0x00004000U
#define Sbcc_CR12_PMW5_FREQ_1                   0x00008000U
#define Sbcc_CR12_PMW4_FREQ_0                   0x00010000U
#define Sbcc_CR12_PMW4_FREQ_1                   0x00020000U
#define Sbcc_CR12_PMW3_FREQ_0                   0x00040000U
#define Sbcc_CR12_PMW3_FREQ_1                   0x00080000U
#define Sbcc_CR12_PMW2_FREQ_0                   0x00100000U
#define Sbcc_CR12_PMW2_FREQ_1                   0x00200000U
#define Sbcc_CR12_PMW1_FREQ_0                   0x00400000U
#define Sbcc_CR12_PMW1_FREQ_1                   0x00800000U

/* Control Register CR24 (0x18) */
#define Sbcc_CR24_CAN_IDE                       0x00400000U
#define Sbcc_CR24_BR_1                          0x00200000U
#define Sbcc_CR24_BR_0                          0x00100000U
#define Sbcc_CR24_SAMPLE_3                      0x00080000U
#define Sbcc_CR24_SAMPLE_2                      0x00040000U
#define Sbcc_CR24_SAMPLE_1                      0x00020000U
#define Sbcc_CR24_SAMPLE_0                      0x00010000U
#define Sbcc_CR24_DLC_3                         0x00008000U
#define Sbcc_CR24_DLC_2                         0x00004000U
#define Sbcc_CR24_DLC_1                         0x00002000U
#define Sbcc_CR24_DLC_0                         0x00001000U
#define Sbcc_CR24_ID_10                         0x00000400U
#define Sbcc_CR24_ID_9                          0x00000200U
#define Sbcc_CR24_ID_8                          0x00000100U
#define Sbcc_CR24_ID_7                          0x00000080U
#define Sbcc_CR24_ID_6                          0x00000040U
#define Sbcc_CR24_ID_5                          0x00000020U
#define Sbcc_CR24_ID_4                          0x00000010U
#define Sbcc_CR24_ID_3                          0x00000008U
#define Sbcc_CR24_ID_2                          0x00000004U
#define Sbcc_CR24_ID_1                          0x00000002U
#define Sbcc_CR24_ID_0                          0x00000001U

#define Sbcc_CR26_ID_MASK_10                    0x00000400U
#define Sbcc_CR26_ID_MASK_9                     0x00000200U
#define Sbcc_CR26_ID_MASK_8                     0x00000100U
#define Sbcc_CR26_ID_MASK_7                     0x00000080U
#define Sbcc_CR26_ID_MASK_6                     0x00000040U
#define Sbcc_CR26_ID_MASK_5                     0x00000020U
#define Sbcc_CR26_ID_MASK_4                     0x00000010U
#define Sbcc_CR26_ID_MASK_3                     0x00000008U
#define Sbcc_CR26_ID_MASK_2                     0x00000004U
#define Sbcc_CR26_ID_MASK_1                     0x00000002U
#define Sbcc_CR26_ID_MASK_0                     0x00000001U

/* Control Register CR34 (0x22) */
#define Sbcc_CR34_CP_OFF                        0x00000004U
#define Sbcc_CR34_ICMP                          0x00000002U
#define Sbcc_CR34_WD_EN                         0x00000001U

/* Configuration Register (0x3F)*/
#define Sbcc_CFGR_WU_CONFIG                     0x00800000U
#define Sbcc_CFGR_LIN_WU_CONFIG                 0x00400000U
#define Sbcc_CFGR_LIN_HS_EN                     0x00200000U
#define Sbcc_CFGR_TSD_CONFIG                    0x00100000U
#define Sbcc_CFGR_ECV_HV                        0x00080000U
#define Sbcc_CFGR_DM                            0x00040000U
#define Sbcc_CFGR_ICMP_CONFIG_EN                0x00020000U
#define Sbcc_CFGR_WD_CONFIG_EN                  0x00010000U
#define Sbcc_CFGR_MASK_OL_HS1                   0x00008000U
#define Sbcc_CFGR_MASK_OL_LS1                   0x00004000U
#define Sbcc_CFGR_MASK_TW                       0x00002000U
#define Sbcc_CFGR_MASK_EC_OL                    0x00001000U
#define Sbcc_CFGR_MASK_OL                       0x00000800U
#define Sbcc_CFGR_MASK_SPIE                     0x00000400U
#define Sbcc_CFGR_MASK_PLE                      0x00000200U
#define Sbcc_CFGR_MASK_GW                       0x00000100U
#define Sbcc_CFGR_CP_OFF_EN                     0x00000080U
#define Sbcc_CFGR_CP_LOW_CONFIG                 0x00000040U
#define Sbcc_CFGR_CP_DITH_DIS                   0x00000020U
#define Sbcc_CFGR_FS_FORCED                     0x00000010U
#define Sbcc_CFGR_CAN_SUP_5V2_EN                0x00000008U
#define Sbcc_CFGR_TRIG                          0x00000001U

/* Status register SR1*/
#define Sbcc_SR1_VPOR                           ((uint32)0x01U << 0U)
#define Sbcc_SR1_WDFAIL                         ((uint32)0x01U << 1U)
#define Sbcc_SR1_V1UV                           ((uint32)0x01U << 15U)
#define Sbcc_SR1_FS                             ((uint32)0x01U << 3U)
#define Sbcc_SR1_TSD2                           ((uint32)0x01U << 5U)
#define Sbcc_SR1_LIN                            ((uint32)0x01U << 18U)
#define Sbcc_SR1_CAN                            ((uint32)0x01U << 19U)
#define Sbcc_SR1_WU                             ((uint32)0x01U << 20U)

/* Status register SR2*/
#define Sbcc_SR2_VS_UV                          ((uint32)0x01U << 0U)
#define Sbcc_SR2_VS_OV                          ((uint32)0x01U << 1U)
#define Sbcc_SR2_VSREG_UV                       ((uint32)0x01U << 2U)
#define Sbcc_SR2_VSREG_OV                       ((uint32)0x01U << 3U)
#define Sbcc_SR2_VSREG_EW                       ((uint32)0x01U << 4U)
#define Sbcc_SR2_V1FAIL                         ((uint32)0x01U << 5U)
#define Sbcc_SR2_V2FAIL                         ((uint32)0x01U << 6U)
#define Sbcc_SR2_V2SC                           ((uint32)0x01U << 7U)
#define Sbcc_SR2_TW                             ((uint32)0x01U << 8U)
#define Sbcc_SR2_CP_LOW                         ((uint32)0x01U << 9U)
#define Sbcc_SR2_SPI_CNT                        ((uint32)0x01U << 10U)
#define Sbcc_SR2_SPI_INV                        ((uint32)0x01U << 11U)
#define Sbcc_SR2_DS_LS1                         ((uint32)0x01U << 12U)
#define Sbcc_SR2_DS_LS2                         ((uint32)0x01U << 13U)
#define Sbcc_SR2_DS_HS1                         ((uint32)0x01U << 14U)
#define Sbcc_SR2_DS_HS2                         ((uint32)0x01U << 15U)
#define Sbcc_SR2_CAN_SLOW                       ((uint32)0x01U << 16U)
#define Sbcc_SR2_CAN_TDOM                       ((uint32)0x01U << 17U)
#define Sbcc_SR2_CAN_PDOM                       ((uint32)0x01U << 18U)
#define Sbcc_SR2_CAN_PREC                       ((uint32)0x01U << 19U)
#define Sbcc_SR2_CAN_RREC                       ((uint32)0x01U << 20U)
#define Sbcc_SR2_LIN_PREC                       ((uint32)0x01U << 21U)
#define Sbcc_SR2_LIN_TDOM                       ((uint32)0x01U << 22U)
#define Sbcc_SR2_LIN_PDOM                       ((uint32)0x01U << 23U)


/* Status register SR3*/
#define Sbcc_SR3_LS1FS0_OC                      ((uint32)0x01U << 0U)
#define Sbcc_SR3_LS2FS0_OC                      ((uint32)0x01U << 1U)
#define Sbcc_SR3_OUTHS_OC                       ((uint32)0x01U << 2U)
#define Sbcc_SR3_OUT15_OC                       ((uint32)0x01U << 3U)
#define Sbcc_SR3_OUT14_OC                       ((uint32)0x01U << 4U)
#define Sbcc_SR3_OUT13_OC                       ((uint32)0x01U << 5U)
#define Sbcc_SR3_OUT12_OC                       ((uint32)0x01U << 6U)
#define Sbcc_SR3_OUT11_OC                       ((uint32)0x01U << 7U)
#define Sbcc_SR3_OUT10_OC                       ((uint32)0x01U << 8U)
#define Sbcc_SR3_OUT9_OC                        ((uint32)0x01U << 9U)
#define Sbcc_SR3_OUT8_OC                        ((uint32)0x01U << 10U)
#define Sbcc_SR3_OUT7_OC                        ((uint32)0x01U << 11U)
#define Sbcc_SR3_OUT6_LS_OC                     ((uint32)0x01U << 12U)
#define Sbcc_SR3_OUT6_HS_OC                     ((uint32)0x01U << 13U)
#define Sbcc_SR3_OUT5_LS_OC                     ((uint32)0x01U << 14U)
#define Sbcc_SR3_OUT5_HS_OC                     ((uint32)0x01U << 15U)
#define Sbcc_SR3_OUT4_LS_OC                     ((uint32)0x01U << 16U)
#define Sbcc_SR3_OUT4_HS_OC                     ((uint32)0x01U << 17U)
#define Sbcc_SR3_OUT3_LS_OC                     ((uint32)0x01U << 18U)
#define Sbcc_SR3_OUT3_HS_OC                     ((uint32)0x01U << 19U)
#define Sbcc_SR3_OUT2_LS_OC                     ((uint32)0x01U << 20U)
#define Sbcc_SR3_OUT2_HS_OC                     ((uint32)0x01U << 21U)
#define Sbcc_SR3_OUT1_LS_OC                     ((uint32)0x01U << 22U)
#define Sbcc_SR3_OUT1_HS_OC                     ((uint32)0x01U << 23U)

/* Status register SR4*/
#define Sbcc_SR4_ECV_OC                         ((uint32)0x01U << 0U)
#define Sbcc_SR4_DS_HEAT                        ((uint32)0x01U << 1U)

/* Status register SR5*/
#define Sbcc_SR5_ECV_OL                         ((uint32)0x01U << 0U)
#define Sbcc_SR3_GH_OL                          ((uint32)0x01U << 1U)
#define Sbcc_SR3_OUTHS_OL                       ((uint32)0x01U << 2U)
#define Sbcc_SR3_OUT15_OL                       ((uint32)0x01U << 3U)
#define Sbcc_SR3_OUT14_OL                       ((uint32)0x01U << 4U)
#define Sbcc_SR3_OUT13_OL                       ((uint32)0x01U << 5U)
#define Sbcc_SR3_OUT12_OL                       ((uint32)0x01U << 6U)
#define Sbcc_SR3_OUT11_OL                       ((uint32)0x01U << 7U)
#define Sbcc_SR3_OUT10_OL                       ((uint32)0x01U << 8U)
#define Sbcc_SR3_OUT9_OL                        ((uint32)0x01U << 9U)
#define Sbcc_SR3_OUT8_OL                        ((uint32)0x01U << 10U)
#define Sbcc_SR3_OUT7_OL                        ((uint32)0x01U << 11U)
#define Sbcc_SR3_OUT6_LS_OL                     ((uint32)0x01U << 12U)
#define Sbcc_SR3_OUT6_HS_OL                     ((uint32)0x01U << 13U)
#define Sbcc_SR3_OUT5_LS_OL                     ((uint32)0x01U << 14U)
#define Sbcc_SR3_OUT5_HS_OL                     ((uint32)0x01U << 15U)
#define Sbcc_SR3_OUT4_LS_OL                     ((uint32)0x01U << 16U)
#define Sbcc_SR3_OUT4_HS_OL                     ((uint32)0x01U << 17U)
#define Sbcc_SR3_OUT3_LS_OL                     ((uint32)0x01U << 18U)
#define Sbcc_SR3_OUT3_HS_OL                     ((uint32)0x01U << 19U)
#define Sbcc_SR3_OUT2_LS_OL                     ((uint32)0x01U << 20U)
#define Sbcc_SR3_OUT2_HS_OL                     ((uint32)0x01U << 21U)
#define Sbcc_SR3_OUT1_LS_OL                     ((uint32)0x01U << 22U)
#define Sbcc_SR3_OUT1_HS_OL                     ((uint32)0x01U << 23U)

/* Status register SR6*/
#define Sbcc_SR6_TSD1_CL1                       ((uint32)0x01U << 0U)
#define Sbcc_SR6_TSD1_CL2                       ((uint32)0x01U << 1U)
#define Sbcc_SR6_TSD1_CL3                       ((uint32)0x01U << 2U)
#define Sbcc_SR6_TSD1_CL4                       ((uint32)0x01U << 3U)
#define Sbcc_SR6_TSD1_CL5                       ((uint32)0x01U << 4U)
#define Sbcc_SR6_TSD1_CL6                       ((uint32)0x01U << 5U)
#define Sbcc_SR6_TW_CL1                         ((uint32)0x01U << 8U)             
#define Sbcc_SR6_TW_CL2                         ((uint32)0x01U << 9U)             
#define Sbcc_SR6_TW_CL3                         ((uint32)0x01U << 10U)             
#define Sbcc_SR6_TW_CL4                         ((uint32)0x01U << 11U)             
#define Sbcc_SR6_TW_CL5                         ((uint32)0x01U << 12U)             
#define Sbcc_SR6_TW_CL6                         ((uint32)0x01U << 13U)   

/* Status Register SR11 (0x3B) */
#define Sbcc_SR11_VS_MASK                       0x003FF000U
#define Sbcc_SR11_VWU_MASK                      0x000003FFU
#define Sbcc_SR11_VS_START_BIT                  12U
#define Sbcc_SR11_VWU_START_BIT                 0U

/* Status Register SR12 (0x3B) */
#define Sbcc_SR12_SWRD_CR23                     0x00000100U
#define Sbcc_SR12_SWRD_CR24                     0x00000200U
#define Sbcc_SR12_SWRD_CR25                     0x00000400U
#define Sbcc_SR12_SWRD_CR26                     0x00000800U
#define Sbcc_SR12_SWRD_CR27                     0x00001000U
#define Sbcc_SR12_SWRD_CR28                     0x00002000U
#define Sbcc_SR12_SWRD_CR29                     0x00004000U
#define Sbcc_SR12_FERR                          ((uint32)0x01U << 0U)
#define Sbcc_SR12_OSC_FAIL                      ((uint32)0x01U << 6U)
#define Sbcc_SR12_SYSERR                        ((uint32)0x01U << 7U)

/* Out config type */
#define Sbcc_OUT_CFG_OFF                        0U
#define Sbcc_OUT_CFG_ON                         1U
#define Sbcc_OUT_CFG_TIMER1                     2U
#define Sbcc_OUT_CFG_TIMER2                     3U
#define Sbcc_OUT_CFG_PWM1                       4U
#define Sbcc_OUT_CFG_PWM2                       5U
#define Sbcc_OUT_CFG_PWM3                       6U
#define Sbcc_OUT_CFG_PWM4                       7U
#define Sbcc_OUT_CFG_PWM5                       8U
#define Sbcc_OUT_CFG_PWM6                       9U
#define Sbcc_OUT_CFG_PWM7                       10U
#define Sbcc_OUT_CFG_PWM8                       11U
#define Sbcc_OUT_CFG_PWM9                       12U
#define Sbcc_OUT_CFG_PWM10                      13U
#define Sbcc_OUT_CFG_DIR1                       14U
#define Sbcc_OUT_CFG_DIR2                       15U

/* channel status flag masks */
#define Sbcc_CHANNEL_RUN                        0x01U
#define Sbcc_CHANNEL_WAIT                       0x02U
#define Sbcc_CHANNEL_DISABLED                   0x04U
#define Sbcc_CHANNEL_RESUME                     0x08U
#define Sbcc_CHANNEL_FEEDBACK_VALID             0x10U
#define Sbcc_CHANNEL_FEEDBACK_VALIDATING        0x20U
#define Sbcc_CHANNEL_UNDERLOAD_PRESENT          0x40U
#define Sbcc_CHANNEL_OVERLOAD_PRESENT           0x80U
#define Sbcc_CHANNEL_NO_FLAG                    0x00U

/** Diagnostic machine possible states  */
/** Diagnosis feedback selection step */
#define Sbcc_DIAG_STS_SEL_FB                    0U
/** Diagnosis wait feedback syncronization event */
#define Sbcc_DIAG_STS_WAIT_SEM                  1U
/** Diagnosis wait feedback stabilization step */
#define Sbcc_DIAG_STS_WAIT_STAB                 2U
/** Diagnosis acquire feedback step */
#define Sbcc_DIAG_STS_ACQUIRING                 3U
/** Total diagnosis steps */
#define Sbcc_DIAG_STEPS                         4U


/* _____ G L O B A L - T Y P E S _____________________________________________*/
/** \brief Diagnosis step function type */
typedef void (* Sbcc_DiagFunctions_t)(uint8 Device);

typedef enum
{
  SBCC_INDEX_TABLE_PING = 0,
  SBCC_INDEX_TABLE_PONG
}Sbcc_IndexTableType_t;

typedef enum
{
  SBCC_OUTPUT_LS = 0,
  SBCC_OUTPUT_HS,
  SBCC_DLC_PWR_Release,
  SBCC_DLC_PWR_Reset,
  SBCC_DLC_PWR_Crash,
  SBCC_DLC_PWR_PclLock,
  SBCC_DLC_PWR_PclUnlock,
  SBCC_DLC_PWR_BreakOn,
  SBCC_DLC_PWR_IDLE
}Sbcc_OutputType_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 VPOR:1;                    /**< VS Power-on Reset threshold (VPOR) reached */
    uint32 WDFAIL:1;                  /**< Watchdog failure*/
    uint32 FORCED_SLEEP_WD:1;         /**< Entered Forced Vbat_standby mode due to multiple watchdog failures */
    uint32 FORCED_SLEEP_TSD2_V1SC:1;  /**< Entered Forced Vbat_standby mode due to: Thermal shutdown or Short circuit on V1 during startup*/
    uint32 TSD1:1;                    /**< Thermal Shutdown 1 was reached */
    uint32 TSD2:1;                    /**< Thermal Shutdown 2 was reached */
    uint32 DEVICE_STATE:2;            /**< State from which the device woke up */
    uint32 WDFAIL_CNT:4;              /**< Indicates number of subsequent watchdog failures. */
    uint32 V1_RESTART:3;              /**< Indicates the number of TSD2 events which caused a restart of voltage regulator V1 */
    uint32 V1UV:1;                    /**< Indicates undervoltage condition at voltage regulator V1 (V1 < VRTx) */
    uint32 DEBUG_ACTIVE:1;            /**< Debug Mode Active: indicates Device is in Debug mode */
    uint32 WAKE_TIMER:1;              /**< Wake-up by Timer: shows wake up source */
    uint32 WAKE_LIN:1;                /**< Wake-up by LIN: shows wake up source */
    uint32 WAKE_CAN:1;                /**< Wake-up by CAN: shows wake up source */
    uint32 WU_WAKE:1;                 /**< Wake-up by WU: shows wake up source */
    uint32 Reserved:1;                /**< Reserved */
    uint32 WU_STATE:1;                /**< State of WU input */
    uint32 Reserved1:9;               /**< Reserved */
  }Bits;
} Sbcc_StatusRegister1_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 VS_UV:1;           /**< Indicates the voltage at VS has reached the undervoltage threshold */
    uint32 VS_OV:1;           /**< Indicates the voltage at VS has reached the overvoltage threshold*/
    uint32 VSREG_UV:1;        /**< Indicates the voltage at VSREG has reached the undervoltage threshold */
    uint32 VSREG_OV:1;        /**< Indicates the voltage at VSREG has reached the overvoltage threshold*/
    uint32 VSREG_EW:1;        /**< Indicates the voltage at VSREG has reached the early warning threshold(configured in CR3) */
    uint32 V1FAIL:1;          /**< Indicates a V1 fail event occurred since last readout */
    uint32 V2FAIL:1;          /**< Indicates a V2 fail event occurred since last readout */
    uint32 V2SC:1;            /**< Indicates a short circuit to GND condition of V2 at turn-on of the regulator */
    uint32 TW:1;              /**< Indicates the temperature has reached the thermal warning threshold */
    uint32 CP_LOW:1;          /**< Indicates that the charge pump voltage is too low */
    uint32 SPI_SCK_CNT:1;     /**< Indicates an SPI frame with wrong number of CLK cycles was detected */
    uint32 SPI_INV_CMD:1;     /**< Invalid SPI command */
    uint32 DSMON_LS1:1;       /**< Indicates a short-circuit or open-load condition was detected */
    uint32 DSMON_LS2:1;       /**< Indicates a short-circuit or open-load condition was detected */
    uint32 DSMON_HS1:1;       /**< Indicates a short-circuit or open-load condition was detected */
    uint32 DSMON_HS2:1;       /**< Indicates a short-circuit or open-load condition was detected */
    uint32 CAN_SUP_LOW:1;     /**< Voltage at CAN supply pin reached the CAN supply low warning threshold */
    uint32 CAN_TXD_DOM:1;     /**< TxDC pin is dominant for t > tdom(TXDC */
    uint32 CAN_PERM_DOM:1;    /**< CAN bus signal is dominant for t > tCAN */
    uint32 CAN_PERM_REC:1;    /**< CAN bus signal did not follow TxDC for 4 times */
    uint32 CAN_RXD_REC:1;     /**< RxDC has not followed TxDC for 4 times */
    uint32 LIN_PERM_REC:1;    /**< LIN bus signal does not follow TxDL within tLIN */
    uint32 LIN_TXD_DOM:1;     /**< TxDL pin is dominant for t > tdom(TXDL) */
    uint32 LIN_PERM_DOM:1;    /**< LIN bus signal is dominant for t > tdom(bus) */
    uint32 Reserved:8;        /**< Reserved */
  }Bits;
} Sbcc_StatusRegister2_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 LS1FSO_OC:1;         /**< Indicates the output was shut down due to overcurrent condition */
    uint32 LS2FSO_OC:1;         /**< Indicates the output was shut down due to overcurrent condition */
    uint32 OUTHS_OC_TH_EX:1;    /**< Indicates the output was shut down due to overcurrent condition. */
    uint32 OUT15_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT14_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT13_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT12_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT11_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT10_OC:1;          /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT9_OC:1;           /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT8_OC_TH_EX:1;     /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT7_OC_TH_EX:1;     /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT6_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT6_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT5_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT5_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT4_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT4_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT3_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT3_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT2_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT2_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT1_LS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 OUT1_HS_OC_TH_EX:1;  /**< Indicates the output was shut down due to overcurrent condition.*/
    uint32 Reserved:8;          /**< Reserved */
  }Bits;
} Sbcc_StatusRegister3_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 ECV_OC:1;            /**< Indicates the output was shut down due to overcurrent condition */
    uint32 DSMON_HEAT:1;        /**< Indicates a short-circuit condition was detected */
    uint32 OUTHS_OCR_ALERT:1;   /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 Reserved:7;          /**< Reserved */
    uint32 OUT8_OCR_ALERT:1;    /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT7_OCR_ALERT:1;    /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT6_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT6_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT5_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT5_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT4_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT4_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT3_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT3_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT2_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT2_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT1_LS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 OUT1_HS_OCR_ALERT:1; /**< Indicates that the output reached the overcurrent threshold and is in autorecovery mode */
    uint32 Reserved1:8;          /**< Reserved */
  }Bits;
} Sbcc_StatusRegister4_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 ECV_OL:1;           /**< Indicates an open-load condition was detected at the output */
    uint32 GH_OL:1;            /**< Indicates an open-load condition was detected at the output */
    uint32 OUTHS_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT15_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT14_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT13_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT12_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT11_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT10_OL:1;         /**< Indicates an open-load condition was detected at the output */
    uint32 OUT9_OL:1;          /**< Indicates an open-load condition was detected at the output */
    uint32 OUT8_OL:1;          /**< Indicates an open-load condition was detected at the output */
    uint32 OUT7_OL:1;          /**< Indicates an open-load condition was detected at the output */
    uint32 OUT6_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT6_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT5_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT5_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT4_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT4_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT3_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT3_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT2_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT2_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT1_LS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 OUT1_HS_OL:1;       /**< Indicates an open-load condition was detected at the output */
    uint32 Reserved:8;         /**< Reserved */
  }Bits;
} Sbcc_StatusRegister5_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 TSD1_CL1:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 TSD1_CL2:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 TSD1_CL3:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 TSD1_CL4:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 TSD1_CL5:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 TSD1_CL6:1;       /**< Indicates Cluster x has reached the thermal shutdown threshold (TSD1) and shut down */
    uint32 Reserved:2;       /**< Reserved */
    uint32 TW_CL1:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 TW_CL2:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 TW_CL3:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 TW_CL4:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 TW_CL5:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 TW_CL6:1;         /**< Indicates Cluster x has reached the thermal warning threshold */
    uint32 Reserved1:2;      /**< Reserved */
    uint32 ECV_VHI:1;        /**< Indicates the electrochrome voltage is not reached */
    uint32 ECV_VNR:1;        /**< Indicates the electrochrome voltage is not reached */
    uint32 Reserved2:4;      /**< Reserved */
    uint32 WD_TIMER_STATE:2; /**< Indicates Watchdog timer status: 00: 0 - 33%, 01: 33 - 66%, 11: 66 - 100%*/
    uint32 Reserved3:8;         /**< Reserved */
  }Bits;
} Sbcc_StatusRegister6_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 TEMP_CLy:10;        /**< Temperature Cluster y: binary coded voltage of temperature diode for cluster y */
    uint32 Reserved:2;         /**< Reserved */
    uint32 TEMP_CLx:10;        /**< Temperature Cluster x: Binary coded voltage of temperature diode for cluster x */
    uint32 Reserved1:10;       /**< Reserved */
  }Bits;
} Sbcc_StatusRegister7_9_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 Reserved:12;     /**< Reserved */
    uint32 VSREG:10;        /**< Binary coded voltage at VSREG pin */
    uint32 Reserved1:10;    /**< Reserved */
  }Bits;
} Sbcc_StatusRegister10_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 VWU:10;          /**< Binary coded voltage at WU pin */
    uint32 Reserved:2;      /**< Reserved */
    uint32 VS:10;           /**< Binary coded voltage at VS pin */
    uint32 Reserved1:10;    /**< Reserved */
  }Bits;
} Sbcc_StatusRegister11_t;

typedef union
{
  uint32 Word;
  struct
  {
    uint32 FDERR:1;       /**< This bit is set at overflow of the Frame Error Counter (FECNT) in SR12 */
    uint32 WUF:1;         /**< Wake up flag for Wake up Frame */
    uint32 WUP:1;         /**< Wake up flag for Wake up Pattern */
    uint32 CANTO:1;       /**< No communication on the bus,Indicates that there was a transition from TRX BIAS to TRX Sleep */
    uint32 TX_SYNC:1;     /**< Indicates that last received frame was decoded correctly */
    uint32 CAN_SILENT:1;  /**< Online monitoring bit to see if there is silence on the bus */
    uint32 OSC_FAIL:1;    /**< CAN selective wake oscillator failure */
    uint32 SYSERR:1;      /**< CAN System Error */
    uint32 SWRD_CR23:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR24:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR25:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR26:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR27:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR28:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 SWRD_CR29:1;   /**< Status flag for Read operation to Selective Wakeup relevant Registers, 0: Not Read , 1: Read done */
    uint32 Reserved:1;    /**< Reserved */
    uint32 FECNT:5;       /**< CAN Frame Detect Error Counter */
    uint32 Reserved1:11;   /**< Reserved */
  }Bits;
} Sbcc_StatusRegister12_t;

typedef struct
{
  Sbcc_StatusRegister1_t    SR1;
  Sbcc_StatusRegister2_t    SR2;
  Sbcc_StatusRegister3_t    SR3;
  Sbcc_StatusRegister4_t    SR4;
  Sbcc_StatusRegister5_t    SR5;
  Sbcc_StatusRegister6_t    SR6;
  Sbcc_StatusRegister7_9_t  SR7;
  Sbcc_StatusRegister7_9_t  SR8;
  Sbcc_StatusRegister7_9_t  SR9;
  Sbcc_StatusRegister10_t   SR10;
  Sbcc_StatusRegister11_t   SR11;
  Sbcc_StatusRegister12_t   SR12;
} Sbcc_AllStatusRegister_t;

/** \brief  Channel description and diagnosis structure.\n
  Note : this structure could be divided in two because the first part
  are only a const values. This could be done if there is a need
  of reduce the space occupied in RAM */
typedef struct
{
  Sbcc_OutputType_t   OutType;/**<  Output HS or LS */
  uint8   Duty;               /**< Applied duty cycle */
  uint8   Flags;              /**< Channel Running flags */
  uint16  DiagValue;          /**< Last acquired channel analog feedback */
} Sbcc_Channel_t;

typedef struct
{
  uint8    RegInedx;
  uint32   HS_Bit;
  uint32   LS_Bit;
  uint32   BitMask;
  uint8    BitOffset;
} Sbcc_OutputBitCfg_t;

/** \brief  Device diagnosis structure */
typedef struct
{
  uint8   DiagAnalogInput;  /**< Device analog feedback reference */
  uint8   FeedbackSelected; /**< Current feedback selected */
  uint16  HoldSampleCnt;    /**< Waiting stabilization and sampling counter */
  uint8   DiagSts;          /**< Diagnosis machine current status */
  /* There are 3 bytes unused */
} Sbcc_Device_t;

typedef struct {
  uint16 OlMatureTime;
  uint8 OcMatureTime;
  uint8 DematureTime;
  uint16 ProtectTime;
  uint8 TempClId;
  uint32 OcrMask;
  uint32 OcrAlertMask;
  uint32 OlMask;
  uint32 OcMask;
  uint8 Combined;
} Sbcc_DiagCfg_t;

typedef struct {
  uint32 State: 2;
  uint32 ErrType: 3;
  uint32 LastErr: 3;
  uint32 WaitFb: 1;
  uint32 SuspendOCR: 1;
  uint32 DiagTimer: 8;
  uint32 ProtectTimer: 16;
} Sbcc_DiagInfo_t;


/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ G L O B A L - M A C R O S ___________________________________________*/

/* _____ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/

#endif /* SBCC_PRIV_H */

/* _____ E N D _____ (sbcc_priv.h) ___________________________________________*/

