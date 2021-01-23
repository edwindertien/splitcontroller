/*************************************************************
   Sketch for controlling a modified PS4 controller
   using two nunchucks or analog nunchuck equivalent
   (two hand-held controllers with two axis and two buttons)

   2 nunChucks (multiplexed) using code from WiiChuckDemo by
   2008 Tod E. Kurt, http://thingm.com/

   Controller is Arduino Due, 1k series resistors are used
   between PS4 buttons and Due outputs. All inputs are active-low
   except R1, L1 and the Z-axes.

   AnalogWrite is used to mimick joystick signals, RC low-pass
   filters (10uF, 1k) have been used to make the analog signals


   E13 - November 2020
   --

   Original nunchuck:  signal:    ext. cable    black nunchuck:
   white -             ground     orange        white
   red -               3.3+v      black         red
   green -             data  A4   blue          blue
   yellow -            clock A5   red           green

   PCA 9540 pinout
         _____
   SCL -|     |- SC1
   SDA -|     |- SD1
   VCC -|     |- GND
   SD0 -|_____|- SC0

   both the nunchucks and the PCA95040 can be powered from pins, 5V
   For I2C using internal pull-up is sufficient

   for the wired handheld controllers the following pinout
   has been used: on RJ45 connectors:

   1 analog_X
   2 analog_Y
   3 VCC
   4 GND (switch)
   5 GND (switch)
   6 GND (potentiometer)
   7 switch_1
   8 switch_2

 **************************************************************/
