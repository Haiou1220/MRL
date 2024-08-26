

/******************************************************************************/
/**
* \file       asm330lhh_types.h
* \brief      
* \details    
*             
* \author     Hongming.ZHENGH
* \date       09/05/2023
* \par        History:
* 
\verbatim
  Version     Author                    Date            Desc   
  1.0         Hongming.ZHENGH           09/05/2023
\endverbatim  
*
*/
/**************** (C) Copyright 2023 Magneti Marelli Guangzhou ****************/

#ifndef ASM330LHH_TYPES_H
#define ASM330LHH_TYPES_H

/*=======[I N C L U D E S]====================================================*/
#include "Std_Types.h"
# include "Platform_Types.h"

/*=======[M A C R O S]========================================================*/
/* spi read bit option */
#define ASM330LHH_REG_READ_BIT 0x8000u

/* spi write bit option */
#define ASM330LHH_REG_WRITE_BIT 0x0000u

/* spi register address */
#define ASM330LHH_PIN_CTRL_ADDR 0x0200u /* R/W */
#define ASM330LHH_FIFO_CTRL1_ADDR 0x0700u /* R/W */
#define ASM330LHH_FIFO_CTRL2_ADDR 0x0800u /* R/W */
#define ASM330LHH_FIFO_CTRL3_ADDR 0x0900u /* R/W */
#define ASM330LHH_FIFO_CTRL4_ADDR 0x0A00u /* R/W */ 
#define ASM330LHH_COUNTER_BDR_REG1_ADDR 0x0B00u /* R/W */
#define ASM330LHH_COUNTER_BDR_REG2_ADDR 0x0C00u /* R/W */ 
#define ASM330LHH_INT1_CTRL_ADDR 0x0D00u /* R/W */
#define ASM330LHH_INT2_CTRL_ADDR 0x0E00u /* R/W */
#define ASM330LHH_WHO_AM_I_ADDR  0x0F00u /* R */
#define ASM330LHH_CTRL1_XL_ADDR 0x1000u /* R/W */
#define ASM330LHH_CTRL2_G_ADDR 0x1100u /* R/W */
#define ASM330LHH_CTRL3_C_ADDR 0x1200u /* R/W */
#define ASM330LHH_CTRL4_C_ADDR 0x1300u /* R/W */
#define ASM330LHH_CTRL5_C_ADDR 0x1400u /* R/W */
#define ASM330LHH_CTRL6_C_ADDR 0x1500u /* R/W */
#define ASM330LHH_CTRL7_G_ADDR 0x1600u /* R/W */
#define ASM330LHH_CTRL8_XL_ADDR 0x1700u /* R/W */
#define ASM330LHH_CTRL9_XL_ADDR 0x1800u /* R/W */
#define ASM330LHH_CTRL10_C_ADDR 0x1900u /* R/W */
#define ASM330LHH_ALL_INT_SRC_ADDR 0x1A00u /* R */ 
#define ASM330LHH_WAKE_UP_SRC_ADDR 0x1B00u /* R */
#define ASM330LHH_D6D_SRC_ADDR 0x1D00u /* R */ 
#define ASM330LHH_STATUS_REG_ADDR 0x1E00u /* R */
#define ASM330LHH_OUT_TEMP_L_ADDR 0x2000u /* R */
#define ASM330LHH_OUT_TEMP_H_ADDR 0x2100u /* R */ 
#define ASM330LHH_OUTX_H_G_ADDR 0x2300u /* R */
#define ASM330LHH_OUTX_L_G_ADDR 0x2200u /* R */
#define ASM330LHH_OUTY_H_G_ADDR 0x2500u /* R */
#define ASM330LHH_OUTY_L_G_ADDR 0x2400u /* R */
#define ASM330LHH_OUTZ_H_G_ADDR 0x2700u /* R */
#define ASM330LHH_OUTZ_L_G_ADDR 0x2600u /* R */
#define ASM330LHH_OUTX_H_A_ADDR 0x2900u /* R */
#define ASM330LHH_OUTX_L_A_ADDR 0x2800u /* R */ 
#define ASM330LHH_OUTY_H_A_ADDR 0x2B00u /* R */
#define ASM330LHH_OUTY_L_A_ADDR 0x2A00u /* R */
#define ASM330LHH_OUTZ_H_A_ADDR 0x2D00u /* R */
#define ASM330LHH_OUTZ_L_A_ADDR 0x2C00u /* R */
#define ASM330LHH_FIFO_STATUS1_ADDR 0x3A00u /* R */
#define ASM330LHH_FIFO_STATUS2_ADDR 0x3B00u /* R */
#define ASM330LHH_TIMESTAMP0_ADDR 0x4000u /* R */
#define ASM330LHH_TIMESTAMP1_ADDR 0x4100u /* R */ 
#define ASM330LHH_TIMESTAMP2_ADDR 0x4200u /* R */
#define ASM330LHH_TIMESTAMP3_ADDR 0x4300u /* R */ 
#define ASM330LHH_INT_CFG0_ADDR 0x5600u /* R/W */
#define ASM330LHH_INT_CFG1_ADDR 0x5800u /* R/W */
#define ASM330LHH_THS_6D_ADDR 0x5900u /* R/W */
#define ASM330LHH_WAKE_UP_THS_ADDR 0x5B00u /* R/W */
#define ASM330LHH_WAKE_UP_DUR_ADDR 0x5C00u /* R/W */
#define ASM330LHH_FREE_FALL_ADDR 0x5D00u /* R/W */
#define ASM330LHH_MD1_CFG_ADDR 0x5E00u /* R/W */
#define ASM330LHH_MD2_CFG_ADDR 0x5F00u /* R/W */
#define ASM330LHH_INTERNAL_FREQ_FINE_ADDR 0x6300u /* R */
#define ASM330LHH_X_OFS_USR_ADDR 0x7300u /* R/W */
#define ASM330LHH_Y_OFS_USR_ADDR 0x7400u /* R/W */
#define ASM330LHH_Z_OFS_USR_ADDR 0x7500u /* R/W */
#define ASM330LHH_FIFO_DATA_OUT_TAG_ADDR 0x7800u /* R */
#define ASM330LHH_FIFO_DATA_OUT_X_H_ADDR 0x7A00u /* R */
#define ASM330LHH_FIFO_DATA_OUT_X_L_ADDR 0x7900u /* R */
#define ASM330LHH_FIFO_DATA_OUT_Y_H_ADDR 0x7C00u /* R */
#define ASM330LHH_FIFO_DATA_OUT_Y_L_ADDR 0x7B00u /* R */
#define ASM330LHH_FIFO_DATA_OUT_Z_H_ADDR 0x7E00u /* R */
#define ASM330LHH_FIFO_DATA_OUT_Z_L_ADDR 0x7D00u /* R */

/* spi register ID */
typedef enum {
    ASM330LHH_PIN_CTRL_ID,
    ASM330LHH_FIFO_CTRL1_ID,
    ASM330LHH_FIFO_CTRL2_ID,
    ASM330LHH_FIFO_CTRL3_ID,
    ASM330LHH_FIFO_CTRL4_ID,
    ASM330LHH_COUNTER_BDR_REG1_ID,
    ASM330LHH_COUNTER_BDR_REG2_ID,
    ASM330LHH_INT1_CTRL_ID,
    ASM330LHH_INT2_CTRL_ID,
    ASM330LHH_CTRL1_XL_ID,
    ASM330LHH_CTRL2_G_ID,
    ASM330LHH_CTRL3_C_ID,
    ASM330LHH_CTRL4_C_ID,
    ASM330LHH_CTRL5_C_ID,
    ASM330LHH_CTRL6_C_ID,
    ASM330LHH_CTRL7_G_ID,
    ASM330LHH_CTRL8_XL_ID,
    ASM330LHH_CTRL9_XL_ID,
    ASM330LHH_CTRL10_C_ID,
    ASM330LHH_INT_CFG0_ID,
    ASM330LHH_INT_CFG1_ID,
    ASM330LHH_THS_6D_ID,
    ASM330LHH_WAKE_UP_THS_ID,
    ASM330LHH_WAKE_UP_DUR_ID,
    ASM330LHH_FREE_FALL_ID,
    ASM330LHH_MD1_CFG_ID,
    ASM330LHH_MD2_CFG_ID,
    ASM330LHH_X_OFS_USR_ID,
    ASM330LHH_Y_OFS_USR_ID,
    ASM330LHH_Z_OFS_USR_ID,
    ASM330LHH_CTRL_REG_NUM
}ASM330LHH_CTRL_REG_ID_TYPE;


