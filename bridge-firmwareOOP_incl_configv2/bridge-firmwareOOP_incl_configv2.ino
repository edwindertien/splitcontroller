/*************************************************************
  TODO adjust map references for m_ps4pins


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
#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#include <ArduinoSTL.h>

#include <iostream>
#include <map>
#include<vector>
#include<iterator>

// below is for reading CSV files from SD
#include <limits.h>


// below are for ReprapDiscount Smart Controller
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

#define CS_PIN SS

File file;

// define 1 if nunchuck, 0 if PS4 mod
#define NUNCHCK 1

// define 1 if SD/LCD module present
#define SDCLDMODULE 1

Adafruit_NeoPixel leftRing = Adafruit_NeoPixel(12, 53, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing = Adafruit_NeoPixel(12, 51, NEO_GRB + NEO_KHZ800);

enum Device {NUNCHUCK, PS4CONTR};

#if NUNCHCK > 0
#include <NunchuckFunctions.h>
NunchuckFunctions* nunchuckFunctions = new NunchuckFunctions;
const Device dv = NUNCHUCK;
#else
const Device dv = PS4CONTR;
#endif

enum Mode {GREEN, RED, BLUE};
enum Side {LEFT, RIGHT};
enum PinType {DIGITALPIN, ANALOGPIN, PWMPIN};
enum SplitContrButtons {JLJOY, RJOY, LBTN1, LBTN2, RBTN1, RBTN2};

const uint8_t greenLED = 13;
const uint8_t redLED = 12;

struct PinAllocation {
  std::string     btn;
  uint8_t           pinnum;
  PinType         pinType;
  std::string     mappedps4func;
  bool operator==(const PinAllocation& r) const   {
    return btn == r.btn;
  }
};

struct custCSVmapping
{
  std::string menu;
  std::string side;
  std::string btn;
  std::string assgndto;
  std::string mode;
};


std::vector<PinAllocation> pinalloc =
{
  {"joyRXax2", 2, DIGITALPIN},
  {"joyRYax1", 3, DIGITALPIN},
  {"joyRZb11", 4, DIGITALPIN},
  {"joyLZb12", 7, DIGITALPIN},
  {"joyLYax4", 6, DIGITALPIN},
  {"joyLXax3", 5, DIGITALPIN},
  {"sqbutton", 26, DIGITALPIN},
  {"circbutton", 28, DIGITALPIN},
  {"psbutton", 30, DIGITALPIN},
  {"l2button", 32, DIGITALPIN},
  {"r2button", 34, DIGITALPIN},
  {"tributton", 36, DIGITALPIN},
  {"xbutton", 38, DIGITALPIN},
  {"lhat", 40, DIGITALPIN},
  {"rhat", 42, DIGITALPIN},
  {"uhat", 44, DIGITALPIN},
  {"dhat", 46, DIGITALPIN},
  {"r1button6", 48, DIGITALPIN},
  {"l1button5", 50, DIGITALPIN},
  {"optbutton",  52, DIGITALPIN},
  {"shrebutton", A8, DIGITALPIN},
  {"accx1", A5, ANALOGPIN},
  {"accy1", A4, ANALOGPIN},
  {"accx2", A1, ANALOGPIN},
  {"accy2", A0, ANALOGPIN},
  {"c_button1", A6, ANALOGPIN},
  {"z_button1", A7, ANALOGPIN},
  {"c_button2", A2, ANALOGPIN},
  {"z_button2", A3, ANALOGPIN},
  {"PWM1", 2, PWMPIN},
  {"PWM2", 3, PWMPIN},
  {"PWM3", 5, PWMPIN},
  {"PWM4", 6, PWMPIN},
  {"PWM5", 7, PWMPIN}
};

std::map<custCSVmapping, PinAllocation> mPinAlloc =
{
  {{green,left ,none,joyRXax2,repeat},{"joyRXax2", 2, DIGITALPIN}},
  {{green,left ,none,joyRYax1,repeat},{"joyRYax1", 3, DIGITALPIN}},
  {{green,left ,none,joyRZb11,repeat},{"joyRZb11", 4, DIGITALPIN}},
  {{green,left ,none,joyLZb12,repeat},{"joyLZb12", 7, DIGITALPIN}},
  {{green,left ,none,joyLYax4,repeat},{"joyLYax4", 6, DIGITALPIN}},
  {{green,left ,none,joyLXax3,repeat},{"joyLXax3", 5, DIGITALPIN}},
  {{green,left ,none,sqbutton,repeat},{"sqbutton", 26, DIGITALPIN}},
  {{green,left ,none,circbutton,repeat},{"circbutton", 28, DIGITALPIN}},
  {{green,left ,none,psbutton,repeat},{"psbutton", 30, DIGITALPIN}},
  {{green,left ,none,l2button,repeat},{"l2button", 32, DIGITALPIN}},
  {{green,left ,none,r2button,repeat},{"r2button", 34, DIGITALPIN}},
  {{green,left ,none,tributton,toggle},{"tributton", 36, DIGITALPIN}},
  {{green,left ,none,xbutton,toggle},{"xbutton", 38, DIGITALPIN}},
  {{green,left ,none,lhat,toggle},{"lhat", 40, DIGITALPIN}},
  {{green,left ,none,rhat,toggle},{"rhat", 42, DIGITALPIN}},
  {{green,left ,none,uhat,toggle},{"uhat", 44, DIGITALPIN}},
  {{green,left ,none,dhat,toggle},{"dhat", 46, DIGITALPIN}},
  {{green,left ,none,r1button6,toggle},{"r1button6", 48, DIGITALPIN}},
  {{green,left ,none,l1button5,toggle},{"l1button5", 50, DIGITALPIN}},
  {{green,left ,none,optbutton,toggle},{"optbutton",  52, DIGITALPIN}},
  {{green,left ,none,accx1,toggle},{"shrebutton", A8, DIGITALPIN}},
  {{green,left ,none,accy1,toggle},{"accx1", A5, ANALOGPIN}},
  {{green,left ,none,accx2,toggle},{"accy1", A4, ANALOGPIN}},
  {{green,left ,none,accy2,toggle},{"accx2", A1, ANALOGPIN}},
  {{green,left ,none,c_button1,toggle},{"accy2", A0, ANALOGPIN}},
  {{green,left ,none,z_button1,toggle},{"c_button1", A6, ANALOGPIN}},
  {{green,left ,none,c_button2,toggle},{"z_button1", A7, ANALOGPIN}},
  {{green,left ,none,z_button2,toggle},{"c_button2", A2, ANALOGPIN}},
  {{green,left ,none,z_button2,toggle},{"z_button2", A3, ANALOGPIN}},
  {{green,left ,none,PWM1,toggle},{"PWM1", 2, PWMPIN}},
  {{green,left ,none,PWM2,toggle},{"PWM2", 3, PWMPIN}},
  {{green,left ,none,PWM3,toggle},{"PWM3", 5, PWMPIN}},
  {{green,left ,none,PWM3,toggle},{"PWM4", 6, PWMPIN}},
  {{green,left ,none,PWM3,toggle},{"PWM5", 7, PWMPIN},{}}
};



unsigned long loopTime;

// buffers for the coloured rings:
unsigned char Abuffer[12];
unsigned char Bbuffer[12];
unsigned char Cbuffer[12];
unsigned char Dbuffer[12];
boolean L1state, R1state, R2state, L2state, Rreleased;
boolean LeftReleased, RightReleased;

class SDreader {
  private:
    const uint8_t BUTTON = 41;
    const uint8_t BTN_EN1 = 31;
    const uint8_t BTN_EN2 = 33;
    const uint8_t BTN_ENC = 35;
    const uint8_t BUZZER = 37;
    const uint8_t SDCARDDETECT = 49;
    const uint8_t SLAVESELECT = 53;
    volatile uint8_t position;
    File root;
    const uint8_t rs = 16, en = 17, d4 = 23, d5 = 25, d6 = 27, d7 = 29;
    LiquidCrystal lcd;

    //    static stringstream ss;

  public:
    SDreader() : lcd(rs, en, d4, d5, d6, d7) {
      pinMode(BUZZER, OUTPUT);
      pinMode(BUTTON, INPUT_PULLUP);
      pinMode(BTN_ENC, INPUT_PULLUP);
      pinMode(BTN_EN1, INPUT_PULLUP);
      pinMode(BTN_EN2, INPUT_PULLUP);
      pinMode(SDCARDDETECT, INPUT_PULLUP);

      // set up the LCD's number of columns and rows:
      lcd.begin(20, 4);
      // Print a message to the LCD.
      lcd.print("Initializing SD card...");
      if (!SD.begin(SLAVESELECT)) {
        lcd.print("initialization failed!");
        while (1);
      }
      lcd.print("initialization done.");
      root = SD.open("/");
      printDirectory(root, 0);
      lcd.print("done!");
      // here: go to function to read data
    }


    void readData() {
      // while true? except stop
      static unsigned long looptimer; // for timing in this loop.
      encoder();  // run the service routine, could also be using timer uint8_terrupt
      if (millis() > looptimer + 99) {
        looptimer = millis();
        lcd.setCursor(0, 1); // line 1 is the second row, since counting begins with 0
        lcd.print(millis() / 1000); // print the number of seconds since reset:
        if (!digitalRead(BTN_ENC)) position = 0; // reset encoder when pressed
        lcd.setCursor(5, 1);
        lcd.print(position);
        lcd.print("    "); // clean up after long numbers have been printed
        digitalWrite(BUZZER, !digitalRead(BUTTON)); // beep beep beep....
        lcd.setCursor(10, 1);
        lcd.print(digitalRead(SDCARDDETECT)); // just for checking. It could be part of
        // a better routine checking a card has been inserted and (re)connecting.
      }
    }


    void encoder() {
      uint8_t A = digitalRead(BTN_EN1);
      uint8_t B = digitalRead(BTN_EN2);
      static uint8_t dA, dB;
      // full quadrature, so 4 values per 'click'
      if ((A == 1 && dA == 0 && B == 0) || (A == 0 && dA == 1 && B == 1) || (B == 1 && dB == 0 && A == 1) || (B == 0 && dB == 1 && A == 0)) position++;
      if ((B == 1 && dB == 0 && A == 0) || (B == 0 && dB == 1 && A == 1) || (A == 1 && dA == 0 && B == 1) || (A == 0 && dA == 1 && B == 0)) position--;
      dA = A; // store previous values
      dB = B; // store previous values
    }

    void printDirectory(File dir, int numTabs) {
      while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
          // no more files
          break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
          Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
          Serial.println("/");
          printDirectory(entry, numTabs + 1);
        } else {
          // files have sizes, directories do not
          Serial.print("\t\t");
          Serial.println(entry.size(), DEC);
        }
        entry.close();
      }
    }
};


class CSVreader {
  protected:
    File csvfile;
    custCSVmapping cstmMapping;

  public:
    //std::vector <custCSVmapping> allcstmMappings;
    std::string menu, side, button, assigndto, mode;

    CSVreader() {
      SDreader sdreader;

      // sdreader -> callfunc();
    }

    void readVals() {
      while (csvfile.read()) {
        getline(csvfile, menu, ',');
        getline(csvfile, side, ',') ;
        getline(csvfile, button, ',') ;
        getline(csvfile, assigndto, ',');
        getline(csvfile, mode, '\n');
      }
    }

//menu  side  button  assigned to    mode

    void readCSVFile() {
      csvfile = SD.open("test_config_file.csv", FILE_READ);

      std::string menu, side, button, assignedto, mode;
      long x, y;
      if (!SD.begin(SS)) {
        Serial.println("begin error");
        return;
      }
      if (!csvfile) {
        Serial.println("open error");
        return;
      }
      while (readVals(&menu, &side, &button, &assignedto, &mode)) {
         cstmMapping.menu = menu;
         cstmMapping.side = side;
         cstmMapping.assignedto = assignedto;
         cstmMapping.mode = mode;
         // find matching pinAlloc -->  findinMapping(custCSVmapping csvElement) 
         mPinAlloc.insert({ cstmMapping, pnAlloc });
         
      /*
       * new struct
       * assignedto --> match with pinalloc
       * mPinAlloc --> struct, matched pinalloc
       * 
       * 
       */
        Serial.print("x: ");
        Serial.println(menu);
        Serial.print("y: ");
        Serial.println(button);
        Serial.println();
      }
      Serial.println("Done");

      csvfile.close();
    }

    void readCSV() {
      

    }

    void findinMapping(custCSVmapping csvElement) {
      for (auto & element : pinalloc) {
        // if element.btn == csvElement.assgndto {
      // append mPinAlloc, csvElement, element}
        element.doSomething ();
      }
      
    }

