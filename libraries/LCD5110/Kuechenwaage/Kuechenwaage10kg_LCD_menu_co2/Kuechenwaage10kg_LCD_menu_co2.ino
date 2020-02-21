#define MOVING_AVG 2
#define ONTIME 30
#define TAR_AVG 20
#define WEIGHT_AVG 3
#define MAX_MODE_COUNTS 4


//Menu Settings
const uint8_t num_menu = 42;
char menu[][12] = {"Apfel", "Avocado", "Bohnen", "Brot", "Brotchen", "Butter", "Eier", "Feldsalat", "Fisch TK", "Geflugel", "Gurken", "Hackfl.", "Kartoffeln", "Kase", "Kiwi", "Lachs", "Lamm", "Linsen", "Magerquark", "Mais (Dose)", "Margarine", "Mehl", "Milch", "Nudeln", "Olivenol", "Pilze fr.", "Quark", "Rapsol", "Reis", "Rind", "Rosenkohl", "Sahne", "Saure Sahne", "Schwein", "SoBluOl", "Tofu", "Tomate", "Tomate (Wi)", "Wurst", "Zitrone", "Zucker", "Zwiebeln"};
float faktor[] = {0.25, 0.51, 0.74, 0.6, 0.62, 9.2, 2, 0.27, 4.1, 3.7, 0.45, 5.6, 0.4, 5.82, 0.66, 6.3, 7.7, 0.61, 2.52, 1.2, 1.8, 0.34, 1.44, 0.46, 3.1, 1.31, 3.4, 2.7, 3.05, 12.3, 0.28, 4.31, 3.05, 4.2, 2.2, 1.66, 0.77, 2.92, 3.6, 0.36, 0.6, 0.25,};
const uint8_t komma = 3;

#include <HX711Array.h>
#include <NTC.h>
#include <Sleep.h>
uint8_t dout[] = {HX711_DOUT};
uint8_t sck = HX711_SCK;
long adc;
long adcs[MOVING_AVG];
long current_i = 0;
float temp;


HX711Array scale;
Scale4PointCal cal;
NTC_HX711 ntc(scale, 2 * 470000.0, 10.0);


#include <LCD5110_Basic.h>

LCD5110 myGLCD(NOKIA_SCLK, NOKIA_DN, NOKIA_DC, NOKIA_RST, NOKIA_CE);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

volatile uint16_t tics;
ISR(TIMER2_OVF_vect) {
  tics++;
}


volatile uint8_t on_flag = 0;
volatile uint8_t tar_flag = 0;
void isr_int0(void) {
  on_flag = 1;
}

void isr_int1(void) {
  if (! tar_flag) tar_flag = 1;
}

const uint8_t black[] PROGMEM = {
  B00101010, B00010101
};


uint8_t bar_pos;
void printBar(void) {
  for (uint8_t i = 0; i < 84; i += 2) {
    myGLCD.drawBitmap (i, 0, black, 2, 8);
  }
  bar_pos = 83;
}

char text_buffer[17];

#include "fablablogo.h"
#include "sprueche.h"

uint16_t wakeup;
uint8_t on = 1;
float last_weight, current_weight, max_weight;
uint8_t max_mode = 0;
uint8_t max_mode_i = 0;
float weights[MAX_MODE_COUNTS];
uint8_t mode = 0;
uint8_t selected = 0;
uint8_t reprint = 0;
uint8_t c = 0;
uint8_t offset;


void setup()
{
  pinMode(NOKIA_VCC, OUTPUT);
  pinMode(NOKIA_LED, OUTPUT);
  digitalWrite(NOKIA_VCC, HIGH);
  delay(1);
  myGLCD.InitLCD();
  myGLCD.drawBitmap (12, 0, Fablablogo, 60, 48);
  delay(1000);
  Sleep.setupTimer2(2); //init timer2 to 1/16 sec
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), isr_int0, FALLING);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), isr_int1, FALLING);

  Serial.begin(9600);
  Serial.println("Starting ...");
  Serial.flush();
  scale.begin(dout, 1, sck); //start HX711Array with 1 ADCs
  scale.set_no_sleep(1);
  cal.readConf();
  cal.printConf();
  Serial.flush();
  tar_flag = 1;
  scale.power_up();
  wakeup = tics;
}


