##ARDUINO BRAIN LIBRARY

###OVERVIEW

Brain is an Arduino Library for parsing data from Neurosky-based EEG headsets. Tested units include the Star Wars Force Trainer and Mattel MindFlex.

It's designed to make it simple to send out an ASCII string of comma-separated values over serial, or to access processed brain wave information directly in your Arduino sketch. See the examples for code demonstrating each use case.

Instructions for the hardware-end of this hack are available here: [http://frontiernerds.com/brain-hack](http://frontiernerds.com/brain-hack)

This library pairs nicely with the Processing-based [BrainGrapher](https://github.com/kitschpatrol/BrainGrapher).

The `getCSV()` function returns a string (well, char*) listing the most recent brain data, in the following format:
"signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma"

Signal strength ranges from 0 - 200. Counterintuitively, 0 means the unit has connected successfully, and 200 means there is no signal.

The attention and meditation values both run from 0 - 100. Intuitively, higher numbers represent more attention or meditation.

The EEG power values — delta, theta, etc... - are a heavily filtered representation of the relative activity in different brain wavelengths. These values can not be mapped directly to physical values (e.g. volts), but are still of use when considered over time or relative to each other.

###Repository Rename
This project was formerly "Arduino-Brain-Library" on GitHub, but was renamed to just "Brain" in 2014 to play nicely with the Arduino IDE's new ""Sketch --> Import Library..." menu item.

###SOFTWARE SERIAL
Past versions of this library relied on a separate branch implementing support for NewSoftSerial. Improvements to both the bundled SoftwareSerial library and the underlying class hierarchy in Arduino 1.0 has allowed a single version of the Brain library to cover both use cases. Please **do not use** the old [softserial](https://github.com/kitschpatrol/Brain/tree/softserial) branch of this library! Please see


###EXAMPLES
Three examples are provided with the library:

- `BrainBlinker.ino` blinks an LED faster as your "attention" value increases.
- `BrainSerialTest.ino` prints out a CSV of brain data over serial.
- `BrainSoftSerialTest.ino` reads brain data over SoftwareSerial and prints a CSV over hardware serial. 

###INSTALLATION
There are two ways to install the library:

#####Installation through the Arduino IDE
Download the [latest release](https://github.com/kitschpatrol/Brain/releases), and then in the Arduino IDE go to ""Sketch --> Import Library..." and point the file dialogue to the Brain.zip file you just downloaded.

#####Manual Installation
Download the [latest release](https://github.com/kitschpatrol/Brain/releases), decompress, rename it to Brain, and drag "Brain" folder to Arduino's "libraries" folder inside your sketch folder. (On the Mac, this is ~/Documents/Arduino/libraries by default. You may need to create this folder if it does not exist.) Restart the Arduino environment.


###FUNCTION OVERVIEW

- `Brain(HardwareSerial &_brainSerial)`  
Instantiates the brain library on a hardware serial port.

- `boolean update();`  
Call this in your main loop to read data from the headset. Returns true if there is a fresh packet.

- `char* readErrors();`  
Character string containing the most recent errors. Worth printing this out over serial if you're having trouble.

- `char* readCSV();`  
Character string with all of the latest brain values in a comma-delimited format. Intended to be printed over serial. The data is returned in this order: `signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma`


- `byte readSignalQuality();`  
Returns the latest signal quality reading. 200 is high, 0 is no signal. This (and the remainder of the functions) are mainly intended for use when you want the Arduino to use the brain data internally. (Saves you the hassle / memory expenditure of parsing the CSV.)

- `byte readAttention();`  
Returns the NeuroSky "eSense" attention value.

- `byte readMeditation();`  
Returns the NeuroSky "eSense" meditation value.

- `unsigned long* readPowerArray();`  
Returns an array of the eight power-band (FFT) values, in the same order as the CSV.

- `unsigned long readDelta();`  
Returns the delta (1-3Hz) power value, often associated with sleep.

- `unsigned long readTheta();`  
Returns the theta (4-7Hz) power value, associated with a relaxed, meditative state.

- `unsigned long readLowAlpha();`  
Returns the low alpha (8-9Hz) power value, higher when eyes are closed or you're relaxed/

- `unsigned long readHighAlpha();`  
Returns the high alpha (10-12Hz) power value.

- `unsigned long readLowBeta();`  
Returns the low beta (13-17Hz) power value, higher when you're alert and focused.

- `unsigned long readHighBeta();`  
Returns the high beta (18-30Hz) power value.

- `unsigned long readLowGamma();`  
Returns the low gamma (31-40Hz) power value, associated with multi-sensory processing.

- `unsigned long readMidGamma();`  
Returns the high gamma (41-50Hz) power value.


###MINUTIAE
Note that the connection to the Neurosky headset is half-duplex — it will use up the RX pin on your Arduino, but you will still be able to send data back to a PC via USB. (Although in this case you won't be able to send data to the Arduino from the PC.)

You may wish to read brain data over a software serial connection instead of the Arduino's hardware serial (this frees up the hardware serial for full-duplex operation). There's a different branch of this library designed for use with NewSoftSerial: [http://github.com/kitschpatrol/Arduino-Brain-Library/tree/softserial](http://github.com/kitschpatrol/Arduino-Brain-Library/tree/softserial)

The MindFlex provides access to more brain data than the Force Trainer — in addition to the signal strength, attention and meditation values, the MindFlex gives access to 8 bands of EEG power data.

Brain data comes in quite slowly... expect a fresh packet every second or so.


###CONTACT
Eric Mika  
ermika@gmail.com  
[http://frontiernerds.com](http://frontiernerds.com)


###COLOPHON
Created by Eric Mika at NYU ITP in the spring of 2010.
Updated for Arduino 1.0 in 2013.
Revised, retested, and cleaned up in early 2014.

My thanks to Tom Igoe for his assistance during development. Thanks to my collaborators Sofy Yuditskaya and Arturo Vidich. (E>)

Thanks also to [Eero af Heurlin](https://github.com/rambo) for making some considered changes in his fork.


###LICENSE
Copyright Eric Mika, 2010-2014
Licensed under the GNU Lesser General Public License.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program. If not, see http://www.gnu.org/licenses/.