#include "lps22hb.h"
#include "imu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

IMU_EN_SENSOR_TYPE enMotionSensorType;
IMU_ST_ANGLES_DATA stAngles;
IMU_ST_SENSOR_DATA stGyroRawData;
IMU_ST_SENSOR_DATA stAccelRawData;
IMU_ST_SENSOR_DATA stMagnRawData;
float PRESS_DATA = 0;
float TEMP_DATA = 0;
uint8_t u8Buf[3];


//when calling function, initialize a 2d float arry: float arr[6][3];
//formatted using original 10DOF loop()

/**
 * Reads data from 10DOF sensors
 *
 * The 2D Array is formatted as:
 * [0] -> Angle (roll,pitch,yaw)
 * [1] -> Pressure (pressure,0,0)
 * [2] -> Temperature (temperature,0,0)
 * [3] -> Acceleration (x,y,z)
 * [4] -> Gyroscope (x,y,z)
 * [5] -> Magnetic (x,y,z)
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