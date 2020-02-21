// LCD5110_Sleep_Mode
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of sleep mode.
//
#include <LCD5110_Basic.h>

LCD5110 myGLCD(NOKIA_SCLK, NOKIA_DN, NOKIA_DC, NOKIA_RST, NOKIA_CE);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];

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
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  myGLCD.print("Entering", CENTER, 0);
  myGLCD.print("Sleep Mode", CENTER, 8);
  myGLCD.print("in", CENTER, 16);
  myGLCD.print("Seconds", CENTER, 40);

  myGLCD.setFont(MediumNumbers);
  for (int s=10; s>=0; s--)
  {
    myGLCD.printNumI(s, CENTER, 24, 2, '0');
    delay(1000);
  }
  
  myGLCD.enableSleep();
  digitalWrite(NOKIA_LED, LOW);

  delay(5000);
  digitalWrite(NOKIA_LED, HIGH);

  myGLCD.disableSleep();
  myGLCD.setFont(SmallFont);
  myGLCD.print("Awake again!", CENTER, 0);
  myGLCD.print("The screen was", CENTER, 16);
  myGLCD.print("cleared while", CENTER, 24);
  myGLCD.print("in Sleep Mode.", CENTER, 32);
  delay(5000);
}