/* spi register ID */
typedef enum {
    ASM330LHH_WHO_AM_I_ID,
    ASM330LHH_ALL_INT_SRC_ID,
    ASM330LHH_WAKE_UP_SRC_ID,
    ASM330LHH_D6D_SRC_ID,
    ASM330LHH_STATUS_REG_ID,
    ASM330LHH_OUT_TEMP_L_ID,
    ASM330LHH_OUT_TEMP_H_ID,
    ASM330LHH_OUTX_H_G_ID,
    ASM330LHH_OUTX_L_G_ID,
    ASM330LHH_OUTY_H_G_ID,
    ASM330LHH_OUTY_L_G_ID,
    ASM330LHH_OUTZ_H_G_ID,
    ASM330LHH_OUTZ_L_G_ID,
    ASM330LHH_OUTX_H_A_ID,
    ASM330LHH_OUTX_L_A_ID,
    ASM330LHH_OUTY_H_A_ID,
    ASM330LHH_OUTY_L_A_ID,
    ASM330LHH_OUTZ_H_A_ID,
    ASM330LHH_OUTZ_L_A_ID,
    ASM330LHH_FIFO_STATUS1_ID,
    ASM330LHH_FIFO_STATUS2_ID,
    ASM330LHH_TIMESTAMP0_ID,
    ASM330LHH_TIMESTAMP1_ID,
    ASM330LHH_TIMESTAMP2_ID,
    ASM330LHH_TIMESTAMP3_ID,
    ASM330LHH_INTERNAL_FREQ_FINE_ID,
    ASM330LHH_FIFO_DATA_OUT_TAG_ID,
    ASM330LHH_FIFO_DATA_OUT_X_H_ID,
    ASM330LHH_FIFO_DATA_OUT_X_L_ID,
    ASM330LHH_FIFO_DATA_OUT_Y_H_ID,
    ASM330LHH_FIFO_DATA_OUT_Y_L_ID,
    ASM330LHH_FIFO_DATA_OUT_Z_H_ID,
    ASM330LHH_FIFO_DATA_OUT_Z_L_ID,
    ASM330LHH_OUT_DATA_REG_NUM
}ASM330LHH_OUT_DATA_REG_ID_TYPE;


/* Enable pull-up on SDO pin. Default value: 0
    0: SDO pin pull-up disconnected; 
    1: SDO pin with pull-up*/
#define ASM330LHH_PIN_CTRL_BIT_SDO_PU_DISABLE 0x3Fu /* default */
#define ASM330LHH_PIN_CTRL_BIT_SDO_PU_ENABLE 0x7Fu

/* FIFO watermark threshold, in conjunction with WTM8 in FIFO_CTRL2 (08h)
    1 LSB = 1 sensor (6 bytes) + TAG (1 byte) written in FIFO
    Watermark flag rises when the number of bytes written in the FIFO is greater than
    or equal to the threshold level. */
#define ASM330LHH_FIFO_CTRL1_BIT_WTM_bit7_0_DEFAULT 0x00u
#define ASM330LHH_FIFO_CTRL1_BIT_WTM_bit7_0_MASK 0xFFu

/* Sensing chain FIFO stop values memorization at threshold level
    0: FIFO depth is not limited (default);
    1: FIFO depth is limited to threshold level, defined in FIFO_CTRL1 (07h) and
        FIFO_CTRL2 (08h)) */
#define ASM330LHH_FIFO_CTRL2_BIT_STOP_ON_WTM_DISABLE 0x00u
#define ASM330LHH_FIFO_CTRL2_BIT_STOP_ON_WTM_ENABLE 0x80u

/* Enables ODR CHANGE virtual sensor to be batched in FIFO */
#define ASM330LHH_FIFO_CTRL2_BIT_ODRCHG_DISABLE 0x00u
#define ASM330LHH_FIFO_CTRL2_BIT_ODRCHG_ENABLE 0x10u

/* FIFO watermark threshold, in conjunction with WTM[7:0] in FIFO_CTRL1 (07h)
    1 LSB = 1 sensor (6 bytes) + TAG (1 byte) written in FIFO
    Watermark flag rises when the number of bytes written in the FIFO is greater than
    or equal to the threshold level. */
#define ASM330LHH_FIFO_CTRL2_BIT_WTM_bit8_DEFAULT 0x00u
#define ASM330LHH_FIFO_CTRL2_BIT_WTM_bit8_MASK 0x01u

/* Selects Batch Data Rate (writing frequency in FIFO) for gyroscope data. 
    0000: Gyro not batched in FIFO (default);
    0001: 12.5 Hz;
    0010: 26 Hz;
    0011: 52 Hz;
    0100: 104 Hz;
    0101: 208 Hz;
    0110: 417 Hz;
    0111: 833 Hz;
    1000: 1667 Hz;
    1001: 3333 Hz;
    1010: 6667 Hz;
    1011: 6.5 Hz;
    1100-1111: not allowed */
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_NotBatched 0x00u /* default */
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_12_5_Hz 0x10u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_26_Hz 0x20u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_52_HZ 0x30u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_104_HZ 0x40u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_208_HZ 0x50u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_417_HZ 0x60u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_833_HZ 0x70u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_1667_HZ 0x80u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_3333_HZ 0x90u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_6667_HZ 0xA0u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_GY_6_5_HZ 0xB0u

/* Selects Batch Data Rate (writing frequency in FIFO) for accelerometer data.
    0000: Accelerometer not batched in FIFO (default);
    0001: 12.5 Hz;
    0010: 26 Hz;
    0011: 52 Hz;
    0100: 104 Hz;
    0101: 208 Hz;
    0110: 417 Hz;
    0111: 833 Hz;
    1000: 1667 Hz;
    1001: 3333 Hz;
    1010: 6667 Hz;
    1011-1111: not allowed */
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_NotBatched 0x00u /* default */
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_12_5_Hz 0x01u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_26_Hz 0x02u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_52_HZ 0x03u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_104_HZ 0x04u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_208_HZ 0x05u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_417_HZ 0x06u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_833_HZ 0x07u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_1667_HZ 0x08u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_3333_HZ 0x09u
#define ASM330LHH_FIFO_CTRL3_BIT_BDR_XL_6667_HZ 0x0Au

/* Selects decimation for timestamp batching in FIFO. Writing rate will be the maximum
    rate between XL and GYRO BDR divided by decimation decoder.
    00: Timestamp not batched in FIFO (default);
    01: Decimation 1: max(BDR_XL[Hz],BDR_GY[Hz]) [Hz];
    10: Decimation 8: max(BDR_XL[Hz],BDR_GY[Hz])/8 [Hz];
    11: Decimation 32: max(BDR_XL[Hz],BDR_GY[Hz])/32 [Hz] */
#define ASM330LHH_FIFO_CTRL4_BIT_DEC_TS_BATCH_NotBatched 0x00u /* default */
#define ASM330LHH_FIFO_CTRL4_BIT_DEC_TS_BATCH_1 0x40u
#define ASM330LHH_FIFO_CTRL4_BIT_DEC_TS_BATCH_8 0x80u
#define ASM330LHH_FIFO_CTRL4_BIT_DEC_TS_BATCH_32 0xC0u

/* Selects batching data rate (writing frequency in FIFO) for temperature data
    00: Temperature not batched in FIFO (default);
    01: 1.6 Hz;
    10: 12.5 Hz;
    11: 52 Hz */
#define ASM330LHH_FIFO_CTRL4_BIT_ODR_T_BATCH_NotBatched 0x00u /* default */
#define ASM330LHH_FIFO_CTRL4_BIT_ODR_T_BATCH_1_6_Hz 0x10u
#define ASM330LHH_FIFO_CTRL4_BIT_ODR_T_BATCH_12_5_Hz 0x20u
#define ASM330LHH_FIFO_CTRL4_BIT_ODR_T_BATCH_52_Hz 0x30u

/* FIFO mode selection
    000: Bypass mode: FIFO disabled;
    001: FIFO mode: stops collecting data when FIFO is full;
    010: Reserved;
    011: Continuous-to-FIFO mode:
        Continuous mode until trigger is deasserted, then FIFO mode;
    100: Bypass-to-Continuous mode:
        Bypass mode until trigger is deasserted, then Continuous mode;
    101: Reserved;
    110: Continuous mode:
        If the FIFO is full, the new sample overwrites the older one;
    111: Bypass-to-FIFO mode:
        Bypass mode until trigger is deasserted, then FIFO mode. */
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_Bypass_mode 0x00u
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_FIFO_mode 0x01u
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_Continuous_to_FIFO_mode 0x03u
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_Bypass_to_Continuous_mode 0x04u
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_Continuous_mode 0x06u
#define ASM330LHH_FIFO_CTRL4_BIT_FIFO_MODE_Bypass_to_FIFO_mode 0x07u

/* Enables pulsed data-ready mode
    0: Data-ready latched mode (returns to 0 only after an interface reading) (default);
    1: Data-ready pulsed mode (the data ready pulses are 75 渭s long */
#define ASM330LHH_COUNTER_BDR_REG1_BIT_Dataready_Latched_mode 0x00u /* default */
#define ASM330LHH_COUNTER_BDR_REG1_BIT_Dataready_Pulsed_mode 0x80u

/* Resets the internal counter of batching events for a single sensor.
    This bit is automatically reset to zero if it was set to 鈥�1鈥�. */
#define ASM330LHH_COUNTER_BDR_REG1_BIT_RST_COUNTER_BDR_DISABLE 0x00u
#define ASM330LHH_COUNTER_BDR_REG1_BIT_RST_COUNTER_BDR_ENABLE 0x40u

/* Selects the trigger for the internal counter of batching events between XL and gyro.
    0: XL batching event;
    1: GYRO batching event */
#define ASM330LHH_COUNTER_BDR_REG1_BIT_TRIG_COUNTER_BDR_XL 0x00u
#define ASM330LHH_COUNTER_BDR_REG1_BIT_TRIG_COUNTER_BDR_GYRO 0x20u

/* In conjunction with CNT_BDR_TH_[7:0] in COUNTER_BDR_REG2 (0Ch), sets the
    threshold for the internal counter of batching events. When this counter reaches the
    threshold, the counter is reset and the COUNTER_BDR_IA flag in FIFO_STATUS2
    (3Bh) is set to 鈥�1鈥�. */
