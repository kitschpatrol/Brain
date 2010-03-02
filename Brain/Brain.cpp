#include "WProgram.h"
#include "Brain.h"

Brain::Brain(HardwareSerial &_brainSerial) {
	brainSerial = &_brainSerial;
	
	// Keep the rest of the initialization process in a separate method in case
	// we overload the constructor.
	init();
}

void Brain::init() {
	brainSerial->begin(9600);

	freshPacket = false;
	inPacket = false;
	packetIndex = 0;
	packetLength = 0;
	checksum = 0;
	checksumAccumulator = 0;
	eegPowerLength = 0;
	hasPower = false;
	
	signalQuality = 200;
	attention = 0;
	meditation = 0;

	clearEegPower();	
}

boolean Brain::update() {
	latestError = "";

	if (brainSerial->available()) {
		latestByte = brainSerial->read();

		// Build a packet if we know we're and not just listening for sync bytes.
		if (inPacket) {
		
			// First byte after the sync bytes is the length of the upcoming packet.
			if (packetIndex == 0) {
				packetLength = latestByte;

				// Catch error if packet is too long
				if (packetLength > MAX_PACKET_LENGTH) {
					// Packet exceeded max length
					// Send an error
					latestError = "ERROR: Packet too long";
					inPacket = false;
				}
			}
			else if (packetIndex <= packetLength) {
				// Run of the mill data bytes.
				
				// Print them here

				// Store the byte in an array for parsing later.
				packetData[packetIndex - 1] = latestByte;

				// Keep building the checksum.
				checksumAccumulator += latestByte;
			}
			else if (packetIndex > packetLength) {
				// We're at the end of the data payload.
				
				// Check the checksum.
				checksum = latestByte;
				checksumAccumulator = 255 - checksumAccumulator;

				// Do they match?
				if (checksum == checksumAccumulator) {

					// Parse the data. parsePacker() returns true if parsing succeeds.
					if (parsePacket()) {
						freshPacket = true;
					}
					else {
						// Parsing failed, send an error.
						latestError = "ERROR: Could not parse";
						// good place to print the packet if debugging
					}
				}
				else {
					// Checksum mismatch, send an error.
					latestError = "ERROR: Checksum";
					// good place to print the packet if debugging
				}
				// End of packet
				
				// Reset, prep for next packet
				inPacket = false;
			}
			
			packetIndex++;
		}
		
		// Look for the start of the packet
		if ((latestByte == 170) && (lastByte == 170) && !inPacket) {
			// Start of packet
			inPacket = true;
			packetIndex = 0;
			packetLength = 0; // Technically not necessarry.
			checksum = 0; // Technically not necessary.
			checksumAccumulator = 0;
			//clearPacket(); // Zeros the packet array, technically not necessarry.
			//clearEegPower(); // Zeros the EEG power. Necessary if hasPower turns false... better off on the getter end?	 
		}
		
		// Keep track of the last byte so we can find the sync byte pairs.
		lastByte = latestByte;
	}
	
	if(freshPacket) {
		freshPacket = false;
		return true;
	}
	else {
		return false;
	}
	
}

void Brain::clearPacket() {
	for (byte i = 0; i < MAX_PACKET_LENGTH; i++) {
		packetData[i] = 0;
	}	 
}

void Brain::clearEegPower() {
	// Zero the power bands.
	for(byte i = 0; i < EEG_POWER_BANDS; i++) {
		eegPower[i] = 0;
	}
}

boolean Brain::parsePacket() {
	// Loop through the packet, extracting data.
	// Based on mindset_communications_protocol.pdf from the Neurosky Mindset SDK.
	hasPower = false;
	clearEegPower();	// clear the eeg power to make sure we're honest about missing values... null would be better than 0.
	
	for (byte i = 0; i < packetLength; i++) {
		switch (packetData[i]) {
			case 2:
				signalQuality = packetData[++i];
				break;
			case 4:
				attention = packetData[++i];
				break;
			case 5:
				meditation = packetData[++i];
				break;
			case 131:
				// ASIC_EEG_POWER: eight big-endian 3-byte unsigned integer values representing delta, theta, low-alpha high-alpha, low-beta, high-beta, low-gamma, and mid-gamma EEG band power values			 
				// The next byte sets the length, usually 24 (Eight 24-bit numbers... big endian?)
				eegPowerLength = packetData[++i];

				// Extract the values. Possible memory savings here by creating three temp longs?
				for(int j = 0; j < EEG_POWER_BANDS; j++) {
					eegPower[j] = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
				}

				hasPower = true;
				// This seems to happen once during start-up on the force trainer. Strange.

				break;
			default:
				return false;
		}
	}
	return true;
}

