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

 //  arduino-cli compile --fqbn arduino:sam:arduino_due_x_dbg bridge-firmwareOOP &>> debuglog.txt
// arduino-cli upload -p /dev/ttyACM0 -b arduino:sam:arduino_due_x_dbg -i <sketchname>.arduino.sam.arduino_due_x_dbg.bin
//arduino-cli compile  --fqbn arduino:sam:arduino_due_x_dbg bridge-firmwareOOP
//arduino-cli compile --fqbn arduino:sam:arduino_due_x bridge-firmwareOOP

//#define NUNCHUCK (1)
//#define DEBUG (1)

#include <Adafruit_NeoPixel.h>
#include <ArduinoSTL.h>
#include "NunchuckFunctions.h"

#include <iostream>
#include <map>

// define 1 if nunchuck, 0 if PS4 mod
#define NUNCHCK 1

Adafruit_NeoPixel leftRing = Adafruit_NeoPixel(12, 53, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing = Adafruit_NeoPixel(12, 51, NEO_GRB + NEO_KHZ800);

NunchuckFunctions nunchuckFunctions;

enum Device {NUNCHUCK, PS4CONTR};

const Device dv;

#if NUNCHCK > 0
  #include "NunchuckFunctions.h"
  dv = NUNCHUCK;
#else
  dv = PS4CONTR;
#endif

enum Mode {GREEN, RED, BLUE};

const uint8_t greenLED = 13;
const uint8_t redLED = 12;

std::map<std::string,uint8_t> m_ps4pins =
{
  {"joyRXax2", 2},
  {"joyRYax1", 3},
  {"joyRZb11", 4},
  {"joyLZb12", 7},
  {"joyLYax4", 6},
  {"joyLXax3", 5},
  {"sqbutton", 26},
  {"circbutton", 28},
  {"psbutton", 30},
  {"l2button", 32},
  {"r2button", 34},
  {"tributton", 36},
  {"xbutton", 38},
  {"lhat", 40},
  {"rhat", 42},
  {"uhat", 44},
  {"dhat", 46},
  {"r1button6", 48},
  {"l1button5", 50},
  {"optbutton",  52}
};

// nunchuck or analog joystick
std::map<std::string,uint8_t> m_analogpins =
{
  {"accx1", A5},
  {"accy1", A4},
  {"accx2", A1},
  {"accy2", A0},
  {"c_button1", A6},
  {"z_button1", A7},
  {"c_button2", A2},
  {"z_button2", A3},
  {"shrebutton", A8}
};

// joy out X, Y, Z
std::map<std::string,uint8_t> m_PWMpins =
{
  {"PWM1", 2},
  {"PWM2", 3},
  {"PWM3", 5},
  {"PWM4", 6},
  {"PWM5", 7}
};

unsigned long loopTime;

// buffers for the coloured rings:
unsigned char Abuffer[12];
unsigned char Bbuffer[12];
unsigned char Cbuffer[12];
unsigned char Dbuffer[12];
boolean L1state, R1state, R2state, L2state, Rreleased;
boolean LeftReleased, RightReleased;

class Contr {
  protected:
    static uint8_t CENTER;// = 127; // just a default value, will be overridden by derived class
    static uint8_t MARGIN; //= 50; // just a default value, will be overridden by derived class
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

    // method has optional argument c to check whether a
    // exceeds value b plus optional margin so on positive side of defined margin
    bool is_above_margin(int a, int center = CENTER, int margin = MARGIN) {
      return (a > (center + margin));
    }
    // method has optional argument c to check whether a
    // exceeds value b minus optional margin so on negative side of defined margin
    bool is_below_margin(int a, int center = CENTER, int margin = MARGIN) {
      return (a < (center - margin));
    }

    bool exceeds_neutral_range(int a, int center = CENTER, int margin = MARGIN) {
      return (a < (center - margin) || a > (center + margin));
    }
public:
    void operate() {
      int writebuffer;
      int writeout;
      int writeout1;
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
          (is_above_margin(accx[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4pins["optbutton"], writeout);
          Abuffer[9] = writebuffer;

          (is_below_margin(accx[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_analogpins["shrebutton"], writeout);
          Abuffer[3] = writebuffer;

          (is_above_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4pins["psbutton"], writeout);  // PS
          Abuffer[6] = writebuffer;

          (is_below_margin(accy[0])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4pins["joyLXax3"] , writeout);  // Z-as  ?? pin 5 = joyLXax3
          Abuffer[0] = writebuffer;

          //visualisation
          for (int i = 0; i < 12; i++) {
            leftRing.setPixelColor(i, leftRing.Color(Abuffer[i], 0, 0));
          }
          leftRing.show();
          break;
        case BLUE:
          (is_above_margin(accx[0])) ? (writeout  = HIGH,writeout1 = LOW, writebuffer = MAX) : (writeout  = LOW, writeout1 = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4pins["l1button5"], writeout);
          digitalWrite(26, writeout1);
          Abuffer[9] = writebuffer;

          (is_below_margin(accx[0])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4pins["l1button5"], writeout);
          Abuffer[3] = writebuffer;

          (is_above_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4pins["sqbutton"], writeout);
          Abuffer[6] = writebuffer;

          (is_below_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4pins["circbutton"], writeout);
          Abuffer[0] = writebuffer;

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
          int tempmargin = 80;
          (is_above_margin(accx[1], tempmargin) || is_below_margin(accx[1], tempmargin)) ? (writeout  = accx[1]) : (writeout  = 127);
          analogWrite(m_PWMpins["PWM4"],writeout);

          (is_above_margin(accy[1], tempmargin) || is_below_margin(accy[1], tempmargin)) ? (writeout  = accy[1]) : (writeout  = 127);
          analogWrite(m_PWMpins["PWM4"],writeout);

         if(exceeds_neutral_range(accx[1]) || exceeds_neutral_range(accy[1])) {
           directionB = ((int)(6.3 * (3.1415 + atan2(accx[1] - CENTER, accy[1] - CENTER)) / 3.1415));
         }
         (c_button[1]) ? (writeout = LOW) : (writeout = HIGH);
         digitalWrite(m_ps4pins["sqbutton"], writeout);

         for (int i = 0; i < 12; i++) {
           rightRing.setPixelColor(i, rightRing.Color(0, MIN, 0));
           if (i == directionB)rightRing.setPixelColor(i, rightRing.Color(0, MAX, 0));
         }
         rightRing.show();
         break;
       case RED:
        (c_button[1]) ? (writeout = LOW) : (writeout = HIGH);
        digitalWrite(m_ps4pins["r2button"], writeout);

        // original definition was probably wrong: (accx[1] > CENTER + MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN)
        (is_above_margin(accx[1]) && is_above_margin(accy[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["circbutton"], writeout);
        Bbuffer[9] = writebuffer;

        (is_above_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
        digitalWrite(m_ps4pins["r1button6"] , writeout);
        Bbuffer[7] = writebuffer;
        Bbuffer[8] = writebuffer;

        (is_below_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout = HIGH, writebuffer = MAX) : (writeout = LOW, writebuffer = MIN);
        digitalWrite(m_ps4pins["l1button5"], writeout);
        Bbuffer[4] = writebuffer;
        Bbuffer[5] = writebuffer;

        (is_above_margin(accx[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["r2button"], writeout);
        Bbuffer[10] = writebuffer;
        Bbuffer[11] = writebuffer;

        (is_below_margin(accx[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["l2button"], writeout);
        Bbuffer[1] = writebuffer;
        Bbuffer[2] = writebuffer;

        (is_below_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);

        // original definition was probably wrong: if (accx[1] < CENTER - MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN)
        (is_below_margin(accx[1]) && is_above_margin(accy[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["sqbutton"], writeout);
        Bbuffer[3] = writebuffer;

        // original definition was probably wrong: if (accy[1] > CENTER + MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN )
        (is_above_margin(accy[1]) && is_above_margin(accx[1]) && is_below_margin(accx[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["tributton"], writeout);
        Bbuffer[6] = writebuffer;

        // original definition was probably wrong: if (accy[1] < CENTER - MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN )
        (is_below_margin(accy[1]) && is_above_margin(accx[1]) && is_below_margin(accx[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
        digitalWrite(m_ps4pins["xbutton"], writeout);
        Bbuffer[0] = writebuffer;

         for (int i = 0; i < 12; i++) {
           rightRing.setPixelColor(i, rightRing.Color(Bbuffer[i], 0, 0));
         }
         rightRing.show();
         break;
       case BLUE:
         if (Rreleased == false) {

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
      if (LeftReleased == false) {
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
      if (RightReleased == false) {
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
    }
};

class Nunchuckcontr : protected Contr {
  private:
    const uint8_t NUNCHUCK_X_OFFSET = 0;
    const uint8_t NUNCHUCK_Y_OFFSET = 0;
  public:
    Nunchuckcontr() {
      CENTER = 127;
      MARGIN = 16;
    }

  // destructor
    ~Nunchuckcontr() {

    }
    // setup function
    void init() {
      #if NUNCHCK > 0
        nunchuckFunctions::Wire.setClock(10000) ;
        // 20 and 21 are probably clock nunchucks
        pinMode(20, INPUT_PULLUP);
        pinMode(21, INPUT_PULLUP);
        nunchuckFunctions::selectNunchuckChannel(0);
        nunchuckFunctions::nunchuck_init();  // send the initilization handshake
        nunchuckFunctions::selectNunchuckChannel(1);
        nunchuckFunctions::nunchuck_init();  // send the initilization handshake
      #endif
    }
    // loop function
    void readInput() {
      // read both nunchucks
      #if NUNCHCK > 0
      for (int n = 0; n < 2; n++) {
        nunchuckFunctions::selectNunchuckChannel(n);
        delay(10);
        if (nunchuckFunctions::nunchuck_get_data()) {
          accx[n] = (int)nunchuckFunctions::nunchuck_joyx() - NUNCHUCK_X_OFFSET;
          accy[n] = (int)nunchuckFunctions::nunchuck_joyy() - NUNCHUCK_Y_OFFSET;
          c_button[n] = (int)nunchuckFunctions::nunchuck_cbutton();
          z_button[n] = (int)nunchuckFunctions::nunchuck_zbutton();
        }
      }
      #endif
    }
};

class Modps4contr : protected Contr {
  private:
  public:

  Modps4contr() {
    CENTER = 127;
    MARGIN = 50;
  }

  ~Modps4contr() {

  }

  void init() {

  }

  void readInput() {
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


Contr *controller = NULL;
// setup is like int main()
void setup() {
#ifdef DEBUG
  Serial.begin(115200);   // debug info to usb serial
#endif
  delay(20);        // wait for the nunchuck to be powered up
  // empty object for null-checking
  // call init()
  if(dv == NUNCHUCK) {
    Nunchuckcontr controller;
  }else {
    Modps4contr controller;
  }
}

void loop() {
  if (millis() > loopTime + 19) { // run at 10 Hz
    loopTime = millis();
      if(controller) {
        controller -> readInput();
        controller -> operate();
      }
    }

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
