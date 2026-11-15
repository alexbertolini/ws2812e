# ESP32-S3 Christmas Light Controller

A rotating set of animated lighting patterns for a WS2812E addressable LED strip, driven by an ESP32-S3. Eleven patterns — twinkle, chases, comets, fire, aurora, and more — cycle automatically every 10 seconds.

## Hardware involved

| Part | Notes |
|---|---|
| ESP32-S3 dev board | This project was built and tested on a SparkleIoT XH-S3E module (ESP32-S3-WROOM-1 based), on a generic DevKitC-1-style breakout. Any ESP32-S3 board works. |
| WS2812E addressable RGB LED strip | 300 LEDs in this build. WS2812E shares the same protocol/timing as WS2812B, so any WS2812B-compatible code and library works. |
| 5V power supply | Sized for your LED count — each LED can draw up to ~60mA at full white, so budget accordingly (a 300-LED strip can draw several amps at high brightness). |
| 74AHCT125 logic level shifter (optional) | Converts the ESP32's 3.3V data signal to a clean 5V signal for the strip. Strongly recommended — see "Why the level shifter" below. |
| Resistor, 330–470 ohm (optional) | In series on the data line, placed after the level shifter's output, right before the strip's data input. |
| Timer socket (optional) | Used upstream of the 5V power adapter (on the AC side) to turn the whole thing on/off on a schedule. Cutting power this way is safe — there's no persistent storage being written during operation, so the board just restarts cleanly from `setup()` when power returns. |

Summary:

| Connection | From | To |
|---|---|---|
| 5V power | Power supply + | ESP32-S3 5V pin, and LED strip 5V |
| Ground | Power supply – | ESP32-S3 GND, level shifter GND, and LED strip GND (all common) |
| Data out | ESP32-S3 GPIO4 | Level shifter input (3.3V side) |
| Data out (shifted) | Level shifter output (5V side) | Through 330–470 ohm resistor, then to LED strip DIN |

**Why the level shifter:** WS2812-family chips read a logic "high" as roughly 0.7 × their supply voltage. At a proper 5V supply, that threshold sits right around 3.5V — just above what the ESP32-S3's 3.3V GPIO can reliably output. Without a level shifter, you may see garbled colors, a stuck/overly bright first pixel, or flickering, especially as supply voltage approaches a true 5V. The 74AHCT125 cleanly converts the 3.3V logic signal to 5V before it reaches the strip.

**Why common ground matters:** If the ESP32-S3 is powered from a separate source (e.g. USB from a computer) while the strip has its own supply, tying both grounds together can create a ground loop, since many wall supplies and computer USB ports are both referenced to mains earth independently. The cleanest fix is to power the ESP32-S3 from the *same* 5V supply as the strip (via its 5V/GND header pins, not USB), so there's only one ground reference in the whole system.

## Software requirements

- [Arduino IDE](https://www.arduino.cc/en/software) (or PlatformIO)
- ESP32 board support installed, with an ESP32-S3 board selected
- **FastLED** library, installed via Library Manager

## Personalization — settings you'll likely want to change

All of these are `#define` constants near the top of the sketch:

| Constant | Default | What it controls |
|---|---|---|
| `LED_PIN` | `4` | The GPIO driving the strip's data line. Change if you wire it to a different pin. |
| `NUM_LEDS` | `300` | Must match your actual strip length exactly, or you'll get a dark tail or an out-of-bounds write. |
| `LED_TYPE` | `WS2812B` | WS2812E uses the same protocol, so leave as-is unless you swap to a different chip (e.g. SK6812, APA102). |
| `COLOR_ORDER` | `GRB` | Most WS2812-family strips use GRB, but some batches are RGB or BRG. If colors look swapped (e.g. green shows where red should be), change this. |
| `BRIGHTNESS` | `180` | Global brightness ceiling (0–255), applied on top of each pattern's own colors. Lower this to reduce power draw. |
| `PATTERN_DURATION_MS` | `10000` | How long each pattern runs before switching to the next, in milliseconds. |
| `NUM_PATTERNS` | `11` | Must match the number of `case` entries in the `switch` inside `loop()`. |

Per-pattern tuning (inside each pattern function):
- `GREEN` / `RED` — base color definitions used by several patterns; adjust the channel values to change intensity.
- Each pattern's own `delay(...)` at the end of its function controls that pattern's animation speed independently of the others.

## Patterns included

0. **Twinkle** — warm white base with random sparkling flashes
1. **Candy cane chase** — red/white blocks marching along the strip
2. **Marquee** — alternating red/green blocks shifting position
3. **Snow** — cool white shimmer with occasional red accent pulses
4. **Comet sweep** — a bright flash travels end-to-end, flipping each LED it passes between green and red
5. **Gradient wave** — a slow-moving blend across red, gold, and green
6. **Fire** — a classic heat-map fire simulation
7. **Icicle drip** — blue-white "drips" falling along a dim icy background
8. **Aurora** — slow shifting green/blue/purple waves
9. **Breathing** — the whole strip fades in and out, swapping between red and green at the darkest point of each breath
10. **Multi-comet** — three colored comets (red, green, gold) bouncing independently along the strip

Patterns rotate automatically in this order; reorder or remove entries from the `switch` statement in `loop()` to customize the sequence.


