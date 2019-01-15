#include <SoftwareSerial.h>
#include "Magellan.h"
#include "SHT1x.h"

// Magellan authentication
Magellan magel;
char auth[] = "f15819e0-f0e5-11e8-a028-9771a15972bf";
void postMagellan();

// Message from UART
SoftwareSerial miniSerial(5, 6); // RX, TX
int co2;

// Define the variables for dust computation
SoftwareSerial dustSerial(10, 11); // RX, TX
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
int index = 0;
char value;
char previousValue;

// Define the string to post to Magellan
String payload = "";
String stationId = "A01";
String tempValue = "";
String humidValue = "";
String dustValue = "";
String co2Value = "";

//Humid&Temp setup
SHT1x tempHumid(7, 8);

void setup()
{

  // Open serial port for receive the message from Pro mini
  Serial.begin(9600);
  Serial.flush();

  // Magellan authentication
  magel.begin(auth);

  miniSerial.begin(9600);
  miniSerial.flush();
  dustSerial.begin(9600);
  dustSerial.flush();

}

void loop()
{
  
  // Get the dust value
  dustSerial.listen();
  delay(100);
  index = 0;
  while (dustSerial.available()) {
    value = dustSerial.read();
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)) {
      break;
    }
    if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
      previousValue = value;
    }
    else if (index == 5) {
      pm1 = 256 * previousValue + value;
    }
    else if (index == 7) {
      pm2_5 = 256 * previousValue + value;
      dustValue = String(pm2_5);
    }
    else if (index == 9) {
      pm10 = 256 * previousValue + value;
    }
    else if (index > 15) {
      break;
    }
    index++;
  }

  // Get the co2 value
  miniSerial.listen();
  delay(100);
  if (miniSerial.available()) {
    co2 = miniSerial.parseInt();
    if (miniSerial.read() == '\n') {
      if (co2 >= 0 && co2 <= 5000) {
        co2Value = String(co2);
      }
    }
  }

  // Get the temp & humid value
  tempValue = String(tempHumid.readHumidity());
  humidValue = String(tempHumid.retrieveTemperatureC());

  // Send string to Magellan
  if (dustValue != "" && co2Value != "" && tempValue != "" && humidValue != "") {
    postMagellan(stationId, tempValue, humidValue, dustValue, co2Value);
  }
}

void postMagellan(String stationId, String tempValue, String humidValue, String dustValue, String co2Value) {

  // Set the form of string to send to Magellan
  payload = "{\"stationId\":\"" + stationId + "\", \"tempValue\":" + tempValue + ", \"humidValue\":" + humidValue + ", \"dustValue\":" + dustValue + ", \"co2Value\":" + co2Value + "}";
  // Send string to Magellan
  magel.post(payload);

}