//#define NUNCHUCK (1)
#define DEBUG (1)

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel leftRing = Adafruit_NeoPixel(12, 53, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing = Adafruit_NeoPixel(12, 51, NEO_GRB + NEO_KHZ800);

const enum Device{ NUNCHUCK, PS4CONTR};

// Device is constant because the device won't change during use.
const Device dv = PS4CONTR;

// replace by ternary --> device enum
#ifdef NUNCHUCK
  #include <Wire.h>
  #include "nunchuck_funcs.h"

//  #define CENTER 127
//  #define MARGIN 16
#else
//  #define CENTER 127
//  #define MARGIN 50
#endif
//#define MIN 5
//#define MAX 130

// num of pins on Arduino. Here: Arduino DUE
const uint8_t numArduinoDigitPins = 54;
const uint8_t numArduinoAnalogIn = 12;
const uint8_t numArduinoAnalogOut = 2;

std::map<std::string,uint8_t> m_ps4pins;
m_ps4pins["joyRXax2"] = 2;     // Joystick R - X - axis 2
m_ps4pins["joyRYax1"] = 3;     // Joystick R - Y - axis 1
m_ps4pins["joyRZb11"] = 4;     // Joystick R - z - button 11 // hoog actief
m_ps4pins["joyLZb12"] = 7;     // Joystick L - z - button 12 // hoog actief
m_ps4pins["joyLYax4"] = 6;     // Joystick L - Y - axis 4
m_ps4pins["joyLXax3"] = 5;     // Joystick L - X - axis 3
m_ps4pins["sqbutton"] = 26;    // square button
m_ps4pins["circbutton"] = 28;  // circle button
m_ps4pins["psbutton"] = 30;    // PS
m_ps4pins["l2button"] = 32;    // L2
m_ps4pins["r2button"] = 34;    // R2
m_ps4pins["tributton"] = 36;   // triangle button
m_ps4pins["xbutton"] = 38;     // cross (X) button
m_ps4pins["lhat"] = 40;        // left - hat
m_ps4pins["rhat"] = 42;        // right - hat
m_ps4pins["uhat"] = 44;        // up - hat
m_ps4pins["dhat"] = 46;        // down - hat
m_ps4pins["r1button6"] = 48;   // R1   // button 6 // hoog actief
m_ps4pins["l1button5"] = 50;   // L1   // button 5 // hoog actief
m_ps4pins["optbutton"] =  52;  // options button
m_ps4pins["shrebutton"] = A8;   // share

std::map<std::string,uint8_t>::iterator it;

/*
std::map<char, int> m;
m.insert(std::make_pair('c', 0));  // c is for cookie

std::map<char, int>::iterator it = m.find('c');
if (it != m.end())
    it->second = 42;

*/

// is array
int ps4pins[21] = {2, // Joystick R - X - axis 2
                   3, // Joystick R - Y - axis 1
                   4, // Joystick R - z - button 11 // hoog actief
                   7, // Joystick L - z - button 12 // hoog actief
                   6, // Joystick L - Y - axis 4
                   5, // Joystick L - X - axis 3
                   26, // vierkant
                   28, // rondje
                   30, // PS
                   32, // L2
                   34, // R2
                   36, // driehoek
                   38, // Kruis (X)
                   40, // links  - hat
                   42, // rechts - hat
                   44, // boven  - hat
                   46, // bendeden - hat
                   48, // R1   // button 6 // hoog actief
                   50, // L1   // button 5 // hoog actief
                   52, // options
                   A8, // share
                  };
#define greenLED 13
#define redLED 12
unsigned long loopTime;
/*
// buffer for storing nunchuck or analog joystick data:
unsigned char accx[2];
unsigned char accy[2];
unsigned char c_button[2];
unsigned char z_button[2];
*/

// buffers for the coloured rings:
unsigned char Abuffer[12];
unsigned char Bbuffer[12];
unsigned char Cbuffer[12];
unsigned char Dbuffer[12];
boolean L1state, R1state, R2state, L2state, Rreleased;
int Astate, Areleased, Bstate, Breleased;

class Contr {
  public:
    const uint8_t CENTER;
    const uint8_t MARGIN;
    const uint8_t MIN = 5;
    const uint8_t MAX = 130;

    // buffer for storing nunchuck or analog joystick data:
    unsigned char accx[2];
    unsigned char accy[2];
    unsigned char c_button[2];
    unsigned char z_button[2];

    // default constructor
    Contr() {
      init();
    }

    // this one private? would make sense, except it is parent class
    void init() {


      pinMode(13, OUTPUT); // greenLED
      pinMode(9, OUTPUT);

      for (const auto& [key, pinnr] : m_ps4pins) {
        pinMode(pinnr, OUTPUT);
        digitalWrite(pinnr, HIGH);
      }



      digitalWrite(4, LOW); // hoog actief
      digitalWrite(7, LOW); // hoog actief
      digitalWrite(48, LOW);// hoog actief
      digitalWrite(50, LOW);// hoog actief

      analogWrite(2, 127);  // start with centervoltage
      analogWrite(3, 127);
      analogWrite(5, 127);
      analogWrite(6, 127);

      pinMode(A2, INPUT_PULLUP);  // for the switches.
      pinMode(A3, INPUT_PULLUP);
      pinMode(A6, INPUT_PULLUP);
      pinMode(A7, INPUT_PULLUP);

    #ifdef DEBUG
      Serial.println("started up");
    #endif
      leftRing.begin();
      rightRing.begin();
      for (int i = 0; i < 12; i++) {
        leftRing.setPixelColor(i, leftRing.Color(0, 0, 0));
        rightRing.setPixelColor(i, leftRing.Color(0, 0, 0));
        Abuffer[i] = MIN;
        Bbuffer[i] = MIN;
        Cbuffer[i] = MIN;
        Dbuffer[i] = MIN;
      }
      leftRing.show();
      rightRing.show();

    }
    void readInput() {
       //override by derived
    }
};

class Nunchuckcontr : public Contr {
  private:
    const uint8_t NUNCHUCK_X_OFFSET = 0;
    const uint8_t NUNCHUCK_Y_OFFSET = 0;
    //uint8_t pinLED;
  public:
      // make paramterized constructor
    Nunchuckctr(uint8_t pinLED) {
      CENTER = 127;
      MARGIN = 16;
      // constructor
      //this->pinLED = pinLED;
      //pinMode(pinLED, OUTPUT);
    }

  // destructor
    ~Nunchuckctr() {

    }
    // setup function
    void init() {
      Wire.setClock(10000) ;
      pinMode(20, INPUT_PULLUP);
      pinMode(21, INPUT_PULLUP);
      selectNunchuckChannel(0);
      nunchuck_init();  // send the initilization handshake
      selectNunchuckChannel(1);
      nunchuck_init();  // send the initilization handshake
    }

    // setup function
    void selectNunchuckChannel(int channel) {
      if (channel == 1) channel = 5;
      else channel = 4;
      Wire.begin();                // join i2c bus as master
      Wire.beginTransmission(0b1110000);// transmit to device 0x52
      Wire.write(channel);// sends memory address
      Wire.endTransmission();// stop transmitting
    }
    // loop function
    void readInput() {
      // read both nunchucks
      for (int n = 0; n < 2; n++) {
        selectNunchuckChannel(n);
        delay(10);
        if (nunchuck_get_data()) {
          accx[n] = (int)nunchuck_joyx() - NUNCHUCK_X_OFFSET;
          accy[n] = (int)nunchuck_joyy() - NUNCHUCK_Y_OFFSET;
          c_button[n] = (int)nunchuck_cbutton();
          z_button[n] = (int)nunchuck_zbutton();
        }
      }
    }

};

class Modps4contr : public Contr {
  private:
  public:

  Modps4contr {
    CENTER = 127;
    MARGIN = 50;

  }

  ~Mmodps4contr {

  }

  void init() {

  }

  void readInput() {
    accx[0] = map(analogRead(A5), 0, 832, 0, 255);
    accy[0] = map(analogRead(A4), 0, 832, 0, 255);
    accx[1] = map(analogRead(A1), 0, 832, 0, 255);
    accy[1] = map(analogRead(A0), 0, 832, 0, 255);
    c_button[0] = !digitalRead(A6);
    z_button[0] = !digitalRead(A7);
    c_button[1] = !digitalRead(A2);
    z_button[1] = !digitalRead(A3);
  }
};

void setup() {
#ifdef DEBUG
  Serial.begin(115200);   // debug info to usb serial
#endif
  delay(20);        // wait for the nunchuck to be powered up

  //(expression 1) ? expression 2 : expression 3
  // new obj nunchuck contr : new obj ps4contr
  (dv == NUNCHUCK) ? () : ();
  // call init()

} // end setup

void loop() {
  if (millis() > loopTime + 19) { // run at 10 Hz
    loopTime = millis();


///////// LINKS - NUNCHUCK FUNCTIONS ////
// GROEN


// links
    switch(Astate) {
      case 0: // groen

        break;
      case 1: // rood
        break;
      default: // blauw
    }

void lgroen() {}
    if (Astate == 0 ) {
      int directionA = -1;
      analogWrite(3, accx[0]); // axis 1
      analogWrite(2, 255-accy[0]); // axis 2
      // visualisation
      if (accx[0] > CENTER + MARGIN || accx[0] < CENTER - MARGIN || accy[0] > CENTER + MARGIN || accy[0] < CENTER - MARGIN)directionA = ((int)(6.5 * (3.1415 + atan2(accx[0] - CENTER, accy[0] - CENTER)) / 3.1415)); else directionA = -1;

      if (z_button[0]) digitalWrite(38, LOW); else digitalWrite(38, HIGH); // (X)

      for (int i = 0; i < 12; i++) {
        leftRing.setPixelColor(i, leftRing.Color(0, MIN, 0));
        if (i == directionA)leftRing.setPixelColor(i, leftRing.Color(0, MAX, 0));
      }
      leftRing.show();
    }
  }
// ROOD
void lrood() {
    else if (Astate == 1) {
      if (accx[0] > CENTER + MARGIN) {
        digitalWrite(52, LOW);  // options
        Abuffer[9] = MAX;
      }
      else {
        digitalWrite(52, HIGH);
        Abuffer[9] = MIN;
      }
      if (accx[0] < CENTER - MARGIN) {
        digitalWrite(A8, LOW);  // share
        Abuffer[3] = MAX;
      }
      else {
        digitalWrite(A8, HIGH);
        Abuffer[3] = MIN;
      }
      if (accy[0] > CENTER + MARGIN) {
        digitalWrite(30, LOW);  // PS
        Abuffer[6] = MAX;
      }
      else {
        digitalWrite(30, HIGH);
        Abuffer[6] = MIN;
      }
      if (accy[0] < CENTER - MARGIN) {
        digitalWrite(5, HIGH);  // Z-as
        Abuffer[0] = MAX;
      }
      else {
        digitalWrite(5, LOW);
        Abuffer[0] = MIN;
      }
      //visualisation
      for (int i = 0; i < 12; i++) {
        leftRing.setPixelColor(i, leftRing.Color(Abuffer[i], 0, 0));
      }
      leftRing.show();
    }
  }

  void lblauw() {
// BLAUW:
    else {
      if (accx[0] > CENTER + MARGIN) {
        digitalWrite(50, HIGH);  // vierkantje + L1
        digitalWrite(26, LOW);
        Abuffer[9] = MAX;
      }
      else {
        digitalWrite(50, LOW);
        digitalWrite(26, HIGH);
        Abuffer[9] = MIN;
      }
      if (accx[0] < CENTER - MARGIN) {
        digitalWrite(50, HIGH);  //
        Abuffer[3] = MAX;
      }
      else {
        digitalWrite(50, LOW);
        Abuffer[3] = MIN;
      }
      if (accy[0] > CENTER + MARGIN) {
        digitalWrite(26, LOW);  //
        Abuffer[6] = MAX;
      }
      else {
        digitalWrite(26, HIGH);
        Abuffer[6] = MIN;
      }
      if (accy[0] < CENTER - MARGIN) {
        digitalWrite(28, LOW);  //
        Abuffer[0] = MAX;
      }
      else {
        digitalWrite(28, HIGH);
        Abuffer[0] = MIN;
      }
      //visualisation
      for (int i = 0; i < 12; i++) {
        leftRing.setPixelColor(i, leftRing.Color(0, 0, Abuffer[i]));
      }
      leftRing.show();
    }
  }
 /////////RECHTS////////////////////////////
 // GROEN
    int directionB = -1;
    if (Bstate == 0 ) {

 //   analogWrite(6, 127);  // axis 3
 //    analogWrite(7, 127);  // axis 4

      if (accx[1] > (CENTER + 80)) analogWrite(6,accx[1]);
      else if (accx[1] <(CENTER - 80)) analogWrite(6,accx[1]);
      else analogWrite(6,127);

      if (accy[1] > (CENTER + 80)) analogWrite(7,accy[1]);
      else if (accy[1] <(CENTER - 80)) analogWrite(7,accy[1]);
      else analogWrite(7,127);



      if (accx[1] > CENTER + MARGIN || accx[1] < CENTER - MARGIN || accy[1] > CENTER + MARGIN || accy[1] < CENTER - MARGIN)directionB = ((int)(6.3 * (3.1415 + atan2(accx[1] - CENTER, accy[1] - CENTER)) / 3.1415)); else directionB = -1;
      if (c_button[1]) digitalWrite(26, LOW); else digitalWrite(26, HIGH); // (vierkant)
      for (int i = 0; i < 12; i++) {
        rightRing.setPixelColor(i, rightRing.Color(0, MIN, 0));
        if (i == directionB)rightRing.setPixelColor(i, rightRing.Color(0, MAX, 0));
      }
      rightRing.show();
    }
// ROOD
    else if (Bstate == 1) {

      if (c_button[1]) digitalWrite(34, LOW); else digitalWrite(34, HIGH); // (R2)


      if (accx[1] > CENTER + MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN) {
        digitalWrite(28, LOW);  // rondje
        Bbuffer[9] = MAX;
      }
      else {
        digitalWrite(28, HIGH);
        Bbuffer[9] = MIN;
      }
      if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN) {
        digitalWrite(48, HIGH);  // R1
        Bbuffer[7] = MAX;
        Bbuffer[8] = MAX;
      }
      else {
        digitalWrite(48, LOW);  // R1
        Bbuffer[7] = MIN;
        Bbuffer[8] = MIN;
      }
      if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN) {
        digitalWrite(50, HIGH);  // L1
        Bbuffer[4] = MAX;
        Bbuffer[5] = MAX;
      }
      else {
        digitalWrite(50, LOW);  // L1
        Bbuffer[5] = MIN;
        Bbuffer[4] = MIN;
      }
      if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN) {
        digitalWrite(34, LOW);  // R2
        Bbuffer[10] = MAX;
        Bbuffer[11] = MAX;
      }
      else {
        if(c_button[1]==0)digitalWrite(34, HIGH);  // R2
        Bbuffer[10] = MIN;
        Bbuffer[11] = MIN;
      }
      if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN) {
        digitalWrite(32, LOW);  // L2
        Bbuffer[1] = MAX;
        Bbuffer[2] = MAX;
      }
      else {
        digitalWrite(32, HIGH);  // L2
        Bbuffer[1] = MIN;
        Bbuffer[2] = MIN;
      }
      if (accx[1] < CENTER - MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN) {
        digitalWrite(26, LOW);  // vierkantje
        Bbuffer[3] = MAX;
      }
      else {
        digitalWrite(26, HIGH);
        Bbuffer[3] = MIN;
      }
      if (accy[1] > CENTER + MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN ) {
        digitalWrite(36, LOW);  // driehoekje
        Bbuffer[6] = MAX;
      }
      else {
        digitalWrite(36, HIGH);
        Bbuffer[6] = MIN;
      }
      if (accy[1] < CENTER - MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN ) {
        digitalWrite(38, LOW);  // kruisje
        Bbuffer[0] = MAX;
      }
      else {
        if(z_button[0]==0) digitalWrite(38, HIGH);
        Bbuffer[0] = MIN;
      }

      for (int i = 0; i < 12; i++) {
        rightRing.setPixelColor(i, rightRing.Color(Bbuffer[i], 0, 0));
      }
      rightRing.show();
    }
