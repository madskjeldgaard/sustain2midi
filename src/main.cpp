#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <MIDI.h>

#include <Bounce2.h>
Bounce2::Button sustainPedal1 = Bounce2::Button();

constexpr auto SUSTAIN_PIN = 0;
constexpr auto MIDI_VELOCITY = 127;
constexpr auto TO_CHANNEL = 1;

constexpr auto PIN = SUSTAIN_PIN;
constexpr auto PIN_MODE = INPUT_PULLUP;
constexpr auto INTERVAL_IN_MS = 5;

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);

static bool toggle_state1 = false;
static int output_note = 48; // Default output note

void handle_sustain() {
  sustainPedal1.update();

  if (sustainPedal1.pressed()) {

    if (toggle_state1) {
      MIDI.sendNoteOn(output_note, MIDI_VELOCITY, TO_CHANNEL);
    } else {
      MIDI.sendNoteOff(output_note, 0, TO_CHANNEL);
    }

    toggle_state1 = !toggle_state1;
  }
}

void handle_midi_note_on(byte channel, byte note, byte velocity) {

  // Send note off and reset toggle state before changing note

  if (toggle_state1) {
    MIDI.sendNoteOff(output_note, 0, TO_CHANNEL);
  }

  toggle_state1 = false;
  output_note = note;
}

void setup() {
  Serial.begin(115200);

  TinyUSBDevice.setManufacturerDescriptor("MadsKjeldgaard");
  TinyUSBDevice.setProductDescriptor("Sustain2Midi");

  usbMidi.begin();
  MIDI.begin();

  // Set the MIDI note on handling function
  MIDI.setHandleNoteOn(handle_midi_note_on);

  // Setup button
  sustainPedal1.attach(PIN, PIN_MODE);
  sustainPedal1.interval(INTERVAL_IN_MS);
  sustainPedal1.setPressedState(LOW);
}

void loop() {
  handle_sustain();
  MIDI.read(); // Add this line to read incoming MIDI messages
}
