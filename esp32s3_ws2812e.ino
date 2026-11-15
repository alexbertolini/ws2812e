/*
 * WS2812E RGB LED strip on ESP32-S3 - Christmas animation rotation
 * Library: "FastLED" (Arduino Library Manager)
 *
 * Wiring:
 *   Strip 5V  -> external 5V supply (+)
 *   Strip GND -> external 5V supply (-) AND ESP32 GND (grounds must be common)
 *   Strip DIN -> GPIO 4 -> 74AHCT125 level shifter -> 330-470 ohm resistor -> strip DIN
 *
 * See README.md and wiring-diagram.svg for full wiring and setup notes.
 *
 * Eleven patterns auto-rotate every 10 seconds:
 *    0. Twinkle            - warm white base, random white sparkles
 *    1. Candy cane chase   - red/white blocks marching
 *    2. Marquee            - alternating red/green blocks shifting
 *    3. Snow                - cool white shimmer, red accent pulses
 *    4. Comet sweep        - flash sweeps end to end, flips green/red
 *    5. Gradient wave      - slow red/gold/green blend
 *    6. Fire               - heat-map fire simulation
 *    7. Icicle drip        - blue-white drips on an icy background
 *    8. Aurora             - slow shifting green/blue/purple waves
 *    9. Breathing          - whole strip fades in/out, swaps red/green in the dark
 *   10. Multi-comet        - three colored comets bouncing independently
 */

#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    300
#define LED_TYPE    WS2812B   // WS2812E uses the same timing/protocol as WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  180

#define PATTERN_DURATION_MS 10000UL
#define NUM_PATTERNS 11

CRGB leds[NUM_LEDS];

uint8_t currentPattern = 0;
unsigned long patternStartTime = 0;

// ---- shared colors ----
CRGB GREEN = CRGB(0, 40, 0);
CRGB RED   = CRGB(40, 0, 0);

// ---- pattern function declarations ----
void patternTwinkle(bool reset);
void patternCandyCane(bool reset);
void patternMarquee(bool reset);
void patternSnow(bool reset);
void patternComet(bool reset);
void patternGradient(bool reset);
void patternFire(bool reset);
void patternIcicleDrip(bool reset);
void patternAurora(bool reset);
void patternBreathing(bool reset);
void patternMultiComet(bool reset);

void setup() {
  Serial.begin(115200);
  delay(500);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  patternStartTime = millis();
  Serial.println("FastLED Christmas rotation ready.");
}

void loop() {
  unsigned long now = millis();
  bool justSwitched = false;

  if (now - patternStartTime >= PATTERN_DURATION_MS) {
    currentPattern = (currentPattern + 1) % NUM_PATTERNS;
    patternStartTime = now;
    justSwitched = true;
  }

  switch (currentPattern) {
    case 0:  patternTwinkle(justSwitched);   break;
    case 1:  patternCandyCane(justSwitched); break;
    case 2:  patternMarquee(justSwitched);   break;
    case 3:  patternSnow(justSwitched);      break;
    case 4:  patternComet(justSwitched);     break;
    case 5:  patternGradient(justSwitched);  break;
    case 6:  patternFire(justSwitched);      break;
    case 7:  patternIcicleDrip(justSwitched); break;
    case 8:  patternAurora(justSwitched);    break;
    case 9:  patternBreathing(justSwitched); break;
    case 10: patternMultiComet(justSwitched); break;
  }
}

// ---------------------------------------------------------------
// 0. Twinkling warm white fairy lights
// ---------------------------------------------------------------
void patternTwinkle(bool reset) {
  static CRGB base = CRGB(50, 25, 5);  // warm amber base

  if (reset) {
    fill_solid(leds, NUM_LEDS, base);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 3) {
      leds[i] = CRGB::White;
    } else {
      leds[i].nscale8(250);  // slow fade back to base
      if (leds[i].r < base.r) leds[i].r = base.r;
      if (leds[i].g < base.g) leds[i].g = base.g;
      if (leds[i].b < base.b) leds[i].b = base.b;
    }
  }
  FastLED.show();
  delay(30);
}

