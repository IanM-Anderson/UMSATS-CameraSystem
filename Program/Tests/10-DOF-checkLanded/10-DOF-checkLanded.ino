/*
The 10-DOF will need to turn off the camera once the rocket has landed back on the ground
the 10-DOF will turn off the camera when it detects an acceleration of 0
However when the sensor is first powered on the acceleration will be 0
Therefore, we need to be able to tell when the rocket was launched.
Once we know when it was launched then we can start testing if it has landed.

At the apex and at terminal velocity the acceleration will be 0
However the x and y acceleration will not be 0
Therefore, we will know when the rocket hits the ground when all the accelerations are 0
*/
int accelOfset = 10; // value of the inacruacy of the sensor
int altOfset = 100; // value of the margin of error of altitude

// data = {roll(0), pitch(1), yaw(2), pressure(3), temp(4), accelX(5), accelY(6), accelZ(7), gyroX, gyroY, gyroZ, magX, magY, magZ}
bool checkLanded(float data[14]){
    // check if landed, return if so, return false if not
    bool cameraOff = false;
    // check if all accel values are 0 ish
    if(data[5] <= accelOfset && data[6] <= accelOfset && data[7] <= accelOfset){
        // now that accel is 0 ish check the altitude just to double check it has landed
        float altitude = ((pow(1013.25/data[3], (1/5.257))-1) * (data[4]+273.15))/0.0065;
        if(altitude <= altOfset){
            // turn the camera off since we know that it has landed
            cameraOff = true;
        }
    }

    return cameraOff; //cameraOff is a bool and will turn true
}