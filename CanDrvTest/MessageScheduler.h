void MessageScheduler_Init()
{
  data_7e0[0] = 0x02; //additional Data Bytes
  data_7e0[1] = 0x01; //Show Current Data
  data_7e0[2] = 0x05; //Engine Coolant Temperature
  data_7e0[3] = 0x00;
  data_7e0[4] = 0x00;
  data_7e0[5] = 0x00;
  data_7e0[6] = 0x00;
  data_7e0[7] = 0x00;
}
void MessageScheduler_cyclic()
{
  static uint32_t lastRequest = 0;
  static uint8_t currentMessage = 1;
  if(T15_validator.get())
  {
  if (lastRequest < millis() / 100)
  {
    if (CANDrv_FRMMan_Get_MSG_State(0) != TX_PENDING)
    {
      lastRequest = millis() / 100;
      switch(currentMessage)
      {
        case 1:
          data_7e0[2] = 0x05; //request Engine coolant Temperature;
          CANDrv_FRMMan_Send_Msg(0);
          break;
        case 2:
        case 3:
        case 4:
          break;
        case 5:
          break;
        case 6:
        case 7:
        case 8:
          break;
          
        case 10:
        default:
          currentMessage = 0;
          break;
      }
      currentMessage++;
    }

  }
  }
  else
  {
    currentMessage = 1;
  }


  //if (T15_validator.get())CANDrv_FRMMan_Send_Msg(0);
}
