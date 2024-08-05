#include <cppQueue.h>
#include <Wire.h>

const int PAYLOADADDR[3] = {8,9,10};
const int REQTIMEOUT = 500;
const int DATASIZE = 1000;

const byte EMPTYFLAG = 0x00;
const byte DONEFLAG = 0xFF;

cppQueue DAVEInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue SAIRAInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue DMLOMInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue inputBuffers[3] = {DAVEInBuffer, SAIRAInBuffer, DMLOMInBuffer};
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);

byte inByte = 0;
int byteCount = 0;
int payloadIDIn = 1;
int payloadIDOut = 1;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Wire.begin(); // join i2c bus 
    delay(1000); //1 second delay to give Wire library time to initalise I2C

    Serial.println("Controller Nano Starting");

    //fills the input buffer with example data for testing, comment out this function for normal use
    fillQueue();
}

void loop() {
    //transmits contents of input buffer to the payload nano
    while(inputBuffers[payloadIDIn].isEmpty() == false ){
        byte outByte;
        //Takes a byte/character out of the input buffer
        inputBuffers[payloadIDIn].pop(&outByte);
        
        Serial.println("Transmitting data");
        Serial.println(outByte);

        Wire.beginTransmission(PAYLOADADDR[payloadIDIn]); // transmit to device
        Wire.write(outByte);        // sends the byte from the buffer
        Wire.endTransmission();    // stop transmitting
    } 


    inByte = 0;
    byteCount = 1;
    //repeatedly queries the payload nano for output data, only stopping when the nano has nothing to send
    while(byteCount == 1){
        Serial.println("Requesting data");
        //Requests two bytes to allow for 2 byte control flag, but 1 byte is expected for normal data transfer
        byteCount = Wire.requestFrom(PAYLOADADDR[payloadIDOut], 2);

        while (Wire.available()) {
            inByte = Wire.read();

            //If the payload nano sends data, its saved to the controller output buffer
            if(byteCount == 1){
                Serial.println("Recieving byte ");
                Serial.print(inByte);

                //Flushes the output buffer once its full
                //This will get replaced with CAN output code
                if(outputBuffer.isFull()){
                    Serial.println("Output buffer full: Flushing");
                    outputBuffer.flush();
                }

                outputBuffer.push(&inByte);
            }
            else if(byteCount == 2){ //if 2 bytes of data is recieved in a single read, it's a control message
                inByte = Wire.read();
                if(inByte == DONEFLAG){
                    Serial.println("Payload Nano finished with process");
                }else{
                    Serial.println("Payload Nano has nothing to send");
                    delay(REQTIMEOUT);
                }
                Wire.read();
                
            }
            else{
                Serial.println("Warning: no response from Payload Nano");
                delay(REQTIMEOUT);
            }
        }
    }
}

//Fills the I2C input buffer with example data for testing
void fillQueue(){
    byte DEMODATA[64] = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 
                           0x00, 0x01, 0x00, 0x00, 0xFF, 0xE1, 0x00, 0x2A, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x49, 0x49, 
                           0x2A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x31, 0x01, 0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 
                           0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x69, 0x63, 0x61, 0x73, 0x61, 0x00, 0x00};
    for (int i = 0 ; i < 64 ; i++)
	{
		byte entry = DEMODATA[i];
		inputBuffers[payloadIDIn].push(&entry);
	}
}