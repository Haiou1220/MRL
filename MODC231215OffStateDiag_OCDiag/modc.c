/***********************************************************************************************************/
/**
* \file       modc.c
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
#include <string.h>
#include "modc.h"
#include "modc_config.h"
#include "modc_priv.h"
#include "spic.h"
#include "Pwm.h"

/*____________________________ L O C A L - D E F I N E S __________________________________________________*/
/*  #define MODC_DEBUG_DIAG_APP_INTERFACE */ /* debug */
/* #define MODC_DEBUG_HBCTL_REGVIEW  */ /* debug */
/* #define DCU_FL  */ /* NEED TO DEL */

/* MODC main state definition*/
#define  Modc_STATE_SLEEP                  0U
#define  Modc_STATE_PRE_SLEEP              1U
#define  Modc_STATE_WAKEUP_DEVICE          2U
#define  Modc_STATE_READ_STATE_REGISTER    3U
#define  Modc_STATE_DETECT_DEVICE          4U
#define  Modc_STATE_INIT_REGISTER          5U
#define  Modc_STATE_OOL_OSC_STATUS         6U
#define  Modc_STATE_READ_STATUS            7U
#define  Modc_STATE_ERROR                  8U
#define  Modc_STATE_RESET_DEVICE           9U

/* MODC device diagnosis state definition*/
#define  Modc_DIAG_STATE_ERR_FREE         0U /* No error*/
#define  Modc_DIAG_STATE_ERR_TBC          1U /* Error detected, not confirmed yet*/
#define  Modc_DIAG_STATE_ERR_TSD          2U /* In thermal protection state*/
#define  Modc_DIAG_STATE_ERR_WDT          3U /* In Watch Dog Fault state*/
#define  Modc_DIAG_STATE_ERR_VS_OV_UV     4U /* In VS Undervoltage state*/
#define  Modc_DIAG_STATE_ERR_CPUV         5U /* In CP Undervoltage state*/
#define  Modc_DIAG_STATE_ERR_OC           6U /* In over-current protection state*/
#define  Modc_DIAG_STATE_ERR_VGS          7U /* In gate driver fault state*/
#define  Modc_DIAG_STATE_ERR              8U /* error that need re-start device*/
#define  Modc_DIAG_STATE_WARN             9U /* error that don't need re-start device*/

#define  Modc_DIAG_STATE_RECOVERY         10U /* In recovery state*/
#define  Modc_DIAG_STATE_RECOVERY_REQ_OK  11U /* In RECOVERY_REQ state*/
#define  Modc_DIAG_STATE_SUSPEND          12U /* In suspend state*/


#define BYTE_BITS_LEN                (8U)
#define FAULT_REG_INIT_VALUE         (0xFFU)
#define MODC_IC_STATE1_SPI_OK        (0x80U)
#define MODC_DEV_ID                  MODC_DEV_ID_TLE92108_232
#define MODC_SPI_DBT_VALUE		       (0x5U) /*delay between transfers*/
#define MODC_SPI_DSP_VALUE		       (0x01U) /*delay SCK to PCS*/
#define MODC_SPI_DPS_VALUE		       (0x01U) /*delay PCS to SCK*/

#define MODC_SPI_WAKE_FILTER_CNT    (4U)

#define MODC_SPI_ERROR_COUNTER      (20U)

#define MODC_TxQueueSizeChan         MODC_HB_MAX

#if (defined(DCU_FL) || defined(DCU_FR))/*Front*/
#define Modc_ChipOneMotorTotal  5U
#define Modc_ChipTwoMotorTotal  5U
#else/* Rear */
#define Modc_ChipOneMotorTotal 5U
#define Modc_ChipTwoMotorTotal 2U
#endif

#define Modc_ChipOfMotorMaxTotal  (Modc_ChipOneMotorTotal>Modc_ChipTwoMotorTotal)?(Modc_ChipOneMotorTotal):(Modc_ChipTwoMotorTotal)

#if (defined(DCU_FL) || defined(DCU_FR))/* Front TLE92108 & TLE92108*/
static Modc_MotorFullBridgeComp_t Modc_ChipOfMotorHBComp[Modc_SLAVE_DEVICE_NUM][Modc_ChipOfMotorMaxTotal] = {\
{/*for device PRE1*/
{MODC_PRE1Bridge_ONE,MODC_PRE1Bridge_TWO},
{MODC_PRE1Bridge_THREE,MODC_PRE1Bridge_FOUR},
{MODC_PRE1Bridge_FIVE,MODC_PRE1Bridge_FOUR},
{MODC_PRE1Bridge_SIX,MODC_PRE1Bridge_SEVEN},
{MODC_PRE1Bridge_EIGHT,MODC_PRE1Bridge_SEVEN}
},
{/*for device PRE2*/
{MODC_PRE2Bridge_ONE,MODC_PRE2Bridge_TWO},
{MODC_PRE2Bridge_THREE,MODC_PRE2Bridge_FOUR},
{MODC_PRE2Bridge_FIVE,MODC_PRE2Bridge_FOUR},
{MODC_PRE2Bridge_SIX,MODC_PRE2Bridge_EIGHT},
{MODC_PRE2Bridge_SEVEN,MODC_PRE2Bridge_EIGHT}
}
};
#else/* Rear TLE92108 & TLE92104*/
static Modc_MotorFullBridgeComp_t Modc_ChipOfMotorHBComp[Modc_SLAVE_DEVICE_NUM][Modc_ChipOfMotorMaxTotal] = {\
{/*for device PRE1*/
{MODC_PRE1Bridge_ONE,MODC_PRE1Bridge_TWO},
{MODC_PRE1Bridge_FIVE,MODC_PRE1Bridge_THREE},
{MODC_PRE1Bridge_FOUR,MODC_PRE1Bridge_THREE},
{MODC_PRE1Bridge_SEVEN,MODC_PRE1Bridge_SIX},
{MODC_PRE1Bridge_EIGHT,MODC_PRE1Bridge_SIX}
},
{/*for device PRE2*/
{MODC_PRE2Bridge_ONE,MODC_PRE2Bridge_THREE},
{MODC_PRE2Bridge_TWO,MODC_PRE2Bridge_FOUR}
}
};
#endif
/*____________________________ L O C A L - T Y P E S ______________________________________________________*/
/** \brief  xxxx*/

/*____________________________ L O C A L - S T R U C T U R E ______________________________________________*/
/** \brief xxx*/

/*____________________________ G L O B A L - D A T A ______________________________________________________*/


/*____________________________ L O C A L - D A T A ________________________________________________________*/
/** SPI Out/In message */
static Modc_SpiOutBuffer_t Modc_SpiOutBuffer[Modc_SLAVE_DEVICE_NUM];

#if defined(DCU_FL) || defined(DCU_FR)
static Modc_SpiFrameOutData_t Modc_SpiFrameOutData[MODC_TotalReg];
static Modc_SpiFrameInData_t Modc_SpiFrameInData[MODC_TotalReg];
static uint8 Modc_SpiFrameSize = 0;
#else
static Modc_SpiInBuffer_t Modc_SpiInBuffer[Modc_SLAVE_DEVICE_NUM];
#endif

static SPIC_Reqtype_t Modc_SpiContext;
static Modc_DiagAllFault_t Modc_DiagResult[Modc_SLAVE_DEVICE_NUM] = {0};
static Modc_DiagAllFault_t Modc_InstantErr[Modc_SLAVE_DEVICE_NUM] = {0};
static Modc_DiagFault_t  Modc_DiagFAULT[Modc_SLAVE_DEVICE_NUM] = {0};

/** Modc state*/
#if defined(DCU_FL) || defined(DCU_FR)
static uint8 Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_IDLE;
#else
static uint8 Modc_TransmitFlagStep[Modc_SLAVE_DEVICE_NUM] = {MODC_TRANSMIT_STEP_IDLE};
#endif
static uint8 Modc_State = Modc_STATE_SLEEP;
static uint8 Modc_CheckOlscCompl = FALSE;
static uint8 Modc_DiagState[Modc_SLAVE_DEVICE_NUM] = {Modc_DIAG_STATE_ERR_FREE,Modc_DIAG_STATE_ERR_FREE};
static uint16 Modc_ErrCnt[Modc_SLAVE_DEVICE_NUM] = {0,0};
static uint16 Modc_ErrRestoreCnt[Modc_SLAVE_DEVICE_NUM] = {0,0};
static boolean Modc_Running = FALSE;
static boolean Modc_FrameTransmitted = FALSE;
static boolean Modc_CommErr = FALSE;
static Modc_OffStateOLSCDetect_t Modc_OLSCDetect[Modc_SLAVE_DEVICE_NUM][Modc_ChipOfMotorMaxTotal] = {0};  /*Offline Short Circuit and Open Load Detection*/
static uint8 Modc_SMCount = 0;    /*State Machine Counter*/
static uint8 Modc_NewDataFlag = FALSE;
static uint8 Modc_OSD_UPDATE_STATUS_REG_REQ = FALSE;
static uint8 wakeFilterCnt;
static uint8 Modc_SpiErrorCounter = 0U;
static uint8 Modc_SubSM = MODC_SUBSM_IDLE;
Modc_TLE92108_REG_t Modc_DeviceReg[Modc_SLAVE_DEVICE_NUM];

Modc_ChIdInQue_t Modc_TxQueueChan[MODC_TxQueueSizeChan + 1];

Modc_ChanQueue_t Modc_TxChannelQueue =
{
  0, 0, MODC_TxQueueSizeChan + 1,  Modc_TxQueueChan
};

#if defined(DCU_FL) || defined(DCU_FR)
const uint16 Modc_DeviceID[Modc_SLAVE_DEVICE_NUM] =
{
    MODC_DEV_ID_TLE92108_232,
    MODC_DEV_ID_TLE92108_232
};
#else
const uint16 Modc_DeviceID[Modc_SLAVE_DEVICE_NUM] =
{
    MODC_DEV_ID_TLE92108_232,
    MODC_DEV_ID_TLE92104_232
};

const uint8 Modc_SpiChannel[Modc_SPI_CHANNEL_NUM] =
{
    Spi_SpiChannel_Soc_Spi_Scb6_1,
    Spi_SpiChannel_Soc_Spi_Scb0_1
};
#endif



/*____________________________ L O C A L - F U N C T I O N S - P R O T O T Y P E S ________________________*/
static void Modc_SendData(void);
static void Modc_StateMachine(void);
static void Modc_ErrorCheck(int ChipNum);
/*static uint16 Modc_GetWriteBuffer(uint8 regID);
static void Modc_SetWriteBuffer(uint8 regID, uint16 cmd);
static uint8 Modc_BitsToZero(uint8 startBit, uint8 bitwidth);*/
static uint8 Modc_InitRegister(void);
static boolean Modc_CommunicationIsOk(void);

static void Modc_DiagDevice(void);
/*static void Modc_SpiCommunicationCheck(void);*/
#if 0 /*not used*/
static boolean Modc_IsDisableTimeFinished(void);
static void Modc_RegLock(void);
static void Modc_RegUnLock(void);
static void Modc_SetDeadTime(uint8 cmd);
static void Modc_WatchDogEn(void);
static void Modc_WatchDogDisable(void);
static void Modc_WatchDogDlyTime(uint8 cmd);
static void Modc_SetCurrent(uint8 cmd);
#endif
static void Modc_ClearFaultSts(int ChipNum);

#if defined(DCU_FL) || defined(DCU_FR)
static void Modc_SpiStartTramsmit(void);
#else
static void Modc_SpiStartTramsmit(uint8 Modc_ChipNum);
#endif
static boolean Modc_SpiTransmitIdle(void);
static void Modc_HalfBridgeRegHandle(void);
static uint8 Modc_ReadStatusRegister(void);
static uint8 Modc_ReadALLRegisterBank0(void);
static uint8 Modc_ReadALLRegisterBank1(void);
static boolean Modc_AppendToQueue( Modc_ChanId_t Id, Modc_ChanDirec_t Direct);
static boolean Modc_FindIdInQueue ( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id );
static boolean Modc_AddToQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id ,Modc_ChanDirec_t Direct);
static boolean Modc_ReadFromQueue( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId, Modc_ChanDirec_t * pDirect );
static boolean Modc_RemoveFromQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId );
static uint8 Modc_EnableDevice(void);
static uint8 Modc_CheckOlsc(void);
/*static void Modc_WDReset(void);*/
//static void Modc_WriteRegisterBank0(uint8 RegAddr, uint16 WriteData, uint8 *OutIndex);
static void Modc_WriteRegisterBank0(uint8 ChipNum, uint8 RegAddr, uint16 WriteData, uint8 *OutIndex);
static void Modc_ReadRegister(uint8 ChipNum, uint8 RegAddr, uint8 *OutIndex);
static uint8 Modc_DisableWatchDog(void);
static uint8 Modc_ExitFailSafeMode(void);
static uint8 Modc_ClearGENSTAT(int ChipNum);
static uint8 Modc_Clear_DSOV_GENSTAT(int ChipNum);
static void Modc_GetStatusRegFault(int ChipNum);
uint8 Modc_OffState_SetMotorConnectConf(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp);
Modc_MotorConnectState_t Modc_OffState_GetMotorConnectState(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp, Modc_GetMotorConnectStateType_t  GetMCS_Type);

#ifdef MODC_DEBUG_DIAG_APP_INTERFACE /* DEBUG_DIAG for HBoc vsov vsuv cpuv */
volatile uint16 MODC_GetOCError0 = 0, MODC_GetOCError1 = 0;
volatile boolean MODC_GetOCErrorFlag0 = 0, MODC_GetOCErrorFlag1 = 0;
volatile boolean GetVSUVError0 = 0, GetVSUVError1 = 0;
volatile boolean GetVSOVError0 = 0, GetVSOVError1 = 0;
volatile boolean GetCPUVError0 = 0, GetCPUVError1 = 0;
#endif

/*____________________________ G L O B A L - F U N C T I O N S ____________________________________________*/
/***********************************************************************************************************/
/**
* \brief      Init the module and SPI messages.
*
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
void MODC_Init(void)
{ 
  (void)memset(Modc_SpiOutBuffer,  0x0, sizeof(Modc_SpiOutBuffer));

#if defined(DCU_FL) || defined(DCU_FR)
  (void)memset(Modc_SpiFrameOutData,  0x0, sizeof(Modc_SpiFrameOutData));
  (void)memset(Modc_SpiFrameInData,  0x0, sizeof(Modc_SpiFrameInData));
#else
  (void)memset(Modc_SpiInBuffer,  0x0, sizeof(Modc_SpiInBuffer));
#endif
  /*(void)Spi_SetHWUnitAsyncMode(LPSPI_2_HWUNIT, SPI_INTERRUPT_MODE);*/
  Modc_State = Modc_STATE_WAKEUP_DEVICE;
  Modc_CheckOlscCompl = FALSE;
  Modc_NewDataFlag = FALSE;
  Modc_ErrCnt[0] = Modc_ErrCnt[1] = 0;
  Modc_ErrRestoreCnt[0] = Modc_ErrRestoreCnt[1] = 0;

  memset(Modc_OLSCDetect, 0x00, sizeof(Modc_OLSCDetect));

  Modc_SMCount = 0;
  Modc_SubSM = MODC_SUBSM_IDLE;
}