// BLAUW
    else {
      if (Rreleased == 0) {
        if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 0) {
          digitalWrite(48, HIGH);  // R1
          R1state = 1;
          Rreleased = 1;
          Bbuffer[7] = MAX;
          Bbuffer[8] = MAX;
        }

        else if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 1) {
          digitalWrite(48, LOW);
          R1state = 0;
          Rreleased = 1;
          Bbuffer[7] = MIN;
          Bbuffer[8] = MIN;
        }


        else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 0) {
          digitalWrite(50, HIGH); // L1
          L1state = 1;
          Rreleased = 1;
          Bbuffer[4] = MAX;
          Bbuffer[5] = MAX;
        }
        else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 1) {
          digitalWrite(50, LOW);
          L1state = 0;
          Rreleased = 1;
          Bbuffer[4] = MIN;
          Bbuffer[5] = MIN;
        }

        else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 0) {
          digitalWrite(34, LOW); // R2
          R2state = 1;
          Rreleased = 1;
          Bbuffer[10] = MAX;
          Bbuffer[11] = MAX;
        }
        else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 1) {
          digitalWrite(34, HIGH);
          R2state = 0;
          Rreleased = 1;
          Bbuffer[10] = MIN;
          Bbuffer[11] = MIN;
        }

        else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 0) {
          digitalWrite(32, LOW); // L2
          L2state = 1;
          Rreleased = 1;
          Bbuffer[1] = MAX;
          Bbuffer[2] = MAX;
        }
        else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 1) {
          digitalWrite(32, HIGH);
          L2state = 0;
          Rreleased = 1;
          Bbuffer[1] = MIN;
          Bbuffer[2] = MIN;
        }
      }
      if (accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN && accy[1] < CENTER + MARGIN && accy[1] > CENTER - MARGIN) Rreleased = 0;

      for (int i = 0; i < 12; i++) {
        rightRing.setPixelColor(i, rightRing.Color(0, 0, Bbuffer[i]));
      }
      rightRing.show();
    }
    ///// NUNCHUCK 1 button switch ///////
    if (Areleased == 0) {
      if (c_button[0] == 1 && Astate == 0) {
        Astate = 1;
      }
      else if (c_button[0] == 1 && Astate == 1) {
        Astate = 2;
      }
      else if (c_button[0] == 1 && Astate == 2) {
        Astate = 0;
      }
 /*     if (c_button[0] == 1 && Astate != 2 ) {
        Astate = 2;
      }
      else if (c_button[0] == 1 && Astate != 0) {
        Astate = 0;
      }*/
    }
    Areleased =  c_button[0];

    ///// NUNCHUCK 2 button switch ///////
    if (Breleased == 0) {
      if (z_button[1] == 1 && Bstate == 0) {
        Bstate = 1;
      }
      else if (z_button[1] == 1 && Bstate ==1) {
        Bstate = 2;
      }
      else if (z_button[1] == 1 && Bstate ==2) {
        Bstate = 0;
      }
      /*
      if (c_button[1] == 1 && Bstate != 2 ) {
        Bstate = 2;
      }
      else if (c_button[1] == 1 && Bstate != 0) {
        Bstate = 0;
      }*/
    }
    Breleased =  z_button[1];

#ifdef DEBUG
    // debug code to the USB serial
    Serial.print(accx[0]);
    Serial.print("\t");
    Serial.print(accy[0]);
    Serial.print("\t");
    Serial.print(accx[1]);
    Serial.print("\t");
    Serial.print(accy[1]);
    Serial.print("\t");
    Serial.print(z_button[0]);
    Serial.print("\t");
    Serial.print(z_button[1]);
    Serial.print("\t");
    Serial.print(c_button[0]);
    Serial.print("\t");
    Serial.println(c_button[1]);
#endif
  }
}