#define ASM330LHH_COUNTER_BDR_REG1_BIT_CNT_BDR_TH_9_8_DEFAULT 0x00u
#define ASM330LHH_COUNTER_BDR_REG1_BIT_CNT_BDR_TH_9_8_MASK 0x02u

/* In conjunction with CNT_BDR_TH_[9:8] in COUNTER_BDR_REG1 (0Bh), sets
    the threshold for the internal counter of batching events. When this counter
    reaches the threshold, the counter is reset and the COUNTER_BDR_IA flag in
    FIFO_STATUS2 (3Bh) is set to 鈥�1鈥�. */
#define ASM330LHH_COUNTER_BDR_REG2_BIT_CNT_BDR_TH_7_0_DEFAULT 0x00u
#define ASM330LHH_COUNTER_BDR_REG2_BIT_CNT_BDR_TH_7_0_MASK 0xFFu

/* Sends DEN_DRDY (DEN stamped on sensor data flag) to INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_DEN_DRDY_flag_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_DEN_DRDY_flag_ENABLE 0x80u

/* Enables COUNTER_BDR_IA interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_CNT_BDR_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_CNT_BDR_ENABLE 0x40u

/* Enables FIFO full flag interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_FULL_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_FULL_ENABLE 0x20u

/* Enables FIFO overrun interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_OVR_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_OVR_ENABLE 0x10u

/* Enables FIFO threshold interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_TH_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_FIFO_TH_ENABLE 0x08u

/* Enables boot status on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_BOOT_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_BOOT_ENABLE 0x04u

/* Gyroscope data-ready interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_DRDY_G_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_DRDY_G_ENABLE 0x02u

/* Accelerometer data-ready interrupt on INT1 pin. */
#define ASM330LHH_INT1_CTRL_BIT_INT1_DRDY_XL_DISABLE 0x00u
#define ASM330LHH_INT1_CTRL_BIT_INT1_DRDY_XL_ENABLE 0x01u

/* Enables COUNTER_BDR_IA interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_CNT_BDR_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_CNT_BDR_ENABLE 0x40u

/* Enables FIFO full flag interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_FIFO_FULL_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_FIFO_FULL_ENABLE 0x20u

/* Enables FIFO overrun interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_FIFO_OVR_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_FIFO_OVR_ENABLE 0x10u

/* Enables FIFO threshold interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT_FIFO_TH_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT_FIFO_TH_ENABLE 0x08u

/* Temperature sensor data-ready interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_TEMP_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_TEMP_ENABLE 0x04u

/* Gyroscope data-ready interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_G_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_G_ENABLE 0x02u

/* Accelerometer data-ready interrupt on INT2 pin. */
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_XL_DISABLE 0x00u
#define ASM330LHH_INT2_CTRL_BIT_INT2_DRDY_XL_ENABLE 0x01u

/* This is a read-only register. Its value is fixed at 6Bh. */
#define ASM330LHH_WHO_AM_I_BIT_WHO_AM_I 0x6Bu

/* Accelerometer ODR selection
    0000: Power-down (default);
    0001: 12.5 Hz;
    0010: 26 Hz;
    0011: 52 Hz;
    0100: 104 Hz;
    0101: 208 Hz;
    0110: 417 Hz;
    0111: 833 Hz;
    1000: 1667 Hz;
    1001: 3333 Hz;
    1010: 6667 Hz;
    1011: not allowed;
    11xx: not allowed */
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_Power_down 0x00u /* default */
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_12_5_Hz 0x10u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_26_Hz 0x20u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_52_Hz 0x30u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_104_Hz 0x40u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_208_Hz 0x50u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_417_Hz 0x60u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_833_Hz 0x70u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_1667_Hz 0x80u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_3333_Hz 0x90u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_6667_Hz 0xA0u
#define ASM330LHH_CTRL1_XL_BIT_ODR_XL_MASK 0xF0u

typedef enum
{
    ASM330LHH_XL_ODR_Power_down,
    ASM330LHH_XL_ODR_12_5_Hz,
    ASM330LHH_XL_ODR_26_Hz,
    ASM330LHH_XL_ODR_52_Hz,
    ASM330LHH_XL_ODR_104_Hz,
    ASM330LHH_XL_ODR_208_Hz,
    ASM330LHH_XL_ODR_417_Hz,
    ASM330LHH_XL_ODR_833_Hz,
    ASM330LHH_XL_ODR_1667_Hz,
    ASM330LHH_XL_ODR_3333_Hz,
    ASM330LHH_XL_ODR_6667_Hz,
}ASM330LHH_XL_ODR_TYPE;

/* Accelerometer full-scale selection
    00: 卤2 g (default);
    01: 卤16 g;
    10: 卤4 g;
    11: 卤8 g */
#define ASM330LHH_CTRL1_XL_BIT_FS_XL_2g 0x00u /* default */
#define ASM330LHH_CTRL1_XL_BIT_FS_XL_16g 0x04u
#define ASM330LHH_CTRL1_XL_BIT_FS_XL_4g 0x08u
#define ASM330LHH_CTRL1_XL_BIT_FS_XL_8g 0x0Cu
#define ASM330LHH_CTRL1_XL_BIT_FS_XL_MASK 0x0Cu

typedef enum
{
    ASM330LHH_XL_FS_2g,
    ASM330LHH_XL_FS_16g,
    ASM330LHH_XL_FS_4g,
    ASM330LHH_XL_FS_8g,
}ASM330LHH_XL_FS_TYPE;

/* Accelerometer high-resolution selection:
    0: Output from first stage digital stage filtering selected (default);
    1: Output from LPF2 second filtering stage selected) */
#define ASM330LHH_CTRL1_XL_BIT_LPF2_XL_DISABLE 0x00u /* default */
#define ASM330LHH_CTRL1_XL_BIT_LPF2_XL_ENABLE 0x02u

/* Gyroscope ODR selection.
    0000: Power-down;
    0001: 12.5 Hz;
    0010: 26 Hz;
    0011: 52 Hz;
    0100: 104 Hz;
    0101: 208 Hz;
    0110: 417 Hz;
    0111: 833 Hz;
    1000: 1667 Hz;
    1001: 3333 Hz;
    1010: 6667 Hz;
    1011: not allowed;
    11xx: not allowed */
#define ASM330LHH_CTRL2_G_BIT_ODR_G_Power_down 0x00u /* default */
#define ASM330LHH_CTRL2_G_BIT_ODR_G_12_5_Hz 0x10u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_26_Hz 0x20u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_52_Hz 0x30u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_104_Hz 0x40u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_208_Hz 0x50u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_417_Hz 0x60u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_833_Hz 0x70u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_1667_Hz 0x80u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_3333_Hz 0x90u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_6667_Hz 0xA0u
#define ASM330LHH_CTRL2_G_BIT_ODR_G_MASK 0xF0u

typedef enum
{
    ASM330LHH_G_ODR_Power_down = 0,
    ASM330LHH_G_ODR_12_5_Hz,
    ASM330LHH_G_ODR_26_Hz,
    ASM330LHH_G_ODR_52_Hz,
    ASM330LHH_G_ODR_104_Hz,
    ASM330LHH_G_ODR_208_Hz,
    ASM330LHH_G_ODR_417_Hz,
    ASM330LHH_G_ODR_833_Hz,
    ASM330LHH_G_ODR_1667_Hz,
    ASM330LHH_G_ODR_3333_Hz,
    ASM330LHH_G_ODR_6667_Hz,
}ASM330LHH_G_ODR_TYPE;

/* Gyroscope chain full-scale selection
    00: 卤250 dps;
    01: 卤500 dps;
    10: 卤1000 dps;
    11: 卤2000 dps */
#define ASM330LHH_CTRL2_G_BIT_FS_G_250_dps 0x00u
#define ASM330LHH_CTRL2_G_BIT_FS_G_500_dps 0x04u
#define ASM330LHH_CTRL2_G_BIT_FS_G_1000_dps 0x08u
#define ASM330LHH_CTRL2_G_BIT_FS_G_2000_dps 0x0Cu
#define ASM330LHH_CTRL2_G_BIT_FS_G_MASK 0x0Cu

typedef enum
{
    ASM330LHH_G_FS_125_dps,
    ASM330LHH_G_FS_250_dps,
    ASM330LHH_G_FS_500_dps,
    ASM330LHH_G_FS_1000_dps,
    ASM330LHH_G_FS_2000_dps,
    ASM330LHH_G_FS_4000_dps,
}ASM330LHH_G_FS_TYPE;

/* Selects gyro chain full-scale 卤125 dps
    0: FS selected through bits FS[1:0]_G;
    1: FS set to 卤125 dps */
#define ASM330LHH_CTRL2_G_BIT_FS_125_THROUGH_FS_G 0x00u
#define ASM330LHH_CTRL2_G_BIT_FS_125_dps_SET 0x02u

/* Selects gyro chain full-scale 卤4000 dps
    0: FS selected through bits FS[1:0]_G or FS_125;
    1: FS set to 卤4000 dps */
#define ASM330LHH_CTRL2_G_BIT_FS_4000_THROUGH_FS_G_FS_125 0x00u
#define ASM330LHH_CTRL2_G_BIT_FS_4000_dps_SET 0x01u

/* Reboots memory content. Default value: 0
    0: normal mode; 1: reboot memory content
    Note: the accelerometer must be ON. This bit is automatically cleared. */
#define ASM330LHH_CTRL3_C_BIT_BOOT_NormalMode 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_BOOT_Reboot 0x80u

/* Block Data Update. Default value: 0
    0: continuous update;
    1: output registers are not updated until MSB and LSB have been read */
