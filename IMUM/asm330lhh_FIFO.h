
/******************************************************************************/
/**
* \file       asm330lhh_fifo.h
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

#ifndef ASM330LHH_FIFO_H
#define ASM330LHH_FIFO_H

/*=======[I N C L U D E S]====================================================*/
#include "asm330lhh_types.h"
#include "Spi.h"
/*=======[M A C R O S]========================================================*/

/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/


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
void asm330lhh_FIFO_TaskInit(void);

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
void asm330lhh_FIFO_Task(uint16 FIFO_DIFF);

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
boolean asm330lhh_FIFO_Get_Words(ASM330LHH_FIFO_DATA_OUT_TYPE *dest, uint16 words);

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
uint16 asm330lhh_FIFO_Get_UnreadWordsNumber(void);


#endif /* ASM330LHH_FIFO_H */

/*=======[E N D   O F   F I L E]==============================================*/

