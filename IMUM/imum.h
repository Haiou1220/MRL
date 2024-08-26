/******************************************************************************/
/**
* \file       imum.h
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

#ifndef IMUM_H
#define IMUM_H

#include "Platform_Types.h"

extern uint8 PowerOnDelayCnt;
extern boolean is_initialized; /* imu already initialized */
extern boolean is_reset_set; /* TRUE: already reset during initialization */
extern int32_t imu_state; 
extern uint32_t imu_data_cnt; /* read imu data count */
extern int32_t imu_error_code; /* default 0 without error*/

extern sint16 gyro_data[3]; /* Gyroscope data (full-scale selection: ±2000 dps )*/
extern sint16 acc_data[3]; /* Accelerometer data (full-scale selection: ±2g)*/

void IMU_Manager(void); 
void IMU_EndNotification(void);

#endif


/* _____ E N D _____ (imum.h) ________________________________________________*/
