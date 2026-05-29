

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
#include <PluggableUSB.h>
#include <HID.h>   // for HIDDescriptor, D_HIDREPORT and the HID_* request
                   // constants. We never call HID(), so the stock single-
                   // interface HID singleton is never instantiated.

// Each AtariStick instance is its own USB HID interface. The host therefore
// enumerates one independent joystick per instance -- separate IOHIDDevices,
// not report-id collections inside a single interface. This is what lets two
// sticks show up as two devices in hosts that enumerate by device (e.g.
// VirtualC64 on macOS), which the older report-id approach could not do.

// HID report descriptor for one Atari-style joystick: 2 buttons + X/Y axes.
// No REPORT_ID -- with one interface per stick there is nothing to multiplex,
// so every instance shares this single descriptor. It lives in PROGMEM because
// the USB layer transmits descriptors straight from flash (TRANSFER_PGM).
static const uint8_t PROGMEM ataristick_hid_report_desc[] =
{
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x04,		// USAGE (Joystick)
	0xa1, 0x01,		// COLLECTION (Application)
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
	0xc0			// END_COLLECTION
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

class AtariStick : public PluggableUSBModule
{

private:

	unsigned long delay; // Default debounce delay in microseconds

	byte changeMask; // Contains a bit mask of changed pin states

	byte pins[FUNC_COUNT];
	Debouncer states[FUNC_COUNT];

	ataristick_report_t report;

	uint8_t epType[1]; // Endpoint type for this interface's interrupt-IN endpoint

	void sendState();

protected:

	// PluggableUSBModule overrides -- one HID interface per instance.
	int getInterface(uint8_t* interfaceCount);
	int getDescriptor(USBSetup& setup);
	bool setup(USBSetup& setup);

public:

	// debounceDelay: contact debounce timeout in microseconds. Each instance
	// registers as its own USB HID joystick; just declare one per stick.
	AtariStick(unsigned long debounceDelay);

	void setPin(StickFunc func, int pin);
	void setPin(StickFunc func, int pin, unsigned long debounceDelay);

	void update();

	byte changed();
	bool changed(StickFunc func);

	bool state(StickFunc func);

};

#endif /* _AtariStick_h_ */
