/*
Since we will have a sensor for the camera system to detect a landing
we might as well log the data since it will be cool to have

Therefore every loop we will call a function to stor the data
*/

void datalog10DOF(float data[14]){
    // Store all the data in the SD card
    // make a string for assembling the data to log:
    String dataString = "";

    // find the number of elements in the array
    // number of bytes divided by number of bytes per element
    int numDataElements = sizeof(data) / sizeof(data[0])
    // loop through all the elements in the array and add them to the dataString
    for(int i = 0; i <= numDataElements; i++){
        dataString += data[i];
        // if it is not the last element then add a comma to seperate the data
        if(i != numDataElements){
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