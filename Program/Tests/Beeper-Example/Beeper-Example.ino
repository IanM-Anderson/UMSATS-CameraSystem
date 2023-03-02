void setup() {
  // put your setup code here, to run once:
  // the buzzer is on pin 1 (GPIO 0)
  pinMode(0, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  tone(0, 1000); // Start the sound at 1KHz
  delay(1000); // wait for a second
  noTone(0); // stop the sound
  delay(1000);  // wait 1 sec
}
