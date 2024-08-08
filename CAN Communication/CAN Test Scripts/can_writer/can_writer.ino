#include <cppQueue.h>
#include <MCP2515.h>

const int DATASIZE = 1000;
const int CANID = 0x11;
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);


void setup(){
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  fillQueue();
}

void loop(){
    while(outputBuffer.isEmpty() == false){
        byte outByte;
        //Takes a byte/character out of the input buffer
        Serial.println("Transmitting data to CAN bus");
        Serial.println(outputBuffer.getCount());

        CAN.beginPacket(CANID);
        int i = 0;
        while(i < 8 && outputBuffer.isEmpty() == false){
            i++;
            outputBuffer.pop(&outByte);
            CAN.write(outByte);
        }
        CAN.endPacket();
    }
    delay(1000);
}

//Fills the I2C input buffer with example data for testing
void fillQueue(){
    byte DEMODATA[64] = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 
                           0x00, 0x01, 0x00, 0x00, 0xFF, 0xE1, 0x00, 0x2A, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x49, 0x49, 
                           0x2A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x31, 0x01, 0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 
                           0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x69, 0x63, 0x61, 0x73, 0x61, 0x00, 0x00};
    for (int i = 0 ; i < 64 ; i++)
	{
		byte entry = i;
		outputBuffer.push(&entry);
	}
}