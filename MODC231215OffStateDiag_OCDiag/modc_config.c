/***********************************************************************************************************/
/**
* \file       modc_config.c
* \brief      H/Half-bridge Motor driver control
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

/*____________________________ I N C L U D E - F I L E S __________________________________________________*/

#include "modc_config.h"
#include "modc.h"
#include "modc_priv.h"
#include "spic.h"
#include "Dio.h"
#include "syst.h"
/*____________________________ L O C A L - M A C R O S ____________________________________________________*/ 


/*____________________________ L O C A L - T Y P E S ______________________________________________________*/


/*____________________________ L O C A L - S T R U C T U R E ______________________________________________*/


/*____________________________ G L O B A L - D A T A ______________________________________________________*/
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
void Modc_WakeUpDevice(uint8 state)
{
#if defined(DCU_FL) || defined(DCU_FR)
	Dio_WriteChannel(DioConf_DioChannel_C_PRE_EN,state);
#else
  Dio_WriteChannel(DioConf_DioChannel_C_PRE1_EN,state);
  Dio_WriteChannel(DioConf_DioChannel_C_PRE2_EN,state);
#endif
}

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
uint8 Modc_GetDeviceWakeUpState(void)
{
  return 0;/*(uint8)Dio_ReadChannel(DioConf_DioChannel_C_NSLEEP);*/
}

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
void Modc_SpiDataTransmitStart(void)
{
  /* set the pin low to select the chip */
  Dio_WriteChannel(DioConf_DioChannel_SPI2_CS,FALSE);
  //SYST_Wait(SYST_2_5uS);
}

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
void Modc_SpiDataTransmitStop(void)
{
  /* set the pin high to disable the chip */
  Dio_WriteChannel(DioConf_DioChannel_SPI2_CS,TRUE);
  SYST_Wait(SYST_5uS);
}

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
Spi_SeqResultType Modc_SpiGetSequenceResult(uint8 Modc_SpiChannelSequence)
{
  return Spi_GetSequenceResult(Modc_SpiChannelSequence);
}

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
Std_ReturnType Modc_SpiTransmitFrame(uint8 Modc_SpiConf_SpiChannel, SPIC_Reqtype_t* req)
{
  Std_ReturnType Ret = E_NOT_OK;

  Ret = Spi_SetupEB(Modc_SpiConf_SpiChannel, req->sendBuff,req->receBuff, req->transferCount);
  if(E_OK == Ret)
  {

    Ret = Spi_SyncTransmit(Modc_SpiConf_SpiChannel);
    /*Ret = Spi_AsyncTransmit(Modc_SpiConf_SpiChannel);*/

  }

  return Ret;
}

/*______ E N D _____ (predc_config.c) ____________________________________________*/

