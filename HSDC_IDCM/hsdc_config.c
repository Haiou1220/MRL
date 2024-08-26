/***********************************************************************************************************/
/**
* \file       hsdc_config.c
* \brief      High-side driver control
* \details    N/A
*
* \author     lub
* \date       2023-07-03
* \par        History:
*
*/
/********************* (C) Copyright 2023 Magneti Marelli Guangzhou ****************************************/

/*____________________________ I N C L U D E - F I L E S __________________________________________________*/
#include "hsdc.h"
#include "spic.h"

/*____________________________ L O C A L - M A C R O S ____________________________________________________*/ 


/*____________________________ L O C A L - T Y P E S ______________________________________________________*/


/*____________________________ L O C A L - S T R U C T U R E ______________________________________________*/


/*____________________________ G L O B A L - D A T A ______________________________________________________*/
Std_ReturnType Hsdc_SpiTransmitFrame(SPIC_Reqtype_t* req)
{
  Std_ReturnType Ret = E_NOT_OK;

  Ret = Spi_SetupEB(HSDC_SPI_CHANNEL, req->sendBuff, req->receBuff, req->transferCount);
  if(E_OK == Ret)
  {
    Ret = Spi_AsyncTransmit(HSDC_SPI_SEQUENCE);
  }
  return Ret;
}

/*______ E N D _____ (hsdc_config.c) ____________________________________________*/

