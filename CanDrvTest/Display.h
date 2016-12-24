
void Display_Init_StaticText();

void Display_Init()
{

  //DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
  pinMode(6, OUTPUT);
  pinMode(22, OUTPUT);

  TFTscreen.begin();
  pinMode(6, OUTPUT);
  pinMode(22, OUTPUT);
  Display_Init_StaticText();

  // ste the font size very large for the loop
  TFTscreen.setTextSize(5);

}

void convertIntToString(char* txt, uint8_t length, int16_t number)
{
  uint16_t currentStage = 1;
  if (number < 0)
  {
    length -= 1;
    txt[0] = '-';
    txt++;
  }
  for (int i = 0; i < length; ++i)
  {
    currentStage *= 10;
  }
  for (int i = 0; i < length ; ++i)
  {
    uint8_t numberInCurrentStage = (number % currentStage) / (currentStage / 10);
    txt[i] = 30 + numberInCurrentStage;

  }
}
inline uint8_t tmot_to_x(int16_t tmot)
{
  if(tmot < 0)
    return 2;
  if(tmot > 156)
    return 156+2;
  return (uint8_t) tmot+2;
}

void Display_show_tmot()
{
  int16_t tmot_loc = tmot_validator.get();
  const uint16_t TMOT_textsize = 3;
  const uint16_t TMOT_pos_x = 85;
  const uint16_t TMOT_pos_y = 45;
  static char tmot_old_txt[] = " - ";
  char tmot_txt[] = " - ";
  char tmot_mask[] = " - ";

  TFTscreen.setTextSize(TMOT_textsize);
  
  if ((tmot_loc >= 0))
  {
    tmot_txt[0] = (tmot_loc / 100) + 0x30;
    tmot_txt[1] = (tmot_loc % 100) / 10 + 0x30;
    tmot_txt[2] = (tmot_loc % 10) + 0x30;
  }
  else
  {
    tmot_txt[0] = '-';
    tmot_txt[1] = ((-tmot_loc) % 100) / 10 + 0x30;
    tmot_txt[2] = ((-tmot_loc) % 10) + 0x30;
    if (tmot_loc = -100)
    {
      tmot_txt[0] = ' ';
      tmot_txt[1] = '-';
      tmot_txt[2] = ' ';
    }
  }
  for (uint8_t i; i < 3; ++i)
  {
    if (tmot_old_txt[i] == tmot_txt[i])
    {
      tmot_mask[i] = ' ';
    }
    else
    {
      tmot_mask[i] = tmot_old_txt[i];
    }
  }

  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(tmot_mask, TMOT_pos_x, TMOT_pos_y);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(tmot_txt, TMOT_pos_x, TMOT_pos_y);
  for (uint8_t i = 0; i < 3; ++i)
    tmot_old_txt[i] = tmot_txt[i];
  if(tmot_loc < 0)
  {
    TFTscreen.fill(0,0,0);
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.rect(2,17,156,20);
  }
  else
  {
    uint8_t xpos = tmot_to_x(tmot_loc)-2;
    if(tmot_loc < 70)
    {
      TFTscreen.stroke(255,0,0);
      TFTscreen.fill(255,0,0);
    }
    else if(tmot_loc < 110)
    {
      TFTscreen.stroke(0,255,0);
      TFTscreen.fill(0,255,0);
    }
    else
    {
      TFTscreen.stroke(0,0,255);
      TFTscreen.fill(0,0,255);
    }
    TFTscreen.rect(2,17,xpos,20);
    TFTscreen.fill(0,0,0);
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.rect(xpos+2,17,156-xpos,20);
  }
  
}
void Display_show_Powerstat()
{
  static char PowerStat[] = "0000";
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(PowerStat, 0, 100);
  PowerStat[0] = (T15_validator.get()) ? '1' : '0';
  PowerStat[1] = (StandbyOn_validator.get())?'1':'0';
  PowerStat[2] = (KeyInLock_validator.get())?'1':'0';
  //PowerStat[3] = (EngineStarting)?'1':'0';
  PowerStat[4] = 0;
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(PowerStat, 0, 100);
  TFTscreen.setTextSize(5);  
}

void Display_static_tmot()
{
    TFTscreen.stroke(255, 255, 255);
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(0,15,160,24);
  //TFTscreen.fill(255,255,255);
  //TFTscreen.rect(2,17,100,20);
  TFTscreen.line(2,15,2,5);

  uint8_t x_m40 = tmot_to_x(-40);
  uint8_t x_0 = tmot_to_x(0);
  uint8_t x_50 = tmot_to_x(50);
  uint8_t x_100 = tmot_to_x(100);
  uint8_t x_150 = tmot_to_x(150);
  uint8_t x_215 = tmot_to_x(215);
  
  TFTscreen.setTextSize(1);
  TFTscreen.line(x_0,15,x_0,5);
  TFTscreen.text("0", 4, 2);
  
  TFTscreen.line(x_50,15,x_50,5);
  TFTscreen.text(" 50", x_50-18, 2);
  
  TFTscreen.line(x_100,15,x_100,5);
  TFTscreen.text("100", x_100-18, 2);

  TFTscreen.line(x_150,15,x_150,5);
  TFTscreen.text("150", x_150-18, 2);
  
  
  TFTscreen.setTextSize(3);
  // write the text to the top left corner of the screen
  TFTscreen.text("tmot: ", 0, 45);
  TFTscreen.text(" - ", 85, 45);
}
void Display_Init_StaticText()
{
  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  TFTscreen.setRotation(1);

  Display_static_tmot();
}
void Display_cyclic()
{
  Display_show_tmot();
  Display_show_Powerstat();
/*  TFTscreen.line(0,128,128,128);

  uint8_t circlearray[] = {128,128,128,128,128,128,128,128,128,128,128,127,127,127,127,127,127,127,127,126,126,126,126,126,126,125,125,125,125,124,124,124,124,123,123,123,123,122,122,122,121,121,121,120,
120,119,119,119,118,118,117,117,117,116,116,115,115,114,114,113,113,112,111,111,110,110,109,108,108,107,107,106,105,104,104,103,102,101,101,100,99,98,97,97,96,95,94,93,92,91,90,
89,88,87,86,85,84,82,81,80,79,77,76,75,73,72,70,69,67,65,64,62,60,58,56,54,52,50,47,45,42,39,35,32,28,23,16,0};

for(uint8_t i = 1;i<128;++i)
{
    TFTscreen.line(i-1,128-circlearray[128-(i-1)],i,128-circlearray[128-i]);
}
delay(500);
for(uint8_t i = 1;i<128;++i)
{
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.line(i-1,128-circlearray[128-(i-1)],128,128);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.line(i,128-circlearray[128-(i)],128,128);
  
  delay(10);
}*/




  
/*
  static char txtcode[] = "000";
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(txtcode, 0, 60);
  txtcode[0] = (0 / 100) + 0x30;
  txtcode[1] = (0 % 100) / 10 + 0x30;
  txtcode[2] = (0 % 10) + 0x30;
  txtcode[3] = 0;

  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(txtcode, 0, 60);

  static char PowerStat[] = "0000";
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(PowerStat, 0, 100);
  PowerStat[0] = (T15_validator.get()) ? '1' : '0';
  PowerStat[1] = (StandbyOn_validator.get())?'1':'0';
  PowerStat[2] = (KeyInLock_validator.get())?'1':'0';
  //PowerStat[3] = (EngineStarting)?'1':'0';
  PowerStat[4] = 0;
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(PowerStat, 0, 100);
  TFTscreen.setTextSize(5);*/


}

