// modified  https://github.com/zeroeth/time_loop
#include <Adafruit_NeoPixel.h>
#include <Time.h>  

#define SET_CLOCK_PIN 2
#define SET_TIME_PIN 3
#define RING_PIN 6

// Set Clock Button
int clock_state;
int last_clock_state = LOW;

// Set Time Button
int time_state;
int last_time_state = LOW;

// Track Clock Set States
int num_states = 5;       // Number of clock states:
int clock_time_state = 0; // 0 - Run Clock
                          // 1 - Set Hour
                          // 2 - Set Minute
                          // 3 - Set Second
                          // 4 - Set Colors

// Debounce buttons
long last_clock_debounce_time = 0;
long last_time_debounce_time = 0;
long debounce_delay = 50; // milliseconds

// From adaFruit NEOPIXEL goggles example: Gamma correction improves appearance of midrange colors
const uint8_t gamma[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
      7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
     13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
     20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
     30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
     42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
     58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
     76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
     97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
    122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
    150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
    182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
    218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
};

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
int inner_offset = 24;
int inner_pixels = 24;
int outer_pixels = 60;
int pixels = inner_pixels + outer_pixels;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, RING_PIN, NEO_GRB + NEO_KHZ800);

//Set number of hours, should be 12 or 24
int num_hours = 12;

//Set the top pixel for each ring
int inner_top_led = 11 % inner_pixels;
int outer_top_led = 36 % outer_pixels;

// Off Color
uint32_t off_color    = strip.Color (  0,  0,  0);

//RGB
int current_clock_color = 0;
uint8_t current_brightness = 0;
uint32_t milli_color, second_color, minute_color, hour_color;
uint32_t point_color = strip.Color ( 64, 64, 64); // white

// Keep the current time
int current_second = 0;
int current_minute = 0;
int current_hour = 0;

/* CLOCK */
class ClockPositions
{
 public:
  uint8_t px_milli;
  uint8_t px_second;
  uint8_t px_minute;
  uint8_t px_hour;

  ClockPositions ();
  void update    ();
};


ClockPositions::ClockPositions()
{
  px_milli = px_second = px_minute = px_hour = 0;
}

void ClockPositions::update()
{
  // Inner Loop
  // If px is greater than inner pixels must subtract inner pixels
  px_hour   = inner_top_led + map (current_hour % num_hours, 0,  num_hours, 0, inner_pixels);
  if (px_hour > inner_pixels) { px_hour = px_hour - inner_pixels; };

  // Outer Loop
  // If px is greater than total pixels must subtract outer pixels
  // Update the milliseconds only if clock isn't being set
  if (clock_time_state == 0) {
    px_milli  = outer_top_led + map ((millis() %  1000), 0,  1000, 0, outer_pixels);
    if (px_milli > pixels) { px_milli = px_milli - outer_pixels; };
  }

  px_second = outer_top_led + map (current_second % 60, 0, 60, 0, outer_pixels);
  if (px_second > pixels) { px_second = px_second - outer_pixels; };
  
  px_minute = outer_top_led + map (current_minute % 60, 0, 60, 0, outer_pixels);
  if (px_minute > pixels) { px_minute = px_minute - outer_pixels;};

  // Fade up to full brightness (above 255 causes roll over)
  if (current_brightness < 255) {
    current_brightness += 1;
    strip.setBrightness(current_brightness);
  }
}


/* CLOCK VIEW */

class ClockSegments
{
 public:
  ClockPositions    &positions;
  Adafruit_NeoPixel &strip;

  ClockSegments (Adafruit_NeoPixel&, ClockPositions&);
  void draw  ();
  void clear ();
  void add_color (uint8_t position, uint32_t color);
  uint32_t blend (uint32_t color1, uint32_t color2);
};


ClockSegments::ClockSegments (Adafruit_NeoPixel& n_strip, ClockPositions& n_positions): strip (n_strip), positions (n_positions)
{
}


