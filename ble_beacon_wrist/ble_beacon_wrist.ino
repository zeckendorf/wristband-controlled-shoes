#include <bluefruit.h>
#define MANUFACTURER_ID   0x00E0
#define MAJOR_ID   0xEEFF

/* TODO:
 *  - Adjust state based on button presses
 *  - 
 */

uint8_t beaconUuid[16] = 
{ 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

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
  // reinit beacon every loop (realistically should only do this when state changes)
  initBeacon();

  // use serial port to set state
  receiveSerial();

  // update state 
  /* state += 1;
  if (state > 0x0005)
  {
    state = 0x00;
  }*/
}

void receiveSerial(){
  
    // send data only when you receive data:
    if (Serial.available() > 0) {
            // read the incoming byte:
            state = Serial.parseInt();

            // say what you got:
            Serial.print("Current State: ");
            Serial.println(state, DEC);
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
