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
#define  Modc_DIAG_STATE_ERR_PVDD_OV_UV   4U /* In VM Undervoltage state*/
#define  Modc_DIAG_STATE_ERR_VCPUV        5U /* In VCP Undervoltage state*/
#define  Modc_DIAG_STATE_ERR_OC           6U /* In over-current protection state*/
#define  Modc_DIAG_STATE_ERR_VGS          7U /* In gate driver fault state*/
#define  Modc_DIAG_STATE_ERR              8U /* error that need re-start device*/
#define  Modc_DIAG_STATE_WARN             9U /* error that don't need re-start device*/

#define  Modc_DIAG_STATE_RECOVERY         10U /* In recovery state*/
#define  Modc_DIAG_STATE_SUSPEND          11U /* In suspend state*/


#define BYTE_BITS_LEN                (8U)
#define FAULT_REG_INIT_VALUE         (0xFFU)
#define MODC_IC_STATE1_SPI_OK        (0x80U)
#define MODC_IC_STATE3_DEFAULT_VALUE (0x08U)
#define MODC_SPI_DBT_VALUE		       (0x5U) /*delay between transfers*/
#define MODC_SPI_DSP_VALUE		       (0x01U) /*delay SCK to PCS*/
#define MODC_SPI_DPS_VALUE		       (0x01U) /*delay PCS to SCK*/

#define MODC_SPI_WAKE_FILTER_CNT    (4U)

#define MODC_SPI_ERROR_COUNTER      (20U)

#define MODC_TxQueueSizeChan         MODC_HB_MAX
/*____________________________ L O C A L - T Y P E S ______________________________________________________*/
/** \brief  xxxx*/

/*____________________________ L O C A L - S T R U C T U R E ______________________________________________*/
/** \brief xxx*/

/*____________________________ G L O B A L - D A T A ______________________________________________________*/


/*____________________________ L O C A L - D A T A ________________________________________________________*/
/** SPI Out/In message */
static Modc_SpiOutData_t Modc_SpiOutData[MODC_TotalReg] = {0U};
static Modc_SpiInData_t Modc_SpiInData[MODC_TotalReg] = {0U};

static SPIC_Reqtype_t Modc_SpiContext;
static uint64 Modc_DiagResult = MODC_DIAG_ERR_FREE;
static uint64 Modc_InstantErr = MODC_DIAG_ERR_FREE;

/** Modc state*/
static uint8 Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_IDLE;
static uint8 Modc_State = Modc_STATE_SLEEP;
static uint8 Modc_DiagState = Modc_DIAG_STATE_ERR_FREE;
static uint16 Modc_ErrCnt = 0;
static uint16 Modc_ErrRestoreCnt = 0;
static boolean Modc_Running = FALSE;
static boolean Modc_FrameTransmitted = FALSE;
static boolean Modc_CommErr = FALSE;
static uint8 Modc_OLSCDetect = 0;  /*Offline Short Circuit and Open Load Detection*/
static uint8 Modc_SMCount = 0;    /*State Machine Counter*/
static uint8 Modc_NewDataFlag = FALSE;
static uint8 wakeFilterCnt;
static uint8 Modc_SpiErrorCounter = 0U;
static Modc_DRV8718S_Q1_REG_t Modc_DeviceReg;
static uint8 Modc_TransmitTimeOutCnt = 0;

Modc_ChIdInQue_t Modc_TxQueueChan[MODC_TxQueueSizeChan + 1];

Modc_ChanQueue_t Modc_TxChannelQueue =
{
  0, 0, MODC_TxQueueSizeChan + 1,  Modc_TxQueueChan
};

