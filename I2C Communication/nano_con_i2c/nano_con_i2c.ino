#include <cppQueue.h>
#include <Wire.h>

const int ADDR = 8;
const byte EMPTYFLAG = 0x00;
const byte DONEFLAG = 0xFF;
const int DATASIZE = 1000;
const int I2CFREQ = 1000000;

cppQueue inputBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);
byte liveData;
bool finished;

void setup() {
	Serial.begin(9600);
	Serial.println("Payload Nano starting");
	Wire.begin(ADDR);                // join i2c bus with address
	Wire.onReceive(I2CReceiveEvent); // function that executes whenever data is received from writer
	Wire.onRequest(I2CRequestEvent);
	Wire.setClock(I2CFREQ);
	delay(1000);
}

void loop() {
	//While loop pretends to be a process consuming data, moving a byte in the input buffer to the output buffer every second, this will get replaced with an actual tinyml process
	while(inputBuffer.isEmpty() == false){
		inputBuffer.pop(&liveData);
		if(outputBuffer.isFull()){
			Serial.println("Output buffer full: Flushing");
			outputBuffer.flush();
		}

		outputBuffer.push(&liveData);
	}
}

void I2CReceiveEvent(int howMany) {
	byte inByte;
	inByte = Wire.read();
	Serial.print("Recieving byte ");
	Serial.println(inByte, HEX);

	if(inputBuffer.isFull()){
		Serial.println("Input buffer full: Flushing");
		inputBuffer.flush();
	}

	inputBuffer.push(&inByte);
}

void I2CRequestEvent(){
	byte outByte;
	int dataBytes = min(outputBuffer.getCount(), 8);

	if(finished){
		Serial.println("Done with process");
		Wire.write(DONEFLAG);
	}else{
		Serial.print("Sending flag byte ");
		Serial.println(dataBytes, HEX);
		//Number of data bytes in message sent as first byte
		Wire.write(dataBytes);

		for(int i = 0; i < dataBytes; i++){
			outputBuffer.pop(&outByte);
			Wire.write(outByte);
			Serial.print("Sending byte ");
			Serial.println(outByte, HEX);
		}
	}
}