// ---------------------------------------------------------------
// 1. Candy cane chase: red/white blocks marching along the strip
// ---------------------------------------------------------------
void patternCandyCane(bool reset) {
  static uint8_t offset = 0;
  const uint8_t blockSize = 10;

  if (reset) offset = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    uint16_t pos = (i + offset) % (blockSize * 2);
    leds[i] = (pos < blockSize) ? CRGB(60, 0, 0) : CRGB(45, 45, 45);
  }
  FastLED.show();

  offset = (offset + 1) % (blockSize * 2);
  delay(60);
}

// ---------------------------------------------------------------
// 2. Red/green marquee chase
// ---------------------------------------------------------------
void patternMarquee(bool reset) {
  static uint8_t offset = 0;
  const uint8_t groupSize = 3;

  if (reset) offset = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    bool green = (((i + offset) / groupSize) % 2 == 0);
    leds[i] = green ? GREEN : RED;
  }
  FastLED.show();

  offset = (offset + 1) % (groupSize * 2);
  delay(100);
}

// ---------------------------------------------------------------
// 3. Snow shimmer with occasional red accent pulses
// ---------------------------------------------------------------
void patternSnow(bool reset) {
  static CRGB base = CRGB(18, 18, 26);  // cool dim base

  if (reset) {
    fill_solid(leds, NUM_LEDS, base);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t roll = random8();
    if (roll < 2) {
      leds[i] = CRGB(200, 200, 255);  // bright snow sparkle
    } else if (roll < 3) {
      leds[i] = CRGB(90, 0, 0);       // red accent pulse
    } else {
      leds[i].nscale8(250);
      if (leds[i].r < base.r) leds[i].r = base.r;
      if (leds[i].g < base.g) leds[i].g = base.g;
      if (leds[i].b < base.b) leds[i].b = base.b;
    }
  }
  FastLED.show();
  delay(30);
}

// ---------------------------------------------------------------
// 4. Comet sweep: bright flash travels end to end, flips green/red
// ---------------------------------------------------------------
void patternComet(bool reset) {
  static bool isGreen[NUM_LEDS];
  static int sweepPos = 0;
  static int sweepDir = 1;
  const uint8_t FLASH_FADE_SPEED = 100;

  if (reset) {
    sweepPos = 0;
    sweepDir = 1;
    for (int i = 0; i < NUM_LEDS; i++) isGreen[i] = true;
    fill_solid(leds, NUM_LEDS, GREEN);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == sweepPos) {
      leds[i] = CRGB::White;
      isGreen[i] = !isGreen[i];
    } else {
      CRGB target = isGreen[i] ? GREEN : RED;
      leds[i].nscale8(255 - FLASH_FADE_SPEED);
      if (leds[i].r < target.r) leds[i].r = target.r;
      if (leds[i].g < target.g) leds[i].g = target.g;
      if (leds[i].b < target.b) leds[i].b = target.b;
    }
  }
  FastLED.show();

  sweepPos += sweepDir;
  if (sweepPos >= NUM_LEDS - 1 || sweepPos <= 0) {
    sweepDir = -sweepDir;
  }

  delay(1);
}

// ---------------------------------------------------------------
// 5. Slow red/green/gold gradient wave
// ---------------------------------------------------------------
void patternGradient(bool reset) {
  static uint8_t startHue = 0;
  static CRGBPalette16 pal = CRGBPalette16(
    CRGB::Red, CRGB::OrangeRed, CRGB::Gold, CRGB::Green,
    CRGB::Green, CRGB::Gold, CRGB::OrangeRed, CRGB::Red,
    CRGB::Red, CRGB::OrangeRed, CRGB::Gold, CRGB::Green,
    CRGB::Green, CRGB::Gold, CRGB::OrangeRed, CRGB::Red
  );

  if (reset) startHue = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = (uint8_t)((i * 255 / NUM_LEDS) + startHue);
    leds[i] = ColorFromPalette(pal, index, 120, LINEARBLEND);
  }
  FastLED.show();

  startHue++;
  delay(20);
}