/*____________________________ L O C A L - F U N C T I O N S - P R O T O T Y P E S ________________________*/
static void Modc_SendData(void);
static void Modc_ParseSpiRxData(void);
static void Modc_StateMachine(void);
static void Modc_ErrorCheck(void);
static uint16 Modc_GetWriteBuffer(uint8 regID);
static void Modc_SetWriteBuffer(uint8 regID, uint16 cmd);
static uint8 Modc_BitsToZero(uint8 startBit, uint8 bitwidth);
static uint8 Modc_InitRegister(void);
static boolean Modc_CommunicationIsOk(void);
static void Modc_VcpUvFault(void);
static void Modc_DiagDevice(void);
static void Modc_SpiCommunicationCheck(void);
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
static void Modc_ClearFaultSts(void);
static void Modc_SpiStartTramsmit(void);
static void Modc_HalfBridgeRegHandle(void);
static uint8 Modc_ReadStatusRegister(void);
static uint8 Modc_ReadALLRegister(void);
static boolean Modc_AppendToQueue( Modc_ChanId_t Id, Modc_ChanDirec_t Direct);
static boolean Modc_FindIdInQueue ( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id );
static boolean Modc_AddToQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t Id ,Modc_ChanDirec_t Direct);
static boolean Modc_ReadFromQueue( const Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId, Modc_ChanDirec_t * pDirect );
static boolean Modc_RemoveFromQueue( Modc_ChanQueue_t * pQueue, Modc_ChanId_t * pId );
static uint8 Modc_EnableDevice(void);
static void Modc_CheckOlsc(void);
static void Modc_WDReset(void);
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
  (void)memset(Modc_SpiInData,  0x0, sizeof(Modc_SpiInData));
  (void)memset(Modc_SpiOutData,  0x0, sizeof(Modc_SpiOutData));

  Modc_SpiContext.receBuff = (void *)Modc_SpiInData;
  Modc_SpiContext.sendBuff = (void *)Modc_SpiOutData;
  
  /*(void)Spi_SetHWUnitAsyncMode(LPSPI_2_HWUNIT, SPI_INTERRUPT_MODE);*/
  Modc_State = Modc_STATE_WAKEUP_DEVICE;
  Modc_NewDataFlag = FALSE;
  Modc_ErrCnt = 0;
  Modc_ErrRestoreCnt = 0;
  Modc_OLSCDetect = 0;
  Modc_SMCount = 0;
  Modc_TransmitTimeOutCnt = 0;
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
    Modc_ParseSpiRxData();
    Modc_SpiCommunicationCheck();
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
uint8 MODC_GetRegisterValue(uint8 regID)
{
   uint8 result = 0x0;
   uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
   if(regID < MODC_TotalReg)
   {
      result = Modc_DevReg[regID];
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
* \brief      Return the error present status for the driver.
*
* \author
* \param      N/A
* \retval     Modc_DiagResult - error representative level
* \note       N/A
*/
/***********************************************************************************************************/
boolean MODC_DiagOverCurrernt(void)
{
 return ((Modc_DiagResult == MODC_DIAG_ERR_OC_HS1)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS1)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS2)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS2)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS3)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS3)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS4)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS4)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS5)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS5)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS6)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS6)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS7)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS7)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_HS8)\
  ||(Modc_DiagResult == MODC_DIAG_ERR_OC_LS8));
}

/***********************************************************************************************************/
/**
* \brief      Return the error present status for the driver.
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
uint64 MODC_GetErrorPresent(void)
{
  return Modc_DiagResult;
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
  return (uint64)Modc_InstantErr;
}

void MODC_TxCompleteCallback(void)
{
  /*WD_TaskRefreshWdg(WD_SPI_MODC);*/
}

/***********************************************************************************************************/
/**
* \brief      MODC_GetRunningState
*             Get the deivce running or not
* \author
* \param      N/A
* \retval     TRUE: running  FALSE: not running
* \note       N/A
*/
/***********************************************************************************************************/
boolean MODC_GetRunningState(void)
{
  return Modc_Running;
}

/***********************************************************************************************************/
/**
* \brief      MODC_VerifyCommunication
*             Get the SPI communication error flag
* \author
* \param      N/A
* \retval     TRUE: spi error  FALSE: spi ok
* \note       N/A
*/
/***********************************************************************************************************/
boolean MODC_VerifyCommunication(void)
{
  return Modc_CommErr;
}

