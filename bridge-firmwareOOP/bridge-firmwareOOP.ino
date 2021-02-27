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
#include <ArduinoSTL.h>

Adafruit_NeoPixel leftRing = Adafruit_NeoPixel(12, 53, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing = Adafruit_NeoPixel(12, 51, NEO_GRB + NEO_KHZ800);

const enum Device {NUNCHUCK, PS4CONTR};

const enum Mode {GREEN = 0, RED = 1, BLUE = 2};

// Device is constant because the device won't change during use.
const Device dv = PS4CONTR;

// replace by ternary --> device enum as declared above
#ifdef NUNCHUCK
  #include <Wire.h>
  #include "nunchuck_functions.h"

#endif

const uint8_t greenLED = 13;
const uint8_t redLED = 12;

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

// nunchuck or analog joystick
std::map<std::string,string> m_analogpins;
m_analogpins["accx1"] = "A5";
m_analogpins["accy1"] = "A4";
m_analogpins["accx2"] = "A1";
m_analogpins["accy2"] = "A0";
m_analogpins["c_button1"] = "A6";
m_analogpins["z_button1"] = "A7";
m_analogpins["c_button2"] = "A2";
m_analogpins["z_button2"] = "A3";

std::map<std::string,string> m_PWMpins;
m_PWMpins["PWM1"] = 2;
m_PWMpins["PWM2"] = 3;
m_PWMpins["PWM3"] = 5;
m_PWMpins["PWM4"] = 6;
m_PWMpins["PWM5"] = 7;

unsigned long loopTime;

// buffers for the coloured rings:
unsigned char Abuffer[12];
unsigned char Bbuffer[12];
unsigned char Cbuffer[12];
unsigned char Dbuffer[12];
boolean L1state, R1state, R2state, L2state, Rreleased;
int LeftReleased, RightReleased;

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

    // init modes
    Mode LeftMode = RED;
    Mode RightMode = RED;

    // default constructor
    Contr() {
      init();
    }

    // this one private? would make sense, except it is parent class
    void init() {
      pinMode(m_ps4pins["greenLED"], OUTPUT); // greenLED 13
      pinMode(9, OUTPUT);

      for (const auto& [key, pinnr] : m_ps4pins) {
        pinMode(pinnr, OUTPUT);
        digitalWrite(pinnr, HIGH);
      }

      digitalWrite(m_ps4pins["joyRZb11"], LOW); // hoog actief   4
      digitalWrite(m_ps4pins["joyLZb12"], LOW); // hoog actief 7
      digitalWrite(m_ps4pins["r1button6"], LOW);// hoog actief  48
      digitalWrite(m_ps4pins["l1button5"], LOW);// hoog actief  50

      analogWrite(m_PWMpins["PWM1"], 127);  // start with centervoltage
      analogWrite(m_PWMpins["PWM2"], 127);
      analogWrite(m_PWMpins["PWM3"], 127);
      analogWrite(m_PWMpins["PWM4"], 127);

      pinMode(m_analogpins["c_button2"], INPUT_PULLUP);  // for the switches.
      pinMode(m_analogpins["z_button2"], INPUT_PULLUP);
      pinMode(m_analogpins["c_button1"], INPUT_PULLUP);
      pinMode(m_analogpins["z_button1"], INPUT_PULLUP);

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
  public:
      // make paramterized constructor
    Nunchuckcontr(uint8_t pinLED) {
      CENTER = 127;
      MARGIN = 16;
      // constructor
      //this->pinLED = pinLED;
      //pinMode(pinLED, OUTPUT);
    }

  // destructor
    ~Nunchuckcontr() {

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

  void readAnalogInput() {
    accx[0] = map(analogRead(m_analogpins["accx1"]), 0, 832, 0, 255);
    accy[0] = map(analogRead(m_analogpins["accy1"]), 0, 832, 0, 255);
    accx[1] = map(analogRead(m_analogpins["accx2"]), 0, 832, 0, 255);
    accy[1] = map(analogRead(m_analogpins["accy2"]), 0, 832, 0, 255);
    c_button[0] = !digitalRead(m_analogpins["c_button1"]);
    z_button[0] = !digitalRead(m_analogpins["z_button1"]);
    c_button[1] = !digitalRead(m_analogpins["c_button2"]);
    z_button[1] = !digitalRead(m_analogpins["z_button2"]);
  }
};

// method has optional argument c to check whether a
// exceeds value b plus optional margin so on positive side of defined margin
bool is_above_margin(int a, int b = CENTER, int c = MARGIN) {
  return (a > (b + c));
}
// method has optional argument c to check whether a
// exceeds value b minus optional margin so on negative side of defined margin
bool is_below_margin(int a, int b = CENTER, int c = MARGIN)) {
  return (a < (b - c));
}

bool exceeds_neutral_range(int a, int b = CENTER, int c = MARGIN)) {
  return (a < (b - c) || a > (b + c));
}

