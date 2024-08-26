
/******************************************************************************/
/**
* \file       asm330lhh_io.h
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

#ifndef ASM330LHH_IO_H
#define ASM330LHH_IO_H

/*=======[I N C L U D E S]====================================================*/
#include "asm330lhh_types.h"

/*=======[M A C R O S]========================================================*/

/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/

/******************************************************************************/
/*
 * Brief               <asm330lhh update ctrl register>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-regData[in]   <ctrl register data>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <TRUE: write success; FALSE: write failure> 
 */
/******************************************************************************/
boolean asm330lhh_WriteCtrlReg(uint8 reg, uint8 regData);
boolean asm330lhh_IO_SpiWrite(uint8 reg, uint8 regData);
/******************************************************************************/
/*
 * Brief               <asm330lhh update stat register>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-regData[in]   <state register address>
 * Param-readData[out]     <state register data>
 * Param-Name[in/out]  <N/A>
 * Return              <TRUE: read success; FALSE: read failure> 
 */
/******************************************************************************/
boolean asm330lhh_ReadStatReg(uint16 regData, uint8 *readData);

/******************************************************************************/
/*
 * Brief               <asm330lhh async spi multiple read sequence>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-regData[in]   <register read address> 
 * Param-bytes[in]     <read register bytes>
 * Param-readData[out]   <state register data>
 * Return              <read back register data> 
 */
/******************************************************************************/
void asm330lhh_IO_AsyncSpiMultipleRead(uint16 regData,uint16 bytes, uint8 *readdata);
boolean asm330lhh_IO_SpiRead(uint8 regData, uint8 *readbackData);
/******************************************************************************/
/*
 * Brief               <asm330lhh get INT1 pin>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <N/A>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <TRUE: INT1 pin high level; FALSE: INT1 pin low level;> 
 */
/******************************************************************************/
boolean asm330lhh_IO_GetINT1_Pin(void);

/******************************************************************************/
/*
 * Brief               <asm330lhh get INT2 pin>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <N/A>
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <TRUE: INT2 pin high level; FALSE: INT2 pin low level;>
 */
/******************************************************************************/
boolean asm330lhh_IO_GetINT2_Pin(void);

#endif /* asm330lhh_IO_H */

/*=======[E N D   O F   F I L E]==============================================*/
