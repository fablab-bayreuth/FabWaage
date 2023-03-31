
const unsigned char bigPoints[][34] PROGMEM = {
  "Flug\nAustralien",
  "Flug\nSri Lanka",
  "10000 km\nim SUV",
  "1000 l\nHeizoel",
  "1000 l\nDiesel",
  "10000 km\nAutofahrt",
  "10000 km\nE-Smart+Okostrom",
  "Flug\nNew York",
  "Flug\nBrasilien",
  "Neues\nHandy",
  "1 Jahr\nFleichkonsum",
  "1 Jahr\nMilchprodukte",
  "Kreuzfahrt\n7 Tage",
  "1000 l\nBenzin",
  "1 Jahr\nNeue Kleider" 
};

float tonnen[]={
  11,4.4,3,3,3,2.1,0.6,3.6,6.3,0.05,0.4,0.4,1.5,2.7,0.95
  
};

const uint8_t max_spruch=15;

void printBigPoint(void){
  myGLCD.clrScr();
  uint8_t spruch=random(0,max_spruch);
  const unsigned char* p=bigPoints[spruch];
  uint8_t line=0;
  unsigned char c=pgm_read_byte(p);
  uint8_t col=0;
  while(c){
    c=pgm_read_byte(p);
    text_buffer[col]=c;
    col++;
    if(c=='\n'){
      text_buffer[col-1]=0;
      myGLCD.print(text_buffer, CENTER, line*8);
      col=0;
      line++;
    }
    p++;
  }
  myGLCD.print(text_buffer, CENTER, line*8);
  line++;
  dtostrf(tonnen[spruch], 4, 2, text_buffer); 
  strcat(text_buffer," Tonnen");
  myGLCD.print(text_buffer, CENTER, line*8);
  line++;
  myGLCD.print("==", CENTER, line*8);
  line++;
  uint8_t selected=random(0,num_menu);
  itoa(tonnen[spruch]*1000/faktor[selected], text_buffer,10); 
  strcat(text_buffer," kg");
  myGLCD.print(text_buffer, CENTER, line*8);
  line++;
  myGLCD.print(menu[selected], CENTER, line*8);
  

}
