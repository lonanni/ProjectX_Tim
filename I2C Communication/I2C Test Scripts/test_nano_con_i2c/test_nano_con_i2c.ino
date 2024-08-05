//this sketch tests the functionality of nano_con_i2c.ino
//upload this test to an arduino connected via I2C to another arduino running nano_con_i2c.ino
#include <cppQueue.h>
#include <Wire.h>

const int PAYLOADADDR = 8;
const int REQTIMEOUT = 500;
const int DATASIZE = 1000;

const byte EMPTYFLAG = 0x00;
const byte DONEFLAG = 0xFF;

cppQueue inputBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);



void setup() {
    byte outByte = 0;
    byte inByte = 0;
    int byteCount = 0;

    Serial.begin(9600);
    while (!Serial);
    Wire.begin();
    delay(1000);

    Serial.println("nano_con_i2c_test Starting");

    //Recieve/Store/Send Test------------------------------------------------------------------
    fillQueue();

    while(inputBuffer.isEmpty() == false ){
        inputBuffer.pop(&outByte);
        Wire.beginTransmission(PAYLOADADDR); // transmit to device
        Wire.write(outByte);        // sends the byte from the buffer
        Wire.endTransmission();    // stop transmitting
    }

    for(int i = 0; i < DATASIZE; i ++){
        byteCount = Wire.requestFrom(PAYLOADADDR, 2);
        while (Wire.available()) {
            inByte = Wire.read();
            if(byteCount != 1 || inByte != i%256){
                Serial.println("Expected 1 byte: ");
                Serial.print(i%256);
                Serial.print("Recieved ");
                Serial.print(byteCount);
                Serial.print(" bytes: ");
                Serial.print(outByte);
            }
        }
    }

    Serial.println("Send/Store/Recieve Test Complete");

    //Empty Flag Test------------------------------------------------------------------
    byteCount = Wire.requestFrom(PAYLOADADDR, 2);
    while (Wire.available()) {
        inByte = Wire.read();
        if(byteCount != 2 || inByte != EMPTYFLAG){
            Serial.println("Expected 2 bytes: ");
            Serial.print(EMPTYFLAG);
            Serial.print("Recieved ");
            Serial.print(byteCount);
            Serial.print(" bytes: ");
            Serial.print(inByte);
        }
    }
    Serial.println("Empty Flag Test Complete");

}

void loop() {

}

//Fills the I2C input buffer with example data for testing
void fillQueue(){
    for (int i = 0 ; i < DATASIZE ; i++)
	{
		byte entry = i%256;
		inputBuffer.push(&entry);
	}
}