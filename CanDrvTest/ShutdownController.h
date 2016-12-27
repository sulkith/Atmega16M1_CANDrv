uint8_t SCState = 0;
void ShutdownController_Init()
{
  //pinMode(24, INPUT);
  //KeepRunning.set(1);
  SCState = 0;
}
void(* resetFunc) (void) = 0;//declare reset function at address 0
void shutdown()
{
  SMCR = 0x05;
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  pinMode(24, INPUT);
  EICRA = 0x00; //Low Level Interrupt
  EIMSK = 0x08; //Enable INT3
  Display_Shutdown();
  digitalWrite(14, HIGH);
  PRR = 0xFF;
  sleep_cpu();
  PRR = 0x00;
  digitalWrite(14, LOW);
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
    case 0:
      if (millis() >= 5000)SCState = 1;
      break;
    case 1:
      if (T15_validator.get() != 1)
      {
        T15_offTime = millis();
        SCState = 2;
      }
      break;
    case 2:
      if (T15_validator.get() > 0)
      {
        SCState = 1;
        break;
      }
      else
      {
        if (millis() > T15_offTime + 5000)
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
