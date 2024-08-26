/******************************************************************************/
/**
* \file       sbcc.c
* \brief      
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

/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include <string.h>
#include "spic.h"
#include "sbcc.h"
#include "ppfc.h"
//#include "doup.h"
//#include "pmm.h"
//#include "watchdog.h"
#include "CanNm_Cfg.h"
#include "Mcu.h"
#include "CanIf.h"

#include "EOL_Main.h"

/* _____ L O C A L - D E F I N E _____________________________________________*/
#define SBCC_MAX_INDEX_NUM                50U
#define Sbcc_DIAG_MONITORING              0U
#define Sbcc_DIAG_MATURE                  1U
#define Sbcc_DIAG_DEMATURE                2U
#define Sbcc_DIAG_IDLE                    3U
#define Sbcc_DIAG_SW_OCR_PERIOD           (50U / Sbcc_BasiTimerPeriod)
#define Sbcc_PROTECT_TIME                 (2000U / Sbcc_BasiTimerPeriod)
#define Sbcc_PROTECT_TEMP                 (125)
#define Sbcc_SPI_E_ResendNum              (5U) /*if SPI error, then resend 5 times*/
#define Sbcc_BasiTimerPeriod              (10U)
#define Sbcc_PowerOnFilterTime            (500U)
#define TW_CL1_Postion                    (8U)

#define SBCC_SPI_DBT_VALUE		            (0x01U) /*delay between transfers*/
#define SBCC_SPI_DSP_VALUE		            (0x01U) /*delay SCK to PCS*/
#define SBCC_SPI_DPS_VALUE		            (0x01U) /*delay PCS to SCK*/

/* _____ L O C A L - T Y P E S _______________________________________________*/

/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ L O C A L - D A T A _________________________________________________*/
static uint32 Sbcc_TxBuffer[SBCC_MAX_INDEX_NUM];
static uint32 Sbcc_RxBuffer[SBCC_MAX_INDEX_NUM];

/** \brief  SPI output message-default value */
static uint32 Sbcc_OutputDataTable[SBCC_NUM_OF_OUT_INDEX] =
{
  /* Sbcc_FRAME_RESET and Sbcc_FRAME_CLEAR */
  (Sbcc_BTS_READ_CLR|Sbcc_ConfReg_Addr), /* 0 */                                       
  
  /* Control register default value */
  (Sbcc_BTS_WRITE | Sbcc_CR1_Addr | Sbcc_CR1_WU_EN | Sbcc_CR1_LIN_WU_EN | Sbcc_CR1_CAN_WU_EN),/* 1*/
   
  (Sbcc_BTS_READ | Sbcc_CR1_Addr), /* 2 */                                             

  (Sbcc_BTS_WRITE | Sbcc_CR2_Addr | Sbcc_CR2_LIN_TXD_TOUT_EN ), /* 3*/                

  (Sbcc_BTS_READ | Sbcc_CR2_Addr),/* 4 */                                             

  (Sbcc_BTS_WRITE | Sbcc_CR3_Addr | Sbcc_CR3_VSREG_LOCK_EN | Sbcc_CR3_VS_LOCK_EN \
   |Sbcc_CR3_VSREG_OV_SD_EN | Sbcc_CR3_VSREG_UV_SD_EN | Sbcc_CR3_VS_OV_SD_EN \
   |Sbcc_CR3_VS_UV_SD_EN),/* 5 */ 

  (Sbcc_BTS_READ|Sbcc_CR3_Addr),/* 6 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR4_Addr),/* 7 */ 
 
  (Sbcc_BTS_READ|Sbcc_CR4_Addr),/* 8 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR5_Addr),/* 9 */ 

  (Sbcc_BTS_READ|Sbcc_CR5_Addr),/* 10 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR6_Addr),/* 11*/ 

  (Sbcc_BTS_READ|Sbcc_CR6_Addr),/* 12 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR7_Addr),/* 13 */ 

  (Sbcc_BTS_READ|Sbcc_CR7_Addr),/* 14 */ 

  (Sbcc_BTS_WRITE |Sbcc_CR8_Addr | Sbcc_CR8_OUT1_OCR_THX_EN | Sbcc_CR8_OUT2_OCR_THX_EN \
   |Sbcc_CR8_OUT3_OCR_THX_EN | Sbcc_CR8_OUT4_OCR_THX_EN | Sbcc_CR8_OUT5_OCR_THX_EN \
   |Sbcc_CR8_OUT6_OCR_THX_EN | Sbcc_CR8_OUT7_OCR_THX_EN | Sbcc_CR8_OUT8_OCR_THX_EN \
   |Sbcc_CR8_OUTHS_OCR_THX_EN),/* 15 */ 

  (Sbcc_BTS_READ | Sbcc_CR8_Addr),/* 16*/ 

  (Sbcc_BTS_WRITE | Sbcc_CR9_Addr),/* 17 */ 

  (Sbcc_BTS_READ | Sbcc_CR9_Addr),/* 18 */ 

  (Sbcc_BTS_WRITE | Sbcc_CR10_Addr | Sbcc_CR10_DIAG_2 | Sbcc_CR10_DIAG_1 \
   |Sbcc_CR10_DIAG_0 | Sbcc_CR10_GH_TH_2 | Sbcc_CR10_GH_TH_1 | Sbcc_CR10_GH_TH_0 \
   |Sbcc_CR10_COPT_3 | Sbcc_CR10_COPT_2 | Sbcc_CR10_COPT_1 | Sbcc_CR10_COPT_0),/* 19 */ 

  (Sbcc_BTS_READ | Sbcc_CR10_Addr),/* 20 */ 

  (Sbcc_BTS_WRITE | Sbcc_CR11_Addr),/* 21 */ 

  (Sbcc_BTS_READ | Sbcc_CR11_Addr),/* 22 */ 

  (Sbcc_BTS_WRITE | Sbcc_CR12_Addr | Sbcc_CR12_PMW1_FREQ_0 | Sbcc_CR12_PMW2_FREQ_0 \
  |Sbcc_CR12_PMW3_FREQ_0 |Sbcc_CR12_PMW4_FREQ_0 | Sbcc_CR12_PMW5_FREQ_0 | Sbcc_CR12_PMW6_FREQ_0 \
  |Sbcc_CR12_PMW7_FREQ_0 | Sbcc_CR12_PMW8_FREQ_0 | Sbcc_CR12_PMW9_FREQ_0 |Sbcc_CR12_PMW10_FREQ_0),/* 23 */ 

  (Sbcc_BTS_READ|Sbcc_CR12_Addr),/* 24 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR13_Addr),/* 25 */ 

  (Sbcc_BTS_READ|Sbcc_CR13_Addr),/* 26 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR14_Addr),/* 27*/ 

  (Sbcc_BTS_READ|Sbcc_CR14_Addr),/* 28 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR15_Addr),/* 29 */ 

  (Sbcc_BTS_READ|Sbcc_CR15_Addr),/* 30 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR16_Addr),/* 31*/ 

  (Sbcc_BTS_READ|Sbcc_CR16_Addr),/* 32 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR17_Addr),/* 33 */ 

  (Sbcc_BTS_READ|Sbcc_CR17_Addr),/* 34 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR18_Addr),/* 35 */ 
 
  (Sbcc_BTS_READ|Sbcc_CR18_Addr),/* 36 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR23_Addr),/* 37*/ 

  (Sbcc_BTS_READ|Sbcc_CR23_Addr),/* 38 */ 

  (Sbcc_BTS_WRITE | Sbcc_CR24_Addr | Sbcc_CR24_SAMPLE_3 | Sbcc_CR24_SAMPLE_1),/* 39*/ 

  (Sbcc_BTS_READ|Sbcc_CR24_Addr),/* 40 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR25_Addr),/* 41 */ 

  (Sbcc_BTS_READ|Sbcc_CR25_Addr),/* 42 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR26_Addr),/* 43 */ 

  (Sbcc_BTS_READ|Sbcc_CR26_Addr),/* 44 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR27_Addr),/* 45 */ 

  (Sbcc_BTS_READ|Sbcc_CR27_Addr),/* 46*/ 

  (Sbcc_BTS_WRITE|Sbcc_CR28_Addr),/* 47 */ 

  (Sbcc_BTS_READ|Sbcc_CR28_Addr),/* 48*/ 

  (Sbcc_BTS_WRITE|Sbcc_CR29_Addr),/* 49 */

  (Sbcc_BTS_READ|Sbcc_CR29_Addr),/* 50 */ 

  (Sbcc_BTS_WRITE|Sbcc_CR34_Addr|Sbcc_CR34_WD_EN ),/* 51*/ 

  (Sbcc_BTS_READ|Sbcc_CR34_Addr),/* 52*/ 

  (Sbcc_BTS_WRITE|Sbcc_ConfReg_Addr|Sbcc_CFGR_WU_CONFIG),/* 53 */ 

  (Sbcc_BTS_READ|Sbcc_ConfReg_Addr),/* 54 */ 

  /* Status register default value */
  (Sbcc_BTS_READ|Sbcc_SR1_Addr),/* 55 */ 

  (Sbcc_BTS_READ|Sbcc_SR2_Addr),/* 56 */ 

  (Sbcc_BTS_READ|Sbcc_SR3_Addr),/* 57*/ 

  (Sbcc_BTS_READ|Sbcc_SR4_Addr),/* 58*/ 

  (Sbcc_BTS_READ|Sbcc_SR5_Addr),/* 59 */ 

  (Sbcc_BTS_READ|Sbcc_SR6_Addr),/* 60 */ 

  (Sbcc_BTS_READ|Sbcc_SR7_Addr),/* 61 */ 

  (Sbcc_BTS_READ|Sbcc_SR8_Addr),/* 62*/ 

  (Sbcc_BTS_READ|Sbcc_SR9_Addr),/* 63*/ 

  (Sbcc_BTS_READ|Sbcc_SR10_Addr),/* 64*/ 

  (Sbcc_BTS_READ|Sbcc_SR11_Addr),/* 65 */ 

  (Sbcc_BTS_READ|Sbcc_SR12_Addr),/* 66 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR1_Addr),/* 67*/ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR2_Addr),/* 68 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR3_Addr),/* 69 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR4_Addr),/* 70 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR5_Addr),/* 71 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR6_Addr),/* 72 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR7_Addr),/* 73*/ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR8_Addr),/* 74*/ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR9_Addr),/* 75 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR10_Addr),/* 76 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR11_Addr),/* 77 */ 

  (Sbcc_BTS_READ_CLR|Sbcc_SR12_Addr),/* 78 */ 
  (Sbcc_BTS_READ_DEV_INFOR|0x00000000U), /* Sbcc_FRAME_READ_COMP_CODE */
  (Sbcc_BTS_READ_DEV_INFOR|0x01000000U), /* Sbcc_FRAME_READ_DEV_FAM */
  (Sbcc_BTS_READ_DEV_INFOR|0x02000000U), /* Sbcc_FRAME_READ_DEV_NO1 */
  (Sbcc_BTS_READ_DEV_INFOR|0x03000000U), /* Sbcc_FRAME_READ_DEV_NO2 */
  (Sbcc_BTS_READ_DEV_INFOR|0x04000000U), /* Sbcc_FRAME_READ_DEV_NO3 */
  (Sbcc_BTS_READ_DEV_INFOR|0x05000000U), /* Sbcc_FRAME_READ_DEV_NO4 */
  (Sbcc_BTS_READ_DEV_INFOR|0x06000000U), /* Sbcc_FRAME_READ_DEV_NO5 */
};

/** \brief  SPI input message */
static uint32 Sbcc_IutputDataTable[SBCC_NUM_OF_OUT_INDEX] = {0};

