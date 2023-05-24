
// Accel Sensor
#include "MPU9250.h"
// Presure Sensor
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>


Adafruit_LPS22 lps; // Pressure Class opbject
MPU9250 mpu; // Accel Class object

float SEA_LEVEL_PRESSURE = 1013.25;  //pressure in hPa
bool rocketStart = false;


float ACCEL_Z_THRESHOLD = 0.00;  // acceleration in g   // number to detect launch
int ALT_SHUTOFF = 500;           // value of the margin of error of altitude
int NET_RESTING_ACCEL_VALUE = 1;

int SHUTOFF_ACCEL_COUNT = 500;  // the number of 0 accel values needed in a row before shut down
int nulAccelCounter = 0;        // var to store the number of 0 accel values in a row

float accelX = 0.0000;
float accelY = 0.0000;
float accelZ = 0.0000;
float pressureData = 0.00;
float tempData = 0.00;

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
  // check rocket start
  if (!rocketStart) {
    rocketStart = checkRocketStart();
  }

  // if the rocket has started then it will start to check if it has landed
  // if it has both started and landed for x loops then turn everything off
  if (rocketStart && checkLanded()) {
    nulAccelCounter++;
    // check to see if the checkLanded function is true for x amount of loops
    if (nulAccelCounter == SHUTOFF_ACCEL_COUNT) {
      Serial.println("CAMERA OFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
  }
  // the else makes sure that the camera will only turn off after the accel is 0 for x duration
  // if the rocket is still moving in the air (or has not started yet)
  else {
    // insure that the nulAccelCounter is 0 while the rocket is still moving
    nulAccelCounter = 0;
  }

  delay(50);
}

void read10DOFData(){
  // update the values of the sensors
  mpu.update();
  sensors_event_t temp;
  sensors_event_t pressure;
  lps.getEvent(&pressure, &temp);// get pressure

  pressureData = pressure.pressure;
  tempData = temp.temperature;
  accelX = mpu.getAccX();
  accelY = mpu.getAccY();
  accelZ = mpu.getAccZ();
}

bool checkRocketStart() {  // needs testing
  Serial.print("Rocket Start: ");
  Serial.println(accelZ >= ACCEL_Z_THRESHOLD);
  return accelZ >= ACCEL_Z_THRESHOLD;
}

bool checkLanded() {  // needs testing
  // check if landed, return if so, return false if not
  Serial.println(accelX);
  bool landed = false;
  // find the net accel
  float netAccel = sqrt(pow(accelX, 2) + pow(accelY, 2) + pow(accelZ, 2));
  Serial.print("Net Accel: ");
  Serial.println(netAccel);
  // check if net accel value is 1 ish (+ or - 0.5)
  if (NET_RESTING_ACCEL_VALUE - 0.5 <= netAccel && netAccel <= NET_RESTING_ACCEL_VALUE + 0.5) {
    // now that accel is 0 ish check the altitude just to double check it has landed
    float altitude = ((pow(SEA_LEVEL_PRESSURE / pressureData, (1 / 5.257)) - 1) * (tempData + 273.15)) / 0.0065;
    Serial.print("Altitude: ");
    Serial.println(altitude);
    if (altitude <= ALT_SHUTOFF) {
      // we know that it has landed
      landed = true;
    }
  }
  // Serial.print("Landed: ");
  // Serial.println(landed);
  return landed;  //landed is a bool and will turn true
}

void printData(){
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
}