#if 0
/***********************************************************************************************************/
/**
* \brief      Put to sleep the module.
*
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
void MODC_Sleep(void)
{
  Modc_State = Modc_STATE_SLEEP;
}
#endif

//hanson--no this function-use PWM control
#if 0
/***********************************************************************************************************/
/**
* \brief      Switch on/off the pre-driver output channel.
*
* \author
* \param      Channel(uint8) : pre-driver output channel
* \param      Status(boolean) : TRUE to switch on, FALSE to switch off
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
void MODC_SwitchOnOff(uint8 Channel, boolean Status)
{
  if ((Channel < MODC_NUM_OF_CH) && Modc_OutputState[Channel] != Status )
  {
    Modc_OutputState[Channel] = Status;

    if( Modc_OutputState[MODC_RWL_UP] == TRUE && Modc_OutputState[MODC_RWL_DOWN] == FALSE )
    {
      if( Modc_State >= Modc_STATE_OUTPUT_CTRL && Modc_State != Modc_STATE_ERROR )
      {
        Modc_CtrlReg.bit.HSLS1 = Modc_HSD_ON;
        Modc_CtrlReg.bit.HSLS2 = Modc_LSD_ON;
      }
    }

    if( Modc_OutputState[MODC_RWL_UP] == FALSE && Modc_OutputState[MODC_RWL_DOWN] == TRUE )
    {
      if( Modc_State >= Modc_STATE_OUTPUT_CTRL && Modc_State != Modc_STATE_ERROR )
      {
        Modc_CtrlReg.bit.HSLS1 = Modc_LSD_ON;
        Modc_CtrlReg.bit.HSLS2 = Modc_HSD_ON;
      }
    }
    
    if( Modc_OutputState[MODC_RWL_UP] == FALSE && Modc_OutputState[MODC_RWL_DOWN] == FALSE )
    {
      Modc_CtrlReg.bit.HSLS1 = Modc_ALL_OFF;
      Modc_CtrlReg.bit.HSLS2 = Modc_ALL_OFF;
    }
  }
}
#endif

/***********************************************************************************************************/
/**
* \brief      Main task for this driver, run diagnosis and main statemachine.
*
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/

void MODC_Runnable(void)
{
  if(TRUE)//BATP_GetPwrSrc_dV(BATP_SRC_KL30_2) > MODC_6_5V)
  {
    /* Modc_SpiCommunicationCheck(); */
    Modc_DiagDevice();
    Modc_StateMachine();
    Modc_SendData();
  }
  else
  {
    wakeFilterCnt = 0u;
    Modc_Running = FALSE;
    Modc_FrameTransmitted = FALSE;
    Modc_CommErr = FALSE;
    Modc_SpiErrorCounter = 0U;
  }

#ifdef MODC_DEBUG_DIAG_APP_INTERFACE /* app  Interface testing */
  MODC_GetOCError0 = MODC_GetOCError(0u);
  MODC_GetOCError1 = MODC_GetOCError(1u);
  MODC_GetOCErrorFlag0 = MODC_GetOCErrorFlag(0u);
  MODC_GetOCErrorFlag1 = MODC_GetOCErrorFlag(1u);
  GetVSUVError0 = MODC_GetVSUVError(0U);
  GetVSUVError1 = MODC_GetVSUVError(1U);
  GetVSOVError0 = MODC_GetVSOVError(0U);
  GetVSOVError1 = MODC_GetVSOVError(1U);
  GetCPUVError0 = MODC_GetCPUVError(0U);
  GetCPUVError1 = MODC_GetCPUVError(1U);
#endif
}

/******************************************************************************/
/** 
* \brief      MODC_GetRegisterValue
* 
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
uint16 MODC_GetRegisterValueBank0(uint8 ChipNum, uint8 regID)
{
   uint16 result = 0;
   uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg[ChipNum];
   if(regID < MODC_TotalReg)
   {
      result = Modc_DevReg[regID];
   }
   
   return result;
}

/******************************************************************************/
/** 
* \brief      MODC_GetRegisterValueBank1
* 
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
uint16 MODC_GetRegisterValueBank1(uint8 ChipNum, uint8 regID)
{
   uint16 result = 0;
   uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg[ChipNum];
   if(regID < MODC_TotalReg)
   {
      switch(regID)
      {
        case MODC_CCP_BLK2_FW_Addr:
          result = Modc_DeviceReg[ChipNum].Ccp_blk2_fw_reg;
          break;
        case MODC_PWM_PCHG_INIT_Addr:
          result = Modc_DeviceReg[ChipNum].Pwm_pchg_init_reg;
          break;
        case MODC_PWM_ICHG_FW_Addr:
          result = Modc_DeviceReg[ChipNum].Pwm_ichg_fw_reg;
          break;
        case MODC_PWM_PDCHG_INIT_Addr:
          result = Modc_DeviceReg[ChipNum].Pwm_pdchg_init_reg;
          break;
        case MODC_PWM_ICHGMAX_CCP_BLK3_FW_Addr:
          result = Modc_DeviceReg[ChipNum].Pwm_ichgmax_ccp_blk3_fw_reg;
          break;
        default:
          break;
      }
   }
   
   return result;
}
#if 0
/***********************************************************************************************************/
/**
* \brief      Function called after a SPI message reception.
*
* \author
* \param      Channel(uint8) : SPI channel
* \param      Data(uint32) : received data
* \param      Valid(boolean) : validity flag of the received data
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/

void MODC_IntrCallback(void *driverState, spi_event_t event, void *userData)
{
  /* If the received data is valid */
//  if( Channel == Modc_SPI_CHANNEL)
  {
    
  }
}
#endif

/***********************************************************************************************************/
/**
* \brief      Return the OC error flag for the driver.
* \param      N/A
* \retval     boolean  0:error  1:NO error
* \note       N/A
*/
/***********************************************************************************************************/
boolean MODC_GetOCErrorFlag(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return 0;
  }
  return (Modc_DiagResult[ChipNum].ErrOC.ErrOCState)?(1U):(0U);
}

/***********************************************************************************************************/
/**
* \brief      Return the OC error  status for the driver.
* \param      ChipNum 
* \retval     Modc_ErrOC_t                    
*             ERR_OC_HS[1-8] - HS[1-8] over current
*             ERR_OC_LS[1-8] - LS[1-8] over current
*/
/***********************************************************************************************************/
uint16 MODC_GetOCError(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return 0;
  }
  return  Modc_DiagResult[ChipNum].ErrOC.ErrOCState;  
}


/***********************************************************************************************************/
/**
* \brief      Return instant error status for the driver.
*
* \author
* \param      N/A
* \retval     Modc_DiagResult - error representative level
*             MODC_DIAG_ERR_FREE   - no error present
*             MODC_DIAG_ERR_TSD    - thermal shutdown
*             MODC_DIAG_ERR_OC_HS1 - HS1 over current
*             MODC_DIAG_ERR_OC_LS1 - HS1 over current
*             MODC_DIAG_ERR_OC_HS2 - HS2 over current
*             MODC_DIAG_ERR_OC_LS2 - LS2 over current
* \note       N/A
*/
/***********************************************************************************************************/
uint64 MODC_GetInstantError(void)
{
  return 0;
}

boolean MODC_GetCPUVError(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return 0;
  }
  return Modc_DiagResult[ChipNum].ErrCPUV;
}

boolean MODC_GetVSUVError(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return 0;
  }
  return Modc_DiagResult[ChipNum].ErrVSUV;
}

boolean MODC_GetVSOVError(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return 0;
  }
  return Modc_DiagResult[ChipNum].ErrVSOV;
}

uint8 MODC_GetTSDError(void)
{
  return 0;
}
uint8 MODC_GetCS2Error(void)
{
  return 0;
}
uint8 MODC_GetCS1Error(void)
{
  return 0;
}


void MODC_TxCompleteCallback(void)
{
  /*WD_TaskRefreshWdg(WD_SPI_MODC);*/
}

boolean MODC_GetRunningState(void)
{
  return Modc_Running;
}

boolean MODC_VerifyCommunication(void)
{
  return !Modc_CommErr;
}

void MODC_ResetDevice(void)
{
  Modc_State = Modc_STATE_RESET_DEVICE;
}

/*____________________________ L O C A L - F U N C T I O N S ______________________________________________*/
/***********************************************************************************************************/
/**
* \brief      Modc_StateMachine : Main state machine, it should called in a period task or alarm.
*
* \author     Kevin ZHAO
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
static void Modc_StateMachine(void)
{
  static uint8 numOfCfgRetry = 0;
  static uint8 lastModcState = Modc_STATE_SLEEP;

#if 0 /* reading  all reg for debug*/
{
    /* reading  all reg for debug*/
  static uint16 DiagReadRegPeriod = 0;
  if (DiagReadRegPeriod++ >= 1000)
  {    
    if (FALSE == Modc_NewDataFlag)
    {
      if (TRUE == Modc_ReadALLRegisterBank0()) 
      {
        DiagReadRegPeriod = 0;
        Modc_NewDataFlag = TRUE;
      }
    }
  }
}/* debug */
#endif

  switch(Modc_State)
  {
    case Modc_STATE_SLEEP:
      if( TRUE == Modc_GetDeviceWakeUpState() )
      {
        /* Disable the device*/
        Modc_WakeUpDevice(FALSE);
      }
      break;

    case Modc_STATE_WAKEUP_DEVICE:
      Modc_WakeUpDevice(TRUE);
      Modc_SMCount = 0;
      Modc_SubSM = MODC_DISABLE_WATCH_DOG;
      Modc_State = Modc_STATE_READ_STATE_REGISTER;
      lastModcState = Modc_STATE_WAKEUP_DEVICE;
      break;

    case Modc_STATE_READ_STATE_REGISTER:
      Modc_SMCount++;
      if(Modc_SMCount >= Modc_DEVICE_WAKEUP_TIME)
      {
        Modc_SMCount = Modc_DEVICE_WAKEUP_TIME;
        if(Modc_SubSM == MODC_DISABLE_WATCH_DOG)
        {
          if(TRUE == Modc_DisableWatchDog())
          {
            Modc_SubSM = MODC_READ_BANK0_REG;
          }
        }
        else  if(Modc_SubSM == MODC_READ_BANK0_REG)
        {
          if(TRUE == Modc_ReadALLRegisterBank0())
          {
            Modc_SubSM = MODC_READ_BANK1_REG;
          }
        }
        else if(Modc_SubSM == MODC_READ_BANK1_REG)
        {
          if(TRUE == Modc_ReadALLRegisterBank1())
          {
            Modc_SubSM = MODC_SUBSM_IDLE;
            Modc_SMCount = 0;
            Modc_FrameTransmitted = TRUE;
            Modc_State = Modc_STATE_DETECT_DEVICE;
          }
        }
        else
        {
          ; /*do nothing*/
        }
      }
      lastModcState = Modc_STATE_READ_STATE_REGISTER;
      break;

    case Modc_STATE_DETECT_DEVICE:
      if(TRUE == Modc_CommunicationIsOk())  /*communication is OK*/
      {
        Modc_State = Modc_STATE_INIT_REGISTER;
        numOfCfgRetry = 0;
      }
      else
      {
        /* Retry to read status register */
        if(TRUE == Modc_ReadStatusRegister())
        {
          numOfCfgRetry++;
          if( numOfCfgRetry >= Modc_MAX_NUM_OF_CFG_RETRY )
          {
            Modc_State = Modc_STATE_ERROR;
            /* Disable the device*/
            Modc_WakeUpDevice(FALSE);
          }
        }
      }
      lastModcState = Modc_STATE_DETECT_DEVICE;
      break;

    case Modc_STATE_INIT_REGISTER:
      /* Configurate device if the communication is OK*/
      if(TRUE == Modc_InitRegister()) /*   enable DIAG:BD_PASS = 0(0:active) ,HBxMODE = 0u(off) ,HBxVDSTH = 7u(MAX)*/
      {
        Modc_SMCount = 0;
        Modc_State = Modc_STATE_OOL_OSC_STATUS;
      }
      lastModcState = Modc_STATE_INIT_REGISTER;
      break;

    case Modc_STATE_OOL_OSC_STATUS: /* OFF-STATE DIAG */ /* offline short circuit and open load diagnostics */
      Modc_SMCount++;
      if (Modc_SMCount >= Modc_OLSC_DETECT_TIME) /* Wait for Init is done*/
      {
        Modc_SMCount = Modc_OLSC_DETECT_TIME;   

        if (TRUE == Modc_CheckOlsc()) /* check offStata compl */
        {
          Modc_CheckOlscCompl = TRUE;
        }
        else
        {
          Modc_CheckOlscCompl = FALSE;
        }

        if (Modc_CheckOlscCompl)
        {
          if (TRUE == Modc_EnableDevice()) /*   disable DIAG:restore Desired  HBxVDSTH */
          {
            Modc_State = Modc_STATE_READ_STATUS;
            Modc_SMCount = 0;            
          }
        }
      }else/*  read Status Register when non-arrival Modc_OLSC_DETECT_TIME */
      {
        Modc_ReadStatusRegister();
      }

      lastModcState = Modc_STATE_OOL_OSC_STATUS;
      break;

    case Modc_STATE_READ_STATUS:
#if 0
      if((Modc_DeviceReg.ic_stat1_reg & MODC_FAULT ) == MODC_FAULT)
      {
        /* Disable the device for a while if error is confirmed*/
        if( Modc_DiagState >= Modc_DIAG_STATE_ERR_TSD && Modc_DiagState <= Modc_DIAG_STATE_ERR_VGS )
        {
          Modc_State = Modc_STATE_ERROR;
          /* Disable the device*/
          Modc_WakeUpDevice(FALSE);
        }
      }      
#endif
      {
        Modc_HalfBridgeRegHandle();
      }

#ifdef MODC_DEBUG_HBCTL_REGVIEW
      { /* DEBUG */
        static uint16 count_time = 0;
        if (count_time++ > 400)/* 5ms*400=2s */
        { 
            count_time = 0;          
            if (FALSE == Modc_NewDataFlag)
            {
              if (TRUE == Modc_ReadALLRegisterBank0()) /* DEBUG read all reg  2s period */
              {
                Modc_NewDataFlag = TRUE;
              }
            }
            volatile static uint8 req_chanel_button = 1;
            if (req_chanel_button)
            {
              req_chanel_button = 0;
              MODC_ChannelSetupReuqest(0, MODC_GDS_DRIVE_LOW_SIDE);
              MODC_ChannelSetupReuqest(1, MODC_GDS_DRIVE_HIGH_SIDE);

              MODC_ChannelSetupReuqest(8, MODC_GDS_DRIVE_LOW_SIDE);
              MODC_ChannelSetupReuqest(9, MODC_GDS_DRIVE_HIGH_SIDE);
            }          
        }
      }
#endif      

#ifdef MODC_WATCH_DOG_ENABLE
      Modc_SMCount++;
      if(Modc_SMCount >= Modc_WATCH_DOG_RESET_TIME)
      {
        Modc_SMCount = Modc_WATCH_DOG_RESET_TIME;
        if(TRUE == Modc_NewDataFlag)
        {
          Modc_WDReset();
          Modc_SMCount = 0;
        }
      }
#endif    
      lastModcState = Modc_STATE_READ_STATUS;
      break;
#if 0
    case Modc_STATE_ERROR:
      if( Modc_DiagState == Modc_DIAG_STATE_RECOVERY || lastModcState == Modc_STATE_DETECT_DEVICE )
      {
        numOfCfgRetry = 0;
        Modc_DeviceReg.ic_stat1_reg = 0x00;
        /* Enable the device*/
        Modc_State = Modc_STATE_WAKEUP_DEVICE;
      }

      break;
    
    case Modc_STATE_RESET_DEVICE:
      /* Disable the device*/
      Modc_WakeUpDevice(FALSE);
      Modc_SMCount++;
      if(Modc_SMCount >= Modc_DEVICE_RESET_TIME)
      {
        Modc_SMCount = 0;
        MODC_Init();
        Modc_State = Modc_STATE_WAKEUP_DEVICE;
      }
      break;
#endif
    default:
      break;
  }
  Modc_NewDataFlag = FALSE;/* SPI DATA REQ FLAG RESET */
}

