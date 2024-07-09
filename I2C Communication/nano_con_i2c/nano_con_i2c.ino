#include <Wire.h>

const int ADDR = 8;
char c;

void setup() {
	Wire.begin(ADDR);                // join i2c bus with address
	Wire.onReceive(I2CReceiveEvent); // function that executes whenever data is received from writer
	Wire.onRequest(I2CRequestEvent);

	Serial.begin(9600);
}

void loop() {
	//Add other functionality here
	delay(100);
}

void I2CReceiveEvent(int howMany) {
	c = Wire.read(); // receive a character
	Serial.println(c);
}

void I2CRequestEvent(){
	Wire.write(c)
}