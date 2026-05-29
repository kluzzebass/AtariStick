# AtariStick — Paddle Support Spec

Status: **planned, not built.** This is a design note for adding paddle support
later. Nothing here is implemented yet.

## Goal

Read Atari/C64 paddles on the existing dual-connector hardware and present each
paddle to the host as a USB HID device (1 axis + 1 button). Atari and C64
paddles are electrically identical on the pins that matter (verified — see
References), so **one design serves both, with no mode switch.**

## Verified facts (datasheet-backed)

Per control port, paddles use:

- **Pin 9** — paddle A potentiometer
- **Pin 5** — paddle B potentiometer
- **Pin 3** — paddle A button (the joystick LEFT line)
- **Pin 4** — paddle B button (the joystick RIGHT line)
- **Pin 7** — +5V (required for paddles)
- **Pin 8** — GND

Pots are 1 MΩ linear taper. Atari and C64 agree on all of the above; the only
C64-specific quirk (its paddle buttons route through the keyboard matrix) is
internal to the C64 and does not affect this adapter.

## Wiring delta (per connector)

| Pin | Now                | For paddles                                   |
|-----|--------------------|-----------------------------------------------|
| 9   | A9 (read digital)  | A9, read **analog** — no rewire               |
| 5   | unconnected        | route to a free ADC pin — **1 new wire**      |
| 3,4 | digital (L/R)      | already wired — buttons need nothing           |
| 7   | optional/unwired   | **must connect to VCC**                        |
| 8   | GND                | unchanged                                     |

So the real hardware change per connector is: **bring out pin 5, and ensure
pin 7 (+5V) is connected.** Pins 3/4/9 are already wired.

ADC budget: 2 pots × 2 connectors = 4 ADC channels. The Pro Micro exposes 9
(A0–A3, A6–A10). Plenty of headroom.

## The fiddly part — the analog front end (real design risk)

A paddle is wired as a **2-terminal rheostat** (a variable resistor: +5V → pot →
pin), **not** a 3-terminal voltage divider. The pot pin therefore presents a
*variable resistance*, not directly a *variable voltage* the ADC can read. Two
ways to handle it:

- **Option A — fixed pulldown resistor (simplest).** Add a fixed resistor (value
  TBD, ballpark ~1 MΩ) from each pot pin to GND. Pot + resistor form a divider;
  `analogRead()` reads the midpoint. Cheap, then all firmware. Downside:
  range/linearity depend on the resistor choice vs the 1 MΩ pot — needs bench
  tuning.
- **Option B — RC timing (authentic).** Replicate how SID/POKEY read it: charge
  a capacitor through the pot and time it with a hardware timer. No divider
  resistor, more faithful, but more firmware and timing-critical.

**Recommendation:** start with Option A. Measure the actual pot range with a
multimeter, pick the pulldown resistor to map that range to the full ADC span,
then auto-range in firmware to absorb pot-to-pot variance and 40-year drift.
Move to Option B only if A's resolution disappoints.

**This step must wait on measuring a real paddle** (pot type, resistance sweep,
wiper pin) before committing to a resistor value.

## Firmware

- New `AtariPaddle : public PluggableUSBModule` class, mirroring `AtariStick`
  (own HID interface → own device). Descriptor: one absolute axis + one button.
- Axis resolution / range: paddles are commonly read as 0–255. Confirm what
  VirtualC64 expects for a paddle axis before fixing the descriptor (TBD — do
  not assume the joystick's −127..127).
- `analogRead()` the pot pin, scale to the axis. **Auto-range:** track observed
  min/max and map to the full output — self-calibrating, no user config.
- Button = `digitalRead()` on pin 3 (or 4) using the existing `Debouncer`.
- Reuse the existing `setPin` / `update` patterns where they fit.

## HID enumeration

Each paddle = its own interface = its own device (paddle 1, paddle 2, …),
consistent with the dual-stick model and what emulators expect. A full paddle
*pair* on one connector = 2 devices.

Endpoint budget: CDC (3) + N stick/paddle interfaces ≤ 7 usable endpoints, so
about **4 devices total** before running out — document the cap.

## Test protocol (hardware gate)

1. Measure the paddle with a multimeter — confirm it is a passive pot, find the
   wiper pin and resistance sweep. **Gate: must sweep smoothly ~0–1 MΩ.**
2. Wire pin 9 + pin 7 + a pulldown on one connector; flash a one-paddle test
   sketch.
3. Use an IOKit HID monitor (as in the joystick tests) to confirm the axis
   sweeps the full range as the knob turns, and the button registers.
4. Confirm in VirtualC64 with a paddle-aware game (Pong/Arkanoid-style) that the
   knob actually controls play.
5. Only then: add the second pot pin (pin 5), the second connector, and commit.

A single paddle is enough to validate the pin-9 (paddle A) path end to end.

## Out of scope

Firmware autofire, Amiga/ST mouse, and the C64 1351 proportional mouse were all
considered and dropped (modern USB equivalents exist, or the wiring/encoding is
incompatible with this all-digital adapter).

## References

- [Atari joystick port — Wikipedia](https://en.wikipedia.org/wiki/Atari_joystick_port)
- [Control Port — C64-Wiki](https://www.c64-wiki.com/wiki/Control_Port)
