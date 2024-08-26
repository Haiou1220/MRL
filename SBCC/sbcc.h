/******************************************************************************/
/**
* \file       sbcc.h
* \brief      SBC(L99DZ100G) Control Module public header    
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

#ifndef SBCC_H
#define SBCC_H
/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "sbcc_config.h"
#include "sbcc_dynamic.h"

/* _____ G L O B A L - D E F I N E ___________________________________________*/
#define SBCC_DATA_LEN 2
#define SBCC_TX_DMA_CHN    DMAC_CHANNEL_4
#define SBCC_RX_DMA_CHN    DMAC_CHANNEL_5
#define SBCC_TIMER         GPTC_TAUJ1_0

#define SBCC_ERROR_INVALID        0U
#define SBCC_ERROR_NONE           1U
#define SBCC_ERROR_OPEN_LOAD      2U
#define SBCC_ERROR_OVER_CURRENT   3U
#define SBCC_ERROR_OCR_ALERT      4U

#define SBCC_TEMP_CL1             0U
#define SBCC_TEMP_CL2             1U
#define SBCC_TEMP_CL3             2U
#define SBCC_TEMP_CL4             3U
#define SBCC_TEMP_CL5             4U
#define SBCC_TEMP_CL6             5U
#define SBCC_TEMP_CL_TOTAL        6U
#define SBCC_TEMP_CL_INVALID      0xFFU

#define WD_CFG_IS_TWS1        (0U)
#define WD_CFG_IS_TWS2        (1U)
#define WD_CFG_IS_TWS3        (2U)
#define WD_CFG_IS_TWS4        (3U)
/* _____ G L O B A L - T Y P E S _____________________________________________*/
typedef enum
{
  SBCC_OutputLow = 0,
  SBCC_OutputHigh,
  SBCC_DlcPwrRelease,
  SBCC_DlcPwrReset,
  SBCC_DlcPwrCrash,
  SBCC_DlcPwrPclLock,
  SBCC_DlcPwrPclUnlock,
  SBCC_DlcPwrBreakOn,
  SBCC_DlcPwrIDLE,
  SBCC_OutputOff
}SBCC_OutputState_t;

typedef enum
{
  SBCC_FWL_Down = 0,
  SBCC_FWL_Up = 1,
  SBCC_FWL_Off = 2
}SBCC_FWLState_t;

typedef enum
{
   ERROR_LOG_GLOBAL = 0,
   ERROR_LOG_SR1,
   ERROR_LOG_SR2,
   ERROR_LOG_SR3,
   ERROR_LOG_SR4,
   ERROR_LOG_SR5,
   ERROR_LOG_SR6,
   ERROR_LOG_SR12,
   ERROR_LOG_NUN,
}SBCC_ErrorLog_Type_t;
/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ G L O B A L - M A C R O S ___________________________________________*/
#if 0
#define SBCC_HW_TEST
#endif

/* _____ G L O B A L - F U N C T I O N S - P R O T O T Y P E S _______________*/
extern void SBCC_Init(void);
extern void SBCC_WakeUp(void);
extern void SBCC_Sleep(void);
extern void SBCC_Manager(void);
extern boolean SBCC_CanTransceiverEnableDisable(boolean Status);
extern void SBCC_TxCompleteCallback(void);
extern void SBCC_SetOutputType(uint8 Channel, Sbcc_OutputType_t Type);
extern Sbcc_OutputType_t SBCC_GetOutputType(uint8 Channel);
extern boolean SBCC_SwitchOnOff(uint8 Channel, boolean Status);
extern boolean SBCC_SwitchOffButKeepRunSt(uint8 Channel);
#if 0
extern uint16 SBCC_GetVSInputValue(void);
#endif
extern uint8 SBCC_GetErrorPresent(uint8 Channel);
extern boolean SBCC_HasValidFeedback(uint8 Channel);
/*extern void SBCC_EnableCanCommunicationAfterBusOff(void);*/
extern void SBCC_RefreshWD(void);
extern void SBCC_ToggleWD(boolean * Value);
extern void SBCC_GoToStandByMode(void);
extern void SBCC_OutHs(boolean flag);
extern void SBCC_WuConfigure(boolean value);
extern void SBCC_ClearVporFlag(void);
extern boolean SBCC_GetVporFlag(void);
extern boolean SBCC_SpiCommunicationEstablished(void);
extern boolean SBCC_SetPwmDuty(uint8 channel, uint16 duty);
extern uint8 SBCC_GetInstantError(uint8 channel);
extern uint32 SBCC_GetSR1Val(void);
extern uint32 SBCC_GetSR2Val(void);
extern uint32 SBCC_GetSR3Val(void);
extern uint32 SBCC_GetSR4Val(void);
extern uint32 SBCC_GetSR5Val(void);
extern uint32 SBCC_GetSR6Val(void);
extern uint32 SBCC_GetSR7Val(void);
extern uint32 SBCC_GetSR8Val(void);
extern uint32 SBCC_GetSR9Val(void);
extern uint32 SBCC_GetSR10Val(void);
extern uint32 SBCC_GetSR11Val(void);
extern uint32 SBCC_GetSR12Val(void);
extern uint32 SBCC_GetStateError(SBCC_ErrorLog_Type_t channel);
extern boolean SBCC_CheckCANSystemError(void);
extern void SBCC_SetGH_OL(boolean st);
extern boolean SBCC_GetHeatOpenLoadSetting(void);
extern boolean SBCC_Is5V2OverLoad(void);
extern void SBCC_Clear5V2OverLoad(void);
extern boolean  SBCC_GetTwFlag(uint8 Tw_cluster);
extern void SBCC_ClearWarningBit(uint8 Tw_cluster);
extern sint16 SBCC_GetClusterTemperature(uint8 clusterId);
extern void SBCC_SuspendOCR(uint8 channel);
extern void SBCC_ResumeOCR(uint8 channel);
extern void SBCC_CurrentMonitorEn(uint8 Channel, boolean Enable);
extern void SBCC_Set5V2State(boolean st);
#if 0
extern boolean SBCC_GetWakedUpByCanFlag(void);
extern void SBCC_ClearWakedUpByCanFlag(boolean flag);
#endif
extern uint32 SBCC_ReadRegisterValue(uint8 Channel);
extern void SBCC_ClearWakeupFlag(void);
#if 0
extern boolean SBCC_GetWakeupFlag(void);
#endif
extern 	boolean SBCC_GetCurrentVporFlag(void);
extern 	boolean SBCC_GetCurrentWakeupFlag(void);
extern boolean SBCC_GetWakeupFlag(void);
extern boolean SBCC_GetWDFailFlag(void);
/*ECG function*/
void SBCC_SetEcFunctionON(void);
void SBCC_SetEcFunctionOFF(void);
void SBCC_SetEcVolValue(uint8 value);
void SBCC_EcFastDischargeOn(void);
void SBCC_EcFastDischargeOFF(void);
void SBCC_EcOcrON(void);
void SBCC_EcOcrOFF(void);
extern void SBCC_WDResetHandler(uint8 cfg);
extern boolean SBCC_GetWDFailFlag(void);
extern boolean SBCC_GetLinShortToGndErr(void);
extern boolean SBCC_CheckCommunicition(void);
extern void Sbcc_SendCmdHandler(void);
#endif /* SBCC_H */

/* _____ E N D _____ (sbcc.h) ________________________________________________*/


