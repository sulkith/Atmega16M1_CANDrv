/** \file CANDrv.cpp
    \brief CANDrv cpp file

    This File contains the Implementation of the CANDrv and the FRMMan
*/

#include <avr/io.h>
#include "CANDrv.h"
#include "Arduino.h"

#if defined(__AVR_ATmega16M1__)
#define NB_MOB  6
#endif

#if F_CPU == 8000000L
#define CANBT1_500  0x02       // Tscl  = 2x Tclkio = 250 ns
#define CANBT2_500  0x04       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#define CANBT3_500  0x13       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#define CANBT1_800  0x00       // Tscl  = 2x Tclkio = 250 ns
#define CANBT2_800  0x0a       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#define CANBT3_800  0x10       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#endif

CANPAGEHandler::CANPAGEHandler(uint8_t mob_nr)
{
  canpage_tmp = CANPAGE;
  if (mob_nr > NB_MOB)
  {
    CANPAGE = 0x00;
    return;
  }
  CANPAGE = (mob_nr << 4); //set MOBNB and Auto increment
}

CANPAGEHandler::~CANPAGEHandler()
{
  CANPAGE = canpage_tmp;
}
//};
MobConfigElement *__internal_CAN_Config;

/**\fn uint8_t CANDrv_Set_bt(CanBaudrate baudrate)
 * \brief set the Timing Parameters for the CAN.
 * 
 * the Values of the Registers CANBT1-3 are also defined in this file.<br><br>
 * !ONLY USED INTERNALLY!
 * \param baudrate the baudrate to set.
 * \return 1 on success. 0 in case of an error.
 */
inline uint8_t CANDrv_Set_bt(CanBaudrate baudrate)
{
  switch (baudrate)
  {
    case CAN_500k:
      CANBT1 = CANBT1_500;
      CANBT2 = CANBT2_500;
      CANBT3 = CANBT3_500;
      return 1;
    case CAN_800k:
      CANBT1 = CANBT1_800;
      CANBT2 = CANBT2_800;
      CANBT3 = CANBT3_800;
      return 1;
    default:
      return 0;
  }
}
/**\fn void CLEAR_RXOK()
 * \brief Clears the interrupt FLAG.
 */
inline void CLEAR_RXOK()
{
  CANSTMOB &= ~(1 << RXOK); // clear interrupt flag
}
void CANDrv_ClearAll_MOB(void)
{
  uint8_t  MOB_Nr;
  for (uint8_t  MOB_Nr = 0; MOB_Nr < NB_MOB; MOB_Nr++)
  {
    CANPAGE = (MOB_Nr << 4);    //Page index
    for (volatile uint8_t *canstmob_ptr = &CANSTMOB; canstmob_ptr < &CANSTML; ++canstmob_ptr)
    {
      *canstmob_ptr = 0;//Clean all Can Registers from CANSTMOB to CANSTML
    }
    //TODO Clean FIFO
  }
}
uint8_t CANDrv_Init(CanBaudrate baudrate)
{
  CANGCON  =  (1 << SWRES); //reset CAN
  if (CANDrv_Set_bt(baudrate) == 0)return 0; //return 0 if error while initializing Bit Timing Register

  CANDrv_ClearAll_MOB();
  CANGCON |=  (1 << ENASTB); //enable CAN
  return (1);
}

