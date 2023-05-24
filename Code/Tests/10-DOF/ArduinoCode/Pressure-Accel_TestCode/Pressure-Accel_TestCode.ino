
// Accel Sensor
#include "MPU9250.h"
// Presure Sensor
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>


Adafruit_LPS22 lps; // Pressure Class opbject
MPU9250 mpu; // Accel Class object

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(500);

  initMPU();
  initLPS();

}

void initMPU(){
  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
        delay(5000);
    }
  }
}
void initLPS(){
  // Try to initialize!
  if (!lps.begin_I2C(0x5C)) {
    Serial.println("Failed to find LPS22 chip");
    while (1) { delay(10); }
  }
  Serial.println("LPS22 Found!");

  lps.setDataRate(LPS22_RATE_10_HZ);
  Serial.print("Data rate set to: ");
  switch (lps.getDataRate()) {
    case LPS22_RATE_ONE_SHOT: Serial.println("One Shot / Power Down"); break;
    case LPS22_RATE_1_HZ: Serial.println("1 Hz"); break;
    case LPS22_RATE_10_HZ: Serial.println("10 Hz"); break;
    case LPS22_RATE_25_HZ: Serial.println("25 Hz"); break;
    case LPS22_RATE_50_HZ: Serial.println("50 Hz"); break;
    case LPS22_RATE_75_HZ: Serial.println("75 Hz"); break;
  }
}

void loop() {
  mpu.update();
  sensors_event_t temp;
  sensors_event_t pressure;
  lps.getEvent(&pressure, &temp);// get pressure

  Serial.print("X, Y, Z Acc: ");
  Serial.print(mpu.getAccX(), 4);
  Serial.print(", ");
  Serial.print(mpu.getAccY(), 4);
  Serial.print(", ");
  Serial.println(mpu.getAccZ(), 4);

  Serial.print("Temperature: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.print("Pressure: ");Serial.print(pressure.pressure);Serial.println(" hPa");
  Serial.println("");
  delay(100);
}
