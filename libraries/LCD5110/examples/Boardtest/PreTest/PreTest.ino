/*
 * Test to run bevor LCD is soldered to board
 * Check pins of LCD and HX711 against GND to
 * see if there are solder shortcuts on the chip
 * 
 */

void setup(void){
  Serial.begin(9600);
  Serial.println("Starting");
  for(uint8_t i=2;i<14;i++){
    pinMode(i,INPUT_PULLUP);
    Serial.print(i);
    Serial.print(" ");
  }
  for(uint8_t i=0;i<6;i++){
    pinMode(A0+i,INPUT_PULLUP);
    Serial.print("A");
    Serial.print(i);
    Serial.print(" ");
  }
  Serial.println();
}

void loop(void){
  for(uint8_t i=2;i<14;i++){
    if(i>9) Serial.print(" ");
    
    Serial.print(digitalRead(i));
    Serial.print(" ");
  }
  for(uint8_t i=0;i<6;i++){
    Serial.print(" ");
    Serial.print(digitalRead(A0+i));    
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
  
}