static const Sbcc_OutputBitCfg_t Sbcc_OutputBitCfg[SBCC_NUM_OF_PHY_CH] =
{
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT1_HS, Sbcc_CR4_OUT1_LS, Sbcc_CR4_OUT1_HS|Sbcc_CR4_OUT1_LS, 20},                    /* SBCC_OUT_CHANNEL1 */
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT2_HS, Sbcc_CR4_OUT2_LS, Sbcc_CR4_OUT2_HS|Sbcc_CR4_OUT2_LS, 16},                    /* SBCC_OUT_CHANNEL2 */
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT3_HS, Sbcc_CR4_OUT3_LS, Sbcc_CR4_OUT3_HS|Sbcc_CR4_OUT3_LS, 12},                    /* SBCC_OUT_CHANNEL3 */
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT4_HS, Sbcc_CR4_OUT4_LS, Sbcc_CR4_OUT4_HS|Sbcc_CR4_OUT4_LS, 8},                     /* SBCC_OUT_CHANNEL4 */
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT5_HS, Sbcc_CR4_OUT5_LS, Sbcc_CR4_OUT5_HS|Sbcc_CR4_OUT5_LS, 4},                     /* SBCC_OUT_CHANNEL5 */
  {Sbcc_FRAME_WRITE_CR4, Sbcc_CR4_OUT6_HS, Sbcc_CR4_OUT6_LS, Sbcc_CR4_OUT6_HS|Sbcc_CR4_OUT6_LS, 0},                     /* SBCC_OUT_CHANNEL6 */

  {Sbcc_FRAME_WRITE_CR5, Sbcc_CR5_OUT7_0, 0, Sbcc_CR5_OUT7_0|Sbcc_CR5_OUT7_1|Sbcc_CR5_OUT7_2|Sbcc_CR5_OUT7_3, 20},      /* SBCC_OUT_CHANNEL7 */
  {Sbcc_FRAME_WRITE_CR5, Sbcc_CR5_OUT8_0, 0, Sbcc_CR5_OUT8_0|Sbcc_CR5_OUT8_1|Sbcc_CR5_OUT8_2|Sbcc_CR5_OUT8_3, 16},      /* SBCC_OUT_CHANNEL8 */
  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT9_0, 0, Sbcc_CR6_OUT9_0|Sbcc_CR6_OUT9_1|Sbcc_CR6_OUT9_2|Sbcc_CR6_OUT9_3, 20},      /* SBCC_OUT_CHANNEL9 */
  {Sbcc_FRAME_WRITE_CR5, Sbcc_CR5_OUT10_0, 0, Sbcc_CR5_OUT10_0|Sbcc_CR5_OUT10_1|Sbcc_CR5_OUT10_2|Sbcc_CR5_OUT10_3, 8},  /* SBCC_OUT_CHANNEL10 */
  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT11_0, 0, Sbcc_CR6_OUT11_0|Sbcc_CR6_OUT11_1|Sbcc_CR6_OUT11_2|Sbcc_CR6_OUT11_3, 16}, /* SBCC_OUT_CHANNEL11 */
  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT12_0, 0, Sbcc_CR6_OUT12_0|Sbcc_CR6_OUT12_1|Sbcc_CR6_OUT12_2|Sbcc_CR6_OUT12_3, 12}, /* SBCC_OUT_CHANNEL12 */

  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT13_0, 0, Sbcc_CR6_OUT13_0|Sbcc_CR6_OUT13_1|Sbcc_CR6_OUT13_2|Sbcc_CR6_OUT13_3, 8},  /* SBCC_OUT_CHANNEL13 */
  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT14_0, 0, Sbcc_CR6_OUT14_0|Sbcc_CR6_OUT14_1|Sbcc_CR6_OUT14_2|Sbcc_CR6_OUT14_3, 4},  /* SBCC_OUT_CHANNEL14 */
  {Sbcc_FRAME_WRITE_CR6, Sbcc_CR6_OUT15_0, 0, Sbcc_CR6_OUT15_0|Sbcc_CR6_OUT15_1|Sbcc_CR6_OUT15_2|Sbcc_CR6_OUT15_3, 0},  /* SBCC_OUT_CHANNEL15 */
  {Sbcc_FRAME_WRITE_CR5, Sbcc_CR5_OUTHS_0, 0, Sbcc_CR5_OUTHS_0|Sbcc_CR5_OUTHS_1|Sbcc_CR5_OUTHS_2|Sbcc_CR5_OUTHS_3, 0},  /* SBCC_OUT_CHANNEL_HS */
  {Sbcc_FRAME_WRITE_CR5, Sbcc_CR5_GH, 0, Sbcc_CR5_GH, 4},                                                               /* SBCC_OUT_CHANNEL_GH */
  {Sbcc_FRAME_WRITE_CR1, 0, 0, 0, 0},                                                                                   /* SBCC_OUT_CHANNEL_FWL_UP */
  {Sbcc_FRAME_WRITE_CR1, 0, 0, 0, 0},                                                                                   /* SBCC_OUT_CHANNEL_FWL_DOWN */
  {0,0,0,0,0},                                                                                                          /* SBCC_OUT_CHANNEL_EC */
};

static Sbcc_IndexTableType_t Sbcc_UsingPingOrPong = SBCC_INDEX_TABLE_PING;
static uint8 Sbcc_OutputIndexTablePing[SBCC_MAX_INDEX_NUM] = {0};
static uint8 Sbcc_OutputIndexTablePong[SBCC_MAX_INDEX_NUM] = {0};
static uint8 Sbcc_QueueMaskPing[(SBCC_NUM_OF_OUT_INDEX + 7U)/8U];
static uint8 Sbcc_QueueMaskPong[(SBCC_NUM_OF_OUT_INDEX + 7U)/8U];
static uint32 statusReigsterError[(uint8)ERROR_LOG_NUN] = {0u};

/** \brief  Record how many commands need to be handled in 5ms */
static uint8 Sbcc_OutputIndexTotalNumPing = 0;
static uint8 Sbcc_OutputIndexTotalNumPong = 0;

static uint8 Sbcc_Gsb_Value = 0x00;
static Sbcc_DiagInfo_t Sbcc_DiagInfo[SBCC_NUM_OF_PHY_CH];

static Sbcc_AllStatusRegister_t Sbcc_AllStatusRegister = {0};
static SPIC_Reqtype_t Sbcc_req;

static boolean Sbcc_VporFlag = FALSE;
static boolean Sbcc_HeaterOpenLoadIsSet = FALSE;
static boolean Sbcc_wakedupByCAN = FALSE;
static boolean Sbcc_wakedupByWU = FALSE;
/*static uint8 Sbcc_LastFLWin_Action = 0;*/
static boolean Sbcc_V2FailOrSc = FALSE;
static SBCC_OutputState_t Sbcc_DlcSt = SBCC_OutputOff;

static boolean Sbcc_WDFailFlag = FALSE;
static boolean Sbcc_WDToggleValue = FALSE;
static boolean Sbcc_PowerOnFilterFlag = FALSE;
/* _____ L O C A L - M A C R O S _____________________________________________*/
#define SBCC_GET_OUTPUT_TABLE(Index) (Sbcc_OutputDataTable[Index])
#define SBCC_SET_OUTPUT_TABLE(Index, data) \
do \
{  \
     Sbcc_OutputDataTable[Index] = (uint32)(data); \
} while (0)

#define SBCC_GET_INPUT_TABLE(Index)  (Sbcc_IutputDataTable[Index] )


#define Sbcc_GetDutyCfgRegIdx(pwmx) \
  ((((pwmx)/ 2U) * 2U) + Sbcc_FRAME_WRITE_CR13)


#define Sbcc_SetDutyCfgReg(pwmx, reg, duty) do { \
    (duty) = (uint16)((duty) * 1023UL / 100UL); \
    if ((pwmx) % 2U == 0U) { \
      (reg) &= ~((uint32)0x3FFU << 12U); \
      (reg) |= (uint32)(duty) << 12U; \
    } else { \
      (reg) &= ~((uint32)0x3FFU); \
      (reg) |= (uint32)(duty); \
    } \
  } while (0)

#define Sbcc_GetOlMatureTime(channel) (Sbcc_DiagCfg[channel].OlMatureTime / Sbcc_BasiTimerPeriod)
#define Sbcc_GetOcMatureTime(channel) (Sbcc_DiagCfg[channel].OcMatureTime / Sbcc_BasiTimerPeriod)
#define Sbcc_GetDematureTime(channel) (Sbcc_DiagCfg[channel].DematureTime / Sbcc_BasiTimerPeriod)
#define Sbcc_GetProtectTime(channel)  (Sbcc_DiagCfg[channel].ProtectTime / Sbcc_BasiTimerPeriod)
#define Sbcc_GetTempClId(channel)     (Sbcc_DiagCfg[channel].TempClId)
#define Sbcc_GetOcrMask(channel)      (Sbcc_DiagCfg[channel].OcrMask)
#define Sbcc_GetOcrAlertMask(channel) (Sbcc_DiagCfg[channel].OcrAlertMask)
#define Sbcc_GetOlMask(channel)       (Sbcc_DiagCfg[channel].OlMask)
#define Sbcc_GetOcMask(channel)       (Sbcc_DiagCfg[channel].OcMask)
#define Sbcc_GetCombined(channel)     (Sbcc_DiagCfg[channel].Combined)

/* _____ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/
static void Sbcc_SBCRegisterInit(void);
static boolean Sbcc_CheckEnterQueue(uint8 Index);
static boolean Sbcc_OutputHSOrLS(uint32 HS_Bit, uint32 LS_Bit, SBCC_OutputState_t Status);
static boolean Sbcc_OutputHS(uint8 Index, uint32 HS_Bit, uint32 BitMask, SBCC_OutputState_t Status);
static void Sbcc_StatusRegisterUpdate(void);
static void Sbcc_EnableDisableOCR(uint8 Channel, boolean Enable);
static void Sbcc_NormalErrorHandler(void);
static uint32 Sbcc_CheckSR12(void);
static uint32 Sbcc_CheckSR6(void);
static uint32 Sbcc_CheckSR5(void);
static uint32 Sbcc_CheckSR4(void);
static uint32 Sbcc_CheckSR3(void);
static uint32 Sbcc_CheckSR2(void);
static uint32 Sbcc_CheckSR1(void);
static void Sbcc_ClearBitsOfSR(uint8 RegInedx,  uint32 valueToWrite);
static void Sbcc_ClearErrFlag(uint8 channel, uint8 errType);
static boolean Sbcc_OutCfg(uint8 channel, uint8 cfg);
static uint32 Sbcc_SyncTrigBitWithWD(uint32 value);
static void Sbcc_DiagManager(void);
static void Sbcc_AbnormalConHandle(uint8 index, uint8 err);
static void Sbcc_StartDiag(uint8 index, uint8 error);
static void Sbcc_CheckRSTBError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckSPIError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckPleError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckFeError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckDEError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckGwError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_CheckFsError(uint32 errorLog[(uint8)ERROR_LOG_NUN]);
static void Sbcc_DLC_Action(uint8 RegInedx, SBCC_OutputState_t Status);
static boolean Sbcc_OutputExec(uint8 Channel, SBCC_OutputState_t HS_LS,boolean Status);
static boolean Sbcc_OverTempAfterShortCheck(uint8 Channel);
static boolean Sbcc_OperationConditonCheck(uint8 Channel, boolean Status);
static void Sbcc_SetWDFailFlag(boolean Status);
static void Sbcc_WatchdogRefresh(boolean * Value);
static void Sbcc_ReadStatusReg(void);
static void Sbcc_DmConfigure(boolean value);
#ifdef SBCC_CAN_DEBUG_MESSAGE
static void Sbcc_Debug_msg(void);
static uint8 Sbcc_CheckPncRegister(void);
#endif
/* _____ G L O B A L - F U N C T I O N S _____________________________________*/
#ifdef SBCC_CAN_DEBUG_MESSAGE
uint8 myMsg[8];
static void Sbcc_Debug_msg(void)
{
  //static timeCount = 0;
  //if ((timeCount % 2) == 0)
  {
    Can_PduType dummy;
    dummy.id = 0x666; /* ID of dummy message */
    dummy.length = 8; /* Set message length to 8 */
    dummy.sdu = (uint8 *)&myMsg[0];
    dummy.swPduHandle = 0u;
    Can_Write(0u, &dummy);
  }
  //timeCount++;
}

static uint8 Sbcc_CheckPncRegister(void)
{
  uint8 vaildFlag = 0x7F;
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR23)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR23] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR23] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 0);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR24)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR24] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR24] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 1);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR25)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR25] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR25] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 2);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR26)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR26] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR26] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 3);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR27)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR27] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR27] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 4);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR28)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR28] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR28] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 5);
    }
  }
  if(Sbcc_AllStatusRegister.SR12.Bits.SWRD_CR29)
  {
    if((Sbcc_OutputDataTable[Sbcc_FRAME_WRITE_CR29] & 0xFFFFFF) != (Sbcc_IutputDataTable[Sbcc_FRAME_READ_CR29] & 0xFFFFFF))
    {
      vaildFlag &= ~(0x01 << 6);
    }
  }
  return vaildFlag;
}
#endif
/******************************************************************************/
/** 
* \brief      SBCC_Init \n
*             Init the module and SPI messages
*
* \author     Weiwen.CHEN
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_Init(void)
{
  /*Partial Networking configure*/
#if SBCC_PN_ENABLE
  uint8 i;
  uint32 temp;
  uint8 PncMaskbuff[6] = {0,0,0,0,0,0};

  for(i=0; i<sizeof(PncMaskbuff); i++)
  {
    PncMaskbuff[i] = CanNm_PnInfoByteConfig[i].PnFilterMaskOfPnInfoByteConfig;
  }

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR24);
  temp &= ~(uint32)(0x57FU);//filter ID NM mast receive ID
  temp |= (0x57FU);//filter ID NM mast receive ID
  temp |= Sbcc_CR24_DLC_3;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR24, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR24);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR26);
  temp &= ~(uint32)0x07FU;//mask
  temp |= 0x07FU;//mask
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR26, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR26);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR27);
  temp &= ~((uint32)PncMaskbuff[0]<<16U);//NM message byte2 
  temp |= ((uint32)PncMaskbuff[0]<<16U);//NM message byte2 
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR27, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR27);
  
  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR28);
  temp &= ~((uint32)PncMaskbuff[1]);//NM message byte3 
  temp &= ~((uint32)PncMaskbuff[2]<<8U);//NM message byte4 
  temp &= ~((uint32)PncMaskbuff[3]<<16U);//NM message byte5;
  
  temp |= (PncMaskbuff[1]);//NM message byte3;
  temp |= ((uint32)PncMaskbuff[2]<<8U);//NM message byte4;
  temp |= ((uint32)PncMaskbuff[3]<<16U);//NM message byte5;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR28, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR28);
  
  
  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR29);
  temp &= ~((uint32)PncMaskbuff[4]);//NM message byte6;
  temp &= ~((uint32)PncMaskbuff[5]<<8U);//NM message byte7;
  temp |= (PncMaskbuff[4]);//NM message byte6;
  temp |= ((uint32)PncMaskbuff[5]<<8U);//NM message byte7;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR29, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR29);

  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR23);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR24);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR25);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR26);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR27);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR28);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CR29);
