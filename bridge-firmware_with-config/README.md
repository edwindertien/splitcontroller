The SD/LCD module code is used for an Arduino Due connected to a ReprapDiscount Smart Controller
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

************************************************************
SPLITCONTROLLER
************************************************************
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