#if defined(DCU_FL) || defined(DCU_FR)
static void Modc_SpiStartTramsmit(void)
{
  Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_READ_WRITE;
}
#else
static void Modc_SpiStartTramsmit(uint8 Modc_ChipNum)
{
  Modc_TransmitFlagStep[Modc_ChipNum] = MODC_TRANSMIT_STEP_READ_WRITE;
}
#endif

static boolean Modc_CheckSpiTransmitIdle(void)
{
  uint8 Modc_Ret = FALSE;
#if defined(DCU_FL) || defined(DCU_FR)
  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
#else
  if((Modc_TransmitFlagStep[MODC_CHIP_ONE] == MODC_TRANSMIT_STEP_IDLE)
     && (Modc_TransmitFlagStep[MODC_CHIP_TWO] == MODC_TRANSMIT_STEP_IDLE))
#endif
  {
    Modc_Ret = TRUE;
  }
  return Modc_Ret;
}

static void Modc_UpdateRegisterBank1(uint8 ChipNum, uint8 RegAddr, uint16 UpdateData)
{
  uint16 *Modc_Reg = (uint16 *)&Modc_DeviceReg[ChipNum];

  switch(RegAddr)
  {
    case MODC_CCP_BLK2_FW_Addr:
      Modc_DeviceReg[ChipNum].Ccp_blk2_fw_reg = UpdateData;
      break;
    case MODC_PWM_PCHG_INIT_Addr:
      Modc_DeviceReg[ChipNum].Pwm_pchg_init_reg = UpdateData;
      break;
    case MODC_PWM_ICHG_FW_Addr:
      Modc_DeviceReg[ChipNum].Pwm_ichg_fw_reg = UpdateData;
      break;
    case MODC_PWM_PDCHG_INIT_Addr:
      Modc_DeviceReg[ChipNum].Pwm_pdchg_init_reg = UpdateData;
      break;
    case MODC_PWM_ICHGMAX_CCP_BLK3_FW_Addr:
      Modc_DeviceReg[ChipNum].Pwm_ichgmax_ccp_blk3_fw_reg = UpdateData;
      break;
    default:
      Modc_Reg[RegAddr] = UpdateData;
      break;
  }
}

/***********************************************************************************************************/
/**
* \brief      Modc_ParseSpiRxData
*             parse the register value to Modc_DeviceReg after a SPI read
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
static void Modc_ParseSpiRxData(uint8 Modc_ChipNum)
{
  uint8 i = 0;
  uint8 RegAddressSize = 0;
  uint8 RegAddress = 0;
  uint16 RegReceiveValue = 0;
  uint16 *Modc_Reg = (uint16 *)&Modc_DeviceReg[Modc_ChipNum];

  RegAddressSize = sizeof(Modc_TLE92108_REG_t)/sizeof(uint16);

  for(i = 0; i < Modc_SpiOutBuffer[Modc_ChipNum].Modc_SetupSize; i++)
  {
    if(MODC_READ == Modc_SpiOutBuffer[Modc_ChipNum].Modc_SpiOutData[i].AddressByte.Op)
    {
      RegAddress = Modc_SpiOutBuffer[Modc_ChipNum].Modc_SpiOutData[i].AddressByte.Addr;
#if defined(DCU_FL) || defined(DCU_FR)
      RegReceiveValue = Modc_SpiFrameInData[i].RespByte[Modc_SLAVE_DEVICE_NUM - Modc_ChipNum - 1];
#else
      RegReceiveValue = Modc_SpiInBuffer[Modc_ChipNum].Modc_SpiInData[i].ResponseData.Word;
#endif
      if(RegAddress < RegAddressSize)
      {
        if(MODC_REG_BANK1 == (Modc_Reg[MODC_GENCTRL1_Addr] & MODC_REG_BANK1)) /*REG_BANK_1*/
        {
          Modc_UpdateRegisterBank1(Modc_ChipNum,RegAddress,RegReceiveValue);
        }
        else
        {
          Modc_Reg[RegAddress] = RegReceiveValue;
        }
      }
    }
  }
}

#if 0
/***********************************************************************************************************/
/**
* \brief      Modc_WDReset
*             Reset the watch dog timer
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
static void Modc_WDReset(void)
{
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Modc_RegValue = 0;
  Index = Modc_SpiContext.transferCount;

  if(Index < (MODC_TotalReg - 1))
  {
    if((Modc_DeviceReg.ic_ctrl2_reg & MODC_WD_EN) == MODC_WD_EN)
    {
      tempCmd.byte.addres = MODC_IC_CTRL2Addr;
      tempCmd.byte.command = MODC_WRITE;
      Modc_RegValue = Modc_DevReg[MODC_IC_CTRL2Addr];
      /*Invert WD_RST bit to restart the watchdog timer.*/
      if(Modc_RegValue & MODC_WD_RST)
      {
        Modc_RegValue &= ~MODC_WD_RST;
      }
      else
      {
        Modc_RegValue |= MODC_WD_RST;
      }
      tempCmd.byte.dataToWrite = Modc_RegValue;
      Modc_SpiOutData[Index++] = tempCmd;
      Modc_DevReg[MODC_IC_CTRL2Addr] = Modc_RegValue;

      Modc_SpiContext.transferCount = Index;
    }
  }
}
#endif

#if defined(DCU_FL) || defined(DCU_FR)
/******************************************************************************/
/**
* \brief      Modc_SpiFramePackage
*             Package the SPI Frame according to the Daisy chain format
* \author
* \param      N/A
* \retval     N/A
* \note
*/
/******************************************************************************/
void Modc_SpiFramePackage(void)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint8 i = 0;
  uint8 j = 0;

  Modc_SpiFrameSize = Modc_SpiOutBuffer[0].Modc_SetupSize;
  for(i = 1; i < Modc_SLAVE_DEVICE_NUM; i++)
  {
    if(Modc_SpiFrameSize < Modc_SpiOutBuffer[i].Modc_SetupSize)
    {
      Modc_SpiFrameSize = Modc_SpiOutBuffer[i].Modc_SetupSize;
    }
  }

  for(i = 0; i < Modc_SLAVE_DEVICE_NUM; i++)
  {
    if(Modc_SpiOutBuffer[i].Modc_SetupSize < Modc_SpiFrameSize)
    {
      tempCmd.AddressByte.Msb = 0x01;
      tempCmd.AddressByte.LABT = (i == (Modc_SLAVE_DEVICE_NUM - 1)) ? 0x01 : 0x00;
      tempCmd.AddressByte.Addr = 0x1F;
      tempCmd.AddressByte.Op = MODC_READ;
      tempCmd.dataToWrite.Word = 0x0000;

      for(j = Modc_SpiOutBuffer[i].Modc_SetupSize; j < Modc_SpiFrameSize; j++)
      {
        Modc_SpiOutBuffer[i].Modc_SpiOutData[j] = tempCmd;
      }
    }
  }

  for(i = 0;i < Modc_SpiFrameSize; i++)
  {
    for(j = 0; j < Modc_SLAVE_DEVICE_NUM; j++)
    {
      Modc_SpiFrameOutData[i].AddrByte[j] = *((uint8 *)&Modc_SpiOutBuffer[Modc_SLAVE_DEVICE_NUM - j - 1].Modc_SpiOutData[i].AddressByte);
      Modc_SpiFrameOutData[i].DataByte[j] = Modc_SpiOutBuffer[Modc_SLAVE_DEVICE_NUM - j - 1].Modc_SpiOutData[i].dataToWrite.Word;
    }
  }
}
#endif
/***********************************************************************************************************/
/**
* \brief      Send data to pre-driver. Clear receiving buffer before sending to make sure that the latest \n
*             status data will be used in statemachine.
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
static void Modc_SendData(void)
{
#if defined(DCU_FL) || defined(DCU_FR)
  Spi_SeqResultType SpiSeqResult = SPI_SEQ_FAILED;
  uint8 Modc_TransmitIndex = 0;
  uint8 Modc_TransmitDone = FALSE;
  uint8 Modc_GetSpiResultCnt = 0;
  uint8 Modc_l_ChipNum = 0;

  switch(Modc_TransmitFlagStep)
  {
    case MODC_TRANSMIT_STEP_IDLE:
      break;

    case MODC_TRANSMIT_STEP_READ_WRITE:
      Modc_SpiFramePackage();
      for(Modc_TransmitIndex = 0;Modc_TransmitIndex < Modc_SpiFrameSize; Modc_TransmitIndex++)
      {
        Modc_SpiContext.sendBuff = (void *)&Modc_SpiFrameOutData[Modc_TransmitIndex];
        Modc_SpiContext.receBuff = (void *)&Modc_SpiFrameInData[Modc_TransmitIndex];
        Modc_SpiContext.transferCount = 3;
        Modc_SpiDataTransmitStart();
        Modc_SpiTransmitFrame(Spi_SpiChannel_Soc_Spi_Scb6_1,&Modc_SpiContext);

        Modc_TransmitDone = FALSE;
        while(FALSE == Modc_TransmitDone)
        {
          SpiSeqResult = Modc_SpiGetSequenceResult(Spi_SpiChannel_Soc_Spi_Scb6_1);
          Modc_GetSpiResultCnt++;
          if((SPI_SEQ_OK == SpiSeqResult)
              || (Modc_GetSpiResultCnt > Modc_CHECK_SPI_RESULT_TIMES))
          {
            Modc_GetSpiResultCnt = 0;
            Modc_SpiDataTransmitStop();
            Modc_TransmitDone = TRUE;
          }
        }
      }
      Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_CHECK;
      break;

    case MODC_TRANSMIT_STEP_CHECK:
      /* for async spi tx, must wait for tx done then can check the rx data */
      //SpiSeqResult = Modc_SpiGetSequenceResult(Spi_SpiChannel_Soc_Spi_Scb6_1);
      //if (SPI_SEQ_OK == SpiSeqResult)
      {
        Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_IDLE;
        for(Modc_l_ChipNum = 0; Modc_l_ChipNum < Modc_SLAVE_DEVICE_NUM; Modc_l_ChipNum++)
        {
          Modc_ParseSpiRxData(Modc_l_ChipNum);
        }
      }
      break;

    default:
      break;
  }
#else
  Spi_SeqResultType SpiSeqResult = SPI_SEQ_FAILED;
  uint8 Modc_ChipNum = 0;
  uint8 Modc_TransmitDone = FALSE;
  uint8 Modc_TransmitIndex = 0;
  uint8 Modc_GetSpiResultCnt = 0;

  for(Modc_ChipNum = 0; Modc_ChipNum < Modc_SLAVE_DEVICE_NUM; Modc_ChipNum++)
  {
    switch(Modc_TransmitFlagStep[Modc_ChipNum])
    {
      case MODC_TRANSMIT_STEP_IDLE:
        break;

      case MODC_TRANSMIT_STEP_READ_WRITE:
        SpiSeqResult = Modc_SpiGetSequenceResult(Modc_SpiChannel[Modc_ChipNum]);
        if (SPI_SEQ_OK == SpiSeqResult)
        {
          for(Modc_TransmitIndex = 0;Modc_TransmitIndex < Modc_SpiOutBuffer[Modc_ChipNum].Modc_SetupSize; Modc_TransmitIndex++)
          {
            Modc_SpiContext.sendBuff = (void *)&Modc_SpiOutBuffer[Modc_ChipNum].Modc_SpiOutData[Modc_TransmitIndex];
            Modc_SpiContext.receBuff = (void *)&Modc_SpiInBuffer[Modc_ChipNum].Modc_SpiInData[Modc_TransmitIndex];
            Modc_SpiContext.transferCount = 1;

            Modc_SpiTransmitFrame(Modc_SpiChannel[Modc_ChipNum],&Modc_SpiContext);

            Modc_TransmitDone = FALSE;
            while(FALSE == Modc_TransmitDone)
            {
              SpiSeqResult = Modc_SpiGetSequenceResult(Modc_SpiChannel[Modc_ChipNum]);
              Modc_GetSpiResultCnt++;
              if((SPI_SEQ_OK == SpiSeqResult)
                  || (Modc_GetSpiResultCnt > Modc_CHECK_SPI_RESULT_TIMES))
              {
                Modc_GetSpiResultCnt = 0;
                Modc_TransmitDone = TRUE;
              }
            }
          }

          Modc_ParseSpiRxData(Modc_ChipNum);
          Modc_TransmitFlagStep[Modc_ChipNum] = MODC_TRANSMIT_STEP_IDLE;
          //Modc_TransmitFlagStep[Modc_ChipNum] = MODC_TRANSMIT_STEP_CHECK;
        }
        break;
#if 0
      case MODC_TRANSMIT_STEP_CHECK:
        /* for async spi tx, must wait for tx done then can check the rx data */
        SpiSeqResult = Modc_SpiGetSequenceResult(Modc_SpiChannel[Modc_ChipNum]);
        if (SPI_SEQ_PENDING != SpiSeqResult)
        {
          Modc_TransmitFlagStep[Modc_ChipNum] = MODC_TRANSMIT_STEP_IDLE;
          Modc_ParseSpiRxData(Modc_ChipNum);
        }
        break;
#endif
      default:
        break;
    }
  }
