#include "pinning.h"



#define COLOR_BG 0,0,0
#define COLOR_FG 0,70,255
#define COLOR_FG_COLD 255,0,0
#define COLOR_FG_OK 0,255,0
#define COLOR_FG_HOT 0,0,255


TFT TFTscreen = TFT(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);

void Display_Init_StaticText();

void Display_Init()
{
  pinMode(DISPLAY_POWER_LS, OUTPUT);
  pinMode(DISPLAY_BL, OUTPUT);
  
  //digitalWrite(DP_POWER_LS, LOW);//Deactivate Power to Display
  //delay(50);
  digitalWrite(DISPLAY_POWER_LS, HIGH);//Activate Power to Display
  //delay(50);
  TFTscreen.begin();
  Display_Init_StaticText();
  digitalWrite(DISPLAY_BL, HIGH);
}
void Display_Shutdown()
{
  pinMode(DISPLAY_RST, INPUT);
  pinMode(DISPLAY_DC, INPUT);
  pinMode(DISPLAY_CS, INPUT);
  pinMode(DISPLAY_MOSI, INPUT);
  pinMode(DISPLAY_SCK, INPUT);
  pinMode(DISPLAY_BL, INPUT);
  digitalWrite(DISPLAY_POWER_LS, LOW);//cut Power to the Display
}
void Display_Powersave(bool powersave)
{
  //if(powersave)
    //digitalWrite(DISPLAY_BL, LOW);//cut Power to the Backlight
  //else
  //  digitalWrite(DISPLAY_BL, HIGH);//enable Power to the Backlight
    
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

  TFTscreen.stroke(COLOR_BG);
  TFTscreen.text(tmot_mask, TMOT_pos_x, TMOT_pos_y);
  TFTscreen.stroke(COLOR_FG);
  TFTscreen.text(tmot_txt, TMOT_pos_x, TMOT_pos_y);
  for (uint8_t i = 0; i < 3; ++i)
    tmot_old_txt[i] = tmot_txt[i];
  if(tmot_loc < 0)
  {
    TFTscreen.fill(COLOR_BG);
    TFTscreen.stroke(COLOR_BG);
    TFTscreen.rect(2,17,156,20);
  }
  else
  {
    uint8_t xpos = tmot_to_x(tmot_loc)-2;
    if(tmot_loc < 70)
    {
      TFTscreen.stroke(COLOR_FG_COLD);
      TFTscreen.fill(COLOR_FG_COLD);
    }
    else if(tmot_loc < 110)
    {
      TFTscreen.stroke(COLOR_FG_OK);
      TFTscreen.fill(COLOR_FG_OK);
    }
    else
    {
      TFTscreen.stroke(COLOR_FG_HOT);
      TFTscreen.fill(COLOR_FG_HOT);
    }
    TFTscreen.rect(2,17,xpos,20);
    TFTscreen.fill(COLOR_BG);
    TFTscreen.stroke(COLOR_BG);
    TFTscreen.rect(xpos+2,17,156-xpos,20);
  }
  
}
void Display_show_Powerstat()
{
  static char PowerStat[] = "0000";
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(COLOR_BG);
  TFTscreen.text(PowerStat, 0, 100);
  PowerStat[0] = (T15_validator.get()) ? '1' : '0';
  PowerStat[1] = (StandbyOn_validator.get())?'1':'0';
  PowerStat[2] = (KeyInLock_validator.get())?'1':'0';
  //PowerStat[3] = (EngineStarting)?'1':'0';
  PowerStat[4] = 0;
  TFTscreen.stroke(COLOR_FG);
  TFTscreen.text(PowerStat, 0, 100);
  TFTscreen.setTextSize(5);  
}

void Display_static_tmot()
{
    TFTscreen.stroke(COLOR_FG);
  TFTscreen.fill(COLOR_BG);
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
  TFTscreen.background(COLOR_BG);
  TFTscreen.setRotation(1);

  Display_static_tmot();
}
void Display_cyclic()
{
  Display_show_tmot();
  Display_show_Powerstat();
}

