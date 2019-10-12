#include "FastLED.h"

// Arduino Uno + Spectrum Shield
// LED Music Visualizer v0.2 

// Designed and Developed by Aaron Soler
// Resources used
// FastLED https://github.com/FastLED/FastLED/wiki/Overview
// Sparkfun Spectrum Shield https://learn.sparkfun.com/tutorials/spectrum-shield-hookup-guide/all

// This was the first time I ever coded in C or C++
// Thanks to this guy I was inspired to create something like his but on a bigger scale
// https://youtu.be/Af74A0Tx-wA
// https://github.com/the-red-team/Arduino-FastLED-Music-Visualizer



// Adjust config to suit your needs

// CONFIG START ********************************************************************************************

// LED SETUP
#define NUM_LEDS    300     // Number of LEDS per strip
#define BRIGHTNESS  255     // Max brightness = 255
#define PIN_0       9       // PIN 9
#define PIN_1       10      // PIN 10
#define LED_TYPE    WS2812B // LED chipset
#define COLOR_ORDER RGB     
#define HUE_SPEED   1.5       // Rainbow wave speed
#define HUE_SPREAD  2       // Compression of rainbow

CRGB strip[NUM_LEDS];       // LED Strip array

// AUDIO SETUP
#define STROBE  4           // STROBE
#define RESET   5           // RESET
#define DC_One  A0          // Audio from first MSGEQ7
#define DC_Two  A1          // Audio from second MSGEQ7
#define BASS    0           // Bass band

int Frequencies_One[7];
int Frequencies_Two[7]; 
int band;
int audio_input = 0;
long react = 0;             // NUMBER OF LEDs BEING LIT
long pre_react = 0;
long pos_react = 0;

// STANDARD VARIABLES
int hue = 0;
int led;

// CONFIG END **********************************************************************************************

void setup() {

  // SPECTRUM SETUP
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(DC_One, INPUT);
  pinMode(DC_Two, INPUT);  
  digitalWrite(STROBE, HIGH);
  digitalWrite(RESET, HIGH);

  // LED LIGHTING SETUP
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, PIN_0, COLOR_ORDER>(strip, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_1, COLOR_ORDER>(strip, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // CLEAR LEDS
  for (int led = 0; led < NUM_LEDS; led++){
    strip[led] = CRGB(0, 0, 0);
  }
  FastLED.show();

  // SERIAL AND INPUT SETUP
  Serial.begin(115200);
  Serial.println("\nListening...");
}

void loop() {
  readMSGEQ7();
  audio();
  rainbowWave();
}

// CUSTOM METHODS ******************************************************************************************

// Function to read 7 band equalizers
void readMSGEQ7()
{
  digitalWrite(RESET, HIGH);
  digitalWrite(RESET, LOW);
  for(band = 0; band < 7; band++)
  {
    digitalWrite(STROBE, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    Frequencies_One[band] = analogRead(DC_One); // store left band reading
    Frequencies_Two[band] = analogRead(DC_Two); // ... and the right
    digitalWrite(STROBE, HIGH); 
  }
}

void audio()
{
  if (Frequencies_One[BASS] > Frequencies_Two[BASS]){
    audio_input = Frequencies_One[BASS];
  } else {
    audio_input = Frequencies_Two[BASS];
  }

  pre_react = ((long)NUM_LEDS * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

  if (pre_react < 50) {
    pre_react = 0;
  }

  Serial.println(pre_react);
}

void rainbowWave()
{
  hueDelta();

  // This flips the animation and adds decay onto 
  if (pre_react > pos_react) {
    pos_react = pre_react;
  } else {
    pos_react-=4;
  }
  
  for (led = 0; led < NUM_LEDS; led++) {
    if (led < pos_react) {
      //rainbow wave
      //strip[led].setHue((hue) + (led * HUE_SPREAD));
      //hue wave
      strip[led].setHue(hue);
      //light blue
      //strip[led].setHue(180/(360/255));
    } else {
      strip[led].setRGB( 0, 0, 0);
    }
  }
  FastLED.show();
}

void hueDelta()
{
  hue += HUE_SPEED;
  if (hue >= 255) {
    hue = 0;
  }
}
