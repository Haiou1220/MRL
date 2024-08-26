/******************************************************************************/
/**
* \file       sbcc_dynamic.c
* \brief
* \details
*
* \author     H.Huisheng
* \date       1/14/2020
* \par        History:
*
\verbatim
  Version     Author                    Date            Desc
  1.0         H.Huisheng                1/14/2020
\endverbatim
*
*/
/**************** (C) Copyright 2018 Magneti Marelli Guangzhou ****************/

/* _____ I N C L U D E - F I L E S ___________________________________________*/
#include "sbcc_dynamic.h"
 
/* _____ L O C A L - D E F I N E _____________________________________________*/

/* _____ L O C A L - T Y P E S _______________________________________________*/

/* _____ G L O B A L - D A T A _______________________________________________*/

/* _____ L O C A L - D A T A _________________________________________________*/
Sbcc_Channel_t Sbcc_Channels[SBCC_NUM_OF_CH] =
{
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*0*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*1*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*2*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*3*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*4*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*5*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*6*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*7*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*8*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*9*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*10*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*11*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*12*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*13*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*14*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*15*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*16*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*17*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*18*/
  {SBCC_OUTPUT_HS, 0, Sbcc_CHANNEL_NO_FLAG, 0}, /*19*/
};

/* _____ L O C A L - M A C R O S _____________________________________________*/

/* _____ L O C A L - F U N C T I O N S - P R O T O T Y P E S _________________*/

/* _____ G L O B A L - F U N C T I O N S _____________________________________*/

/* _____ L O C A L - F U N C T I O N S _______________________________________*/

/* _____E N D _____ (sbcc_dynamic.c) _________________________________________*/

