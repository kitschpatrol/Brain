#include "WProgram.h"
#include "Brain.h"

Brain::Brain(NewSoftSerial &_brainSerial) {
  // Would be nice if we could use the more generic Print parent class.
  // But then there's more cruft to maintain in the actual sketch.
  // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264179436  
  brainSerial = &_brainSerial;

  init();
}

Brain::Brain(NewSoftSerial &_brainSerial, Print &_printer) {
  brainSerial = &_brainSerial;
  printer = &_printer;

  init();
}

void Brain::init() {
  brainSerial->begin(9600);

  sendCSV = true;
  noisyErrors = true;
  debug = false;

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

  delta = 0;
  theta = 0;
  lowAlpha = 0;
  highAlpha = 0;
  lowBeta = 0;
  highBeta = 0;
  lowGamma = 0;
  midGamma = 0;
}

void Brain::update() {
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
        checksumAccumulator =  255 - checksumAccumulator;

        // Do they match?
        if (checksum == checksumAccumulator) {

          // Parse the data. parsePacker() returns true if parsing succeeds.
          if (parsePacket()) {
            // send the CSV out over serial
            
            // ??? Seems to work fine, but is this an issue when
            // ??? the printer doesn't actually point anyhwere?
            // ??? e.g. when the first constructor is used?
            if (sendCSV) printCSV();
            
            //if (debug) printDebug();
            if (debug) printPacket();
          }
          else {
            // Parsing failed, send an error.
            if (noisyErrors) printer->println("ERROR: Packet parsing failed.");
            if (debug) printPacket();            
          }
        }
        else {
          // Checksum mismatch, send an error.
          if (noisyErrors) printer->println("ERROR: Checksum mismatch.");          
          if (debug) printPacket();
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
      clearPacket(); // Zeros the packet array, technically not necessarry.      
    }
    
    // Keep track of the last byte so we can find the sync byte pairs.
    lastByte = latestByte;
  }
}

void Brain::clearPacket() {
  for (byte i = 0; i < MAX_PACKET_LENGTH; i++) {
    packetData[i] = 0;
  }  
}

boolean Brain::parsePacket() {
  // Loop through the packet, extracting data.
  // Based on mindset_communications_protocol.pdf from the Neurosky Mindset SDK.
  hasPower = false;
  
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
        hasPower = true;
        // ASIC_EEG_POWER: eight big-endian 3-byte unsigned integer values representing delta, theta, low-alpha high-alpha, low-beta, high-beta, low-gamma, and mid-gamma EEG band power values      

        // The next byte sets the length, usually 24 (Eight 24-byte numbers... big endian?)
        eegPowerLength = packetData[++i];

        // Extract the values. Possible memory savings here by creating three temp longs?
        delta = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        theta = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        lowAlpha = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];      
        highAlpha = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        lowBeta = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        highBeta = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        lowGamma = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
        midGamma = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];

        // This seems to happen once during start-up on the force trainer. Strange.

        break;        
      default:
        return false;
    }
  }
  return true;
}

// If the constructor received a print object, then assume we should send out the data.
void Brain::printCSV() {
  // Print the CSV
  printer->print(signalQuality, DEC);
  printer->print(",");
  printer->print(attention, DEC);            
  printer->print(",");
  printer->print(meditation, DEC);    

  if(hasPower) {
    printer->print(",");              
    printer->print(delta, DEC);
    printer->print(",");
    printer->print(theta, DEC);
    printer->print(",");
    printer->print(lowAlpha, DEC);
    printer->print(",");
    printer->print(highAlpha, DEC);
    printer->print(",");
    printer->print(lowBeta, DEC);
    printer->print(",");
    printer->print(highBeta, DEC);
    printer->print(",");
    printer->print(lowGamma, DEC);
    printer->print(",");
    printer->print(midGamma, DEC);         
  }

  printer->println("");
}

// For debugging, print the entire contents of the packet data array.
void Brain::printPacket() {
  printer->print("[");
  for (byte i = 0; i < MAX_PACKET_LENGTH; i++) {
    printer->print(packetData[i], DEC);

    if(i < MAX_PACKET_LENGTH - 1) {
      printer->print(", ");
    }
  }
  printer->println("]");
}

void Brain::printDebug() {
  printer->println("");  
  printer->println("--- Start Packet ---");
  printer->print("Signal Quality: ");
  printer->println(signalQuality, DEC);
  printer->print("Attention: ");
  printer->println(attention, DEC);
  printer->print("Meditation: ");
  printer->println(meditation, DEC);

  if(hasPower) {
    printer->println("");
    printer->println("EEG POWER:");    
    printer->print("Delta: ");              
    printer->println(delta, DEC);
    printer->print("Theta: ");
    printer->println(theta, DEC);
    printer->print("Low Alpha: ");
    printer->println(lowAlpha, DEC);
    printer->print("High Alpha: ");
    printer->println(highAlpha, DEC);
    printer->print("Low Beta: ");
    printer->println(lowBeta, DEC);
    printer->print("High Beta: ");
    printer->println(highBeta, DEC);
    printer->print("Low Gamma: ");
    printer->println(lowGamma, DEC);
    printer->print("Mid Gamma: ");
    printer->println(midGamma, DEC);         
  }
  
  printer->println("");
  printer->print("Checksum Calculated: ");
  printer->println(checksumAccumulator, DEC);
  printer->print("Checksum Expected: ");
  printer->println(checksum, DEC);

  printer->println("--- End Packet ---");
  printer->println("");  
}