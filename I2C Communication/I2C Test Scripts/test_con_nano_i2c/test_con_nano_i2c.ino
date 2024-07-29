#include <Wire.h>

const int NANOADDR = 8;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Wire.begin(); // join i2c bus (address optional for writer)
    Serial.println("starting"); // transmit to device

}

void loop() {
    char charToWrite = Serial.read();
    //readSerial(charToWrite);
    
    Wire.beginTransmission(NANOADDR); // transmit to device
    Serial.println("Writing Character");
    Serial.println(charToWrite);
    Wire.write(charToWrite);        // sends the given value
    Wire.endTransmission();    // stop transmitting

    //Requests the nano to echo the sent character back
    int bytesToRead = 1;
    Wire.requestFrom(NANOADDR, bytesToRead);
    while (Wire.available()) {
        Serial.println("Recieving Character");
        char data = Wire.read();
        Serial.println(data);
    }

    delay(500);
}

/* Read input serial */
int readSerial(char result[]) {
    int i = 0;
    while (1) {
        while (Serial.available() > 0) {
            char inChar = Serial.read();
            if (inChar == '\n') {
                result[i] = '\0';
                Serial.flush();
                return 0;
            }

            if (inChar != '\r') {
                result[i] = inChar;
                i++;
            }
        }
    }
}