#endif
}

#if 0
static uint16 Modc_GetWriteBuffer(uint8 regID)
{
  uint16 ret = 0x0;

  if(regID < MODC_TotalReg)
  {
    ret = Modc_SpiOutData[regID].Word;
  }
  return ret;
}

static void Modc_SetWriteBuffer(uint8 regID, uint16 cmd)
{
   if(regID < MODC_TotalReg)
   {
     Modc_SpiOutData[regID].Word = cmd;
   }
}

/******************************************************************************/
/** 
* \brief      Modc_BitsToZero
*             set the specific bit(s) to zero
* \author
* \param      N/A 
* \retval     N/A 
* \note       Just apply to BYTE witdh
*/
/******************************************************************************/
static uint8 Modc_BitsToZero(uint8 startBit, uint8 bitwidth)
{
  uint8 valueTemp = 0x0;
  uint8 bitIndex;
  
  if((startBit + bitwidth) <= BYTE_BITS_LEN)
  {
    for(bitIndex=0; bitIndex<bitwidth; bitIndex++)
    {
      valueTemp |= (uint8)((uint8)0x01U << (uint8)(startBit + bitIndex));
    }
  }
  else
  {
    /*error parameters*/ 
  }
  return (uint8)(~valueTemp);
}
#endif
#if 0
static void Modc_WriteRegisterBank0(uint8 RegAddr, uint16 WriteData, uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.AddressByte.Msb = 0x01;
    tempCmd.AddressByte.LABT = 0x01;
    tempCmd.AddressByte.Addr = RegAddr;
    tempCmd.AddressByte.Op = MODC_WRITE;
    tempCmd.dataToWrite.Word = WriteData;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[RegAddr] = WriteData;
  }

  *OutIndex = Index;
}
#endif

static void Modc_WriteRegisterBank0(uint8 ChipNum, uint8 RegAddr, uint16 WriteData, uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  Modc_SpiOutData_t *Modc_SpiOut = Modc_SpiOutBuffer[ChipNum].Modc_SpiOutData;
  uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg[ChipNum];
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.AddressByte.Msb = 0x01;
#if defined(DCU_FL) || defined(DCU_FR)
		tempCmd.AddressByte.LABT = (ChipNum == (Modc_SLAVE_DEVICE_NUM - 1)) ? 0x01 : 0x00;
#else
    tempCmd.AddressByte.LABT = 0x01;
#endif
    tempCmd.AddressByte.Addr = RegAddr;
    tempCmd.AddressByte.Op = MODC_WRITE;
    tempCmd.dataToWrite.Word = WriteData;
    Modc_SpiOut[Index++] = tempCmd;
    Modc_DevReg[RegAddr] = WriteData;
  }

  *OutIndex = Index;
}

static void Modc_WriteRegisterBank1(uint8 RegAddr, uint16 WriteData, uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.AddressByte.Msb = 0x01;
    tempCmd.AddressByte.LABT = 0x01;
    tempCmd.AddressByte.Addr = RegAddr;
    tempCmd.AddressByte.Op = MODC_WRITE;
    tempCmd.dataToWrite.Word = WriteData;
    //Modc_SpiOutData[Index++] = tempCmd; /* WHY NOT USED */
   // Modc_UpdateRegisterBank1(RegAddr,WriteData);
  }

  *OutIndex = Index;
}

static void Modc_ReadRegister(uint8 ChipNum, uint8 RegAddr, uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  Modc_SpiOutData_t *Modc_SpiOut = Modc_SpiOutBuffer[ChipNum].Modc_SpiOutData;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.AddressByte.Msb = 0x01;
#if defined(DCU_FL) || defined(DCU_FR)
		tempCmd.AddressByte.LABT = (ChipNum == (Modc_SLAVE_DEVICE_NUM - 1)) ? 0x01 : 0x00;
#else
    tempCmd.AddressByte.LABT = 0x01;
#endif
    tempCmd.AddressByte.Addr = RegAddr;
    tempCmd.AddressByte.Op = MODC_READ;
    tempCmd.dataToWrite.Word = 0x0000;
    Modc_SpiOut[Index++] = tempCmd;
  }

  *OutIndex = Index;
}

/******************************************************************************/
/**
* \brief      Modc_EnableDevice
*             Eanble Gate driver
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_EnableDevice(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Ret = FALSE;
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      /*Set driver to actvie mode*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,Modc_DeviceReg[Modc_ChipNumIndex].Genctrl2_reg & (~MODC_BD_PASS),&Index[Modc_ChipNumIndex]);

      /*VDS1 VDS2 REG:Set drain-source overvoltage threshold*/  /* 3U = 0.3v*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS1_Addr,\
      (MODC_VDS_TH_0_30V<<MODC_VDS1_HB4VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS1_HB3VDSTH_OFFSET)|\
      (MODC_VDS_TH_0_30V<<MODC_VDS1_HB2VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS1_HB1VDSTH_OFFSET),\
      &Index[Modc_ChipNumIndex]);
#if defined(DCU_FL) || defined(DCU_FR)
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,\
      (MODC_VDS_TH_0_30V<<MODC_VDS2_HB8VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS2_HB7VDSTH_OFFSET)|\
      (MODC_VDS_TH_0_30V<<MODC_VDS2_HB6VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS2_HB5VDSTH_OFFSET),\
      &Index[Modc_ChipNumIndex]);
#else
      if(MODC_CHIP_ONE == Modc_ChipNumIndex)/* REAR seat chipTwo is TLE92104*/
      {
        Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,\
      (MODC_VDS_TH_0_30V<<MODC_VDS2_HB8VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS2_HB7VDSTH_OFFSET)|\
      (MODC_VDS_TH_0_30V<<MODC_VDS2_HB6VDSTH_OFFSET)|(MODC_VDS_TH_0_30V<<MODC_VDS2_HB5VDSTH_OFFSET),\
        &Index[Modc_ChipNumIndex]);
      }
#endif

      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];

#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif

    Ret = TRUE;
  }

  return Ret;
}

/******************************************************************************/
/** 
* \brief      Modc_InitRegister
*             configure the register to default value
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_InitRegister(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Ret = FALSE;
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;
  uint16 Modc_Temp = 0;
	
  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      /*Set REG_BLANK to 0*/
      Modc_Temp = MODC_GetRegisterValueBank0(Modc_ChipNumIndex,MODC_GENCTRL1_Addr);
			MODC_SET_REG_VAL(Modc_Temp,MODC_REG_BANK_MSK | MODC_OCEN_MSK, MODC_REG_BANK0 | MODC_OCEN_DIS);
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL1_Addr,Modc_Temp,&Index[Modc_ChipNumIndex]);

      /* Set VDS voer voltage filter time to 3us 11b*/
      Modc_Temp = MODC_GetRegisterValueBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr);
			MODC_SET_REG_VAL(Modc_Temp,MODC_TFVDS_MSK, MODC_TFVDS_3uS);
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,Modc_Temp,&Index[Modc_ChipNumIndex]);

      /* Set Second pair cross-current protection and blank times to 16us/16us*/
      Modc_Temp = MODC_GetRegisterValueBank0(Modc_ChipNumIndex,MODC_CCP_BLK2_ACT_Addr);
			MODC_SET_REG_VAL(Modc_Temp,MODC_TBLANK2_ACT_MSK | MODC_TCCP2_ACT_MSK, MODC_TBLANK2_ACT_16uS | MODC_TCCP2_ACT_16uS);
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_CCP_BLK2_ACT_Addr,Modc_Temp,&Index[Modc_ChipNumIndex]);

      /* chose the second pair cross-current protection and blank times for HB8/HB7/HB6 */
#if defined(DCU_FL) || defined(DCU_FR)
      Modc_Temp = MODC_GetRegisterValueBank0(Modc_ChipNumIndex,MODC_CCP_BLK1_Addr);
			MODC_SET_REG_VAL(Modc_Temp,MODC_HB8CCPBLK_MSK | MODC_HB7CCPBLK_MSK | MODC_HB6CCPBLK_MSK, \
                                MODC_HB8CCPBLK_CCP2_BLANK2 | MODC_HB7CCPBLK_CCP2_BLANK2 | MODC_HB6CCPBLK_CCP2_BLANK2);
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_CCP_BLK1_Addr,Modc_Temp,&Index[Modc_ChipNumIndex]);
#endif
      /*Set drain-source overvoltage threshold*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS1_Addr,0x0FFF,&Index[Modc_ChipNumIndex]);
#if defined(DCU_FL) || defined(DCU_FR)
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,0x0FFF,&Index[Modc_ChipNumIndex]);
#else
      if(MODC_CHIP_ONE == Modc_ChipNumIndex)
      {
        Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,0x0FFF,&Index[Modc_ChipNumIndex]);
      }
#endif

      /*Clear HBMODE*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_HBMODE_Addr,0x0000,&Index[Modc_ChipNumIndex]);
      

      /*Hbidiag reg :Set  CSAxL to 0*/
      /*Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_HBIDIAG_Addr,Modc_DeviceReg[Modc_ChipNumIndex].Hbidiag_reg & (~(MODC_CSA2L | MODC_CSA1L)),&Index[Modc_ChipNumIndex]);*/

      /* Hbidiag reg: Pull-down activated */
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_HBIDIAG_Addr,Modc_DeviceReg[Modc_ChipNumIndex].Hbidiag_reg | (0x0000),&Index[Modc_ChipNumIndex]);                  

      /* GENCTRL2: OCTH2&OCTH1 to xxB (clear and SET ) */ /* set both 1u */
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,\
      ((Modc_DeviceReg[Modc_ChipNumIndex].Genctrl2_reg & (~(MODC_GC2_OCTH2_MASK|MODC_GC2_OCTH1_MASK)))|(MODC_GC2_OCTH2_01B | MODC_GC2_OCTH1_01B )),\
      &Index[Modc_ChipNumIndex]);
      
      /*GENCTRL2 REG:BD_PASS = 0(actvie) :Set driver to actvie mode*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,Modc_DeviceReg[Modc_ChipNumIndex].Genctrl2_reg & (~MODC_BD_PASS),&Index[Modc_ChipNumIndex]);

      /*VDS1 VDS2 REG:Set drain-source overvoltage threshold  for off state diag*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS1_Addr,\
      (MODC_VDS_TH_2V<<MODC_VDS1_HB4VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS1_HB3VDSTH_OFFSET)|\
      (MODC_VDS_TH_2V<<MODC_VDS1_HB2VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS1_HB1VDSTH_OFFSET),\
      &Index[Modc_ChipNumIndex]);
#if defined(DCU_FL) || defined(DCU_FR)
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,\
      (MODC_VDS_TH_2V<<MODC_VDS2_HB8VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS2_HB7VDSTH_OFFSET)|\
      (MODC_VDS_TH_2V<<MODC_VDS2_HB6VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS2_HB5VDSTH_OFFSET),\
      &Index[Modc_ChipNumIndex]);
#else
      if(MODC_CHIP_ONE == Modc_ChipNumIndex)/* REAR seat chipTwo is TLE92104*/
      {
        Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_VDS2_Addr,\
      (MODC_VDS_TH_2V<<MODC_VDS2_HB8VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS2_HB7VDSTH_OFFSET)|\
      (MODC_VDS_TH_2V<<MODC_VDS2_HB6VDSTH_OFFSET)|(MODC_VDS_TH_2V<<MODC_VDS2_HB5VDSTH_OFFSET),\
        &Index[Modc_ChipNumIndex]);
      }
#endif


      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];

#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif

    Ret = TRUE;
  }

  return Ret;
}

/******************************************************************************/
/**
* \brief      Modc_ReadStatusRegister
*             Read all the state register from device
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_ReadStatusRegister(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Modc_RegAddr = 0;
  uint8 Ret = FALSE;
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      for(Modc_RegAddr = MODC_GENSTAT_Addr; Modc_RegAddr <= MODC_TRISE_FALL3_Addr; Modc_RegAddr++)
      {
        Modc_ReadRegister(Modc_ChipNumIndex,Modc_RegAddr,&Index[Modc_ChipNumIndex]);
      }
      Modc_ReadRegister(Modc_ChipNumIndex,MODC_DEVID_Addr,&Index[Modc_ChipNumIndex]);
      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];
#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif

    Ret = TRUE;
  }

  return Ret;
}

/******************************************************************************/
/**
* \brief      Modc_ReadALLRegisterBank0
*             Read all the register value from device
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_ReadALLRegisterBank0(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Modc_RegAddr = 0;
  uint8 Ret = FALSE;
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      for(Modc_RegAddr = MODC_GENCTRL1_Addr; Modc_RegAddr <= MODC_TRISE_FALL3_Addr; Modc_RegAddr++)
      {
        Modc_ReadRegister(Modc_ChipNumIndex,Modc_RegAddr,&Index[Modc_ChipNumIndex]);
      }
      Modc_ReadRegister(Modc_ChipNumIndex,MODC_DEVID_Addr,&Index[Modc_ChipNumIndex]);
      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];
#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif

    Ret = TRUE;
  }

  return Ret;
}

/******************************************************************************/
/**
* \brief      Modc_ReadALLRegisterBank1
*             Read all the register value from device
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_ReadALLRegisterBank1(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg;
  uint8 Ret = FALSE;
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      Modc_DevReg = (uint16 *)&Modc_DeviceReg[Modc_ChipNumIndex];
      /*Set REG_BLANK to 1*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL1_Addr,Modc_DevReg[MODC_GENCTRL1_Addr] | MODC_REG_BANK1,&Index[Modc_ChipNumIndex]);

      Modc_ReadRegister(Modc_ChipNumIndex,MODC_CCP_BLK2_FW_Addr,&Index[Modc_ChipNumIndex]);

      Modc_ReadRegister(Modc_ChipNumIndex,MODC_PWM_PCHG_INIT_Addr,&Index[Modc_ChipNumIndex]);

      Modc_ReadRegister(Modc_ChipNumIndex,MODC_PWM_ICHG_FW_Addr,&Index[Modc_ChipNumIndex]);

      Modc_ReadRegister(Modc_ChipNumIndex,MODC_PWM_PDCHG_INIT_Addr,&Index[Modc_ChipNumIndex]);

      Modc_ReadRegister(Modc_ChipNumIndex,MODC_PWM_ICHGMAX_CCP_BLK3_FW_Addr,&Index[Modc_ChipNumIndex]);

      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];
#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif

    Ret = TRUE;
  }

  return Ret;
}

static uint8 Modc_DisableWatchDog(void)
{
  uint8 Index[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Modc_ChipNumIndex = MODC_CHIP_ONE;
  uint8 Ret = FALSE;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
    {
      /*Setp 1: Set UNLOCK*/
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL1_Addr,0x00A7,&Index[Modc_ChipNumIndex]);

      /*Setp 2: Set WDDIS*/
      //Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,0x4380,&Index[Modc_ChipNumIndex]);

      /*Test: */
      Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_GENCTRL2_Addr,0x438F,&Index[Modc_ChipNumIndex]);

      /*Set CSAxL to 0*/
      /*Modc_WriteRegisterBank0(Modc_ChipNumIndex,MODC_HBIDIAG_Addr,0x0000,&Index[Modc_ChipNumIndex]);*/

      Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Index[Modc_ChipNumIndex];
