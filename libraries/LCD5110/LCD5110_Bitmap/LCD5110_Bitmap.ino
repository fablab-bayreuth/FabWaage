// LCD5110_Bitmap 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of how to use bitmaps.
// You can also see how to use invert().
//

#include <LCD5110_Basic.h>
LCD5110 myGLCD(NOKIA_SCLK, NOKIA_DN, NOKIA_DC, NOKIA_RST, NOKIA_CE);

extern uint8_t arduino_logo[];
extern uint8_t oshw_logo[];

void setup()
{
  pinMode(NOKIA_VCC, OUTPUT);
  pinMode(NOKIA_LED, OUTPUT);
  digitalWrite(NOKIA_VCC, HIGH);
  digitalWrite(NOKIA_LED, HIGH);
  
  myGLCD.InitLCD();
}

void loop()
{
  myGLCD.drawBitmap(0, 0, arduino_logo, 84, 48);
  delay(4000);
  for (int i=0; i<2; i++)
  {
    myGLCD.invert(true);
    delay(500);
    myGLCD.invert(false);
    delay(500);
  }
  delay(4000);

  myGLCD.clrScr();
  myGLCD.drawBitmap(14, 0, oshw_logo, 56, 48);
  delay(4000);
  for (int i=0; i<2; i++)
  {
    myGLCD.invert(true);
    delay(500);
    myGLCD.invert(false);
    delay(500);
  }
  delay(4000);
}
