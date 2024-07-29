#include <cppQueue.h>
#include <Wire.h>

const int ADDR = 8;
char c;
char liveData;

const int DATASIZE = 16;
cppQueue inputBuffer(sizeof(char), DATASIZE, FIFO);
cppQueue outputBuffer(sizeof(char), DATASIZE, FIFO);


void setup() {
	
	Serial.begin(9600);
	delay(1000);
	Serial.println("Payload Nano Starting");

	Wire.begin(ADDR);                // join i2c bus with address
	Wire.onReceive(I2CReceiveEvent); // function that executes whenever data is received from writer
	Wire.onRequest(I2CRequestEvent);
}

void loop() {
	//While loop pretends to be a process consuming data, moving a byte in the input buffer to the output buffer every second
	//This will get replaced with an actual tinyml process
	while(inputBuffer.isEmpty() == false){
		Serial.print("Processing ");
		inputBuffer.pop(&liveData);
		Serial.println(liveData);

		if(outputBuffer.isFull()){
			Serial.println("Output Buffer full! Flushing...");
			outputBuffer.flush();
		}

		outputBuffer.push(&liveData);
		delay(1000);
	}
}

void I2CReceiveEvent(int howMany) {
	char inByte;
	Serial.println("Character Recieved!");
	inByte = Wire.read();

	if(inputBuffer.isFull()){
		Serial.println("Input Buffer full! Flushing...");
		inputBuffer.flush();
	}

	inputBuffer.push(&inByte);
	Serial.println(inByte);
}

void I2CRequestEvent(){
	char outByte;
	Serial.println("Replying Character...");
	if (outputBuffer.isEmpty()){
		Serial.println("Not ready");
		//Sends a ! character to tell the controller no data is available yet
		Wire.write('!');
	}else{
		outputBuffer.pop(&outByte);
		Serial.println(outByte);
		Wire.write(outByte);
	}
	
}