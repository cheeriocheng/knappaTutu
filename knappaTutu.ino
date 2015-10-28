/*
Oct 27. based on neopixel example, button example, and acclerometer example
 
 1. breadboard circuit with accelerometer
 2. readings => light mapping
 3. do we need a button, or just a tap on acclerometer? 
 4. solder the board together on arduino micro
 5. tweak light sequence
 */

#include <Adafruit_NeoPixel.h>



#define NEOPIXEL_PIN 6
//#define BUTTON_PIN 3



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

int x, y, z ;
int minA = 300 ; 
int maxA = 0; 
int c ;

//ACCEL
const int numReadings = 8;
int readings[numReadings];  
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


//COLOR
float rainbowStep =0 ;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // setup button input pin

  // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0; //emperical . make convergence faster         
  //initiate accelerometer
  Serial.begin(9600);      // sets the serial port to 9600

    colorWipe(strip.Color(240, 0, 240), 15); // purple
  colorWipe(strip.Color(0, 0, 0), 15); // dark 

}

void loop() {

  //update button. swith between two states.

  //--- DANCE MODE ---
  if (true) {
    //read accelerometer data
    x = analogRead(0);       // read analog input pin 0
    y = analogRead(1);       // read analog input pin 1
    z = analogRead(2);       // read analog input pin 2

      // low pass filter
    //calculate the vector sum of all channels
    // subtract the last reading:
    total= total - readings[index];         
    // read from the sensor:  
    readings[index] = sqrt ( pow (x, 2) + pow (y, 2) + pow (z, 2))/4 ;
    //    readings[index] = x/4;
    // add the reading to the total:
    total= total + readings[index];       
    // advance to the next position in the array:  
    index = index + 1;                    
    // if we're at the end of the array...
    if (index >= numReadings)              
      // ...wrap around to the beginning: 
      index = 0;                           
    // calculate the average:
    average = total / numReadings;         

    if (average < minA) {
      minA = average ;
    } 
    else if (average > maxA) {
      maxA = average ;
    }

    c= average ;

    if (c < minA) {
      minA = c ;
    } 
    else if (c > maxA) {
      maxA = c ;
    }


    //TODO DON'T DO SERIAL 
    //  Serial.println(c) ;
    //    Serial.print(minA) ; //50
    //    Serial.print(" " ) ;
    //    Serial.print(maxA) ;// 260
    //    Serial.print(" " ) ;


    //map to color   

    if (average > maxA *.98){
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 100,100,100); 
      }
      strip.show();
      Serial.println("!!!") ;
    }
    else if (average > maxA *.93){
      rainbowStep += 5; 
      Serial.println("fast") ;
    }
    else {
      rainbowStep += 0.05;  //the integer increament every 5 cycles..
      Serial.println("slow") ;
    }

    if (rainbowStep >255 ){
      rainbowStep = 0 ;
    }

    rainbowByStep(rainbowStep);

    strip.show();
    delay (2);

  }

  //---  DEMO MODE ---
  else
  {

    // Some example procedures showing how to display to the pixels:
    //  colorWipe(strip.Color(255, 0, 0), 50); // Red
    //  colorWipe(strip.Color(0, 255, 0), 50); // Green
    //  colorWipe(strip.Color(0, 0, 255), 50); // Blue
    //  // Send a theater pixel chase in...
    // theaterChase(strip.Color(127, 127, 127), 50); // White
    //  theaterChase(strip.Color(127,   0,   0), 50); // Red
    //  theaterChase(strip.Color(  0,   0, 127), 50); // Blue
    rainbow(20);
    //    rainbowCycle(20);
    //    theaterChaseRainbow(5); 
  }




}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}



void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowByStep(float rStep){

  int j = (int)rStep %256;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

void solid (uint32_t c){
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(c) );

  } 
}
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}







