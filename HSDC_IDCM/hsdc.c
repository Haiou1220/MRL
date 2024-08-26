/***********************************************************************************************************/
/**
* \file       hsdc.c
* \brief      High-side driver control
* \details    N/A
*
* \author     lub
* \date       2023-07-03
* \par        History:
*
*/
/********************* (C) Copyright 2023 Magneti Marelli Guangzhou ****************************************/

/*_____ I N C L U D E - F I L E S ____________________________________________*/
#include "hsdc.h"
#include "string.h"
#include "EOL_Main.h"

/*_____ L O C A L - D E F I N E S ____________________________________________*/

/*_____ L O C A L - T Y P E S ________________________________________________*/

/*_____ G L O B A L - D A T A ________________________________________________*/

/*_____ P R I V A T E - D A T A ______________________________________________*/

/*_____ L O C A L - D A T A __________________________________________________*/
static uint8 Hsdc_PingBuffer[HSDC_MAX_INDEX_NUM];
static uint8 Hsdc_PongBuffer[HSDC_MAX_INDEX_NUM];
static uint8 Hsdc_QueuePingId[HSDC_MAX_INDEX_NUM];
static uint8 Hsdc_QueuePongId[HSDC_MAX_INDEX_NUM];
static uint8 Hsdc_OutputIndexTotalNumPing = 0u;
static uint8 Hsdc_OutputIndexTotalNumPong = 0u;
static Hsdc_IndexTableType_t Hsdc_UsingPingOrPong = HSDC_INDEX_TABLE_PING;

static uint8 Hsdc_TxBuffer[HSDC_MAX_INDEX_NUM + 1];
static uint8 Hsdc_RxBuffer[HSDC_MAX_INDEX_NUM + 1];

static uint8 Hsdc_OutputDataTable[HSDC_MAX_INDEX_NUM] = {
  READ_REG_OUT,
  READ_REG_RCS,
  READ_REG_SRC,
  READ_REG_OCR,
  READ_REG_RCD,
  READ_REG_KRC,
  READ_REG_PCS,
  READ_REG_HWCR,
  READ_REG_ICS,
  READ_REG_DCR,
  READ_REG_WRN,
  READ_REG_STD,
  READ_REG_ERR,

  WRITE_REG_OUT,
  WRITE_REG_OCR,
  WRITE_REG_RCD,
  WRITE_REG_KRC,
  WRITE_REG_PCS,
  WRITE_REG_HWCR,
  WRITE_REG_ICS,
  WRITE_REG_DCR,
};

static SPIC_Reqtype_t Hsdc_Req;
static Hsdc_AllRegister_t Hsdc_AllRegister = {0u};
static boolean Hsdc_InitResetFlag = FALSE;
static boolean Hsdc_SwitchRegFlag = FALSE;
static Hsdc_WRN_ReadReg_t Hsdc_WrnDiag = {0u};
static Hsdc_STD_ReadReg_t Hsdc_StdDiag = {0u};
static Hsdc_ERR_ReadReg_t Hsdc_ErrDiag = {0u};
static uint8 Hsdc_ResetTimer = 0u;
static uint8 Hsdc_RestartTimer = 0u;
static uint8 Hsdc_RestartCount = 0u;
static uint8 Hsdc_ReadRegTimer = 0u;
static uint8 Hsdc_ReadRegCount = 0u;
/*_____ L O C A L - F U N C T I O N S - P R O T O T Y P E S __________________*/
static void Hsdc_SendCmdHandler(void);
static boolean Hsdc_CheckQueneID(Hsdc_CmdIndexTable_t index, uint8 data);
static uint8 Hsdc_QueneByID(Hsdc_CmdIndexTable_t id);
static uint8 Hsdc_QueneToBuffer(Hsdc_CmdIndexTable_t id, uint8 data);
static void Hsdc_WriteReg_OUT(uint8 channel);
static void Hsdc_WriteReg_OCR(uint8 channel);
static void Hsdc_WriteReg_RCD(uint8 channel);
static void Hsdc_WriteReg_KRC(uint8 channel);
static void Hsdc_WriteReg_PCS(boolean pcc, boolean clc, boolean src);
static void Hsdc_WriteReg_HWCR(boolean col, boolean rst, boolean clc);
static void Hsdc_WriteReg_ICS(uint8 channel);
static void Hsdc_WriteReg_DCR_MUX(Hsdc_MUX_Channel_t mux);
static void Hsdc_WriteReg_DCR_SWR(boolean swr);
static void Hsdc_WriteRegCheckSwitchReq(boolean reqSwr);
static void Hsdc_PerformReset(void);
static void Hsdc_ConfigInit(void);
static void Hsdc_ReadRegById(Hsdc_CmdIndexTable_t id);
static void Hsdc_UpdateAllRegister(uint8 index, uint8 data);
static void Hsdc_UpdateDiagRegister(void);
static void Hsdc_ErrorHandle(void);
static void Hsdc_ReadDiagRegister(void);
static void Hsdc_ReadAllRegister(void);
static boolean Hsdc_GetInitResetFlag(void);
/*_____ P R I V A T E - F U N C T I O N S - P R O T O T Y P E S _____________*/

