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
*/

#include "SHT1x_CTR.h"

#define digitalWriteS(arg1, arg2) digitalWrite(arg1, arg2); delayMicroseconds(1)
#define digitalWriteL(arg1, arg2) digitalWrite(arg1, arg2); delayMicroseconds(3)

#define TIMESPAN(cmd) (cmd == READ_TEMPERATURE ? (statusRegister & RESOLUTION ? 80 : 320 ) : (cmd == READ_HUMIDITY & !(statusRegister & RESOLUTION) ? 80 : 20))

SHT1xController::SHT1xController(unsigned char dataPin, unsigned char clockPin) {
	this->dataPin = dataPin;
	this->clockPin = clockPin;
	
	pinMode(clockPin, OUTPUT); //sets up the clock line

	#ifndef SHT1X_SIMPLE 
		readStatusRegister(); //updates register for the first time
	#endif
}

float SHT1xController::readTemperatureC() {
	short val;						// Raw temperature value returned from sensor

	// Conversion coefficients from SHT15 datasheet
	const float d1 = -40.1;			// for 12/14 Bit (in Cº) @ 5V
	const float d2 = 0.01;			// for 14 Bit (in Cº) @ 5V

	lastError = sendCommand(READ_TEMPERATURE);

	if (lastError != RETURN_SUCCESS)
		return NAN; //error

	pinMode(dataPin, INPUT);

	short timeout = TIMESPAN(READ_TEMPERATURE);

	for (short i = 0; i < timeout/5; i++) {
		delay(5);
		if (digitalRead(dataPin) == LOW) {
			break;
		}
		else if (i == timeout)
			lastError = ERROR_TIMEOUT;
			return NAN; //error
	}

	// Fetch the value from the sensor
	val = shiftIn(true) << 8;

#ifndef SHT1X_NO_CRC_CHECK
	val |= shiftIn(true);

	unsigned char checksum = shiftIn(false); //sends the sensor into idle mode and retrieves checksum
	unsigned char crc = calcCRC(READ_TEMPERATURE, reverseOrder(statusRegister & 0xF));
	crc = calcCRC((val >> 8 & 0xF), crc);
	crc = calcCRC((val & 0xF), crc);

	if (crc != reverseOrder(checksum)) {
		lastError = ERROR_CRC_MISMATCH; //error
		return NAN;
	}
#else
	val |= shiftIn(false); //sends the sensor into idle mode
#endif
	
	if (statusRegister & RESOLUTION) { //12bit measure
		return (val * d2h) + d1;
	}
	else { //14bit
		return (val * d2) + d1;
	}
}

float SHT1xController::readTemperatureF() {
	short val;						// Raw temperature value returned from sensor
	// Conversion coefficients from SHT15 datasheet
	const float d1 = -40.2;			// for 12/14 Bit (in Fº) @ 5V
	const float d2 = 0.018;			// for 14 Bit (in Fº) @ 5V
	const float d2h = 0.072;		// for 12 Bit (in Fº) @ 5V

	lastError = sendCommand(READ_TEMPERATURE);

	if (lastError != RETURN_SUCCESS)
		return NAN; //error

	pinMode(dataPin, INPUT);

	short timeout = TIMESPAN(READ_TEMPERATURE);

	for (short i = 0; i < timeout/5; i++) {
		delay(5);
		if (digitalRead(dataPin) == LOW) {
			break;
		}
		else if (i == timeout)
			lastError = ERROR_TIMEOUT;
			return NAN; //error
	}

	// Fetch the value from the sensor
	val = shiftIn(true) << 8;

#ifndef SHT1X_NO_CRC_CHECK
	val |= shiftIn(true);

	unsigned char checksum = shiftIn(false); //sends the sensor into idle mode and retrieves checksum
	unsigned char crc = calcCRC(READ_TEMPERATURE, reverseOrder(statusRegister & 0xF));
	crc = calcCRC((val >> 8 & 0xF), crc);
	crc = calcCRC((val & 0xF), crc);

	if (crc != reverseOrder(checksum)) {
		lastError = ERROR_CRC_MISMATCH; //error
		return NAN;
	}
#else
	val |= shiftIn(false); //sends the sensor into idle mode
#endif
	if (statusRegister & RESOLUTION) { //12bit measure
		return (val * d2h) + d1;
	}
	else { //14bit
		return (val * d2) + d1;
	}
}