/***********************************************************************************************************/
/**
* \brief      MODC_ResetDevice
*             Reset the whole device
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
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
      Modc_State = Modc_STATE_READ_STATE_REGISTER;
      lastModcState = Modc_STATE_WAKEUP_DEVICE;
      break;

    case Modc_STATE_READ_STATE_REGISTER:
      if(TRUE == Modc_ReadALLRegister())
      {
        Modc_FrameTransmitted = TRUE;
        Modc_State = Modc_STATE_DETECT_DEVICE;
      }
      lastModcState = Modc_STATE_READ_STATE_REGISTER;
      break;

    case Modc_STATE_DETECT_DEVICE:
      if(Modc_CommunicationIsOk())/*communication is OK*/
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
      if(TRUE == Modc_InitRegister())
      {
        Modc_SMCount = 0;
        Modc_State = Modc_STATE_OOL_OSC_STATUS;
      }
      lastModcState = Modc_STATE_INIT_REGISTER;
      break;

    case Modc_STATE_OOL_OSC_STATUS:
      /* Wait for OOL OSC is done*/
      Modc_SMCount++;
      if(Modc_SMCount >= Modc_OLSC_DETECT_TIME)
      {
        Modc_SMCount = Modc_OLSC_DETECT_TIME;
        Modc_CheckOlsc();

        if(TRUE == Modc_EnableDevice())
        {
          Modc_State = Modc_STATE_READ_STATUS;
          Modc_SMCount = 0;
        }
      }
      else
      {
        Modc_ReadStatusRegister();
      }
      lastModcState = Modc_STATE_OOL_OSC_STATUS;
      break;

    case Modc_STATE_READ_STATUS:
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
      else
      {
        Modc_HalfBridgeRegHandle();
      }

      if(FALSE == Modc_NewDataFlag)
      {
        if(TRUE == Modc_ReadStatusRegister())
        {
          Modc_NewDataFlag = TRUE;
        }
      }
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
      Modc_NewDataFlag = FALSE;

      lastModcState = Modc_STATE_READ_STATUS;
      break;
    
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
    default:
      break;
  }
}

/***********************************************************************************************************/
/**
* \brief      Modc_SpiStartTramsmit
*             Set to start transmit SPI data
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/***********************************************************************************************************/
static void Modc_SpiStartTramsmit(void)
{
  Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_READ_WRITE;
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
static void Modc_ParseSpiRxData(void)
{
  uint8 i = 0;
  uint8 *Modc_Reg = (uint8 *)&Modc_DeviceReg;
  Spi_SeqResultType SpiSeqResult = SPI_SEQ_FAILED;

  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_CHECK)
  {
    SpiSeqResult = Modc_SpiGetSequenceResult();
    if (SPI_SEQ_OK == SpiSeqResult)
    {
      for(i = 0; i < Modc_SpiContext.transferCount; i++)
      {
        if(MODC_READ == Modc_SpiOutData[i].byte.command)
        {
          if(Modc_SpiOutData[i].byte.addres < sizeof(Modc_DRV8718S_Q1_REG_t))
          {
            Modc_Reg[Modc_SpiOutData[i].byte.addres] = Modc_SpiInData[i].byte.Value;
          }
        }
      }

      Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_IDLE;
    }
  }
}

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
  Spi_SeqResultType SpiSeqResult = SPI_SEQ_FAILED;

  switch(Modc_TransmitFlagStep)
  {
    case MODC_TRANSMIT_STEP_IDLE:
      break;

    case MODC_TRANSMIT_STEP_READ_WRITE:
      SpiSeqResult = Modc_SpiGetSequenceResult();
      if (SPI_SEQ_OK == SpiSeqResult)
      {
        Modc_SpiTransmitFrame(&Modc_SpiContext);
        Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_CHECK;
        Modc_TransmitTimeOutCnt = 0;
      }
      break;

    case MODC_TRANSMIT_STEP_CHECK:
      Modc_TransmitTimeOutCnt++;
      if(Modc_TransmitTimeOutCnt >= Modc_SPI_TRANSMIT_TIMEOUT)
      {
        Modc_TransmitTimeOutCnt = 0;
        Modc_TransmitFlagStep = MODC_TRANSMIT_STEP_IDLE;
      }
      break;

    default:
      break;
  }
}

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
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Ret = FALSE;

  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    /*for status register --7*/
    tempCmd.byte.addres = MODC_IC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_DI_OLSC | MODC_UNLOCK;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_IC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x1F*/
    tempCmd.byte.addres = MODC_VDS_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL1;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x20*/
    tempCmd.byte.addres = MODC_VDS_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL2;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL2Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x21*/
    tempCmd.byte.addres = MODC_VDS_CTRL3Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL3;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL3Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x22*/
    tempCmd.byte.addres = MODC_VDS_CTRL4Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL4;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL4Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x23*/
    tempCmd.byte.addres = MODC_OLSC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = 0x00;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_OLSC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x24*/
    tempCmd.byte.addres = MODC_OLSC_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = 0x00;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_OLSC_CTRL2Addr] = tempCmd.byte.dataToWrite;
#ifdef MODC_WATCH_DOG_ENABLE
    /*for status register --0x08*/
    tempCmd.byte.addres = MODC_IC_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_IC_CTRL2;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_IC_CTRL2Addr] = tempCmd.byte.dataToWrite;