#endif
}

/******************************************************************************/
/** 
* \brief      SBCC_WakeUp
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_WakeUp(void)
{
  Sbcc_SBCRegisterInit();
  SBCC_CanTransceiverEnableDisable(TRUE);
  SBCC_SwitchOnOff(SBCC_HS, TRUE);
#if (defined DCU_FL) || (defined DCU_FR)
  //SBCC_Set5V2State(TRUE);
#endif
}

/******************************************************************************/
/** 
* \brief      SBCC_Sleep
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_Sleep(void)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);

  temp &= ~((uint32)(Sbcc_CR1_HEN|Sbcc_CR1_V2_0|Sbcc_CR1_V2_1)); /* H-bridge disable and set V2 to OFF */
  temp = Sbcc_SyncTrigBitWithWD(temp);/* keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);

  /*get the open load settings*/
  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10);
  Sbcc_HeaterOpenLoadIsSet = ((temp & Sbcc_CR10_GH_OL_EN) != 0x00U);

  SBCC_SwitchOnOff(SBCC_HS, FALSE);
  SBCC_CanTransceiverEnableDisable(FALSE);
}

/******************************************************************************/
/** 
* \brief      SBCC_Manager \n
*             5ms timer callback
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_Manager(void)
{
  Sbcc_WatchdogRefresh(&Sbcc_WDToggleValue);
  Sbcc_StatusRegisterUpdate();
  Sbcc_NormalErrorHandler();
  Sbcc_DiagManager();
  Sbcc_ReadStatusReg();
  Sbcc_SendCmdHandler();
#ifdef SBCC_CAN_DEBUG_MESSAGE
  Sbcc_Debug_msg();
#endif
}

void SpiJob_0_SBC_EndNotification(void)
{
  uint8 i;
  
  /* Start to send first command to SBC */
  if(SBCC_INDEX_TABLE_PING == Sbcc_UsingPingOrPong)
  {
    /* read out last RX frames */
    i = 0;
    while (Sbcc_OutputIndexTotalNumPong > 0U)
    {
      /* Re-send if SPI error */
      if (((uint8)(Sbcc_RxBuffer[i] >> 24U) & Sbcc_BTS_GSB_SPIE) != 0x00U)
      {
        (void)Sbcc_CheckEnterQueue(Sbcc_OutputIndexTablePong[i]);
      }

      

      (void)memset(Sbcc_QueueMaskPong, 0, sizeof(Sbcc_QueueMaskPong));
      Sbcc_IutputDataTable[Sbcc_OutputIndexTablePong[i]] = Sbcc_RxBuffer[i];
      Sbcc_OutputIndexTotalNumPong--;
      i++;
    }
  }
  else
  {
    /* read out last RX frames */
    i = 0;
    while (Sbcc_OutputIndexTotalNumPing > 0U)
    {
      /* Re-send if SPI error */
      if (((uint8)(Sbcc_RxBuffer[i] >> 24U) & Sbcc_BTS_GSB_SPIE) != 0x00U)
      {
        (void)Sbcc_CheckEnterQueue(Sbcc_OutputIndexTablePing[i]);
      }

      (void)memset(Sbcc_QueueMaskPing, 0, sizeof(Sbcc_QueueMaskPing));
      Sbcc_IutputDataTable[Sbcc_OutputIndexTablePing[i]] = Sbcc_RxBuffer[i];
      Sbcc_OutputIndexTotalNumPing--;
      i++;
    }
  }
  //WD_TaskRefreshWdg(WD_SPI_SBC);
}

/******************************************************************************/
/** 
* \brief      SBCC_CanTransceiverEnableDisable
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_CanTransceiverEnableDisable(boolean Status)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);

  if(Status)
  {
    temp |= (Sbcc_CR1_CAN_RXEN|Sbcc_CR1_CAN_TXEN|Sbcc_CR1_CAN_Go_TRX_RDY);
  }
  else
  {
    /*don't add [Sbcc_CR1_CAN_RXEN] to below code*/
    temp &= (uint32)(~(uint32)(Sbcc_CR1_CAN_TXEN|Sbcc_CR1_CAN_Go_TRX_RDY));
    temp |= Sbcc_CR1_SWEN;
  }
  
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);

  return Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);
}

/******************************************************************************/
/** 
* \brief      SBCC_SetOutputType \n
*             Set SBC output type 
*
* \author     H.Huisheng    
* \param      Channel(uint8)          : SBC output channel \n
* \param      Type(Sbcc_OutputType_t) : \n
*                                     -SBCC_OUTPUT_LS \n
*                                     -SBCC_OUTPUT_HS
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_SetOutputType(uint8 Channel, Sbcc_OutputType_t Type)
{
  Sbcc_Channels[Channel].OutType = Type;
}

/******************************************************************************/
/** 
* \brief      SBCC_GetOutputType \n
*             Get SBC output type
*
* \author     H.Huisheng  
* \param      Channel(uint8) : SBC output channel
* \retval     Sbcc_OutputType_t : \n
*                               -SBCC_OUTPUT_LS \n
*                               -SBCC_OUTPUT_HS
* \note       N/A
*/
/******************************************************************************/
Sbcc_OutputType_t SBCC_GetOutputType(uint8 Channel)
{
  Sbcc_OutputType_t Type = Sbcc_Channels[Channel].OutType;
  return Type;
}

/******************************************************************************/
/** 
* \brief      SBCC_SwitchOnOff \n
*             Switch on/off the desired SBC output channel
* 
* \author     H.Huisheng
* \param      Channel(uint8)  : SBC output channel 
* \param      Status(boolean) : \n
                               -ENABLE/TRUE : channel switch on \n
                               -DISABLE/FALSE : channel switch off 
* \retval     boolean : \n 
                       -TRUE \n
                       -FALSE
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_SwitchOnOff(uint8 Channel, boolean Status)
{
  boolean RetVal = TRUE;
  boolean SwitchEnbale;
  SBCC_OutputState_t HS_LS;

  SwitchEnbale = Sbcc_OperationConditonCheck(Channel, Status);
  
  if(SwitchEnbale)
  {
    if(Status)
    {
      HS_LS = (SBCC_OutputState_t)Sbcc_Channels[Channel].OutType;
      if (Sbcc_DiagInfo[Channel].SuspendOCR != 0x0U)
      {
        Sbcc_EnableDisableOCR(Channel, FALSE);
      }
      else
      {
        Sbcc_EnableDisableOCR(Channel, TRUE);
      }
      
      // if (SBCC_GetErrorPresent(Channel) == SBCC_ERROR_OVER_CURRENT)
      {
        SBCC_ClearWarningBit(Sbcc_GetTempClId(Channel));
        Sbcc_ClearErrFlag(Channel, SBCC_ERROR_OPEN_LOAD);
        Sbcc_ClearErrFlag(Channel, SBCC_ERROR_OVER_CURRENT);
      }
      Sbcc_DiagInfo[Channel].State = Sbcc_DIAG_MONITORING;
      Sbcc_DiagInfo[Channel].ErrType = SBCC_ERROR_INVALID;
      Sbcc_Channels[Channel].Flags |= Sbcc_CHANNEL_RUN;
      Sbcc_Channels[Channel].Duty = 100U;
    }
    else
    {
      HS_LS = SBCC_OutputOff;
      Sbcc_Channels[Channel].Flags &= (uint8)(~Sbcc_CHANNEL_RUN);
      Sbcc_Channels[Channel].Duty = 0U;
    }

    if (Channel == SBCC_OUT_CHANNEL_GH)
    {
      SBCC_SetGH_OL(!Status);
    }

    (void)Sbcc_OutputExec(Channel, HS_LS, Status);
    
  }
  else
  {
    RetVal = FALSE;
  }

  return RetVal;
}

/******************************************************************************/
/**
* \brief      SBCC_SwitchOnOff \n
*             Switch on/off the desired SBC output channel
*
* \author     Ken
* \param      Channel(uint8)  : SBC output channel
* \retval     boolean : \n
                       -TRUE \n
                       -FALSE
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_SwitchOffButKeepRunSt(uint8 Channel)
{
  boolean RetVal = TRUE;
  boolean SwitchEnbale;

  SwitchEnbale = Sbcc_OperationConditonCheck(Channel, FALSE);

  if(SwitchEnbale)
  {
    Sbcc_Channels[Channel].Duty = 0U;
    if (Channel == SBCC_OUT_CHANNEL_GH)
    {
      SBCC_SetGH_OL(TRUE);
    }

    (void)Sbcc_OutputExec(Channel, SBCC_OutputOff, FALSE);
  }
  else
  {
    RetVal = FALSE;
  }

  return RetVal;
}

#if 0
/******************************************************************************/
/** 
* \brief      SBCC_GetVSInputValue
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
uint16 SBCC_GetVSInputValue(void)
{
  uint32 temp = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR11);

  temp &= Sbcc_SR11_VS_MASK;
  temp = temp >> Sbcc_SR11_VS_START_BIT;
  return (uint16)temp;
}
#endif

/******************************************************************************/
/** 
* \brief      SBCC_GetErrorPresent \n
*             Return the error present for a given channel
*
* \author     H.Huisheng
* \param      Channel(uint8) : SBC channel
* \retval     Value(uint8) : error representative level \n
*               -SBCC_ERROR_NONE(0) : no error present \n
*               -SBCC_ERROR_UNDER_LOAD(1) : an underload/openload was detected \n
*               -SBCC_ERROR_OVER_LOAD(2) : an overload was detected 
* \note       N/A
*/
/******************************************************************************/
uint8 SBCC_GetErrorPresent(uint8 Channel)
{
  uint8 result = Sbcc_DiagInfo[Channel].ErrType;

  if (result == SBCC_ERROR_OCR_ALERT)
  {
    result = SBCC_ERROR_OVER_CURRENT;
  }
  return result;
}

/******************************************************************************/
/** 
* \brief      SBCC_HasValidFeedback
* 
* \author     H.Huisheng
* \param      Channel(uint8) : SBC channel 
* \retval     boolean: \n
                      -TRUE \n
                      -FALSE 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_HasValidFeedback(uint8 Channel)
{
  return Sbcc_DiagInfo[Channel].ErrType != SBCC_ERROR_INVALID;
}

#if 0
boolean SBCC_GetWakedUpByCanFlag(void)
{
  return Sbcc_wakedupByCAN;
}
void SBCC_ClearWakedUpByCanFlag(boolean flag)
{
  Sbcc_wakedupByCAN = flag;
}
#endif
/*let error handle to process such error*/
#if 0
/******************************************************************************/
/** 
* \brief      SBCC_EnableCanCommunicationAfterBusOff \n
*             restore CAN communication after bus off.
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_EnableCanCommunicationAfterBusOff(void)
{

  uint8 RegInedx = 0x00;
  uint32 temp = 0;

  RegInedx = Sbcc_FRAME_WRITE_CFG;
  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  temp |= (Sbcc_CFGR_MASK_SPIE|Sbcc_CFGR_MASK_GW|Sbcc_CFGR_MASK_PLE); /* Mask SPI error and global warning */
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);

  Sbcc_CheckEnterQueue(Sbcc_FRAME_RESET);
  /*configure regiter CFG*/
  Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CFG);
  /*clear SR2 to enable CAN communication*/
  Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_CLEAR_SR2);
}
#endif

/******************************************************************************/
/** 
* \brief      SBCC_ReadRegisterValue \n
*             read the value of SBC register
*
* \author     H.Huisheng
* \param      Channel(uint8) :                      
* \retval     uint32 : -register value 
                      
* \note       N/A
*/
/******************************************************************************/
uint32 SBCC_ReadRegisterValue(uint8 Channel)
{  
   uint32 RetVal = 0;
   if(Channel < SBCC_NUM_OF_OUT_INDEX)
   {
     (void)Sbcc_CheckEnterQueue(Channel);
     RetVal = SBCC_GET_INPUT_TABLE(Channel);
   }

  return RetVal;
}

