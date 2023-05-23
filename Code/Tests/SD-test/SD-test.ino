#include <SPI.h>
#include <SD.h>

// pins for the datalogger
const int _MISO = 16;
const int _MOSI = 19;
const int _CS = 17;
const int _SCK = 18;

void setup(){
    Serial.begin(115200);  // start the serial monitor
    while (!SD.begin(_CS)) {
    Serial.println("ERROR init SD");
    // there is an error so beep multiple times
    // code for SD error:
    // beepBepper(0, 1000, 100);
    delay(2000);  // wait before you try to init again
  }
}

void loop(){
    Serial.println(testSD()); // should print 1 everyloop
    delay(1000);
}

bool testSD(){
    // input the test SD code here...
    // we want to have the test print somthing to the SD 
    // then check if the somthing was actully printed to the SD
  bool testSuc = false;
  
  String testLine = "!test!"; // the string we are looking for
  // opens the SD card file in write mode
  File dataFile1 = SD.open("datalog.txt", FILE_WRITE);
  // print our test line to the file
  dataFile1.print(testLine);
  dataFile1.print("<*"); // add some chars at the end in order to select only the testline
  dataFile1.close();

  File dataFile2 = SD.open("datalog.txt", FILE_READ);
  String testLine1 = dataFile2.readStringUntil('*'); // stores a String until it finds the char '*'
  int endPoint = testLine1.indexOf('<'); // finds the index of the char '<'
  int startPoint = testLine1.indexOf('!'); // finds the index of the char '!'
  String testLine2 = testLine1.substring(startPoint, endPoint); // stores the String from '!' to but not including '<'
  // check to see if testLine2 is equal to "!test!"
  if(testLine.equals(testLine2)){
    // then print ok!
    testSuc = true;
  }
  // close the data file
  dataFile2.close();
  return testSuc;
}