// main program to run the camera system
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

// ---------------------------------------- Variables ----------------------------------------------
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
float dataO[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
float ACCEL_Z_THRESHOLD = 5.00;//acceleration in ms^-2
int ALT_SHUTOFF = 100; // value of the margin of error of altitude
float SEA_LEVEL_PRESSURE = 1013.25;//pressure in hPa

bool rocketStart = false;
// ------------------------------------------------------------------------------------------------------

void setup(){
    Serial.begin(115200);
    // init camera
    // check init

    // init Datalogger
    // check init

    // init 10-DOF
    // check init
    while(init10DOF() == false){
        Serial.println("ERROR init 10-DOF");
        delay(100);
    }

    // beep

    // test the camera, datalogger and 10-DOF
    // beep, beep, beep

    // start camera recording
}

bool init10DOF(){
    // init 10-DOF and return true if successful
    bool initiated = false;
    // init the IMU sensor
    imuInit(&enMotionSensorType);
    // then init the other sensors and check if they inited
	if (LPS22HB_INIT() && IMU_EN_SENSOR_TYPE_MPU9250 == enMotionSensorType){
		Serial.println("10-DOF init is GOOD");
        // set initiated to true since the sensors have started
        initiated = true;
	}
    // if they did not init then print an error message
    else{
        Serial.println("10-DOF init BAD");
    }
    return initiated; // return true or false if inited
}

void loop(){
    // read 10-DOF values
    read10DOFData();
    // check rocket start
    if(!rocketStart){
        rocketStart = checkRocketStart(dataO);
    }
    // if the rocket has started then it will start to check if it has landed
    // if it has both started and landed then turn everything off
    if(rocketStart && checkLanded(dataO)){
        // turn off camera
        // finish logging anything
        // turn pico to sleep mode
    }
    // check storage
        // if full delet oldest data if not save data
    datalog10DOF(dataO)
}

/**
 * Reads data from 10DOF sensors
 * Updates the 2d array for data with the new values
 */
void read10DOFData(){
    //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

    LPS22HB_START_ONESHOT();
    if((I2C_readByte(LPS_STATUS)&0x01)==0x01)   //a new pressure data is generated
    @ -51,12 +59,20 @@ void loop() {
        u8Buf[1]=I2C_readByte(LPS_TEMP_OUT_H);
        TEMP_DATA=(float)((u8Buf[1]<<8)+u8Buf[0])/100.0f;
    }

    imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);

    dataO[0][0] = stAngles.fPitch; 
    dataO[0][1] = stAngles.fYaw;
    dataO[0][2] = stAngles.fRoll; 
    dataO[1][0] = PRESS_DATA;
    dataO[1][1] = 0;
    dataO[1][2] = 0;
    dataO[2][0] = TEMP_DATA;
    dataO[2][1] = 0;
    dataO[2][2] = 0;    
    dataO[3][0] = stAccelRawData.s16X;
    dataO[3][1] = stAccelRawData.s16Y;
    dataO[3][2] = stAccelRawData.s16Z;    
    dataO[4][0] = stGyroRawData.s16X;
    dataO[4][1] = stGyroRawData.s16Y;
    dataO[4][2] = stGyroRawData.s16Z;    
    dataO[5][0] = stMagnRawData.s16X;
    dataO[5][1] = stMagnRawData.s16Y;
    dataO[5][2] = stMagnRawData.s16Z;    
}

bool checkRocketStart(float data[][]){
    return data[3][2] >= ACCEL_Z_THRESHOLD
}

bool checkLanded(float data[][]){//need to check for proper parameter type, needs some testing
    // check if landed, return if so, return false if not
    bool cameraOff = false;
    // check if all accel values are 0 ish
    if(data[3][0] <= accelOfset && data[3][1] <= accelOfset && data[7] <= accelOfset){
        // now that accel is 0 ish check the altitude just to double check it has landed
        float altitude = ((pow(SEA_LEVEL_PRESSURE/data[1][0], (1/5.257))-1) * (data[2][0]+273.15))/0.0065;
        if(altitude <= ALT_SHUTOFF){
            // turn the camera off since we know that it has landed
            cameraOff = true;
        }
    }

    return cameraOff; //cameraOff is a bool and will turn true
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