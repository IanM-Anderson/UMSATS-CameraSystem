/*
Since we will have a sensor for the camera system to detect a landing
we might as well log the data since it will be cool to have

Therefore every loop we will call a function to store the data
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