#define ASM330LHH_CTRL3_C_BIT_BDU_ContinuousUpdate 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_BDU_NotUpdateUntilRead 0x40u

/* Interrupt activation level. Default value: 0
    0: interrupt output pins active high; 
    1: interrupt output pins active low */
#define ASM330LHH_CTRL3_C_BIT_H_LACTIVE_High 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_H_LACTIVE_Low 0x20u

/* Push-pull/open-drain selection on INT1 and INT2 pins. This bit must be set to '0'
    when H_LACTIVE is set to '1'. Default value: 0
    0: push-pull mode; 1: open-drain mode */
#define ASM330LHH_CTRL3_C_BIT_PP_OD_PushPull_Mode 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_PP_OD_OpenDrain_Mode 0x10u

/* SPI Serial Interface Mode selection. Default value: 0
    0: 4-wire interface; 1: 3-wire interface */
#define ASM330LHH_CTRL3_C_BIT_SIM_4_Wire 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_SIM_3_Wire 0x08u

/* Register address automatically incremented during a multiple byte access with a
    serial interface (I虏C or SPI). Default value: 1
    0: disabled; 1: enabled */
#define ASM330LHH_CTRL3_C_BIT_IF_INC_DISABLE 0x00u
#define ASM330LHH_CTRL3_C_BIT_IF_INC_ENABLE 0x04u /* default */

/* Software reset. Default value: 0
    0: normal mode; 1: reset device
    This bit is automatically cleared. */
#define ASM330LHH_CTRL3_C_BIT_SW_RESET_NormalMode 0x00u /* default */
#define ASM330LHH_CTRL3_C_BIT_SW_RESET_ResetDevice 0x01u

/* Enables gyroscope Sleep mode. Default value:0
    0: disabled; 1: enabled */
#define ASM330LHH_CTRL4_C_BIT_SLEEP_G_DISABLE 0x00u
#define ASM330LHH_CTRL4_C_BIT_SLEEP_G_ENABLE 0x40u

/* All interrupt signals available on INT1 pin enable. Default value: 0
    0: interrupt signals divided between INT1 and INT2 pins;
    1: all interrupt signals in logic or on INT1 pin */
#define ASM330LHH_CTRL4_C_BIT_INT2_on_INT1_DISABLE 0x00u
#define ASM330LHH_CTRL4_C_BIT_INT2_on_INT1_ENABLE 0x20u

/* Enables data available
    0: disabled;
    1: mask DRDY on pin (both XL & Gyro) until filter settling ends (XL and Gyro
    independently masked). */
#define ASM330LHH_CTRL4_C_BIT_DRDY_MASK_DISABLE 0x00u
#define ASM330LHH_CTRL4_C_BIT_DRDY_MASK_ENABLE 0x08u

/* Disables I虏C interface. Default value: 0
    0: SPI and I虏C interfaces enabled (default);
    1: I虏C interface disabled */
#define ASM330LHH_CTRL4_C_BIT_I2C_and_SPI_ENABLE 0x00u
#define ASM330LHH_CTRL4_C_BIT_I2C_DISABLE 0x04u

/* Enables gyroscope digital LPF1; the bandwidth can be selected through
    FTYPE[2:0] in CTRL6_C (15h).
    0: disabled; 1: enabled */
#define ASM330LHH_CTRL4_C_BIT_I2C_LPF1_SEL_G_DISABLE 0x00u
#define ASM330LHH_CTRL4_C_BIT_I2C_LPF1_SEL_G_ENABLE 0x02u

/* Circular burst-mode (rounding) read of the output registers. Default value: 00
    00: no rounding;
    01: accelerometer only;
    10: gyroscope only;
    11: gyroscope + accelerometer */
#define ASM330LHH_CTRL5_C_BIT_ROUNDING_NO 0x00u
#define ASM330LHH_CTRL5_C_BIT_ROUNDING_Accelerometer_Only 0x20u
#define ASM330LHH_CTRL5_C_BIT_ROUNDING_Gyroscope_Only 0x40u
#define ASM330LHH_CTRL5_C_BIT_ROUNDING_Gyroscope_Accelerometer 0x60u

/* Angular rate sensor self-test enable. Default value: 00
    00: Self-test disabled; 
    Other: refer to Table 48
    Table 48. Angular rate sensor self-test mode selection
    ST1_G    ST0_G    Self-test mode
    0        0        Normal mode
    0        1        Positive sign self-test
    1        0        Not allowed
    1        1        Negative sign self-test */
#define ASM330LHH_CTRL5_C_BIT_ST_G_NormalMode 0x00u
#define ASM330LHH_CTRL5_C_BIT_ST_G_Positive_Sign_SelfTest 0x04u
#define ASM330LHH_CTRL5_C_BIT_ST_G_Negative_Sign_SelfTest 0x0Cu

/* Linear acceleration sensor self-test enable. Default value: 00
    00: Self-test disabled; 
    Other: refer to Table 49
    Table 49. Linear acceleration sensor self-test mode selection
    ST1_XL    ST0_XL    Self-test mode
    0        0        Normal mode
    0        1        Positive sign self-test
    1        0        Negative sign self-test
    1        1        Not allowed    */
#define ASM330LHH_CTRL5_C_BIT_ST_XL_NormalMode 0x00u
#define ASM330LHH_CTRL5_C_BIT_ST_XL_Positive_Sign_SelfTest 0x04u
#define ASM330LHH_CTRL5_C_BIT_ST_XL_Negative_Sign_SelfTest 0x0Cu

/* DEN data edge-sensitive trigger enable. */
#define ASM330LHH_CTRL6_C_BIT_TRIG_EN_DISABLE 0x00u
#define ASM330LHH_CTRL6_C_BIT_TRIG_EN_ENABLE 0x80u

/* DEN data level-sensitive trigger enable. */
#define ASM330LHH_CTRL6_C_BIT_LVL1_EN_DISABLE 0x00u
#define ASM330LHH_CTRL6_C_BIT_LVL1_EN_ENABLE 0x40u

/* DEN level-sensitive latched enable. */
#define ASM330LHH_CTRL6_C_BIT_LVL2_EN_DISABLE 0x00u
#define ASM330LHH_CTRL6_C_BIT_LVL2_EN_ENABLE 0x20u

/* Edge-sensitive trigger mode is selected */
#define ASM330LHH_CTRL6_C_BITS_Edge_Sensitive_Trigger ASM330LHH_CTRL6_C_BIT_TRIG_EN_ENABLE | \
                                                      ASM330LHH_CTRL6_C_BIT_LVL1_EN_DISABLE | \
                                                      ASM330LHH_CTRL6_C_BIT_LVL2_EN_DISABLE

/* Level-sensitive trigger mode is selected */
#define ASM330LHH_CTRL6_C_BITS_Level_Sensitive_Trigger ASM330LHH_CTRL6_C_BIT_TRIG_EN_DISABLE | \
                                                       ASM330LHH_CTRL6_C_BIT_LVL1_EN_ENABLE | \
                                                       ASM330LHH_CTRL6_C_BIT_LVL2_EN_DISABLE

/* Level-sensitive latched mode is selected */
#define ASM330LHH_CTRL6_C_BITS_Level_Sensitive_Latched ASM330LHH_CTRL6_C_BIT_TRIG_EN_DISABLE | \
                                                       ASM330LHH_CTRL6_C_BIT_LVL1_EN_ENABLE | \
                                                       ASM330LHH_CTRL6_C_BIT_LVL2_EN_ENABLE

/* Level-sensitive FIFO enable mode is selected */
#define ASM330LHH_CTRL6_C_BITS_Level_Sensitive_FIFO ASM330LHH_CTRL6_C_BIT_TRIG_EN_ENABLE | \
                                                    ASM330LHH_CTRL6_C_BIT_LVL1_EN_ENABLE | \
                                                    ASM330LHH_CTRL6_C_BIT_LVL2_EN_DISABLE

/* Weight of XL user offset bits of registers X_OFS_USR (73h), Y_OFS_USR (74h),
    Z_OFS_USR (75h)
    0 = 2^-10 g/LSB;
    1 = 2^-6 g/LSB */
#define ASM330LHH_CTRL6_C_BIT_USR_OFF_W_2_10 0x00u
#define ASM330LHH_CTRL6_C_BIT_USR_OFF_W_2_6 0x08u

/* Gyroscope low-pass filter (LPF1) bandwidth selection.
    Table 53 shows the selectable bandwidth values.
    Table 53. Gyroscope LPF1 bandwidth selection
    FTYPE
    [2:0]    12.5 Hz  26 Hz   52 Hz   104 Hz    208 Hz   416 Hz   833 Hz   1.67 kHz   3.33 kHz   6.67 kHz
    000        4.3     8.3     16.7    33        67       133      222      274        292        297
    001        4.3     8.3     16.7    33        67       128      186      212        220        223
    010        4.3     8.3     16.7    33        67       112      140      150        153        154
    011        4.3     8.3     16.7    33        67       134      260      390        451        470
    100        4.3     8.3     16.7    34        62       86       96       99         NA         NA
    101        4.3     8.3     16.9    31        43       48       49       50         NA         NA
    110        4.3     8.3     13.4    19        23       24.6     25       25         NA         NA
    111        4.3     8.3     9.8     11.6      12.2     12.4     12.6     12.6       NA         NA */
#define ASM330LHH_CTRL6_C_BIT_FTYPE_DEFAULT 0x00u
#define ASM330LHH_CTRL6_C_BIT_FTYPE_MASK 0x07u

