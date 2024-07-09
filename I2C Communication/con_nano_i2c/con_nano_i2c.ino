#include <Wire.h>

const int NANOADDR = 8;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Wire.begin(); // join i2c bus (address optional for writer)
}

void loop() {
    char charToWrite[0];
    readSerial(charToWrite);
    Serial.println(charToWrite);

    Wire.beginTransmission(NANOADDR); // transmit to device
    Wire.write(charToWrite);        // sends the given value
    Wire.endTransmission();    // stop transmitting

    //Requests the nano to echo the sent character back
    int bytesToRead = 1;
    Wire.requestFrom(NANOADDR, bytesToRead);
    while (Wire.available()) {
        char data = Wire.read();
        Serial.print(data);
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