// DEPRECATED
void Brain::printCSV() {
	// Print the CSV over serial
	brainSerial->print(signalQuality, DEC);
	brainSerial->print(",");
	brainSerial->print(attention, DEC);
	brainSerial->print(",");
	brainSerial->print(meditation, DEC);

	if (hasPower) {
		for(int i = 0; i < EEG_POWER_BANDS; i++) {
			brainSerial->print(",");
			brainSerial->print(eegPower[i], DEC);
		}
	}
 
	brainSerial->println("");
}

char* Brain::getErrors() {
	return latestError;
}

char* Brain::getCSV() {
	// spit out a big string?
	// find out how big this really needs to be 
	// should be popped off the stack once it goes out of scope?
	// make the character array as small as possible
	
	if(hasPower) {
		// With current hardware, at most we would have...
		// 3 x 3 char bytes
		// 8 x 10 char ulongs
		// 10 x 1 char commas
		// 1 x 1 char 0 (string termination)
		// -------------------------
		// 100 characters

		char csvBuffer[100];	
		
		sprintf(csvBuffer,"%d,%d,%d,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
			signalQuality,
			attention,
			meditation,
			eegPower[0],
			eegPower[1],
			eegPower[2],
			eegPower[3],
			eegPower[4],
			eegPower[5],
			eegPower[6],
			eegPower[7]
		);
		
		return csvBuffer;
	}
	else {
		// With current hardware, at most we would have...
		// 3 x 3 char bytes
		// 2 x 1 char commas
		// 1 x 1 char 0 (string termination)
		// -------------------------
		// 12 characters
		
		char csvBuffer[12];
		
		sprintf(csvBuffer,"%d,%d,%d",
			signalQuality,
			attention,
			meditation
		);
		
		return csvBuffer;
	}
}

// For debugging, print the entire contents of the packet data array.
void Brain::printPacket() {
	brainSerial->print("[");
	for (byte i = 0; i < MAX_PACKET_LENGTH; i++) {
		brainSerial->print(packetData[i], DEC);
 
			if (i < MAX_PACKET_LENGTH - 1) {
				brainSerial->print(", ");
			}
	}
	brainSerial->println("]");
}

void Brain::printDebug() {
	brainSerial->println("");	 
	brainSerial->println("--- Start Packet ---");
	brainSerial->print("Signal Quality: ");
	brainSerial->println(signalQuality, DEC);
	brainSerial->print("Attention: ");
	brainSerial->println(attention, DEC);
	brainSerial->print("Meditation: ");
	brainSerial->println(meditation, DEC);

	if (hasPower) {
		brainSerial->println("");
		brainSerial->println("EEG POWER:");
		brainSerial->print("Delta: ");
		brainSerial->println(eegPower[0], DEC);
		brainSerial->print("Theta: ");
		brainSerial->println(eegPower[1], DEC);
		brainSerial->print("Low Alpha: ");
		brainSerial->println(eegPower[2], DEC);
		brainSerial->print("High Alpha: ");
		brainSerial->println(eegPower[3], DEC);
		brainSerial->print("Low Beta: ");
		brainSerial->println(eegPower[4], DEC);
		brainSerial->print("High Beta: ");
		brainSerial->println(eegPower[5], DEC);
		brainSerial->print("Low Gamma: ");
		brainSerial->println(eegPower[6], DEC);
		brainSerial->print("Mid Gamma: ");
		brainSerial->println(eegPower[7], DEC);
	}

	brainSerial->println("");
	brainSerial->print("Checksum Calculated: ");
	brainSerial->println(checksumAccumulator, DEC);
	brainSerial->print("Checksum Expected: ");
	brainSerial->println(checksum, DEC);

	brainSerial->println("--- End Packet ---");
	brainSerial->println("");	 
}

byte Brain::getSignalQuality() {
	return signalQuality;
}

byte Brain::getAttention() {
	return attention;
}

byte Brain::getMeditation() {
	return meditation;
}

unsigned long* Brain::getPowerArray() {
	return eegPower;
}

unsigned long Brain::getDelta() {
	return eegPower[0];
}

unsigned long Brain::getTheta() {
	return eegPower[1];
}

unsigned long Brain::getLowAlpha() {
	return eegPower[2];
}

unsigned long Brain::getHighAlpha() {
	return eegPower[3];
}

unsigned long Brain::getLowBeta() {
	return eegPower[4];
}

unsigned long Brain::getHighBeta() {
	return eegPower[5];
}

unsigned long Brain::getLowGamma() {
	return eegPower[6];
}

unsigned long Brain::getMidGamma() {
	return eegPower[7];
}