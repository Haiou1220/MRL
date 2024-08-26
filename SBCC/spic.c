/******************************************************************************/
/**
* \file       spic.c
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

/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "Platform_Types.h"
#include "spic.h"
#include "sbcc_config.h"

/* _____ L O C A L - D E F I N E _____________________________________________*/

/* _____ L O C A L - T Y P E S _______________________________________________*/

/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ L O C A L - D A T A _________________________________________________*/

/* _____ L O C A L - M A C R O S _____________________________________________*/

/* _____ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/

/* _____ G L O B A L - F U N C T I O N S _____________________________________*/
/******************************************************************************/
/** 
* \brief      SPIC_Master_Send
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
Std_ReturnType SPIC_Master_Send(SPIC_Reqtype_t* req)
{
  Std_ReturnType Ret = E_NOT_OK;

#if SBCC_SPI_4_BYTES_ENABLE
	Ret = Spi_SetupEB(SBCC_SPI_CHANNEL, req->sendBuff, req->receBuff, req->transferCount*SBCC_SPI_4_BYTES);
#else
	Ret = Spi_SetupEB(SBCC_SPI_CHANNEL, req->sendBuff, req->receBuff, req->transferCount);
#endif
	if(E_OK == Ret)
	{
		Ret = Spi_SyncTransmit(SBCC_SPI_CHANNEL);
	}
	return Ret;
}

/* _____ L O C A L - F U N C T I O N S _______________________________________*/

/* _____E N D _____ (spic.c) _________________________________________________*/


