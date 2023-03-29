
void setup(){
    Serial.begin(115200);  // start the serial monitor
}

void loop(){
    Serial.println(testSD); // should print 1 everyloop
    delay(1000);
}

bool testSD(){
    // input the test SD code here...
    // we want to have the test print somthing to the SD 
    // then check if the somthing was actully printed to the SD

}