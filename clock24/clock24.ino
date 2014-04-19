// modified  https://github.com/zeroeth/time_loop
#include <Adafruit_NeoPixel.h>
#include <Time.h>  

#define PIN 6

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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, PIN, NEO_GRB + NEO_KHZ800);

//Set number of hours, should be 12 or 24
int num_hours = 12;

//Set the top pixel for each ring
int inner_top_led = 15 % inner_pixels;
int outer_top_led = 35 % outer_pixels;

// Off Color
uint32_t off_color    = strip.Color (  0,  0,  0);

//RGB
uint32_t milli_color  = strip.Color ( 42,  0,  0); // was (10, 0, 0)
uint32_t second_color = strip.Color (  0,  0, 42); // was (0, 0, 10)
uint32_t minute_color = strip.Color (  0, 42,  0); // was (15,10,10)
uint32_t hour_color   = strip.Color ( 42, 42, 42); // was (0, 10, 0)

//tequila sunrise color scheme
/*
uint32_t milli_color  = strip.Color ( 44, 21, 0); // redest orange
uint32_t second_color = strip.Color ( 44, 30, 0); // slightly yellower
uint32_t hour_color   = strip.Color ( 44, 42, 0); // yellow
uint32_t minute_color = strip.Color ( 43,  0, 5); // red
*/

//blue, green, & purple color scheme
/*
uint32_t milli_color  = strip.Color ( 24,  0, 24); // magenta
uint32_t second_color = strip.Color ( 17,  0, 44); // purple
uint32_t hour_color   = strip.Color (  0, 10, 44); // royal blue
uint32_t minute_color = strip.Color (  0, 44, 10); // green
*/

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
  // Get Time
  time_t t = now();
  current_second = second(t);
  current_minute = minute(t);
  current_hour = hour(t);

  // Inner Loop
  px_hour   = inner_top_led + map (current_hour % num_hours, 0,  num_hours, 0, inner_pixels);
  if (px_hour > pixels) { px_hour = px_hour - inner_pixels; };
  
  //px_milli  = map ((millis() %  1000), 0,  1000, 0, pixels);
  //if (px_mllli > inner_pixels) { px_milli = px_milli - inner_pixels; };

  // Outer Loop
  px_second = outer_top_led + map (current_second % 60, 0, 60, 0, outer_pixels);
  if (px_second > outer_pixels) { px_second = px_second - outer_pixels; };
  
  px_minute = outer_top_led + map (current_minute % 60, 0, 60, 0, outer_pixels);
  if (px_minute > pixels) { px_minute = px_minute - outer_pixels;};
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

  add_color ((positions.px_hour+0)              % inner_pixels, hour_color);
  add_color ((positions.px_hour+1)              % inner_pixels, hour_color);
  
  add_color (inner_offset + positions.px_minute % outer_pixels, minute_color);

  add_color (inner_offset + positions.px_second % outer_pixels, second_color);

  // add_color (positions.px_milli      % inner_pixels, milli_color);

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


/* SIMPLE MIXER */
// add rgb and clamp


/* APP */
ClockPositions positions;
ClockSegments  segments(strip, positions);

void setup ()
{
  strip.begin ();
  strip.show (); // Initialize all pixels to 'off'
  
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //rainbowMultiCycle(10);
}


void loop ()
{
  positions.update ();
  segments.draw ();
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint32_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
// across two rings
void rainbowMultiCycle(uint8_t wait) {
  uint16_t i, j, k;

  int num_cycles = 1;
  for(k=0; k<256*num_cycles; k++) { // cycles of all colors on wheel
    for(i=0; i< inner_pixels; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / inner_pixels) + k) & 255));
    }
    for(j=0; j< outer_pixels; j++) {
      strip.setPixelColor(inner_pixels + j, Wheel(((j * 256 / outer_pixels) + k) & 255));
    }
    strip.show();
    delay(wait);
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
