# Remote Controlled Shoes

These exist with [some ubiquity](https://www.amazon.com/s?k=led+shoes+with+remote&ref=nb_sb_noss), but at the prompting of a friend I thought it might be fun to design a system that would work more flexibly in different form factors. Ultimately, one device sits as a combination BLE beacon and peripheral that can pair with a phone and transmit the LED state via BLE  advertising packets. 

## Files

**`ble_central_beacon.ino`**

Device acting as BLE central, ordinarily this pairs with peripherals but in this case simply scans for advertising packets, extracts the UUID, and if it is a match for its wristband, will sync its state. Deployed to any number of synced devices beyond shoes, e.g. LED headphones synced with music.

**`ble_beacon_wrist.ino`**

Device acting as the BLE beacon &mdash; pairs with a control phone as a peripheral to set the button states, and then upon button presses will adjust its state and transmit this information via the BLE beacon `minor` value. BLE beacon architecture is as follows:

* `uuid`: 16-byte identifier for all devices in a given family (e.g. LED shoes). First 8-bytes represent the entire family of these devices, second 8-bytes are a unique identifier for this instance.

* `major`: 2-byte category for a type of paired devices (i.e. wristband and two pairs of shoes, or set of 16 headphones, etc.)

* `minor`: 2-byte system color state

## In Action

*Cycling Through Basics:*

![its working](/media/working.gif)
