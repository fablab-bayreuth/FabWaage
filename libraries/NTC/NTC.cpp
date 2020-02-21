#include "NTC.h"
#include <Sleep.h>
#include <math.h>

float NTC_Sensor::R2T(float r){
	float log_r = log(r / ntc_type * 10);
	return 440.61073 - 75.69303 * log_r + 4.20199 * log_r * log_r
			- 0.09586 * log_r * log_r * log_r;
}


float NTC_Sensor::getTemp(void){
	return R2T(readResistance());
}

NTC_Sensor::NTC_Sensor(float nt){
	ntc_type=nt;
}

float NTC_Calc::readResistance(void){
	return ntc_type;
}


NTC_ADC::NTC_ADC(uint8_t pp,uint8_t ap,float pr,float nt):NTC_Sensor(nt){
	power_pin=pp;
	adc_pin=ap;
	pre_resistor=pr;
}

float NTC_ADC::readResistance(void){
	pinMode(power_pin,OUTPUT);
	analogReference(DEFAULT);
	digitalWrite(power_pin,HIGH);
	int adc=analogRead(adc_pin);
	digitalWrite(power_pin,LOW);
	pinMode(power_pin,INPUT);
	return pre_resistor * adc / (1023-adc);

}

NTC_HX711::NTC_HX711(HX711Array &hx, uint8_t pp, float pr, float nt, uint8_t n):NTC_Sensor(nt){
	 power_pin=pp;
	 hx711=&hx;
	 pre_resistor=pr;
	 number=n;
}
NTC_HX711::NTC_HX711(HX711Array &hx, float pr, float nt, uint8_t n):NTC_Sensor(nt){
	 power_pin=0;
	 hx711=&hx;
	 pre_resistor=pr;
	 number=n;
}

float NTC_HX711::readResistance(void){
	if(power_pin>0){
		pinMode(power_pin,OUTPUT);
		digitalWrite(power_pin,HIGH);
		delay(1);
	}
	hx711->power_up();
	hx711->set_gain(32);
	hx711->read();
	float adc=hx711->get_single_readings()[number];
	adc=adc/32/256/256/256;
	hx711->set_gain(128);
	hx711->power_down();
	if(power_pin){
		digitalWrite(power_pin,LOW);
		pinMode(power_pin,INPUT);
	}
	return adc/(1-adc)*pre_resistor;
}

float NTC_HX711::getTemp(uint8_t nr){
	float adc=hx711->get_single_readings()[nr];
	adc=adc/32/256/256/256;
	return R2T(adc/(1-adc)*pre_resistor);
}