#if defined(DCU_FL) || defined(DCU_FR)
		}

    Modc_SpiStartTramsmit();
#else
      Modc_SpiStartTramsmit(Modc_ChipNumIndex);
    }
#endif
    Ret = TRUE;
  }

  return Ret;
}
#if 0
static uint8 Modc_ExitFailSafeMode(void)
{
  uint8 Index = 0;
  uint16 *Modc_DevReg = (uint16 *)&Modc_DeviceReg;
  uint8 Ret = FALSE;

  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    /*Setp 1: Clear GENSTAT register*/
    Modc_WriteRegisterBank0(MODC_CHIP_ONE,MODC_GENSTAT_Addr,0x0000,&Index);

    /*Setp 2: Set WDTRIG to 1*/
    Modc_WriteRegisterBank0(MODC_CHIP_ONE,MODC_GENCTRL1_Addr,Modc_DevReg[MODC_GENCTRL1_Addr] | MODC_WTDRIG,&Index);

    /*Setp 3: Set WDTRIG to 0*/
    Modc_WriteRegisterBank0(MODC_CHIP_ONE,MODC_GENCTRL1_Addr,Modc_DevReg[MODC_GENCTRL1_Addr] & (~MODC_WTDRIG),&Index);

    Modc_SpiContext.transferCount = Index;
    Modc_SpiStartTramsmit();
    Ret = TRUE;
  }

  return Ret;
}
#endif
static uint8 Modc_ClearGENSTAT(int ChipNum)
{
  uint8 Index = 0;
  uint8 Ret = FALSE;

  if (TRUE == Modc_CheckSpiTransmitIdle())
  {
    /* Clear GENSTAT register*/
    Modc_WriteRegisterBank0(ChipNum, MODC_GENSTAT_Addr, 0x0000, &Index);

    Modc_SpiOutBuffer[ChipNum].Modc_SetupSize = Index;

#if defined(DCU_FL) || defined(DCU_FR)
    Modc_SpiStartTramsmit();
#else
    Modc_SpiStartTramsmit(ChipNum);
#endif
    Ret = TRUE;
  }
  return Ret;
}

static uint8 Modc_Clear_DSOV_GENSTAT(int ChipNum)
{
  uint8 Index = 0;
  uint8 Ret = FALSE;
  if (TRUE == Modc_CheckSpiTransmitIdle())
  {
    Modc_WriteRegisterBank0(ChipNum, MODC_DSOV_Addr, 0x0000, &Index);/* Clear DSOV register*/   
    Modc_WriteRegisterBank0(ChipNum, MODC_GENSTAT_Addr, 0x0000, &Index); /* Clear GENSTAT register*/
    Modc_SpiOutBuffer[ChipNum].Modc_SetupSize = Index;

#if defined(DCU_FL) || defined(DCU_FR)
    Modc_SpiStartTramsmit();
#else
    Modc_SpiStartTramsmit(ChipNum);
#endif
    Ret = TRUE;
  }
  return Ret;
}

static uint8 Modc_WriteRegCommand_Bank0Bank1(int ChipNum, uint8 RegAddr_Bank0Bank1, uint16 WriteData)
{
  uint8 Index = 0;
  uint8 Ret = FALSE;
  if (TRUE == Modc_CheckSpiTransmitIdle())
  {
    Modc_WriteRegisterBank0(ChipNum, RegAddr_Bank0Bank1, WriteData, &Index);   
    Modc_SpiOutBuffer[ChipNum].Modc_SetupSize = Index;

#if defined(DCU_FL) || defined(DCU_FR)
    Modc_SpiStartTramsmit();
#else
    Modc_SpiStartTramsmit(ChipNum);
#endif
    Ret = TRUE;
  }
  return Ret;
}





/******************************************************************************/
/**
* \brief      Modc_RegisterConfigBrgCtrlHalfBridge
*             configure control register BRG_CTRL1/BRG_CTRL2
* \author
* \param      N/A
* \retval     N/A
* \note
*/
/******************************************************************************/
void Modc_RegisterConfigBrgCtrlHalfBridge(Modc_ChanId_t channel,uint8 SetupParam,uint8 *OutIndex)
{
  uint8 Modc_ShiftBit = 0;
  uint16 Modc_RegValue = 0;
  uint16 *Modc_DevReg;
  uint8 Index = *OutIndex;
  uint16 Modc_SeupParam = 0;
  uint8 Modc_ChipNum = 0;

  if(Index < (MODC_TotalReg - 1))
  {
    Modc_ChipNum = channel / 8;
    Modc_DevReg = (uint16 *)&Modc_DeviceReg[Modc_ChipNum];
    channel %= 8;

    Modc_ShiftBit = channel * 2;
    Modc_RegValue = Modc_DevReg[MODC_HBMODE_Addr] & (~(0x0003 << Modc_ShiftBit));  /*clear the old parameter*/
    Modc_SeupParam = SetupParam;
    Modc_RegValue |= (Modc_SeupParam << Modc_ShiftBit);  /* mergy the new parameter */

    Modc_WriteRegisterBank0(Modc_ChipNum,MODC_HBMODE_Addr,Modc_RegValue,&Index);
  }

  *OutIndex = Index;
}


/******************************************************************************/
/**
* \brief      Modc_RegisterConfigPWMCtrlMapHalfBridge
*             configure control register PWM_CTRL1/PWM_CTRL2
* \author
* \param      N/A
* \retval     N/A
* \note
*/
/******************************************************************************/
void Modc_RegisterConfigPWMCtrlMapHalfBridge(Modc_ChanId_t channel,MODC_PwmChannelId_t PwmChannel,uint8 PwmChannelSwitch,uint8 *OutIndex)
{
  uint8 Modc_ShiftBit = 0;
  uint16 Modc_RegValue = 0;
  uint16 *Modc_DevReg;
  uint8 Index = *OutIndex;
  uint16 Modc_SeupParam = 0;
  uint8 Modc_ChipNum = 0;

  if(Index < (MODC_TotalReg - 1))
  {
    Modc_ChipNum = channel / 8;
    Modc_DevReg = (uint16 *)&Modc_DeviceReg[Modc_ChipNum];
    channel %= 8;

    Modc_ShiftBit = PwmChannel * 4;
    Modc_RegValue = Modc_DevReg[MODC_PWMSET_Addr] & (~(0x0001 << Modc_ShiftBit));  /*clear the old parameter*/
    Modc_SeupParam = PwmChannelSwitch;
    Modc_RegValue |= (Modc_SeupParam << Modc_ShiftBit);  /* mergy the new parameter */

    Modc_ShiftBit++;
    Modc_RegValue &= ~(0x0007 << Modc_ShiftBit);  /*clear the old parameter*/
    Modc_SeupParam = channel;
    Modc_RegValue |= (Modc_SeupParam << Modc_ShiftBit);  /* mergy the new parameter */

    Modc_WriteRegisterBank0(Modc_ChipNum,MODC_PWMSET_Addr,Modc_RegValue,&Index);
  }
  *OutIndex = Index;
}
#if 0
/******************************************************************************/
/**
* \brief      Modc_RegisterConfigPWMCtrlHLHalfBridge
*             configure control register PWM_CTRL3
* \author
* \param      N/A
* \retval     N/A
* \note
*/
/******************************************************************************/
void Modc_RegisterConfigPWMCtrlHLHalfBridge(Modc_ChanId_t channel,uint8 SetupParam,uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint8 Modc_ShiftBit = 0;
  uint8 Modc_RegValue = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.byte.addres = MODC_PWM_CTRL3Addr;
    tempCmd.byte.command = MODC_WRITE;
    Modc_ShiftBit = 8 - channel;
    Modc_RegValue = Modc_DevReg[MODC_PWM_CTRL3Addr] & (~(0x01 << Modc_ShiftBit)); /*clear the old parameter*/
    Modc_RegValue |= (SetupParam << Modc_ShiftBit); /* mergy the new parameter */
    tempCmd.byte.dataToWrite = Modc_RegValue;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_PWM_CTRL3Addr] = Modc_RegValue;
  }
  *OutIndex = Index;
}

/******************************************************************************/
/**
* \brief      Modc_RegisterConfigPWMCtrlFWHalfBridge
*             configure control register PWM_CTRL4
* \author
* \param      N/A
* \retval     N/A
* \note
*/
/******************************************************************************/
void Modc_RegisterConfigPWMCtrlFWHalfBridge(Modc_ChanId_t channel,uint8 SetupParam,uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint8 Modc_ShiftBit = 0;
  uint8 Modc_RegValue = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    tempCmd.byte.addres = MODC_PWM_CTRL4Addr;
    tempCmd.byte.command = MODC_WRITE;
    Modc_ShiftBit = 8 - channel;
    Modc_RegValue = Modc_DevReg[MODC_PWM_CTRL4Addr] & (~(0x01 << Modc_ShiftBit));/*clear the old parameter*/
    Modc_RegValue |= (SetupParam << Modc_ShiftBit);/* mergy the new parameter */
    tempCmd.byte.dataToWrite = Modc_RegValue;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_PWM_CTRL4Addr] = Modc_RegValue;
  }
  *OutIndex = Index;
}
#endif

