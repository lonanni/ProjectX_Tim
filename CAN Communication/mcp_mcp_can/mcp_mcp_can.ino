#include <cppQueue.h>
#include <CAN.h>

const int DATASIZE = 1000;
const int CANID = 0x12;

cppQueue DAVEInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue SAIRAInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue DMLOMInBuffer(sizeof(byte), DATASIZE, FIFO);
cppQueue inputBuffers[3] = {DAVEInBuffer, SAIRAInBuffer, DMLOMInBuffer};
cppQueue outputBuffer(sizeof(byte), DATASIZE, FIFO);

int payloadPendingIDIn = 0;
int payloadIDIn = 0;
int payloadIDOut = 0;

void setup(){
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  CAN.setPins(9,2);
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  CAN.onReceive(CANRecieveEvent);
}

/*

when data is recived from CAN, save it to the input buffer
read flag to determine payloadIDIn
when output buffer contains data, send it

*/

void loop(){
    if(inputBuffers[payloadIDIn].isEmpty() && payloadPendingIDIn != payloadIDIn){
        payloadIDIn = payloadPendingIDIn;
    }

    //transmits contents of input buffer to the can bus
    while(outputBuffer.isEmpty() == false ){
        byte outByte;
        //Takes a byte/character out of the input buffer        
        Serial.println("Transmitting data to CAN bus");

        CAN.beginPacket(CANID);
        int i = 0;
        while(i < 8 && outputBuffer.isEmpty() == false){
            outputBuffer.pop(&outByte);
            CAN.write(outByte);
        }
        CAN.endPacket();
    } 
}

void CANRecieveEvent(int packetSize) {
    // received a packet
    Serial.print("Received ");  
    if (CAN.packetExtended()) {
        Serial.print("extended ");
    }   
    if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
        Serial.print("RTR ");
    }   
    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);  
    switch(CAN.packetId()){ 
    }   
    if (CAN.packetRtr()) {
        Serial.print(" and requested length ");
        Serial.println(CAN.packetDlc());
    } else {
        Serial.print(" and length ");
        Serial.println(packetSize);   
        // only print packet data for non-RTR packets
        while (CAN.available()) {
          	byte inByte;
            inByte = CAN.read();
            if(inputBuffers[payloadIDIn].isFull()){
                Serial.println("Warning: Input buffer full");
                inputBuffers[payloadIDIn].flush();
            }
            inputBuffers[payloadIDIn].push(&inByte);
        }
    }
}