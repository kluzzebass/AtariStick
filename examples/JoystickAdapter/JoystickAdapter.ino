
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

#define PIN_UP 0
#define PIN_DOWN 1
#define PIN_LEFT 2
#define PIN_RIGHT 3
#define PIN_BTN1 4
#define PIN_BTN2 5


// Use a 1ms debounce timeout. If you still experience contact bounce, increase this delay.
AtariStick stick = AtariStick(1);

void setup()
{
	// Assign digital pins to joystick functions
	stick.setPin(FUNC_UP, PIN_UP);
	stick.setPin(FUNC_DOWN, PIN_DOWN);
	stick.setPin(FUNC_LEFT, PIN_LEFT);
	stick.setPin(FUNC_RIGHT, PIN_RIGHT);
	stick.setPin(FUNC_BTN1, PIN_BTN1);
	stick.setPin(FUNC_BTN2, PIN_BTN2);

}

void loop()
{
	// Read digital inputs, and send HID report up the wire if something changed.
	stick.update();
}