/* Enables gyroscope digital high-pass filter. The filter is enabled only if the gyro is
    in HP mode. Default value: 0
    (0: HPF disabled; 1: HPF enabled) */
#define ASM330LHH_CTRL7_G_BIT_HP_EN_G_DISABLE 0x00u
#define ASM330LHH_CTRL7_G_BIT_HP_EN_G_ENABLE 0x40u

/* Gyroscope digital HP filter cutoff selection. Default: 00
    00 = 16 mHz;
    01 = 65 mHz;
    10 = 260 mHz;
    11 = 1.04 Hz */
#define ASM330LHH_CTRL7_G_BIT_HPM_G_16mHz 0x00u
#define ASM330LHH_CTRL7_G_BIT_HPM_G_65mHz 0x10u
#define ASM330LHH_CTRL7_G_BIT_HPM_G_260mHz 0x20u
#define ASM330LHH_CTRL7_G_BIT_HPM_G_1_04mHz 0x30u

/* Enables accelerometer user offset correction block; it's valid for the low-pass
    path - see Figure 13. Default value: 0
    0: accelerometer user offset correction block bypassed;
    1: accelerometer user offset correction block enabled */
#define ASM330LHH_CTRL7_G_BIT_USR_OFF_ON_OUT_Bypassed 0x00u
#define ASM330LHH_CTRL7_G_BIT_USR_OFF_ON_OUT_Enable 0x02u

/* Accelerometer LPF2 and HP filter configuration and cutoff setting. Refer to Table 58.
                    Table 58. Accelerometer bandwidth configurations
    Filter type        HP_SLOPE_XL_EN        LPF2_XL_EN        HPCF_XL_[2:0]        Bandwidth
                                                 0                   -                ODR/2
                                        -----------------------------------------------------------
                                                                    000               ODR/4
                                                                    001               ODR/10
                                                                    010               ODR/20
                                                                    011               ODR/45
    Low pass                0                    1                  100               ODR/100
                                                                    101               ODR/200
                                                                    110               ODR/400
                                                                    111               ODR/800
    -----------------------------------------------------------------------------------------------
                                                                    000               SLOPE (ODR/4)
                                                                    001               ODR/10
    High pass                1                   -                  010               ODR/20
                                                                    011               ODR/45
                                                                    100               ODR/100
                                                                    101               ODR/200
                                                                    110               ODR/400
                                                                    111               ODR/800 */
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_4 0x00u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_10 0x20u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_20 0x40u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_45 0x60u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_100 0x80u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_200 0xA0u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_400 0xC0u
#define ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_800 0xD0u

/* Enables accelerometer high-pass filter reference mode (valid for high-pass path -
    HP_SLOPE_XL_EN bit must be 鈥�1鈥�). Default value: 0
    (0: disabled, 1: enabled(1)) */
#define ASM330LHH_CTRL8_XL_BIT_HP_REF_MODE_XL_DISABLE 0x00u
#define ASM330LHH_CTRL8_XL_BIT_HP_REF_MODE_XL_ENABLE 0x10u

/* Enables accelerometer LPF2 and HPF fast-settling mode. The filter sets the second samples after writing this bit. 
    Active only during device exit from powerdown mode. Default value: 0
    (0: disabled, 1: enabled) */
#define ASM330LHH_CTRL8_XL_BIT_FASTSETTL_MODE_XL_DISABLE 0x00u
#define ASM330LHH_CTRL8_XL_BIT_FASTSETTL_MODE_XL_ENABLE 0x08u

/* Accelerometer slope filter / high-pass filter selection. Refer to Figure 15. */
#define ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS 0x00u
#define ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS 0x04u

/* LPF2 on 6D function selection. Refer to Figure 15. Default value: 0
    0: ODR/2 low-pass filtered data sent to 6D interrupt function;
    1: LPF2 output data sent to 6D interrupt function */
#define ASM330LHH_CTRL8_XL_BIT_LOW_PASS_ON_6D_ODR_2_LOW_PASS_FILTERED 0x00u
#define ASM330LHH_CTRL8_XL_BIT_LOW_PASS_ON_6D_LPF2_OUTPUT 0x01u

/* Low pass, Bandwidth: ODR/2, LPF2_XL_EN(CTRL1_XL): 0 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_2 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS

/* Low pass, Bandwidth: ODR/4, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_4 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                               ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_4

/* Low pass, Bandwidth: ODR/10, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_10 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_10    

/* Low pass, Bandwidth: ODR/20, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_20 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_20

/* Low pass, Bandwidth: ODR/45, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_45 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_45

/* Low pass, Bandwidth: ODR/100, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_100 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_100

/* Low pass, Bandwidth: ODR/200, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_200 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_200

/* Low pass, Bandwidth: ODR/400, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_400 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_400

/* Low pass, Bandwidth: ODR/800, LPF2_XL_EN(CTRL1_XL): 1 */
#define ASM330LHH_CTRL8_XL_BITS_LOW_PASS_ODR_800 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_LOW_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_800                        

/* High pass, Bandwidth: ODR/4 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_SLOPE_ODR_4 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                      ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_4

/* High pass, Bandwidth: ODR/10 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_10 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_10    

/* High pass, Bandwidth: ODR/20 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_20 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_20

/* High pass, Bandwidth: ODR/45 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_45 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                 ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_45

/* High pass, Bandwidth: ODR/100 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_100 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                  ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_100

/* High pass, Bandwidth: ODR/200 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_200 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                  ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_200

/* High pass, Bandwidth: ODR/400 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_400 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                  ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_400

/* High pass, Bandwidth: ODR/800 */
#define ASM330LHH_CTRL8_XL_BITS_HIGH_PASS_ODR_800 ASM330LHH_CTRL8_XL_BIT_HP_SLOPE_XL_EN_HIGH_PASS | \
                                                  ASM330LHH_CTRL8_XL_BIT_HPCF_XL_ODR_800

/* DEN value stored in LSB of X-axis. Default value: 1
    (0: DEN not stored in X-axis LSB; 1: DEN stored in X-axis LSB) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_X_NOT_STORED_IN_X_axis_LSB 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_X_STORED_IN_X_axis_LSB 0x80u

/* DEN value stored in LSB of Y-axis. Default value: 1
    (0: DEN not stored in Y-axis LSB; 1: DEN stored in Y-axis LSB) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_Y_NOT_STORED_IN_Y_axis_LSB 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_Y_STORED_IN_Y_axis_LSB 0x40u

/* DEN value stored in LSB of Z-axis. Default value: 1
    (0: DEN not stored in Z-axis LSB; 1: DEN stored in Z-axis LSB) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_Z_NOT_STORED_IN_Z_axis_LSB 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_Z_STORED_IN_Z_axis_LSB 0x40u

/* DEN stamping sensor selection. Default value: 0
    0: DEN pin info stamped in the gyroscope axis selected by bits [7:5];
    1: DEN pin info stamped in the accelerometer axis selected by bits [7:5]) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_XL_G_Stamped_In_The_Gyroscope_Axis 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_XL_G_Stamped_In_The_Accelerometer_Axis 0x10u

/* Extends DEN functionality to accelerometer sensor. Default value: 0
    (0: disabled; 1: enabled) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_XL_EN_DISABLE 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_XL_EN_ENABLE 0x80u

/* DEN active level configuration. Default value: 0
    (0: active low; 1: active high) */
#define ASM330LHH_CTRL9_XL_BIT_DEN_LH_ACTIVE_LOW 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEN_LH_ACTIVE_HIGH 0x40u

/* Enables the proper device configuration. Default value: 0
    It is recommended to always set this bit to 1 during device configuration.
    0: default;
    1: enabled */
#define ASM330LHH_CTRL9_XL_BIT_DEVICE_CONF_DEFAULT 0x00u
#define ASM330LHH_CTRL9_XL_BIT_DEVICE_CONF_ENABLE 0x02u

/* Enables timestamp counter. Default value: 0
    (0: disabled; 1: enabled)
    The counter is readable in TIMESTAMP0 (40h), TIMESTAMP1 (41h),
TIMESTAMP2 (42h), and TIMESTAMP3 (43h). */
#define ASM330LHH_CTRL10_C_BIT_TIMESTAMP_EN_DISABLE 0x00u
#define ASM330LHH_CTRL10_C_BIT_TIMESTAMP_EN_ENABLE 0x20u

/* Alerts timestamp overflow within 6.4 ms */
#define ASM330LHH_ALL_INT_SRC_BIT_TIMESTAMP_ENDCOUNT_NOT_OVERFLOW 0x00u
#define ASM330LHH_ALL_INT_SRC_BIT_TIMESTAMP_ENDCOUNT_OVERFLOW 0x80u

/* Detects change event in activity/inactivity status. Default value: 0
    (0: change status not detected; 1: change status detected) */
#define ASM330LHH_ALL_INT_SRC_BIT_SLEEP_CHANGE_IA_NOT_DETECTED 0x00u
#define ASM330LHH_ALL_INT_SRC_BIT_SLEEP_CHANGE_IA_DETECTED 0x20u

/* Interrupt active for change in position of portrait, landscape, face-up,    face-down. Default value: 0
    (0: change in position not detected; 1: change in position detected) */
#define ASM330LHH_ALL_INT_SRC_BIT_D6D_IA_D6D_IA_CHANGE_NOT_DETECTED 0x00u
#define ASM330LHH_ALL_INT_SRC_BIT_D6D_IA_D6D_IA_CHANGE_DETECTED 0x10u

