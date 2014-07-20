#include <LPD8806.h>
#include <SPI.h>
#include <Servo.h>

// set up for the Laser Mount 
Servo servo1;  // bottom servo
Servo servo2;  // top servo
int angle1 = 0;  // bottom servo
int angle2 = 0;  // top servo
const int servoPin1 = 8;  // control pin for bottom servo
const int servoPin2 = 10;
const int feedServoPin = 9;

const int laserPin = 5;
const int motionDetectorPin = A0;

boolean isLaserTime = false;
boolean isTreatTime = false;

int laserTimer = 0;
int treatTimer = 0;

// setup code for the lights
// Number of RGB LEDs in strand:
int nLEDs = 12;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);

Servo feedServo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
int feedServoTimer = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  setupServo();
  
  // set up the LED strip
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();  
  strip.Color(0,0,0);

  // feed servo
  feedServo.attach(feedServoPin);
}

void setupServo(){
 
 servo1.attach(servoPin1);
 servo2.attach(servoPin2);  
  
}

// this is the code to move the laser in lines
int horizontalDistance = 60;
int verticalDistance = 60;


void moveLaser(){
  if (laserTimer < 30){
    digitalWrite(laserPin, HIGH);
    angle2 = 0;
    for (angle1 = 120; angle1  < 120 + horizontalDistance; angle1++){
      servo1.write(angle1);
      servo2.write(angle2);
      angle2++;
      delay(5); 
    }
  laserTimer++;    
    
  }else{
    Serial.println("Laser Time cutover: It is treat time!!");
     // now it is treat time
     digitalWrite(laserPin, LOW);
    isLaserTime = false;
    isTreatTime = true; 
  }
  Serial.println("laserTimer: ");
  Serial.println(laserTimer);

}

// the loop routine runs over and over again forever:
// for the demo, we will loop to proximity
// once triggered run the laser/servo program for 30 seconds
// then feed treat
void loop() {
  // read the input on analog pin 0:
  if (!isLaserTime && ! isTreatTime){
    // reset stuff:
    feedServo.write(90);
    strip.Color(0,0,0);   
  
    Serial.println("Waiting for Laser Time!");
   // then read the motion detector   
    int motionValue = analogRead(motionDetectorPin);
    // for testing:
    motionValue = 500;
    laserTimer = 0;
    if (motionValue > 300){
      isLaserTime = true;
      digitalWrite(laserPin, HIGH);
      moveLaser();
    }   
  }else if (isLaserTime && !isTreatTime){
      Serial.println("It is LASER time!!");
      digitalWrite(laserPin, HIGH);
      moveLaser();    
  }else if (!isLaserTime && isTreatTime){
      Serial.println("It is TREAT TIME!!");
      feedTreat();  
  }
}


// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}

void feedTreat(){
     // light the lights 
     Serial.println("It is treat time");
    // rainbow lights
    rainbowCycle(1);  // make it go through the cycle fairly fast
    strip.Color(0,0,0);   
    // move the treat feeder 
   feedServo.write(80);
  delay(30);
    feedServo.write(90);
    feedServoTimer = 0; 
     treatTimer = 0;
    isTreatTime = false; 
  strip.Color(0,0,0);    
 }
