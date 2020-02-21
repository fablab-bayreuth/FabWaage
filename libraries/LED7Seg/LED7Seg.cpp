#include "LED7Seg.h"

volatile uint8_t LED7Seg::bcd[LEDNUMDIGITS]={255,255,255,255,255};
volatile uint8_t mplex_offset=0;
const uint8_t mplex_pins[]={A2,A3,A4,A5,4};
volatile int LED7Seg::_scroll_counter=0;
volatile int LED7Seg::_scroll_delay=200;
volatile char* LED7Seg::_scroll_ptr=0;
volatile bool LED7Seg::_is_scrolling=false;

void LED7Seg::timer1isr(void){
// Handle new char via scrolling
	if(_is_scrolling){
		_scroll_counter++;
		if(_scroll_counter>=_scroll_delay){
			uint8_t i=0;
			while(i<(LEDNUMDIGITS-1)){
				bcd[i]=bcd[i+1];
				i++;
			}
			if(*_scroll_ptr){
				bcd[i]=char2bcd(*_scroll_ptr);
				_scroll_ptr++;
			} else {
				bcd[i]=B11111111;
				i=0;
				_is_scrolling=false;
				while(i<(LEDNUMDIGITS)){
					if(bcd[i]!=0xff){
						_is_scrolling=true;
						break;
					}
					i++;
				}
			}
			_scroll_counter=0;
		}
	}

	//A+B -> PD6+PD7
	//C-H -> PB0-PB5
	digitalWrite(mplex_pins[mplex_offset],LOW);
	mplex_offset++;
	if(mplex_offset==LEDNUMDIGITS) mplex_offset=0;
	PORTD&=B00111111; //Clear bits
	PORTD|=LED7Seg::bcd[mplex_offset]<<6;
	PORTB&=B11000000; //Clear bits
	PORTB|=LED7Seg::bcd[mplex_offset]>>2;

	digitalWrite(mplex_pins[mplex_offset],HIGH);
	TCNT1 = 0;
}

ISR(TIMER1_COMPA_vect){

	LED7Seg::timer1isr();
}

void LED7Seg::begin(void){
	noInterrupts();           // Alle Interrupts temporär abschalten
	TCCR1A = 0;
	TCCR1B = 0;
	OCR1A = 8000;           // Output Compare Register vorbelegen
	TCCR1B |= (1 << CS10);    // Prescale-Wert 1 spezifizieren -> 1000
	TCNT1 = 0;
	TIMSK1 |= (1 << OCIE1A);  // Timer Compare Interrupt aktivieren
	interrupts();

	for(uint8_t i=0;i<LEDNUMDIGITS;i++){
		digitalWrite(mplex_pins[i],LOW);
		pinMode(mplex_pins[i],OUTPUT);
	}

	PORTD|=B11000000;
	DDRD|=B11000000;
	PORTB|=B00111111;
	DDRB|=B00111111;

}

void LED7Seg::end(void){
	TIMSK1=0;   // Timer 1 deaktivieren
	for(uint8_t i=0;i<LEDNUMDIGITS;i++){
		digitalWrite(mplex_pins[i],LOW);
		pinMode(mplex_pins[i],INPUT);
	}
	DDRD&=B00111111;
	DDRB&=B11000000;
}

void LED7Seg::sleep(void){
	end();
	Sleep.sleep();
	begin();
}


static uint8_t LED7Seg::char2bcd(char c){
	switch(c){
	case '0':
	case 'O':
	case 'D':
		return B11000000;
		break;
	case '1':
	case 'I':
		return B11111001;
		break;
	case '2':
	case 'Z':
		return B10100100;
		break;
	case '3':
		return B10110000;
		break;
	case '4':
		return B10011001;
		break;
	case '5':
	case 'S':
		return B10010010;
		break;
	case '6':
		return B10000010;
		break;
	case '7':
		return B11111000;
		break;
	case '8':
	case 'B':
		return B10000000;
		break;
	case '9':
		return B10010000;
		break;
	case '-':
		return B10111111;
		break;
	case ' ':
		return B11111111;
		break;
	case 'A':
	case 'R':
		return B10001000;
		break;
	case 'C':
		return B11000110;
		break;
	case 'E':
		return B10000110;
		break;
	case 'F':
		return B10001110;
		break;
	case 'H':
		return B10001001;
		break;
	case 'L':
		return B11000111;
		break;
	case 'P':
		return B10001100;
		break;
	case 'U':
		return B11000001;
		break;
	case 'T':
		return B11001110;
		break;
	case 'Y':
		return B10011001;
		break;
	case 'o':
		return B10100011;
		break;
	case 'd':
		return B10100001;
		break;
	case 'c':
		return B10100111;
		break;
	}
	return B11111111;

}

void LED7Seg::set(uint8_t v,uint8_t pos){
	if(pos>=LEDNUMDIGITS) return;
	LED7Seg::bcd[pos]=~v;
}

void LED7Seg::clear(void){
	for(uint8_t i=0;i<LEDNUMDIGITS;i++){
		set(0x0,i);
	}
}

void LED7Seg::write(float f,uint8_t digits){
	_is_scrolling=false;
	char text_buffer[LEDNUMDIGITS+2];
    dtostrf(f, 6, digits, text_buffer); // avr-libc function for floats
    uint8_t offset=0;
    for(uint8_t i=0;i<LEDNUMDIGITS+1;i++){
    	if(text_buffer[i]=='.'){
    		bitClear(LED7Seg::bcd[offset-1],7);
    		continue;
    	}
    	LED7Seg::bcd[offset]=char2bcd(text_buffer[i]);
    	offset++;
    }
}
void LED7Seg::write(double d,uint8_t digits ){
	write((float) d,digits);
}

void LED7Seg::write(int i){
	_is_scrolling=false;
	char text_buffer[LEDNUMDIGITS+1];
	itoa(i,text_buffer,10);
	uint8_t offset=0;
	for(uint8_t i=0;i<(LEDNUMDIGITS-strlen(text_buffer));i++){
    	LED7Seg::bcd[offset]=char2bcd(' ');
    	offset++;
	}
    for(uint8_t i=0;i<strlen(text_buffer);i++){
    	LED7Seg::bcd[offset]=char2bcd(text_buffer[i]);
    	offset++;
    }
}

void LED7Seg::write(const char* c){
	_is_scrolling=false;
	uint8_t offset=0;
	while(c[offset] && offset<LEDNUMDIGITS){
    	LED7Seg::bcd[offset]=char2bcd(c[offset]);
    	offset++;
	}
	while(offset<LEDNUMDIGITS){
		LED7Seg::bcd[offset]=B11111111;
		offset++;
	}
}

void LED7Seg::scroll(const char* c,int d){
	clear();
	noInterrupts();
	_scroll_ptr=c;
	_scroll_delay=d;
	_is_scrolling=true;
	_scroll_counter=0;
	interrupts();
}

bool LED7Seg::is_scrolling(void){
	return _is_scrolling;
}

/*
void LED7Seg::scroll(const char* c,int d){
	char l;
	char b[]="     ";
	write(b);
	while(l=*c){
		c++;
		for(uint8_t i=0;i<4;i++){
			b[i]=b[i+1];
		}
		b[4]=l;
		write(b);
		delay(d);
	}
	for(uint8_t j=0;j<5;j++){
		for(uint8_t i=0;i<4;i++){
			b[i]=b[i+1];
		}
		b[4]=' ';
		write(b);
		delay(d);
	}
}
*/