static MODC_PwmChannelId_t Modc_CheckHBPwmActived(uint8 Channel)
{
  MODC_PwmChannelId_t Modc_PwmChannel = MODC_PWMCH_MAX;
  uint8 Modc_ChipNum = 0;

  Modc_ChipNum = Channel / 8;
  Channel %= 8;

  if((((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x000E) >> 1) == Channel)
    && ((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x0001) == 0x0001))
  {
    Modc_PwmChannel = MODC_PWMCH1;
  }
  else if((((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x00E0) >> 5) == Channel)
    && ((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x0010) == 0x0010))
  {
    Modc_PwmChannel = MODC_PWMCH2;
  }
  else if((((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x0E00) >> 9) == Channel)
      && ((Modc_DeviceReg[Modc_ChipNum].Pwmset_reg & 0x0100) == 0x0100))
  {
    Modc_PwmChannel = MODC_PWMCH3;
  }

  return Modc_PwmChannel;
}
/******************************************************************************/
/**
* \brief      Modc_HalfBridgeGateDriverStateHandle
*             configure the Half bridge Gate Driver State
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_HalfBridgeGateDriverStateHandle(Modc_ChanId_t channel, Modc_HalfBridgeState_t GateDriverState,uint8 *OutLenght)
{
  uint8 Modc_RegCounter = *OutLenght;
  MODC_PwmChannelId_t Modc_PwmCH = MODC_PWMCH_MAX;

  if(channel < MODC_HB_MAX)
  {
    switch(GateDriverState)
    {
      case MODC_HIGH_IMPEDANCE:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        break;
      case MODC_DRIVE_LOW_SIDE:
        Modc_PwmCH = Modc_CheckHBPwmActived(channel);
        if(MODC_PWMCH_MAX != Modc_PwmCH)
        {
          Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, Modc_PwmCH,MODC_PWMCH_OFF,&Modc_RegCounter);
        }
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_LO, &Modc_RegCounter);
        break;
      case MODC_DRIVE_HIGH_SIDE:
        Modc_PwmCH = Modc_CheckHBPwmActived(channel);
        if(MODC_PWMCH_MAX != Modc_PwmCH)
        {
          Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, Modc_PwmCH,MODC_PWMCH_OFF,&Modc_RegCounter);
        }
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI, &Modc_RegCounter);
        break;
      case MODC_PWM1_HS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH1,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI, &Modc_RegCounter);
        break;
      case MODC_PWM1_LS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH1,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_LO, &Modc_RegCounter);
        break;
      case MODC_PWM2_HS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH2,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI, &Modc_RegCounter);
        break;
      case MODC_PWM2_LS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH2,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_LO, &Modc_RegCounter);
        break;
      case MODC_PWM3_HS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH3,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI, &Modc_RegCounter);
        break;
      case MODC_PWM3_LS_ACITVE_FW:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlMapHalfBridge(channel, MODC_PWMCH3,MODC_PWMCH_ON,&Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_LO, &Modc_RegCounter);
        break;
#if 0
      case MODC_PWM_HS_PASSIVE_FW:
        Modc_RegisterConfigPWMCtrlHLHalfBridge(channel, 0x00, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlFWHalfBridge(channel, 0x01, &Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_PWM, &Modc_RegCounter);
        break;
      case MODC_PWM_LS_PASSIVE_FW:
        Modc_RegisterConfigPWMCtrlHLHalfBridge(channel, 0x01, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlFWHalfBridge(channel, 0x01, &Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_PWM, &Modc_RegCounter);
        break;
#endif
      default:
        break;
    }
  }

  *OutLenght = Modc_RegCounter;
}

#if 0
static void Modc_CheckOverHeat(void)
{
  if((Modc_DeviceReg.ic_stat2_reg  & MODC_OTSD ) == MODC_OTSD)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_TSD;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_TSD_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_TSD;
      if(Modc_ErrCnt < Modc_ERR_CNT_TSD_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_TSD_PROT_TIME;
      }
      Modc_DiagState = Modc_DIAG_STATE_ERR_TSD;
    }
  }
}

#endif

/* ok */
static void Modc_CheckOC_HS1(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS1DSOV) == MODC_HS1DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS1 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS1 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS1(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS1DSOV) == MODC_LS1DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS1 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS1 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS2(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS2DSOV) == MODC_HS2DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS2 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS2 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS2(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS2DSOV) == MODC_LS2DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS2 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS2 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS3(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS3DSOV) == MODC_HS3DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS3 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS3 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS3(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS3DSOV) == MODC_LS3DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS3 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS3 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS4(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS4DSOV) == MODC_HS4DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS4 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS4 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS4(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS4DSOV) == MODC_LS4DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS4 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS4 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS5(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS5DSOV) == MODC_HS5DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS5 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS5 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS5(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS5DSOV) == MODC_LS5DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS5 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS5 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS6(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS6DSOV) == MODC_HS6DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS6 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS6 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS6(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS6DSOV) == MODC_LS6DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS6 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS6 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS7(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS7DSOV) == MODC_HS7DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS7 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS7 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS7(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS7DSOV) == MODC_LS7DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS7 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS7 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_HS8(int ChipNum) 
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_HS8DSOV) == MODC_HS8DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS8 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_HS8 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckOC_LS8(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if ((Modc_DeviceReg[ChipNum].Dsov_reg & MODC_LS8DSOV) == MODC_LS8DSOV)
  {
    Modc_InstantErr[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS8 = TRUE;
    if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_ERR_OC_THRES) /* filter */
    {
      Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult[ChipNum].ErrOC.ErrOCBit.ERR_OC_LS8 = TRUE;
      Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_OC;
      if (Modc_ErrCnt[ChipNum] < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

#if 0
static void Modc_CheckFault_LS1(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_L1_VGS ) == MODC_L1_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS1;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS1;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS1(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_H1_VGS ) == MODC_H1_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS1;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS1;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS2(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_L2_VGS ) == MODC_L2_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS2;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS2;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS2(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_H2_VGS ) == MODC_H2_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS2;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS2;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS3(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_L3_VGS ) == MODC_L3_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS3;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS3;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS3(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_H3_VGS ) == MODC_H3_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS3;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS3;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS4(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_L4_VGS ) == MODC_L4_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS4;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS4;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS4(void)
{
  if((Modc_DeviceReg.vgs_stat1_reg & MODC_H4_VGS ) == MODC_H4_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS4;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS4;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS5(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_L5_VGS ) == MODC_L5_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS5;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS5;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS5(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_H5_VGS ) == MODC_H5_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS5;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS5;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS6(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_L6_VGS ) == MODC_L6_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS6;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS6;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS6(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_H6_VGS ) == MODC_H6_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS6;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS6;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS7(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_L7_VGS ) == MODC_L7_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS7;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS7;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS7(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_H7_VGS ) == MODC_H7_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS7;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS7;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_LS8(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_L8_VGS ) == MODC_L8_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_LS8;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_LS8;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}

static void Modc_CheckFault_HS8(void)
{
  if((Modc_DeviceReg.vgs_stat2_reg & MODC_H8_VGS ) == MODC_H8_VGS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_FAULT_HS8;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_Fault_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_FAULT_HS8;
      Modc_DiagState = Modc_DIAG_STATE_ERR_VGS;
      if(Modc_ErrCnt < Modc_ERR_CNT_Fault_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
      }
    }
  }
}
#endif
static void Modc_CheckOverCurrent(int ChipNum)
{
  Modc_CheckOC_HS1(ChipNum);
  Modc_CheckOC_LS1(ChipNum);
  Modc_CheckOC_HS2(ChipNum);
  Modc_CheckOC_LS2(ChipNum);
  Modc_CheckOC_HS3(ChipNum);
  Modc_CheckOC_LS3(ChipNum);
  Modc_CheckOC_HS4(ChipNum);
  Modc_CheckOC_LS4(ChipNum);
  Modc_CheckOC_HS5(ChipNum);
  Modc_CheckOC_LS5(ChipNum);
  Modc_CheckOC_HS6(ChipNum);
  Modc_CheckOC_LS6(ChipNum);
  Modc_CheckOC_HS7(ChipNum);
  Modc_CheckOC_LS7(ChipNum);
  Modc_CheckOC_HS8(ChipNum);
  Modc_CheckOC_LS8(ChipNum);
}


#if 0
static void Modc_CheckGateFault(void)
{
  Modc_CheckFault_LS1();
  Modc_CheckFault_HS1();
  Modc_CheckFault_LS2();
  Modc_CheckFault_HS2();
  Modc_CheckFault_LS3();
  Modc_CheckFault_HS3();
  Modc_CheckFault_LS4();
  Modc_CheckFault_HS4();
  Modc_CheckFault_LS5();
  Modc_CheckFault_HS5();
  Modc_CheckFault_LS6();
  Modc_CheckFault_HS6();
  Modc_CheckFault_LS7();
  Modc_CheckFault_HS7();
  Modc_CheckFault_LS8();
  Modc_CheckFault_HS8();
}

static void Modc_WatchDogFault(void)
{
  if((Modc_DeviceReg.ic_stat2_reg  & MODC_WD_FLT ) == MODC_WD_FLT)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_WDT_FAULT;
    Modc_DiagResult |= MODC_DIAG_ERR_WDT_FAULT;
    Modc_DiagState = Modc_DIAG_STATE_ERR_WDT;
    Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
  }
}
#endif

/******************************************************************************/
/** 
* \brief      Modc_OvUvFault
*             VS under and over voltage fault.
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_OvUvFault(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_VSOV)
  {
    Modc_InstantErr[ChipNum].ErrVSOV = TRUE;
    Modc_DiagResult[ChipNum].ErrVSOV = TRUE;
    Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_VS_OV_UV;
    Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_Fault_PROT_TIME;
  }
  if (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_VSUV)
  {
    Modc_InstantErr[ChipNum].ErrVSUV = TRUE;
    Modc_DiagResult[ChipNum].ErrVSUV = TRUE;
    Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_VS_OV_UV;
    Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_Fault_PROT_TIME;
  }
}


/******************************************************************************/
/** 
* \brief      Modc_CPUVFault
*             CP under voltage fault
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_CPUVFault(int ChipNum)
{
  if (ChipNum >= Modc_SLAVE_DEVICE_NUM)
  {
    return;
  }
  if (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_CPUV)
  {
    Modc_InstantErr[ChipNum].ErrCPUV = TRUE;
    Modc_DiagResult[ChipNum].ErrCPUV = TRUE;
    Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_CPUV;
    Modc_ErrCnt[ChipNum] = Modc_ERR_CNT_Fault_PROT_TIME;
  }
}

boolean Modc_GetOffStateCheckIsCompl(void)   
{
  return Modc_CheckOlscCompl;
}

uint8 Modc_OffState_MotorConnectConf_ParaCheck(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp)
{
  /* regularize ChipNum in range [0-1] */
  if ((ChipNum > MODC_CHIP_TWO))
  {
    return FALSE; /* setup false */
  }
  /* MODC_CHIP_ONE ：regularize x & y in range [ 0 ----7]: [MODC_PRE1Bridge_ONE----MODC_PRE1Bridge_EIGHT] */
  if (MODC_CHIP_ONE == ChipNum)
  {
    if (FALSE == (((MFBComp.xBridgeNum <= MODC_PRE1Bridge_EIGHT) && (MFBComp.xBridgeNum >= MODC_PRE1Bridge_ONE)) &&
                  ((MFBComp.yBridgeNum <= MODC_PRE1Bridge_EIGHT) && (MFBComp.yBridgeNum >= MODC_PRE1Bridge_ONE))))
      return FALSE; /* setup false */
  }
  /* MODC_CHIP_TWO ：regularize x & y in range [ 8 ----15]: [MODC_PRE2Bridge_ONE----MODC_PRE2Bridge_EIGHT] */
  if (MODC_CHIP_TWO == ChipNum)
  {
    if (FALSE == (((MFBComp.xBridgeNum <= MODC_PRE2Bridge_EIGHT) && (MFBComp.xBridgeNum >= MODC_PRE2Bridge_ONE)) &&
                  ((MFBComp.yBridgeNum <= MODC_PRE2Bridge_EIGHT) && (MFBComp.yBridgeNum >= MODC_PRE2Bridge_ONE))))
      return FALSE; /* setup false */
  }
  /* regularize x & y for not equal */
  if ((MFBComp.xBridgeNum == MFBComp.yBridgeNum))
  {
    return FALSE; /* setup false */
  }
  return TRUE;
}

void Modc_OffState_MotorConnectConf_GetBridgeMSBLSB(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp,uint8 *Bridge_MSB,uint8 *Bridge_LSB)
{
  uint8 Bridge_MSB_temp = 0u, Bridge_LSB_temp = 0u;
  Bridge_MSB_temp = ((MFBComp.xBridgeNum) > (MFBComp.yBridgeNum)) ? (MFBComp.xBridgeNum) : (MFBComp.yBridgeNum);
  Bridge_LSB_temp = ((MFBComp.xBridgeNum) < (MFBComp.yBridgeNum)) ? (MFBComp.xBridgeNum) : (MFBComp.yBridgeNum);
  /*  set Bridge_MSB & Bridge_LSB in range: [0-7] */
  if (ChipNum == MODC_CHIP_ONE)
  {
    *Bridge_MSB = Bridge_MSB_temp;
    *Bridge_LSB = Bridge_LSB_temp;
  }
  else
  {
    *Bridge_MSB = Bridge_MSB_temp - MODC_PRE2Bridge_ONE;
    *Bridge_LSB = Bridge_LSB_temp - MODC_PRE2Bridge_ONE;
  }
}

uint8 Modc_OffState_SetMotorConnectConf(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp)
{
  uint8  Bridge_MSB = 0u, Bridge_LSB = 0u,ret = FALSE;
  uint16 SetupHBIDIAG_REGVal = 0u;

  if (FALSE == Modc_OffState_MotorConnectConf_ParaCheck(ChipNum, MFBComp))
  {
    return FALSE;
  }

  Modc_OffState_MotorConnectConf_GetBridgeMSBLSB( ChipNum,  MFBComp,&Bridge_MSB,&Bridge_LSB);

/*   X X X X X X X X  HB8IDIAG HB7IDIAG HB6IDIAG HB5IDIAG HB4IDIAG HB3IDIAG HB2IDIAG HB1IDIAG */
  /* ALL config: IPDDIAG HB1 ON (HB1IDIAG=1), IPDDIAG HB2 OFF (HB2IDIAG=0) */ /* SHORT TO GND :NOT TO CONFIG HBxIDIAG，default:0bit*/
  /*result: open load:  Vsh2:High   Vsh1:LOW */
  /*result: SHORT VBAT:  Vsh2:High   Vsh1:High */
  /* 0u << hight bit ; 1u << low bit */

  /* CLEAR  REG BIT field*/
  SetupHBIDIAG_REGVal = Modc_DeviceReg[ChipNum].Hbidiag_reg & 0xff00U;
  /* set REG BIT field */
  SetupHBIDIAG_REGVal |= (0x00ffU & ((0u << Bridge_MSB) | (1u << Bridge_LSB)));
  /* send cmd  AND update local reg*/
  if (TRUE == Modc_WriteRegCommand_Bank0Bank1(ChipNum, MODC_HBIDIAG_Addr, SetupHBIDIAG_REGVal))
  {
    Modc_DeviceReg[ChipNum].Hbidiag_reg = SetupHBIDIAG_REGVal;
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }
  return ret;
}

uint8 Modc_OffState_MotorConnectConf_GetHBVOUTState(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp, Modc_HBVOUTState_t *Modc_HBVState)
{
  uint8 Bridge_MSB = 0u, Bridge_LSB = 0u, resul_HBxVOUT = 0u, ret = FALSE;

  if (FALSE == Modc_OffState_MotorConnectConf_ParaCheck(ChipNum, MFBComp))
  {
    return FALSE;
  }

  Modc_OffState_MotorConnectConf_GetBridgeMSBLSB(ChipNum, MFBComp, &Bridge_MSB, &Bridge_LSB);

  /* X X X X X X X X HB8VOUT HB7VOUT HB6VOUT HB5VOUT HB4VOUT HB3VOUT HB2VOUT HB1VOUT */
  /* ALL config: IPDDIAG HB1 ON (HB1IDIAG=1), IPDDIAG HB2 OFF (HB2IDIAG=0) */ /* SHORT TO GND :NOT TO CONFIG HBxIDIAG，default:0bit*/
  /*result: open load:  Vsh2:High (HB2Vout = 1)  Vsh1:LOW (HB1Vout = 0)*/
  /*result: SHORT VBAT:  Vsh2:High (HB2Vout = 1)  Vsh1:High (HB1Vout = 1)*/
  /* 0u << hight bit ; 1u << low bit */

  /* get HBxVOUT reg bit field  */
  resul_HBxVOUT = (0xff & Modc_DeviceReg[ChipNum].Hbvout_pwmerr_reg);
  /* select BIT field  for BIT MASK  */
  resul_HBxVOUT &= ((1u << Bridge_MSB) | (1u << Bridge_LSB)); /* BIT select MASK */
  /* SWITCH:compare for resul */
  if (((0u << Bridge_MSB) | (0u << Bridge_LSB)) == resul_HBxVOUT)
  {
    *Modc_HBVState = Modc_HBVOUT_MSB_LSB_LOW_LOW;
    ret = TRUE;
  }
  else if (((1u << Bridge_MSB) | (0u << Bridge_LSB)) == resul_HBxVOUT)
  {
    *Modc_HBVState = Modc_HBVOUT_MSB_LSB_HIGH_LOW;
    ret = TRUE;
  }
  else if (((1u << Bridge_MSB) | (1u << Bridge_LSB)) == resul_HBxVOUT)
  {
    *Modc_HBVState = Modc_HBVOUT_MSB_LSB_HIGH_HIGH;
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }
  return ret;
}

Modc_MotorConnectState_t Modc_OffState_GetMotorConnectState(MODC_ChipNum_t ChipNum, Modc_MotorFullBridgeComp_t MFBComp, Modc_GetMotorConnectStateType_t  GetMCS_Type)
{
  Modc_HBVOUTState_t Modc_HBVState = Modc_HBVOUT_MSB_LSB_MAX;
  Modc_MotorConnectState_t MCState = Modc_UnknownConnect;

  if (Modc_GetMotorCS_SGND ==  GetMCS_Type) /*  short to gnd */
  {
    if (TRUE == Modc_OffState_MotorConnectConf_GetHBVOUTState(ChipNum, MFBComp, &Modc_HBVState))
    {
      if (Modc_HBVOUT_MSB_LSB_LOW_LOW == Modc_HBVState)
      {
        MCState = Modc_ShortToGND;
      }
    }
  }
  else if (Modc_GetMotorCS_SVBAT_OL_NML ==  GetMCS_Type) /* short to vbat,open load,normal */
  {
    if (TRUE == Modc_OffState_MotorConnectConf_GetHBVOUTState(ChipNum, MFBComp, &Modc_HBVState))
    {
      if (Modc_HBVOUT_MSB_LSB_LOW_LOW == Modc_HBVState) /* normal */
      {
        MCState = Modc_NormalConnect;
      }
      else if (Modc_HBVOUT_MSB_LSB_HIGH_LOW == Modc_HBVState) /* open load, */
      {
        MCState = Modc_OpenLoad;
      }
      else if (Modc_HBVOUT_MSB_LSB_HIGH_HIGH == Modc_HBVState) /* short to vbat */
      {
        MCState = Modc_ShortToVBAT;
      }
    }
  }

  return MCState;
}

uint8 Modc_OffStateDiagStepOne(void) /* StepOne Diag short to gnd */
{
  uint8  Detect_SGND_COMPL = 0U;
  if (TRUE == Modc_CheckSpiTransmitIdle()) /* CHIP in  IDLE */
  {
    for (int ChipNum = MODC_CHIP_ONE; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
    {
      for (int Index = 0U; Index < ((MODC_CHIP_ONE == ChipNum) ? (Modc_ChipOneMotorTotal) : (Modc_ChipTwoMotorTotal)); Index++) /* traversing all Motor for ChipOne AND ChipTwo */
      {
        if ( Modc_MotorCS_DS_NULL == Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_DetectStep) /* not yet detected now to detect */
        {
          Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_DetectStep =  Modc_MotorCS_DS_SGND_COMPL;
          if (Modc_ShortToGND == Modc_OffState_GetMotorConnectState(ChipNum, Modc_ChipOfMotorHBComp[ChipNum][Index],  Modc_GetMotorCS_SGND))
          {
            Modc_OLSCDetect[ChipNum][Index].Modc_MotorConnectState = Modc_ShortToGND;
          }
        }
      }
    }
  }
  /* check whether complete */
  for (int ChipNum = MODC_CHIP_ONE; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
  {
    for (int Index = 0U; Index < ((MODC_CHIP_ONE == ChipNum) ? (Modc_ChipOneMotorTotal) : (Modc_ChipTwoMotorTotal)); Index++) /* traversing all Motor for  ChipOne ChipTwo */
    {
      if ( Modc_MotorCS_DS_SGND_COMPL == Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_DetectStep)
      {
         Detect_SGND_COMPL++;
      }
    }
  }
  if ((Modc_ChipOneMotorTotal + Modc_ChipTwoMotorTotal) ==  Detect_SGND_COMPL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

uint8 Modc_OffStateDiagStepTwo(void) /* into condition :(two chip is idle])StepTwo Diag :short to vbat,ol,normal */
{
  static uint8 OffStateDiagST_Step = 0u;
  uint8  SetBreakFlag = FALSE,Detect_SVBAT_OL_NML_COMPL = 0U,ret = FALSE;
  Modc_MotorConnectState_t MotorCS = Modc_UnknownConnect;
  static uint8 OSD_UPDATE_STATUS_REG_COMPL = FALSE,OSD_UPDATE_STATUS_REG_COMPL_ASSIST = FALSE;;

  switch (OffStateDiagST_Step)
  {
  case 0U:/* config*/
  for (int ChipNum = MODC_CHIP_ONE; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
  {
    for (int Index = 0U; Index < ((MODC_CHIP_ONE == ChipNum) ? (Modc_ChipOneMotorTotal) : (Modc_ChipTwoMotorTotal)); Index++) 
    {
      if((Modc_OLSCDetect[ChipNum][Index].Modc_MotorConnectState != Modc_ShortToGND)&&\
      (Modc_MotorCS_ConfigCompl != Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_Conf))/* config only the Motor connet is not shortToGND  */
      {
          if(TRUE == Modc_OffState_SetMotorConnectConf(ChipNum, Modc_ChipOfMotorHBComp[ChipNum][Index]))/*might miss it,it doesn't matter.*/
          {
              Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_Conf = Modc_MotorCS_ConfigCompl;/* config COMPL */              
              OffStateDiagST_Step = 1U;
              SetBreakFlag = TRUE;/* break when config one */
              break;
          }
      }
    }
    if(TRUE ==  SetBreakFlag)
    {
      break;
    }
  }
        
    break;
  case 1U:/* get connet State */

    if (TRUE == OSD_UPDATE_STATUS_REG_COMPL_ASSIST)
    {
      OSD_UPDATE_STATUS_REG_COMPL_ASSIST = FALSE;
      OSD_UPDATE_STATUS_REG_COMPL = TRUE;
    }
    else
    {
      if (TRUE == Modc_OSD_UPDATE_STATUS_REG_REQ)
      {
        Modc_OSD_UPDATE_STATUS_REG_REQ = FALSE;
        OSD_UPDATE_STATUS_REG_COMPL_ASSIST = TRUE;
      }
    }

  if( OSD_UPDATE_STATUS_REG_COMPL )/*  */  
  {                       
    if (TRUE == Modc_CheckSpiTransmitIdle()) /* CHIP in  IDLE */
    {
      OSD_UPDATE_STATUS_REG_COMPL = FALSE;

      for (int ChipNum = MODC_CHIP_ONE; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
      {
        for (int Index = 0U; Index < ((MODC_CHIP_ONE == ChipNum) ? (Modc_ChipOneMotorTotal) : (Modc_ChipTwoMotorTotal)); Index++)
        {
          /* ONLY FOR Modc_MotorCS_ConfigCompl  AND NOT Modc_MotorCS_DS_SVBAT_OL_NML_COMPL */
          if ((Modc_MotorCS_ConfigCompl == Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_Conf) &&
              (Modc_MotorCS_DS_SVBAT_OL_NML_COMPL != Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_DetectStep))
          {
            MotorCS = Modc_OffState_GetMotorConnectState(ChipNum, Modc_ChipOfMotorHBComp[ChipNum][Index], Modc_GetMotorCS_SVBAT_OL_NML);
            if (MotorCS != Modc_UnknownConnect) /* GET correct */
            {
              Modc_OLSCDetect[ChipNum][Index].Modc_MotorConnectState = MotorCS;
              Modc_OLSCDetect[ChipNum][Index].Modc_MotorCS_DetectStep = Modc_MotorCS_DS_SVBAT_OL_NML_COMPL;
              OffStateDiagST_Step = 2U;
              SetBreakFlag = TRUE;/* break when get one */
              break;
            }
          }
        }
        if (TRUE == SetBreakFlag)
        {
          break;
        }
      }
    }
  }
    break;
  case 2U:/* CHECK all motor connet is COMPL? */
    for (int ChipNum = MODC_CHIP_ONE; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
    {
      for (int Index = 0U; Index < ((MODC_CHIP_ONE == ChipNum) ? (Modc_ChipOneMotorTotal) : (Modc_ChipTwoMotorTotal)); Index++)
      {
       if(Modc_UnknownConnect != Modc_OLSCDetect[ChipNum][Index].Modc_MotorConnectState )
       {

          Detect_SVBAT_OL_NML_COMPL++; /*   */
       }
      }
    }
    
    if (Detect_SVBAT_OL_NML_COMPL < (Modc_ChipOneMotorTotal + Modc_ChipTwoMotorTotal))
    {
      OffStateDiagST_Step = 0U; /* not COMPL*/
    }
    else
    {
      OffStateDiagST_Step = 3U; /*COMPL*/
    }

    break;
  case 3U:/* complate */
    ret = TRUE;
    break;

  default:
    break;
  }
  return ret;
}

uint8 Modc_OffStateDiagStep(void)
{
  static uint8 OffStateDiagStep = 0u;
  uint8 ret = FALSE;
  switch (OffStateDiagStep)
  {
  case 0U: /* DiagStep:0U:DiagStepOne   */
    if (TRUE == Modc_OffStateDiagStepOne())
    {
      OffStateDiagStep = 1U;
    }
    break;
  case 1U: /* DiagStep:1U:DiagStepTwo   */
    if (TRUE == Modc_OffStateDiagStepTwo())
    {
      OffStateDiagStep = 2U;
    }
    break;
  case 2U: /* Modc_OffStateDiagStep complate */
    ret = TRUE;
    break;
  default:
    break;
  }
  return ret;
}

/******************************************************************************/
/**
* \brief      Modc_CheckOlsc
*             get the OOL/OSC state from VDS_STATE register
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_CheckOlsc(void)
{
  /* reading status reg for offstate diag*/
  static uint16 OffStateDiagReadRegPeriod = 0;
  if (OffStateDiagReadRegPeriod++ >= MODC_OFFSTATE_DIAG_READ_STATUS_REG_PERIOD)
  {
    OffStateDiagReadRegPeriod = MODC_OFFSTATE_DIAG_READ_STATUS_REG_PERIOD;
    if (FALSE == Modc_NewDataFlag)
    {
      if (TRUE == Modc_ReadStatusRegister()) /* keep reading status reg for use diag */
      {
        Modc_NewDataFlag = TRUE;
        OffStateDiagReadRegPeriod = 0u; 
        Modc_OSD_UPDATE_STATUS_REG_REQ = TRUE;       
      }
    }
  }
   return Modc_OffStateDiagStep();/*  check OffStateDiag*/
}

/******************************************************************************/
/**
* \brief      Modc_GetMotorConnection
*             return the motor connection 
* \author
* \param      N/A
* \retval     
*             
* \note       N/A
*/
/******************************************************************************/
Modc_MotorConnectState_t Modc_GetMotorConnection(MODC_ChipNum_t ChipNum,uint8 MotorNum)
{
  Modc_MotorConnectState_t MCState = Modc_UnknownConnect;

  /* regularize ChipNum&MotorNum in range  */
  if ((MODC_CHIP_ONE == ChipNum)&&(MotorNum < Modc_ChipOneMotorTotal))
  {     
    MCState = Modc_OLSCDetect[MODC_CHIP_ONE][MotorNum].Modc_MotorConnectState ;
  }else if ((MODC_CHIP_TWO == ChipNum)&&(MotorNum < Modc_ChipTwoMotorTotal))
  {
    MCState = Modc_OLSCDetect[MODC_CHIP_TWO][MotorNum].Modc_MotorConnectState ;
  }
  
  return MCState;
}

/******************************************************************************/
/** 
* \brief      Modc_GetStatusRegFault
* 
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_GetStatusRegFault(int ChipNum)
{
  Modc_DiagFAULT[ChipNum].fault_OC1 = (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_OC1) ? (1U) : (0U);
  Modc_DiagFAULT[ChipNum].fault_OC2 = (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_OC2) ? (1U) : (0U);
  Modc_DiagFAULT[ChipNum].fault_VSUV = (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_VSUV) ? (1U) : (0U);
  Modc_DiagFAULT[ChipNum].fault_VSOV = (Modc_DeviceReg[ChipNum].Genstat_reg & MODC_GENSTAT_VSOV) ? (1U) : (0U);
  Modc_DiagFAULT[ChipNum].fault_DSOV = ((Modc_DeviceReg[ChipNum].Dsov_reg) ? (1U) : (0U));
}

/*____________________________ L O C A L - F U N C T I O N S ______________________________________________*/
/***********************************************************************************************************/
/**
* \brief      Diagnosis the device.
*
* \author     Kevin ZHAO
* \param      N/A
* \retval     N/A
* \note       (1) Diganosis statemachine must run in prior to main statemachine to use stable receiving status.
*             (2) Modc_DiagResult will be available for Modc_ERR_CNT_OC_PROT_TIME/Modc_ERR_CNT_TSD_PROT_TIME
*                 only, application should poll this result in a shorter time to determine error status.
*/
/***********************************************************************************************************/
static void Modc_DiagDevice(void)
{
  for (int ChipNum = 0; ChipNum < Modc_SLAVE_DEVICE_NUM; ChipNum++)
  {
    Modc_GetStatusRegFault(ChipNum);

    switch (Modc_DiagState[ChipNum])
    {
    case Modc_DIAG_STATE_ERR_FREE:
      if (Modc_DiagFAULT[ChipNum].FAULT) /* ChipNum have FAULT */
      {
        Modc_ErrCnt[ChipNum] += Modc_ERR_CNT_PLUS_STEP;
        Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_TBC;
        Modc_ErrRestoreCnt[ChipNum] = 0;
      }
      else
      {
        Modc_InstantErr[ChipNum].DiagState = MODC_DIAG_ERR_FREE;       
        if (Modc_DiagResult[ChipNum].DiagState != MODC_DIAG_ERR_FREE)
        {
          Modc_ErrRestoreCnt[ChipNum]++;
          if (Modc_ErrRestoreCnt[ChipNum] > 1000)
          {
            Modc_DiagResult[ChipNum].DiagState = MODC_DIAG_ERR_FREE;
          }
        }
      }
      break;

    case Modc_DIAG_STATE_ERR_TBC:
      /*something fault happened*/
      if (Modc_DiagFAULT[ChipNum].FAULT) /* HAVE FAULT */
      {
        /*Polling for false flags*/
        Modc_ErrorCheck(ChipNum);
      }
      else
      {
        Modc_InstantErr[ChipNum].DiagState = MODC_DIAG_ERR_FREE;
        if (Modc_ErrCnt[ChipNum] > Modc_ERR_CNT_MINUS_STEP)
        {
          Modc_ErrCnt[ChipNum] -= Modc_ERR_CNT_MINUS_STEP;
        }
        else
        {
          Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_FREE;
        }
      }
      break;

    case Modc_DIAG_STATE_ERR_TSD:
    case Modc_DIAG_STATE_ERR_WDT:
    case Modc_DIAG_STATE_ERR_CPUV:
    case Modc_DIAG_STATE_ERR_VS_OV_UV:         
    case Modc_DIAG_STATE_ERR_VGS: 
    case Modc_DIAG_STATE_ERR_OC: 
/*       DealyReadErrFlag = (DealyReadErrFlagCount++ > 10U) ? (1U) : (0U);
      if (Modc_DiagFAULT[ChipNum].FAULT && DealyReadErrFlag) 
      {
        DealyReadErrFlagCount = DealyReadErrFlag = FALSE;
        if (TRUE == Modc_CheckSpiTransmitIdle()) 
        {
          Modc_Clear_DSOV_GENSTAT(ChipNum);
          Modc_DiagState[ChipNum] = Modc_DIAG_STATE_;
        }
      }  */         
    case Modc_DIAG_STATE_ERR:/*Fault establishment*/
      if (Modc_ErrCnt[ChipNum] >= Modc_ERR_CNT_RECOVERY_STEP)
      {
        Modc_ErrCnt[ChipNum] -= Modc_ERR_CNT_RECOVERY_STEP;
      }
      else
      {
        Modc_DiagState[ChipNum] = Modc_DIAG_STATE_RECOVERY;
      }
      break;

    case Modc_DIAG_STATE_SUSPEND:
      Modc_InstantErr[ChipNum].DiagState = MODC_DIAG_ERR_FREE;
      break;

    case Modc_DIAG_STATE_RECOVERY:/* recovery device */
      if (Modc_DiagFAULT[ChipNum].FAULT) /* HAVE FAULT */
      {
        if (TRUE == Modc_CheckSpiTransmitIdle()) /* Modc_TransmitFlagStep[ChipNum] */
        {
          Modc_Clear_DSOV_GENSTAT(ChipNum);
          Modc_DiagState[ChipNum] = Modc_DIAG_STATE_RECOVERY_REQ_OK;
        }
      }
    break;

    case Modc_DIAG_STATE_RECOVERY_REQ_OK: /* RECOVERY_REQ complete */                  
      if (!Modc_DiagFAULT[ChipNum].FAULT ) /* NO FAULT */
      {      
        Modc_InstantErr[ChipNum].DiagState = MODC_DIAG_ERR_FREE;
        /*NO FAULT  clear  Modc_DiagResult */
        Modc_DiagResult[ChipNum].DiagState = MODC_DIAG_ERR_FREE;
        memset((void *)&Modc_DiagResult[ChipNum], 0x00, sizeof(Modc_DiagAllFault_t)); /* ok  */
        Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_FREE;
      }
      else /* HAVE FAULT  continue TO CHECK FAULT*/
      {
        Modc_DiagState[ChipNum] = Modc_DIAG_STATE_ERR_TBC;
      }
      break;

    default:
      break;
    }
  }

  /* reading  status reg for diag*/
  static uint16 DiagReadRegPeriod = 0;
  if (DiagReadRegPeriod++ >= MODC_DIAG_READ_STATUS_REG_PERIOD)
  {
    DiagReadRegPeriod = 0;
    if (FALSE == Modc_NewDataFlag)
    {
      if (TRUE == Modc_ReadStatusRegister()) /* ok */ /* keep reading status reg for use diag */
      {        
        Modc_NewDataFlag = TRUE;
      }
    }
  }
        
}

/******************************************************************************/
/** 
* \brief      Modc_ErrorJudge
* 
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_ErrorCheck(int ChipNum)
{
  /* Modc_CheckOverHeat(); */
    Modc_CheckOverCurrent(ChipNum);
 // Modc_CheckGateFault();   /* gate connet diag */
 /*  Modc_WatchDogFault(); */
    Modc_OvUvFault(ChipNum);  /* undervoltage and overvoltage*/
    Modc_CPUVFault(ChipNum);/* Charge Pump undervoltage */ 
}

static void Modc_ClearFaultSts(int ChipNum)
{
  Modc_ClearGENSTAT(ChipNum);
}


/******************************************************************************/
/**
* \brief      Modc_CommunicationIsOk
*
* \author
* \param      N/A
* \retval     TRUE: communication is OK, FALSE:  communicaiton is not OK
* \note       N/A
*/
/******************************************************************************/
static boolean Modc_CommunicationIsOk(void)
{
  boolean ret = FALSE;
  uint8 Modc_ChipNum = 0;
	
  for(Modc_ChipNum = 0; Modc_ChipNum < Modc_SLAVE_DEVICE_NUM; Modc_ChipNum++)
  {
    if(Modc_DeviceReg[Modc_ChipNum].Dev_id != Modc_DeviceID[Modc_ChipNum])
    {
      break;
    }
  }

  if(Modc_ChipNum == Modc_SLAVE_DEVICE_NUM)
  {
    ret = TRUE;   /*communication is OK*/
  }

  return ret;
}

#if  0
static void Modc_SpiCommunicationCheck(void)
{
  if (!((boolean)Modc_GetDeviceWakeUpState()))
  {
    wakeFilterCnt = 0;
    Modc_Running = FALSE;
  }
  else
  {
    if (wakeFilterCnt < MODC_SPI_WAKE_FILTER_CNT)
    {
      wakeFilterCnt++;
    }
    else
    {
      Modc_Running = TRUE;
    }
  }

  if (Modc_FrameTransmitted)
  {
    if (Modc_Running
     && !Modc_CommunicationIsOk())
    {
      if (Modc_SpiErrorCounter < MODC_SPI_ERROR_COUNTER)
      {
        Modc_SpiErrorCounter++;
      }
      else
      {
        Modc_CommErr = TRUE;
      }
    }
    else
    {
      Modc_CommErr = FALSE;
      Modc_SpiErrorCounter = 0U;
    }
  }
}
#endif
#if 0 /*not used*/
static boolean Modc_IsDisableTimeFinished(void)
{
  boolean ret = FALSE;

  if( Modc_ErrCnt > Modc_ERR_CNT_MINUS_STEP )
  {
    Modc_ErrCnt -= Modc_ERR_CNT_MINUS_STEP;
  }
  else
  {
    ret = TRUE;
  }

  return ret;
}

/*register detail operation*/
static void Modc_RegLock(void)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_MainCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_MainCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_LOCK_START_BIT, MODC_LOCK_WIDTH);
   tempCmd.byte.dataToWrite |= MODC_LOCK;
   
   Modc_SetWriteBuffer(MODC_MainCtrAddr, tempCmd.Word);
}

static void Modc_RegUnLock(void)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_MainCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_MainCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_LOCK_START_BIT, MODC_LOCK_WIDTH);
   tempCmd.byte.dataToWrite |= MODC_UNLOCK;
   
   Modc_SetWriteBuffer(MODC_MainCtrAddr, tempCmd.Word);
}

static void Modc_SetDeadTime(uint8 cmd)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_CurrentCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_CurrentCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_TDEAD_START_BIT, MODC_TDEAD_WIDTH);
   tempCmd.byte.dataToWrite |= cmd;
   
   Modc_SetWriteBuffer(MODC_CurrentCtrAddr, tempCmd.Word);
}

static void Modc_WatchDogEn(void)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_CurrentCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_CurrentCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_WD_EN_START_BIT, MODC_WD_EN_WIDTH);
   tempCmd.byte.dataToWrite |= MODC_WD_EN;
   
   Modc_SetWriteBuffer(MODC_CurrentCtrAddr, tempCmd.Word);
}

