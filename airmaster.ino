#include <Arduino.h>
#include <SoftwareSerial.h>

const byte rxPin = 13;
const byte txPin = 15;
SoftwareSerial airMasterSerial(rxPin, txPin);

int LED = 2; // Assign LED pin i.e: D1 on NodeMCU

void setup() {
  pinMode(LED, OUTPUT);
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Initialize serial communication at 19200 bits per second:
  airMasterSerial.begin(19200);
  digitalWrite(LED, HIGH);

  Serial.begin(9600);
}

void loop() {
  digitalWrite(LED, HIGH); // turn the LED off

  // Send command to the sensor:
  byte command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
  airMasterSerial.write(command, sizeof(command));

  // Wait for 2 seconds (2000 milliseconds):
  delay(1000);

  // Check if at least 40 bytes are available to read:
  if(airMasterSerial.available() >= 40) {
    byte buffer[40];
    // Read 40 bytes into the buffer:
    for(int i = 0; i < 40; i++) {
      airMasterSerial.readBytes(buffer, 40);
    }

    // Process the data:
    unsigned int pm25 = buffer[2] | buffer[1] << 8;
    unsigned int pm10 = buffer[4] | buffer[3] << 8;
    unsigned int hcho = buffer[6] | buffer[5] << 8;
    unsigned int tvoc = buffer[8] | buffer[7] << 8;
    unsigned int co2 = buffer[10] | buffer[9] << 8;
    double temp = (buffer[12] | buffer[11] << 8) / 100.0;
    double humidity = (buffer[14] | buffer[13] << 8) / 100.0;

    unsigned int ppm03 = buffer[20] | buffer[19] << 8;
    unsigned int ppm05 = buffer[22] | buffer[21] << 8;
    unsigned int ppm1 = buffer[24] | buffer[23] << 8;
    unsigned int ppm2 = buffer[26] | buffer[25] << 8;
    unsigned int ppm5 = buffer[28] | buffer[27] << 8;
    unsigned int ppm10 = buffer[30] | buffer[29] << 8;

    unsigned int check = buffer[37] | buffer[36] << 8;
    unsigned int sum = 0;
    for(int i = 0; i < 36; i++) {
      sum += buffer[i];
    }

    if (check == sum && check != 0) {
      digitalWrite(LED, LOW); // turn the LED on
      
      // Print the data:
      Serial.print("PM2.5: "); Serial.print(pm25); Serial.println(" ug/m3");
      Serial.print("PM10: "); Serial.print(pm10); Serial.println(" ug/m3");
      Serial.print("HCHO: "); Serial.print(hcho); Serial.println(" ug/m3");
      Serial.print("TVOC: "); Serial.print(tvoc); Serial.println(" ug/m3");
      Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
      Serial.print("T: "); Serial.print(temp); Serial.println(" °C");
      Serial.print("H: "); Serial.print(humidity); Serial.println(" %");

      Serial.print("PM0.3: "); Serial.print(ppm03); Serial.println(" ppm");
      Serial.print("PM0.5: "); Serial.print(ppm05); Serial.println(" ppm");
      Serial.print("PM1.0: "); Serial.print(ppm1); Serial.println(" ppm");
      Serial.print("PM2.5: "); Serial.print(ppm2); Serial.println(" ppm");
      Serial.print("PM5.0: "); Serial.print(ppm5); Serial.println(" ppm");
      Serial.print("PM10: "); Serial.print(ppm10); Serial.println(" ppm");

      delay(5000); // wait for 5 seconds
    } else {
      Serial.println("Reading data...");
    }
  }
  // Adding a small delay ensures that we don't continuously poll the serial buffer:
  delay(1000);
}