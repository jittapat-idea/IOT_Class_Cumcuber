#include <Adafruit_NeoPixel.h> 

const uint32_t COLOR_DATA_ARRAY[] = {
   0x00FF00, // Starting with the Green color
   0xFF0000,// the Red color
   0x0000FF,// Blue color
   0x00FFFF,// light Blue
   0xFFFF00,//yellow
   0xFF00FF,//purple
   0x8F8F8F,//while
   0x000000,
   0x000000,
   0x000000,
   0x000000,
   0x000000,
   0x000000,
   0x000000,
   0x000000,
   0x000000
};

#define NUM_PIXELS  (8)           // The number of pixels
#define WS2812_PIN  (GPIO_NUM_18) // The ESP32 pin for DATA output

// Create an object from the 'Adafruit_NeoPixel' class 
Adafruit_NeoPixel pixels(NUM_PIXELS, WS2812_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  pixels.begin(); // Initialize the NeoPixel WS2812 strip.
  pixels.setBrightness(255); // Set the brightness to 255.
}

void loop() {
  static uint32_t NUM_COLORS  = sizeof(COLOR_DATA_ARRAY)/sizeof(uint32_t);
  static uint32_t start_index = NUM_COLORS/2 - 1;

  // Set the color for each pixel.
  for ( uint16_t i=0; i < NUM_PIXELS; i++ ) {
    // Get the color value from the array.
    uint32_t color = COLOR_DATA_ARRAY[ (start_index + i) % NUM_COLORS ];
    // Set the color value of the i-th pixel.
    pixels.setPixelColor( i, color );
  }
  // Update the pixels.
  pixels.show(); 

  // Increment the start index by 1.
  start_index = (start_index+1) % NUM_COLORS;
  delay(500);
}
