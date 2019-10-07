/*********************************************************************
Author: Sam Zeckendorf
  Date: 10/18/18
  File: ble_beacon_origin.ino

Uses the NRF52 module to generate a state based on button presses 
and transmit them to remote sources for illumination of an LED strip
*********************************************************************/

#include <bluefruit.h>
#include <Bounce2.h>

#define MANUFACTURER_ID   0x00E0
#define MAJOR_ID   0xEEFF
#define BUTTON_PIN 7

/* TODO:
 *  - 
 */

uint8_t beaconUuid[16] = 
{ 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

// global vars
BLEBeacon beacon;
uint16_t state;
Bounce debouncer = Bounce(); // Instantiate a Bounce object

void setup()
{
  // BLE setup
  Serial.begin(115200);
  Serial.println("Wristband Beacon for LED Shoe");
  Bluefruit.begin();
  Bluefruit.setName("LED_WB_0x0000");
  Bluefruit.setTxPower(4); 

  // Initialize beacon with state in the minor packet
  state = 0x00;
  initBeacon();

  // Init button reader
  debouncer.attach(BUTTON_PIN, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds
  
}

void loop()
{
  // get button state
  debouncer.update(); // Update the Bounce instance

  // use serial port to set state
  receiveSerial();
  
  if ( debouncer.fell() ) {  // Call code if button transitions from HIGH to LOW
     
     state += 1;
     
     if (state > 5) 
      state = 0;

      // reinit beacon every loop (realistically should only do this when state changes)
      initBeacon();
      
     Serial.print("Current State: ");
     Serial.println(state, DEC);
   }
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
  beacon = BLEBeacon(beaconUuid, MAJOR_ID, state, -24);
    
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
