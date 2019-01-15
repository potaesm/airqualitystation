#include <SoftwareSerial.h>

#define co2Pin 12

char co2Value[9];
String co2 = "";

SoftwareSerial co2Serial(A1, A0); // RX, TX
SoftwareSerial unoSerial(5, 6); // RX, TX

// Command for MH-Z14
byte addArray[] = { 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };

// SETUP
void setup()
{

  // Open serial port for send the message to UNO
  Serial.begin(9600);
  Serial.flush();
  co2Serial.begin(9600);
  co2Serial.flush();
  unoSerial.begin(9600);
  unoSerial.flush();
  Serial.println("Setup complete!");

}
// END OF SETUP

// MAIN LOOP
void loop()
{
  co2Serial.listen();
  co2Serial.write(addArray, 9);
  co2Serial.readBytes(co2Value, 9);
  int resHigh = (int) co2Value[2];
  int resLow  = (int) co2Value[3];
  int pulse = (256 * resHigh) + resLow;
  co2 = String(pulse);

  unoSerial.listen();
  unoSerial.print(co2);
  unoSerial.print('\n');

  Serial.print("PPM: ");
  Serial.println(co2);

  delay(800);

}
// END OF MAIN LOOP
