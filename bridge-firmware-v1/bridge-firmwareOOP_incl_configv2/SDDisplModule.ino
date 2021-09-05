/*
  This code is used for an Arduino Due connected to a ReprapDiscount Smart Controller
  This is a standard control panel for use on (for example) a Prusa MK3 printer
  and contains a 4x20 character LCD display, rotary encoder, button, buzzer and SD card
  slot.

  The code assumes a pin-out as used on the RAMPS 1.4 arduino shield (normally for
  Arduino MEGA). The only change is that on the DUE the SPI port is not mapped to pins
  50, 51 and 52, but the ISP header at the center of the board has to be used

  This code uses the standard built-in examples for Arduino, both for the LCD from
  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld
  and the SD library 'file list' example.

  Note that the DUE works on 3.3V but the display controller assumes 5V. It has a level
  shifter (again) to operate the SD card on 3.3V. (I tried to make it work powered by 3.3V
  although the display works, the SD card does not. The control board is hence powered by 5.0 V)

  The following pin-out for the two 10-way EXP connectors on the display board are used:
   ______________________________________
  | EXP1 connector pin | Arduino DUE pin |
  | 1 buzzer           | 37              |
  | 2 ENCODER-button   | 35              |
  | 3 LCD enable       | 17              |
  | 4 LCD RS           | 16              |
  | 5 LCD D4           | 23              |
  | 6 LCD D5           | 25              |
  | 7 LCD D6           | 27              |
  | 8 LCD D7           | 29              |
  | 9 GND              | GND             |
  | 10 VCC             | +5V (not 3.3V!) |
  |____________________|_________________|
   ______________________________________
  | EXP2 connector pin | Arduino DUE pin |
  | 1 MISO             | ISP pin 1 MISO  |
  | 2 SCK              | ISP pin 3 SCK   |
  | 3 ENCODER-1        | 31              |
  | 4 ISP slave sel    | 53              |
  | 5 ENCODER-2        | 33              |
  | 6 MOSI             | ISP pin 4 MOSI  |
  | 7 SD card detect   | 49              |
  | 8 button           | 41              |
  | 9 GND              | GND             |
  | 10 NC              |                 |
  |____________________|_________________|

  (cc) e13, May 2021


#include <ArduinoSTL.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

class SDreader1 {
private:
  const uint8_t BUTTON = 41;
  const uint8_t BTN_EN1 = 31;
  const uint8_t BTN_EN2 = 33;
  const uint8_t BTN_ENC = 35;
  const uint8_t BUZZER = 37;
  const uint8_t SDCARDDETECT = 49;
  const uint8_t SLAVESELECT = 53;
  volatile uint8_t position;
  const uint8_t rs = 16, en = 17, d4 = 23, d5 = 25, d6 = 27, d7 = 29;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  File root;

public:
  SDreader() {
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(BTN_ENC, INPUT_PULLUP);
    pinMode(BTN_EN1, INPUT_PULLUP);
    pinMode(BTN_EN2, INPUT_PULLUP);
    pinMode(SDCARDDETECT,INPUT_PULLUP);

    // set up the LCD's number of columns and rows:
    lcd.begin(20, 4);
    // Print a message to the LCD.
    lcd.print("now checking SD...");
    //
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
      delay(10); // wait for serial port to connect. Needed for native USB port only
    }
    Serial.print("Initializing SD card...");
    if (!SD.begin(SLAVESELECT)) {
      Serial.println("initialization failed!");
      while (1);
    }
    Serial.println("initialization done.");
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
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
      digitalWrite(BUZZER,!digitalRead(BUTTON)); // beep beep beep....
      lcd.setCursor(10, 1);
      lcd.print(digitalRead(SDCARDDETECT)); // just for checking. It could be part of
      // a better routine checking a card has been inserted and (re)connecting.
    }
  }


  void encoder(){
    uint8_t A = digitalRead(BTN_EN1);
    uint8_t B = digitalRead(BTN_EN2);
    static uint8_t dA,dB;
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
}
*/