/* Wake-up event status. Default value: 0
    (0: event not detected, 1: event detected) */
#define ASM330LHH_ALL_INT_SRC_BIT_WU_IA_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_ALL_INT_SRC_BIT_WU_IA_EVENT_DETECTED 0x02u

/* Free-fall event status. Default value: 0
    (0: event not detected, 1: event detected) */
#define ASM330LHH_ALL_INT_SRC_BIT_FF_IA_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_ALL_INT_SRC_BIT_FF_IA_EVENT_DETECTED 0x01u

/* Detects change event in activity/inactivity status. Default value: 0
    (0: change status not detected; 1: change status detected) */
#define ASM330LHH_WAKE_UP_SRC_BIT_SLEEP_CHANGE_IA_CHANGE_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_SLEEP_CHANGE_IA_CHANGE_DETECTED 0x40u

/* Free-fall event detection status. Default: 0
    (0: free-fall event not detected; 1: free-fall event detected) */
#define ASM330LHH_WAKE_UP_SRC_BIT_FF_IA_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_FF_IA_EVENT_DETECTED 0x20u

/* Sleep status bit. Default value: 0
    (0: activity status; 1: inactivity status) */
#define ASM330LHH_WAKE_UP_SRC_BIT_SLEEP_STATE_ACTIVITY 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_SLEEP_STATE_INACTIVITY 0x10u

/* Wakeup event detection status. Default value: 0
    (0: wakeup event not detected; 1: wakeup event detected.)*/
#define ASM330LHH_WAKE_UP_SRC_BIT_WU_IA_WAKEUP_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_WU_IA_WAKEUP_DETECTED 0x08u

/* Wakeup event detection status on X-axis. Default value: 0
    (0: wakeup event on X-axis not detected; 1: wakeup event on X-axis detected) */
#define ASM330LHH_WAKE_UP_SRC_BIT_X_WU_WAKEUP_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_X_WU_WAKEUP_DETECTED 0x04u

/* Wakeup event detection status on Y-axis. Default value: 0
    (0: wakeup event on Y-axis not detected; 1: wakeup event on Y-axis detected) */
#define ASM330LHH_WAKE_UP_SRC_BIT_Y_WU_WAKEUP_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_Y_WU_WAKEUP_DETECTED 0x02u

/* Wakeup event detection status on Z-axis. Default value: 0
    (0: wakeup event on Z-axis not detected; 1: wakeup event on Z-axis detected) */
#define ASM330LHH_WAKE_UP_SRC_BIT_Z_WU_WAKEUP_NOT_DETECTED 0x00u
#define ASM330LHH_WAKE_UP_SRC_BIT_Z_WU_WAKEUP_DETECTED 0x01u

/* DEN data-ready signal. It is set high when data output is related to the data coming from a
    DEN active condition.(1) */
#define ASM330LHH_D6D_SRC_BIT_DEN_DRDY_DATA_NOT_READY 0x00u
#define ASM330LHH_D6D_SRC_BIT_DEN_DRDY_DATA_READY 0x80u

/* Interrupt active for change position portrait, landscape, face-up, face-down. Default value: 0
    (0: change position not detected; 1: change position detected) */
#define ASM330LHH_D6D_SRC_BIT_D6D_IA_CHANGE_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_D6D_IA_CHANGE_DETECTED 0x40u

/* Z-axis high event (over threshold). Default value: 0
    (0: event not detected; 1: event (over threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_ZH_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_ZH_EVENT_DETECTED 0x20u

/* Z-axis low event (under threshold). Default value: 0
    (0: event not detected; 1: event (under threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_ZL_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_ZL_EVENT_DETECTED 0x10u

/* Y-axis high event (over threshold). Default value: 0
    (0: event not detected; 1: event (over-threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_YH_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_YH_EVENT_DETECTED 0x08u

/* Y-axis low event (under threshold). Default value: 0
    (0: event not detected; 1: event (under threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_YL_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_YL_EVENT_DETECTED 0x04u

/* X-axis high event (over threshold). Default value: 0
    (0: event not detected; 1: event (over threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_XH_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_XH_EVENT_DETECTED 0x02u

/* X-axis low event (under threshold). Default value: 0
    (0: event not detected; 1: event (under threshold) detected) */
#define ASM330LHH_D6D_SRC_BIT_XL_EVENT_NOT_DETECTED 0x00u
#define ASM330LHH_D6D_SRC_BIT_XL_EVENT_DETECTED 0x01u

/* Temperature new data available. Default: 0
    0: no set of data is available at temperature sensor output;
    1: a new set of data is available at temperature sensor output */
#define ASM330LHH_STATUS_REG_BIT_TDA_NO_NEW_DATA 0x00u
#define ASM330LHH_STATUS_REG_BIT_TDA_NEW_DATA_AVAILABLE 0x04u

/* Gyroscope new data available. Default value: 0
    0: no set of data available at gyroscope output;
    1: a new set of data is available at gyroscope output */
#define ASM330LHH_STATUS_REG_BIT_GDA_NO_NEW_DATA 0x00u
#define ASM330LHH_STATUS_REG_BIT_GDA_NEW_DATA_AVAILABLE 0x02u

/* Accelerometer new data available. Default value: 0
    0: no set of data available at accelerometer output;
    1: a new set of data is available at accelerometer output */
#define ASM330LHH_STATUS_REG_BIT_XLDA_NO_NEW_DATA 0x00u
#define ASM330LHH_STATUS_REG_BIT_XLDA_NEW_DATA_AVAILABLE 0x01u

/* Number of unread sensor data (TAG + 6 bytes) stored in FIFO
    In conjunction with DIFF_FIFO[9:8] in FIFO_STATUS2 (3Bh). */
#define ASM330LHH_FIFO_STATUS1_BITS_DIFF_FIFO_7_0 0xFFu

/* Number of unread sensor data (TAG + 6 bytes) stored in FIFO. Default value: 00
    In conjunction with DIFF_FIFO[7:0] in FIFO_STATUS1 (3Ah) */
#define ASM330LHH_FIFO_STATUS2_BITS_DIFF_FIFO_9_8_MASK 0x03u

/* FIFO watermark status. Default value: 0
    0: FIFO filling is lower than WTM;
    1: FIFO filling is equal to or greater than WTM)
    Watermark is set through bits WTM[8:0] in FIFO_CTRL2 (08h) and FIFO_CTRL1(07h). */
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_WTM_IA_LOWER 0x00u
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_WTM_IA_GREATER 0x80u

/* FIFO overrun status. Default value: 0
    (0: FIFO is not completely filled; 1: FIFO is completely filled) */
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_OVR_IA_NOT_FILLED 0x00u
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_OVR_IA_FILLED 0x40u

/* Smart FIFO full status. Default value: 0
    (0: FIFO is not full; 1: FIFO will be full at the next ODR) */
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_FULL_IA_NOT_FULL 0x00u
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_FULL_IA_FULL_NEXT_ODR 0x20u

/* Counter BDR reaches the CNT_BDR_TH_[9:0] threshold set in
    COUNTER_BDR_REG1 (0Bh) and COUNTER_BDR_REG2 (0Ch). Default value: 0
    This bit is reset when these registers are read. */
#define ASM330LHH_FIFO_STATUS2_BIT_COUNTER_BDR_IA_NOT_REACH 0x00u
#define ASM330LHH_FIFO_STATUS2_BIT_COUNTER_BDR_IA_REACHED 0x10u

/* Latched FIFO overrun status. Default value: 0
    This bit is reset when this register is read. */
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_OVR_NOT_LATCHED 0x00u
#define ASM330LHH_FIFO_STATUS2_BIT_FIFO_OVR_LATCHED 0x08u

/* This bit allows immediately clearing the latched interrupts of an event detection
    upon the read of the corresponding status register. It must be set to 1 together
    with LIR. Default value: 0
    (0: latched interrupt signal cleared at the end of the ODR period;
    1: latched interrupt signal immediately cleared) */
#define ASM330LHH_INT_CFG0_BIT_INT_CLR_ON_READ_DISABLE 0x00u
#define ASM330LHH_INT_CFG0_BIT_INT_CLR_ON_READ_ENABLE 0x40u

/* Activity/inactivity interrupt mode configuration.
    If INT1_SLEEP_CHANGE or INT2_SLEEP_CHANGE bits are enabled, drives
    the sleep status or sleep change on INT pins. Default value: 0
    (0: sleep change notification on INT pins; 1: sleep status reported on INT pins) */
#define ASM330LHH_INT_CFG0_BIT_SLEEP_STATUS_ON_INT_CHNAGE_NOTIFY 0x00u
#define ASM330LHH_INT_CFG0_BIT_SLEEP_STATUS_ON_INT_STATUS_REPORT 0x20u

/* HPF or SLOPE filter selection on wake-up and Activity/Inactivity functions.
    Default value: 0
    (0: SLOPE filter applied; 1: HPF applied) */
#define ASM330LHH_INT_CFG0_BIT_SLOPE_FDS_SLOPE_FILTER_APPLIED 0x00u
#define ASM330LHH_INT_CFG0_BIT_SLOPE_FDS_HPF_APPLIED 0x10u

/* Latched Interrupt. Default value: 0
    (0: interrupt request not latched; 1: interrupt request latched) */
#define ASM330LHH_INT_CFG0_BIT_LIR_NOT_LATCHED 0x00u
#define ASM330LHH_INT_CFG0_BIT_LIR_LATCHED 0x01u

