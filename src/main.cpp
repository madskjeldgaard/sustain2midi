#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <Bounce2.h>
#include <MIDI.h>
Bounce2::Button sustainPedal1 = Bounce2::Button();

#include <arduino-timer.h>
auto timer = timer_create_default();
Timer<1, micros>
    led_timer; // create a timer with 1 task and microsecond resolution

#define SUS2MIDI_NEOPIXEL = true
#define DEBUG = false

constexpr auto SUSTAIN_PIN = 0;
constexpr auto MIDI_VELOCITY = 127;
constexpr auto TO_CHANNEL = 1;

constexpr auto PIN = SUSTAIN_PIN;
constexpr auto PIN_MODE = INPUT_PULLUP;
constexpr auto INTERVAL_IN_MS = 5;

#ifdef SUS2MIDI_NEOPIXEL
#include <Adafruit_NeoPixel.h>
constexpr auto LED_BRIGHTNESS = 255; // Set LED brightness (0-255)
constexpr auto LED_PIN = 5;          // Change this to your LED pin
constexpr auto NUM_LEDS = 1;         // Change this to the number of LEDs
// NeoPixel object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);

static bool toggle_state1 = true;

#ifdef SUS2MIDI_NEOPIXEL
static int output_note = 48; // Default output note
static int red_value = 0;    // Red color value
static int green_value = 0;  // Green color value

void update_led(bool is_on) {
  if (is_on) {
    strip.setPixelColor(0, strip.Color(red_value, green_value, 0));
  } else {
    strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn off LED
  }
  strip.show(); // Update LED
}

#endif

void handle_sustain() {
  sustainPedal1.update();

  if (sustainPedal1.released()) {

#ifdef DEBUG
    Serial.println("Sustain pedal released");
    Serial.println("Toggle state is: ");
    Serial.println(toggle_state1 ? "on" : "off");
#endif

    if (toggle_state1) {
      MIDI.sendNoteOn(output_note, MIDI_VELOCITY, TO_CHANNEL);
    } else {
      MIDI.sendNoteOff(output_note, 0, TO_CHANNEL);
    }

#ifdef SUS2MIDI_NEOPIXEL
    update_led(toggle_state1);
#endif

    toggle_state1 = !toggle_state1;
  }
}

void handle_midi_note_on(byte channel, byte note, byte velocity) {

  // Send note off and reset toggle state before changing note
  if (toggle_state1) {
#ifdef DEBUG
    Serial.println("Toggle state is on, sending note off just in case");
#endif
    MIDI.sendNoteOff(output_note, 0, TO_CHANNEL);
  }

  sustainPedal1.update();
#ifdef DEBUG
  Serial.print("Received note on: ");
  Serial.println(note);
#endif
  output_note = note;

#ifdef SUS2MIDI_NEOPIXEL
  // Map note number to red and green color values
  red_value = map(output_note, 0, 127, 255, 0);
  green_value = map(output_note, 0, 127, 0, 255);

  // Briefly flash the LED to indicate the new note
  update_led(true);
  timer.in(150, [](void *) -> bool {
    update_led(false);
    return false;
  });

#endif
}

void setup() {
  Serial.begin(115200);

#ifdef DEBUG
  Serial.println("Sustain2Midi");
  Serial.println("Initializing...");
#endif

  TinyUSBDevice.setManufacturerDescriptor("MadsKjeldgaard");
  TinyUSBDevice.setProductDescriptor("Sustain2Midi");

  usbMidi.begin();
  MIDI.begin();

  // Set the MIDI note on handling function
  MIDI.setHandleNoteOn(handle_midi_note_on);
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    // Do nothing
  });

  // Turn midi thru off
  MIDI.turnThruOff();

  // Setup button
  sustainPedal1.attach(PIN, PIN_MODE);
  sustainPedal1.interval(INTERVAL_IN_MS);
  sustainPedal1.setPressedState(LOW);
  sustainPedal1.update();

// Update button state to initialize toggle state
#ifdef SUS2MIDI_NEOPIXEL
  // Initialize NeoPixel strip
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS); // Set LED brightness
  // Map note number to red and green color values
  red_value = map(output_note, 0, 127, 255, 0);
  green_value = map(output_note, 0, 127, 0, 255);
  strip.show(); // Initialize all pixels to 'off'
#endif
}

void loop() {
  MIDI.read();
  handle_sustain();
  timer.tick();
}