/******************************************************************************/
/** 
* \brief      SBCC_RefreshWD
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_RefreshWD(void)
{
#if SBCC_WD_ENABLE

  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);

#endif
}

/******************************************************************************/
/** 
* \brief      SBCC_ToggleWD \n
              toggle the value which will be wrote to CR1 register
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_ToggleWD(boolean * Value)
{
#if SBCC_WD_ENABLE
    uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);
  
    if((*Value) == TRUE)
    {
      temp &= ~((uint32)Sbcc_CR1_TRIG);
      (*Value) = FALSE;
    }
    else
    {
      temp |= (Sbcc_CR1_TRIG);
      (*Value) = TRUE;
    }
  
    SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);
#endif
}

/******************************************************************************/
/** 
* \brief      SBCC_GoToStandByMode \n
*             Command which put SBC go to Standby mode
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_GoToStandByMode(void)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);

  /*
      bit3  bit2  bit1
      0     1     1       v1_standby
      1     0     1       Vbat_standby
  */
  temp &= ~((uint32)(Sbcc_CR1_PARITY|Sbcc_CR1_STBY_SEL|Sbcc_CR1_GO_STBY));

  /*v1_standby*/
  #if 0
    temp |= (Sbcc_CR1_STBY_SEL|Sbcc_CR1_GO_STBY);
  #else
    /*Vbat_standby*/
    temp |= (Sbcc_CR1_PARITY|Sbcc_CR1_GO_STBY);
  #endif
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/

  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);
}


/******************************************************************************/
/** 
* \brief      SBCC_WuConfigure \n
*             Configure the WU function
* \author     H.Huisheng
* \param      value(boolean) : \n
*                             -TRUE: configured as wake up input function \n
*                             -FALSE:configured as voltage measurement function
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_WuConfigure(boolean value)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG);

  if(value == TRUE)
  {
    temp &= ~Sbcc_CFGR_WU_CONFIG; /*configured as wake up input--don't remove this configuration*/
  }
  else
  {
    temp |= Sbcc_CFGR_WU_CONFIG; /* configured as voltage measurement */
  }
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CFG);
}

/******************************************************************************/
/** 
* \brief      SBCC_GetVporFlag \n
*             get the power on reset flag 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetVporFlag(void)
{
  return Sbcc_VporFlag;
}

/******************************************************************************/
/** 
* \brief      SBCC_GetVporFlag \n
*             get the power on reset flag 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetCurrentVporFlag(void)
{
  boolean ret_value = FALSE;
  uint32 registerValue = Sbcc_AllStatusRegister.SR1.Word;
  
  /*check Vpor*/
  if((registerValue & Sbcc_SR1_VPOR) != 0x00U)
  {
	 ret_value = TRUE;
  }

  return ret_value; 
}


/******************************************************************************/
/** 
* \brief      SBCC_GetWakeupFlag \n
*             get the wake up flag 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetCurrentWakeupFlag(void)
{
  boolean ret_value = FALSE;
  
  if(((Sbcc_AllStatusRegister.SR1.Word & Sbcc_SR1_CAN) != 0u)
  	||((Sbcc_AllStatusRegister.SR1.Word & Sbcc_SR1_WU) != 0u))
  {
    ret_value = TRUE;
  }
  
  return ret_value;
}

/******************************************************************************/
/** 
* \brief      SBCC_GetWakeupFlag \n
*             get the wake up flag 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetWakeupFlag(void)
{
#if defined(PIL_TEST)
  return TRUE;
#else
  return (Sbcc_wakedupByCAN || Sbcc_wakedupByWU);
#endif
}

/******************************************************************************/
/** 
* \brief      SBCC_ClearVporFlag \n
*             clear the power on reset flag when DCU goes to sleep
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_ClearVporFlag(void)
{
  Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, Sbcc_SR1_VPOR);
}

/******************************************************************************/
/** 
* \brief      SBCC_GetWDFailFlag \n
*             	   get the watch dog fail flag 
*
* \author     Weipeng.LAI
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetWDFailFlag(void)
{
  return Sbcc_WDFailFlag;
}

/******************************************************************************/
/** 
* \brief      SBCC_ClearWakeupFlag \n
*             clear the wake up flag
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_ClearWakeupFlag(void)
{
  Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, Sbcc_SR1_CAN|Sbcc_SR1_WU);
}

/******************************************************************************/
/** 
* \brief      SBCC_SpiCommunicationEstablished \n
*             check if SPI communication with SBC has esatablished or not
*
* \author     H.Huisheng
* \param      N/A 
* \retval     boolean : SPI communication status. \n    
                      -TRUE  : SPI communication has been established \n
                      -FALSE : SPI communication hasn't been established
* \note       
                <
                 This function is used to decide whether the change from hardware
                 input should wake up the system or not. 
                 Theoretically, when the hardware input callback function is called,
                 the SPI communication has been established, because the call back function
                 has 50ms debounce time.
                >
*/
/******************************************************************************/
boolean SBCC_SpiCommunicationEstablished(void)
{
  return (Sbcc_Gsb_Value != 0x0U);
}

/******************************************************************************/
/**
* \brief      Set output PWM duty
*
* \author     X.Ken
* \param      channel : channel to be set(SBCC_OUT_CHANNEL7~SBCC_OUT_CHANNEL_HS) \n
* \param      duty :    duty to be set(0~100)
* \retval     \n
               -FALSE : Set duty fail
               -TRUE  : Set duty success
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_SetPwmDuty(uint8 channel, uint16 duty)
{
  uint32 temp;
  boolean RetVal = FALSE;
  uint8 pwmx;
  uint8 RegIdx;

  if ((duty != Sbcc_Channels[channel].Duty)
   && ((Sbcc_Channels[channel].Flags & Sbcc_CHANNEL_RUN) != 0U))
  {
    Sbcc_Channels[channel].Duty = (uint8)duty;
    
    switch (channel)
    {
      case SBCC_OUT_CHANNEL7:
      case SBCC_OUT_CHANNEL8:
      case SBCC_OUT_CHANNEL9:
      case SBCC_OUT_CHANNEL10:
      case SBCC_OUT_CHANNEL11:
      case SBCC_OUT_CHANNEL12:
      case SBCC_OUT_CHANNEL13:
      case SBCC_OUT_CHANNEL14:
      case SBCC_OUT_CHANNEL15:
      case SBCC_OUT_CHANNEL_HS:
        pwmx = channel - SBCC_OUT_CHANNEL7;
        RetVal = Sbcc_OutCfg(channel, channel - SBCC_OUT_CHANNEL7 + Sbcc_OUT_CFG_PWM1);

        RegIdx = Sbcc_GetDutyCfgRegIdx(pwmx);
        temp = SBCC_GET_OUTPUT_TABLE(RegIdx);
        Sbcc_SetDutyCfgReg(pwmx, temp, duty);
        SBCC_SET_OUTPUT_TABLE(Sbcc_GetDutyCfgRegIdx(pwmx), temp);
        RetVal = Sbcc_CheckEnterQueue(RegIdx) && RetVal;
      break;
    default:
      break;
    }
  }

  return RetVal;
}

/******************************************************************************/
/**
* \brief      Get Instant Error
*
* \author     X.Ken
* \param      channel : channel to be get
* \retval     error type : \n
                    SBCC_ERROR_INVALID \n
                    SBCC_ERROR_NONE \n
                    SBCC_ERROR_OPEN_LOAD \n
                    SBCC_ERROR_OVER_CURRENT
* \note       N/A
*/
/******************************************************************************/
uint8 SBCC_GetInstantError(uint8 channel)
{
  uint32 ocFlag;
  uint32 olFlag = Sbcc_AllStatusRegister.SR5.Word;
  uint32 ocrAlert = Sbcc_AllStatusRegister.SR4.Word;
  uint8 ret;

  switch (channel)
  {
  case SBCC_OUT_CHANNEL_GH:
    ocFlag = Sbcc_AllStatusRegister.SR4.Word;
    break;

  case SBCC_OUT_CHANNEL_FWL_UP:
  case SBCC_OUT_CHANNEL_FWL_DOWN:
    ocFlag = Sbcc_AllStatusRegister.SR2.Word;
    break;

  default:
    ocFlag = Sbcc_AllStatusRegister.SR3.Word;
    break;
  }

  if ((ocFlag & Sbcc_GetOcMask(channel)) != 0U)
  {
    ret = SBCC_ERROR_OVER_CURRENT;
  }
  else if ((ocrAlert & Sbcc_GetOcrAlertMask(channel)) != 0U)
  {
    ret = SBCC_ERROR_OCR_ALERT;
  }
  else if ((olFlag & Sbcc_GetOlMask(channel)) != 0U)
  {
    ret = SBCC_ERROR_OPEN_LOAD;
    if ((Sbcc_GetCombined(channel) < SBCC_NUM_OF_CH)
     && ((Sbcc_Channels[channel].Flags & Sbcc_CHANNEL_RUN) != 0U)
     && ((Sbcc_Channels[Sbcc_GetCombined(channel)].Flags & Sbcc_CHANNEL_RUN) != 0U)
     && (Sbcc_Channels[channel].OutType == Sbcc_Channels[Sbcc_GetCombined(channel)].OutType))
    {
      ret = SBCC_ERROR_INVALID;
    }
  }
  else
  {
    ret = SBCC_ERROR_NONE;
  }

  return ret;
}

/******************************************************************************/
/**
* \brief      Get SR Value
*
* \author     Andy CAO
* \param      N/A
* \retval     SR value
* \note       N/A
*/
/******************************************************************************/
uint32 SBCC_GetSR1Val(void)
{
  return Sbcc_AllStatusRegister.SR1.Word;
}

uint32 SBCC_GetSR2Val(void)
{
  return Sbcc_AllStatusRegister.SR2.Word;
}

uint32 SBCC_GetSR3Val(void)
{
  return Sbcc_AllStatusRegister.SR3.Word;
}

uint32 SBCC_GetSR4Val(void)
{
  return Sbcc_AllStatusRegister.SR4.Word;
}

uint32 SBCC_GetSR5Val(void)
{
  return Sbcc_AllStatusRegister.SR5.Word;
}

uint32 SBCC_GetSR6Val(void)
{
  return Sbcc_AllStatusRegister.SR6.Word;
}

uint32 SBCC_GetSR7Val(void)
{
  return Sbcc_AllStatusRegister.SR7.Word;
}

uint32 SBCC_GetSR8Val(void)
{
  return Sbcc_AllStatusRegister.SR8.Word;
}

uint32 SBCC_GetSR9Val(void)
{
  return Sbcc_AllStatusRegister.SR9.Word;
}

uint32 SBCC_GetSR10Val(void)
{
  return Sbcc_AllStatusRegister.SR10.Word;
}

uint32 SBCC_GetSR11Val(void)
{
  return Sbcc_AllStatusRegister.SR11.Word;
}

uint32 SBCC_GetSR12Val(void)
{
  return Sbcc_AllStatusRegister.SR12.Word;
}

uint32 SBCC_GetStateError(SBCC_ErrorLog_Type_t channel)
{
  if((channel < ERROR_LOG_NUN) && (TRUE == Sbcc_PowerOnFilterFlag))
  {
    return statusReigsterError[channel];
  }
}

boolean SBCC_CheckCANSystemError(void)
{
  return (Sbcc_AllStatusRegister.SR12.Bits.SYSERR);
}
/******************************************************************************/
/** 
* \brief      SBCC_SetGH_OL
* 
* \author     H.Huisheng
* \param      st(boolean) : \n
*                          -TRUE:  enable the GH open load diagnostic function \n
*                          -FALSE: disable the GH open load diagnostic function
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_SetGH_OL(boolean st)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10);

  if (st)
  {
    temp |= Sbcc_CR10_GH_OL_EN;
  }
  else
  {
    temp &= ~Sbcc_CR10_GH_OL_EN;
  }

  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR10);
}

/******************************************************************************/
/** 
* \brief      SBCC_GetHeatOpenLoadSetting
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_GetHeatOpenLoadSetting(void)
{
  return Sbcc_HeaterOpenLoadIsSet;
}

/******************************************************************************/
/** 
* \brief      SBCC_Is5V2OverLoad
* 
* \author     Ken.X
* \param      N/A 
* \retval     5V_2 over load state
* \note       N/A
*/
/******************************************************************************/
boolean SBCC_Is5V2OverLoad(void)
{
  uint32 srVal = Sbcc_AllStatusRegister.SR2.Word;

  if ((srVal & (Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL)) != 0x0U) 
  {
    Sbcc_V2FailOrSc = TRUE;
  }
  
  return Sbcc_V2FailOrSc;
}