float SHT1xController::readHumidity() {
	short val;						// Raw humidity value returned from sensor

// Conversion coefficients from SHT15 datasheet (V3/V4)
	const float C1 = -4.0000;		// for 8/12 Bit
	const float C2 = 0.0405;		// for 12 Bit
	const float C3 = -2.8000E-6;	// for 12 Bit
	const float T1 = 0.01;			// for 8/12 Bit
	const float T2 = 0.00008;		// for 12 Bit 
	
	const float C2h = 0.6480;		// for 8 Bit
	const float C3h = -7.2000E-4;	// for 8 Bit
	const float T2h = 0.00128;		// for 8 Bit



	// Start transmission and send READ_HUMIDITY unsigned char

	lastError = sendCommand(READ_HUMIDITY);

	if (lastError != RETURN_SUCCESS) 
		return NAN; //error

	pinMode(dataPin, INPUT);

	short timeout = TIMESPAN(READ_HUMIDITY);

	for (short i = 0; i < timeout/5; i++) {
		delay(5);
		if (digitalRead(dataPin) == LOW) {
			break;
		}
		else if (i == timeout)
			lastError = ERROR_TIMEOUT;
			return NAN; //error
	}

	// Fetch the value from the sensor
	val = shiftIn(true) << 8;

#ifndef SHT1X_NO_CRC_CHECK
	val |= shiftIn(true);

	unsigned char checksum = shiftIn(false); //sends the sensor into idle mode and retrieves checksum
	unsigned char crc = calcCRC(READ_HUMIDITY, reverseOrder(statusRegister & 0xF));
	crc = calcCRC((val >> 8 & 0xF), crc);
	crc = calcCRC((val & 0xF), crc);
	
	if (crc != reverseOrder(checksum)) {
		lastError = ERROR_CRC_MISMATCH; //error
		return NAN;
	}
#else
	val |= shiftIn(false); //sends the sensor into idle mode
#endif
	if (statusRegister & RESOLUTION) { //8bit measure

		// Apply linear conversion to raw value
		return C1 + (C2h + C3h * val) * val;
	}
	else { //12bit
		// Apply linear conversion to raw value
		return C1 + (C2 + C3 * val) * val;
	}
}

float SHT1xController::readHumidity(float tempC){

// Conversion coefficients from SHT15 datasheet (V3/V4)
	const float C1 = -4.0000;		// for 8/12 Bit
	const float C2 = 0.0405;		// for 12 Bit
	const float C3 = -2.8000E-6;	// for 12 Bit
	const float T1 = 0.01;			// for 8/12 Bit
	const float T2 = 0.00008;		// for 12 Bit 
	
	const float C2h = 0.6480;		// for 8 Bit
	const float C3h = -7.2000E-4;	// for 8 Bit
	const float T2h = 0.00128;		// for 8 Bit
	
	float humidity = readHumidity();

	if (statusRegister & RESOLUTION) { //8bit measure

		int val = (-C2h + sqrt(C2h*C2h - 4*C3h*(C1 - humidity))) / (2*C3h);
		// Correct humidity value for current temperature
		return (tempC - 25.0) * (T1 + T2h * val) + humidity;
		
	}
	else { //12bit
		// Apply linear conversion to raw value
		int val = (-C2 + sqrt(C2*C2 - 4*C3*(C1 - humidity))) / (2*C3);
		
		// Correct humidity value for current temperature
		return (tempC - 25.0) * (T1 + T2 * val) + humidity;
	}
}

float SHT1xController::readDewpoint() {

	const float Tn = 243.12; 		//0 < T < 50
	const float m = 17.62;			//0 < T < 50
	const float Tnh = 272.62; 		//-40 < T < 0
	const float mh = 272.62; 		//-40 < T < 0

	float T = readTemperatureC();

	if (getLastError() != RETURN_SUCCESS){
		return T;
	}

#ifndef SHT1X_NO_SAFEGUARD
	delay(timeUntilUnblock()); //we need to force the program to sleep due to the safeguard
#endif

	float RH = readHumidity(T); 

	if (getLastError() != RETURN_SUCCESS){
		return RH;
	}
	
	if (T > 0){
		return Tn* ((log(RH/100) + (m*T)/(Tn + T)) / (m - log(RH/100) - (m*T)/(Tn + T)));
	}
	else{
		return Tnh* ((log(RH/100) + (mh*T)/(Tnh + T)) / (mh - log(RH/100) - (mh*T)/(Tnh + T)));
	}
}

