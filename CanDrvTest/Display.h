#include "pinning.h"

#define COLOR_BG 0,0,0
#define COLOR_DEL COLOR_BG
#define COLOR_FG 0,85,255
#define COLOR_FG_COLD 255,0,0
#define COLOR_FG_OK 0,255,0
#define COLOR_FG_HOT 0,0,255

//TMOT_BAR
const uint16_t TMOT_BAR_pos_y = 2;
const uint16_t TMOT_BAR_size_y = 14;//24

//TMOT_TEXT
const uint16_t TMOT_STATIC_pos_x = 2;
const uint16_t TMOT_STATIC_pos_y = TMOT_BAR_pos_y + TMOT_BAR_size_y + 18;
const uint16_t TMOT_textsize = 3;//3
const uint16_t TMOT_pos_x = 85 + TMOT_STATIC_pos_x; //85
const uint16_t TMOT_pos_y = TMOT_STATIC_pos_y;//45
const uint16_t TMOT_STATIC_textsize = TMOT_textsize;

//POWERSTAT_TEXT
const uint16_t POWERSTAT_textsize = 2;
const uint16_t POWERSTAT_pos_x = 110;//110
const uint16_t POWERSTAT_pos_y = 110;//110


TFT TFTscreen = TFT(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);


void Display_Init_StaticText();

void Display_Init()
{
  pinMode(DISPLAY_POWER_LS, OUTPUT);
  pinMode(DISPLAY_BL, OUTPUT);

  digitalWrite(DISPLAY_POWER_LS, HIGH);//Activate Power to Display

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
  if (powersave)
    digitalWrite(DISPLAY_BL, LOW);//cut Power to the Backlight
  else
    digitalWrite(DISPLAY_BL, HIGH);//enable Power to the Backlight

}
inline uint8_t tmot_to_x(int16_t tmot)
{
  if (tmot < 0)
    return 2;
  if (tmot > 156)
    return 156 + 2;
  return (uint8_t) tmot + 2;
}
uint8_t calculateMask(char* old_string, char* new_string, uint8_t len, char* mask_string)
{
  uint8_t maskcount = 0;
  for (uint8_t i = 0; i < len; ++i)
  {
    if ((uint8_t)old_string[i] != (uint8_t)new_string[i])
    {
      mask_string[i] = old_string[i];
      maskcount++;
    }
    else
    {
      mask_string[i] = ' ';
    }
  }
  return maskcount;
}
void Display_show_tmot()
{
  int16_t tmot_loc = tmot_validator.get();
  static char tmot_old_txt[4] = " - ";
  char tmot_txt[4] = " - ";
  char tmot_mask[4] = "   ";

  TFTscreen.setTextSize(TMOT_textsize);

  if ((tmot_loc >= 0))
  {
    tmot_txt[0] = (tmot_loc >= 100) ? ((tmot_loc / 100) + 0x30)     : ' ';
    tmot_txt[1] = (tmot_loc >= 10) ? ((tmot_loc % 100) / 10 + 0x30) : ' ';
    tmot_txt[2] = (tmot_loc % 10) + 0x30;
  }
  else
  {
    if (tmot_loc != -100) //if tmot_loc = -100 tmot_txt is already " - "
    {
      tmot_txt[0] = '-';
      tmot_txt[1] = ((-tmot_loc) % 100) / 10 + 0x30;
      tmot_txt[2] = ((-tmot_loc) % 10) + 0x30;
    }
  }

  uint8_t maskcount = calculateMask(tmot_old_txt, tmot_txt, 3, tmot_mask);

  if (maskcount > 0)
  {
    TFTscreen.stroke(COLOR_DEL);
    TFTscreen.text(tmot_mask, TMOT_pos_x, TMOT_pos_y);
  }
  TFTscreen.stroke(COLOR_FG);
  TFTscreen.text(tmot_txt, TMOT_pos_x, TMOT_pos_y);

  for (uint8_t i = 0; i < 3; ++i)
    tmot_old_txt[i] = tmot_txt[i];

  if (tmot_loc < 0)
  {
    TFTscreen.fill(COLOR_BG);
    TFTscreen.stroke(COLOR_BG);
    TFTscreen.rect(2, 15 + TMOT_BAR_pos_y, 156, TMOT_BAR_size_y - 4);
  }
  else
  {
    uint8_t xpos = tmot_to_x(tmot_loc) - 2;
    if (tmot_loc < 70)
    {
      TFTscreen.stroke(COLOR_FG_COLD);
      TFTscreen.fill(COLOR_FG_COLD);
    }
    else if (tmot_loc < 110)
    {
      TFTscreen.stroke(COLOR_FG_OK);
      TFTscreen.fill(COLOR_FG_OK);
    }
    else
    {
      TFTscreen.stroke(COLOR_FG_HOT);
      TFTscreen.fill(COLOR_FG_HOT);
    }
    TFTscreen.rect(2, 15 + TMOT_BAR_pos_y, xpos, TMOT_BAR_size_y - 4);
    TFTscreen.fill(COLOR_BG);
    TFTscreen.stroke(COLOR_BG);
    TFTscreen.rect(xpos + 2, 15 + TMOT_BAR_pos_y, 156 - xpos, TMOT_BAR_size_y - 4);
  }
}
void Display_show_Powerstat()
{
  char PowerStat[5] = "0000";
  static char PowerStat_old[5] = "    ";
  char PowerStat_Mask[5] = "    ";


  PowerStat[0] = (T15_validator.get()) ? '1' : '0';
  PowerStat[1] = (StandbyOn_validator.get()) ? '1' : '0';
  PowerStat[2] = (KeyInLock_validator.get()) ? '1' : '0';
  PowerStat[3] = (SCState % 10) + 0x30;
  PowerStat[4] = 0;
  TFTscreen.setTextSize(POWERSTAT_textsize);
  if (calculateMask(PowerStat_old, PowerStat, 4, PowerStat_Mask) > 0)
  {
    TFTscreen.stroke(COLOR_DEL);
    TFTscreen.text(PowerStat_Mask, POWERSTAT_pos_x, POWERSTAT_pos_y);
  }

  TFTscreen.stroke(COLOR_FG);
  TFTscreen.text(PowerStat, POWERSTAT_pos_x, POWERSTAT_pos_y);
  TFTscreen.setTextSize(5);
  for (uint8_t i = 0; i < 4; ++i)PowerStat_old[i] = PowerStat[i];
}

