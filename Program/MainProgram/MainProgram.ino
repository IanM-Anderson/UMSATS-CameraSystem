// main program to run the camera system

void setup(){
    Serial.begin(115200);
    // init camera
    // check init

    // init Datalogger
    // check init

    // init 10-DOF
    // check init
    if(init10DOF() == false){
        Serial.println("ERROR init 10-DOF");
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
    // check landed
        // if landed, end program
    // check storage
        // if full delet oldest data if not save data
}