/******************************************************************************/
/** 
* \brief      SBCC_Clear5V2OverLoad
* 
* \author     Ken.X
* \param      N/A 
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
void SBCC_Clear5V2OverLoad(void)
{
  uint32 regVal = Sbcc_AllStatusRegister.SR2.Word;

  Sbcc_V2FailOrSc = FALSE;
  if ((regVal & (Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL)) != 0x0U)
  {
    (void)Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL);
  }
}


/******************************************************************************/
/** 
* \brief      SBCC_GetTwFlag
* 
* \author     H.Huisheng
* \param      
              IN:
                  SBCC_TEMP_CL1
                  SBCC_TEMP_CL2
                  SBCC_TEMP_CL3
                  SBCC_TEMP_CL4
                  SBCC_TEMP_CL5
                  SBCC_TEMP_CL6
              OUT:
                  TRUE: Thermal warning
                  FALE: Not warning
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
boolean  SBCC_GetTwFlag(uint8 Tw_cluster)
{
  boolean ret_value = FALSE;
  uint32 registerValue = Sbcc_AllStatusRegister.SR6.Word;
  
  if(Tw_cluster < SBCC_TEMP_CL_TOTAL)
  {
     ret_value = (((registerValue >> ((uint8)Tw_cluster + TW_CL1_Postion)) & 0x01U) == 0x01U);
  }
  return ret_value;
}

/******************************************************************************/
/** 
* \brief      SBCC_ClearWarningBit
* 
* \author     H.Huisheng
* \param      
              IN:
                  SBCC_TEMP_CL1
                  SBCC_TEMP_CL2
                  SBCC_TEMP_CL3
                  SBCC_TEMP_CL4
                  SBCC_TEMP_CL5
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_ClearWarningBit(uint8 Tw_cluster)
{
  uint32 valueMaskLinError ;

  if (Tw_cluster < SBCC_TEMP_CL_TOTAL)
  {
    #if defined(SBCC_HW_TEST)
    valueMaskLinError = Sbcc_SR6_TW_CL1
      | Sbcc_SR6_TW_CL2
      | Sbcc_SR6_TW_CL3
      | Sbcc_SR6_TW_CL4
      | Sbcc_SR6_TW_CL5
      | Sbcc_SR6_TW_CL6;
    #else
    valueMaskLinError = (uint32)(0x01) << ((uint8)Tw_cluster + TW_CL1_Postion);
    #endif
    Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR6, valueMaskLinError); 
  }
}

sint16 SBCC_GetClusterTemperature(uint8 clusterId)
{
  sint16  result  = 0;
  
  if (clusterId < SBCC_TEMP_CL_TOTAL)
  {
    uint8 srId = Sbcc_FRAME_READ_SR7 + (clusterId / 2u);
    uint8 bitoff = (clusterId % 2u == 0u) ? 0U : 12U;
    uint32 tempAD = (SBCC_GET_INPUT_TABLE(srId) >> bitoff) & 0x000003ffU;
    uint32 tempValue = tempAD * 488U / 1000U;
    
    result = (sint16)350 - (sint16)tempValue;
  }
  return result;
}

void SBCC_SuspendOCR(uint8 channel) 
{
  Sbcc_DiagInfo[channel].SuspendOCR = (uint32)TRUE;
}

void SBCC_ResumeOCR(uint8 channel) 
{
  Sbcc_DiagInfo[channel].SuspendOCR = (uint32)FALSE;
}
/******************************************************************************/
/** 
* \brief      SBCC_GoToStandByMode \n
*             Command which put SBC go to Standby mode
*
* \author     Hancen Chen
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void SBCC_CurrentMonitorEn(uint8 Channel, boolean Enable)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7);
  temp &= 0xFFFFFFF0U;

  /*
      bit3  bit2  bit1  bit0  |   CH
      0     0     0     0     |   OUT1
      0     0     0     1     |   OUT2
      0     0     1     0     |   OUT3
      0     0     1     1     |   OUT4
        ......................
      1     1     1     1     |   OUT_HS
      
  */
  if (Enable)
  {
    temp |= Sbcc_CR7_CM_EN;
    temp |= ((uint32)Sbcc_CM_CH_FlagMasks[Channel]);
  }
  else
  {
    temp &= (uint32)(~(uint32)Sbcc_CR7_CM_EN);
  }

  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR7);
}

void SBCC_Set5V2State(boolean st)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);

  if (st)
  {
    temp |= (Sbcc_CR1_V2_0|Sbcc_CR1_V2_1); 
  }
  else 
  {
    temp &= (uint32)(~(uint32)(Sbcc_CR1_V2_0|Sbcc_CR1_V2_1)); 
  }
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);
}

/*ECG function*/
void SBCC_SetEcFunctionON(void)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);
  temp &= (~(uint32)Sbcc_CR11_ECV_LS);
  temp |= Sbcc_CR11_ECON;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_SetEcFunctionOFF(void)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp &= (~(uint32)Sbcc_CR11_ECON);
  temp |= Sbcc_CR11_ECV_LS;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);

}

void SBCC_SetEcVolValue(uint8 value)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp &= (~((uint32)Sbcc_CR11_EC_0|(uint32)Sbcc_CR11_EC_1|(uint32)Sbcc_CR11_EC_2| \
              (uint32)Sbcc_CR11_EC_3|(uint32)Sbcc_CR11_EC_4|(uint32)Sbcc_CR11_EC_5)
                  );

  if(value > 63U)
  {
    value = 63U;
  }
  temp |= value;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_EcFastDischargeOn(void)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp |= Sbcc_CR11_ECV_LS;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_EcFastDischargeOFF(void)
{  
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp &= (~(uint32)Sbcc_CR11_ECV_LS);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_EcOcrON(void)
{  
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp |= Sbcc_CR11_ECV_OCR;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_EcOcrOFF(void)
{  
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11);

  temp &= (~(uint32)Sbcc_CR11_ECV_OCR);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR11, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR11);
}

void SBCC_WDResetHandler(uint8 cfg)
{  
  uint8 RegInedx;
  uint32 temp;
  /*WRITE cr1*/
  RegInedx = Sbcc_FRAME_WRITE_CR1;
  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  temp &= (~(uint32)Sbcc_CR1_TRIG); /* Feed watch dog TRIG with 0*/
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
  (void)Sbcc_CheckEnterQueue(RegInedx);

  RegInedx = Sbcc_FRAME_WRITE_CFG;
  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  temp |= Sbcc_CFGR_TSD_CONFIG; /* Selective shut down of power stage cluster */
  temp |= Sbcc_CFGR_WD_CONFIG_EN;  /* Watchdog configuration Enable */
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
  (void)Sbcc_CheckEnterQueue(RegInedx);

  RegInedx = Sbcc_FRAME_WRITE_CR2;
  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  temp |= (Sbcc_CR2_V1_RESET_1|Sbcc_CR2_V1_RESET_0); /* Set Reset threshold of V1 under 3.5V */
  switch(cfg)
  {
    case WD_CFG_IS_TWS1:
      temp &= (~((uint32)Sbcc_CR2_WD_TIME_0|(uint32)Sbcc_CR2_WD_TIME_1)); /* Set Watchdog Trigger Time to TSW1(from 7.5ms to 12ms) */
    break;
    case WD_CFG_IS_TWS2: /* Set Watchdog Trigger Time to TSW2(from 37.5ms to 60ms) */
      temp |= Sbcc_CR2_WD_TIME_0; 
      temp &= (~(uint32)Sbcc_CR2_WD_TIME_1);
    break;
    case WD_CFG_IS_TWS3: /* Set Watchdog Trigger Time to TSW3(from 75ms to 120ms) */
      temp &= (~(uint32)Sbcc_CR2_WD_TIME_0);
      temp |= Sbcc_CR2_WD_TIME_1;
    break;
    default:
      temp |= (Sbcc_CR2_WD_TIME_0|Sbcc_CR2_WD_TIME_1); /* Set Watchdog Trigger Time to TSW4(from 160ms to 240ms) */
    break;
  }
  Sbcc_SetWDFailFlag(FALSE);
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
  (void)Sbcc_CheckEnterQueue(RegInedx);
}

boolean SBCC_CheckCommunicition(void)
{
  boolean ret = TRUE;
  uint32 regVal = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_DEV_NO1);

  if((regVal & 0x00550000U) == 0x00550000U);
  {
    ret = FALSE;
  }
  return ret;
}


/* _____ L O C A L - F U N C T I O N S _______________________________________*/
/******************************************************************************/
/** 
* \brief      Sbcc_SBCRegisterInit \n
*             Init the SPI outgoing messages 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_SBCRegisterInit(void)
{
  uint32 temp;

  /*let the error handle process to reset the flag*/
  /*Sbcc_CheckEnterQueue(Sbcc_FRAME_RESET);*/
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_SR1);/*Need the Vpor flag*/

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);
  /*temp |= (Sbcc_CR1_HEN);*/ /* H-bridge enabled */
  temp |= (Sbcc_CR1_V2_0|Sbcc_CR1_V2_1); /* V2 ON in all modes */
  temp |= Sbcc_CR1_CAN_RXEN;
  temp &= (uint32)(~(uint32)Sbcc_CR1_CANTO_IRQ_EN);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10);
  temp &= (uint32)(~(uint32)(Sbcc_CR10_SD|Sbcc_CR10_GH_TH_0|Sbcc_CR10_GH_TH_1|Sbcc_CR10_GH_TH_2
           |Sbcc_CR10_DIAG_2|Sbcc_CR10_DIAG_1|Sbcc_CR10_DIAG_0)); /* Slow decay and Slow decay single */
  temp |= (Sbcc_CR10_SDS|Sbcc_CR10_GH_OL_EN|SBCC_SCD_HB_750MV);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR10);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG);
  temp |= Sbcc_CFGR_TSD_CONFIG;    /* Selective shut down of power stage cluster */
  temp |= Sbcc_CFGR_WD_CONFIG_EN;  /* Watchdog configuration Enable */
  /*temp |= Sbcc_CFGR_ECV_HV; */       /*EC output 1.5V*/
  
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CFG);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR2);
  temp |= (Sbcc_CR2_V1_RESET_1|Sbcc_CR2_V1_RESET_0); /* Set Reset threshold of V1 under 3.5V */
  temp |= (Sbcc_CR2_WD_TIME_0|Sbcc_CR2_WD_TIME_1); /* Set Watchdog Trigger Time to TSW4(from 150ms to 240ms) */
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR2, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR2);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR3);
  /* Disable overvoltage protection and enable recover automatically */
  temp &= (uint32)(~(uint32)(Sbcc_CR3_VSREG_LOCK_EN|Sbcc_CR3_VS_LOCK_EN|Sbcc_CR3_VS_OV_SD_EN|Sbcc_CR3_VSREG_OV_SD_EN));
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR3, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR3);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR5);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR5, temp);/* disable the OUT_HS */
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR5);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7);
  temp |= (Sbcc_CR7_HB_TON_1|Sbcc_CR7_HB_TON_0|Sbcc_CR7_HS_TON_1|Sbcc_CR7_HS_TON_0);
  temp &= (uint32)(~(uint32)Sbcc_CR7_OCR_FREQ);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR7);

  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR12);
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR12, temp);/* Init PWM Freq */
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR12);
  
  temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR9);
  temp |= Sbcc_CR9_OUT10_OC|Sbcc_CR9_OUT10_OL|Sbcc_CR9_OUT8_RDSON|Sbcc_CR9_OUT12_OC|Sbcc_CR9_OUT12_OL;
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR9, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR9);

  /* fix defect 103195, due to time limit, just init WELCOME_LET output to off,
     consider to init all output to off in next phase */
  //(void)SBCC_SwitchOnOff(SBCC_WELLCOME_LED, FALSE);
}

/******************************************************************************/
/** 
* \brief      Sbcc_ReadStatusReg \n
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_ReadStatusReg(void)
{
  uint8 i;
  static uint16 updateOutCnt;
#if 1
  updateOutCnt = (updateOutCnt + 1U) % 10U;
  switch (updateOutCnt)
  {
  case 0U:
    (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR4);
    break;
  case 2U:
    (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR5);
    break;
  case 4U:
    (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR6);
    break;
  default:
    break;
  }
  
  /* Insert read Status Register command into queue */
  for(i = Sbcc_FRAME_READ_SR1; i<=Sbcc_FRAME_READ_SR12; i++)
  {
    (void)SBCC_ReadRegisterValue(i);
  }
