// Serial out Brain Lib Example

// If you want to send brain data out over serial (to a PC, for example),
// give the Brain class constructor a second argument with the serial
// object you want to use for output. Be sure to call update() in the main
// loop.

#include <NewSoftSerial.h>
#include <Brain.h>

// Set up the software serial to
// listen for brain serial data on pin 2.
NewSoftSerial brainSerial(2, 3);

// Set up the brain parser. It listens on the 
// first argument, and sends output to the second object.
Brain brain(brainSerial, Serial);

void setup() {
 // Start the hardware serial.
 Serial.begin(9600);
 
 // Send detailed packet info.
 brain.debug = true;
}

void loop() {
  // Grabs the brain data and sends CSV out over the hardware serial.
  // Expect packets about once per second.
  brain.update();
}
