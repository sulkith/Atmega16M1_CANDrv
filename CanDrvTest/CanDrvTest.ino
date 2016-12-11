#include <avr/io.h>
#include "CANDrv.h"
#define MIN(x,y)((x<y)?x:y)


uint8_t ledstate = 1;

void testReceiveMessage();

uint8_t data_130[8];
uint8_t data_7e8[8];
uint8_t data_7e0[8];
void receiveData_generic(int mob_NR);
MobConfigElement CAN_Config[] =
{
  {TX_DATA_SW_DRIVEN,{0x7e0,0x000,0,8,(uint8_t*)&data_7e0}},
  {RX_DATA_INTERRUPT_DRIVEN,{0x130,0x3FF,0,8,(uint8_t*)&data_130},&receiveData_generic_restart_toggleLED,0,0},
  {RX_DATA_INTERRUPT_DRIVEN,{0x7e8,0x3FF,0,8,(uint8_t*)&data_7e8},&receiveData_generic_restart,0,0},
  {UNUSED},
  {UNUSED},
  {UNUSED}
};
void receiveData_generic_restart_toggleLED(uint8_t mob_NR)
{
  receiveData_generic_restart(mob_NR);
  ledstate++;
  digitalWrite(14, (ledstate%2)>0);
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
  CANDrv_Init(CAN_500k); //init CAN
  CANDrv_FRMMan_Init(CAN_Config);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
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
  CANDrv_FRMMan_Send_Msg(0);
  mob_settings ms;
  uint8_t buffer[8];
  ms.data = (uint8_t*)&buffer;
  CANDrv_FRMMan_Get_Msg(2,&ms);
  
}
