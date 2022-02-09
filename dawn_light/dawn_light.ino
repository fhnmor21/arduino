#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 8

#define minTempK 1000
#define maxTempK 6500

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


void setup() {
  // debug on serial terminal
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  
  // Setting up neo pixel
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

}


void loop() {
  tmElements_t tm;

  uint16_t minutes = 0;
  if (RTC.read(tm)) {
    minutes = tm.Hour * 60 + tm.Minute;
  }

  uint16_t K = 0;
  if (minutes > 240 && minutes < 420){
    K = 1000 + 42*(minutes - 240);
  }

  if (minutes > 1080 && minutes < 1320){
    K = 6000 + 25*(1080 - minutes);
  }

  if (K > 1000 && K < 6500) {
    uint32_t colorRGB = K2RGB(K);
    uint8_t brightness = 64*((float)K/maxTempK);
    colorWipe(colorRGB, brightness, 250);
  } else {
    colorWipe(0, 0, 250);
  }
  
  
//  Serial.write("Hour = ");
//  Serial.println(tm.Hour);
//  Serial.write("minutes = ");
//  Serial.println(minutes);
  
  delay(5000);  
}


// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, uint8_t brightness, int wait) {
  strip.setBrightness(brightness);
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}


// Clamps a value between 2 limits
float biclamp(float value, float lower, float upper){
  if (value < lower)
   return lower;

  if (value > upper)
    return upper;

  return value;
}


// Convert colorTemperature in K to RGB
// reference: https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
uint32_t K2RGB(uint16_t ctK){
  // clamping upper and lower bounds then scaling
  if (ctK < 1000) ctK = 1000;
  if (ctK > 40000) ctK = 40000;
  uint32_t small_ctK = ctK / 100;

  // Red calculations
  uint8_t red   = 0;
  if (small_ctK <= 66){
    red = 255;
  } else {
    float tmp_red = 329.698727446 * pow(small_ctK - 60, -0.1332047592);
    red = (uint8_t) biclamp(tmp_red, 0.0, 255.0);
  }

  // Green calculations
  uint8_t green = 0;
  if (small_ctK <= 66){
    float tmp_green = 99.4708025861 * log(small_ctK) - 161.1195681661;
    green = (uint8_t) biclamp(tmp_green, 0.0, 255.0);
  } else {
    float tmp_green = 288.1221695283 * pow(small_ctK - 60, -0.0755148492);
    green = (uint8_t) biclamp(tmp_green, 0.0, 255.0);
  }

  // Blue calculations
  uint8_t blue  = 0;
  if (small_ctK >= 66){
    blue = 255;
  } else 
  if (small_ctK <= 19){
    blue  = 0;
  } else {
    float tmp_blue = 138.5177312231 * log(small_ctK - 10) - 305.0447927307;
    blue = (uint8_t) biclamp(tmp_blue, 0.0, 255.0);
  }

//  Serial.write("ColorK = ");
//  Serial.println(ctK);
//  Serial.write("Red = ");
//  Serial.println(red);
//  Serial.write("Green = ");
//  Serial.println(green);
//  Serial.write("Blue = ");
//  Serial.println(blue);
    
  return strip.Color(red, green, blue);
}

//
//// color temperature range test
//int minTempK = 1000;
//int maxTempK = 6500;
//void testColorK(){
//  for(int k = minTempK; k <= maxTempK; k=k+200) {
//    uint32_t colorRGB = K2RGB(k);
//    uint8_t brightness = 64*((float)k/maxTempK);
////    Serial.write("Brightness = ");
////    Serial.println(brightness);
////    Serial.write("\n");
//    colorWipe(colorRGB, brightness, 100);
//    delay(500); 
//  }
//}