/*_____ L O C A L - M A C R O S ______________________________________________*/

/*_____ G L O B A L - F U N C T I O N S ______________________________________*/
/******************************************************************************/
/** 
* \brief      Hsdc_Init 
*
* \author     lub
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Hsdc_Init(void)
{
  /*Init memory*/
  memset(Hsdc_PingBuffer, 0u, sizeof(Hsdc_PingBuffer));
  memset(Hsdc_PongBuffer, 0u, sizeof(Hsdc_PongBuffer));
  memset(Hsdc_RxBuffer, 0u, sizeof(Hsdc_RxBuffer));
  memset(Hsdc_TxBuffer, 0u, sizeof(Hsdc_TxBuffer));
  memset(Hsdc_QueuePingId, 0xFFu, sizeof(Hsdc_QueuePingId));
  memset(Hsdc_QueuePongId, 0xFFu, sizeof(Hsdc_QueuePongId));

  Flexio_Spi_Ip_UpdateTransferMode(1, FLEXIO_SPI_IP_INTERRUPT); /*1:BTS71033*/
  /*perform soft reset*/
  Hsdc_InitResetFlag = FALSE;
  Hsdc_PerformReset();
}

/******************************************************************************/
/** 
* \brief      Hsdc_MainFunction_10ms 
*
* \author     lub
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Hsdc_MainFunction_10ms(void)
{
  if(FALSE == Hsdc_InitResetFlag)
  {
    Hsdc_ResetTimer++;
    if(Hsdc_StdDiag.Byte || Hsdc_WrnDiag.Byte)/*finish reset*/
    {
      Hsdc_InitResetFlag = TRUE;
      Hsdc_ResetTimer = 0u;
      Hsdc_ConfigInit();
    }
    else if(HSDC_RETRY_TIMER <= Hsdc_ResetTimer)/*retry reset*/
    {
      Hsdc_ResetTimer = 0u;
      Hsdc_PerformReset();
    }
  }
  else 
  {
    /*normal run, read all register*/
    Hsdc_ReadRegTimer++;
#if defined(DCU_FL) || defined(DCU_FR)
    if (TRUE != EOL_M_CheckIsRunning())
    {
      Hsdc_SetOutputChannel(OUT_PWR_DOOR_HALL_PWR, HSDC_E_OPEN);
    }
#endif
    if(HSDC_READ_DIAG_TIMER == Hsdc_ReadRegTimer)
    {
      Hsdc_ReadRegTimer = 0u;
      Hsdc_ReadRegCount++;
      if(HSDC_READ_ALL_Count == Hsdc_ReadRegCount)
      {
        Hsdc_ReadRegCount = 0u;
        Hsdc_ReadAllRegister();
      }
      Hsdc_ReadDiagRegister();
    }
  }
  /*update STDDIAG,WRNDIAG,ERRDIAG register data*/
  Hsdc_UpdateDiagRegister();
  /*Error handle*/
  Hsdc_ErrorHandle();
  /*Spi send frame*/
  Hsdc_SendCmdHandler();
}