// ---------------------------------------------------------------
// 6. Fireplace flicker (classic heat-map fire simulation)
// ---------------------------------------------------------------
void patternFire(bool reset) {
  static byte heat[NUM_LEDS];
  if (reset) memset(heat, 0, sizeof(heat));

  // cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((55 * 10) / NUM_LEDS) + 2));
  }
  // heat drifts upward and diffuses
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  // randomly ignite new sparks near one end
  if (random8() < 120) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  // map heat values to fire colors
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor(heat[j]);
  }
  FastLED.show();
  delay(15);
}

// ---------------------------------------------------------------
// 7. Icicle drip: blue-white drips falling on a dim icy background
// ---------------------------------------------------------------
void patternIcicleDrip(bool reset) {
  const uint8_t NUM_DRIPS = 5;
  static int dripPos[NUM_DRIPS];
  static bool dripActive[NUM_DRIPS];
  CRGB bg = CRGB(5, 5, 15);

  if (reset) {
    fill_solid(leds, NUM_LEDS, bg);
    for (int d = 0; d < NUM_DRIPS; d++) dripActive[d] = false;
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(240);
    if (leds[i].r < bg.r) leds[i].r = bg.r;
    if (leds[i].g < bg.g) leds[i].g = bg.g;
    if (leds[i].b < bg.b) leds[i].b = bg.b;
  }

  for (int d = 0; d < NUM_DRIPS; d++) {
    if (!dripActive[d]) {
      if (random8() < 5) { dripActive[d] = true; dripPos[d] = 0; }
    } else {
      leds[dripPos[d]] = CRGB(200, 220, 255);
      dripPos[d]++;
      if (dripPos[d] >= NUM_LEDS) dripActive[d] = false;
    }
  }
  FastLED.show();
  delay(20);
}

// ---------------------------------------------------------------
// 8. Aurora / northern lights: slow shifting green-blue-purple waves
// ---------------------------------------------------------------
void patternAurora(bool reset) {
  static uint16_t t = 0;
  if (reset) t = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave1 = sin8(i * 2 + t);
    uint8_t wave2 = sin8(i * 3 - t * 2);
    uint8_t hue = 95 + (wave1 / 8) - (wave2 / 8);  // green through blue/purple
    uint8_t bri = 40 + (wave1 / 4);
    leds[i] = CHSV(hue, 200, bri);
  }
  FastLED.show();
  t += 3;
  delay(25);
}

// ---------------------------------------------------------------
// 9. Breathing pulse: whole strip fades in/out, swaps red/green
//    at the darkest point of each breath
// ---------------------------------------------------------------
void patternBreathing(bool reset) {
  static uint8_t phase = 0;
  static bool green = true;
  if (reset) { phase = 0; green = true; }

  uint8_t bri = sin8(phase);
  CRGB color = green ? CRGB(0, 255, 0) : CRGB(255, 0, 0);
  color.nscale8(bri);
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();

  uint8_t prevPhase = phase;
  phase += 2;
  // sin8's darkest point is at phase 192, not at the 255->0 wrap, so switch color there
  if (prevPhase < 192 && phase >= 192) green = !green;
  delay(20);
}

// ---------------------------------------------------------------
// 10. Multi-comet: three colored comets (red, green, gold) bouncing
//     independently along the strip
// ---------------------------------------------------------------
void patternMultiComet(bool reset) {
  static int pos[3];
  static int dir[3];
  CRGB colors[3] = { CRGB(50, 0, 0), CRGB(0, 50, 0), CRGB(50, 40, 0) };

  if (reset) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    pos[0] = 0; pos[1] = NUM_LEDS / 3; pos[2] = 2 * NUM_LEDS / 3;
    dir[0] = dir[1] = dir[2] = 1;
  }

  for (int i = 0; i < NUM_LEDS; i++) leds[i].nscale8(230);

  for (int c = 0; c < 3; c++) {
    leds[pos[c]] = CRGB::White;
    if (pos[c] + 1 < NUM_LEDS) leds[pos[c] + 1] += colors[c];
    if (pos[c] - 1 >= 0) leds[pos[c] - 1] += colors[c];

    pos[c] += dir[c];
    if (pos[c] <= 0 || pos[c] >= NUM_LEDS - 1) dir[c] = -dir[c];
  }
  FastLED.show();
  delay(10);
}
