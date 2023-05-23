/*
init the 10-DOF sensor
*/

bool init10DOF() {
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
  return initiated;  // return true or false if inited
}