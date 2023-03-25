// main program to run the camera system
#include "lps22hb.h"
#include "imu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SPI.h>
#include <SD.h>

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
int accelOfset = 0;

int SHUTOFF_ACCEL_COUNT = 500;  // the number of 0 accel values needed in a row before shut down
int nulAccelCounter = 0;        // var to store the number of 0 accel values in a row
// ------------------------------------------------------------------------------------------------------

void beepBepper(int pin, int frequency, int duration) {
  // send a tone with frequency to a pin
  tone(pin, frequency);
  delay(duration);  // keep the tone going for some duration
  noTone(pin);      // turn off the tone
}

void setup() {
  Serial.begin(115200);
  // ******* init camera and check that it is inited

  // ******* init Datalogger and check that it is inited

  // init 10-DOF and check that it is inited
  while (init10DOF() == false) {
    Serial.println("ERROR init 10-DOF");
    delay(100);
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
  imuInit(&enMotionSensorType);
  // check the IMU sensor type
  if (IMU_EN_SENSOR_TYPE_MPU9250 == enMotionSensorType) {
    Serial.println("Motion sersor is MPU9250\n");
  }

  // init the main sensor
  if (!LPS22HB_INIT()) {
    // if there was a error
    Serial.println("LPS22HB Init Error\n");
    initiated = false;
  }
  return initiated;  // return true or false if inited
}

void loop() {
  // read 10-DOF values
  read10DOFData();
  Serial.println(dataArray[3][0]);
  Serial.println(dataArray[3][1]);
  Serial.println(dataArray[3][2]);

  // loop 30 times so that we can settle the readings
  for (int i = 0; i <= 30; i++) {
    read10DOFData();
    delay(10);
  }
  // now that the readings are stable get the net accel for when it is resting
  accelOfset = sqrt(pow(dataArray[3][0], 2) + pow(dataArray[3][1], 2) + pow(dataArray[3][2], 2));

  // check rocket start
  if (!rocketStart) {  // maybe change to a while loop
    rocketStart = checkRocketStart(dataArray);
  }

  // if the rocket has started then it will start to check if it has landed
  // if it has both started and landed for x loops then turn everything off
  if (rocketStart && checkLanded(dataArray)) {
    nulAccelCounter++;
    // check to see if the checkLanded function is true for x amount of loops
    if (nulAccelCounter == SHUTOFF_ACCEL_COUNT) {
      Serial.println("CAMERA OFF!!!!!!!!!!!!!!!");
      // ******* turn off camera
      // ******* finish logging anything
      // ******* turn pico to sleep mode
    }
  }
  // the else makes sure that the camera will only turn off after the accel is 0 for x duration
  // if the rocket is still moving in the air (or has not started yet)
  else {
    Serial.println("Not Camera Off");
    // insure that the nulAccelCounter is 0 while the rocket is still moving
    nulAccelCounter = 0;
  }

  // ******* check storage
  // ******* if full delet oldest data if not save data

  // if we want we can data log the 10-DOF data (uncomment the line)
  //datalog10DOF(dataArray)
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
  Serial.print("Rocket Start: ");
  Serial.println(data[3][2] >= ACCEL_Z_THRESHOLD);
  return data[3][2] >= ACCEL_Z_THRESHOLD;
}

bool checkLanded(float data[5][3]) {  // needs testing
  // check if landed, return if so, return false if not
  bool landed = false;
  float netAccel = sqrt(pow(data[3][0], 2) + pow(data[3][1], 2) + pow(data[3][2], 2));
  // check if z accel value is 0 ish
  if (netAccel <= accelOfset) {
    // now that accel is 0 ish check the altitude just to double check it has landed
    float altitude = ((pow(SEA_LEVEL_PRESSURE / data[1][0], (1 / 5.257)) - 1) * (data[2][0] + 273.15)) / 0.0065;
    // Serial.print("Altitude: ");
    // Serial.println(altitude);
    if (altitude <= ALT_SHUTOFF) {
      // we know that it has landed
      landed = true;
    }
  }
  Serial.print("Landed: ");
  Serial.println(landed);
  return landed;  //landed is a bool and will turn true
}

void datalog10DOF(float data[5][3]) {
  // Store all the data in the SD card
  // make a string for assembling the data to log:
  String dataString = "";
  /**
     * The 2D Array is formatted as:
     * [0] -> Angle (roll,pitch,yaw)
     * [1] -> Pressure (pressure,0,0)
     * [2] -> Temperature (temperature,0,0)
     * [3] -> Acceleration (x,y,z)
     * [4] -> Gyroscope (x,y,z)
     * [5] -> Magnetic (x,y,z)
    */
  dataString += "ROLL: |";
  dataString += data[0][0];
  dataString += "|;  ";
  dataString += "PITCH: |";
  dataString += data[0][1];
  dataString += "|;  ";
  dataString += "YAW: |";
  dataString += data[0][2];
  dataString += "|;  ";
  dataString += "PRESSURE: |";
  dataString += data[1][0];
  dataString += "|;  ";
  dataString += "TEMP: |";
  dataString += data[2][0];
  dataString += "|;  ";
  dataString += "ACCELERATION: |";
  dataString += data[3][0];
  dataString += "|x, |";
  dataString += data[3][1];
  dataString += "|y, |";
  dataString += data[3][2];
  dataString += "|z;  ";
  dataString += "GYROSCOPE: |";
  dataString += data[4][0];
  dataString += "|x, |";
  dataString += data[4][1];
  dataString += "|y, |";
  dataString += data[4][2];
  dataString += "|z;  ";
  dataString += "MAGNETIC: |";
  dataString += data[5][0];
  dataString += "|x, |";
  dataString += data[5][1];
  dataString += "|y, |";
  dataString += data[5][2];
  dataString += "|z";

  // open the file
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}