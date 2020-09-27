#include <HX711Array.h>
#include <NTC.h>
#include <Sleep.h>
uint8_t dout[] = {HX711_DOUT};
uint8_t sck = HX711_SCK;

HX711Array scale;
NTC_HX711 ntc(scale, 2 * 470000.0, 10.0);

#include <LCD5110_Basic.h>
LCD5110 myGLCD(NOKIA_SCLK, NOKIA_DN, NOKIA_DC, NOKIA_RST, NOKIA_CE);

extern uint8_t SmallFont[];

volatile uint8_t on_flag = 0;
volatile uint8_t tar_flag = 0;

volatile uint16_t tics;
ISR(TIMER2_OVF_vect) {
  tics++;
}

void isr_int0(void) {
  on_flag = 1;
}

void isr_int1(void) {
  if (! tar_flag) tar_flag = 1;
}
void setup()
{
  Serial.begin(9600);
  Sleep.setupTimer2(2); //init timer2 to 1/16 sec
  pinMode(NOKIA_VCC, OUTPUT);
  pinMode(NOKIA_LED, OUTPUT);
  digitalWrite(NOKIA_VCC, HIGH);
  Sleep.setupTimer2(2); //init timer2 to 1/16 sec
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), isr_int0, FALLING);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), isr_int1, FALLING);

  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.print("Starting", LEFT, 8);
  scale.begin(dout, 1, sck); //start HX711Array with 1 ADCs
  scale.set_no_sleep(1);
  scale.power_up();
  Serial.println("Setup done");
}

void loop()
{
  Serial.println("Loop start");
  if (on_flag) {
    delay(100);
    digitalWrite(NOKIA_LED, ! digitalRead(NOKIA_LED));
    on_flag = 0;
  }
  scale.power_up();
  ntc.readResistance();
  float temp = ntc.getTemp(0);
  scale.power_up();
  scale.read_average(1);
  long adc =  scale.read_average(20);
  myGLCD.clrScr();
  char text[20];
  dtostrf(temp, 4, 2, text); // avr-libc function for floats
  myGLCD.print("TEMP:", LEFT, 8);
  myGLCD.print(text, RIGHT, 8);
  ltoa(temp, text, 10);
  myGLCD.print("ADC:", LEFT, 16);
  myGLCD.print(text, RIGHT, 16);
  delay(500);
  Serial.println("loop done");

}
