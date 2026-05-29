

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


#include <AtariStick.h>

// Joystick 1 pins
#define P1_UP    0
#define P1_DOWN  1
#define P1_LEFT  2
#define P1_RIGHT 3
#define P1_BTN1  4
#define P1_BTN2  5

// Joystick 2 pins
#define P2_UP    6
#define P2_DOWN  7
#define P2_LEFT  8
#define P2_RIGHT 9
#define P2_BTN1  10
#define P2_BTN2  16


// Two sticks. Each registers as its own USB HID interface, so the host
// enumerates two independent joysticks from this single Arduino.
AtariStick stick1 = AtariStick(1000);
AtariStick stick2 = AtariStick(1000);

void setup()
{
	// Joystick 1
	stick1.setPin(FUNC_UP, P1_UP);
	stick1.setPin(FUNC_DOWN, P1_DOWN);
	stick1.setPin(FUNC_LEFT, P1_LEFT);
	stick1.setPin(FUNC_RIGHT, P1_RIGHT);
	stick1.setPin(FUNC_BTN1, P1_BTN1);
	stick1.setPin(FUNC_BTN2, P1_BTN2);

	// Joystick 2
	stick2.setPin(FUNC_UP, P2_UP);
	stick2.setPin(FUNC_DOWN, P2_DOWN);
	stick2.setPin(FUNC_LEFT, P2_LEFT);
	stick2.setPin(FUNC_RIGHT, P2_RIGHT);
	stick2.setPin(FUNC_BTN1, P2_BTN1);
	stick2.setPin(FUNC_BTN2, P2_BTN2);
}

void loop()
{
	// Read both sets of digital inputs and send HID reports on change.
	stick1.update();
	stick2.update();
}
