

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

// Constructor. Takes a contact debounce timeout. Each instance plugs itself in
// as its own USB HID interface, so the host sees one independent joystick per
// instance -- declare one AtariStick per physical stick.
AtariStick::AtariStick(unsigned long debounceDelay)
	: PluggableUSBModule(1, 1, epType)
{
	delay = debounceDelay;
	epType[0] = EP_TYPE_INTERRUPT_IN;
	PluggableUSB().plug(this);
}

// Emit this interface's interface + HID + endpoint descriptor during USB setup.
int AtariStick::getInterface(uint8_t* interfaceCount)
{
	*interfaceCount += 1;
	HIDDescriptor desc =
	{
		D_INTERFACE(pluggedInterface, 1,
			USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
		D_HIDREPORT(sizeof(ataristick_hid_report_desc)),
		D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint),
			USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
	};
	return USB_SendControl(0, &desc, sizeof(desc));
}

// Serve the HID report descriptor, but only for this instance's interface.
int AtariStick::getDescriptor(USBSetup& setup)
{
	if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) return 0;
	if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) return 0;
	if (setup.wIndex != pluggedInterface) return 0;
	return USB_SendControl(TRANSFER_PGM, ataristick_hid_report_desc,
		sizeof(ataristick_hid_report_desc));
}

// Acknowledge the standard HID class control requests for this interface.
bool AtariStick::setup(USBSetup& setup)
{
	if (pluggedInterface != setup.wIndex) return false;

	uint8_t request = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;

	if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
	{
		if (request == HID_GET_REPORT)   return true;
		if (request == HID_GET_PROTOCOL) return true;
		if (request == HID_GET_IDLE)     return true;
	}

	if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
	{
		if (request == HID_SET_PROTOCOL) return true;
		if (request == HID_SET_IDLE)     return true;
		if (request == HID_SET_REPORT)   return true;
	}

	return false;
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
	
	// Send the report on this interface's own interrupt-IN endpoint.
	USB_Send(pluggedEndpoint | TRANSFER_RELEASE, &report, sizeof(ataristick_report_t));
}
