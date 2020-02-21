/*
 * Sketch for Balance with 7-Segment LED Display
 * 
 * Please install BayEOS-LowPower-Board Package
 * https://github.com/BayCEER/BayEOS-Arduino
 * and choose BayEOS Low Power Board with ATMega328p, 8MHz 
 * 
 * stefan@holzheu.de
 * 
 */

#include <HX711Array.h>
#include <NTC.h>
#include <Sleep.h>
#include <LED7Seg.h>

LED7Seg led;

uint8_t dout[] = {HX711_DOUT};
uint8_t sck = HX711_SCK;
long adc;
#define MOVING_AVG 10
#define MAX_DEV 0.1
#define TAR_AVG 20
#define WEIGHT_AVG 3
#define ONTIME 30

long adcs[MOVING_AVG];
uint8_t current_i = 0;
uint8_t dev_count=0;

float temp;


HX711Array scale;
Scale4PointCal cal;
NTC_HX711 ntc(scale, 2 * 470000.0, 10.0);



volatile uint16_t tics;
ISR(TIMER2_OVF_vect) {
  tics++;
}


volatile uint8_t tar_flag = 0;
void isr_int0(void) {
  tar_flag = 1;
}


void setup()
{
  Sleep.setupTimer2(2); //init timer2 to 1/16 sec
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), isr_int0, FALLING);

  Serial.begin(9600);
  Serial.println("Starting ...");
  Serial.flush();
  scale.begin(dout, 1, sck); //start HX711Array with 1 ADCs
  scale.set_no_sleep(1);
  //read calibration data from EEPROM
  cal.readConf();
  cal.printConf();
  Serial.flush();
  tar_flag = 1;
  scale.power_up();
  led.begin(); //Initialize the PINs and setup TIMER1 compare interrupt (each ms)
}

uint16_t wakeup;
uint8_t on = 1;
float last_weight, current_weight;
bool switch_off_flag;

inline void store_wakeup(void){
    noInterrupts();
    wakeup = tics;
    interrupts();  
}

void loop()
{
  //Check, if we reached the maximum ontime or switch_off_flag is set
  if (switch_off_flag || (tics - wakeup) > ONTIME * 16) {
    on = 0;
    scale.power_down();
    led.sleep(); //The sketch stops here until we press TARE
    scale.power_up();
    delay(1000); //This shows last value
    switch_off_flag=0;
    
  }

  //Tar-Flag from INT0
  if (tar_flag) {
    //Check voltage
    if (! on) {
      on=1;
      float bat = 3.3 * 2 * analogRead(A7) / 1023;
      if (bat < 3.8) {
        led.write("Lo BA");
        delay(1000);
      }
    } 
    led.scroll("FABLAB BAYREUTH");
    delay(1500);
    //Temperature compensation is not needed
//    temp = ntc.getTemp(0);
    temp=20;
    scale.power_up();
    scale.read_average(2);
    adc = scale.read_average(20);
    cal.setTare(adc, temp);
    tar_flag = 0;

    //Wait until scrolling is done
    while(led.is_scrolling()){
      
    }
    dev_count=0;
    store_wakeup();   
  }
  
  // Read ADC and store value in array
  adcs[current_i] = scale.read_average(5);
  current_i++;
  if (current_i == MOVING_AVG) current_i = 0;
  last_weight = current_weight;
  current_weight = cal.getWeight(adcs[current_i], temp);
  led.write(current_weight, 2);


  // Got a larger weight change 
  if (abs(current_weight - last_weight) > 5) {
    store_wakeup();
  }


  //Check if the deviation from one measurment to the other is lower than MAX_DEV (e.g. 0.1kg)
  if(current_weight>1 && abs(current_weight-last_weight)<MAX_DEV){
    dev_count++;
  } else {
    dev_count=0;
  }

  //We have enough constant measuremnts and calculate the average
  if(dev_count>MOVING_AVG){
    adc = 0;
    for (uint8_t i = 0; i < MOVING_AVG; i++) {
      adc += adcs[i];
    }
    adc /= MOVING_AVG;
    led.clear();
    delay(300);
    current_weight = cal.getWeight(adc, temp);
    led.write(current_weight, 2);
    unsigned long start=millis();
    //Wait for 10s or tar_flag (INT0)
    while((millis()-start)<10000 && ! tar_flag){
      
    }
    //No TAR-Flag - switch off on next loop call
    if(! tar_flag){
      switch_off_flag=1;
    }
  }


}