/* Enables hardcoded functions */
#define ASM330LHH_INT_CFG1_BIT_INTERRUPTS_DISABLE 0x00u
#define ASM330LHH_INT_CFG1_BIT_INTERRUPTS_ENABLE 0x80u

/* Enables activity/inactivity (sleep) function. Default value: 00
    00: stationary/motion-only interrupts generated, XL and gyro do not change;
    01: sets accelerometer ODR to 12.5 Hz, gyro does not change;
    10: sets accelerometer ODR to 12.5 Hz, gyro to sleep mode;
    11: sets accelerometer ODR to 12.5 Hz, gyro to power-down mode */
#define ASM330LHH_INT_CFG1_BIT_INACT_EN_A_AND_G_NOT_CHNAGE 0x00u
#define ASM330LHH_INT_CFG1_BIT_INACT_EN_A_ODR_12_5_G_NOT_CHNAGE 0x20u
#define ASM330LHH_INT_CFG1_BIT_INACT_EN_A_ODR_12_5_G_SLEEP_MODE 0x40u
#define ASM330LHH_INT_CFG1_BIT_INACT_EN_A_ODR_12_5_G_POWER_DOWN 0x60u

/* Enables detection of 4D orientation. Z-axis position detection is disabled.
    Default value: 0 (0: disabled; 1: enabled) */
#define ASM330LHH_THS_6D_BIT_D4D_EN_DISABLE 0x00u
#define ASM330LHH_THS_6D_BIT_D4D_EN_ENABLE 0x80u

/* Threshold for 4D/6D function
    00: 80 degrees (default);
    01: 70 degrees;
    10: 60 degrees;
    11: 50 degrees */
#define ASM330LHH_THS_6D_BIT_SIXD_THS_80_DEGRESS 0x00u
#define ASM330LHH_THS_6D_BIT_SIXD_THS_70_DEGRESS 0x20u
#define ASM330LHH_THS_6D_BIT_SIXD_THS_60_DEGRESS 0x40u
#define ASM330LHH_THS_6D_BIT_SIXD_THS_50_DEGRESS 0x60u

/* Sends the low-pass filtered data with user offset correction (instead of high-pass
    filtered data) to the wakeup function. */
#define ASM330LHH_WAKE_UP_THS_BIT_DISABLE 0x00u
#define ASM330LHH_WAKE_UP_THS_BIT_ENABLE 0x40u

/* Threshold for wakeup: 1 LSB weight depends on WAKE_THS_W in
    WAKE_UP_DUR (5Ch). Default value: 000000 */
#define ASM330LHH_WAKE_UP_THS_BITS_WK_THS_DEFAULT 0x00u
#define ASM330LHH_WAKE_UP_THS_BITS_WK_THS_MASK 0x3Fu

/* Free fall duration event. Default: 0
    For the complete configuration of the free-fall duration, refer to FF_DUR[4:0] in
    FREE_FALL (5Dh) configuration.
    1 LSB = 1 ODR_time */
#define ASM330LHH_WAKE_UP_DUR_BIT_FF_DUR5_DEFAULT 0x00u
#define ASM330LHH_WAKE_UP_DUR_BIT_FF_DUR5_MASK 0x80u

/* Wake up duration event. Default: 00
    1LSB = 1 ODR_time */
#define ASM330LHH_WAKE_UP_DUR_BIT_WAKE_DUR_DEFAULT 0x00u
#define ASM330LHH_WAKE_UP_DUR_BIT_WAKE_DUR_MASK 0x60u

/* Weight of 1 LSB of wakeup threshold. Default:0
    0: 1 LSB =FS_XL / (26);
    1: 1 LSB = FS_XL / (28) */
#define ASM330LHH_WAKE_UP_DUR_BIT_WAKE_THS_W_XL_26 0x00u
#define ASM330LHH_WAKE_UP_DUR_BIT_WAKE_THS_W_XL_28 0x10u

/* Duration to go in sleep mode. Default value: 0000 (this corresponds to 16 ODR)
    1 LSB = 512 ODR */
#define ASM330LHH_WAKE_UP_DUR_BITS_SLEEP_DUR_DEFAULT 0x00u
#define ASM330LHH_WAKE_UP_DUR_BITS_SLEEP_DUR_MASK 0x0Fu

/* Free-fall duration event. Default: 0
    For the complete configuration of the free fall duration, refer to FF_DUR5 in
    WAKE_UP_DUR (5Ch) configuration */
#define ASM330LHH_FREE_FALL_BITS_FF_DUR_4_0_DEFAULT 0x00u
#define ASM330LHH_FREE_FALL_BITS_FF_DUR_4_0_MASK 0xF8u

/* Free fall threshold setting
    000: 156 mg (default);
    001: 219 mg;
    010: 250 mg;
    011: 312 mg;
    100: 344 mg;
    101: 406 mg;
    110: 469 mg;
    111: 500 mg */
#define ASM330LHH_FREE_FALL_BITS_FF_THS_156_mg 0x00u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_219_mg 0x01u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_250_mg 0x02u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_312_mg 0x03u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_344_mg 0x04u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_406_mg 0x05u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_469_mg 0x06u
#define ASM330LHH_FREE_FALL_BITS_FF_THS_500_mg 0x07u

/* Routing of activity/inactivity recognition event on INT1. Default: 0
    0: routing of activity/inactivity event on INT1 disabled;
    1: routing of activity/inactivity event on INT1 enabled */
#define ASM330LHH_MD1_CFG_BIT_INT1_SLEEP_CHANGE_DISABLE 0x00u
#define ASM330LHH_MD1_CFG_BIT_INT1_SLEEP_CHANGE_ENABLE 0x80u

/* Routing of wakeup event on INT1. Default value: 0
    0: routing of wakeup event on INT1 disabled;
    1: routing of wakeup event on INT1 enabled */
#define ASM330LHH_MD1_CFG_BIT_INT1_WU_DISABLE 0x00u
#define ASM330LHH_MD1_CFG_BIT_INT1_WU_ENABLE 0x20u

/* Routing of free-fall event on INT1. Default value: 0
    0: routing of free-fall event on INT1 disabled;
    1: routing of free-fall event on INT1 enabled */
#define ASM330LHH_MD1_CFG_BIT_INT1_FF_DISABLE 0x00u
#define ASM330LHH_MD1_CFG_BIT_INT1_FF_ENABLE 0x10u

/* Routing of 6D event on INT1. Default value: 0
    0: routing of 6D event on INT1 disabled;
    1: routing of 6D event on INT1 enabled */
#define ASM330LHH_MD1_CFG_BIT_INT1_6D_DISABLE 0x00u
#define ASM330LHH_MD1_CFG_BIT_INT1_6D_ENABLE 0x04u

/* Routing of activity/inactivity recognition event on INT2. Default: 0
    (0: routing of activity/inactivity event on INT2 disabled;
    1: routing of activity/inactivity event on INT2 enabled) */
#define ASM330LHH_MD2_CFG_BIT_INT2_SLEEP_CHANGE_DISABLE 0x00u
#define ASM330LHH_MD2_CFG_BIT_INT2_SLEEP_CHANGE_ENABLE 0x80u

/* Routing of wakeup event on INT2. Default value: 0
    0: routing of wakeup event on INT2 disabled;
    1: routing of wake-up event on INT2 enabled */
#define ASM330LHH_MD2_CFG_BIT_INT2_WU_DISABLE 0x00u
#define ASM330LHH_MD2_CFG_BIT_INT2_WU_ENABLE 0x20u

/* Routing of free-fall event on INT2. Default value: 0
    0: routing of free-fall event on INT2 disabled;
    1: routing of free-fall event on INT2 enabled */
#define ASM330LHH_MD2_CFG_BIT_INT2_FF_DISABLE 0x00u
#define ASM330LHH_MD2_CFG_BIT_INT2_FF_ENABLE 0x10u

/* Routing of 6D event on INT2. Default value: 0
    0: routing of 6D event on INT2 disabled;
    1: routing of 6D event on INT2 enabled */
#define ASM330LHH_MD2_CFG_BIT_INT2_6D_DISABLE 0x00u
#define ASM330LHH_MD2_CFG_BIT_INT2_6D_ENABLE 0x04u

/* Enables routing on INT2 pin of the alert for timestamp overflow within 6.4 ms */
#define ASM330LHH_MD2_CFG_BIT_INT2_TIMESTAMP_DISABLE 0x00u
#define ASM330LHH_MD2_CFG_BIT_INT2_TIMESTAMP_ENABLE 0x01u

/* Difference in percentage of the effective ODR (and timestamp rate) with respect
    to the typical. Step: 0.15%. 8-bit format, 2's complement. */
#define ASM330LHH_INTERNAL_FREQ_FINE_BITS_FREQ_FINE_MASK 0xFFu

/* Accelerometer X-axis user offset correction expressed in two鈥檚 complement,
    weight depends on USR_OFF_W in CTRL6_C (15h). The value must be in the
    range [-127 127]. */

#define ASM330LHH_X_OFS_USR_BIT_X_OFS_USR_DEFAULT 0x00u
#define ASM330LHH_X_OFS_USR_BIT_X_OFS_USR_MASK 0xFFu

/* Accelerometer Y-axis user offset calibration expressed in 2鈥檚 complement, weight
    depends on USR_OFF_W in CTRL6_C (15h). The value must be in the range
    [-127, +127]. */
#define ASM330LHH_Y_OFS_USR_BIT_Y_OFS_USR_DEFAULT 0x00u
#define ASM330LHH_Y_OFS_USR_BIT_Y_OFS_USR_MASK 0xFFu