// setup is like int main()
void setup() {
#ifdef DEBUG
  Serial.begin(115200);   // debug info to usb serial
#endif
  delay(20);        // wait for the nunchuck to be powered up

  //(expression 1) ? expression 2 : expression 3
  // new obj nunchuck contr : new obj ps4contr
  (dv == NUNCHUCK) ? () : ();
  // call init()

}

void loop() {
  if (millis() > loopTime + 19) { // run at 10 Hz
    loopTime = millis();


    }
switch(LeftMode) {
  case GREEN:
    int directionA = -1;
    analogWrite(m_ps4pins["joyRYax1"], accx[0]); // axis 1
    analogWrite(m_ps4pins["joyRXax2"], 255-accy[0]); // axis 2
    // visualisation

    if(exceeds_neutral_range(accx[0]) || exceeds_neutral_range(accy[0])) {
      directionA = ((int)(6.5 * (3.1415 + atan2(accx[0] - CENTER, accy[0] - CENTER)) / 3.1415));
    }

    if (z_button[0]) digitalWrite(m_ps4pins["xbutton"], LOW);
    else digitalWrite(m_ps4pins["xbutton"], HIGH); // (X)

    for (int i = 0; i < 12; i++) {
      leftRing.setPixelColor(i, leftRing.Color(0, MIN, 0));
      if (i == directionA)leftRing.setPixelColor(i, leftRing.Color(0, MAX, 0));
    }
    leftRing.show();
    break;
  case RED:
    int writebuffer;
    int writeout;
    //(m > n) ? "m is bigger" : "n is bigger";
    //(is_above_margin(accx[0])) ? (low and max) : (high and min);
    // check possibilities of enum and bitwise operands (high/low ~ MIN max)
    (is_above_margin(accx[0])) ? (writeout  = LOW; writebuffer = MAX) : (writeout  = HIGH; writebuffer = MIN);
    if (accx[0] > CENTER + MARGIN) {
      digitalWrite(m_ps4pins["optbutton"], LOW);  // options
      Abuffer[9] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["optbutton"] , HIGH);
      Abuffer[9] = MIN;
    }
    if (accx[0] < CENTER - MARGIN) {
      digitalWrite(m_ps4pins["shrebutton"], LOW);  // share
      Abuffer[3] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["shrebutton"], HIGH);
      Abuffer[3] = MIN;
    }
    if (accy[0] > CENTER + MARGIN) {
      digitalWrite(m_ps4pins["psbutton"], LOW);  // PS
      Abuffer[6] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["psbutton"], HIGH);
      Abuffer[6] = MIN;
    }
    if (accy[0] < CENTER - MARGIN) {
      digitalWrite(m_ps4pins["joyLXax3"] , HIGH);  // Z-as  ?? pin 5 = joyLXax3
      Abuffer[0] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["joyLXax3"], LOW);
      Abuffer[0] = MIN;
    }
    //visualisation
    for (int i = 0; i < 12; i++) {
      leftRing.setPixelColor(i, leftRing.Color(Abuffer[i], 0, 0));
    }
    leftRing.show();
    break;
  case BLUE:
    if (accx[0] > CENTER + MARGIN) {
      digitalWrite(m_ps4pins["l1button5"], HIGH);  // vierkantje + L1
      digitalWrite(26, LOW);
      Abuffer[9] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["l1button5"], LOW);
      digitalWrite(m_ps4pins["sqbutton"], HIGH);
      Abuffer[9] = MIN;
    }
    if (accx[0] < CENTER - MARGIN) {
      digitalWrite(m_ps4pins["l1button5"], HIGH);  //
      Abuffer[3] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["l1button5"], LOW);
      Abuffer[3] = MIN;
    }
    if (accy[0] > CENTER + MARGIN) {
      digitalWrite(m_ps4pins["sqbutton"], LOW);  //
      Abuffer[6] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["sqbutton"], HIGH);
      Abuffer[6] = MIN;
    }
    if (accy[0] < CENTER - MARGIN) {
      digitalWrite(m_ps4pins["circbutton"], LOW);  //
      Abuffer[0] = MAX;
    }
    else {
      digitalWrite(m_ps4pins["circbutton"], HIGH);
      Abuffer[0] = MIN;
    }
    //visualisation
    for (int i = 0; i < 12; i++) {
      leftRing.setPixelColor(i, leftRing.Color(0, 0, Abuffer[i]));
    }
    leftRing.show();
    break;
}

