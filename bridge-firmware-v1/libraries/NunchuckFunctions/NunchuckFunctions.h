/*
 * Nunchuck functions  -- Talk to a Wii Nunchuck
 *
 * Original by:
 * 2007 Tod E. Kurt, http://todbot.com/blog/
 *
 * Adapted by:
 * 2021 Lonn,  github.com/lonnwolf031
 *
 */

#include <Arduino.h>
#include <Wire.h>

class NunchuckFunctions {

public:

  NunchuckFunctions();
  ~NunchuckFunctions();

  extern int   nunchuck_buf[6];
  void      nunchuck_init(void);
  void      nunchuck_send_request(void);
  char      nunchuk_decode_byte (char);
  int       nunchuck_get_data(void);
  void      selectNunchuckChannel(int) ;
  void      nunchuck_print_data(void);
  int       nunchuck_zbutton(void);
  int       nunchuck_cbutton(void);
  int       nunchuck_joyx(void);
  int       nunchuck_joyy(void);
  int       nunchuck_accelx(void);
  int       nunchuck_accely(void);
  int       nunchuck_accelz(void);
};