void SHT1xController::reset(void) {
	resetConnection();
	sendCommand(SOFT_RESET);
}

SHT1xController::SHT_RETURN_TYPE SHT1xController::getLastError(){
	return lastError;
}

#ifndef SHT1X_NO_SAFEGUARD
unsigned long SHT1xController::timeUntilUnblock(){
	unsigned long a = 0.1*(millis() - timeStamp);
	unsigned long b = TIMESPAN(lastCmd);

	return a > b ? 0 : 10*(b - a);
}
#endif

#ifndef SHT1X_SIMPLE
SHT1xController::SHT_RETURN_TYPE SHT1xController::writeStatusRegister(unsigned char new_register) {
	lastError = sendCommand(WRITE_STATUS_REGISTER);

	if (lastError == RETURN_SUCCESS) {
		return shiftOut(statusRegister = new_register & 0x7); //masks writable bits
	}

	return lastError;
}

unsigned char SHT1xController::readStatusRegister() {
	lastError = sendCommand(READ_STATUS_REGISTER);

#ifndef SHT1X_NO_CRC_CHECK
		if (lastError == RETURN_SUCCESS) {
			statusRegister = shiftIn(true); //statusregister is only 8 bits long, so after this transmission we should recieve the checksum
			unsigned char checksum = shiftIn(false); //sends the sensor into idle mode and retrieves checksum
			unsigned char crc = calcCRC(READ_STATUS_REGISTER, reverseOrder(statusRegister & 0xF));
			crc = calcCRC(statusRegister, crc);
			
			if (crc != reverseOrder(checksum)) {
				lastError = ERROR_CRC_MISMATCH; //error
			}		
		}
#else
		if (lastError == RETURN_SUCCESS) {
			statusRegister = shiftIn(false); //sends the sensor into idle mode
		}
#endif

	return statusRegister;
}
#endif

SHT1xController::SHT_RETURN_TYPE SHT1xController::sendCommand(SHT_COMMAND_TYPE command) {

#ifndef SHT1X_NO_SAFEGUARD 
	if (!timeUntilUnblock()){ //can't be working for more than 10% of the time
		return lastError = ERROR_OVERLOAD;
	}
	timeStamp = millis();
	lastCmd = command;
#endif

	//begin transmission
	digitalWriteS(dataPin, HIGH); //prevents possible low pulse
	pinMode(dataPin, OUTPUT);
	
	digitalWriteL(clockPin, HIGH); //sends TRANSMISSION START sequence
	digitalWriteS(dataPin, LOW);
	
	digitalWriteL(clockPin, LOW);
	digitalWriteS(clockPin, HIGH);

	digitalWriteS(dataPin, HIGH);
	digitalWriteL(clockPin, LOW);

	if(shiftOut(command) == RETURN_SUCCESS){ //sends command to sensor

		if (command == SOFT_RESET) { //soft reset requested, status register is set to default (0)
			statusRegister = 0; //resets status register
			
			#ifdef SHT1X_NO_SAFEGUARD //without safeguard active, we need to protect the sensor with a delay
				delay(13); //sensor cannot recieve commands at least 11ms
			#endif
		}
	}
	return lastError;
}

SHT1xController::SHT_RETURN_TYPE SHT1xController::shiftOut(unsigned char data) {
	
	pinMode(dataPin, OUTPUT);
	digitalWrite(dataPin, LOW);

	for (unsigned char i = 0; i < 8; i++ )
	{
		digitalWriteS(clockPin, HIGH);
		digitalWrite(dataPin, data << i & 0x80); //Shifts the data by one bit at a time (MSB)
		digitalWriteS(clockPin, LOW);
	}

	pinMode(dataPin, INPUT); //sensor pushes line to LOW for ACK
	digitalWrite(dataPin, SHT1X_PULLUP_STATE); //restore internal pullup
	digitalWriteL(clockPin, HIGH);
	
	if(digitalRead(dataPin) != LOW){ //failure to acknowledge
		return lastError = ERROR_NO_ACK;
	}

	digitalWriteS(clockPin, LOW);
	return lastError = RETURN_SUCCESS;
}

