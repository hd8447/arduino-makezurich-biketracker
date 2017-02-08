#include <Arduino.h>
#include <TinyGPS++.h>
#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include "BikeConfig.h"
#include <Adafruit_NeoPixel.h>
#include "MyMap.h"

#define gpsSerial Serial
MyMap *myMap = NULL;
SoftwareSerial radioSerial(radioRxPin, radioTxPin); // RX, TX
rn2xx3 myLora(radioSerial);
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, pixelPin, NEO_GRB + NEO_KHZ800);
char msgBuffer[msgBufferSize];
TinyGPSPlus gps;

uint8_t bikeState = bikeState_Unknown;
uint16_t count;
uint16_t countMax;
uint8_t r = 10;
uint8_t g = 10;
uint8_t b = 10;
int32_t gpsLat;
double gpsLat2 = 0; // hack
int32_t gpsLon;
double gpsLon2 = 0; // hack
int16_t gpsAltitude;
uint8_t myMapValue;
#include "BikeUtil.h"

void setup() {
  pixel.begin();
  pixelShow(3);  // weiss
  pinMode(battVoltagePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(connectedPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  count = 0;
  countMax = radioSenIntevall_Connected;
  led(false);
  setupRadio(); // TODO: blocker fals kein radio --> fixit
  setupGps();
  myMap = new MyMap();
  myMap->load();
  pixelShow(1); // grau
}

void loop() {
  r = 0;
  g = 0;
  b = 0;
  readBikeState();
  if (bikeState == bikeState_Connected) {
    b = 10; // blue
  } else if (bikeState == bikeState_noGPS) {
    r = 8; g = 3; b = 12; // violett
  } else {
    if (myMapValue == 0) {
      r = 15; // red
      // TODO: turn off bike
    } else if (myMapValue == 2) {
      g = 10; // green
       countMax = radioSenIntevall_DrivingCenter;
    } else {
      r = 10; g = 5; b = 2; // orange
       countMax = radioSenIntevall_DrivingBorder;
    }
  }

  if (count >= countMax) {
    count = 0;
    pixelShow(15); //
    sendRadio();
    // sendRadioAndReceiveCommand();
  }
  led(false);
  readGps();
  myMapValue = myMap->value(gpsLat2, gpsLon2);
  
  smartDelay(500);
  pixelShow(3); //
  smartDelay(500);
  pixelShow(1); //
  count++;
}

void readBikeState() {
  if (digitalRead(connectedPin) == LOW) {
    bikeState = bikeState_Connected;
    countMax = radioSenIntevall_Connected;
  } else {
    if ( gps.speed.isValid()) {
      bikeState = gps.speed.kmph() > 1 ? bikeState_Driving : bikeState_Sleeping;
    } else {
      bikeState = bikeState_noGPS ;
    }
  }
}

void readGps() {
  msgBuffer[0] = 0; // MessageType
  msgBuffer[1] = int8((uint8_t)(gps.date.year() - 2000), gps.date.isValid()); // Datum
  msgBuffer[2] = int8((uint8_t)(gps.date.month() ), gps.date.isValid()); //
  msgBuffer[3] = int8((uint8_t)(gps.date.day() ), gps.date.isValid()); //
  smartDelay(0);
  msgBuffer[4] = int8((uint8_t)(gps.time.hour() ), gps.time.isValid()); // Zeit
  msgBuffer[5] = int8((uint8_t)(gps.time.minute() ), gps.time.isValid()); //
  msgBuffer[6] = int8((uint8_t)(gps.time.second() ), gps.time.isValid()); //
  smartDelay(0);
  msgBuffer[7] = (uint8_t)(analogRead(battVoltagePin) >> 2); // Spannung
  smartDelay(0);
  gpsLat = int32((int32_t)(gps.location.lat() * 10000000), gps.location.isValid()); // lat
  gpsLat2 = gpsLat == 0 ? 0 : gps.location.lat() ;
  msgBuffer[8] = (gpsLat >> 24) & 0xff;
  msgBuffer[9] = (gpsLat >> 16) & 0xff;
  msgBuffer[10] = (gpsLat >> 8) & 0xff;
  msgBuffer[11] = gpsLat & 0xff;
  smartDelay(0);
  gpsLon = int32((int32_t)(gps.location.lng() * 10000000), gps.location.isValid()); // lang
  gpsLon2 = gpsLon == 0 ? 0 : gps.location.lng() ;
  msgBuffer[12] = (gpsLon >> 24) & 0xff;
  msgBuffer[13] = (gpsLon >> 16) & 0xff;
  msgBuffer[14] = (gpsLon >> 8) & 0xff;
  msgBuffer[15] = gpsLon & 0xff;
  smartDelay(0);
  gpsAltitude = int16((int16_t)(gps.altitude.meters()), gps.altitude.isValid()); // höhe, double
  msgBuffer[16] =  (gpsAltitude >> 8) & 0xff;
  msgBuffer[17] =  gpsAltitude & 0xff;
  msgBuffer[18] = int8((uint8_t)(gps.speed.kmph() / 0.36), gps.speed.isValid()); // speed, double
  msgBuffer[19] = int8((uint8_t)(gps.course.deg() / 2), gps.course.isValid()); // richtung, double
  smartDelay(0);
  msgBuffer[20] = int8((uint8_t)(gps.satellites.value()), gps.satellites.isValid()); // Sateliten Anzahl
  msgBuffer[21] = 0; // TODO: timeToFix
  smartDelay(0);
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
    }
  } while (millis() - start < ms);
}

void setupGps() {
  gpsSerial.begin(gpsBaud);
}

void setupRadio() {
  radioSerial.begin(radioBaud);
  pinMode(radioResetPin, OUTPUT);
  digitalWrite(radioResetPin, LOW);
  delay(500);
  digitalWrite(radioResetPin, HIGH);
  delay(100); //wait for the RN2xx3's startup message
  radioSerial.flush();
  myLora.autobaud();
  String hweui = myLora.hweui();
  while (hweui.length() != 16) {
    delay(10000);
    hweui = myLora.hweui();
  }
  bool join_result = myLora.initABP(addr, AppSKey, NwkSKey);
  while (!join_result)  {
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
}


TX_RETURN_TYPE sendRadioPrimary(boolean confirmed) {
  char msgBuffer[msgBufferSize * 2 + 1];
  char buffer[3];
  for (unsigned i = 0; i < msgBufferSize; i++)  {
    sprintf(buffer, "%02X", msgBuffer[i]);
    memcpy(&msgBuffer[i * 2], &buffer, sizeof(buffer));
  }
  String dataToTx(msgBuffer);
  if (confirmed) {
    return myLora.txCommand("mac tx cnf 1 ", dataToTx, false);
  }
  return myLora.txCommand("mac tx uncnf 1 ", dataToTx, false);
}



void sendRadio() {
  //sendRadioPrimary(false);
  myLora.txBytes(msgBuffer, msgBufferSize);
}


char sendRadioAndReceiveCommand() {
  if (sendRadioPrimary(true) == TX_WITH_RX)  {
    String rec = myLora.base16decode(myLora.getRx());
    if (rec.length() == 1) {
      return rec.charAt(0);
    }
  }
  return 0;
}


void led(boolean on) {
  digitalWrite(ledPin, on ? 0 : 1);
}

void pixelShow(uint8_t factor) {
  pixel.setPixelColor(0, pixel.Color(r * factor * pixelBright, g * factor * pixelBright, b * factor * pixelBright));
  pixel.show();
}






