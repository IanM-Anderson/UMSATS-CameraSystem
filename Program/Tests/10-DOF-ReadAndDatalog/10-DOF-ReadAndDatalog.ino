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
// -------------------------------------------- Variables ----------------------------------------------
/**
 * The 2D Array is formatted as:
 * [0] -> Angle (roll,pitch,yaw)
 * [1] -> Pressure (pressure,0,0)
 * [2] -> Temperature (temperature,0,0)
 * [3] -> Acceleration (x,y,z)
 * [4] -> Gyroscope (x,y,z)
 * [5] -> Magnetic (x,y,z)
 */
float dataArray[5][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
float PRESS_DATA = 0;
float TEMP_DATA = 0;

// pins for the datalogger
const int _MISO = 16;
const int _MOSI = 19;
const int _CS = 17;
const int _SCK = 18;
//---------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);  // start the serial monitor
  pinMode(LED_BUILTIN, OUTPUT);
  // ------------- Datalogger ---------------
  // Ensure the SPI pinout the SD card is connected to is configured properly
  SPI.setRX(_MISO);
  SPI.setTX(_MOSI);
  SPI.setSCK(_SCK);
  // see if the card is present and can be initialized:
  
  while (!SD.begin(_CS)) {
    Serial.println("ERROR init SD");
    // there is an error so beep multiple times
    // code for SD error:
    beepBepper(0, 1000, 100);
    // beepBepper(0, 1000, 100);
    delay(2000);  // wait before you try to init again
  }
  // -- Test SD
  while (!testSD()){
    Serial.println("Error, SD test");
    while(!SD.begin(_CS)){
      Serial.println("ERROR init SD");
      // there is an error so beep multiple times
      // code for SD error:
      beepBepper(0, 1000, 100);
      beepBepper(0, 1000, 100);
      delay(2000);  // wait before you try to init again
    }
  }

  // ------------------ 10-DOF -----------------
  // -- init
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
    delay(2000);  // wait before you try to init again
  }
  // Now everything is verified to be working
  // beep to indicate it is all good
  beepBepper(0, 500, 1000);
}

void loop() {
  // For every loop, read the 10-DOF data then store the data on the SD card
  read10DOFData(); // read data
  datalog10DOF(dataArray); // store data
  delay(200); // wait 0.2s
}

/**
 * Sends a tone to a buzzer from a pin on the pico for a certain duration
 * 
 * @param pin The pin that the buzzer is conneceted to
 * @param frequency The frequency of the tone being sent to the buzzer
 * @param duration The length the sound will go for
*/
void beepBepper(int pin, int frequency, int duration) {
  // send a tone with frequency to a pin
  tone(pin, frequency);
  delay(duration);  // keep the tone going for some duration
  noTone(pin);      // turn off the tone
}

/**
 * Tests the SD card by writing a specific line to it then read the SD card
 * to make sure that it was written correctly
 * 
 * @return Returns if the test was successful or not
*/
bool testSD(){
  bool testSuc = false;
  
  String testLine = "!test!"; // the string we are looking for
  // opens the SD card file in write mode
  File dataFile1 = SD.open("datalog.txt", FILE_WRITE);
  // print our test line to the file
  dataFile1.print(testline);
  dataFile1.print("<*"); // add some chars at the end in order to select only the testline
  dataFile1.close();

  File dataFile2 = SD.open("datalog.txt", FILE_READ);
  String testLine1 = dataFile2.readStringUntil('*'); // stores a String until it finds the char '*'
  int endPoint = testLine1.indexOf('<'); // finds the index of the char '<'
  int startPoint = testLine1.indexOf('!'); // finds the index of the char '!'
  String testLine2 = testLine1.substring(startPoint, endPoint); // stores the String from '!' to but not including '<'
  Serial.println(testLine2);
  // check to see if testLine2 is equal to "!test!"
  if(testLine.equals(testLine2)){
    // then print ok!
    Serial.println("YAH!!!!");
    testSuc = true;
  }
  // close the data file
  dataFile2.close();
  return testSuc;
}

/**
 * inits the 10-DOF and return is the init was good
 * 
 * @return Returns if the 10-DOF was inited
*/
bool init10DOF(){
  // init 10-DOF and return true if successful
  bool initiated = true;
  // init the IMU sensor
  imuInit(&enMotionSensorType);
  // check the IMU sensor type
  if (IMU_EN_SENSOR_TYPE_MPU9250 == enMotionSensorType) {
    Serial.printf("Motion sersor is MPU9250\n");
  }

  // init the main sensor
  if (!LPS22HB_INIT()) {
    // if there was a error
    Serial.printf("LPS22HB Init Error\n");
    initiated = false;
  }
  return initiated; // return true or false if inited
}

/**
 * reads and stores the 10-DOF values in the global array
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

/**
 * Stores the values from the 10-DOF sensors to the SD card
 * 
 * @param data The array of values to be stored
*/
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

  Serial.println(dataString);

  // open the file
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(LED_BUILTIN, LOW);
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    Serial.println("error opening datalog.txt");
  }
}