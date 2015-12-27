

/*
 *
 * Copyright (c) 2016 Jan Fredrik Leversund
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#ifndef _AtariStick_h_
#define _AtariStick_h_

#include <Arduino.h>
#include <Debouncer.h>
#include <HID.h>

#define ATARISTICK_REPORT_ID 1


static const uint8_t PROGMEM ataristick_hid_report_desc[] =
{
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x04,		// USAGE (Joystick)
	0xa1, 0x01,		// COLLECTION (Application)
	0x85, ATARISTICK_REPORT_ID,	// REPORT_ID (1)
	0x05, 0x09,		// USAGE_PAGE (Button)
	0x19, 0x01,		// USAGE_MINIMUM (Button 1)
	0x29, 0x02,		// USAGE_MAXIMUM (Button 2)
	0x15, 0x00,		// LOGICAL_MINIMUM (0)
	0x25, 0x01,		// LOGICAL_MAXIMUM (1)
	0x95, 0x02,		// REPORT_COUNT (2)
	0x75, 0x01,		// REPORT_SIZE (1)
	0x81, 0x02,		// INPUT (Data,Var,Abs)
	0x95, 0x01,		// REPORT_COUNT (1)
	0x75, 0x06,		// REPORT_SIZE (6)
	0x81, 0x03,		// INPUT (Cnst,Var,Abs)
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x30,		// USAGE (X)
	0x09, 0x31,		// USAGE (Y)
	0x15, 0x81,		// LOGICAL_MINIMUM (-127)
	0x25, 0x7f,		// LOGICAL_MAXIMUM (127)
	0x75, 0x08,		// REPORT_SIZE (8)
	0x95, 0x02,		// REPORT_COUNT (2)
	0x81, 0x02,		// INPUT (Data,Var,Abs)
	0xc0,			// END_COLLECTION
};

typedef struct
{
	uint8_t btn1 : 1;
	uint8_t btn2 : 1;
	uint8_t pad : 6;
	int8_t x;
	int8_t y;
}
ataristick_report_t;




enum StickFunc: byte { FUNC_UP, FUNC_DOWN, FUNC_LEFT, FUNC_RIGHT, FUNC_BTN1, FUNC_BTN2, FUNC_COUNT };

class AtariStick
{

private:

	int delay; // Default debounce delay
	
	byte changeMask; // Contains a bit mask of changed pin states

	byte pins[FUNC_COUNT];
	Debouncer states[FUNC_COUNT];

	ataristick_report_t report;


	// Which joystick pin is connected to which arduino pin
	byte pinUp;
	byte pinDown;
	byte pinLeft;
	byte pinRight;
	byte pinBtn1;
	byte pinBtn2;

	// List of current joystick states
	Debouncer up;
	Debouncer down;
	Debouncer left;
	Debouncer right;
	Debouncer btn1;
	Debouncer btn2;

	void sendState();

public:
	
	AtariStick(int debounceDelay);
	
	void setPin(StickFunc func, int pin);
	void setPin(StickFunc func, int pin, int debounceDelay);
	
	void update();

	byte changed();
	bool changed(StickFunc func);

	bool state(StickFunc func);

};

#endif /* _AtariStick_h_ */
