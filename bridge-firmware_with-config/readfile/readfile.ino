#include <SD.h>
#include <SPI.h>

File file;

bool readLine(File &f, char* line, size_t maxLen) {
  for (size_t n = 0; n < maxLen; n++) {
    int c = f.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}

bool readStrVals(String* v1, String* v2, String* v3, String* v4, String* loc,String* loc2) {
  char line[200], *ptr, *str;
  if (!readLine(file, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v2 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v3 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v4 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  String a = strtok_r(ptr, ",", &str);
  String first(str);
  *loc = first;
  String let(a);
  *loc2 = let;

  return str != ptr;  // true if number found
}

bool readVals(long* v1, long* v2, long* v3, long* v4, String* loc,String* loc2) {
  char line[200], *ptr, *str;
  if (!readLine(file, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v2 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v3 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v4 = strtol(ptr, &str, 10);
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  String a = strtok_r(ptr, ",", &str);
  String first(str);
  *loc = first;
  String let(a);
  *loc2 = let;

  return str != ptr;  // true if number found
}
/* Close CSV File Reading */

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  //SD Card Reader Setup
  Serial.begin(9600);
  if (!SD.begin(SC)) {
    Serial.println("begin error");
    return;
  }
  file = SD.open("gps.CSV", FILE_READ);
  if (!file) {
    Serial.println("open error");
    return;
  }

}

void loop() // run over and over
{
long x, y, z, k;
String loc,loc2;
  while (readVals(&x, &y, &z, &k, &loc, &loc2)) {

    //First 4 Long datatype variables
    Serial.println(x);
    Serial.println(y);
    Serial.println(z;
    Serial.println(k);
    //Last 2 String type variables
    Serial.println(loc);
    Serial.println(loc2);
  }

}
