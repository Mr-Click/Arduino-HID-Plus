Arduino
========

* Arduino-HID-Plus is a fork of Arduino repository that includes some extra 
features on the HID Keyboard implementation and, in particular:

* support for multiple keyboards layouts with methods: Keyboard.setKeyLayout(); Keyboard.getKeyLayout()

* Currently, the platform supports US_LAYOUT and IT_LAYOUT where each layout is implemented with a different asciimap array.

* support for CAPSLOCK LED status with methods: Keyboard.setLedStatus(); Keyboard.getLedStatus(); Keyboard.isCapsLockOn()

* all these features have been implemented by patching the following files from arduino repository: hardware/arduino/avr/cores/arduino/[HID.cpp,USBAPI.h]


Installation
------------
* Download Arduino-HID-Plus repository with: ```git clone https://github.com/valerio-click/Arduino-HID-Plus.git```

* Compile ardunio with:```cd build/ &&  ant && ant run```

Credits
-------
* This project is a patchwork of several hacks and ideas found on-line:
- https://weizenspr.eu/2013/arduino-leonardo-und-das-keyboard-api-problem/
  where it is presented a patch to directly send keycodes via the Keyboard API (  available at (https://github.com/weizenspreu/Arduino/) 
- http://forum.arduino.cc/index.php?topic=173583.0 useful patch to receivei and set led status from HID driver (Keyboard.setLedstatus() and Keyboard.getLedstatus()). Useful to keep track of CAPS_LOCK status.
- http://unfuck.eu/2015/_media/archive:2014:slides:owning_stuff_via_usb.pdf nice presentation on how to exploit Arduino-Micro as a penetration testing tool.