#endif
}
/******************************************************************************/
/** 
* \brief      Sbcc_SendCmdHandler \n
*             5ms handler, use to send first command to SBC
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
void Sbcc_SendCmdHandler(void)
{
  uint8 i;
  uint8 CurIndexNum;
    /* Read Device No.1 to verify SPI communication */
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_READ_DEV_NO1);
  
  /* Start to send first command to SBC */
  if(SPI_SEQ_PENDING != Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_0_SBC)) 
  {
    if(SBCC_INDEX_TABLE_PING == Sbcc_UsingPingOrPong)
    {
      /* Transfer new frames */
      if (Sbcc_OutputIndexTotalNumPing > 0U)
      {
        CurIndexNum = Sbcc_OutputIndexTotalNumPing;
        i = 0;
        while (CurIndexNum > 0U)
        {
          Sbcc_TxBuffer[i] = Sbcc_OutputDataTable[Sbcc_OutputIndexTablePing[i]];
          /* if read and clear, then we need clear the buffer */
          if ((Sbcc_OutputIndexTablePing[i] >= Sbcc_FRAME_READ_CLEAR_SR1) 
          && (Sbcc_OutputIndexTablePing[i] <= Sbcc_FRAME_READ_CLEAR_SR12))
          {
            Sbcc_OutputDataTable[Sbcc_OutputIndexTablePing[i]] &= 0xFF000000U;
          }
          CurIndexNum--;
          i++;
        }
        Sbcc_UsingPingOrPong = SBCC_INDEX_TABLE_PONG;
        Sbcc_req.receBuff = (void *)Sbcc_RxBuffer;
        Sbcc_req.sendBuff = (void *)Sbcc_TxBuffer;
        Sbcc_req.transferCount = ((uint16)Sbcc_OutputIndexTotalNumPing);
        (void)SPIC_Master_Send(&Sbcc_req);
      }
    }
    else
    {
      /* Transfer new frames */
      if(Sbcc_OutputIndexTotalNumPong > 0U)
      {
        CurIndexNum = Sbcc_OutputIndexTotalNumPong;
        i = 0;
        while (CurIndexNum > 0U)
        {
          Sbcc_TxBuffer[i] = Sbcc_OutputDataTable[Sbcc_OutputIndexTablePong[i]];
          /* if read and clear, then we need clear the buffer */
          if ((Sbcc_OutputIndexTablePong[i] >= Sbcc_FRAME_READ_CLEAR_SR1) 
          && (Sbcc_OutputIndexTablePong[i] <= Sbcc_FRAME_READ_CLEAR_SR12))
          {
            Sbcc_OutputDataTable[Sbcc_OutputIndexTablePong[i]] &= 0xFF000000U;
          }
          CurIndexNum--;
          i++;
        }
        Sbcc_UsingPingOrPong = SBCC_INDEX_TABLE_PING;   /* Switch index table flag */
        Sbcc_req.receBuff = (void *)Sbcc_RxBuffer;
        Sbcc_req.sendBuff = (void *)Sbcc_TxBuffer;
        Sbcc_req.transferCount = ((uint16)Sbcc_OutputIndexTotalNumPong);
        (void)SPIC_Master_Send(&Sbcc_req);
      }
    }
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckEnterQueue
* 
* \author     H.Huisheng
* \param      Index(uint8) : \n
*             -SBC register operation index, please refer to Sbcc_priv.h for the detail definiton                       
* \retval     boolean : \n   
                       -TRUE : success \n
                       -FALSE: error
* \note       N/A
*/
/******************************************************************************/
static boolean Sbcc_CheckEnterQueue(uint8 Index)
{
  boolean RetVal = TRUE;
  uint8 *CurIndexTablePtr, *MaskPtr;
  uint8 CurIndexNum;

  SuspendAllInterrupts();
  if(SBCC_INDEX_TABLE_PING == Sbcc_UsingPingOrPong)
  {
    CurIndexTablePtr = Sbcc_OutputIndexTablePing;
    CurIndexNum = Sbcc_OutputIndexTotalNumPing;
    MaskPtr = Sbcc_QueueMaskPing;
  }
  else
  {
    CurIndexTablePtr = Sbcc_OutputIndexTablePong;
    CurIndexNum = Sbcc_OutputIndexTotalNumPong;
    MaskPtr = Sbcc_QueueMaskPong;
  }

  /* If the index is not in the table, insert it */
  if((MaskPtr[Index/8U] & ((uint8)(1U << (Index % 8u)))) == 0U)
  {
    if(CurIndexNum < SBCC_MAX_INDEX_NUM)
    {
      CurIndexTablePtr[CurIndexNum++] = Index;
      MaskPtr[Index/8U] |= (uint8)(1U << (Index % 8u));
    }
    else
    {
      RetVal = FALSE;
    }
  }

  if(SBCC_INDEX_TABLE_PING == Sbcc_UsingPingOrPong)
  {
    Sbcc_OutputIndexTotalNumPing = CurIndexNum;
  }
  else
  {
    Sbcc_OutputIndexTotalNumPong = CurIndexNum;
  }
  ResumeAllInterrupts();

  return RetVal;
}

/******************************************************************************/
/** 
* \brief      Sbcc_OutputHSOrLS
* 
* \author     H.Huisheng     
* \param      HS_Bit(uint32)             :HS bit configuration \n
* \param      LS_Bit(uint32)             :LS bit configuration \n
* \param      Status(SBCC_OutputState_t) : \n
*                                       SBCC_OutputLow = 0  \n
*                                       SBCC_OutputHigh = 1 \n
*                                       SBCC_OutputOff = 2 

* \retval     operation result : \n
              -TRUE: success  
              -FALSE: fail  
* \note       refer to local varaible Sbcc_OutputBitCfg[] for more information
*/
/******************************************************************************/
static boolean Sbcc_OutputHSOrLS(uint32 HS_Bit, uint32 LS_Bit, SBCC_OutputState_t Status)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR4);
  boolean RetVal;

  temp &= ~(HS_Bit|LS_Bit);
  switch(Status)
  {
    case SBCC_OutputLow:
      temp |= LS_Bit;
      break;
    case SBCC_OutputHigh:
      temp |= HS_Bit;
      break;
    case SBCC_OutputOff:
    default:break;
  }
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR4, temp);
  RetVal = Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR4);

  return RetVal;
}

/******************************************************************************/
/** 
* \brief      Sbcc_OutputHS
*             sbc high side output configuration
* \author     H.Huisheng
* \param      Index(uint8)               :register operation index, refer to sbcc_priv.h \n
* \param      HS_Bit(uint8)              :output configuration bit, refer to SBC manual \n
* \param      BitMask(uint32)            :refer to SBC manual \n
* \param      status(SBCC_OutputState_t) : \n
                                          -SBCC_OutputLow = 0 \n
                                          -SBCC_OutputHigh = 1 \n
                                          -SBCC_OutputOff = 2
* \retval     operation result : \n
                                -TRUE: success  
                                -FALSE: fail 
* \note       refer to local varaible Sbcc_OutputBitCfg[] for more information
*/
/******************************************************************************/
static boolean Sbcc_OutputHS(uint8 Index, uint32 HS_Bit, uint32 BitMask, SBCC_OutputState_t Status)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Index);

  temp &= ~BitMask;
  switch(Status)
  {
    case SBCC_OutputHigh:
      temp |= HS_Bit;
      break;
    case SBCC_OutputOff:
    default:break;
  }
  SBCC_SET_OUTPUT_TABLE(Index, temp);

  return Sbcc_CheckEnterQueue(Index);
}

/******************************************************************************/
/** 
* \brief      Sbcc_EnableDisableOCR
* 
* \author     H.Huisheng    
* \param      channel(uint8)  : output channel number
* \param      Enalbe(boolean) : TRUE or FALSE
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_EnableDisableOCR(uint8 Channel, boolean Enable)
{
  if (Sbcc_GetOcrMask(Channel) != 0U)
  {
    uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7);

    /* Overcurrent recovery for OUTx */
    if(Enable)
    {
      temp |= Sbcc_GetOcrMask(Channel);
    }
    else
    {
      temp &= ~(Sbcc_GetOcrMask(Channel));
    }
    SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR7, temp);
    (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR7);

    /* Enable Overcurrent Recovery with Thermal Expiration for OUTx */
    temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR8);
    if(Enable)
    {
      temp |= Sbcc_GetOcrMask(Channel);
    }
    else
    {
      temp &= ~(Sbcc_GetOcrMask(Channel));
    }
    SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR8, temp);
    (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR8);
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_StatusRegisterUpdate \n
*             Update status register value to local variable
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_StatusRegisterUpdate(void)
{
  Sbcc_AllStatusRegister.SR1.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR1);
  Sbcc_AllStatusRegister.SR2.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR2);
  Sbcc_AllStatusRegister.SR3.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR3);
  Sbcc_AllStatusRegister.SR4.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR4);
  Sbcc_AllStatusRegister.SR5.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR5);
  Sbcc_AllStatusRegister.SR6.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR6);
  Sbcc_AllStatusRegister.SR7.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR7);
  Sbcc_AllStatusRegister.SR8.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR8);
  Sbcc_AllStatusRegister.SR9.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR9);
  Sbcc_AllStatusRegister.SR10.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR10);
  Sbcc_AllStatusRegister.SR11.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR11);
  Sbcc_AllStatusRegister.SR12.Word = SBCC_GET_INPUT_TABLE(Sbcc_FRAME_READ_SR12);
  #ifdef SBCC_CAN_DEBUG_MESSAGE
  myMsg[0] = (Sbcc_AllStatusRegister.SR2.Word >> 24u) & 0xFFu;
  myMsg[1] = (Sbcc_AllStatusRegister.SR2.Word >> 16u) & 0xFFu;
  myMsg[2] = (Sbcc_AllStatusRegister.SR2.Word >> 8u) & 0xFFu;
  myMsg[3] = (Sbcc_AllStatusRegister.SR2.Word) & 0xFFu;
  myMsg[4] = (Sbcc_AllStatusRegister.SR12.Word >> 16u) & 0xFFu;
  myMsg[5] = (Sbcc_AllStatusRegister.SR12.Word >> 8u) & 0xFFu;
  myMsg[6] = (Sbcc_AllStatusRegister.SR12.Word) & 0xFFu;
  myMsg[7] = Sbcc_CheckPncRegister();
  #endif
  Sbcc_Gsb_Value = (uint8)(Sbcc_AllStatusRegister.SR1.Word >> 24U);  
  /*check wake up flag
    keep the code, because bootloader need this function
 */
  if(((Sbcc_AllStatusRegister.SR1.Word & Sbcc_SR1_CAN) != 0u))
  {
    Sbcc_wakedupByCAN = TRUE;
    Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, Sbcc_SR1_CAN);
  }
  
  if(((Sbcc_AllStatusRegister.SR1.Word & Sbcc_SR1_WU) != 0u))
  {
    Sbcc_wakedupByWU = TRUE;
    Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, Sbcc_SR1_WU);
  }
  
}