/******************************************************************************/
/** 
* \brief      Hsdc_EndNotification 
*
* \author     lub
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Hsdc_EndNotification(void)
{
  uint8 i = 0u;
  uint8 *CurBufferPtr, *QueneId, *CurIndexNum;

  if(HSDC_INDEX_TABLE_PING == Hsdc_UsingPingOrPong)
  {
    CurBufferPtr = Hsdc_PongBuffer;
    CurIndexNum = &Hsdc_OutputIndexTotalNumPong;
    QueneId = Hsdc_QueuePongId;
  }
  else
  {
    CurBufferPtr = Hsdc_PingBuffer;
    CurIndexNum = &Hsdc_OutputIndexTotalNumPing;
    QueneId = Hsdc_QueuePingId;
  }
  for (i = 0u; i < *CurIndexNum; i++)
  {
    if(Hsdc_RxBuffer[i + 1] & RW_RESPONSE)
    {
      Hsdc_UpdateAllRegister(QueneId[i], Hsdc_RxBuffer[i + 1u]);
    }
    else
    {
      if(Hsdc_RxBuffer[i + 1] & WRN_STD_RESPONSE)
      {
        Hsdc_AllRegister.WRN.Byte = Hsdc_RxBuffer[i + 1u];
      }
      else
      {
        Hsdc_AllRegister.STD.Byte = Hsdc_RxBuffer[i + 1u];
      }
    }
  }
  memset(QueneId, 0xFFu, (*CurIndexNum));
  *CurIndexNum = 0;
}

/******************************************************************************/
/** 
* \brief      Hsdc_SetOutputChannel 
*
* \author     lub
* \param      chn :
                    OUT_DOOR_PANEL_HEAT,
                    OUT_DOOR_PUDDLE_LAMP,
                    OUT_RESERVED_LED_1,
                    OUT_RESERVED_LED_2,
                    OUT_WC_HALL_PWR,
                    OUT_PWR_DOOR_HALL_PWR,
              sta :
                    HSDC_E_CLOSE = 0u,
                    HSDC_E_OPEN = 1u
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Hsdc_SetOutputChannel(Hsdc_OutputChannel_t chn, Hsdc_OutputControl_t sta)
{
  uint8 curOutput = 0;
  uint8 Hsdc_GetOutputRegValue = 0;
  Hsdc_OUT_WriteReg_t Hsdc_OUT = {0u};
  
  if(FALSE == Hsdc_GetInitResetFlag()) 
    return;
  if(chn < OUT_CHANNEL_MAX)
  {
    Hsdc_GetOutputRegValue = Hsdc_GetOutputChannel();

    Hsdc_OUT.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_OUT];
    curOutput = Hsdc_OUT.Reg.OUTn;
    if(((curOutput >> chn) & 0x01u) != sta)
    {
      if(sta)
      {
        curOutput |= (0x01u << chn);
      }
      else 
      {
        curOutput &= ~(0x01u << chn);
      }
     
    }
    if(Hsdc_GetOutputRegValue != curOutput)
    {
      Hsdc_WriteReg_OUT(curOutput);
    }
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_DisAutoRestart 
*
* \author     lub
* \param      channel :
                        RCD_CHANNEL_0 = 0x02,
                        RCD_CHANNEL_1 = 0x04,
                        RCD_CHANNEL_2 = 0x08
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Hsdc_DisAutoRestart(Hsdc_RCD_Channel_t channel)
{
  Hsdc_WriteReg_RCD(channel);
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetOutputChannel 
*
* \author     lub
* \param      N/A
* \retval     OUT.Reg.OUTn 
* \note       N/A
*/
/******************************************************************************/
uint8 Hsdc_GetOutputChannel(void)
{
  return Hsdc_AllRegister.OUT.Reg.OUTn;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetWarnChannel 
*
* \author     lub
* \param      N/A
* \retval     WrnDiag.Reg.WRNn
* \note       N/A
*/
/******************************************************************************/
uint8 Hsdc_GetWarnChannel(void)
{
  return Hsdc_WrnDiag.Reg.WRNn;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetErrorChannel 
*
* \author     lub
* \param      N/A
* \retval     ErrDiag.Reg.ERRn 
* \note       N/A
*/
/******************************************************************************/
uint8 Hsdc_GetErrorChannel(void)
{
  return Hsdc_ErrDiag.Reg.ERRn;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetStdTransmissionError 
*
* \author     lub
* \param      N/A
* \retval     StdDiag.Reg.TER 
* \note       N/A
*/
/******************************************************************************/
boolean Hsdc_GetStdTransmissionError(void)
{
  return Hsdc_StdDiag.Reg.TER;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetStdChecksumVerification 
*
* \author     lub
* \param      N/A
* \retval     StdDiag.Reg.CSV 
* \note       N/A
*/
/******************************************************************************/
boolean Hsdc_GetStdChecksumVerification(void)
{
  return Hsdc_StdDiag.Reg.CSV;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetStdLimpHomeMonitor 
*
* \author     lub
* \param      N/A
* \retval     StdDiag.Reg.LHI 
* \note       N/A
*/
/******************************************************************************/
boolean Hsdc_GetStdLimpHomeMonitor(void)
{
  return Hsdc_StdDiag.Reg.LHI;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetStdSleepModeMonitor 
*
* \author     lub
* \param      N/A
* \retval     StdDiag.Reg.SLP 
* \note       N/A
*/
/******************************************************************************/
boolean Hsdc_GetStdSleepModeMonitor(void)
{
  return Hsdc_StdDiag.Reg.SLP;
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetStdVsMonitor 
*
* \author     lub
* \param      N/A
* \retval     StdDiag.Reg.VSMON 
* \note       N/A
*/
/******************************************************************************/
boolean Hsdc_GetStdVsMonitor(void)
{
  return Hsdc_StdDiag.Reg.VSMON;
}

/*_____ L O C A L - F U N C T I O N S ________________________________________*/
/******************************************************************************/
/** 
* \brief      Hsdc_SendCmdHandler 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_SendCmdHandler(void)
{
  if(Hsdc_OutputIndexTotalNumPing || Hsdc_OutputIndexTotalNumPong)
  {
    /*SPI sequence is not busy*/
    if(SPI_SEQ_PENDING != Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_4_BTS71033))
    {
      uint8 CurOutputNum = 0u;
      if(HSDC_INDEX_TABLE_PING == Hsdc_UsingPingOrPong)
      {
        CurOutputNum = Hsdc_OutputIndexTotalNumPing;
        memcpy(Hsdc_TxBuffer, Hsdc_PingBuffer, CurOutputNum);
        Hsdc_UsingPingOrPong = HSDC_INDEX_TABLE_PONG;
      }
      else 
      {
        CurOutputNum = Hsdc_OutputIndexTotalNumPong;
        memcpy(Hsdc_TxBuffer, Hsdc_PongBuffer, CurOutputNum);
        Hsdc_UsingPingOrPong = HSDC_INDEX_TABLE_PING;
      }
      if(CurOutputNum > 0)
      {
        /*fill a readonly register to the end of spi buffer*/
        Hsdc_TxBuffer[CurOutputNum] = Hsdc_OutputDataTable[CMD_READ_REG_RCS];
        Hsdc_Req.sendBuff = (void *)Hsdc_TxBuffer;
        Hsdc_Req.receBuff = (void *)Hsdc_RxBuffer;
        Hsdc_Req.transferCount = CurOutputNum + 1;
        (void)Hsdc_SpiTransmitFrame(&Hsdc_Req);
      }
    }
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_CheckQueneID 
*
* \author     lub
* \param      index:(Hsdc_CmdIndexTable_t)
              data: transmit data
* \retval     ret: HSDC_E_NOT_OK / HSDC_E_OK
* \note       N/A
*/
/******************************************************************************/
static boolean Hsdc_CheckQueneID(Hsdc_CmdIndexTable_t index, uint8 data)
{
  boolean ret = HSDC_E_NOT_OK;
  uint8 i = 0u, cnt = 0u;
  uint8 *CurIndexTablePtr, *QueneId;
  uint8 CurIndexNum;

  SuspendAllInterrupts();

  if(HSDC_INDEX_TABLE_PING == Hsdc_UsingPingOrPong)
  {
    CurIndexTablePtr = Hsdc_PingBuffer;
    CurIndexNum = Hsdc_OutputIndexTotalNumPing;
    QueneId = Hsdc_QueuePingId;
  }
  else
  {
    CurIndexTablePtr = Hsdc_PongBuffer;
    CurIndexNum = Hsdc_OutputIndexTotalNumPong;
    QueneId = Hsdc_QueuePongId;
  }

  for (i = 0; i < HSDC_MAX_INDEX_NUM; i++)
  {
    if(index == QueneId[i])
    {
      break;
    }
    cnt++;
  }
  if(HSDC_MAX_INDEX_NUM == cnt)
  {
    QueneId[CurIndexNum] = index;
    CurIndexTablePtr[CurIndexNum] = data;
    CurIndexNum++;

    if(HSDC_INDEX_TABLE_PING == Hsdc_UsingPingOrPong)
    {
      Hsdc_OutputIndexTotalNumPing = CurIndexNum;
    }
    else
    {
      Hsdc_OutputIndexTotalNumPong = CurIndexNum;
    }

    ret = HSDC_E_OK;
  }

  ResumeAllInterrupts();

  return ret;
}

/******************************************************************************/
/** 
* \brief      Hsdc_QueneByID 
*
* \author     lub
* \param      id:(Hsdc_CmdIndexTable_t)
* \retval     ret: HSDC_E_NOT_OK / HSDC_E_OK
* \note       N/A
*/
/******************************************************************************/
static boolean Hsdc_QueneByID(Hsdc_CmdIndexTable_t id)
{
  boolean ret = HSDC_E_NOT_OK;
  if(HSDC_E_OK == Hsdc_CheckQueneID(id, Hsdc_OutputDataTable[id]))//no same id in the quene
  {
    ret = HSDC_E_OK;
  }
  return ret;
}

/******************************************************************************/
/** 
* \brief      Hsdc_QueneToBuffer 
*
* \author     lub
* \param      id:(Hsdc_CmdIndexTable_t)
              data: (uint8)
* \retval     ret: HSDC_E_NOT_OK / HSDC_E_OK
* \note       N/A
*/
/******************************************************************************/
static boolean Hsdc_QueneToBuffer(Hsdc_CmdIndexTable_t id, uint8 data)
{
  boolean ret = HSDC_E_NOT_OK;
  if(HSDC_E_OK == Hsdc_CheckQueneID(id, data))//no same id in the quene
  {
    ret = HSDC_E_OK;
  }
  return ret;
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_OUT 
*
* \author     lub
* \param      out: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_OUT(uint8 out)
{
  Hsdc_OUT_WriteReg_t OUT = {0u};
  OUT.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_OUT];
  OUT.Reg.OUTn = out;
  Hsdc_OutputDataTable[CMD_WRITE_REG_OUT] = OUT.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_OUT, OUT.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_OCR 
*
* \author     lub
* \param      oct: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_OCR(uint8 oct)
{
  Hsdc_OCR_WriteReg_t OCR = {0u};
  OCR.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_OCR];
  OCR.Reg.OCTn = oct;
  Hsdc_OutputDataTable[CMD_WRITE_REG_OCR] = OCR.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_OCR, OCR.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_RCD 
*
* \author     lub
* \param      rcd: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_RCD(uint8 rcd)
{
  Hsdc_RCD_WriteReg_t RCD = {0u};
  RCD.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_RCD];
  RCD.Reg.RCDn = rcd;
  Hsdc_OutputDataTable[CMD_WRITE_REG_RCD] = RCD.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_RCD, RCD.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_KRC 
*
* \author     lub
* \param      krc: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_KRC(uint8 krc)
{
  Hsdc_KRC_WriteReg_t KRC = {0u};
  KRC.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_KRC];
  KRC.Reg.KRCn = krc;
  Hsdc_OutputDataTable[CMD_WRITE_REG_KRC] = KRC.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_KRC, KRC.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_PCS 
*
* \author     lub
* \param      pcc/clc/src: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_PCS(boolean pcc, boolean clc, boolean src)
{
  Hsdc_PCS_WriteReg_t PCS = {0u};
  PCS.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_PCS];
  PCS.Reg.PCCn = pcc;
  PCS.Reg.CLCS = clc;
  PCS.Reg.SRCS = src;
  Hsdc_OutputDataTable[CMD_WRITE_REG_PCS] = PCS.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_PCS, PCS.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_HWCR 
*
* \author     lub
* \param      col/rst/clc: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_HWCR(boolean col, boolean rst, boolean clc)
{
  Hsdc_HWCR_WriteReg_t HWCR = {0u};
  HWCR.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_HWCR];
  HWCR.Reg.COL = col;
  HWCR.Reg.RST = rst;
  HWCR.Reg.CLC = clc;
  Hsdc_OutputDataTable[CMD_WRITE_REG_HWCR] = HWCR.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_HWCR, HWCR.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_ICS 
*
* \author     lub
* \param      ics: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_ICS(uint8 ics)
{
  Hsdc_ICS_WriteReg_t ICS = {0u};
  ICS.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_ICS];
  ICS.Reg.CSRn = ics;
  Hsdc_OutputDataTable[CMD_WRITE_REG_ICS] = ICS.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_ICS, ICS.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_DCR_MUX 
*
* \author     lub
* \param      mux: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_DCR_MUX(Hsdc_MUX_Channel_t mux)
{
  Hsdc_DCR_WriteReg_t DCR = {0u};
  DCR.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_DCR];
  DCR.Reg.MUX = mux;
  Hsdc_OutputDataTable[CMD_WRITE_REG_DCR] = DCR.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_DCR, DCR.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteReg_DCR_SWR 
*
* \author     lub
* \param      swr: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteReg_DCR_SWR(boolean swr)
{
  Hsdc_DCR_WriteReg_t DCR = {0u};
  DCR.Byte = Hsdc_OutputDataTable[CMD_WRITE_REG_DCR];
  DCR.Reg.SWR = swr;
  Hsdc_OutputDataTable[CMD_WRITE_REG_DCR] = DCR.Byte;
  Hsdc_QueneToBuffer(CMD_WRITE_REG_DCR, DCR.Byte);
}

/******************************************************************************/
/** 
* \brief      Hsdc_WriteRegCheckSwitchReq 
*
* \author     lub
* \param      reqSwr: write to register
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_WriteRegCheckSwitchReq(boolean reqSwr)
{
  if((FALSE == Hsdc_SwitchRegFlag) && (TRUE == reqSwr))
  {
    Hsdc_WriteReg_DCR_SWR(TRUE);
  }
  else if((TRUE == Hsdc_SwitchRegFlag) && (FALSE == reqSwr))
  {
    Hsdc_WriteReg_DCR_SWR(FALSE);
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_PerformReset 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_PerformReset(void)
{
  /*soft reset cmd*/
  Hsdc_WriteReg_HWCR(FALSE, TRUE, FALSE);
}

/******************************************************************************/
/** 
* \brief      Hsdc_ConfigInit 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_ConfigInit(void)
{
  Hsdc_WriteReg_DCR_MUX(MUX_ROUT_CHANNEL_0); //exit sleep mode
#if (HSDC_OVERLOAD_LOW_LEVEL == STD_ON)
  Hsdc_WriteReg_OCR(OCR_CHANNEL_0|OCR_CHANNEL_1|OCR_CHANNEL_2);
#endif
}

/******************************************************************************/
/** 
* \brief      Hsdc_ReadRegById 
*
* \author     lub
* \param      id:(Hsdc_CmdIndexTable_t)
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_ReadRegById(Hsdc_CmdIndexTable_t id)
{
  if(READ_REG_MAX > id)
  {
    Hsdc_QueneByID(id);
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_UpdateAllRegister 
*
* \author     lub
* \param      index:(CMD id)
              data :(update data)
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_UpdateAllRegister(uint8 index, uint8 data)
{
  switch (index)
  {
  case CMD_READ_REG_OUT:
    Hsdc_AllRegister.OUT.Byte = data;
    break;
  case CMD_READ_REG_RCS:
    Hsdc_AllRegister.RCS.Byte = data;
    break;
  case CMD_READ_REG_SRC:
    Hsdc_AllRegister.SRC.Byte = data;
    break;
  case CMD_READ_REG_OCR:
    Hsdc_AllRegister.OCR.Byte = data;
    break;
  case CMD_READ_REG_RCD:
    Hsdc_AllRegister.RCD.Byte = data;
    break;
  case CMD_READ_REG_KRC:
    Hsdc_AllRegister.KRC.Byte = data;
    break;
  case CMD_READ_REG_PCS:
    Hsdc_AllRegister.PCS.Byte = data;
    break;
  case CMD_READ_REG_HWCR:
    Hsdc_AllRegister.HWCR.Byte = data;
    break;
  case CMD_READ_REG_ICS:
    Hsdc_AllRegister.ICS.Byte = data;
    break;
  case CMD_READ_REG_DCR:
    Hsdc_AllRegister.DCR.Byte = data;
    break;
  case CMD_READ_REG_WRN:
    Hsdc_AllRegister.WRN.Byte = data;
    break;
  case CMD_READ_REG_STD:
    Hsdc_AllRegister.STD.Byte = data;
    break;
  case CMD_READ_REG_ERR:
    Hsdc_AllRegister.ERR.Byte = data;
    break;
  default:
    break;
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_UpdateDiagRegister 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_UpdateDiagRegister(void)
{
  if(Hsdc_AllRegister.WRN.Byte & 0x3Fu)
  {
    Hsdc_WrnDiag.Byte |= (Hsdc_AllRegister.WRN.Byte & 0x3Fu);
  }
  if(Hsdc_AllRegister.STD.Byte & 0x3Fu)
  {
    Hsdc_StdDiag.Byte |= (Hsdc_AllRegister.STD.Byte & 0x3Fu);
  }
  if(Hsdc_AllRegister.ERR.Byte & 0x3Fu)
  {
    Hsdc_ErrDiag.Byte |= (Hsdc_AllRegister.STD.Byte & 0x3Fu);
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_ErrorHandle 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_ErrorHandle(void)
{
  if(Hsdc_ErrDiag.Reg.ERRn)
  {
    Hsdc_RestartTimer++;
    if (HSDC_RESTART_COUNT <= Hsdc_RestartCount) /*restart times overflow, slow restart*/
    {
      if(HSDC_RESTART_DELAY_SLOW < Hsdc_RestartTimer)
      {
        Hsdc_RestartTimer = 0u;
        Hsdc_RestartCount++;
        Hsdc_WriteReg_HWCR(FALSE, FALSE, TRUE);
      }
    }
    else 
    {
      if(HSDC_RESTART_DELAY_FAST <= Hsdc_RestartTimer)
      {
        Hsdc_RestartTimer = 0u;
        Hsdc_RestartCount++;
        Hsdc_WriteReg_HWCR(FALSE, FALSE, TRUE);
      }
    }
  }
  else 
  {
    Hsdc_RestartCount = 0u;
    Hsdc_RestartTimer = 0u;
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_ReadDiagRegister 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_ReadDiagRegister(void)
{
  /*read All DIAG register*/
  Hsdc_ReadRegById(CMD_READ_REG_WRN);
  Hsdc_ReadRegById(CMD_READ_REG_STD);
  Hsdc_ReadRegById(CMD_READ_REG_ERR);
}

/******************************************************************************/
/** 
* \brief      Hsdc_ReadAllRegister 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Hsdc_ReadAllRegister(void)
{
  /*read all register except for DIAG register*/
  for (uint8 i = 0u; i < 10u; i++)
  {
    Hsdc_ReadRegById(i);
  }
}

/******************************************************************************/
/** 
* \brief      Hsdc_GetInitResetFlag 
*
* \author     lub
* \param      N/A
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static boolean Hsdc_GetInitResetFlag(void)
{
  return Hsdc_InitResetFlag;
}
/*_____ E N D _____ (hsdc.c) _________________________________________________*/