void ClockSegments::draw()
{
  clear();

  // Clock Points
  uint8_t px_00 = inner_offset + outer_top_led + 0;
  if (px_00 > pixels) { px_00 = px_00 - outer_pixels;};
  add_color (px_00, point_color);

  uint8_t px_15 = inner_offset + outer_top_led + 15;
  if (px_15 > pixels) { px_15 = px_15 - outer_pixels;};
  add_color (px_15, point_color);

  uint8_t px_30 = inner_offset + outer_top_led + 30;
  if (px_30 > pixels) { px_30 = px_30 - outer_pixels;};
  add_color (px_30, point_color);

  uint8_t px_45 = inner_offset + outer_top_led + 45;
  if (px_45 > pixels) { px_45 = px_45 - outer_pixels;};
  add_color (px_45, point_color);

  // Inner Ring
  add_color ((positions.px_hour+0)              % inner_pixels, hour_color);
  add_color ((positions.px_hour+1)              % inner_pixels, hour_color);
  
  // Outer Ring
  add_color (inner_offset + positions.px_minute % outer_pixels, minute_color);
  add_color (inner_offset + positions.px_second % outer_pixels, second_color);
  add_color (inner_offset + positions.px_milli  % outer_pixels, milli_color);

  strip.show ();
}


void ClockSegments::add_color (uint8_t position, uint32_t color)
{
  uint32_t blended_color = blend (strip.getPixelColor (position), color);

  /* Gamma mapping */
  uint8_t r,b,g;

  r = (uint8_t)(blended_color >> 16),
  g = (uint8_t)(blended_color >>  8),
  b = (uint8_t)(blended_color >>  0);

  strip.setPixelColor (position, blended_color);
}


uint32_t ClockSegments::blend (uint32_t color1, uint32_t color2)
{
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  return strip.Color (constrain (r1+r2, 0, 255),
                      constrain (g1+g2, 0, 255),
                      constrain (b1+b2, 0, 255));
}


void ClockSegments::clear ()
{
  for(uint16_t i=0; i<strip.numPixels (); i++) {
      strip.setPixelColor (i, off_color);
  }
}

/* Color Palettes */
// Use https://kuler.adobe.com/create/color-wheel
// Analagous color scheme with some spread
// Multiply all values by 0.17 to reduce brightness

void setColorPrimary ()
{
  // primary colors
  milli_color  = strip.Color (  0,  0, 42); // blue
  second_color = strip.Color (  0, 42,  0); // green
  minute_color = strip.Color ( 42,  0,  0); // red
  hour_color   = strip.Color ( 42, 42, 42); // white
}

void setColorRoyal ()
{
  //blue, green, & purple
  milli_color  = strip.Color ( 24,  0, 24); // magenta
  second_color = strip.Color ( 17,  0, 44); // purple
  hour_color   = strip.Color (  0, 10, 44); // royal blue
  minute_color = strip.Color (  0, 44, 10); // green
}

void setColorTequila ()
{
  //tequila sunrise color scheme
  milli_color  = strip.Color ( 44, 21,  0); // redest orange
  second_color = strip.Color ( 44, 30,  0); // slightly yellower
  hour_color   = strip.Color ( 44, 42,  0); // yellow
  minute_color = strip.Color ( 43,  0,  5); // red
}

void setColorRed ()
{
  // 255,0,0 center on red
  milli_color  = strip.Color ( 43, 22,  0); // orange
  second_color = strip.Color ( 39, 10,  0); // red orange
  hour_color   = strip.Color ( 39,  0, 35); // pink
  minute_color = strip.Color ( 24,  0, 43); // purple
}

void setColorOrange ()
{
  // 255,128,0 center on orange
  milli_color  = strip.Color ( 38, 34,  0); // yellow
  second_color = strip.Color ( 34, 24,  0); // orange
  hour_color   = strip.Color ( 34,  8,  0); // red orange
  minute_color = strip.Color ( 38,  0,  3); // red
}

void setColorCyan ()
{
  // 0,255,255 center on cyan
  milli_color  = strip.Color (  0,  2, 43); // bluew
  second_color = strip.Color (  0, 20, 39); // orange
  hour_color   = strip.Color (  0, 39, 19); // red orange
  minute_color = strip.Color (  1, 43,  0); // red
}

void setColorBlue ()
{
  // 0,0,255 center on blue
  milli_color  = strip.Color ( 36,  1, 38); // pink
  second_color = strip.Color ( 17,  1, 34); // purple blue
  hour_color   = strip.Color (  1, 16, 34); // other blue
  minute_color = strip.Color (  0, 34, 38); // light blue
}

