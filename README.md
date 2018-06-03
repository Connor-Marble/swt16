# Robaux SWT16 Firmware

## Attention: When updating the firmware, all patterns and settings are overwritten!

## Version 1.1.1
### New 
### Improvment
* Better visualization of mute mode.
* If the animations are disabled, the screen saver will also be disabled.

---

## Version 1.1.0
### New 
#### 64 Steps per Track
* There is a new pattern concept: instead of 16 steps there are now 64 (8x8) steps per track! [Watch the demo video!](https://youtu.be/YANQiaSv0HA)

#### Debug-Mode
* To check if all triggers, buttons and LEDs are working, there is now the debug mode. Go to Util Mode (Button 16)> Button 13
* The sequencer is stopped. If you then press the buttons 1-16, the respective lamp lights up and the corresponding trigger is triggered. To exit the debug mode, press the home button.

---

## Version 1.0.7
### New
* Flip Mode > Util Mode (Button 16) > Button 3
### Improvment
* Continuous triggers even when track is muted.
* Factory Reset now in > Util Mode (Button 16) > Button 8

---

## Update
The nano board included in the kit can be updated in the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

[Apola Kipso has written a very detailed guide that can help you bring the update to the SWT16 module.](https://medium.com/@apolakipso/update-the-robaux-swt16-firmware-dcae16ad48c4)

### Driver
[Apple Macintosh](http://www.wch.cn/download/CH341SER_MAC_ZIP.html)

[Microsoft Windows](http://www.wch.cn/download/CH341SER_EXE.html)

### Notes:
*[Upload your code on a Macintosh.](https://kig.re/2014/12/31/how-to-use-arduino-nano-mini-pro-with-CH340G-on-mac-osx-yosemite.html)*