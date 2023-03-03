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
float PRESS_DATA=0;
float TEMP_DATA=0;

// pins for the datalogger
const int _MISO = 16;
const int _MOSI = 19;
const int _CS = 17;
const int _SCK = 18;

void beepBepper(int pin, int frequency, int duration){
    // send a tone with frequency to a pin
    tone(pin, frequency);
    delay(duration); // keep the tone going for some duration
    noTone(pin); // turn off the tone
}

void setup(){
    Serial.begin(115200); // start the serial monitor
    // ------------- Datalogger ---------------
    // Ensure the SPI pinout the SD card is connected to is configured properly
    SPI.setRX(_MISO);
    SPI.setTX(_MOSI);
    SPI.setSCK(_SCK);
    // see if the card is present and can be initialized:
    while(!SD.begin(_CS))
    {
      Serial.println("ERROR init SD");
      // there is an error so beep multiple times
      // code for SD error:
      beepBepper(0, 1000, 100);
      beepBepper(0, 1000, 100);
      delay(2000); // wait before you try to init again
    }

    // ------------------ 10-DOF -----------------
    while(init10DOF() == false){
        Serial.println("ERROR init 10-DOF");
        // there is an error so beep multiple times
        // code for 10-DOF error:
        beepBepper(0, 1000, 100);
        beepBepper(0, 1000, 100);
        delay(200);
        beepBepper(0, 1000, 100);
        beepBepper(0, 1000, 100);
        beepBepper(0, 1000, 100);
        delay(2000); // wait before you try to init again
    }

    // Now everything is verified to be working
    // beep to indicate it is all good
    beepBepper(0, 500, 1000);
}

bool init10DOF()
{
    // init the imu sensor
    imuInit(&enMotionSensorType);
    // check the IMU sensor type
	if(IMU_EN_SENSOR_TYPE_MPU9250 == enMotionSensorType)
	{
		Serial.printf("Motion sersor is MPU9250\n" );
	}

    // init the main sensor
	if (!LPS22HB_INIT()){
        // if there was a error
		Serial.printf("LPS22HB Init Error\n");
		return false; // return false because of error
	}
    // there is no error so return true
    return true;
}

void loop(){
    read10DOFData();
    datalog10DOF(data1);
    delay(200);
}

void read10DOFData(){

    //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

    LPS22HB_START_ONESHOT();
    if ((I2C_readByte(LPS_STATUS) & 0x01) == 0x01) // a new pressure data is generated
    {
        u8Buf[0] = I2C_readByte(LPS_PRESS_OUT_XL);
        u8Buf[1] = I2C_readByte(LPS_PRESS_OUT_L);
        u8Buf[2] = I2C_readByte(LPS_PRESS_OUT_H);
        PRESS_DATA = (float)((u8Buf[2] << 16) + (u8Buf[1] << 8) + u8Buf[0]) / 4096.0f;
    }
    if ((I2C_readByte(LPS_STATUS) & 0x02) == 0x02) // a new pressure data is generated
    {
        u8Buf[0] = I2C_readByte(LPS_TEMP_OUT_L);
        u8Buf[1] = I2C_readByte(LPS_TEMP_OUT_H);
        TEMP_DATA = (float)((u8Buf[1] << 8) + u8Buf[0]) / 100.0f;
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

void datalog10DOF(float data[5][3]){
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
    dataString += "ROLL: {" + data[0][0] + "};  ";
    dataString += "PITCH: {" + data[0][1] + "};  ";
    dataString += "YAW: {" + data[0][2] + "};  ";
    dataString += "PRESSURE: {" + data[1][0] + "};  ";
    dataString += "TEMP: {" + data[2][0] + "};  ";
    dataString += "ACCELERATION: " + "{" + data[3][0] + "}x, {" + data[3][1] + "}y, {" + data[3][2] + "}z;  ";
    dataString += "GYROSCOPE: " + "{" + data[4][0] + "}x, {" + data[4][1] + "}y, {" + data[4][2] + "}z;  ";
    dataString += "MAGNETIC: " + "{" + data[5][0] + "}x, {" + data[5][1] + "}y, {" + data[5][2] + "}z";

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