/* APP */
ClockPositions positions;
ClockSegments  segments(strip, positions);

void setup ()
{
  // Initialize Buttons
  pinMode(SET_CLOCK_PIN, INPUT);
  pinMode(SET_TIME_PIN, INPUT);
  
  setColorBlue();
  strip.begin ();
  strip.show (); // Initialize all pixels to 'off'
}


void loop ()
{
  // Change the clock settings state
  int clock_reading = digitalRead(SET_CLOCK_PIN);
  if (clock_reading != last_clock_state) {
    last_clock_debounce_time = millis();
  }
  if ((millis() - last_clock_debounce_time) > debounce_delay) {
    if (clock_reading != clock_state) {
      clock_state = clock_reading;

      // Change clock state on button press
      if (clock_state == HIGH) {
        clock_time_state = (clock_time_state + 1) % num_states;
      }
    }
  }
  last_clock_state = clock_reading;

  // Set the hours, minutes or seconds
  if (clock_time_state != 0) {
    int time_reading = digitalRead(SET_TIME_PIN);
    if (time_reading != last_time_state) {
      last_time_debounce_time = millis();
    }
    if ((millis() - last_time_debounce_time) > debounce_delay) {
      if (time_reading != time_state) {
        time_state = time_reading;

        if (time_state == HIGH) {
          if (clock_time_state == 1) {
            // Update Hours
            current_hour = (current_hour + 1) % 24;
          }
          else if (clock_time_state == 2) {
            // Update minutes
            current_minute = (current_minute + 1) % 60;
          }
          else if (clock_time_state == 3) {
            // Update seconds
            current_second = (current_second + 1) % 60;
          }
          else if (clock_time_state == 4) {
            current_clock_color = (current_clock_color + 1) % 4;
            if (current_clock_color == 0) {
              setColorBlue();
            }
            else if (current_clock_color == 1) {
              setColorRed();
            }
            else if (current_clock_color == 2) {
              setColorCyan();
            }
            else if (current_clock_color == 3) {
              setColorOrange();
            }
          }
        }
      }
    }
    // Set time
    setTime(current_hour, current_minute, current_second, 1, 1, 1970);
    last_time_state = time_reading;
  } 
  else {
    // Get Time only if not in setting mode
    time_t t = now();
    current_second = second(t);
    current_minute = minute(t);
    current_hour = hour(t);
  }

  // Update positions
  positions.update();

  // Animations for 4 clock quarters
  if ((current_minute == 0) && (current_second == 0)) {
    rainbowMultiCycle(10);
  }
  else if ((current_minute == 15) && (current_second == 0)) {
    colorWipe(strip.Color(255, 0, 0), 25); // Red
  }
  else if ((current_minute == 30) && (current_second == 0)) {
    colorWipe(strip.Color(0, 255, 0), 25); // Green
  }
  else if ((current_minute == 45) && (current_second == 0)) {
    colorWipe(strip.Color(0, 0, 255), 25); // Blue
  }

  // Draw the clock
  segments.draw();
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint32_t wait) {
  fadeOut();

  int num_pixels = strip.numPixels();
  for(uint16_t i=0; i<num_pixels; i++) {
    strip.setPixelColor(i, c);
    current_brightness += 255 / num_pixels;
    strip.setBrightness(current_brightness);
    strip.show();
    delay(wait);
  }
  fadeOut();
}

// Slightly different, this makes the rainbow equally distributed throughout
// across two rings
void rainbowMultiCycle(uint8_t wait) {
  uint16_t i, j, k;
  fadeOut();

  for(k=0; k<256; k++) { // cycles of all colors on wheel
    for(i=0; i< inner_pixels; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / inner_pixels) + k) & 255));
    }
    for(j=0; j< outer_pixels; j++) {
      strip.setPixelColor(inner_pixels + j, Wheel(((j * 256 / outer_pixels) + k) & 255));
    }
    current_brightness = k;
    strip.setBrightness(current_brightness);
    strip.show();
    delay(wait);
  }
  fadeOut();
}

void fadeOut()
{
  while (current_brightness != 0) {
    current_brightness -= 1;
    strip.setBrightness(current_brightness);
    strip.show();
    delay(1);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