#endif
    /*for status register --7*/
    tempCmd.byte.addres = MODC_IC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_EN_DRV | MODC_DI_OLSC | MODC_UNLOCK;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_IC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    Modc_SpiContext.transferCount = Index;

    Modc_SpiStartTramsmit();
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
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Ret = FALSE;

  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    /*for status register --7*/
    tempCmd.byte.addres = MODC_IC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_UNLOCK; //Unlock all control registers.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_IC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --B*/
    tempCmd.byte.addres = MODC_PWM_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_PWM_CTRL1;//半桥1   半桥23 ：in1 ， 半桥4 5： in2   
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_PWM_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --C*/
    tempCmd.byte.addres = MODC_PWM_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_PWM_CTRL2;// 半桥67  in3  ,半桥8 in1   
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_PWM_CTRL2Addr] = tempCmd.byte.dataToWrite;

    /*for status register --18*/
    tempCmd.byte.addres = MODC_DRV_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_DRV_CTRL1;//VDS fault independent shutdown mode configuration.VGS fault independent shutdown mode configuration.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_DRV_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --1B*/
    tempCmd.byte.addres = MODC_DRV_CTRL4Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_DRV_CTRL4;//Insertable digital dead-time 2 μs for half-bridge
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_DRV_CTRL4Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x1F*/
    tempCmd.byte.addres = MODC_VDS_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL1_OLSC; //AB Half-bridge  VDS overcurrent monitor threshold 2V.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x20*/
    tempCmd.byte.addres = MODC_VDS_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL2_OLSC;//AB Half-bridge  VDS overcurrent monitor threshold 2V.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL2Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x21*/
    tempCmd.byte.addres = MODC_VDS_CTRL3Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL3_OLSC;//AB Half-bridge  VDS overcurrent monitor threshold 2V.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL3Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x22*/
    tempCmd.byte.addres = MODC_VDS_CTRL4Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_VDS_CTRL4_OLSC;//AB Half-bridge  VDS overcurrent monitor threshold 2V.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_VDS_CTRL4Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x25*/
    tempCmd.byte.addres = MODC_UVOV_CTRLAddr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_UVOV_CTRL;//set the undervoltage and overvoltage monitor configurations.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_UVOV_CTRLAddr] = tempCmd.byte.dataToWrite;

    /*for status register --0x26*/
    tempCmd.byte.addres = MODC_CSA_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_REG_CSA_CTRL1;//gain and reference voltage for shunt amplifier
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_CSA_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --7*/
    tempCmd.byte.addres = MODC_IC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = MODC_EN_OLSC | MODC_UNLOCK;//Enable offline open load and short circuit diagnostic.;Unlock all control registers.
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_IC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x23*/
    tempCmd.byte.addres = MODC_OLSC_CTRL1Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = 0x00;// 0x26 0010 0110  ;offline diagnostic current sources for half-bridges
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_OLSC_CTRL1Addr] = tempCmd.byte.dataToWrite;

    /*for status register --0x24*/
    tempCmd.byte.addres = MODC_OLSC_CTRL2Addr;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = 0x00;//0x64 0110 0100 ;offline diagnostic current sources for half-bridges
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[MODC_OLSC_CTRL2Addr] = tempCmd.byte.dataToWrite;

    Modc_SpiContext.transferCount = Index;
    Modc_SpiStartTramsmit();

    
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
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 Modc_RegAddr = 0;
  uint8 Ret = FALSE;
  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    for(Modc_RegAddr = MODC_IC_STAT1Addr; Modc_RegAddr <= MODC_IC_STAT3Addr; Modc_RegAddr++)
    {
      tempCmd.byte.addres = Modc_RegAddr;
      tempCmd.byte.command = MODC_READ;
      tempCmd.byte.dataToWrite = 0x00;
      Modc_SpiOutData[Index++] = tempCmd;
    }

    Modc_SpiContext.transferCount = Index;

    Modc_SpiStartTramsmit();

    Ret = TRUE;
  }

  return Ret;
}

