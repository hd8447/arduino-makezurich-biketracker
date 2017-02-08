#ifndef BikeConfig_h
#define BikeConfig_h

static const char* addr = "00000000"; // bike1
static const char* AppSKey = "00000000000000000000000000000000";
static const char* NwkSKey = "00000000000000000000000000000000";

static const uint8_t ledPin = 13;
static const uint8_t pixelPin = 9;  // RGB-LED
static const uint8_t pixelBright = 1;  // 1=indoor or 2=outdoor
static const uint8_t radioRxPin = 10; 
static const uint8_t radioTxPin = 11;
static const uint8_t radioResetPin = 12;


static const uint16_t radioSenIntevall_Connected = 60*15; // 15 minutes if the bike ist at home
static const uint16_t radioSenIntevall_DrivingCenter = 60*10; // 10 minutes if the bike is in the city center
static const uint16_t radioSenIntevall_DrivingBorder = 60*5; // 5 minutes if the bike is near the city border


static const uint8_t connectedPin = 7;
static const uint8_t buzzerPin = 8;


static const uint32_t radioBaud = 9600;
static const uint32_t gpsBaud = 9600;
static const int32_t magicGpsValue = 10794 * 2;
static const int battVoltagePin = A0;    // 0...1023

const int msgBufferSize = 22;          // Fixed size of NTP record

static const uint8_t bikeState_Unknown = 'u'; 
static const uint8_t bikeState_Connected = 'c'; // (has Power @ home)
static const uint8_t bikeState_Driving = 'd'; // GPS has speed Signal
static const uint8_t bikeState_Sleeping = 's'; // GPS has no speed Signal
static const uint8_t bikeState_blocked = 'b'; // Bike blockiert
static const uint8_t bikeState_noGPS = 'g'; // 


#endif
