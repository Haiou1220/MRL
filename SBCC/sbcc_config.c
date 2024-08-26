/******************************************************************************/
/**
* \file       sbcc_config.c
* \brief      SBC(L99DZ100G) Control Module configuration file     
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
#include "spic.h"
#include "sbcc.h"
#include "sbcc_config.h"

/* _____ L O C A L - D E F I N E _____________________________________________*/

/* _____ L O C A L - T Y P E S _______________________________________________*/

/* _____ G L O B A L - D A T A _______________________________________________*/
const uint32 Sbcc_CM_CH_FlagMasks[SBCC_NUM_OF_PHY_CH] =
{
  0x00000000,  /* SBCC_OUT_CHANNEL1  */
  0x00000001,  /* SBCC_OUT_CHANNEL2  */
  0x00000002,  /* SBCC_OUT_CHANNEL3  */
  0x00000003,  /* SBCC_OUT_CHANNEL4  */
  0x00000004,  /* SBCC_OUT_CHANNEL5  */
  0x00000005,  /* SBCC_OUT_CHANNEL6  */
  0x00000006,  /* SBCC_OUT_CHANNEL7  */
  0x00000007,  /* SBCC_OUT_CHANNEL8  */
  0x00000008,  /* SBCC_OUT_CHANNEL9  */
  0x00000009,  /* SBCC_OUT_CHANNEL10  */
  0x0000000A,  /* SBCC_OUT_CHANNEL11  */
  0x0000000B,  /* SBCC_OUT_CHANNEL12  */
  0x0000000C,  /* SBCC_OUT_CHANNEL13  */
  0x0000000D,  /* SBCC_OUT_CHANNEL14  */
  0x0000000E,  /* SBCC_OUT_CHANNEL15  */
  0x0000000F,  /* SBCC_OUT_CHANNEL_HS  */
  0x0000000F,  /* Empty  */
  0x0000000F,  /* Empty  */
  0x0000000F,  /* Empty  */ 
  0x00000009,  /* SBCC_OUT_CHANNEL_EC  */
};
const Sbcc_DiagCfg_t Sbcc_DiagCfg[SBCC_NUM_OF_PHY_CH] = 
{
  /* OlMatureTime OcMatureTime DematureTime ProtectTime TempClId  OcrMask OcrAlertMask  OlMask  OcMask*/
  {100, 100, 100, 2000, SBCC_TEMP_CL3,  0x00800000, 0x00C00000, 0x00C00000, 0x00C00000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL1 --  SBCC_MIRROR_COMMON  */
  {200, 200, 200, 2000, SBCC_TEMP_CL3,  0x00400000, 0x00300000, 0x00300000, 0x00300000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL2 --  SBCC_MIRROR_LEFT  */
  {200, 200, 200, 2000, SBCC_TEMP_CL1,  0x00200000, 0x000C0000, 0x000C0000, 0x000C0000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL3 --  SBCC_MIRROR_UP  */
  { 80,  80,  80, 2000, SBCC_TEMP_CL2,  0x00100000, 0x00030000, 0x00030000, 0x00030000, SBCC_OUT_CHANNEL5}, /*  SBCC_OUT_CHANNEL4 --  SBCC_FRONT_DOOR_LOCK  */
  { 80,  80,  80, 2000, SBCC_TEMP_CL2,  0x00080000, 0x0000C000, 0x0000C000, 0x0000C000, SBCC_OUT_CHANNEL4}, /*  SBCC_OUT_CHANNEL5 --  SBCC_FRONT_DOOR_UNLOCK  */
  {200, 200, 200, 2000, SBCC_TEMP_CL3,  0x00040000, 0x00003000, 0x00003000, 0x00003000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL6 --  SBCC_MIRROR_UNFOLD  */
  {50, 50, 50, 2000, SBCC_TEMP_CL4,  0x00020000, 0x00000800, 0x00000800, 0x00000800, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL7 --  SBCC_DOOR_LIGHT */
  {150, 150, 150, 2000, SBCC_TEMP_CL1,  0x00010000, 0x00000400, 0x00000400, 0x00000400, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL8 --  SBCC_EXT_HANDLE_LIGHT  */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000200, 0x00000200, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL9 --  SBCC_LOGO_LED */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000100, 0x00000100, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL10  --  SBCC_CH10 */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000080, 0x00000080, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL11  --  SBCC_RESERVED_LED */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000040, 0x00000040, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL12  --  SBCC_WL_INH_LED */
  { 80,  80,  80, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000020, 0x00000020, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL13  --  SBCC_BLIND_DETECT_LED */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000010, 0x00000010, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL14  --  SBCC_INT_HANDLE_LIGHT  */
  {200, 200, 200, 2000, SBCC_TEMP_CL1,  0x00000000, 0x00000000, 0x00000008, 0x00000008, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL15  --  SBCC_WL_HALL_SENS_PWR  */
  {200, 200, 200, 2000, SBCC_TEMP_CL2,  0x00008000, 0x00000004, 0x00000004, 0x00000004, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL_HS --  SBCC_HS */
  {100, 100, 100, 2000, SBCC_TEMP_CL_INVALID,  0x00000000, 0x00000000, 0x00000002, 0x00000002, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL_GH --  SBCC_MIRROR_HEAT  */
  {100, 100, 100, 2000, SBCC_TEMP_CL_INVALID,  0x00000000, 0x00000000, 0x00000000, 0x00005000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL_FWL_UP --  SBCC_FWL_UP */
  {100, 100, 100, 2000, SBCC_TEMP_CL_INVALID,  0x00000000, 0x00000000, 0x00000000, 0x0000A000, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL_DOWN --  SBCC_FWL_DOWN */
  {200, 200, 200, 2000, SBCC_TEMP_CL4,  0x00000000, 0x00000000, 0x00000100, 0x00000100, SBCC_NUM_OF_CH}, /*  SBCC_OUT_CHANNEL_EC  --  SBCC_MIRROR_EC */
};

/* _____ L O C A L - D A T A _________________________________________________*/
#if 0
/** breif  channel configuration */
const lpspi_master_config_t Sbcc_SpiChannelCfg = {
  .bitsPerSec =             1000000U, /*1M Hz*/
  .whichPcs =               LPSPI_PCS3,/*refer to hardware design(PTB17-LPSPI1_PCS3)*/
  .pcsPolarity =            LPSPI_ACTIVE_LOW,
  .isPcsContinuous =        FALSE,
  .bitcount =               32U,
  .lpspiSrcClk =            24000000U,/*related with clock configuration, now is 48 Mhz*/
  .clkPhase =               LPSPI_CLOCK_PHASE_1ST_EDGE,
  .clkPolarity =            LPSPI_SCK_ACTIVE_HIGH,
  .lsbFirst =               FALSE,
  .transferType =           LPSPI_USING_INTERRUPTS,
  .rxDMAChannel =           0xffU,
  .txDMAChannel =           0xffU,
  .callback =               SBCC_TxCompleteCallback,
  .callbackParam =          NULL
};

lpspi_state_t Sbcc_SpiChannelState;

/** \brief CSIH channel  configuration*/
const SPIC_CfgType_t Sbcc_Spi_Cfg =
{
   &Sbcc_SpiChannelCfg,
   &Sbcc_SpiChannelState
};
#endif

/* _____ L O C A L - M A C R O S _____________________________________________*/

/* _____ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/

/* _____ G L O B A L - F U N C T I O N S _____________________________________*/

/* _____ L O C A L - F U N C T I O N S _______________________________________*/

/* _____E N D _____ (sbcc_config.c) __________________________________________*/

