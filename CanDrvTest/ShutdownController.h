#include "pinning.h"

#define ENABLE_SHUTDOWN true
#define ENABLE_POWERSAVE true

#define SHUTDOWN_STATE_BOOT_UP 0
#define SHUTDOWN_STATE_NORMAL_OPERATION 1
#define SHUTDOWN_STATE_T15_OFF 2
void ShutdownController_Init()
{
  //pinMode(24, INPUT);
  //KeepRunning.set(1);
  pinMode(CAN_STB, OUTPUT);
  digitalWrite(CAN_STB, LOW);
  pinMode(SHUTDOWN_LED, OUTPUT);
  digitalWrite(SHUTDOWN_LED, HIGH);
  SCState = 0;
}
void(* resetFunc) (void) = 0;//declare reset function at address 0
void allPinsToPowersave()
{
  for(int i = 0; i <= 26; ++i)
  {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  pinMode(SHUTDOWN_WAKEUP, INPUT);
  pinMode(DISPLAY_CS, INPUT_PULLUP);
  pinMode(DISPLAY_DC, INPUT_PULLUP);
  pinMode(DISPLAY_RST, INPUT_PULLUP);
  pinMode(DISPLAY_BL, INPUT_PULLUP);
  pinMode(DISPLAY_MOSI, INPUT_PULLUP);
  pinMode(DISPLAY_SCK, INPUT_PULLUP);
}
void shutdown()
{
  SMCR = 0x05;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  allPinsToPowersave();
  pinMode(SHUTDOWN_WAKEUP, INPUT);
  EICRA = 0x00; //Low Level Interrupt
  EIMSK = 0x08; //Enable INT3
  Display_Shutdown();
  digitalWrite(CAN_STB, HIGH);
  digitalWrite(SHUTDOWN_LED, LOW);
  PRR = 0xFF;
  sleep_cpu();
  PRR = 0x00;
  digitalWrite(SHUTDOWN_LED, HIGH);
  delay(500);
  
  EIMSK = 0x00; //Disable INT3
  resetFunc();
  //digitalWrite(14, HIGH);
  Display_Init();
  Display_cyclic();
}
void ShutdownController_cyclic()
{
  static uint32_t T15_offTime = millis();
  switch (SCState)
  {
    case SHUTDOWN_STATE_BOOT_UP:
      if (millis() >= 60000)SCState = SHUTDOWN_STATE_NORMAL_OPERATION;
      if (T15_validator.get() == 1)SCState = SHUTDOWN_STATE_NORMAL_OPERATION;
      break;
    case SHUTDOWN_STATE_NORMAL_OPERATION:
      if (T15_validator.get() != 1)
      {
        if(ENABLE_POWERSAVE)Display_Powersave(true);
        SCState = SHUTDOWN_STATE_T15_OFF;
      }
      else
      {
        if(ENABLE_POWERSAVE)Display_Powersave(false);  
      }
      break;
    case SHUTDOWN_STATE_T15_OFF:
      if (T15_validator.get() > 0)
      {
        SCState = SHUTDOWN_STATE_NORMAL_OPERATION;
        break;
      }
      else
      {
        if(ENABLE_SHUTDOWN && CAN_Active.get() == 0)
        {
          shutdown();
        }
        break;
      }
    default:
      SCState = 0;
  }
  /*
  if (T15_validator.get() > 0)
    KeepRunning.set(1);

  if (KeepRunning.get() == 0)
  {


  }*/
}
