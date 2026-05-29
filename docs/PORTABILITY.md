# USB portability and stick count

Notes for running this on something other than an ATmega32U4, or building an
adapter with more than two sticks.

## What ties it to the platform

The library has no register-level USB code, but it's written against the Arduino
AVR core's `PluggableUSB` API, so it runs wherever that API does and needs
porting where it doesn't.

The pin reads (`pinMode`, `digitalRead`), the debouncing, and the axis/button
logic in `update()`/`sendState()` are plain Arduino and carry over anywhere. The
USB parts don't: the `PluggableUSBModule` base class, `PluggableUSB().plug()`,
`USB_Send`, `USB_SendControl`, `EP_TYPE_INTERRUPT_IN`, and the
`D_INTERFACE`/`D_ENDPOINT`/`D_HIDREPORT` macros all come from the AVR core. A
port means rewriting the three overrides (`getInterface`, `getDescriptor`,
`setup`) and the `USB_Send` call against the target's USB stack; everything else
stays.

Known targets:

- ATmega32U4 (Leonardo, Micro, Pro Micro): what it's built and tested on.
- Other native-USB AVRs (e.g. AT90USB1286): same API, usually more endpoints,
  should drop in.
- SAMD21 (Zero, MKR): has a PluggableUSB, but `USB_Send`/`EP_TYPE_*` and the
  macros differ — expect to adjust the USB methods.
- RP2040, ESP32-S2/S3: different stack (TinyUSB), `PluggableUSB.h` doesn't apply.
- Non-USB AVRs (Uno, Nano): no `USBCON`, nothing to plug into.

## More than two sticks

Each instance only touches its own `pluggedInterface`/`pluggedEndpoint` — no
instance counter, no shared state — so more sticks is just more instances plus
their `setPin`/`update` calls. The code doesn't cap it.

The limit is USB endpoints, which is a separate spec from pin count. Each stick
takes one interrupt-IN endpoint. The 32U4 has ~7 usable; CDC serial takes 3
(that's what gives you auto-reset on upload), leaving room for about four sticks.
A chip with more GPIO doesn't necessarily have more endpoints, so check that
before counting on it. Running out doesn't fail at compile time — enumeration
just breaks — so confirm the endpoint budget for the target chip first.

The USB code lives directly in `AtariStick` rather than behind an abstraction.
If cross-architecture support ever matters, pulling the three overrides and
`USB_Send` out into their own file would keep the port to one place.