/******************************************************************************/
/**
* \brief      Modc_ReadALLRegister
*             Read all the register value from device
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static uint8 Modc_ReadALLRegister(void)
{
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 Modc_RegAddr = 0;
  uint8 Ret = FALSE;
  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    for(Modc_RegAddr = MODC_IC_STAT1Addr; Modc_RegAddr < MODC_RSVD_CTRLAddr; Modc_RegAddr++)
    {
      
      tempCmd.byte.addres = Modc_RegAddr;
      tempCmd.byte.command = MODC_READ;
      tempCmd.byte.dataToWrite = 0x00;
      Modc_SpiOutData[Index++] = tempCmd;
    }

    Modc_SpiContext.transferCount = Index;

    Modc_SpiStartTramsmit();

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
  Modc_SpiOutData_t tempCmd = {0};
  uint8 Modc_BridgeReg = 0;
  uint8 Modc_ShiftBit = 0;
  uint8 Modc_RegValue = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    Modc_BridgeReg = (channel <= 4) ? MODC_BRG_CTRL1Addr : MODC_BRG_CTRL2Addr;

 


    Modc_ShiftBit = (3 - ((channel - 1) % 4)) << 1;
    Modc_RegValue = Modc_DevReg[Modc_BridgeReg] & (~(0x03 << Modc_ShiftBit));  /*clear the old parameter*/
    Modc_RegValue |= (SetupParam << Modc_ShiftBit);  /* mergy the new parameter */



    tempCmd.byte.addres = Modc_BridgeReg;
    tempCmd.byte.command = MODC_WRITE;
    tempCmd.byte.dataToWrite = Modc_RegValue;


    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[Modc_BridgeReg] = Modc_RegValue;
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
void Modc_RegisterConfigPWMCtrlMapHalfBridge(Modc_ChanId_t channel,uint8 SetupParam,uint8 *OutIndex)
{
  Modc_SpiOutData_t tempCmd = {0};
  uint8 Modc_BridgeReg = 0;
  uint8 Modc_ShiftBit = 0;
  uint8 Modc_RegValue = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Index = *OutIndex;

  if(Index < (MODC_TotalReg - 1))
  {
    Modc_BridgeReg = (channel <= 4) ? MODC_PWM_CTRL1Addr : MODC_PWM_CTRL2Addr;
    tempCmd.byte.addres = Modc_BridgeReg;
    tempCmd.byte.command = MODC_WRITE;
    Modc_ShiftBit = (3 - ((channel - 1) % 4)) << 1;
    Modc_RegValue = Modc_DevReg[Modc_BridgeReg] & (~(0x03 << Modc_ShiftBit));  /*clear the old parameter*/
    Modc_RegValue |= (SetupParam << Modc_ShiftBit);  /* mergy the new parameter */
    tempCmd.byte.dataToWrite = Modc_RegValue;
    Modc_SpiOutData[Index++] = tempCmd;
    Modc_DevReg[Modc_BridgeReg] = Modc_RegValue;
  }
  *OutIndex = Index;
}

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

  if(channel < MODC_HB_MAX)
  {
    channel++;
    switch(GateDriverState)
    {
      case MODC_HIGH_IMPEDANCE:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI_Z, &Modc_RegCounter);
        break;
      case MODC_DRIVE_LOW_SIDE:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_LO, &Modc_RegCounter);
        break;
      case MODC_DRIVE_HIGH_SIDE:
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_HI, &Modc_RegCounter);
        break;
      case MODC_PWM_HS_ACITVE_FW:
        Modc_RegisterConfigPWMCtrlHLHalfBridge(channel, 0x00, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlFWHalfBridge(channel, 0x00, &Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_PWM, &Modc_RegCounter);
        break;
      case MODC_PWM_LS_ACITVE_FW:
        Modc_RegisterConfigPWMCtrlHLHalfBridge(channel, 0x01, &Modc_RegCounter);
        Modc_RegisterConfigPWMCtrlFWHalfBridge(channel, 0x00, &Modc_RegCounter);
        Modc_RegisterConfigBrgCtrlHalfBridge(channel, MODC_HB_PWM, &Modc_RegCounter);
        break;
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
      default:
        break;
    }
  }

  *OutLenght = Modc_RegCounter;
}

