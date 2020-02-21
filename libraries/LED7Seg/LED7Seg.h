/*
 * Library for HX711-LED 7 Seg-Board
 *
 * Five LEDs are multiplexed by MOSFETs
 * The library uses Timer1 to run LED multiplexing in Background.
 *
 */

#ifndef LED7Seg_H
#define LED7Seg_h

#define LEDNUMDIGITS 5
#include <Arduino.h>
#include <Sleep.h>

class LED7Seg {
private:
public:
	static volatile uint8_t bcd[LEDNUMDIGITS];
	static volatile int _scroll_counter;
	static volatile int _scroll_delay;
	static volatile bool _is_scrolling;
	static volatile char* _scroll_ptr;
	static void timer1isr(void);


	/*
	 * start display
	 *
	 * set PIN modes output, enable TIMER1
	 */
	void begin(void);

	/*
	 * end display
	 *
	 * set PIN modes input, disable TIMER1
	 */
	void end(void);

	/*
	 * calls end() and goes to sleep. After wake up begin() is called
	 */
	void sleep(void);

	/*
	 * transforms char to BCD. Bit set indicates LED off!
	 */
	static uint8_t char2bcd(char c);

	/*
	 * Write directly to a defined digits
	 *
	 * pos starts with 0 from the left
	 * v ist a byte.
	 * Lowest bit corresponds to A
	 * Highest bit corresponds to the dot
	 * bit set denotes LED segment on
	 */
	void set(uint8_t v,uint8_t pos);

	/*
	 * Clear display
	 */
	void clear();

	/*
	 * Write a float to the display
	 */
	void write(float f,uint8_t digits=1);
	void write(double d,uint8_t digits=1);
	/*
	 * Write a int to the display
	 */
	void write(int i);
	/*
	 * Write a string to the display
	 */
	void write(const char* c);

	/*
	 * Scroll a string  over the display
	 * the scrolling is done through the ISR
	 */
	void scroll(const char* c,int d=200);
	/*
	 * checks if the display is scrolling a text
	 */
	bool is_scrolling(void);


};


#endif