/******************************************************************************/
/** 
* \brief      Sbcc_ClearBitsOfSR
* 
* \author     H.Huisheng     
* \param      RegInedx(uint8)      : register index, refer to sbcc_priv.h 
* \param      valueToWrite(uint32) : value to be wrote to register
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_ClearBitsOfSR(uint8 RegInedx,  uint32 valueToWrite)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(RegInedx);

  temp |= valueToWrite;
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
  (void)Sbcc_CheckEnterQueue(RegInedx);
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR1 \n
*             error check for SR1 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR1(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR1.Word;
  uint32 valueToClearError = 0x00;

  /*check Vpor*/
  if((registerValue & Sbcc_SR1_VPOR) != 0x00U)
  {
    if(Sbcc_VporFlag == FALSE)
    {
      Sbcc_VporFlag = TRUE;
    }
#if 1
    valueToClearError = Sbcc_SR1_VPOR;
#endif
  }

  /*check wdfail*/
  if((registerValue & Sbcc_SR1_WDFAIL) != 0x00U)
  {
    /*valueToClearError |= Sbcc_SR1_WDFAIL; Msg(7:2984) This operation is redundant. The value of the result is always '2'.*/
    valueToClearError = Sbcc_SR1_WDFAIL;
	  Sbcc_SetWDFailFlag(TRUE);
  }

  /*check v1uv*/
  if((registerValue & Sbcc_SR1_V1UV) != 0x00U)
  {
    valueToClearError |= Sbcc_SR1_V1UV;
  }

   /*check fs*/
  if((registerValue & Sbcc_SR1_FS) != 0x00U)
  {
    valueToClearError |= Sbcc_SR1_FS;
  }

   /*check tsd2*/
  if((registerValue & Sbcc_SR1_TSD2) != 0x00U)
  {
    valueToClearError |= Sbcc_SR1_TSD2;
  }

  return valueToClearError;
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR2 \n
*             error check for SR2 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR2(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR2.Word;
  uint32 valueToClearError = registerValue & 0x00ff0fffU;
  /* ignore the LIN related error bits */
 /* uint32 valueMaskLinError = */ /*~(Sbcc_SR2_LIN_PDOM | Sbcc_SR2_LIN_TDOM | Sbcc_SR2_LIN_PREC)*/;

  return (valueToClearError /*& valueMaskLinError*/);
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR3 \n
*             error check for SR3 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR3(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR3.Word;
  /* SR3 is all for error bits */
  uint32 valueToClearError = registerValue & 0x00ffffffU;
  
  return valueToClearError;
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR4 \n
*             error check for SR4 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR4(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR4.Word;
  uint32 valueToClearError = 0x00U;

  if((registerValue & Sbcc_SR4_ECV_OC) != 0x0U)
  {
    valueToClearError = Sbcc_SR4_ECV_OC;
  }

  if((registerValue & Sbcc_SR4_DS_HEAT) != 0x0U)
  {
    valueToClearError |= Sbcc_SR4_DS_HEAT;
  }
  
  return valueToClearError;
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR5 \n
*             error check for SR5 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR5(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR5.Word;
  /* SR5 is all for error bits */
  uint32 valueToClearError = registerValue & 0x00ffffffU;

  return valueToClearError;
}


/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR6 \n
*             error check for Status register 6 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR6(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR6.Word;
  uint32 valueMaskLinError = Sbcc_SR6_TSD1_CL1
    | Sbcc_SR6_TSD1_CL2
    | Sbcc_SR6_TSD1_CL3
    | Sbcc_SR6_TSD1_CL4
    | Sbcc_SR6_TSD1_CL5
    | Sbcc_SR6_TSD1_CL6;
   
  return (registerValue & valueMaskLinError);
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSR12 \n
*             error check for SR12 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static uint32 Sbcc_CheckSR12(void)
{
  uint32 registerValue = Sbcc_AllStatusRegister.SR12.Word;
  uint32 valueToClearError = 0x00U;

  if((registerValue & Sbcc_SR12_FERR) != 0x0U)
  {
    valueToClearError = Sbcc_SR12_FERR;
  }

  if((registerValue & Sbcc_SR12_OSC_FAIL) != 0x0U)
  {
    valueToClearError |= Sbcc_SR12_OSC_FAIL;
  }

  if((registerValue & Sbcc_SR12_SYSERR) != 0x0U)
  {
    valueToClearError |= Sbcc_SR12_SYSERR;
  }

  return valueToClearError;
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckRSTBError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckRSTBError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;

  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_RSTB) != 0x00U)/*RSTB Error*/
  {
    srValue = Sbcc_CheckSR1();
    
    if(srValue != 0x00U)
    {
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, srValue);  
      errorLog[(uint8)ERROR_LOG_SR1] |= srValue;
    }
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckSPIError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckSPIError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue ;
  
  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_SPIE) != 0x00U)/*SPIE Error*/
  {
    srValue = Sbcc_CheckSR2();     
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, srValue /*& (~(Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL))*/);    
      errorLog[(uint8)ERROR_LOG_SR2] |= srValue;
    }   
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckPleError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckPleError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;

  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_PLE) != 0x00U)/*PLE Error*/
  {
    srValue = Sbcc_CheckSR2(); 
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, srValue /*& (~(Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL))*/);      
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR2] |= srValue;
    } 
    
    srValue = Sbcc_CheckSR12();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR12, srValue);      
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR12] |= srValue;
    }   
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckFeError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckFeError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;

  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_FE) != 0x00U)/*FE Error*/
  {
    srValue = Sbcc_CheckSR2(); 
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, srValue /*& (~(Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL))*/);      
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR2] |= srValue;
    }   
    
    srValue = Sbcc_CheckSR3(); 
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      /*Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR3, srValue);*/   
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR3] |= srValue;
    }   
    srValue = Sbcc_CheckSR4();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      /*Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR4, srValue);*/     
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR4] |= srValue;
    }   
    srValue = Sbcc_CheckSR5();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      /*Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR5, srValue);*/    
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR5] |= srValue;
    }   
  }

}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckDEError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckDEError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;
  
  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_DE) != 0x00U)/*DE Error*/
  {
    srValue = Sbcc_CheckSR2();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, srValue /*& (~(Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL))*/);      
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR6] |= srValue;
    }   
    srValue = Sbcc_CheckSR6();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR6, srValue);      
      /*save the sr value*/
      errorLog[(uint8)ERROR_LOG_SR6] |= srValue;
    }   
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckGwError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckGwError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;

  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_GW) != 0x00U)/*GW Error*/
  {
    srValue = Sbcc_CheckSR2();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR2, srValue /*& (~(Sbcc_SR2_V2SC | Sbcc_SR2_V2FAIL))*/);     
      errorLog[(uint8)ERROR_LOG_SR2] |= srValue;
    }   
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_CheckFsError
* 
* \author     H.Huisheng
* \param      errorLog--array, save the error infomation
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_CheckFsError(uint32 errorLog[(uint8)ERROR_LOG_NUN])
{
  uint32 srValue;
  
  if((Sbcc_Gsb_Value & Sbcc_BTS_GSB_FS) != 0x00U)/*FS Error*/
  {
    srValue = Sbcc_CheckSR1();
    if(srValue != 0x00U)
    { 
      /*clear the error bits*/
      Sbcc_ClearBitsOfSR(Sbcc_FRAME_READ_CLEAR_SR1, srValue);     
      errorLog[(uint8)ERROR_LOG_SR1] |= srValue;
    }   
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_NormalErrorHandler \n
*             Sbc error clear function. To exit from fail safe mode 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_NormalErrorHandler(void)
{
  static uint16 powerOnfilter = 0;
  
  if((Sbcc_Gsb_Value != Sbcc_BTS_GSB_GSBN)&&(Sbcc_Gsb_Value != 0x00U))
  {
    /*remove the LIN error first*/ 
    if(Sbcc_CheckSR2() == 0x0U)
    {
      statusReigsterError[(uint8)ERROR_LOG_GLOBAL] |= (uint32)(Sbcc_Gsb_Value & (~((uint32)Sbcc_BTS_GSB_PLE)));
    }
    else
    {
      statusReigsterError[(uint8)ERROR_LOG_GLOBAL] |= (uint32)Sbcc_Gsb_Value;
    }
    
    /*Check reset bit error*/
    Sbcc_CheckRSTBError(statusReigsterError);
  
    /*Check SPI error*/
    Sbcc_CheckSPIError(statusReigsterError);
   
    /*Check physical layer error*/
    Sbcc_CheckPleError(statusReigsterError);

    /*Check function error*/
    Sbcc_CheckFeError(statusReigsterError);

    /*Check device error*/
    Sbcc_CheckDEError(statusReigsterError);
  
    /*Check Global warning error*/
    Sbcc_CheckGwError(statusReigsterError);

    /*Check Fail safe error*/
    Sbcc_CheckFsError(statusReigsterError);
  
  }
 
  if(powerOnfilter < Sbcc_PowerOnFilterTime)
  {
    powerOnfilter += Sbcc_BasiTimerPeriod;
  }
  else
  {
    /*log the sbcc error status*/
    //ERROR_LOG_SaveErrorStatus(statusReigsterError);
    Sbcc_PowerOnFilterFlag = TRUE;
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_ClearErrFlag
* 
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_ClearErrFlag(uint8 channel, uint8 errType)
{
  uint8 regIdx = 0;
  uint32 regVal = 0;

  Sbcc_DiagInfo[channel].WaitFb = (uint32)(TRUE);
  if (errType == SBCC_ERROR_OVER_CURRENT)
  {
    switch (channel)
    {
    case SBCC_OUT_CHANNEL_GH:
      regIdx = Sbcc_FRAME_READ_CLEAR_SR4;
      regVal = Sbcc_AllStatusRegister.SR4.Word & 0x00ffffffU;
      break;

    case SBCC_OUT_CHANNEL_FWL_UP:
    case SBCC_OUT_CHANNEL_FWL_DOWN:
      regIdx = Sbcc_FRAME_READ_CLEAR_SR2;
      regVal = Sbcc_AllStatusRegister.SR2.Word & 0x00ffffffU;
      break;

    default:
      regIdx = Sbcc_FRAME_READ_CLEAR_SR3;
      regVal = Sbcc_AllStatusRegister.SR3.Word & 0x00ffffffU;
      break;
    }
    regVal = regVal & Sbcc_GetOcMask(channel);
  }
  else if (errType == SBCC_ERROR_OPEN_LOAD)
  {
    regIdx = Sbcc_FRAME_READ_CLEAR_SR5;
    regVal = Sbcc_AllStatusRegister.SR5.Word & 0x00ffffffU;
    regVal = regVal & Sbcc_GetOlMask(channel);
  }
  else
  {
    /* Do Nothing */
  }

  if (regVal != 0U)
  {
    Sbcc_ClearBitsOfSR(regIdx, regVal);
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_AbnormalConCheck
* 
* \author     H.Huisheng
* \param      
              index: channel index
              error: current error type

* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_AbnormalConHandle(uint8 index, uint8 err)
{
  uint8 clusterId = Sbcc_GetTempClId(index);
  boolean overTemp = (SBCC_GetClusterTemperature(clusterId) > Sbcc_PROTECT_TEMP);
  boolean tempValue = SBCC_GetTwFlag(clusterId);
  static uint8 preErr[SBCC_NUM_OF_PHY_CH];
  
  /* if output with OCR and OC happened or temperature too high, switch off output immediately */
  if (((err == SBCC_ERROR_OVER_CURRENT && Sbcc_GetOcrMask(index) != 0U && ((boolean)Sbcc_DiagInfo[index].SuspendOCR || (preErr[index] == err))) \
    || (tempValue && overTemp))
    && Sbcc_DiagInfo[index].State != Sbcc_DIAG_IDLE
    && ((Sbcc_Channels[index].Flags & Sbcc_CHANNEL_RUN) != 0U))
  {
    (void)SBCC_SwitchOnOff(index, FALSE);
    Sbcc_DiagInfo[index].ProtectTimer = Sbcc_GetProtectTime(index);
    Sbcc_DiagInfo[index].ErrType = SBCC_ERROR_OVER_CURRENT;
    Sbcc_DiagInfo[index].State = Sbcc_DIAG_IDLE;
  }

  preErr[index] = err;
}

/******************************************************************************/
/** 
* \brief      Sbcc_StartDiag
* 
* \author     H.Huisheng
* \param      
              index: channel index
              error: current error type
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_StartDiag(uint8 index, uint8 error)
{
  switch (Sbcc_DiagInfo[index].State)
  {
   case Sbcc_DIAG_MONITORING:
     if (Sbcc_DiagInfo[index].ErrType != error)
     {
       Sbcc_DiagInfo[index].ErrType = SBCC_ERROR_INVALID;
       if (error == SBCC_ERROR_NONE)
       {
         Sbcc_DiagInfo[index].State = Sbcc_DIAG_DEMATURE;
         Sbcc_DiagInfo[index].DiagTimer = Sbcc_GetDematureTime(index);
       }
     }

     if (error != SBCC_ERROR_NONE)
     {
       if (error == SBCC_ERROR_OPEN_LOAD)
       {
         Sbcc_ClearErrFlag(index, error);
         Sbcc_DiagInfo[index].DiagTimer = Sbcc_GetOlMatureTime(index);
       }
       else
       {
         Sbcc_DiagInfo[index].DiagTimer = Sbcc_GetOcMatureTime(index);
       }
       
       Sbcc_DiagInfo[index].State = Sbcc_DIAG_MATURE;
     }
   break;

 case Sbcc_DIAG_MATURE:
 case Sbcc_DIAG_DEMATURE:
   if ((Sbcc_DiagInfo[index].DiagTimer % Sbcc_DIAG_SW_OCR_PERIOD == 0U)
    || (error == SBCC_ERROR_OPEN_LOAD))
   {
     if ((index != SBCC_MIRROR_HEAT) && (index != SBCC_EC))
     {
       Sbcc_ClearErrFlag(index, error);
     }
   }

   if (error != Sbcc_DiagInfo[index].LastErr)
   {
     Sbcc_DiagInfo[index].State = Sbcc_DIAG_MONITORING;
   }
   else if (Sbcc_DiagInfo[index].DiagTimer == 0U)
   {
     Sbcc_DiagInfo[index].ErrType = error;
     if (error == SBCC_ERROR_OCR_ALERT || error == SBCC_ERROR_OVER_CURRENT)
     {
       (void)SBCC_SwitchOnOff(index, FALSE);
       Sbcc_DiagInfo[index].ProtectTimer = Sbcc_GetProtectTime(index);
       Sbcc_DiagInfo[index].State = Sbcc_DIAG_IDLE;
     }
     else
     {
       Sbcc_DiagInfo[index].State = Sbcc_DIAG_MONITORING;
     }
   }
   else
   {
     /* Do Nothing */
   }
   break;

 case Sbcc_DIAG_IDLE:
   break;

 default:
   /* Recover the state machine if the RAM data been modified unexpectedly */
   Sbcc_DiagInfo[index].State = Sbcc_DIAG_MONITORING;
   break;
 }
 
  Sbcc_DiagInfo[index].LastErr = error;
}

/******************************************************************************/
/** 
* \brief      Sbcc_DiagManager \n
*             Open load and over current diagnostic function 
*
* \author     H.Huisheng
* \param      N/A 
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_DiagManager(void)
{
  uint8 i;
  uint8 currErr;

  for(i = 0; i < SBCC_NUM_OF_PHY_CH; i++)
  {
    if (Sbcc_DiagInfo[i].DiagTimer > 0U)
    {
      Sbcc_DiagInfo[i].DiagTimer--;
    }
    
    if (Sbcc_DiagInfo[i].ProtectTimer > 0U)
    {
      Sbcc_DiagInfo[i].ProtectTimer--;
    }

    /* Wait diag flags */
    if (Sbcc_DiagInfo[i].WaitFb == (uint32)0x01U) 
    {
      Sbcc_DiagInfo[i].WaitFb = (uint32)FALSE;
      continue;
    }

    if ((i == SBCC_OUT_CHANNEL_HS) || (i == SBCC_OUT_CHANNEL10)
    || ((i == SBCC_OUT_CHANNEL_FWL_UP || i == SBCC_OUT_CHANNEL_FWL_DOWN)
        && (Sbcc_DlcSt == SBCC_OutputOff))) 
    {
      continue;
    }
    
    currErr = SBCC_GetInstantError(i);
    
    Sbcc_AbnormalConHandle(i, currErr);
    Sbcc_StartDiag(i, currErr);
  
  }
}

/******************************************************************************/
/** 
* \brief      Sbcc_OutCfg
* \author     H.Huisheng
* \param      channel(uint8) : SBC output channel number \n
* \param      cfg(uint8)     : configuration value
* \retval     operation result : \n
                                -TRUE  : success
                                -FALSE : fail
* \note       N/A
*/
/******************************************************************************/
static boolean Sbcc_OutCfg(uint8 channel, uint8 cfg)
{
  uint8 RegInedx;
  uint32 temp;
  boolean RetVal = FALSE;

  if (channel < SBCC_NUM_OF_PHY_CH)
  {
    RegInedx = Sbcc_OutputBitCfg[channel].RegInedx;

    switch (channel)
    {
    case SBCC_OUT_CHANNEL7:
    case SBCC_OUT_CHANNEL8:
    case SBCC_OUT_CHANNEL9:
    case SBCC_OUT_CHANNEL10:
    case SBCC_OUT_CHANNEL11:
    case SBCC_OUT_CHANNEL12:
    case SBCC_OUT_CHANNEL13:
    case SBCC_OUT_CHANNEL14:
    case SBCC_OUT_CHANNEL15:
    case SBCC_OUT_CHANNEL_HS:
      temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
      temp &= ~Sbcc_OutputBitCfg[channel].BitMask;
      temp |= (uint32)cfg << Sbcc_OutputBitCfg[channel].BitOffset;
      SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
      RetVal = Sbcc_CheckEnterQueue(RegInedx);
      break;

    default:
      break;
    }
  }

  return RetVal;
}

/******************************************************************************/
/** 
* \brief      Sbcc_SyncTrigBitWithWD \n
*             Keep the Trig bit the same with WD feed value
*
* \author     H.Huisheng
* \param      value(uint32) : value of CR1 register           
* \retval     value to be wrote to CR1 
* \note       
              <
                whenever write to CR1 or CFG, check the TRIG bit first
              >
*/
/******************************************************************************/
static uint32 Sbcc_SyncTrigBitWithWD(uint32 value)
{
  uint32 ret_val = value;

  // if(WD_GetWDToggleValue() == TRUE)
  // {
  //   ret_val |= Sbcc_CR1_TRIG;
  // }
  // else
  // {
  //   ret_val &= (uint32)~((uint32)Sbcc_CR1_TRIG);
  // }
  return ret_val;
}

/******************************************************************************/
/** 
* \brief      Sbcc_FWL_UpAction
*             Front window up action function
* \author     H.Huisheng
* \param      
              RegIndex: Control register index to be configured
              Status:   Action type
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
#if 0
static void Sbcc_FWL_UpAction(uint8 RegInedx, boolean Status)
{
  uint32 temp;

  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  if(Status)
  {
    temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
    PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_HIGH);
    /*PPFC_WriteChannel(PPFC_FWL_PWM, TRUE);*/
    Sbcc_LastFLWin_Action = SBCC_OUT_CHANNEL_FWL_UP;
  }
  else
  {
    if(SBCC_OUT_CHANNEL_FWL_UP == Sbcc_LastFLWin_Action)
    {
      temp &= (uint32)(~(uint32)Sbcc_CR1_HEN); /* H-bridge disable */
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_HIGH);
      /*PPFC_WriteChannel(PPFC_FWL_PWM, FALSE);*/
      Sbcc_LastFLWin_Action = SBCC_OUT_CHANNEL_INVALID;
    }
  }
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
}
#endif

