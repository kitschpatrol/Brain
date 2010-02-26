// To do:
// Keyword colors
// Data simulator

#ifndef Brain_h
#define Brain_h

#include "WProgram.h"
#include "NewSoftSerial.h"

#define MAX_PACKET_LENGTH 32

class Brain {
	public:
	  // ??? Any way to instantiate the NewSoftSerial in the library itself?
	  // Use this constructor if you want to use the brain values internally,
	  // or if you want to write your own serial output code.
		Brain(NewSoftSerial &_brainSerial);
		
		// Use this constructor if you want automatic CSV serial out
		Brain(NewSoftSerial &_brainSerial, Print &_printer);		
		
		// ??? Any way to loop this asynchronously in the library without wasting a timer?
		// Put this in the loop() of your sketch.
    void update();
    
    boolean sendCSV;
    boolean noisyErrors;
    boolean debug;
    
    // ??? Better off using getters for these?
    // ??? Better off using the inttypes?
    // ??? I just need unsigned 8 bit ints.
    byte signalQuality;
    byte attention;
    byte meditation;
    
    // ??? Is there a 24 bit unsigned integer type?
    // ??? Each of these is wasting a byte.
    unsigned long delta;
    unsigned long theta;
    unsigned long lowAlpha;
    unsigned long highAlpha;
    unsigned long lowBeta;
    unsigned long highBeta;
    unsigned long lowGamma;
    unsigned long midGamma;
		
  private:
    NewSoftSerial* brainSerial;
    Print* printer;
    byte packetData[MAX_PACKET_LENGTH];
    boolean inPacket;
    byte latestByte;
    byte lastByte;
    byte packetIndex;
    byte packetLength;
    byte checksum;
    byte checksumAccumulator;
    byte eegPowerLength;
    boolean hasPower;
    void clearPacket();
    boolean parsePacket();
    void printPacket();
    void init();
    void printCSV(); // maybe should be public?    
    void printDebug();


};

#endif