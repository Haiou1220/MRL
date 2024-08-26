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
#include "asm330lhh_io.h"
#include "asm330lhh_cfg.h"
#include "Spi.h"
#include "dio.h"
/*=======[M A C R O S]========================================================*/


/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[I N T E R N A L   D A T A]==========================================*/


/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/

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
boolean asm330lhh_WriteCtrlReg(uint8 reg, uint8 regData)
{
    boolean ret = FALSE;
    uint8 readBackData;

    if (asm330lhh_IO_SpiWrite(reg, regData) == TRUE) {
        asm330lhh_IO_SpiRead(reg, &readBackData);
        if (readBackData == regData)
            ret = TRUE; /* compare read data to check if write right */	
        else
            ret = FALSE;
    }
    return ret;
}

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
boolean asm330lhh_ReadStatReg(uint16 regData, uint8 *readData)
{
    return asm330lhh_IO_SpiRead(regData, readData);
}

/******************************************************************************/
/*
 * Brief               <asm330lhh spi write sequence>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-regData[in]   <register write data include address & data> 
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <N/A>
 */
/******************************************************************************/
uint8 writeDataBuffer2[2];
uint8 readDataBuffer2[2];
boolean asm330lhh_IO_SpiWrite(uint8 reg, uint8 regData)
{
    uint32 Count = 0xFFFFFF;
    uint32 SeqStatus;
    writeDataBuffer2[0] = regData;
    writeDataBuffer2[1] = reg;

    Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_1_IMU_ASM330L, writeDataBuffer2, readDataBuffer2, 2u);
    Spi_SyncTransmit(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);

    do
    {
        SeqStatus = Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);
        Count--;
    }
    while ((Count > 0u) && (SeqStatus == SPI_SEQ_PENDING));
    if (Count ==0)
        return FALSE;
    else
        return TRUE;
}

/******************************************************************************/
/*
 * Brief               <asm330lhh spi read sequence>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-regData[in]   <register read address> 
 * Param-Name[out]     <N/A>
 * Param-Name[in/out]  <N/A>
 * Return              <read back register data> 
 */
/******************************************************************************/
uint8 writeDataBuffer[2] = {0};
uint8 readDataBuffer[2] = {0};
boolean asm330lhh_IO_SpiRead(uint8 regData, uint8 *readbackData)
{
    uint8 writeData = (0x80 | regData);
    uint32 Count = 0xFFFFFF;
    Spi_SeqResultType SeqStatus;

    writeDataBuffer[0] = 0;
    writeDataBuffer[1] = writeData;
    SeqStatus = Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);
    if (SeqStatus != SPI_SEQ_OK)
        return FALSE;
    Std_ReturnType ret = Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_1_IMU_ASM330L, writeDataBuffer, readDataBuffer, 2u);
    if (E_OK == ret) {
        ret = Spi_SyncTransmit(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);
        if (E_OK == ret) {
            do
            {
                SeqStatus = Spi_GetSequenceResult(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);
                Count--;
            } while ((Count > 0u) && (SeqStatus == SPI_SEQ_PENDING));
            if (Count != 0) {
                *readbackData = readDataBuffer[0];
                return TRUE;
            }
        }
    }
    return FALSE;
}



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
uint8 writeData[10] = {0};
void asm330lhh_IO_AsyncSpiMultipleRead(uint16 regData,uint16 bytes, uint8 *readdata)
{
    uint8 writeData0 = ((ASM330LHH_REG_READ_BIT | regData) >> 8u);
    writeData[1] = regData & 0xff;
    writeData[0] = writeData0;

    Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_1_IMU_ASM330L, writeData, readdata, bytes);
    Spi_AsyncTransmit(SpiConf_SpiSequence_SpiSequence_1_IMU_ASM330L);

    return;
}

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
boolean asm330lhh_IO_GetINT1_Pin(void)
{
    boolean ret = FALSE;
    ret = Dio_ReadChannel(IMU_MCU_INT1_DioChannel);
    return ret;
}

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
boolean asm330lhh_IO_GetINT2_Pin(void)
{
    boolean ret = FALSE;
    ret = Dio_ReadChannel(IMU_MCU_INT2_DioChannel);
    return ret;
}


