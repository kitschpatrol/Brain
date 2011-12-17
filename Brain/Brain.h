// To do:
// Data simulator

#ifndef Brain_h
#define Brain_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define MAX_PACKET_LENGTH 32
#define EEG_POWER_BANDS 8

// TK some kind of reset timer if we don't receive a packet for >3 seconds?

class Brain {
	public:
		// TK Support for soft serials. A more generic "Serial" type to inherit from would be great.
		// More info on passing Serial objects: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264179436
		Brain(HardwareSerial &_brainSerial);	

		// Run this in the main loop.
		boolean update();

		// String with most recent error.
		char* readErrors();

		// Returns comme-delimited string of all available brain data.
		// Sequence is as below.
		char* readCSV();

		// Individual pieces of brain data.
		byte readSignalQuality();
		byte readAttention();
		byte readMeditation();
		unsigned long* readPowerArray();
		unsigned long readDelta();
		unsigned long readTheta();
		unsigned long readLowAlpha();
		unsigned long readHighAlpha();
		unsigned long readLowBeta();
		unsigned long readHighBeta();
		unsigned long readLowGamma();
		unsigned long readMidGamma();

	private:
		HardwareSerial* brainSerial;		
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
		void clearEegPower();
		boolean parsePacket();
		void printPacket();
		void init();
		void printCSV(); // maybe should be public?		 
		void printDebug();

		// With current hardware, at most we would have...
		// 3 x 3 char bytes
		// 8 x 10 char ulongs
		// 10 x 1 char commas
		// 1 x 1 char 0 (string termination)
		// -------------------------
		// 100 characters		
		char csvBuffer[100];
		
		// Longest error is
		// 22 x 1 char bytes
		// 1 x 1 char 0 (string termination)
		char latestError[23];		
		
		byte signalQuality;
		byte attention;
		byte meditation;

		boolean freshPacket;
		
		// Lighter to just make this public, instead of using the getter?
		unsigned long eegPower[EEG_POWER_BANDS];
};

#endif