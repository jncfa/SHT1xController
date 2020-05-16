/*
* SHT1x Expanded Library by jnfaria
*  Based in the library created by Sparkfun.
*
* Based on previous work by:
*	 Jonathan Oxer: <www.practicalarduino.com>
*    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
*    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
*
* Updated for Arduino 1.6.8 Library Manager  
* August 3, 2017
*
*
* Summary:
*	Manages communication with SHT1x series (SHT10, SHT11, SHT15) temperature / humidity sensors 
*	from Sensirion, supporting both V3 and V4 versions. (www.sensirion.com).
*
* Notes:
*	This library is an expansion of the SHT1x controller provided by Sparkfun. Providing advanced functionlality of the SHT1x module by taping into the  
*	sensor's status register, you can select measurement resolution (supporting 8/12/14 bits data measurements) for increased precision or speed.
*	It also contains an optional CRC-8 checksum calculation to verify data integrity. (See "Notes" to find out how to disable this feature)
*
*	Using preprocessor directives, you can disable some non-essencial procedures to achieve better performance (extra free space / faster processing):
*		- SHT1X_USE_BITWISE_CRC : Changes CRC-8 checksum calculation from a bytewise process (using a lookup table) to a bitwise (slower but frees up some space);
*		- SHT1X_NO_CRC_CHECK : Disables the CRC-8 checksum calculation within the data measurements;
*		- SHT1X_NO_SAFEGUARD : Disables SHT1X overheat safeguard procedures (removes all restrictions from measurement times, but can damage the sensor in long-term usage);
*
*/

#ifndef SHT1X_H //header guard
#define SHT1X_H

#include <Arduino.h>
#include <math.h>

#ifndef SHT1X_NO_PULLUP
#define SHT1X_PULLUP_STATE HIGH	//defines internal pullup state
#endif

class SHT1xController{
private:
	//list of sht1x commands
	enum SHT_COMMAND_TYPE : unsigned char {	//		Command description			            ADDRESS | COMMAND
		READ_TEMPERATURE = 0x03,			// Measure temperature command.		    		[000|00011]
		READ_HUMIDITY = 0x05,				// Measure relative humidity command.	    		[000|00101]
		WRITE_STATUS_REGISTER = 0x06,			// Write status register command.	    		[000|00110]
		READ_STATUS_REGISTER = 0x07,			// Read status register command.	    		[000|00111]
		SOFT_RESET = 0x1E				// Soft reset command.			    		[000|11110]
	};
	
public:
		//Function return types
	enum SHT_RETURN_TYPE : unsigned char {	//      Return type description
		RETURN_SUCCESS = 0,					// Indicates successful exit.
		ERROR_NO_ACK,						// Failure to acknowledge data from sensor.
		ERROR_TIMEOUT,						// Failure to recieve data within expected timeframe.	
		ERROR_CRC_MISMATCH,					// Transmission data corruption.
		ERROR_OVERLOAD,						// Sensor cannot be active for more than 10% of the time.
	};
	
	//Flags for the status register
	enum SHT_REGISTER_FLAGS : unsigned char {//								     ADDRESS | COMMAND	
		END_OF_BATTERY = 0x40,				 // [READ] Detects if VDD is below 2.47 V.		[0100|0000]
		HEATER = 0x04,						 // [READ/WRITE] Usage of sensor's heater.	[0000|0100]
		OTP_RELOAD = 0x02,					 // [READ/WRITE] No reload of calibration data.	[0000|0010]
		RESOLUTION = 0x01,					 // [READ/WRITE] Indicates resolution type.	[0000|0001]
	};
public:

	//Default class constructor. (@param dataPin : Arduino pin connected to the data line; @param clockPin : Arduino pin connected to the clock line)
	SHT1xController(unsigned char dataPin, unsigned char clockPin);

	//Reads current temperature in degrees Celsius.
	float readTemperatureC(void);

	//Reads the sensor's temperature Fahrenheit degrees.
	float readTemperatureF(void);

	//Reads current relative humidity from the sensor.
	float readHumidity(void);

	//Reads current temperature-corrected relative humidity. (@param tempC : Current temperature detected)
	float readHumidity(float tempC);

	//Calculates the dew point for the temperature range -40 to 50ºC.
	float readDewpoint(void);

	//Resets connection with sensor and clears status register.
	void reset(void);

	//Returns the last error detected.
	SHT_RETURN_TYPE getLastError(void);

#ifndef SHT1X_NO_SAFEGUARD

	//Tells user how long before the sensor is available for measurements.
	unsigned long timeUntilUnblock(void);
#endif
	
	//Modifies status register flags. (@param flags : Flags to be written on the register)
	SHT_RETURN_TYPE writeStatusRegister(unsigned char flags); 

	//Reads the current status register.
	unsigned char readStatusRegister(void);

protected: 
	unsigned char statusRegister;

private:
	//Starts a transmission with the sensor. (@param command : Command to be sent to the sensor)
	SHT_RETURN_TYPE sendCommand(SHT_COMMAND_TYPE command);

	//Sends 8 bits via serial communiation with MSBFIRST. (@param data : Data to be transmitted)
	SHT_RETURN_TYPE shiftOut(unsigned char data);

	//Reads 8 bits via serial communiation with MSBFIRST. (@param sendAck : Notifies sensor of a successful reception)
	unsigned char shiftIn(bool sendAck);

	//Resets connection with the sensor (status register remains the same). Note: this must be followed by a transmission.
	void resetConnection(void);

#ifndef SHT1X_NO_CRC_CHECK

	//Calculates CRC-8 for a single byte. (@param data : information recieved / transmitted; @param checksum : start value of checksum)
	static unsigned char calcCRC(unsigned char data, unsigned char checksum);

	//Reverses bit-order in a byte. (@param data : byte to be reversed)
	static unsigned char reverseOrder(unsigned char data);
#endif

protected:
	unsigned char dataPin;
	unsigned char lockPin;
	SHT_RETURN_TYPE lastError;

#ifndef SHT1X_NO_SAFEGUARD
	unsigned long timeStamp; 
	SHT_COMMAND_TYPE lastCmd;
#endif

};

#endif//SHT1X_H
