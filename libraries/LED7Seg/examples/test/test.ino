/*
 * The library uses TIMER1_COMPA_vect to
 * multiplex the LEDs in the background
 * 
 */

#include <LED7Seg.h>

LED7Seg led;

void int0_isr(void){
  
}


void setup(void){
  Serial.begin(9600);
  Serial.println("Starting");
  Serial.flush();
  pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),int0_isr,FALLING);
  led.begin(); //Initialize the PINs and setup TIMER1 compareA interrupt (each ms)
}

void loop(void){
  Serial.println("loop start");
  led.write(-347.3); //Write a float
  delay(1000);
  led.clear();
  delay(1000);
  led.write(1234.5);
  delay(1000);
  led.write(67.89,2); //Write float with two digits
  delay(1000);
  led.write(0); //Write a INT
  delay(1000);
  led.write(-132);
  delay(1000);
  led.write(432);
  delay(1000);
  led.write("Lo BA"); //Write a String
  delay(1000);
  led.write("TEST"); 
  delay(1000);
  led.write("ESEL"); 
  delay(1000);
  led.write("FAB"); 
  delay(1000);
  led.write("LAB"); 
  delay(1000);
  led.scroll("FABLAB BAYREUTH");
  while(led.is_scrolling()){
    delay(10);
    Serial.print('.');
  }
  Serial.println();

  //Free set of sigments:
  led.set(B01011100,0); // o
  led.set(B01111100,1); // b
  led.set(B01001001,2); // three dashes
  led.set(B01010010,3);
  led.set(B01100100,4);
  delay(2000);
  led.sleep(); //switch of the display until external interrupt wakes up the µC
  delay(2000);
  
}
