//this sketch tests the functionality of nano_con_i2c.ino
//upload this test to an arduino connected via I2C to another arduino running nano_con_i2c.ino
#include <cppQueue.h>
#include <Wire.h>

const int PAYLOADADDR = 8;
const int REQTIMEOUT = 500;
const int DATASIZE = 1000;
const int I2CFREQ = 1000000;

const byte EMPTYFLAG = 0x00;
const byte DONEFLAG = 0xFF;

cppQueue inputBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);
bool testSucess = true;

unsigned long StartTime = 0;
unsigned long CurrentTime = 0;
unsigned long ElapsedTime = 0;

void setup() {
    byte inByte = 0;
    int flagByte = 8;
    int databytesRead = 0;
    int byteCount = 9;
    int i = 0;
    
    Serial.begin(9600);
    while (!Serial);
    Wire.begin();
    Wire.setClock(I2CFREQ);
    delay(1000);

    Serial.println("nano_con_i2c_test Starting");

    //Recieve/Store/Send Test------------------------------------------------------------------
    
    Serial.println("Starting Recieve/Store/Send Test Test");

    fillQueue();
    Serial.print("Sending Test Data...");
    StartTime = millis();
    while(inputBuffer.isEmpty() == false){
        byte outByte = 0;
        inputBuffer.pop(&outByte);
        Wire.beginTransmission(PAYLOADADDR); // transmit to device
        Wire.write(outByte);        // sends the byte from the buffer
        Wire.endTransmission();    // stop transmitting
    }
    ElapsedTime = millis() - StartTime;
    Serial.print("Sent in ");
    Serial.print(ElapsedTime);
    Serial.println("ms");

    Serial.print("Requesting Data...");
    StartTime = millis();
    while(i < DATASIZE){
        byteCount = Wire.requestFrom(PAYLOADADDR, 9);
        
        if(byteCount != 9){
            Serial.print("Failure: Expected 9 bytes, recieved ");
            Serial.println(byteCount, HEX);
            testSucess = false;
            i += 8;
        }
        
        flagByte = Wire.read();
        databytesRead = 0;


        while (Wire.available()) {
            if(databytesRead < flagByte){
                databytesRead++;
                inByte = Wire.read();
                
                if(inByte != i%256){
                    Serial.print("Failure: Expected byte ");
                    Serial.print(i%256, HEX);
                    Serial.print(", recieved ");
                    Serial.println(inByte, HEX);
                    testSucess = false;
                }

                if(outputBuffer.isFull()){
                    Serial.println("Failure: Output buffer full");
                    outputBuffer.flush();
                    testSucess = false;
                }
                
                outputBuffer.push(&inByte);
                i++;
            }else{
                Wire.read();
            }
        }
    }
    ElapsedTime = millis() - StartTime;
    Serial.print("Recieved in ");
    Serial.print(ElapsedTime);
    Serial.println("ms");
    Serial.println("Recieve/Store/Send Test Complete");

    //Empty Flag Test------------------------------------------------------------------
    Serial.println("Starting Empty Flag Test");

    byteCount = Wire.requestFrom(PAYLOADADDR, 9);

    if(byteCount != 9){
        Serial.print("Failure: Expected 9 bytes, recieved ");
        Serial.println(byteCount);
        testSucess = false;
    }
    
    flagByte = Wire.read();

    if(flagByte != EMPTYFLAG){
        Serial.print("Failure: Empty Flag, recieved ");
        Serial.println(flagByte);
        testSucess = false;
    }
    Serial.println("Empty Flag Test Complete");

    if(testSucess){
        Serial.println("All tests passed succesfully");
    }else{
        Serial.println("Test failed");
    }
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