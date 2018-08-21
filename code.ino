#include <TheThingsNetwork.h> // https://github.com/TheThingsNetwork/arduino-device-lib
#include <SoftwareSerial.h> 
#include <SimpleDHT.h> // https://github.com/winlinvip/SimpleDHT

// DevAddr, NwkSKey, AppSKey - TTN
const char *devAddr = "2601172D";
const char *nwkSKey = "E2D9DD32BF0D1751168F34DB74C3CC4B";
const char *appSKey = "CCF9364C493AFA4707A47A4C21DC4307";


SoftwareSerial loraSerial(10, 11);
#define debugSerial Serial

#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

int pinDHT11 = 7;
SimpleDHT11 dht11;

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();
}

void loop()
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");

  uint32_t temp = (int)temperature * 100;
  uint32_t hum = (int)humidity * 100;

  Serial.println();
  Serial.print(temp); Serial.print(" *C, "); 
  Serial.print(hum); Serial.println(" H");
  
  debugSerial.println("-- LOOP");

  // Prepare payload of 1 byte to indicate LED status
  byte payload[4];
  payload[0] = highByte(temp);
  payload[1] = lowByte(temp);
  payload[2] = highByte(hum);
  payload[3] = lowByte(hum);

  // Send it off
  ttn.sendBytes(payload, sizeof(payload));

  delay(3000);
}