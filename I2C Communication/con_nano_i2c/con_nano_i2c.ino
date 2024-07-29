#include <cppQueue.h>
#include <Wire.h>

const int NANOADDR = 8;
const int REQTIMEOUT = 500;

const int DATASIZE = 16;
const char DEMODATA[DATASIZE] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};
cppQueue inputBuffer(sizeof(char), DATASIZE, FIFO);
cppQueue outputBuffer(sizeof(char), DATASIZE, FIFO);


void setup() {
    Serial.begin(9600);
    while (!Serial);
    Wire.begin(); // join i2c bus 
    delay(1000); //1 second delay to give Wire library time to initalise I2C

    Serial.println("Starting");

    //fills the input buffer with example data
    fillQueue();
}

void loop() {
    //checks if there is any data to send in the input buffer
    if(inputBuffer.isEmpty() == false){
        //Transmits entire contents of input buffer        
        while(inputBuffer.isEmpty() == false ){
            char byteToWrite;
            //Takes a byte/character out of the input buffer
            inputBuffer.pop(&byteToWrite);
            
            Serial.println("Transmitting Data");
            Serial.println(byteToWrite);

            Wire.beginTransmission(NANOADDR); // transmit to device
            Wire.write(byteToWrite);        // sends the byte from the buffer
            Wire.endTransmission();    // stop transmitting
        } 
        
        Serial.println("Transmission Complete");
    }

    //every loop, the controller queries the payload nano for output data
    char entry = '#';
    while(entry != '!'){
        Serial.println("Requesting Data");
        Wire.requestFrom(NANOADDR, 1);

        while (Wire.available()) {
            entry = Wire.read();
            //If the payload nano has nothing ready to send, the controller waits
            if(entry == '!'){
                Serial.println("Nano not ready");
                delay(REQTIMEOUT);
            }else{
            //If the payload nano has data to send, its saved to the controller output buffer
                Serial.print("Recieving Data ");
                
                //Flushes the output buffer once its full
                //This will get replaced with CAN output code
                if(outputBuffer.isFull()){
                    Serial.println("Output buffer full, flushing");
                    outputBuffer.flush();
                }

                outputBuffer.push(&entry);
                Serial.println(entry);
            }
        }
    }
}

//Fills the I2C input buffer with example data. This will be replaced with CAN input code
void fillQueue(){
    int i;
    for (i = 0 ; i < DATASIZE ; i++)
	{
		char entry = DEMODATA[i];
		inputBuffer.push(&entry);
	}
}