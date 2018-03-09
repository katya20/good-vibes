#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

Adafruit_MMA8451 mma = Adafruit_MMA8451();

// hardware pins
const int ledPin = 3;            // output pin for LED
const int vibePin = 11;           // output pin for VIBE
const int fivev = 4;

// knobs
const int howLongToWaitLED = 30;  // how many millis() to wait between brightness changes
int fadeAmount = 3;               // how much to bump LED each change (note, changes sign)

// internal states
int brightness = 0;               // current LED brightness
int button1State = 0;             // variable for reading the pushbutton status
int button2State = 0;
int lastTimeItHappenedLED = 0;    // time in millis() since last LED action
int lastTimeItHappenedVIBE = 0;   // time in millis() since last VIBE action
int howLongItsBeen;               // temp calculated value

// setup all hardware
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(vibePin, OUTPUT);
  pinMode(fivev, OUTPUT);
  Serial.begin(9600);

// setup for accelerometer: 
  Serial.println("Adafruit MMA8451 test!");
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!"); 
  mma.setRange(MMA8451_RANGE_2_G);  
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
  
}

// for accelerometer:
int n = 0;
int pos_1 = n;
int pos_2 = n;
int state = 0;

void loop() {
//  digitalWrite(plzworkled, HIGH);
  digitalWrite(vibePin, HIGH);

// read accelerometer
  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);
// get orientation
  uint8_t o = mma.getOrientation();

// accelerometer postition states
  switch (o) {
      case MMA8451_PL_PUF: 
        n = 1;
        break;
      case MMA8451_PL_PUB: 
        n = 2;
        break;    
      case MMA8451_PL_PDF: 
        n = 1;
        break;
      case MMA8451_PL_PDB: 
        n = 2;
        break;
      case MMA8451_PL_LRF: 
        n = 1;
        break;
      case MMA8451_PL_LRB: 
        n = 2;
        break;
      case MMA8451_PL_LLF: 
        n = 1;
        break;
      case MMA8451_PL_LLB: 
        n = 2;
        break;
      }

// save previous positions
  int pos_0 = pos_1;
  pos_1 = pos_2;
  pos_2 = n;

// sense motion
  if (pos_2 == pos_0 && pos_2 != pos_1) {
    Serial.println("CHANGE STATE!");
    state += 1;
    if (state == 3) {
      state = 0;
    }
  }
  

// ***** vibe = vibe only *****
  if (state == 1) {
//  digitalWrite(plzworkled, HIGH);
  Serial.println("vibeonly");
  vibe();
}

// ***** both = led and vibe *****
  else if (state == 2) {
   Serial.println("both");
   led();
   vibe();
 }

// ***** none = all off *****
  else {
    Serial.println("off");
    // boring mode, no vibeing
    digitalWrite(vibePin, LOW);
    digitalWrite(ledPin, LOW);
//    digitalWrite(plzworkled, LOW);
  }

//  Serial.println(n);
  delay (400);
}

void led() {
    howLongItsBeen = millis() - lastTimeItHappenedLED;

    // time do adjust LED?
    if (howLongItsBeen >= howLongToWaitLED) {
      brightness = brightness + fadeAmount;
      // did we either top or bottom out? if so, reverse direction
      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      // make sure we don't go over the 255 or 0, or we'll glitch for a moment
      brightness = min(brightness,255);
      brightness = max(brightness,0);
      lastTimeItHappenedLED = millis();
//    } else {
//      brightness = 0;
    }
    // set the new brightness
    analogWrite(ledPin, brightness);
    howLongItsBeen = millis() - lastTimeItHappenedVIBE;
    Serial.println("led");
  (vibePin, HIGH);
}

void vibe() {
howLongItsBeen = millis() - lastTimeItHappenedVIBE;

 // make the pin turn on and off in the heartbeat pattern
 if (howLongItsBeen < 80) {
   digitalWrite(vibePin, HIGH);
 }
 else {
   if (howLongItsBeen < 280) {
     digitalWrite(vibePin, LOW);
   }
   else {
     if (howLongItsBeen < 360) {
       digitalWrite(vibePin, HIGH);
     }
     else {
       if (howLongItsBeen < 1060) {
         digitalWrite(vibePin, LOW);
       }
       else {
         // vibe cycle done, so cycle back to start
         lastTimeItHappenedVIBE = millis();
       }
     }
   }
 }
}

