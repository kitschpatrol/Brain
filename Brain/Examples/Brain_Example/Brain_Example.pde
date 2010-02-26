// Serial out Brain Lib Example

// If you want to send brain data out over serial (to a PC, for example),
// give the Brain class constructor a second argument with the serial
// object you want to use for output. Be sure to call update() in the main
// loop.

#include <Brain.h>

// Set up the brain parser. It listens on the 
// first argument, and sends output to thee second object.
Brain brain(Serial);

void setup() {
  // Why do I have to call this again here, when it's already called
  // in brain's init? If I don't, I get very strange behavior... printing to the serial
  // works in the init function, but update() never seems to get called.
  Serial.begin(9600);
  
 // Send detailed packet info.
  brain.sendCSV = true;
  brain.debug = true;
  brain.noisyErrors = false; 
  
}

void loop() {
  // Grabs the brain data and sends CSV out over the hardware serial.
  // Expect packets about once per second.
  brain.update();
}