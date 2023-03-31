#include <HX711Array.h>
#include <NTC.h>
#include <Sleep.h>
#include <LED7Seg.h>

LED7Seg led;

uint8_t dout[] = {HX711_DOUT};
uint8_t sck = HX711_SCK;
long adc;
#define MOVING_AVG 2
#define TAR_AVG 20
#define WEIGHT_AVG 3
#define MAX_MODE_COUNTS 4
#define ONTIME 30

long adcs[MOVING_AVG];
long current_i = 0;
float temp;


HX711Array scale;
Scale4PointCal cal;
NTC_HX711 ntc(scale, 2 * 470000.0, 10.0);



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



void setup()
{
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
  led.begin(); //Initialize the PINs and setup TIMER1 compareA interrupt (each ms)
  led.scroll("FABLAB BAYREUTH");
}

uint16_t wakeup;
uint8_t on = 1;
float last_weight, current_weight, max_weight;
uint8_t max_mode = 0;
uint8_t max_mode_i = 0;
float weights[MAX_MODE_COUNTS];

void loop()
{
  if (on_flag) {
    wakeup = tics;
    if (! on) {
      led.write("-----");
      on=1;
      float bat = 3.3 * 2 * analogRead(A7) / 1023;
      if (bat < 3.8) {
        led.write("Lo BA");
        delay(1000);
      }
 
    } else {
      noInterrupts();
      tics=wakeup+ONTIME*16;
      interrupts();
    }
    delay(100);
    on_flag = 0;
  }

  if ((tics - wakeup) > ONTIME * 16) {
    on = 0;
    scale.power_down();
    led.sleep();
    scale.power_up();
    
    max_mode = 0;
    on_flag = 1;
    tar_flag = 0;
  }

  
  if (tar_flag == 1) {
    wakeup = tics;
    led.write("TARE"); 
    delay(500);
    if (! digitalRead(3)) {
      max_mode = 1;
      max_weight = 0;
      led.write(" AAA ");
    } else max_mode = 0;
    delay(500);
    ntc.readResistance();
    temp = ntc.getTemp(0);
    scale.power_up();
    scale.read_average(2);
    adc = scale.read_average(20);
    Serial.print("Tare: ");
    Serial.println(adc);
    Serial.flush();
    cal.setTare(adc, temp);
    led.write("donE");
    tar_flag = 2;
  }

  adcs[current_i] = scale.read_average(5);
  Serial.println(adcs[current_i]);
  Serial.flush();
  current_i++;
  if (current_i == MOVING_AVG) current_i = 0;
  adc = 0;
  for (uint8_t i = 0; i < MOVING_AVG; i++) {
    adc += adcs[i];
  }
  adc /= MOVING_AVG;
  char text[10];

  if (tar_flag == 2) {
    wakeup = tics;
    tar_flag = 0;
  }

  
  current_weight = cal.getWeight(adc, temp);
  if (max_mode) {
    if (max_mode == 1) {
      led.write(" 000 ");
      if (current_weight > 5) max_mode = 2;
    }
    if (max_mode == 2 || max_mode == 3) {
      led.write(" PPP ");
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
      led.write(max_weight, 1);
    }
  } else {
    led.write(current_weight, 1);

  }  

  if (abs(current_weight - last_weight) > 5) {
    last_weight = current_weight;
    wakeup = tics;
  }
}
