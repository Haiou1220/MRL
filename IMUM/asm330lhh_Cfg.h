/******************************************************************************/
/**
* \file       asm330lhh_Cfg.h
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

#ifndef ASM330LHH_CFG_H
#define ASM330LHH_CFG_H

/*=======[I N C L U D E S]====================================================*/
#include "Port_Cfg.h"
#include "asm330lhh_types.h"

/*=======[M A C R O S]========================================================*/

#define ASM330LHH_PowerON_DELAY_TIME 5u/* unit: ms */

/* ASM330LHH task tick */
#define ASM330LHH_TICK 5u/* unit: ms */

#define IMU_MCU_INT1_DioChannel PortConfigSet_PTD_PTD17_GPIO_IMU_INT1
#define IMU_MCU_INT2_DioChannel PortConfigSet_PTD_PTD15_GPIO_IMU_INT2


/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/

#endif /* ASM330LHH_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/


