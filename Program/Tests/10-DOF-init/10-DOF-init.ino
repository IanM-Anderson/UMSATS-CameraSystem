/*
init the 10-DOF sensor
*/

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