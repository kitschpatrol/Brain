// Brain Lib Example

// You can access 
#include<NewSoftSerial.h>
#include <Brain.h>

// Set up the software serial to
// listen for brain serial data on pin 2.
NewSoftSerial brainSerial(2, 3);

// Set up the brain parser to listen on the
// software serial.
Brain brain(brainSerial);

void setup() {
}

void loop() {
  // Grabs the brain data. Expect updates about once per second.
  brain.update();
  
  // Here's the attention value. Runs from 0 - 100.
  brain.attention
  
  // (TK do something with it)
  
  // See the documentation or the library source for a full list
  // of brain variables.
}
