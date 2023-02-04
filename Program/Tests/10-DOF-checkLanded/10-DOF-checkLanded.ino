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

bool checkLanded(){
    // check if landed, return if so, return false if not
    bool cameraOff = false;


    return cameraOff; //cameraOff is a bool and will turn true
}