/******************************************************************************/
/**
* \brief      Modc_CheckOverHeat
*             Check whether over heat status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS1
*             Check whether LS1 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS1(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_L1_VDS ) == MODC_L1_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS1;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS1;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS1
*             Check whether HS1 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS1(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_H1_VDS ) == MODC_H1_VDS)
  {
  
    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS1;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS1;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS2
*             Check whether LS2 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS2(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_L2_VDS ) == MODC_L2_VDS)
  {
  
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS2;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS2;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS2
*             Check whether HS2 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS2(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_H2_VDS ) == MODC_H2_VDS)
  {
  
    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS2;
    
    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS2;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS3
*             Check whether LS3 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS3(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_L3_VDS ) == MODC_L3_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS3;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS3;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS3
*             Check whether HS3 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS3(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_H3_VDS ) == MODC_H3_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS3;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS3;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS4
*             Check whether LS4 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS4(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_L4_VDS ) == MODC_L4_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS4;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS4;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS4
*             Check whether HS4 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS4(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_H4_VDS ) == MODC_H4_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS4;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS4;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS5
*             Check whether LS5 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS5(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_L5_VDS ) == MODC_L5_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS5;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS5;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS5
*             Check whether HS5 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS5(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_H5_VDS ) == MODC_H5_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS5;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS5;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS6
*             Check whether LS6 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS6(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_L6_VDS ) == MODC_L6_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS6;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS6;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS6
*             Check whether HS6 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS6(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_H6_VDS ) == MODC_H6_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS6;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS6;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS7
*             Check whether LS7 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS7(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_L7_VDS ) == MODC_L7_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS7;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS7;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS7
*             Check whether HS7 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS7(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_H7_VDS ) == MODC_H7_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS7;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS7;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_LS8
*             Check whether LS8 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_LS8(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_L8_VDS ) == MODC_L8_VDS)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OC_LS8;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_LS8;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckOC_HS8
*             Check whether HS8 over current status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOC_HS8(void)
{
  if((Modc_DeviceReg.vds_stat2_reg & MODC_H8_VDS ) == MODC_H8_VDS)
  {

    Modc_InstantErr |= MODC_DIAG_ERR_OC_HS8;

    if(Modc_ErrCnt < Modc_ERR_CNT_ERR_OC_THRES)
    {
      Modc_ErrCnt += Modc_ERR_CNT_PLUS_STEP;
    }
    else
    {
      Modc_DiagResult |= MODC_DIAG_ERR_OC_HS8;
      Modc_DiagState = Modc_DIAG_STATE_ERR_OC;
      if(Modc_ErrCnt < Modc_ERR_CNT_OC_PROT_TIME)
      {
        Modc_ErrCnt = Modc_ERR_CNT_OC_PROT_TIME;
      }
    }
  }
}

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS1
*             Check whether LS1 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS1
*             Check whether HS1 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS2
*             Check whether LS2 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS2
*             Check whether HS2 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS3
*             Check whether LS3 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS3
*             Check whether HS3 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS4
*             Check whether LS4 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS4
*             Check whether HS4 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS5
*             Check whether LS5 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS5
*             Check whether HS5 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS6
*             Check whether LS6 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS6
*             Check whether HS6 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS7
*             Check whether LS7 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS7
*             Check whether HS7 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_LS8
*             Check whether LS8 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckFault_HS8
*             Check whether HS8 gate fault status present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_CheckOverCurrent
*             Check Over Current
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_CheckOverCurrent(void)
{
  Modc_CheckOC_LS1();
  Modc_CheckOC_HS1();
  Modc_CheckOC_LS2();
  Modc_CheckOC_HS2();
  Modc_CheckOC_LS3();
  Modc_CheckOC_HS3();
  Modc_CheckOC_LS4();
  Modc_CheckOC_HS4();
  Modc_CheckOC_LS5();
  Modc_CheckOC_HS5();
  Modc_CheckOC_LS6();
  Modc_CheckOC_HS6();
  Modc_CheckOC_LS7();
  Modc_CheckOC_HS7();
  Modc_CheckOC_LS8();
  Modc_CheckOC_HS8();
}

/******************************************************************************/
/**
* \brief      Modc_CheckGateFault
*             Check Gate Fault
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/**
* \brief      Modc_WatchDogFault
*             Check whether watch dog fault present
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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

/******************************************************************************/
/** 
* \brief      Modc_OvUvFault
*             VM under voltage fault.
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_OvUvFault(void)
{
  if(((Modc_DeviceReg.ic_stat2_reg  & MODC_PVDD_UV ) == MODC_PVDD_UV)
    || ((Modc_DeviceReg.ic_stat2_reg  & MODC_PVDD_OV ) == MODC_PVDD_OV))
  {
    Modc_InstantErr |= MODC_DIAG_ERR_OV_UV_FAULT;
    Modc_DiagResult |= MODC_DIAG_ERR_OV_UV_FAULT;
    Modc_DiagState = Modc_DIAG_STATE_ERR_PVDD_OV_UV;
    Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
  }
}

/******************************************************************************/
/** 
* \brief      Modc_VcpUvFault
*             VCP under voltage fault
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_VcpUvFault(void)
{
  if((Modc_DeviceReg.ic_stat2_reg  & MODC_VCP_UV ) == MODC_VCP_UV)
  {
    Modc_InstantErr |= MODC_DIAG_ERR_VCP_UV_FAULT;
    Modc_DiagResult |= MODC_DIAG_ERR_VCP_UV_FAULT;
    Modc_DiagState = Modc_DIAG_STATE_ERR_VCPUV;
    Modc_ErrCnt = Modc_ERR_CNT_Fault_PROT_TIME;
  }
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
static void Modc_CheckOlsc(void)
{
  if((Modc_DeviceReg.vds_stat1_reg & MODC_RWL_NORMAL_DETECT) == MODC_RWL_NORMAL_DETECT) /* RWL 接在HB2 HB3 所以<<4 和 <<2  ;B但是 应该判读4bit 其中一个置位 就认为 fault*/
  {
    Modc_OLSCDetect &= ~(0x01 << MODC_HB_RWL);
  }
  else  /*RWL motor connection abnormal*/
  {
    Modc_OLSCDetect |= (0x01 << MODC_HB_RWL);
  }

  if(((Modc_DeviceReg.vds_stat1_reg & MODC_HANDLE_X_NORMAL_DETECT) == MODC_HANDLE_X_NORMAL_DETECT)
      && ((Modc_DeviceReg.vds_stat2_reg & MODC_HANDLE_Y_NORMAL_DETECT) == MODC_HANDLE_Y_NORMAL_DETECT))
  {
    Modc_OLSCDetect &= ~(0x01 << MODC_HB_HANDLE);
  }
  else  /*HANDLE motor connection abnormal*/
  {
    Modc_OLSCDetect |= (0x01 << MODC_HB_HANDLE);
  }

  if((Modc_DeviceReg.vds_stat2_reg & MODC_CINCH_NORMAL_DETECT) == MODC_CINCH_NORMAL_DETECT)
  {
    Modc_OLSCDetect &= ~(0x01 << MODC_HB_CHINCH);
  }
  else  /*CINCH motor connection abnormal*/
  {
    Modc_OLSCDetect |= (0x01 << MODC_HB_CHINCH);
  }
}