static void Modc_WatchDogDisable(void)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_CurrentCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_CurrentCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_WD_EN_START_BIT, MODC_WD_EN_WIDTH);
   
   Modc_SetWriteBuffer(MODC_CurrentCtrAddr, tempCmd.Word);
}



static void Modc_WatchDogDlyTime(uint8 cmd)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_CurrentCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_CurrentCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_WD_DLY_START_BIT, MODC_WD_DLY_WIDTH);
   tempCmd.byte.dataToWrite |= cmd;
   Modc_SetWriteBuffer(MODC_CurrentCtrAddr, tempCmd.Word);
}


static void Modc_SetCurrent(uint8 cmd)
{
   Modc_SpiOutData_t tempCmd;
   
   tempCmd.Word = Modc_GetWriteBuffer(MODC_CurrentCtrAddr);
   /*dumy operation in case the buffer is polluted*/
   tempCmd.byte.addres = MODC_CurrentCtrAddr;
   tempCmd.byte.command = MODC_WRITE;
   
   tempCmd.byte.dataToWrite &= Modc_BitsToZero(MODC_IDRIVE_START_BIT, MODC_IDRIVE_WIDTH);
   tempCmd.byte.dataToWrite |= cmd;
   Modc_SetWriteBuffer(MODC_CurrentCtrAddr, tempCmd.Word);
}
#endif