/*
    void updateMapping(std::vector<PinAllocation> &origMapping, std::vector<custCSVmapping> &newMapping) {

      // std::map<custCSVmapping, PinAllocation> mPinAlloc;
      for (auto const& x : newMapping)
      {
        std::find_if(origMapping.begin(), origMapping.end(), [ = ](const PinAllocation & c)
        {
          return k == c.btn;
        })

        auto it = std::find(myvector.begin(), myvector.end(), 3);
        if (it != myvector.end()) {
          std::cout << "Element found in myvector: " << *it << '\n';
        } else {
          std::cout << "Element not found in myvector\n";
        }


        // try to find x.btn in origMapping, if not: next
        // add both x and found item to map mPinAlloc



      }

    }
    */



};



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

    void init() {
      pinMode(m_ps4pins[0]["greenLED"], OUTPUT); // greenLED 13
      pinMode(9, OUTPUT);

      for (const auto &all : pinalloc) {
        if (all.PinType == DIGITALPIN) {
          pinMode(all.pinnum, OUTPUT);
          digitalWrite(all.pinnum, HIGH);
          // some pins need high active
          if (all.origps4func == "joyRZb11" || all.origps4func == "joyLZb12" || all.origps4func == "r1button6" ||  all.origps4func == "l1button5"  ) {
            digitalWrite(all.pinnum, LOW);
          }
        }
        else if (all.PinType == ANALOGPIN) {
          pinMode(all.pinnum, INPUT_PULLUP);
        }
        else if (all.PinType == PWMPIN) {
          // start with center voltage
          analogWrite(all.pinnum, 127);
        }
      }

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
    void readInput() {
      //override by derived
    }

    void modifyMapping() {
      
    }

    void operate() {
      int writebuffer;
      int writeout;
      int writeout1;
      int directionA;
      switch (LeftMode) {
        case GREEN:
          directionA = -1;
          analogWrite(m_ps4digipins[0]["joyRYax1"], accx[0]); // axis 1
          analogWrite(m_ps4digipins[0]["joyRXax2"], 255 - accy[0]); // axis 2
          // visualisation

          if (exceeds_neutral_range(accx[0]) || exceeds_neutral_range(accy[0])) {
            directionA = ((int)(6.5 * (3.1415 + atan2(accx[0] - CENTER, accy[0] - CENTER)) / 3.1415));
          }

          if (z_button[0]) digitalWrite(m_ps4digipins[0]["xbutton"], LOW);
          else digitalWrite(m_ps4digipins[0]["xbutton"], HIGH); // (X)

          for (int i = 0; i < 12; i++) {
            leftRing.setPixelColor(i, leftRing.Color(0, MIN, 0));
            if (i == directionA)leftRing.setPixelColor(i, leftRing.Color(0, MAX, 0));
          }
          leftRing.show();
          break;
        case RED:
          (is_above_margin(accx[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["optbutton"], writeout);
          Abuffer[9] = writebuffer;

          (is_below_margin(accx[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4digipins["shrebutton"], writeout);
          Abuffer[3] = writebuffer;

          (is_above_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["psbutton"], writeout);  // PS
          Abuffer[6] = writebuffer;

          (is_below_margin(accy[0])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["joyLXax3"] , writeout);  // Z-as  ?? pin 5 = joyLXax3
          Abuffer[0] = writebuffer;

          //visualisation
          for (int i = 0; i < 12; i++) {
            leftRing.setPixelColor(i, leftRing.Color(Abuffer[i], 0, 0));
          }
          leftRing.show();
          break;
        case BLUE:
          (is_above_margin(accx[0])) ? (writeout  = HIGH, writeout1 = LOW, writebuffer = MAX) : (writeout  = LOW, writeout1 = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["l1button5"], writeout);
          // what does 26 refer to?
          digitalWrite(26, writeout1);
          Abuffer[9] = writebuffer;

          (is_below_margin(accx[0])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["l1button5"], writeout);
          Abuffer[3] = writebuffer;

          (is_above_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = HIGH, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["sqbutton"], writeout);
          Abuffer[6] = writebuffer;

          (is_below_margin(accy[0])) ? (writeout  = LOW, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
          digitalWrite(m_ps4digipins[0]["circbutton"], writeout);
          Abuffer[0] = writebuffer;

          //visualisation
          for (int i = 0; i < 12; i++) {
            leftRing.setPixelColor(i, leftRing.Color(0, 0, Abuffer[i]));
          }
          leftRing.show();
          break;
      }

      int directionB = -1;
      int tempmargin;
      //RightMode
      switch (RightMode) {
        case GREEN:
          {
            tempmargin = 80;
            (is_above_margin(accx[1], tempmargin) || is_below_margin(accx[1], tempmargin)) ? (writeout  = accx[1]) : (writeout  = 127);
            analogWrite(m_ps4pwmpins[0]["PWM4"], writeout);

            (is_above_margin(accy[1], tempmargin) || is_below_margin(accy[1], tempmargin)) ? (writeout  = accy[1]) : (writeout  = 127);
            analogWrite(m_ps4pwmpins[0]["PWM4"], writeout);

            if (exceeds_neutral_range(accx[1]) || exceeds_neutral_range(accy[1])) {
              directionB = ((int)(6.3 * (3.1415 + atan2(accx[1] - CENTER, accy[1] - CENTER)) / 3.1415));
            }
            (c_button[1]) ? (writeout = LOW) : (writeout = HIGH);
            digitalWrite(m_ps4digipins[0]["sqbutton"], writeout);

            for (int i = 0; i < 12; i++) {
              rightRing.setPixelColor(i, rightRing.Color(0, MIN, 0));
              if (i == directionB)rightRing.setPixelColor(i, rightRing.Color(0, MAX, 0));
            }
            rightRing.show();
            break;
          }

        case RED:
          {
            (c_button[1]) ? (writeout = LOW) : (writeout = HIGH);
            digitalWrite(m_ps4digipins[0]["r2button"], writeout);

            // original definition was probably wrong: (accx[1] > CENTER + MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN)
            (is_above_margin(accx[1]) && is_above_margin(accy[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["circbutton"], writeout);
            Bbuffer[9] = writebuffer;

            (is_above_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout  = HIGH, writebuffer = MAX) : (writeout  = LOW, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["r1button6"] , writeout);
            Bbuffer[7] = writebuffer;
            Bbuffer[8] = writebuffer;

            (is_below_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout = HIGH, writebuffer = MAX) : (writeout = LOW, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["l1button5"], writeout);
            Bbuffer[4] = writebuffer;
            Bbuffer[5] = writebuffer;

            (is_above_margin(accx[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["r2button"], writeout);
            Bbuffer[10] = writebuffer;
            Bbuffer[11] = writebuffer;

            (is_below_margin(accx[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["l2button"], writeout);
            Bbuffer[1] = writebuffer;
            Bbuffer[2] = writebuffer;

            (is_below_margin(accx[1]) && is_above_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);

            // original definition was probably wrong: if (accx[1] < CENTER - MARGIN && accy[1] > CENTER - MARGIN && accy[1] < CENTER + MARGIN)
            (is_below_margin(accx[1]) && is_above_margin(accy[1]) && is_below_margin(accy[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["sqbutton"], writeout);
            Bbuffer[3] = writebuffer;

            // original definition was probably wrong: if (accy[1] > CENTER + MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN )
            (is_above_margin(accy[1]) && is_above_margin(accx[1]) && is_below_margin(accx[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["tributton"], writeout);
            Bbuffer[6] = writebuffer;

            // original definition was probably wrong: if (accy[1] < CENTER - MARGIN && accx[1] > CENTER - MARGIN && accx[1] < CENTER + MARGIN )
            (is_below_margin(accy[1]) && is_above_margin(accx[1]) && is_below_margin(accx[1])) ? (writeout = LOW, writebuffer = MAX) : (writeout = HIGH, writebuffer = MIN);
            digitalWrite(m_ps4digipins[0]["xbutton"], writeout);
            Bbuffer[0] = writebuffer;

            for (int i = 0; i < 12; i++) {
              rightRing.setPixelColor(i, rightRing.Color(Bbuffer[i], 0, 0));
            }
            rightRing.show();
            break;
          }
        case BLUE:
          {
            if (Rreleased == false) {

              if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 0) {
                digitalWrite(m_ps4digipins[0]["r1button6"], HIGH);  // R1
                R1state = 1;
                Rreleased = 1;
                Bbuffer[7] = MAX;
                Bbuffer[8] = MAX;
              }

              else if (accx[1] > CENTER + MARGIN && accy[1] > CENTER + MARGIN && R1state == 1) {
                digitalWrite(m_ps4digipins[0]["r1button6"], LOW);
                R1state = 0;
                Rreleased = 1;
                Bbuffer[7] = MIN;
                Bbuffer[8] = MIN;
              }


              else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 0) {
                digitalWrite(m_ps4digipins[0]["l1button5"] , HIGH); // L1
                L1state = 1;
                Rreleased = 1;
                Bbuffer[4] = MAX;
                Bbuffer[5] = MAX;
              }
              else if (accx[1] < CENTER - MARGIN && accy[1] > CENTER + MARGIN && L1state == 1) {
                digitalWrite(m_ps4digipins[0]["l1button5"], LOW);
                L1state = 0;
                Rreleased = 1;
                Bbuffer[4] = MIN;
                Bbuffer[5] = MIN;
              }

              else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 0) {
                digitalWrite(m_ps4digipins[0]["r2button"], LOW); // R2
                R2state = 1;
                Rreleased = 1;
                Bbuffer[10] = MAX;
                Bbuffer[11] = MAX;
              }
              else if (accx[1] > CENTER + MARGIN && accy[1] < CENTER - MARGIN && R2state == 1) {
                digitalWrite(m_ps4digipins[0]["r2button"], HIGH);
                R2state = 0;
                Rreleased = 1;
                Bbuffer[10] = MIN;
                Bbuffer[11] = MIN;
              }

              else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 0) {
                digitalWrite(m_ps4digipins[0]["l2button"], LOW); // L2
                L2state = 1;
                Rreleased = 1;
                Bbuffer[1] = MAX;
                Bbuffer[2] = MAX;
              }
              else if (accx[1] < CENTER - MARGIN && accy[1] < CENTER - MARGIN && L2state == 1) {
                digitalWrite(m_ps4digipins[0]["l2button"], HIGH);
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
      }
      LeftReleased =  c_button[0];

      ///// NUNCHUCK 2 button switch ///////
      if (RightReleased == false) {
        if (z_button[1] == 1 && RightMode == 0) {
          RightMode = RED;
        }
        else if (z_button[1] == 1 && RightMode == 1) {
          RightMode = BLUE;
        }
        else if (z_button[1] == 1 && RightMode == 2) {
          RightMode = GREEN;
        }
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
      // 20 and 21 are probably clock nunchucks
      pinMode(20, INPUT_PULLUP);
      pinMode(21, INPUT_PULLUP);
      nunchuckFunctions -> selectNunchuckChannel(0);
      nunchuckFunctions -> nunchuck_init();  // send the initilization handshake
      nunchuckFunctions -> selectNunchuckChannel(1);
      nunchuckFunctions -> nunchuck_init();  // send the initilization handshake
#endif
    }
    // loop function
    void readInput() {
      // read both nunchucks
#if NUNCHCK > 0
      for (int n = 0; n < 2; n++) {
        nunchuckFunctions -> selectNunchuckChannel(n);
        delay(10);
        if (nunchuckFunctions -> nunchuck_get_data()) {
          accx[n] = (int)nunchuckFunctions -> nunchuck_joyx() - NUNCHUCK_X_OFFSET;
          accy[n] = (int)nunchuckFunctions -> nunchuck_joyy() - NUNCHUCK_Y_OFFSET;
          c_button[n] = (int)nunchuckFunctions -> nunchuck_cbutton();
          z_button[n] = (int)nunchuckFunctions -> nunchuck_zbutton();
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
      accx[0] = map(analogRead(m_ps4analogpins["accx1"]), 0, 832, 0, 255);
      accy[0] = map(analogRead(m_ps4analogpins["accy1"]), 0, 832, 0, 255);
      accx[1] = map(analogRead(m_ps4analogpins["accx2"]), 0, 832, 0, 255);
      accy[1] = map(analogRead(m_ps4analogpins["accy2"]), 0, 832, 0, 255);
      c_button[0] = !digitalRead(m_ps4analogpins["c_button1"]);
      z_button[0] = !digitalRead(m_ps4analogpins["z_button1"]);
      c_button[1] = !digitalRead(m_ps4analogpins["c_button2"]);
      z_button[1] = !digitalRead(m_ps4analogpins["z_button2"]);
    }
};



Contr *controller = NULL;

void setup() {

#ifdef DEBUG
  Serial.begin(115200);   // debug info to usb serial
#endif
#ifdef SDCLDMODULE

  // read csv
#endif
  delay(20);        // wait for the nunchuck to be powered up
  // empty object for null-checking
  // call init()
  if (dv == NUNCHUCK) {
    Nunchuckcontr controller;
  } else {
    Modps4contr controller;
  }
}

void loop() {
  if (millis() > loopTime + 19) { // run at 10 Hz
    loopTime = millis();
    if (controller) {
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