/******************************************************************************/
/**
* \brief      MODC_GetMotorConnection
*             return the motor connection is correct or not.
* \author
* \param      N/A
* \retval     TRUR: Connection is OK
*             FAILE: Connection is not OK,may be open load or short to GND or short
*                    to battery, need to check the connection.
* \note       N/A
*/
/******************************************************************************/
uint8 MODC_GetMotorConnection(MODC_HalfBridgeNum_t Modc_Channel)
{
  uint8 Ret = FALSE;
  switch(Modc_Channel)
  {
    case MODC_HB_TWO:
    case MODC_HB_THREE:
      if(0x00 == (Modc_OLSCDetect & (0x01 << MODC_HB_RWL)))
      {
        Ret = TRUE;
      }
      break;
    case MODC_HB_FOUR:
    case MODC_HB_FIVE:
      if(0x00 == (Modc_OLSCDetect & (0x01 << MODC_HB_HANDLE)))
      {
        Ret = TRUE;
      }
      break;
    case MODC_HB_SIX:
    case MODC_HB_SEVEN:
      if(0x00 == (Modc_OLSCDetect & (0x01 << MODC_HB_CHINCH)))
      {
        Ret = TRUE;
      }
      break;
    default:
      break;
  }

  return Ret;
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
 switch(Modc_DiagState)
 {
   case Modc_DIAG_STATE_ERR_FREE:
     if(((Modc_DeviceReg.ic_stat1_reg & MODC_FAULT ) == MODC_FAULT) && (Modc_DeviceReg.ic_stat1_reg != FAULT_REG_INIT_VALUE))
     {
       Modc_ErrCnt+=Modc_ERR_CNT_PLUS_STEP;
       Modc_DiagState = Modc_DIAG_STATE_ERR_TBC;
       Modc_ErrRestoreCnt = 0;
     }
     else
     {
       Modc_InstantErr = MODC_DIAG_ERR_FREE;

       if(Modc_DiagResult != MODC_DIAG_ERR_FREE)
       {
         Modc_ErrRestoreCnt++;
         if(Modc_ErrRestoreCnt > Modc_ERR_CNT_RESTORE_TIME)
         {
           Modc_DiagResult = MODC_DIAG_ERR_FREE;
         }
       }
     }
     break;
 
   case Modc_DIAG_STATE_ERR_TBC:
     /*something fault happened*/
     if((Modc_DeviceReg.ic_stat1_reg & MODC_FAULT ) == MODC_FAULT)
     {
       /*Polling for false flags*/
       Modc_ErrorCheck();
     }
     else
     {
       Modc_InstantErr = MODC_DIAG_ERR_FREE;
       if( Modc_ErrCnt > Modc_ERR_CNT_MINUS_STEP )
       {
         Modc_ErrCnt-=Modc_ERR_CNT_MINUS_STEP;
       }
       else
       {
         Modc_DiagState = Modc_DIAG_STATE_ERR_FREE;
       }
     }
     break;
     
   case Modc_DIAG_STATE_ERR_TSD:
   case Modc_DIAG_STATE_ERR_WDT:
   case Modc_DIAG_STATE_ERR_PVDD_OV_UV:
   case Modc_DIAG_STATE_ERR_VCPUV:
   case Modc_DIAG_STATE_ERR_VGS:
   case Modc_DIAG_STATE_ERR_OC:
     /*Fault establishment, clear flag*/      
     /*if((Modc_DeviceReg.ic_stat1_reg & MODC_FAULT ) == MODC_FAULT)
     {
       if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
       {
         Modc_ClearFaultSts();
       }
     }*/

     if(Modc_ErrCnt >= Modc_ERR_CNT_RECOVERY_STEP)
     {
       Modc_ErrCnt-=Modc_ERR_CNT_RECOVERY_STEP;
     }
     else
     { 
       Modc_DiagState = Modc_DIAG_STATE_RECOVERY;
     }
     break;
 
   case Modc_DIAG_STATE_SUSPEND:
     Modc_InstantErr = MODC_DIAG_ERR_FREE;

     break;
     
   case Modc_DIAG_STATE_RECOVERY:
     if((Modc_DeviceReg.ic_stat1_reg & MODC_FAULT ) != MODC_FAULT)
     {
       Modc_InstantErr = MODC_DIAG_ERR_FREE;
       Modc_DiagState = Modc_DIAG_STATE_ERR_FREE;
     }

     break;
   
   default:
     break;
 }
}

