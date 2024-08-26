
/******************************************************************************/
/**
* \file       imum.h
* \brief      
* \details    
*             
* \author     Hongming.ZHENGH
* \date       09/05/2023
* \par        History:
* 
\verbatim
  Version     Author                    Date            Desc   
  1.0         Hongming.ZHENGH           09/05/2023
\endverbatim  
*
*/
/**************** (C) Copyright 2023 Magneti Marelli Guangzhou ****************/

/*=======[I N C L U D E S]====================================================*/
#include "asm330lhh_fifo.h"
#include "asm330lhh_io.h"
/*=======[M A C R O S]========================================================*/
#ifndef STATIC
#define STATIC static
#endif

#define ASM330LHH_FIFO_DataBufferWords 520u

#define RemainingBuffer(idx) (ASM330LHH_FIFO_DataBufferWords - 1u - idx)
/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/*=======[E X T E R N A L   D A T A]==========================================*/
ASM330LHH_OUT_TEMP_TYPE ASM330LHH_OUT_TEMP_DATA;
ASM330LHH_OUTDATA_G_TYPE ASM330LHH_OUTDATA_G;
ASM330LHH_OUTDATA_A_TYPE ASM330LHH_OUTDATA_A;
ASM330LHH_TIMESTAMP_TYPE ASM330LHH_TIMESTAMP_DATA;

/*=======[I N T E R N A L   D A T A]==========================================*/
STATIC ASM330LHH_FIFO_DATA_OUT_TYPE ASM330LHH_FIFO_DATA_OUT[ASM330LHH_FIFO_DataBufferWords];
STATIC ASM330LHH_FIFO_DATA_OUT_TYPE ASM330LHH_RtFIFO_DATA_OUT[ASM330LHH_FIFO_DataBufferWords];
STATIC ASM330LHH_FIFO_DATA_OUT_TYPE *FIFO_Read_Words_Ptr;
STATIC uint16 FIFO_ReadWordsNumber;
STATIC uint16 FIFO_NewDataIndex;
STATIC uint16 FIFO_ReadDataIndex;
STATIC boolean BufferNeedSync;

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC void asm330lhh_FIFO_Copy_Words(ASM330LHH_FIFO_DATA_OUT_TYPE *dest, ASM330LHH_FIFO_DATA_OUT_TYPE *src, uint16 words);
STATIC void asm330lhh_FIFO_Copy_Words_to_Buffer(void);

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void asm330lhh_FIFO_CopyWordsEnd_Cbk(void);

