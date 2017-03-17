#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include "CANDrv.h"
#include "MessageValidator.h"



#define MIN(x,y)((x<y)?x:y)

uint8_t T15_st;
uint8_t SCState;
MessageValidator<uint8_t> CAN_Active(1000,0,0);
MessageValidator<uint8_t> T15_validator(1000,0,&T15_st);
MessageValidator<uint8_t> KeepRunning(10000,0,0);
MessageValidator<uint8_t> StandbyOn_validator(1000,0,0);
MessageValidator<uint8_t> KeyInLock_validator(1000,0,0);
MessageValidator<int16_t> tmot_validator(1000,-100,0);
#include "Display.h"
#include "ShutdownController.h"

uint8_t data_130[8];
uint8_t data_7e8[8];
uint8_t data_7e0[8];
void receiveData_generic(int mob_NR);
MobConfigElement CAN_Config[] =
{
  {TX_DATA_SW_DRIVEN,{0x7e0,0x000,0,8,(uint8_t*)&data_7e0}},
  {RX_DATA_INTERRUPT_DRIVEN,{0x130,0x3FF,0,8,(uint8_t*)&data_130},&receiveData_restart_130,0,0},
  {RX_DATA_INTERRUPT_DRIVEN,{0x7e8,0x3FF,0,8,(uint8_t*)&data_7e8},&receiveData_restart_7e8,0,0},
  {UNUSED},
  {UNUSED},
  {UNUSED}
};
void receiveData_restart_130(uint8_t mob_NR)
{
  receiveData_generic_restart(mob_NR);
  T15_validator.set((data_130[0]&0x04)>>2);
  StandbyOn_validator.set((data_130[0] & 0x01));
  KeyInLock_validator.set((data_130[1] & 0x40) >> 6);
  CAN_Active.set(1);
}
void receiveData_restart_7e8(uint8_t mob_NR)
{
  receiveData_generic_restart(mob_NR);
  if (data_7e8[1] == 0x41) //show current data
  {
    if (data_7e8[2] == 0x05) //tmot
    {
      tmot_validator.set(data_7e8[3] - 40);
    }
  }
  CAN_Active.set(1);
}
ISR(INT3_vect)
{
}
void setup() {
  
  data_7e0[0] = 0x02; //additional Data Bytes
  data_7e0[1] = 0x01; //Show Current Data
  data_7e0[2] = 0x05; //Engine Coolant Temperature
  data_7e0[3] = 0x33;
  data_7e0[4] = 0x44;
  data_7e0[5] = 0x55;
  data_7e0[6] = 0x66;
  data_7e0[7] = 0x77;
  Display_Init();
  CANDrv_Init(CAN_500k); //init CAN
  CANDrv_FRMMan_Init(CAN_Config);
  delay(100);
  ShutdownController_Init();
  sei();
  
}

void debugCanStatus(uint16_t id = 0)
{
  /*Serial.print("CANSTATUS ");
  Serial.println(id);
  Serial.print("CANSTMOB ");
  Serial.println(CANSTMOB, 16);
  Serial.print("CANCDMOB ");
  Serial.println(CANCDMOB, 16);
  Serial.print("CANTEC ");
  Serial.println(CANTEC, 16);
  Serial.print("CANBT ");
  Serial.print(CANBT1, 16);
  Serial.print(" ");
  Serial.print(CANBT2, 16);
  Serial.print(" ");
  Serial.println(CANBT3, 16);
  Serial.print("CANEN2 ");
  Serial.println(CANEN2);
  Serial.print("CANGSTA ");
  Serial.println(CANGSTA);*/
}

void loop() {
  delay(500);
  if(T15_validator.get())CANDrv_FRMMan_Send_Msg(0);
  Display_cyclic();
  ShutdownController_cyclic();
  
}
