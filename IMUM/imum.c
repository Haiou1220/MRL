/******************************************************************************/
/**
* \file       imum.c
* \brief      
* \details    
*             
* \author     Hongming.ZHENGH
* \date       09/05/2023
* \par        History:
* 
\verbatim
  Version     Author                    Date            Desc   
  1.0         Hongming.ZHENG           09/05/2023
\endverbatim  
*
*/
/**************** (C) Copyright 2023 Magneti Marelli Guangzhou ****************/

/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "asm330lhh_cfg.h"
#include "asm330lhm_reg.h"
#include "asm330lhh_types.h"
#include "asm330lhh_io.h"
#include "asm330lhm_reg.h"
#include "imum.h"
#include "Rte_CtCdd_IMU.h"

/* _____ L O C A L - F U N C T I O N S _____________________________________*/
int32_t ctx_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len) {
  for(uint16_t i = 0; i<len; i++) {
    if (asm330lhh_WriteCtrlReg(reg+i, *(data+(uint8)i)) == FALSE)
      return 1;
  }
  return 0;
}
int32_t ctx_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len) {
  for(uint16_t i = 0; i<len; i++) {
    if (asm330lhh_IO_SpiRead(reg+i, data+i) == FALSE)
      return 1;
  }
  return 0;
}

 /* _____ G L O B A L - D A T A ___________________________________________*/
sint16 gyro_data[3] = {0};
sint16 acc_data[3] = {0};


 /* _____ L O C A L - D A T A ___________________________________________*/
stmdev_ctx_t dev_ctx = {.write_reg=ctx_write, .read_reg=ctx_read,.mdelay=0,.handle=0};
const uint16 INITIALIZED_DELAY_COUNT = 30;
uint8 PowerOnDelayCnt = ASM330LHH_PowerON_DELAY_TIME;
boolean is_initialized = FALSE;
boolean is_reset_set = FALSE;
boolean is_initialized_delayed = FALSE;
int32_t imu_state = 0;
uint32_t loop_cnt = 0;
uint32_t imu_data_cnt = 0;
int32_t imu_error_code = 0;
uint16 initialized_delay_count = 0;


/******************************************************************************/
/** 
* \brief      IMU Manger
* 
* \author     Hongming.ZHENG
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void IMU_Manager(void)
{
  loop_cnt = loop_cnt + 1;
  
  if(PowerOnDelayCnt > 0u)
  {
    PowerOnDelayCnt--;
    imu_state = -1;
  }
  else
  {
    uint8_t reg;
    if (is_initialized == FALSE) {
      uint8_t whoamI, rst;
      imu_error_code =  asm330lhm_device_id_get(&dev_ctx, &whoamI);
      // /* Restore default configuration */
      if (whoamI == ASM330LHM_ID && imu_error_code == 0) {
        imu_state = 1;
        if (is_reset_set == FALSE) {
          imu_state = 2;
          if (asm330lhm_reset_set(&dev_ctx, PROPERTY_ENABLE) == 0) {
              imu_state = 3;
              do {
                imu_state = 4;
                asm330lhm_reset_get(&dev_ctx, &rst);
              } while (rst);
              imu_state = 5;
              is_reset_set = TRUE;
          }
        }
        imu_state = 6;
            
        /* Start device configuration. */
        imu_error_code += asm330lhm_device_conf_set(&dev_ctx, PROPERTY_ENABLE);
        /* Enable Block Data Update */
        imu_error_code += asm330lhm_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
        /* Set Output Data Rate */
        imu_error_code += asm330lhm_xl_data_rate_set(&dev_ctx, ASM330LHM_XL_ODR_208Hz);
        imu_error_code += asm330lhm_gy_data_rate_set(&dev_ctx, ASM330LHM_GY_ODR_208Hz);
        /* Set full scale */
        imu_error_code += asm330lhm_xl_full_scale_set(&dev_ctx, ASM330LHM_2g);
        imu_error_code += asm330lhm_gy_full_scale_set(&dev_ctx, ASM330LHM_2000dps);
        /* Configure filtering chain(No aux interface)
        * Accelerometer - LPF1 + LPF2 path
        */
        imu_error_code += asm330lhm_xl_hp_path_on_out_set(&dev_ctx, ASM330LHM_LP_ODR_DIV_100);
        imu_error_code += asm330lhm_xl_filter_lp2_set(&dev_ctx, PROPERTY_ENABLE);
        if (imu_error_code == 0)
          is_initialized = TRUE;
      }
    }

    if ((FALSE == is_initialized_delayed) && (TRUE == is_initialized))
    {
      if (initialized_delay_count++ > INITIALIZED_DELAY_COUNT)
      {
        is_initialized_delayed = TRUE;
      }
    }
    
    if (is_initialized && is_initialized_delayed) {
      imu_state = 7;
      /* Whether new data available */
      imu_error_code = asm330lhm_xl_flag_data_ready_get(&dev_ctx, &reg);
      if (reg) {
        imu_state = 8;
        imu_data_cnt = imu_data_cnt + 1;
        imu_error_code = asm330lhm_angular_rate_raw_get(&dev_ctx, gyro_data);
        imu_error_code += asm330lhm_acceleration_raw_get(&dev_ctx, acc_data);

        Rte_Write_CpCdd_IMUXaxisAngSpd_state_s16(gyro_data[0]);
        Rte_Write_CpCdd_IMUYaxisAngSpd_state_s16(gyro_data[1]);
        Rte_Write_CpCdd_IMUZaxisAngSpd_state_s16(gyro_data[2]);

        Rte_Write_CpCdd_IMUXaxisAcc_state_s16(acc_data[0]);
        Rte_Write_CpCdd_IMUYaxisAcc_state_s16(acc_data[1]);
        Rte_Write_CpCdd_IMUZaxisAcc_state_s16(acc_data[2]);
		
        
      }
    }
  }
}

void IMU_EndNotification() {}

/* _____E N D _____ (imum.c) _________________________________________________*/
