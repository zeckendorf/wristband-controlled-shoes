/*********************************************************************
Author: Sam Zeckendorf
  Date: 10/18/18
  File: ble_central_beacon.ino

Uses the Bluefruit NR52 feather module to sync states with 
 a BLE beacon and illuminate a string of WS2812 RGB LEDs
*********************************************************************/

/* TODO:
    - Get rid of delays in animations by using millis() and frame time modulus
    -
*/

// External libraries from Adafruit
#include <bluefruit.h>
#include <Adafruit_NeoPixel.h>

// Constant definitions -- these offsets within the ble advertising packet 
// are described here (page 10): http://www.ti.com/lit/an/swra475a/swra475a.pdf
// and also here: https://os.mbed.com/blog/entry/BLE-Beacons-URIBeacon-AltBeacons-iBeacon/
#define PIN 7
#define UUID_SIZE 16     //bytes
#define UUID_OFFSET 9    //bytes (Beacon Prefix: 3B flags + 2B header + 2B company ID + 1B secondary ID + 1B length = 9B)
#define ADV_SCAN_SIZE 31 //bytes
#define MINOR_OFFSET 28  //bytes
#define MINOR_SIZE 2     //bytes
#define NUM_LEDS 35 

// for color data
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// state for color
uint16_t state;

// id of beacon, to be generated via: https://www.uuidgenerator.net/
uint8_t target_uuid[UUID_SIZE] = 
{ 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

void setup()
{

  // LED setup
  strip.begin();
  strip.show();

  // BLE setup
  Serial.begin(115200);
  Serial.println("LED Shoe Listening for Wristband Beacon");
  Serial.println("-----------------------------------\n");
  
  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setName("LED_SHOE_0x0000");

  // Increase Blink rate to different from PrPh advertising mode
  //Bluefruit.setConnLedInterval(250);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */

  // set up scanner so we can monitor beacon packets
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true); // uses scan response packet as well
  Bluefruit.Scanner.start(0);            // 0 = Don't stop scanning after n seconds
}

/**
 * Callback invoked when scanner pick up an advertising data
 * @param report Structural advertising data
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // extract data from report (see nordic api)
  // https://developer.nordicsemi.com/nRF5_SDK/nRF51_SDK_v8.x.x/doc/8.0.0/s110/html/a00225.html
  uint8_t* data = report->data;

  // pull out uuid from data (slick array slicing using memcpy)
  uint8_t scanned_uuid[UUID_SIZE];
  memcpy(scanned_uuid, &data[UUID_OFFSET], sizeof scanned_uuid);

  // if the beacon is ours
  if (arraysEqual(scanned_uuid, target_uuid, UUID_SIZE))
  { 
    // extract state
    state = data[MINOR_OFFSET];
    Serial.print("Shoe Beacon State: ");
    Serial.print(state);
    Serial.print("\n-----------------------------\n");
  }
}

// helper function to determine if two arrays are equal
bool arraysEqual(uint8_t* a, uint8_t* b, int len)
{
  for(int i = 0; i < len; i++)
  {
    if(a[i] != b[i])
      return false;
  }
  return true;
}

// used for controlling LEDs
void loop()
{
  
  if (state == 0x00)
  {
    none();
  }
  else if (state == 0x01)
  {
    red();
  }
  else if (state == 0x02)
  {
    green();
  }
  else if (state == 0x03)
  {
    blue();
  }
  else if (state == 0x04)
  {
    theaterChase(strip.Color(127, 127, 127), 25);
  }
  else if (state == 0x05)
  {
    rainbow(5);
  }
}

/******************
 ***  LED Code! ***
 ******************/
void none() {
  uint16_t i;
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();
}

void red() {
  uint16_t i;
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255,0,0));
    }
    strip.show();
}

void blue() {
  uint16_t i;
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,255));
    }
    strip.show();
}

void green() {
  uint16_t i;
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,255,0));
    }
    strip.show();
}
 
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