/******************************************************************************/
/*
 * Brief               <asm330lhh FIFO Task memory initialize>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <N/A>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
void asm330lhh_FIFO_TaskInit(void)
{
    uint16 idx;
    BufferNeedSync = FALSE;
    FIFO_NewDataIndex = 0u;
    FIFO_ReadDataIndex = ASM330LHH_FIFO_DataBufferWords - 1u;
    FIFO_Read_Words_Ptr = NULL_PTR;
    FIFO_ReadWordsNumber = 0u;

    for(idx = 0u;idx < ASM330LHH_FIFO_DataBufferWords;idx++)
    {
        ASM330LHH_FIFO_DATA_OUT[idx] = (ASM330LHH_FIFO_DATA_OUT_TYPE){0xFF,0xFFFF,0xFFFF,0xFFFF};
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh FIFO task main process>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-FIFO_DIFF[in] <Number of unread sensor data (TAG + 6 bytes) stored in FIFO>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
void asm330lhh_FIFO_Task(uint16 FIFO_DIFF)
{

    if(SPI_SEQ_PENDING != Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L)) //spi sequence IDLE
    {

        if(TRUE == BufferNeedSync)
        {
            asm330lhh_FIFO_Copy_Words(ASM330LHH_FIFO_DATA_OUT,ASM330LHH_RtFIFO_DATA_OUT,(uint16)ASM330LHH_FIFO_DataBufferWords);
            BufferNeedSync = FALSE;
            if(NULL_PTR != FIFO_Read_Words_Ptr)
            {
                asm330lhh_FIFO_Copy_Words_to_Buffer();
            }
        }

        if(FIFO_DIFF > 0u)
        {
            if(RemainingBuffer(FIFO_NewDataIndex) >= FIFO_DIFF)
            {
                asm330lhh_IO_AsyncSpiMultipleRead(ASM330LHH_FIFO_DATA_OUT_TAG_ADDR,FIFO_DIFF * 7u,(uint8 *)&ASM330LHH_RtFIFO_DATA_OUT[FIFO_NewDataIndex+1]);
                BufferNeedSync = TRUE;
                if((FIFO_ReadDataIndex >= FIFO_NewDataIndex) & (FIFO_ReadDataIndex <= (FIFO_NewDataIndex + FIFO_DIFF)))
                {
                    FIFO_ReadDataIndex = FIFO_NewDataIndex + FIFO_DIFF + 1u;
                }
                FIFO_NewDataIndex += FIFO_DIFF;
            }
            else if(0u ==RemainingBuffer(FIFO_NewDataIndex))
            {
                asm330lhh_IO_AsyncSpiMultipleRead(ASM330LHH_FIFO_DATA_OUT_TAG_ADDR,FIFO_DIFF * 7u,(uint8 *)&ASM330LHH_RtFIFO_DATA_OUT[0]);
                BufferNeedSync = TRUE;
                if(FIFO_ReadDataIndex <= (FIFO_DIFF - 1u))
                {
                    FIFO_ReadDataIndex = FIFO_DIFF;
                }
                FIFO_NewDataIndex = FIFO_DIFF - 1u;
            }
            else if(RemainingBuffer(FIFO_NewDataIndex) <= FIFO_DIFF)
            {
                asm330lhh_IO_AsyncSpiMultipleRead(ASM330LHH_FIFO_DATA_OUT_TAG_ADDR,RemainingBuffer(FIFO_NewDataIndex) * 7u,
                                                                            (uint8 *)&ASM330LHH_RtFIFO_DATA_OUT[FIFO_NewDataIndex+1]);
                BufferNeedSync = TRUE;
                if(FIFO_ReadDataIndex >= FIFO_NewDataIndex)
                {
                    FIFO_ReadDataIndex = 0u;
                }
                FIFO_NewDataIndex = ASM330LHH_FIFO_DataBufferWords - 1u;
            }
        }
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh get FIFO data>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-dest[in]      <buffer address to receive FIFO data>
 * Param-words[in]     <number of words to receive FIFO data >
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
boolean asm330lhh_FIFO_Get_Words(ASM330LHH_FIFO_DATA_OUT_TYPE *dest, uint16 words)
{
    boolean ret = FALSE;

    if(words <= asm330lhh_FIFO_Get_UnreadWordsNumber())
    {
        FIFO_Read_Words_Ptr = dest;
        FIFO_ReadWordsNumber = words;
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh get unread FIFO data words number>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <N/A>
 * Param-Name[in]      <N/A>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <unread FIFO data words number in software FIFO>
 */
/******************************************************************************/
uint16 asm330lhh_FIFO_Get_UnreadWordsNumber(void)
{
    uint16 ret = 0u;

    if(FIFO_NewDataIndex < FIFO_ReadDataIndex)
    {
        ret = ASM330LHH_FIFO_DataBufferWords - FIFO_ReadDataIndex + FIFO_NewDataIndex - 1u;
    }
    else
    {
        ret = FIFO_NewDataIndex - FIFO_ReadDataIndex - 1u;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh copy FIFO buffer data>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-dest[in]      <buffer destination to receive date>
 * Param-src[in]       <buffer source address>
 * Param-words[in]     <number of words to copy>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
STATIC void asm330lhh_FIFO_Copy_Words(ASM330LHH_FIFO_DATA_OUT_TYPE *dest, ASM330LHH_FIFO_DATA_OUT_TYPE *src, uint16 words)
{
    uint16 idx;

    if(words <= ASM330LHH_FIFO_DataBufferWords)
    {
        for(idx = 0u;idx < words;idx++)
        {
            dest[idx] = src[idx];
        }
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh copy FIFO buffer data>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <N/A>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
STATIC void asm330lhh_FIFO_Copy_Words_to_Buffer(void)
{
    uint16 idx;

    for(idx = 0u;idx < FIFO_ReadWordsNumber;idx++)
    {
        FIFO_Read_Words_Ptr[idx] = ASM330LHH_FIFO_DATA_OUT[FIFO_ReadDataIndex];
        FIFO_ReadDataIndex++;
        if(FIFO_ReadDataIndex >= ASM330LHH_FIFO_DataBufferWords)
        {
            FIFO_ReadDataIndex = 0u;
        }
    }
    FIFO_Read_Words_Ptr = NULL_PTR;
    asm330lhh_FIFO_CopyWordsEnd_Cbk();

    return;
}