/* Accelerometer Z-axis user offset calibration expressed in 2鈥檚 complement, weight
depends on USR_OFF_W in CTRL6_C (15h). The value must be in the range
[-127, +127]. */
#define ASM330LHH_Z_OFS_USR_BIT_Z_OFS_USR_DEFAULT 0x00u
#define ASM330LHH_Z_OFS_USR_BIT_Z_OFS_USR_MASK 0xFFu

/* Identifies the sensor in:
    FIFO_DATA_OUT_X_H (7Ah) and FIFO_DATA_OUT_X_L (79h),
    FIFO_DATA_OUT_Y_H (7Ch) and FIFO_DATA_OUT_Y_L (7Bh), and
    FIFO_DATA_OUT_Z_H (7Eh) and FIFO_DATA_OUT_Z_L (7Dh) 
            Table 127. FIFO tag
    TAG_SENSOR_[4:0]    Sensor name
        0x01            Gyroscope
        0x02            Accelerometer
        0x03            Temperature
        0x04            Timestamp
        0x05            CFG_Change    */
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_Gyroscope 0x08u
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_Accelerometer 0x10u
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_Temperature 0x18u
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_Timestamp 0x20u
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_CFG_Change 0x21u
#define ASM330LHH_FIFO_DATA_OUT_TAG_BITS_SENSOR_MASK 0xF8u
/* 2-bit counter which identifies sensor time slot */
#define ASM330LHH_FIFO_DATA_OUT_TAG_BIT_TAG_CNT_MASK 0x06u
/* Parity check of TAG content */
#define ASM330LHH_FIFO_DATA_OUT_TAG_BIT_TAG_PARITY_MASK 0x01u

#define ASM330LHH_FIFO_DATA_OUT_X_H_BIT_SHIFT 8u
#define ASM330LHH_FIFO_DATA_OUT_Y_H_BIT_SHIFT 8u
#define ASM330LHH_FIFO_DATA_OUT_Z_H_BIT_SHIFT 8u


/*=======[T Y P E   D E F I N I T I O N S]====================================*/

typedef struct
{
    /* Alerts timestamp overflow within 6.4 ms */
    boolean TIMESTAMP_ENDCOUNT;
    /* Detects change event in activity/inactivity status. Default value: 0
        (0: change status not detected; 1: change status detected) */
    boolean SLEEP_CHANGE_IA;
    /* Interrupt active for change in position of portrait, landscape, face-up,
        face-down. Default value: 0
        (0: change in position not detected; 1: change in position detected) */
    boolean D6D_IA;
    /* Wake-up event status. Default value: 0
        (0: event not detected, 1: event detected) */
    boolean WU_IA;
    /* Free-fall event status. Default value: 0
        (0: event not detected, 1: event detected) */
    boolean FF_IA;
}ASM330LHH_ALL_INT_SRC_TYPE;

typedef struct
{
    /* Detects change event in activity/inactivity status. Default value: 0
        (0: change status not detected; 1: change status detected) */
    boolean SLEEP_CHANGE_IA;
    /* Free-fall event detection status. Default: 0
        (0: free-fall event not detected; 1: free-fall event detected) */
    boolean FF_IA;
    /* Sleep status bit. Default value: 0
        (0: activity status; 1: inactivity status) */
    boolean SLEEP_STATE;
    /* Wakeup event detection status. Default value: 0
        (0: wakeup event not detected; 1: wakeup event detected.) */
    boolean WU_IA;
    /* Wakeup event detection status on X-axis. Default value: 0
        (0: wakeup event on X-axis not detected; 1: wakeup event on X-axis detected) */
    boolean X_WU;
    /* Wakeup event detection status on Y-axis. Default value: 0
        (0: wakeup event on Y-axis not detected; 1: wakeup event on Y-axis detected) */
    boolean Y_WU;
    /* Wakeup event detection status on Z-axis. Default value: 0
        (0: wakeup event on Z-axis not detected; 1: wakeup event on Z-axis detected) */
    boolean Z_WU;
}ASM330LHH_WAKE_UP_SRC_TYPE;

typedef struct
{
    /* DEN data-ready signal. It is set high when data output is related to the data coming from a
        DEN active condition. */
    boolean DEN_DRDY;
    /* Interrupt active for change position portrait, landscape, face-up, face-down. Default value: 0
        (0: change position not detected; 1: change position detected) */
    boolean D6D_IA;
    /* Z-axis high event (over threshold). Default value: 0
        (0: event not detected; 1: event (over threshold) detected) */
    boolean ZH;
    /* Z-axis low event (under threshold). Default value: 0
        (0: event not detected; 1: event (under threshold) detected) */
    boolean ZL;
    /* Y-axis high event (over threshold). Default value: 0
        (0: event not detected; 1: event (over-threshold) detected) */
    boolean YH;
    /* Y-axis low event (under threshold). Default value: 0
        (0: event not detected; 1: event (under threshold) detected) */
    boolean YL;
    /* X-axis high event (over threshold). Default value: 0
        (0: event not detected; 1: event (over threshold) detected) */
    boolean XH;
    /* X-axis low event (under threshold). Default value: 0
        (0: event not detected; 1: event (under threshold) detected) */
    boolean XL;
}ASM330LHH_D6D_SRC_TYPE;

typedef struct
{
    /* Temperature new data available. Default: 0
        0: no set of data is available at temperature sensor output;
        1: a new set of data is available at temperature sensor output */
    boolean TDA;
    /* Gyroscope new data available. Default value: 0
        0: no set of data available at gyroscope output;
        1: a new set of data is available at gyroscope output */
    boolean GDA;
    /* Accelerometer new data available. Default value: 0
        0: no set of data available at accelerometer output;
        1: a new set of data is available at accelerometer output */
    boolean XLDA;
}ASM330LHH_STATUS_REG_TYPE;

typedef struct 
{
    uint16 Temp;/* Temperature sensor output data
                    The value is expressed as two鈥檚 complement sign extended on the MSB. */
} ASM330LHH_OUT_TEMP_TYPE;

typedef struct
{
    uint16 X;/* Gyroscope pitch axis output expressed in 2鈥檚 complement */
    uint16 Y;/* Gyroscope roll axis output expressed in 2鈥檚 complement */
    uint16 Z;/* Gyroscope yaw axis output expressed in 2鈥檚 complement */
}ASM330LHH_OUTDATA_G_TYPE;

typedef struct
{
    uint16 X;/* Accelerometer X-axis output expressed as 2鈥檚 complement */
    uint16 Y;/* Accelerometer Y-axis output expressed as 2鈥檚 complement */
    uint16 Z;/* Accelerometer Z-axis output expressed as 2鈥檚 complement */
}ASM330LHH_OUTDATA_A_TYPE;

typedef struct
{
    /* Number of unread sensor data (TAG + 6 bytes) stored in FIFO. Default value: 00
        In conjunction with DIFF_FIFO[7:0] in FIFO_STATUS1 (3Ah) */
    /* Number of unread sensor data (TAG + 6 bytes) stored in FIFO
        In conjunction with DIFF_FIFO[9:8] in FIFO_STATUS2 (3Bh). */
    uint16 DIFF_FIFO;
    /* FIFO watermark status. Default value: 0
        0: FIFO filling is lower than WTM;
        1: FIFO filling is equal to or greater than WTM
        Watermark is set through bits WTM[8:0] in FIFO_CTRL2 (08h) and FIFO_CTRL1
        (07h). */
    boolean FIFO_WTM_IA;
    /* FIFO overrun status. Default value: 0
        (0: FIFO is not completely filled; 1: FIFO is completely filled) */
    boolean FIFO_OVR_IA;
    /* Smart FIFO full status. Default value: 0
        (0: FIFO is not full; 1: FIFO will be full at the next ODR) */
    boolean FIFO_FULL_IA;
    /* Counter BDR reaches the CNT_BDR_TH_[9:0] threshold set in
        COUNTER_BDR_REG1 (0Bh) and COUNTER_BDR_REG2 (0Ch). Default value: 0
        This bit is reset when these registers are read. */
    boolean COUNTER_BDR_IA;
    /* Latched FIFO overrun status. Default value: 0
        This bit is reset when this register is read. */
    boolean FIFO_OVR_LATCHED;
}ASM330LHH_FIFO_STATUS_TYPE;

typedef struct
{
    uint32 D;/* Timestamp output registers: 1LSB = 25 渭s */
}ASM330LHH_TIMESTAMP_TYPE;

typedef struct
{
    uint8 FREQ_FINE;/* Difference in percentage of the effective ODR (and timestamp rate) with respect
                        to the typical. Step: 0.15%. 8-bit format, 2's complement. */
}ASM330LHH_INTERNAL_FREQ_FINE_TYPE;

typedef struct
{
    uint8 TAG_SENSOR;/* Identifies the sensor  */
    uint8 TAG_CNT;/* 2-bit counter which identifies sensor time slot */
    boolean TAG_PARITY;/* Parity check of TAG content */
}ASM330LHH_FIFO_DATA_OUT_TAG_TYPE;

typedef struct
{
    uint8 TAG;
    /* FIFO X-axis output */
    uint16 X;
    /* FIFO Y-axis output */
    uint16 Y;
    /* FIFO Z-axis output */
    uint16 Z;
}ASM330LHH_FIFO_DATA_OUT_TYPE;


/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/


#endif /* asm330lhh_TYPES_H */

/*=======[E N D   O F   F I L E]==============================================*/

