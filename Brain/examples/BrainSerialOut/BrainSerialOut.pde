// Arduino Brain Library
// Serial out example, 	grabs the brain data and sends CSV out over the hardware serial.
// Eric Mika, 2010

#include <Brain.h>

// Set up the brain parser, pass it the hardware serial object you want to listen on.
Brain brain(Serial);

void setup() {
	// Start the hardware serial.
	Serial.begin(9600);
}

void loop() {
	// Expect packets about once per second.
	if (brain.update()) {
		Serial.println(brain.getCSV());
	}
}