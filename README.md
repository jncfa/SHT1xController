SHT1X Library
========================================

This is a Arduino Library for the SHT1x series (SHT10, SHT11, SHT15) temperature / humidity sensors from Sensirion, supporting both V3 and V4 versions. (www.sensirion.com).
It allows temperature measurements both in Celsius and Fahrenheit degrees, as well as humidity (in %).

This library is an expansion of the SHT1x controller provided by Sparkfun. Provides advanced functionlality of the SHT1x sensors by taping into 
its status register, you can select measurement resolution (supporting 8/12/14 bits data measurements) for increased precision or speed.
It also contains an optional CRC-8 checksum calculation to verify data integrity. (It's active by default, read below to find out how to disable this feature)

Using preprocessor directives, you can enable/disable some non-essencial procedures to achieve better performance (extra space / faster processing):
	- SHT1X_BITWISE_CRC : Changes CRC-8 checksum calculation from a bytewise process (using a lookup table) to a bitwise (slower but frees up some space);
	- SHT1X_NO_SAFEGUARD : Disables SHT1X overheat safeguard procedures (reduces some space, but can damage for the sensor in long-term usage);
	- SHT1X_SIMPLE : Removes all non-essencial procedures (this enables all directives below and disables all status registry methods);
		- SHT1X_NO_CRC_CHECK : Disables the CRC-8 checksum calculation within the data measurements;
		- SHT1X_NO_PULLUP : Makes the data line value unknown (depends on the last bit emitted), but you gain extra memory space;
		- SHT1X_NO_DELAY_PULSE : Removes the delay inbetween pulses (may could result in malfunctioning);

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager. 

Documentation
--------------

* **[Installing an Arduino Library Guide](https://www.arduino.cc/en/Guide/Libraries#toc2)** - Basic information on how to install an Arduino library.
* **[Code Repository](https://github.com/jnfaria/SHT1xController)** - Main repository 

License Information
-------------------

The **code** is beerware; if you see me at the local, and you've found my code helpful, come buy me a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to all contributors and release anything derivative under the same license.

Distributed as-is; no warranty is given.

- jnfaria, 2017
