/***********************************************************************************************************/
/**
* \file       hsdc.h
* \brief      High-side driver control
* \details    N/A
*
* \author     lub
* \date       2023-07-03
* \par        History:
*
*/
/********************* (C) Copyright 2023 Magneti Marelli Guangzhou ****************************************/

#ifndef HSDC_H
#define HSDC_H


/*______ I N C L U D E - F I L E S ___________________________________________*/
#include "Std_Types.h"
#include "hsdc_priv.h"
#include "spic.h"
/*______ G L O B A L - D E F I N E S _________________________________________*/
#define HSDC_SPI_CHANNEL              SpiConf_SpiChannel_SpiChannel_4_BTS71033
#define HSDC_SPI_SEQUENCE             SpiConf_SpiSequence_SpiSequence_4_BTS71033

#define HSDC_E_OK                     0u
#define HSDC_E_NOT_OK                 1u

#define HSDC_READ_DIAG_TIMER          10u
#define HSDC_READ_ALL_Count           10u
#define HSDC_RETRY_TIMER              7u
#define HSDC_RESTART_DELAY_FAST       10u
#define HSDC_RESTART_DELAY_SLOW       100u
#define HSDC_RESTART_COUNT            5u

#define HSDC_OVERLOAD_LOW_LEVEL       STD_OFF
/*______ P R I V A T E - D E F I N E S _______________________________________*/

/*______ G L O B A L - T Y P E _______________________________________________*/
typedef enum{
  HSDC_E_CLOSE = 0u,
  HSDC_E_OPEN = 1u
}Hsdc_OutputControl_t;

typedef enum{
  OUT_HSDC_CHANGEL_NULL,
  OUT_HSDC_DOOR_AMBINET_LAMP,
  OUT_HSDC_DOOR_PANEL_HEAT,
  OUT_HSDC_RESERVED_LED_2,
  OUT_HSDC_WC_HALL_PWR,
  OUT_PWR_DOOR_HALL_PWR,
  OUT_CHANNEL_MAX
}Hsdc_OutputChannel_t;
/*______ G L O B A L - D A T A _______________________________________________*/

/*______ G L O B A L - M A C R O S ___________________________________________*/

/*______ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/
extern void Hsdc_Init(void);
extern void Hsdc_MainFunction_10ms(void);
extern void Hsdc_EndNotification(void);
extern void Hsdc_DisAutoRestart(Hsdc_RCD_Channel_t channel);
extern void Hsdc_SetOutputChannel(Hsdc_OutputChannel_t chn, Hsdc_OutputControl_t sta);
extern uint8 Hsdc_GetOutputChannel(void);
extern uint8 Hsdc_GetWarnChannel(void);
extern uint8 Hsdc_GetErrorChannel(void);
extern boolean Hsdc_GetStdTransmissionError(void);
extern boolean Hsdc_GetStdChecksumVerification(void);
extern boolean Hsdc_GetStdLimpHomeMonitor(void);
extern boolean Hsdc_GetStdSleepModeMonitor(void);
extern boolean Hsdc_GetStdVsMonitor(void);

Std_ReturnType Hsdc_SpiTransmitFrame(SPIC_Reqtype_t* req);
#endif /* HSDC_H */

/*______ E N D _____ (hsdc.h) _________________________________________*/