int directionB = -1;

switch(LeftMode) {
  case GREEN:
    //   analogWrite(6, 127);  // axis 3
    //    analogWrite(7, 127);  // axis 4
   if (accx[1] > (CENTER + 80)) analogWrite(6,accx[1]);
   else if (accx[1] <(CENTER - 80)) analogWrite(6,accx[1]);
   else analogWrite(m_PWMpins["PWM4"],127);
   if (accy[1] > (CENTER + 80)) analogWrite(7,accy[1]);
   else if (accy[1] <(CENTER - 80)) analogWrite(7,accy[1]);
   else analogWrite(m_PWMpins["PWM5"],127);
   if (accx[1] > CENTER + MARGIN || accx[1] < CENTER - MARGIN || accy[1] > CENTER + MARGIN || accy[1] < CENTER - MARGIN)directionB = ((int)(6.3 * (3.1415 + atan2(accx[1] - CENTER, accy[1] - CENTER)) / 3.1415)); else directionB = -1;
   if (c_button[1]) digitalWrite(26, LOW); else digitalWrite(26, HIGH); // (vierkant)
   for (int i = 0; i < 12; i++) {
     rightRing.setPixelColor(i, rightRing.Color(0, MIN, 0));
     if (i == directionB)rightRing.setPixelColor(i, rightRing.Color(0, MAX, 0));
   }
   rightRing.show();
   break;
 case RED:
   if (c_button[1]) digitalWrite(m_ps4pins["r2button"], LOW); else digitalWrite(m_ps4pins["r2button"], HIGH); // (R2)

   if (accx[1] > CENTER + MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN) {
     digitalWrite(m_ps4pins["circbutton"], LOW);  // rondje
     Bbuffer[9] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["circbutton"], HIGH);
     Bbuffer[9] = MIN;
   }
   if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN) {
     digitalWrite(m_ps4pins["r1button6"] , HIGH);  // R1
     Bbuffer[7] = MAX;
     Bbuffer[8] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["r1button6"], LOW);  // R1
     Bbuffer[7] = MIN;
     Bbuffer[8] = MIN;
   }
   if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN) {
     digitalWrite(m_ps4pins["l1button5"], HIGH);  // L1
     Bbuffer[4] = MAX;
     Bbuffer[5] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["l1button5"], LOW);  // L1
     Bbuffer[5] = MIN;
     Bbuffer[4] = MIN;
   }
   if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN) {
     digitalWrite(m_ps4pins["r2button"], LOW);  // R2
     Bbuffer[10] = MAX;
     Bbuffer[11] = MAX;
   }
   else {
     if(c_button[1]==0)digitalWrite(34, HIGH);  // R2
     Bbuffer[10] = MIN;
     Bbuffer[11] = MIN;
   }
   if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN) {
     digitalWrite(m_ps4pins["l2button"], LOW);  // L2
     Bbuffer[1] = MAX;
     Bbuffer[2] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["l2button"], HIGH);  // L2
     Bbuffer[1] = MIN;
     Bbuffer[2] = MIN;
   }
   if (accx[1] < CENTER - MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN) {
     digitalWrite(m_ps4pins["sqbutton"], LOW);  // vierkantje
     Bbuffer[3] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["sqbutton"], HIGH);
     Bbuffer[3] = MIN;
   }
   if (accy[1] > CENTER + MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN ) {
     digitalWrite(m_ps4pins["tributton"], LOW);  // driehoekje
     Bbuffer[6] = MAX;
   }
   else {
     digitalWrite(m_ps4pins["tributton"], HIGH);
     Bbuffer[6] = MIN;
   }
   if (accy[1] < CENTER - MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN ) {
     digitalWrite(m_ps4pins["xbutton"], LOW);  // kruisje
     Bbuffer[0] = MAX;
   }
   else {
     if(z_button[0]==0) digitalWrite(m_ps4pins["xbutton"], HIGH);
     Bbuffer[0] = MIN;
   }

   for (int i = 0; i < 12; i++) {
     rightRing.setPixelColor(i, rightRing.Color(Bbuffer[i], 0, 0));
   }
   rightRing.show();
   break;
 case BLUE:
   if (Rreleased == 0) {
     if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 0) {
       digitalWrite(m_ps4pins["r1button6"], HIGH);  // R1
       R1state = 1;
       Rreleased = 1;
       Bbuffer[7] = MAX;
       Bbuffer[8] = MAX;
     }

     else if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 1) {
       digitalWrite(m_ps4pins["r1button6"], LOW);
       R1state = 0;
       Rreleased = 1;
       Bbuffer[7] = MIN;
       Bbuffer[8] = MIN;
     }


     else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 0) {
       digitalWrite(m_ps4pins["l1button5"] , HIGH); // L1
       L1state = 1;
       Rreleased = 1;
       Bbuffer[4] = MAX;
       Bbuffer[5] = MAX;
     }
     else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 1) {
       digitalWrite(m_ps4pins["l1button5"], LOW);
       L1state = 0;
       Rreleased = 1;
       Bbuffer[4] = MIN;
       Bbuffer[5] = MIN;
     }

     else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 0) {
       digitalWrite(m_ps4pins["r2button"], LOW); // R2
       R2state = 1;
       Rreleased = 1;
       Bbuffer[10] = MAX;
       Bbuffer[11] = MAX;
     }
     else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 1) {
       digitalWrite(m_ps4pins["r2button"], HIGH);
       R2state = 0;
       Rreleased = 1;
       Bbuffer[10] = MIN;
       Bbuffer[11] = MIN;
     }

     else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 0) {
       digitalWrite(m_ps4pins["l2button"], LOW); // L2
       L2state = 1;
       Rreleased = 1;
       Bbuffer[1] = MAX;
       Bbuffer[2] = MAX;
     }
     else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 1) {
       digitalWrite(m_ps4pins["l2button"], HIGH);
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
   break;
}


    ///// NUNCHUCK 1 button switch ///////
    if (LeftReleased == 0) {
      if (c_button[0] == 1 && LeftMode == GREEN) {
        LeftMode = RED;
      }
      else if (c_button[0] == 1 && LeftMode == RED) {
        LeftMode = BLUE;
      }
      else if (c_button[0] == 1 && LeftMode == BLUE) {
        LeftMode = GREEN;
      }
 /*     if (c_button[0] == 1 && LeftMode != 2 ) {
        LeftMode = BLUE;
      }
      else if (c_button[0] == 1 && LeftMode != 0) {
        LeftMode = GREEN;
      }*/
    }
    LeftReleased =  c_button[0];

    ///// NUNCHUCK 2 button switch ///////
    if (RightReleased == 0) {
      if (z_button[1] == 1 && RightMode == 0) {
        RightMode = RED;
      }
      else if (z_button[1] == 1 && RightMode ==1) {
        RightMode = BLUE;
      }
      else if (z_button[1] == 1 && RightMode ==2) {
        RightMode = GREEN;
      }
      /*
      if (c_button[1] == 1 && RightMode != 2 ) {
        RightMode = BLUE;
      }
      else if (c_button[1] == 1 && RightMode != 0) {
        RightMode = GREEN;
      }*/
    }
    RightReleased =  z_button[1];

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
