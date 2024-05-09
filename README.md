[![PlatformIO CI](https://github.com/madskjeldgaard/raspberry-pi-pico-usbmidi-platformio-template/actions/workflows/build.yml/badge.svg)](https://github.com/madskjeldgaard/raspberry-pi-pico-usbmidi-platformio-template/actions/workflows/build.yml)

# Sustain2Midi

![the hardware](pico.jpg) 

This project is a simple Sustain pedal to midi note converter based on a raspberry pi pico board.

## Features

- Dirt cheap (all you need is a mono jack socket and a raspberry pi pico)
- Sends note on/off
- Note number can be changed easily by sending a midi note to the device.
- (Optional) Use a WS2812 NeoPixel to show the state and midi note value via colour (I use [this module](https://www.aliexpress.com/item/1005001565556198.html?spm=a2g0o.order_list.order_list_main.4.17d91802H9j6ak) but I think any one should work) – disable this using the `SUS2MIDI_NEOPIXEL` build flag in the .ini file.

## Dependencies

You need to have platformio installed to make use of this. I prefer using the command line version.

Install it on MacOS by running homebrew:

```bash
brew install platformio
```

## Compiling and uploading

To compile and upload your sketch, simply run the following command:

```bash
pio run -t upload
```