void Display_static_tmot()
{

  TFTscreen.stroke(COLOR_FG);
  TFTscreen.fill(COLOR_BG);
  TFTscreen.rect(0, 13 + TMOT_BAR_pos_y, 160, TMOT_BAR_size_y);
  //TFTscreen.fill(255,255,255);
  //TFTscreen.rect(2,17,100,20);
  TFTscreen.line(2, 13 + TMOT_BAR_pos_y, 2, 3 + TMOT_BAR_pos_y);

  uint8_t x_m40 = tmot_to_x(-40);
  uint8_t x_0 = tmot_to_x(0);
  uint8_t x_50 = tmot_to_x(50);
  uint8_t x_100 = tmot_to_x(100);
  uint8_t x_150 = tmot_to_x(150);
  uint8_t x_215 = tmot_to_x(215);

  TFTscreen.setTextSize(1);
  TFTscreen.line(x_0, 13 + TMOT_BAR_pos_y, x_0, 3 + TMOT_BAR_pos_y);
  TFTscreen.text("0", 4, TMOT_BAR_pos_y);

  TFTscreen.line(x_50, 13 + TMOT_BAR_pos_y, x_50, 3 + TMOT_BAR_pos_y);
  TFTscreen.text(" 50", x_50 - 18, TMOT_BAR_pos_y);

  TFTscreen.line(x_100, 13 + TMOT_BAR_pos_y, x_100, 3 + TMOT_BAR_pos_y);
  TFTscreen.text("100", x_100 - 18, TMOT_BAR_pos_y);

  TFTscreen.line(x_150, 13 + TMOT_BAR_pos_y, x_150, 3 + TMOT_BAR_pos_y);
  TFTscreen.text("150", x_150 - 18, TMOT_BAR_pos_y);


  TFTscreen.setTextSize(TMOT_STATIC_textsize);
  // write the text to the top left corner of the screen
  TFTscreen.text("tmot: ", TMOT_STATIC_pos_x, TMOT_STATIC_pos_y);

  TFTscreen.setTextSize(TMOT_textsize);
  TFTscreen.text(" - ", TMOT_pos_x, TMOT_pos_y);
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
  Display_show_toil();
  Display_show_Powerstat();
}

