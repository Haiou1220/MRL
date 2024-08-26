/******************************************************************************/
/**
* \file       spic_cfg.h
* \brief      
* \details    
*             
* \author     H.Huisheng
* \date       1/6/2020
* \par        History:
* 
\verbatim
  Version     Author                    Date            Desc   
  1.0         H.Huisheng                1/6/2020
\endverbatim  
*
*/
/**************** (C) Copyright 2018 Magneti Marelli Guangzhou ****************/

#ifndef SPIC_CFG_H
#define SPIC_CFG_H
/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "Platform_Types.h"

/* _____ G L O B A L - D E F I N E ___________________________________________*/

/* _____ G L O B A L - T Y P E S _____________________________________________*/
typedef struct
{
  uint16 transferCount;
  void*   sendBuff;
  void*   receBuff;
}SPIC_Reqtype_t;

/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ G L O B A L - M A C R O S ___________________________________________*/

/* _____ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/

#endif /* SPIC_CFG_H */

/* _____ E N D _____ (spic_cfg.h) ____________________________________________*/


