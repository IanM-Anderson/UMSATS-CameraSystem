
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

/**
 * The 2D Array is formatted as:
 * [0] -> Angle (roll,pitch,yaw)
 * [1] -> Pressure (pressure,0,0)
 * [2] -> Temperature (temperature,0,0)
 * [3] -> Acceleration (x,y,z)
 * [4] -> Gyroscope (x,y,z)
 * [5] -> Magnetic (x,y,z)
 */
float data1[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

void setup(){
    Serial.begin(115200);
    while(init10DOF() == false){
        Serial.println("ERROR init 10-DOF");
        delay(100);
    }
}

void loop(){
    read10DOFData();
    datalog10DOF(data1);
    delay(100);
}

void read10DOFData(){

    //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

    LPS22HB_START_ONESHOT();
    if((I2C_readByte(LPS_STATUS)&0x01)==0x01)   //a new pressure data is generated
    @ -51,12 +59,20 @@ void loop() {
        u8Buf[1]=I2C_readByte(LPS_TEMP_OUT_H);
        TEMP_DATA=(float)((u8Buf[1]<<8)+u8Buf[0])/100.0f;
    }

    imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);

    data1[0][0] = stAngles.fPitch; 
    data1[0][1] = stAngles.fYaw;
    data1[0][2] = stAngles.fRoll; 
    data1[1][0] = PRESS_DATA;
    data1[1][1] = 0;
    data1[1][2] = 0;
    data1[2][0] = TEMP_DATA;
    data1[2][1] = 0;
    data1[2][2] = 0;    
    data1[3][0] = stAccelRawData.s16X;
    data1[3][1] = stAccelRawData.s16Y;
    data1[3][2] = stAccelRawData.s16Z;    
    data1[4][0] = stGyroRawData.s16X;
    data1[4][1] = stGyroRawData.s16Y;
    data1[4][2] = stGyroRawData.s16Z;    
    data1[5][0] = stMagnRawData.s16X;
    data1[5][1] = stMagnRawData.s16Y;
    data1[5][2] = stMagnRawData.s16Z;    
}

void datalog10DOF(float data[][]){
    // Store all the data in the SD card
    // make a string for assembling the data to log:
    String dataString = "";
    // loop through all the elements in the array and add them to the dataString
    for(int i = 0; i <= 5; i++){
        for(int j = 0; j <= 3; j++){
            dataString += data[i][j];
            dataString += ", ";
        }
    }

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