void loop()
{
  if (on_flag || tar_flag)
    wakeup = tics;


  if ((tics - wakeup) > ONTIME * 16) {
    //myGLCD.clrScr();
    myGLCD.enableSleep();
    uint8_t led = digitalRead(NOKIA_LED);
    digitalWrite(NOKIA_LED, HIGH);
    digitalWrite(NOKIA_VCC, HIGH);
    //pinMode(NOKIA_VCC, INPUT);
    on = 0;
    scale.power_down();
    Sleep.sleep();
    digitalWrite(NOKIA_LED, led);
    myGLCD.disableSleep();
    //myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.drawBitmap (12, 0, Fablablogo, 60, 48);
 //   printBigPoint();
    scale.power_up();
    max_mode = 0;
    mode = 0;
    ntc.readResistance();
    temp = ntc.getTemp(0);
    scale.power_up();
    scale.read_average(1);
    on = 1;
    on_flag = 0;
    tar_flag = 0;
    myGLCD.clrScr();
    wakeup = tics;

  }


  if (mode) {
    if (tar_flag) {
      delay(50);
      if (selected) {
        selected--;
        if (selected < offset) offset--;
        reprint = 1;
      }
      tar_flag = 0;
    }
    if (on_flag) {
      delay(50);
      c = 0;
      while (! digitalRead(2)) {
        c++;
        delay(50);
        if (c > 5) {
          mode = 0;
          on_flag = 0;
          myGLCD.clrScr();
          tar_flag = 2;
          return;
        }
      }
      on_flag = 0;
      if (selected < (num_menu - 1)) {
        selected++;
        if (selected > (offset + 5) && offset < (num_menu - 6)) offset++;
        reprint = 1;
      }

    }
    if (reprint) {
      myGLCD.clrScr();
      myGLCD.setFont(SmallFont);
      for (uint8_t i = 0; i < 6; i++) {
        if((i + offset) == selected){
          myGLCD.invertText(1);
          myGLCD.print("              ", LEFT, i * 8);
        } else 
          myGLCD.invertText(0);
        
        myGLCD.print(menu[i + offset], LEFT, i * 8);
      }
      reprint = 0;
    }
    return;
  }

  if (on && on_flag && ! digitalRead(2)) {
    c = 0;
    while (! digitalRead(2)) {
      c++;
      delay(100);
      if (c > 5) {
        mode = 1;
        on_flag = 0;
        reprint = 1;
        break;
      }
    }
  }


  if (on_flag) {
    digitalWrite(NOKIA_LED, ! digitalRead(NOKIA_LED));
    delay(100);
    on_flag = 0;
  }

  if (tar_flag == 1) {
    myGLCD.setFont(SmallFont);
//    myGLCD.clrScr();
//    myGLCD.drawBitmap (12, 0, Fablablogo, 60, 48);
    printBigPoint();
    //   myGLCD.print("Tare .... ", CENTER, 8);
    delay(300);
    if (! digitalRead(3)) {
      max_mode = 1;
      myGLCD.print("***MAXMODE***", CENTER, 40);
      delay(500);
    } else max_mode = 0;
    ntc.readResistance();
    temp = ntc.getTemp(0);
    scale.power_up();
    scale.read_average(1);
    adc = scale.read_average(TAR_AVG);
    Serial.print("Tare: ");
    Serial.println(adc);
    Serial.flush();
    cal.setTare(adc, temp);
    myGLCD.clrScr();
    myGLCD.print("  TARE DONE  ", CENTER, 24);
    if(max_mode){
            myGLCD.print("***MAXMODE***", CENTER, 40);

    }
    tar_flag = 2;
  }

  adcs[current_i] = scale.read_average(WEIGHT_AVG);
  Serial.println(adcs[current_i]);
  Serial.flush();
  current_i++;
  if (current_i == MOVING_AVG) current_i = 0;
  adc = 0;
  for (uint8_t i = 0; i < MOVING_AVG; i++) {
    adc += adcs[i];
  }
  adc /= MOVING_AVG;

  if (tar_flag == 2) {
    myGLCD.clrScr();
    wakeup = tics;
    printBar();
    tar_flag = 0;
  }
  current_weight = cal.getWeight(adc, temp);


  myGLCD.setFont(SmallFont);
  uint8_t bar_pos_new = 83.0 * ((float)ONTIME - (float)(tics - wakeup) / 16) / ONTIME;
  float bat = 3.3 * 2 * analogRead(A7) / 1023;
  if (bat < 3.8) {
    myGLCD.print("LOW BAT:", LEFT, 0);
    dtostrf(bat, 4, 2, text_buffer); // avr-libc function for floats
    myGLCD.print("    ", RIGHT, 0);
    myGLCD.print(text_buffer, RIGHT, 0);
  } else {
    if (bar_pos_new > bar_pos) printBar();
    myGLCD.clrRow(0, bar_pos_new, bar_pos);
    bar_pos = bar_pos_new;
  }

  if (! max_mode) {
    myGLCD.print(menu[selected], LEFT, 40);
    dtostrf(current_weight / 1000 * faktor[selected], 6, komma, text_buffer); // avr-libc function for floats
    myGLCD.print("       ", RIGHT, 40);
    myGLCD.print(text_buffer, RIGHT, 40);


  } else
    myGLCD.print("***MAXMODE***", CENTER, 40);

  myGLCD.print("       ", LEFT, 8);
  myGLCD.print("       ", LEFT, 16);
  myGLCD.print("       ", LEFT, 24);

  if (max_mode) {
    if (max_mode == 1) {
      myGLCD.print("PUT ON WEIGHT", CENTER, 16);
      if (current_weight > 5) max_mode = 2;
    }
    if (max_mode == 2 || max_mode == 3) {
      myGLCD.print("WAITING FOR", CENTER, 8);
      myGLCD.print("   CONSTANT   ", CENTER, 16);
      myGLCD.print("WEIGHT", CENTER, 24);
      weights[max_mode_i] = current_weight;
      max_mode_i++;
      if (max_mode_i == MAX_MODE_COUNTS) {
        max_mode_i = 0;
        max_mode = 3;
      }
    }
    if (max_mode == 3) {
      max_weight = 0;
      for (uint8_t i = 0; i < MAX_MODE_COUNTS; i++) {
        max_weight += weights[i];
      }
      max_weight /= MAX_MODE_COUNTS;
      float diff = 0;
      for (uint8_t i = 0; i < MAX_MODE_COUNTS; i++) {
        if (abs(weights[i] - max_weight) > diff) diff = abs(weights[i] - max_weight);
      }
      if (diff < 2) max_mode = 4;
    }
    if (max_mode == 4) {
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumF(max_weight, 1, RIGHT, 8);
    }
  } else {
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumF(current_weight, 1, RIGHT, 8);
  }

  if (abs(current_weight - last_weight) > 5) {
    last_weight = current_weight;
    wakeup = tics;
    printBar();
  }
}
