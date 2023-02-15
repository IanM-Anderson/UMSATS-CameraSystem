//when calling function, initialize a 2d float arry: float arr[x][y] = read10DOFData();
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

float read10DOFData(){
    float arr[6][3] = {};
    //anglesPiYaRo, pressure, tempertaure, accelerationXYZ, gyroscopeXYZ, magneticXYZ

    LPS22HB_START_ONESHOT();
    if((I2C_readByte(LPS_STATUS)&0x01)==0x01)   //a new pressure data is generated
    @ -51,12 +59,20 @@ void loop() {
        u8Buf[1]=I2C_readByte(LPS_TEMP_OUT_H);
        TEMP_DATA=(float)((u8Buf[1]<<8)+u8Buf[0])/100.0f;
    }

    imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);

    arr[0][0] = stAngles.fRoll; 
    arr[0][1] = stAngles.fPitch; 
    arr[0][2] = stAngles.fYaw;
    arr[1][0] = PRESS_DATA;
    arr[2][0] = TEMP_DATA;
    arr[3][0] = stAccelRawData.s16X;
    arr[3][1] = stAccelRawData.s16Y;
    arr[3][2] = stAccelRawData.s16Z;
    arr[4][0] = stGyroRawData.s16X;
    arr[4][1] = stGyroRawData.s16Y;
    arr[4][2] = stGyroRawData.s16Z;
    arr[5][0] = stMagnRawData.s16X;
    arr[5][1] = stMagnRawData.s16Y;
    arr[5][2] = stMagnRawData.s16Z;

    return arr;
}