#include <bluefruit.h>


// Beacon uses the Manufacturer Specific Data field in the advertising
// packet, which means you must provide a valid Manufacturer ID. Update
// the field below to an appropriate value. For a list of valid IDs see:
// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
// 0x004C is Apple (for example)
#define MANUFACTURER_ID   0x00E0
#define MAJOR_ID   0xEEFF

// AirLocate UUID: E2C56DB5-DFFB-48D2-B060-D0F5A71096E0
uint8_t beaconUuid[16] = 
{ 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon;
uint16_t state;

void setup()
{

  // BLE setup
  Serial.begin(115200);
  Serial.println("Wristband Beacon for LED Shoe");
  Bluefruit.begin();
  Bluefruit.setName("LED_WB_0x0000");

  // Initialize beacon with state in the minor packet
  state = 0x00;
  initBeacon();
  
}

void loop()
{
  initBeacon();
  delay(800);
  state += 1;
  if (state > 0x0005)
  {
    state = 0x00;
  }
}

/*------------------------------------------------------------------*/
/* Beacon!
 *------------------------------------------------------------------*/

void initBeacon()
{
  // initialize beacon, accounts for changing state
  beacon = BLEBeacon(beaconUuid, MAJOR_ID, state, -54);
    
  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);
  setupAdv();
}

void setupAdv(void)
{  
  // Set the beacon payload using the BLEBeacon class populated
  // earlier in this example
  Bluefruit.Advertising.setBeacon(beacon);

  // Start advertising
  Bluefruit.Advertising.start();
}
