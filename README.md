# UMSATS-CameraSystem
The University of Manitoba Space Applications and Technology Society (UMSATS) Camera System 2023 Project:
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UMSATS will be launching a rocket for the 2023 competition named Winnie 4.0. Onboard the rocket will be a camera which is the responsibility of the Camera System sub-committee.

The camera system will record footage and store it in a local storage device. If there is time near the end of the project our team will also try and live stream the video to the ground station. Once the system is powered the camera will start to record almost instantly. However, if the storage of the device is full then the oldest footage will be deleted to make room for the new recording.

There will also be an accelerometer and altitude sensor connected to the system so we can determine when the rocket has landed. Once the sensors detect that it has landed then the camera will be shut off so no new footage is recorded.

We use the following equipment in the camera system:
- Raspberry Pi Pico (Main control board for the project)
- 10-DOF (Accelerometer and Barometric pressure sensor)
- Datalogger (Micro SD card Attachment)
- Micro SD card
- Buzzer (Buzzer for a audio indication for when the system is armed)
- Arducam Mini x2 (The camera)
- 180 degree lens x2 (A lens for the cameras)



Installation:
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
The code for the project can be found here but to run and upload the program to the Raspberry Pi Pico you will need to install some applications. Please follow the steps below to run the program:
1. Install the Arduino IDE; you can install it from this link: https://www.arduino.cc/en/software (The images below will use version 2.0.3 however the newest version will be best)
2. Once the software is installed open the software
3. In the top left there will be the "File" menu option and under "File" near the bottom will be "Preferences", click on that.
4. At the very bottom of that menu is a box for "Additional boards managers URL" in that box enter the following URL: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
5. Then click "Ok" to exit the menu
6. In the top menu bar click on "Tools", then "Board: (there might be a name here or it will be empty)", then "Boards Manager..."
7. When you click on "Boards Manager..." a search bar will appear on the left (The search bar will appear on the left for this version however it might be different depending on the version)
8. In the search bar type "pico"
9. One of the options will be labelled "Raspberry Pi Pico/RP240 by Earle". Install that option (you might need to expand the window by dragging on the right-hand side to see the install button)
_______________________________________________________________________________________________________________________________________________________________________
Now To upload the code follow these steps:
1. Under "Tools" click on the "Boards" menu and this time there should be both the "Arduino AVR Boards" and the thing you just downloaded earlier. Click on the new thing and there should be a bunch of different options for boards click the one that says "Raspberry Pi Pico" (should be at the very top) 
2. Then plug the Pi in (you can plug it in before or after step 1 it doesn't matter) (When plugging it in you might need to click the boot button on the board as you do that)
3. Then again in the tools menu, there should be a new option called "port"
4. Under the port option, there should be at least one thing saying "COM" and then a number. (I don't know what number it will be so if there is more than one just try one they try another.) Click on one of the options.
5. Now you are all set hopefully. Click the button with the arrow pointing to the right to upload and then cross your fingers.