unsigned char SHT1xController::shiftIn(bool sendAck) {

	unsigned char val = 0;
	pinMode(dataPin, INPUT);

	for (unsigned char i = 0; i < 8; i++ )
	{
		digitalWriteS(clockPin, HIGH);
		digitalWriteS(clockPin, LOW);
		val = val << 1 | digitalRead(dataPin); //Shifts bits and adds in the recieved one
	}

	pinMode(dataPin, OUTPUT);
	
	digitalWriteS(dataPin, !sendAck); //ACK is given by lowering the data line before sending a high pulse on clock line
	digitalWriteL(clockPin, HIGH);
	digitalWriteS(clockPin, LOW);

	pinMode(dataPin, INPUT);
	digitalWrite(dataPin, SHT1X_PULLUP_STATE); //restore internall pullup

	
	return val;
}

void SHT1xController::resetConnection() {

	digitalWriteS(dataPin, HIGH);
	pinMode(dataPin, OUTPUT);

	for (unsigned char i = 0; i < 9; i++) {
		digitalWriteL(clockPin, HIGH);
		digitalWriteL(clockPin, LOW);
	}
}


#ifndef SHT1X_NO_CRC_CHECK

unsigned char SHT1xController::calcCRC(unsigned char data, unsigned char crc) {
	
#ifdef SHT1X_USE_BITWISE_CRC
		const unsigned char poly = 0x31;   //P(x)=x^8+x^5+x^4+1 = |0001|0011|0001   ~ for CRC-8 checksum

		crc ^= data; //Checks which bits differ from the recieved/transmitted data

		for (unsigned char bit = 0; bit < 8; bit++) {
			if (crc & 0x80) crc = ((crc << 1) ^ poly) & 0xFF; //if the last bit is toggled => left shift and apply polynomial
			else crc <<= 1;
		}

		return crc;
#else
		//lookup table provided by Sensirion (check CRC-8 Checksum Calculation)
		const unsigned char CRC_Table[] = {0, 49, 98, 83, 196, 245, 166, 151, 185, 136,
		 219, 234, 125, 76, 31, 46, 67, 114, 33, 16, 135, 182, 229, 212, 
		 250, 203, 152, 169, 62, 15, 92, 109, 134, 183, 228, 213, 66, 115, 
		 32, 17, 63, 14, 93, 108, 251, 202, 153, 168, 197, 244, 167, 150, 
		 1, 48, 99, 82, 124, 77, 30, 47, 184, 137, 218, 235, 61, 12, 95, 
		 110, 249, 200, 155, 170, 132, 181, 230, 215, 64, 113, 34, 19, 126, 
		 79, 28, 45, 186, 139, 216, 233, 199, 246, 165, 148, 3, 50, 97, 80, 
		 187, 138, 217, 232, 127, 78, 29, 44, 2, 51, 96, 81, 198, 247, 164, 
		 149, 248, 201, 154, 171, 60, 13, 94, 111, 65, 112, 35, 18, 133, 180, 
		 231, 214, 122, 75, 24, 41, 190, 143, 220, 237, 195, 242, 161, 144, 
		 7,	54, 101, 84, 57, 8, 91, 106, 253, 204, 159, 174, 128, 177, 226, 
		 211, 68, 117, 38, 23, 252, 205, 158, 175, 56, 9, 90, 107, 69, 116,
		 39, 22, 129, 176, 227, 210, 191, 142, 221, 236, 123, 74, 25, 40, 
		 6, 55, 100, 85, 194, 243, 160, 145, 71, 118, 37, 20, 131, 178, 225, 
		 208, 254, 207, 156, 173, 58, 11, 88, 105, 4, 53, 102, 87, 192, 241, 
		 162, 147, 189, 140, 223, 238, 121, 72, 27, 42, 193, 240, 163, 146, 
		 5, 52, 103, 86, 120, 73, 26, 43, 188, 141, 222, 239, 130, 179, 224, 
		 209, 70, 119, 36, 21, 59, 10, 89, 104, 255, 206, 157, 172};

		return CRC_Table[crc ^ data];
#endif
}

unsigned char SHT1xController::reverseOrder(unsigned char data) {
	unsigned char r = 0;
	unsigned char s = 8;


	for (; data; data >>= 1) {
		r <<= 1;
		r |= data & 1;
		s--;
	}

	return r << s;
}
#endif
