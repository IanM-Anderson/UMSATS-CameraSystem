// main program to run the camera system
#include "lps22hb.h"
#include "imu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

IMU_EN_SENSOR_TYPE enMotionSensorType;
IMU_ST_ANGLES_DATA stAngles;
IMU_ST_SENSOR_DATA stGyroRawData;
IMU_ST_SENSOR_DATA stAccelRawData;
IMU_ST_SENSOR_DATA stMagnRawData;
float PRESS_DATA = 0;
float TEMP_DATA = 0;
uint8_t u8Buf[3];

// ---------------------------------------- Variables ----------------------------------------------
float SEA_LEVEL_PRESSURE = 1013.25;  //pressure in hPa
bool rocketStart = false;
// DataLogger:
const int _MISO = 16;
const int _MOSI = 19;
const int _CS = 17;
const int _SCK = 18;

// 10-DOF:
/*
 * The 2D Array for 10-DOF is formatted as:
 * [0] -> Angle (roll,pitch,yaw)
 * [1] -> Pressure (pressure,0,0)
 * [2] -> Temperature (temperature,0,0)
 * [3] -> Acceleration (x,y,z)
 * [4] -> Gyroscope (x,y,z)
 * [5] -> Magnetic (x,y,z)
 */
float dataArray[5][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
float ACCEL_Z_THRESHOLD = 0.00;  // acceleration in g   // number to detect launch
int ALT_SHUTOFF = 500;           // value of the margin of error of altitude
int NET_RESTING_ACCEL_VALUE = 17000;

int SHUTOFF_ACCEL_COUNT = 500;  // the number of 0 accel values needed in a row before shut down
int nulAccelCounter = 0;        // var to store the number of 0 accel values in a row
// ------------------------------------------------------------------------------------------------------

void beepBepper(int pin, int frequency, int duration) {
  // send a tone with frequency to a pin
  tone(pin, frequency);
  delay(duration);  // keep the tone going for some duration
  noTone(pin);      // turn off the tone
  delay(duration);
}

void setup() {
  Serial.begin(115200);
  // the buzzer is on pin 1 (GPIO 0)
  pinMode(0, OUTPUT);
  
  // ******* init camera and check that it is inited

  // ******* init Datalogger and check that it is inited
  while (!SD.begin(_CS)) {
    Serial.println("ERROR init SD");
    // there is an error so beep multiple times
    // code for SD error:
    beepBepper(0, 1000, 100);
    beepBepper(0, 1000, 100);
    delay(100);  // wait before you try to init again
  }

  // init 10-DOF and check that it is inited
  while (init10DOF() == false) {
    Serial.println("ERROR init 10-DOF");
    // there is an error so beep multiple times
    // code for 10-DOF error:
    beepBepper(0, 1000, 100);
    beepBepper(0, 1000, 100);
    delay(200);
    beepBepper(0, 1000, 100);
    beepBepper(0, 1000, 100);
    beepBepper(0, 1000, 100);    
    delay(100);  // wait before you try to init again
  }

  // beep beeper to indicate that it has inited
  beepBepper(0, 500, 700);

  // I dont know if this is needed (i might get rid of it)
  // ******* test the camera, datalogger and 10-DOF
  // beep, beep, beep to indicate that the tests have passed
  beepBepper(0, 500, 200);
  beepBepper(0, 500, 200);
  beepBepper(0, 500, 200);

  // ******* start camera recording
}

bool init10DOF() {
  // init 10-DOF and return true if successful
  bool initiated = true;
  // init the IMU sensor
  if (!imuInit(&enMotionSensorType)){
    // if there was an error
    initiated = false;
  }
  // init the main sensor
  if (!LPS22HB_INIT()) {
    // if there was a error
    initiated = false;
  }
  return initiated;  // return true or false if inited
}

void loop() {
  // read 10-DOF values
  read10DOFData();
  Serial.print("Nul Accel Counter: ");
  Serial.println(nulAccelCounter);
  // Serial.println(dataArray[3][0]);
  // Serial.println(dataArray[3][1]);
  // Serial.println(dataArray[3][2]);

  // check rocket start
  if (!rocketStart) {
    rocketStart = checkRocketStart(dataArray);
  }

  // if the rocket has started then it will start to check if it has landed
  // if it has both started and landed for x loops then turn everything off
  if (rocketStart && checkLanded(dataArray)) {
    nulAccelCounter++;
    // check to see if the checkLanded function is true for x amount of loops
    if (nulAccelCounter == SHUTOFF_ACCEL_COUNT) {
      Serial.println("CAMERA OFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      // ******* turn off camera
      // ******* finish logging anything
      // ******* turn pico to sleep mode (might not need this)
    }
  }
  // the else makes sure that the camera will only turn off after the accel is 0 for x duration
  // if the rocket is still moving in the air (or has not started yet)
  else {
    // insure that the nulAccelCounter is 0 while the rocket is still moving
    nulAccelCounter = 0;
  }

  // ******* check storage
  // ******* if full delet oldest data if not save data

  // ----------- Remove after Testing --------------------
  delay(50); // slows it down for testing
}

/**
 * Reads data from 10DOF sensors
 * Updates the 2d array for data with the new values
 */
void read10DOFData() {

  //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

  LPS22HB_START_ONESHOT();
  if ((I2C_readByte(LPS_STATUS) & 0x01) == 0x01)  // a new pressure data is generated
  {
    u8Buf[0] = I2C_readByte(LPS_PRESS_OUT_XL);
    u8Buf[1] = I2C_readByte(LPS_PRESS_OUT_L);
    u8Buf[2] = I2C_readByte(LPS_PRESS_OUT_H);
    PRESS_DATA = (float)((u8Buf[2] << 16) + (u8Buf[1] << 8) + u8Buf[0]) / 4096.0f;
  }
  if ((I2C_readByte(LPS_STATUS) & 0x02) == 0x02)  // a new pressure data is generated
  {
    u8Buf[0] = I2C_readByte(LPS_TEMP_OUT_L);
    u8Buf[1] = I2C_readByte(LPS_TEMP_OUT_H);
    TEMP_DATA = (float)((u8Buf[1] << 8) + u8Buf[0]) / 100.0f;
  }

  imuDataGet(&stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);

  dataArray[0][0] = stAngles.fPitch;
  dataArray[0][1] = stAngles.fYaw;
  dataArray[0][2] = stAngles.fRoll;
  dataArray[1][0] = PRESS_DATA;
  dataArray[1][1] = 0;
  dataArray[1][2] = 0;
  dataArray[2][0] = TEMP_DATA;
  dataArray[2][1] = 0;
  dataArray[2][2] = 0;
  dataArray[3][0] = stAccelRawData.s16X;
  dataArray[3][1] = stAccelRawData.s16Y;
  dataArray[3][2] = stAccelRawData.s16Z;
  dataArray[4][0] = stGyroRawData.s16X;
  dataArray[4][1] = stGyroRawData.s16Y;
  dataArray[4][2] = stGyroRawData.s16Z;
  dataArray[5][0] = stMagnRawData.s16X;
  dataArray[5][1] = stMagnRawData.s16Y;
  dataArray[5][2] = stMagnRawData.s16Z;
}

bool checkRocketStart(float data[5][3]) {  // needs testing
  // Serial.print("Rocket Start: ");
  // Serial.println(data[3][2] >= ACCEL_Z_THRESHOLD);
  return data[3][2] >= ACCEL_Z_THRESHOLD;
}

bool checkLanded(float data[5][3]) {  // needs testing
  // check if landed, return if so, return false if not
  bool landed = false;
  // find the net accel
  float netAccel = sqrt(pow(data[3][0], 2) + pow(data[3][1], 2) + pow(data[3][2], 2));
  Serial.print("Net Accel: ");
  Serial.println(netAccel);
  // check if net accel value is 17000 ish (+ or - 1500)
  if (NET_RESTING_ACCEL_VALUE - 1400 <= netAccel && netAccel <= NET_RESTING_ACCEL_VALUE + 900) {
    // now that accel is 0 ish check the altitude just to double check it has landed
    float altitude = ((pow(SEA_LEVEL_PRESSURE / data[1][0], (1 / 5.257)) - 1) * (data[2][0] + 273.15)) / 0.0065;
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