static void Modc_HalfBridgeRegHandle(void)
{
  Modc_ChanId_t l_CurrChanID = 0;
  Modc_ChanDirec_t l_Direct = 0;
  Modc_ChanQueue_t * l_queue = &Modc_TxChannelQueue;
  uint8 Modc_OutLenght[Modc_SLAVE_DEVICE_NUM] = {0};
  uint8 Modc_ChipNum = 0;
  uint8 Modc_ChipNumIndex = 0;

  if(TRUE == Modc_CheckSpiTransmitIdle())
  {
    /*check, if we have new channel to update */
    while ( TRUE == Modc_ReadFromQueue( l_queue, &l_CurrChanID,  &l_Direct) )
    {
      Modc_ChipNum = l_CurrChanID / 8;
      Modc_HalfBridgeGateDriverStateHandle(l_CurrChanID,l_Direct,&Modc_OutLenght[Modc_ChipNum]);
      Modc_RemoveFromQueue(l_queue,NULL);
      if(Modc_OutLenght[Modc_ChipNum] != 0)
      {
        Modc_NewDataFlag = TRUE;
      }
    }

    if(TRUE == Modc_NewDataFlag)
    {
      for(Modc_ChipNumIndex = MODC_CHIP_ONE; Modc_ChipNumIndex < Modc_SLAVE_DEVICE_NUM; Modc_ChipNumIndex++)
      {
        Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize = Modc_OutLenght[Modc_ChipNumIndex];
#if defined(DCU_FL) || defined(DCU_FR)
			}

      Modc_SpiStartTramsmit();
#else
        if(Modc_SpiOutBuffer[Modc_ChipNumIndex].Modc_SetupSize != 0)
        {
          Modc_SpiStartTramsmit(Modc_ChipNumIndex);
        }
      }
#endif
    }
  }
}
/******************************************************************************/
/**
* \brief      MODC_ChannelSetupReuqest
* \author
* \param      N/A
* \retval     N/A
* \note:   Request to change the channel register setting
           returns true, if request accept
           return  false, if request refuse
*/
/******************************************************************************/
boolean MODC_ChannelSetupReuqest(MODC_HalfBridgeNum_t Channel, MODC_GateDriverState_t GateDriverState)
{
  boolean Accepted = FALSE;

  if(Channel < (Modc_SLAVE_DEVICE_NUM * 8))
  {
    if(GateDriverState < MODC_GDS_MAX)
    {
      Accepted = Modc_AppendToQueue(Channel,GateDriverState);
    }
  }

  return Accepted;
}

/******************************************************************************/
/**
* \brief      Modc_AppendToQueue
* \author
* \param      N/A
* \retval     N/A
* \note:   Append an ID to the back of queue,
           returns true if ID has been added,
           return false, if queue is full or ID already exist
*/
/******************************************************************************/
static boolean Modc_AppendToQueue( Modc_ChanId_t Id, Modc_ChanDirec_t Direct)
{
  boolean l_is_queued = FALSE;

  Modc_ChanQueue_t * l_queue  = &Modc_TxChannelQueue;

  if ( FALSE == Modc_FindIdInQueue( l_queue, Id ) )
  {
    if ( TRUE == Modc_AddToQueue( l_queue, Id, Direct) )
    {
      l_is_queued = TRUE;
    }
    else
    {
      ;
    }
  }
  else
  {
    // already in, no change
    ;
  }

  return (l_is_queued);
}

/******************************************************************************/
/**
* \brief      Modc_FindIdInQueue
* \author
* \param      N/A
* \retval     N/A
* \note:   Find selected ID in queue,
           returns TRUE if ID has been found,
           otherwise return FALSE
*/
/******************************************************************************/
static boolean Modc_FindIdInQueue ( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id )
{
  const uint8 in  = pQueue->In;
  uint8 out = pQueue->Out;

  /* Step from Out to In */
  while (out != in)
  {
    if (pQueue->Buff[out].ChannelId == Id)
    {
      break;
    }

    ++out;
    if (out >= pQueue->Size)
    {
      /* buffer overflow -> start at the begin */
      out = 0;
    }
  }

  return (out != in);
}

/******************************************************************************/
/**
* \brief      Modc_AddToQueue
* \author
* \param      N/A
* \retval     N/A
* \note :  Append an ID to the back of queue,
           returns true if ID has been added,
          return false, if queue is full
*/
/******************************************************************************/
static boolean Modc_AddToQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id, Modc_ChanDirec_t Direct)
{
  boolean Result = FALSE;

  Modc_ChIdInQue_t ChIdDirecTemp;
  const uint8 in = pQueue->In;
  const uint8 out = pQueue->Out;
  uint8 next = in + 1;

  /* prepare next index to be written */
  if ( next >= pQueue->Size )
  {
    /* buffer overflow -> start at the begin */
    next = 0;
  }

  /* is there free place in queue ? */
  if ( next != out )
  {
    /* yes -> free place
        -> write one ID on the input index
        -> shift input index */

    ChIdDirecTemp.ChannelId = Id;
    ChIdDirecTemp.Direction = Direct;
    pQueue->Buff[in] = ChIdDirecTemp;
    pQueue->In = next;

    Result = TRUE;
  }

  return Result;
}

/******************************************************************************/
/**
* \brief      Modc_ReadFromQueue
* \author
* \param      N/A
* \retval     N/A
* \note :  Reads an ID from the head of queue,
          ID is written there, where points pId
          returns true if an ID has been read,
          return false, if queue is empty
          ID will not be removed from queue !!!!!!!!
*/
/******************************************************************************/
static boolean Modc_ReadFromQueue( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId, Modc_ChanDirec_t * pDirect)
{
  boolean Result = FALSE;
  const uint8 in = pQueue->In;
  const uint8 out = pQueue->Out;

  if( in != out )
  {
    *pId = pQueue->Buff[out].ChannelId;
    *pDirect = pQueue->Buff[out].Direction;
    Result = TRUE;
  }
  else
  {
    /*the queue is empty*/
    ;
  }

  return Result;
}

/******************************************************************************/
/**
* \brief      Modc_RemoveFromQueue
* \author
* \param      N/A
* \retval     N/A
/*\note :  Removes an ID from the head of queue,
           ID is written there, where points pId
           returns true if an ID has been removed,
           return false, if queue is empty
*/
/******************************************************************************/
static boolean Modc_RemoveFromQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId )
{
  boolean Result = FALSE;
  const uint8 in = pQueue->In;
  uint8 out = pQueue->Out;
  Modc_ChIdInQue_t ChIdDirecTemp;

  /* is queue empty ? */
  if( in != out )
  {
    /* queue is not empty
        - read the oldest ID
        - shift read (output) index */
    if ( NULL != pId )
    {
      *pId = pQueue->Buff[out].ChannelId;
    }

    /* push invalid index here to not reuse it */
    ChIdDirecTemp.ChannelId = MODC_INVALID_CHANNEL_ID;
    ChIdDirecTemp.Direction = MODC_INVALID_CHANNED_DIRECT;
    pQueue->Buff[out] = ChIdDirecTemp;
    ++out;

    if ( out >= pQueue->Size )
    {
      /* buffer overflow -> start at the begin */
      out = 0;
    }

    pQueue->Out = out;

    Result = TRUE;
  }

  return Result;
}


/*PRQA S 2985--*/
/*PRQA S 4397--*/

/*______ E N D _____ (predc.c) ____________________________________________*/

