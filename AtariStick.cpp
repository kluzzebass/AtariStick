

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

// Constructor, takes a contact debounce timeout as the only parameter.
AtariStick::AtariStick(unsigned long debounceDelay)
{
	delay = debounceDelay;

	// Setup HID report structure globally
	static bool usbInit = false;
	
	if (!usbInit)
	{
	 	static HIDSubDescriptor node(ataristick_hid_report_desc, sizeof(ataristick_hid_report_desc));
		HID().AppendDescriptor(&node);
		usbInit = true;
	}
}

// Assign a digital input pin to a joystick function.
void AtariStick::setPin(StickFunc func, int pin)
{
	pins[func] = pin;
	pinMode(pin, INPUT_PULLUP);
	states[func].delay = delay;
}

// Assign a digital input pin to a joystick function, and also set an individual contact debounce timeout.
void AtariStick::setPin(StickFunc func, int pin, unsigned long debounceDelay)
{
	setPin(func, pin);
	states[func].delay = debounceDelay;
}

// Check on all input pins, and send state report. Call this as often as possible to minimize latency.
void AtariStick::update()
{
	changeMask = 0;

	// Update pin states
	for (int i = 0; i < FUNC_COUNT; i++)
	{
		// Read, flip and debounce datapin.
		states[i].update(!digitalRead(pins[i]));
		
		// Flag any changes
		if (states[i].changed) bitSet(changeMask, i);
	}

	// If something's changed, send a state report
	if (changeMask) sendState();
}

// Returns a bitmask with changed pins. If 0, nothing has changed.
byte AtariStick::changed()
{
	return changeMask;
}

// Has this specific function changed?
bool AtariStick::changed(StickFunc func)
{
	return bitRead(changeMask, func);
}

// What is the state of this specific function?
bool AtariStick::state(StickFunc func)
{
	return states[func].state;
}

// Send a new state report.
void AtariStick::sendState()
{

	// Copy button states
	report.btn1 = state(FUNC_BTN1);
	report.btn2 = state(FUNC_BTN2);

	// Check X axis
	if (state(FUNC_LEFT))
	{
		report.x = -127;
	}
	else if (state(FUNC_RIGHT))
	{
		report.x = 127;
	}
	else
	{
		report.x = 0;
	}

	// Check Y axis (Y-axis is flipped, so min/max is flipped)
	if (state(FUNC_UP))
	{
		report.y = 127;
	}
	else if (state(FUNC_DOWN))
	{
		report.y = -127;
	}
	else
	{
		report.y = 0;
	}
	
	// Send the report.
	HID().SendReport(ATARISTICK_REPORT_ID, &report, sizeof(ataristick_report_t));
}
