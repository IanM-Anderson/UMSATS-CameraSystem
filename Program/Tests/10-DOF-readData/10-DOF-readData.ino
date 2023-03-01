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
float PRESS_DATA=0;
float TEMP_DATA=0;
uint8_t u8Buf[3];


//when calling function, initialize a 2d float arry: float arr[6][3];
//formatted using original 10DOF loop(), so assumes that function returns values properly

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
 * 
 * @return Returns 2D Float array containing values read from 10DOF sensors
 */

void read10DOFData(float data[][]){

    //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

    LPS22HB_START_ONESHOT();
    if((I2C_readByte(LPS_STATUS)&0x01)==0x01)   //a new pressure data is generated
    @ -51,12 +59,20 @@ void loop() {
        u8Buf[1]=I2C_readByte(LPS_TEMP_OUT_H);
        TEMP_DATA=(float)((u8Buf[1]<<8)+u8Buf[0])/100.0f;
    }

    imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);

    data[0][0] = stAngles.fPitch; 
    data[0][1] = stAngles.fYaw;
    data[0][2] = stAngles.fRoll; 
    data[1][0] = PRESS_DATA;
    data[1][1] = 0;
    data[1][2] = 0;
    data[2][0] = TEMP_DATA;
    data[2][1] = 0;
    data[2][2] = 0;    
    data[3][0] = stAccelRawData.s16X;
    data[3][1] = stAccelRawData.s16Y;
    data[3][2] = stAccelRawData.s16Z;    
    data[4][0] = stGyroRawData.s16X;
    data[4][1] = stGyroRawData.s16Y;
    data[4][2] = stGyroRawData.s16Z;    
    data[5][0] = stMagnRawData.s16X;
    data[5][1] = stMagnRawData.s16Y;
    data[5][2] = stMagnRawData.s16Z;    
}