/******************************************************************************/
/** 
* \brief      Sbcc_DLC_Action
*             D
* \author     Tulip
* \param      
              RegIndex: Control register index to be configured
              Status:   Action type
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_DLC_Action(uint8 RegInedx, SBCC_OutputState_t Status)
{
  uint32 temp;
  uint32 tempCR10;
  tempCR10 = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10);
  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  
  switch(Status)
  {
    case SBCC_DlcPwrRelease: /* CH1:HIGH, CH2:GND (6)*/
    {
      Sbcc_DmConfigure(FALSE);
      temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_HIGH);
    }
    break;
    case SBCC_DlcPwrReset:/* CH1:GND, CH2:OPEN sd:1,sds:0 */
    {
      Sbcc_DmConfigure(FALSE);
      temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
      tempCR10 &= (uint32)(~(uint32)Sbcc_CR10_SD);
      tempCR10 |= Sbcc_CR10_SDS;
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
    }
    break;
    case SBCC_DlcPwrCrash:/* CH1:GND, CH2:HIGH  sd:X,sds:X */
    {
      Sbcc_DmConfigure(FALSE);
      temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
    }
    break;
    case SBCC_DlcPwrBreakOn:/* CH1:GND, CH2:GND(7) sd:0,sds:0 */
    case SBCC_DlcPwrPclLock:/* CH1:GND, CH2:GND*/
    {
      if (TRUE != EOL_M_CheckIsRunning())
      {
        Sbcc_DmConfigure(FALSE);
        temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
        tempCR10 &= (uint32)(~(uint32)Sbcc_CR10_SD);
        tempCR10 &= (uint32)(~(uint32)Sbcc_CR10_SDS);
      }
      else
      {
        Sbcc_DmConfigure(FALSE);
        temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
        PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
      }
    }
    break;
    case SBCC_DlcPwrPclUnlock: /* CH1:HIGH, CH2:HIGH(11) */
    {
      if (TRUE != EOL_M_CheckIsRunning())
      {
        Sbcc_DmConfigure(FALSE);
        temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
        tempCR10 |= Sbcc_CR10_SD;
        tempCR10 &= (uint32)(~(uint32)Sbcc_CR10_SDS);
      }
      else
      {
        Sbcc_DmConfigure(FALSE);
        temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
        PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_HIGH);
      }
    }
    break;

    default: /* CH1:OPEN, CH2:OPEN (IDLE and default) */
    {
      Sbcc_DmConfigure(FALSE);
      temp &= (uint32)(~(uint32)Sbcc_CR1_HEN); /* H-bridge disable */
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
    }
    break;
  }
  
  Sbcc_DlcSt = Status;

  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR10, tempCR10);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR10);
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
}



/******************************************************************************/
/** 
* \brief      Sbcc_FWL_DownAction
*             Front window down action function
* \author     H.Huisheng
* \param      
              RegIndex: Control register index to be configured
              Status:   Action type
* \retval     N/A 
* \note       N/A
*/
/******************************************************************************/
#if 0
static void Sbcc_FWL_DownAction(uint8 RegInedx, boolean Status)
{
  uint32 temp;

  temp = SBCC_GET_OUTPUT_TABLE(RegInedx);
  if(Status)
  {
    temp |= Sbcc_CR1_HEN; /* H-bridge enabled */
    PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
    /*PPFC_WriteChannel(PPFC_FWL_PWM, TRUE);*/
    Sbcc_LastFLWin_Action = SBCC_OUT_CHANNEL_FWL_DOWN;
  }
  else
  {
    if(SBCC_OUT_CHANNEL_FWL_DOWN == Sbcc_LastFLWin_Action)
    {
      temp &= (uint32)(~(uint32)Sbcc_CR1_HEN); /* H-bridge disable */
      PPFC_WriteChannel(PPFC_PR_DIRH, PPFC_LOW);
      /*PPFC_WriteChannel(PPFC_FWL_PWM, FALSE);*/
      Sbcc_LastFLWin_Action = SBCC_OUT_CHANNEL_INVALID;
    }
  }
  SBCC_SET_OUTPUT_TABLE(RegInedx, temp);
}
#endif

/******************************************************************************/
/** 
* \brief      Sbcc_OutputExec
              DCU different driver output execution
* 
* \author     H.Huisheng
* \param      
              Channel:  Channel nunber to be operated
              Hs_Ls:    driver type
              status:   action type
* \retval     
              TRUE:  successfully executed
              FALSE: not executed
* \note       N/A
*/
/******************************************************************************/
static boolean Sbcc_OutputExec(uint8 Channel, SBCC_OutputState_t HS_LS,boolean Status)
{
  boolean RetVal = FALSE;
  uint32 HS_Bit; 
  uint32 LS_Bit;
  uint32 BitMask;
  uint8 RegInedx;

  if(Channel < SBCC_NUM_OF_PHY_CH)
  {
    HS_Bit = Sbcc_OutputBitCfg[Channel].HS_Bit;
    LS_Bit = Sbcc_OutputBitCfg[Channel].LS_Bit;
    BitMask = Sbcc_OutputBitCfg[Channel].BitMask;
    RegInedx = Sbcc_OutputBitCfg[Channel].RegInedx;
     
    switch(Channel)
    {
      /* Output channel 1-6 are half bridges(HS or LS) outputs */
      case SBCC_OUT_CHANNEL1:
      case SBCC_OUT_CHANNEL2:
      case SBCC_OUT_CHANNEL3:
      case SBCC_OUT_CHANNEL4:
      case SBCC_OUT_CHANNEL5:
      case SBCC_OUT_CHANNEL6:
        RetVal = Sbcc_OutputHSOrLS(HS_Bit, LS_Bit, HS_LS);
        break;

      /* Output channel 7-15 , HS and GH are stand alone high-side outputs */
      case SBCC_OUT_CHANNEL7:
      case SBCC_OUT_CHANNEL8:
      case SBCC_OUT_CHANNEL9:
      case SBCC_OUT_CHANNEL10:
      case SBCC_OUT_CHANNEL11:
      case SBCC_OUT_CHANNEL12:
      case SBCC_OUT_CHANNEL13:
      case SBCC_OUT_CHANNEL14:
      case SBCC_OUT_CHANNEL15:
      case SBCC_OUT_CHANNEL_HS:
      case SBCC_OUT_CHANNEL_GH:
        RetVal = Sbcc_OutputHS(RegInedx, HS_Bit, BitMask, HS_LS);
        break;

      case SBCC_OUT_CHANNEL_FWL_UP:
          Sbcc_DLC_Action(RegInedx, HS_LS);
          RetVal = Sbcc_CheckEnterQueue(RegInedx);
        break;

#if 0
      case SBCC_OUT_CHANNEL_FWL_DOWN:
        Sbcc_FWL_DownAction(RegInedx, Status);
        RetVal = Sbcc_CheckEnterQueue(RegInedx);
        break;
#endif

      case  SBCC_OUT_CHANNEL_EC:
        if(Status == TRUE)
        {
          SBCC_SetEcFunctionON();
        }
        else
        {
          SBCC_SetEcFunctionOFF();  
        }
        break;
      default:
      break;
    }
  }

  return RetVal;
}

/******************************************************************************/
/** 
* \brief      Sbcc_OverTempAfterShortCheck
* 
* \author     H.Huisheng
* \param     
              channel: channel nunmber to be operated
* \retval     
              TRUE:  No abnormal condition
              FALSE: Has abnormal conditon
* \note       N/A
*/
/******************************************************************************/
static boolean Sbcc_OverTempAfterShortCheck(uint8 Channel)
{
  boolean retValue;
  sint16  tempValue = SBCC_GetClusterTemperature(Sbcc_GetTempClId(Channel));

  retValue = (SBCC_GetErrorPresent(Channel) == SBCC_ERROR_OVER_CURRENT) 
          && (tempValue > Sbcc_PROTECT_TEMP);

  return retValue;
}


/******************************************************************************/
/** 
* \brief      Sbcc_OperationConditonCheck
* 
* \author     H.Huisheng
* \param     
              Channel: channel nunmber to be operated
              Status:  action value
* \retval     
              TRUE:  No abnormal condition
              FALSE: Abnormal condition
* \note       N/A
*/
/******************************************************************************/
static boolean Sbcc_OperationConditonCheck(uint8 Channel, boolean Status)
{
  boolean retValue = TRUE;
  boolean overTempAfterShort = Sbcc_OverTempAfterShortCheck(Channel);

  if (((Channel >= SBCC_OUT_CHANNEL7) && (Sbcc_Channels[Channel].OutType == SBCC_OUTPUT_LS))
   || (Channel == SBCC_OUT_CHANNEL_INVALID)
   || (Status && (overTempAfterShort || (Sbcc_DiagInfo[Channel].ProtectTimer != 0U))))
   {
     retValue = FALSE;
   }
   
  return retValue;
}

/******************************************************************************/
/** 
* \brief      Sbcc_SetWDFailFlag
* 
* \author     Weipeng.LAI
* \param     
\
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_SetWDFailFlag(boolean Status)
{
	Sbcc_WDFailFlag = (boolean)Status;
}

static void Sbcc_WatchdogRefresh(boolean * Value)
{
#if SBCC_WD_ENABLE
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1);

  if((*Value) == TRUE)
  {
    temp &= ~((uint32)Sbcc_CR1_TRIG);
    (*Value) = FALSE;
  }
  else
  {
    temp |= (Sbcc_CR1_TRIG);
    (*Value) = TRUE;
  }

  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CR1, temp);

  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CR1);
#endif
}
/******************************************************************************/
/**
* \brief      Sbcc_DmConfigure \n
*             Configure the DM function
* \author     H.Huisheng
* \param      value(boolean) :
* \retval     N/A
* \note       N/A
*/
/******************************************************************************/
static void Sbcc_DmConfigure(boolean value)
{
  uint32 temp = SBCC_GET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG);

  if(value == FALSE)
  {
    temp &= ~Sbcc_CFGR_DM;
  }
  else
  {
    temp |= Sbcc_CFGR_DM;
  }
  temp = Sbcc_SyncTrigBitWithWD(temp);/*keep the TRIG bit the same*/
  SBCC_SET_OUTPUT_TABLE(Sbcc_FRAME_WRITE_CFG, temp);
  (void)Sbcc_CheckEnterQueue(Sbcc_FRAME_WRITE_CFG);
}
/* _____E N D _____ (sbcc.c) _________________________________________________*/