/******************************************************************************/
/** 
* \brief      Modc_ErrorCheck
* 
* \author
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Modc_ErrorCheck(void)
{
  Modc_CheckOverHeat();
  Modc_CheckOverCurrent();
  Modc_CheckGateFault();
  Modc_WatchDogFault();
  Modc_OvUvFault();
  Modc_VcpUvFault();
}

/******************************************************************************/
/**
* \brief      Modc_ClearFaultSts
*             Clear fault status
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Modc_ClearFaultSts(void)
{
  Modc_SpiOutData_t tempCmd;
  uint8 Index = 0;
  uint8 *Modc_DevReg = (uint8 *)&Modc_DeviceReg;
  uint8 Modc_RegValue = 0;
  /*for status register --7*/
  Modc_RegValue = Modc_DevReg[MODC_IC_CTRL1Addr];
  tempCmd.byte.addres = MODC_IC_CTRL1Addr;
  tempCmd.byte.command = MODC_WRITE;
  tempCmd.byte.dataToWrite = Modc_RegValue | MODC_CLR_FLT;
  Modc_SpiOutData[Index++] = tempCmd;
  Modc_DevReg[MODC_IC_CTRL1Addr] = tempCmd.byte.dataToWrite;

  Modc_SpiContext.transferCount = Index;

  Modc_SpiStartTramsmit();
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

  if(((Modc_DeviceReg.ic_stat1_reg & MODC_IC_STATE1_SPI_OK) == MODC_IC_STATE1_SPI_OK)
      && (Modc_DeviceReg.ic_stat3_reg == MODC_IC_STATE3_DEFAULT_VALUE))/*communication is OK*/
  {
    ret = TRUE;
  }

  return ret;
}

/******************************************************************************/
/**
* \brief      Modc_SpiCommunicationCheck
*             Spi communication handler
* \author
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
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
  uint8 Modc_OutLenght = 0;

  if(Modc_TransmitFlagStep == MODC_TRANSMIT_STEP_IDLE)
  {
    /*check, if we have new channel to update */
    while ( TRUE == Modc_ReadFromQueue( l_queue, &l_CurrChanID,  &l_Direct) )
    {
      Modc_HalfBridgeGateDriverStateHandle(l_CurrChanID,l_Direct,&Modc_OutLenght);
      Modc_RemoveFromQueue(l_queue,NULL);
      Modc_NewDataFlag = TRUE;
    }

    if(TRUE == Modc_NewDataFlag)
    {
      Modc_SpiContext.transferCount = Modc_OutLenght;
      Modc_SpiStartTramsmit();
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

  if(Channel < MODC_HB_MAX)
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