void getMOBsetup(mob_settings *ms)
{
  ms->dlc = CANCDMOB & 0xF;
  ms->ide = ((CANCDMOB & 0x10) > 0) ? 1 : 0;
  if (!ms->ide)
  {
    ms->can_id = ((uint16_t)CANIDT1) << 3 | CANIDT2 >> 5;
    ms->can_msk = ((uint16_t)CANIDM1) << 3 | CANIDM2 >> 5;
  }
  /*else
  {
      //TODO
  }*/
  for (uint8_t i = 0; i < ms->dlc; ++i)
  {
    ms->data[i] = CANMSG;
  }

}
void setupMOB(mob_settings *ms)
{
  setMOB_Operation(DISABLED);
  CANCDMOB |= ((ms->dlc) & 0x0F) | ((ms->ide > 0) ? 0x10 : 0x00); //set DLC and IDE Flag
  if (!ms->ide)
  {
    CANIDT1 = ((uint8_t)((ms->can_id) >> 3));
    CANIDT2 = ((uint8_t)((ms->can_id) & 0x07)) << 5;
    CANIDT4  &= ~(1 << RTRTAG);
    CANIDM1 = ((uint8_t)((ms->can_msk) >> 3));
    CANIDM2 = ((uint8_t)((ms->can_msk) & 0x07)) << 5;
    CANIDM4 = 0x01;
  }
  /*else
  {
      //TODO
  }*/
  for (uint8_t i = 0; i < ms->dlc; ++i)
  {
    CANMSG = ms->data[i];
  }

}
void receiveData_generic(uint8_t mob_NR)
{
  CANPAGEHandler cph(mob_NR);
  for (int i = 0; i < 8; ++i)
  {
    __internal_CAN_Config[mob_NR].ms.data[i] = CANMSG;
  }
  CLEAR_RXOK();
}
void receiveData_generic_restart(uint8_t mob_NR)
{
  CANPAGEHandler cph(mob_NR);
  for (int i = 0; i < 8; ++i)
  {
    __internal_CAN_Config[mob_NR].ms.data[i] = CANMSG;
  }
  CLEAR_RXOK();
  setMOB_Operation(RX_DATA);
}
uint8_t CANDrv_FRMMan_Init(MobConfigElement *CAN_Config)
{
  CANIE1 = 0x00;
  CANIE2 = 0x00;
  uint8_t enabledInterrupts = 0;
  __internal_CAN_Config = CAN_Config;
  for (uint8_t i = 0; i < NB_MOB; ++i)
  {
    CANPAGEHandler cph(i);
    setupMOB(&(__internal_CAN_Config[i].ms));
    setMOB_Operation((mob_operation)__internal_CAN_Config[i].op); //bit fiddling. only last two bits are written.
    if ((__internal_CAN_Config[i].op & 0x40) > 0)
    {
      CANIE2 |= 1 << i; //enable interrupt
      enabledInterrupts |= __internal_CAN_Config[i].op;
    }
  }

  if (enabledInterrupts > 0)
  {
    CANGIE = 0x80 | (enabledInterrupts & 0x30); //enable necessary Interrupts. some bit fiddling involved

  }

  return 0;
}
uint8_t CANDrv_FRMMan_Send_Msg(uint8_t index)
{
  if ((__internal_CAN_Config[index].op & 0x10) == 0) //Check if MOB is for Sending
    return 0;
  CANPAGEHandler cph(index);
  setupMOB(&(__internal_CAN_Config[index].ms));
  setMOB_Operation(TX_DATA);
  return 1;
}
/**
  \fn CANDrv_FRMMan_Get_Msg(uint8_t index, mob_settings *ms, uint8_t iteration = 0)
  \brief get a received Can Message.
  
  This function reads a received Can Message from the internal Buffer.<br>
  It also retries to read if there were any Changes, because of an Interrupt, receiving a new Message.<br>
  In this case the read operation is retried.<br>
  <br>
  !ONLY USED INTERNALLY!
  
  \param index the index of the mob to read
  \param ms OUTPUT Pointer to a message settings object, where the data is written.
  \param iteration Helper argument to limit the number of tries
  \return 1 on success. 0 in case of an error.
*/
uint8_t CANDrv_FRMMan_Get_Msg(uint8_t index, mob_settings *ms, uint8_t iteration = 0)
{
  uint32_t timestamp = __internal_CAN_Config[index].timestamp;

  ms->can_id = __internal_CAN_Config[index].ms.can_id;
  ms->ide = __internal_CAN_Config[index].ms.ide;
  ms->dlc = __internal_CAN_Config[index].ms.dlc;
  for (uint8_t i = 0; i < ms->dlc; ++i)
  {
    ms->data[i] = __internal_CAN_Config[index].ms.data[i];
  }

  if (timestamp < __internal_CAN_Config[index].timestamp)
  {
    //message changed while reading
    //  -->retry
    if (iteration > 2)return 0; //max 3 trys
    return CANDrv_FRMMan_Get_Msg(index, ms, iteration + 1);
  }
  return 1;
}
uint8_t CANDrv_FRMMan_Get_Msg(uint8_t index, mob_settings *ms)
{
  return CANDrv_FRMMan_Get_Msg(index, ms, 0);
}

mob_status CANDrv_FRMMan_Get_MSG_State(uint8_t index)
{
  CANPAGEHandler cph(index);
  uint8_t direction = (CANCDMOB &0xC0) >> 6;
  mob_status result = IDLE;
  if(direction == 0) return (mob_status)0;
  if(direction == 1) result = (mob_status)0x01;
  if((CANSTMOB & 0x1F)>0)
    return (direction == 1)?TX_ERROR:RX_ERROR;
  if((CANSTMOB & 0x60)>0)
    return (direction == 1)?TX_OK:RX_OK;
  return (direction == 1)?TX_PENDING:RX_PENDING;
  
  
}

ISR(CAN_INT_vect)
{
  for (uint8_t i = 0; i < NB_MOB; ++i)
  {
    if (CANSIT2 & (0x1 << i))
    {
      __internal_CAN_Config[i].timestamp = millis();
      (__internal_CAN_Config[i].f)(i);
    }
  }
}
