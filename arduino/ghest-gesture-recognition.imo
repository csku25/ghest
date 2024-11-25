/*
  Project:    Ghest
  Author(s):  K'drian Vega, Zachary Andruchowitz
  Start date: Sept. 30, 2024

  Used Libraries: Adafruit_MPU6050, Adafruit_Sensor
  Credit to: Earle Philhower for Raspberry Pi Pico Arduino Core API - https://github.com/earlephilhower/arduino-pico
*/

#include <Statistic.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <KeyboardBT.h>

#define BUTTON 14
Adafruit_MPU6050 mpu;
int count = 0;
Statistic stats[3];


void setup(void) {
  //start serial communication, baud rate 115,200
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  //set physical pico pins and initialize mpu
  //pinMode(BUTTON, INPUT_PULLUP);
  KeyboardBT.begin("Ghest Bracelet");
  KeyboardBT.begin();

  Wire.setSDA(16);
  Wire.setSCL(17);
  if (!mpu.begin()) {
    Serial.println("Error: Failed to find MPU6050 sensor!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 initialized...\n");

  //configure settings
  mpu.setAccelerometerRange(mpu.getAccelerometerRange());  // Set accelerometer range to +/- 2g
  mpu.setGyroRange(mpu.getGyroRange());       // Set gyroscope range to +/- 250 deg/s
  mpu.setFilterBandwidth(mpu.getFilterBandwidth());    // Set filter bandwidth to 21 Hz

  delay(100);
}

void loop() {
  //create sensor event objects for accelerometer, gyro, and temperature
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  if (BOOTSEL) {
    KeyboardBT.consumerPress(KEY_PLAY_PAUSE);
    Serial.printf("Key pressed");
    KeyboardBT.consumerRelease();
    while (BOOTSEL);
  }
  //get the current sensor readings
  //mpu.getEvent(&accel, &gyro, &temp);
  //discard temp readings

  /*if (digitalRead(BUTTON) == 0) {
    pause
  }*/
  //Serial.printf("Data point: %i\n", count);
  //print accelerometer values (in m/s^2)
  //Serial.printf("Accel: %f, %f, %f m/s^2\n", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);

  float dataPoint[15][3];
  float min_x = 0;
  float min_y = 0;
  float min_z = 0;
  float max_x = 0;
  float max_y = 0;
  float max_z = 0;
  int min_x_idx = 0;
  int min_y_idx = 0;
  int min_z_idx = 0;
  int max_x_idx = 0;
  int max_y_idx = 0;
  int max_z_idx = 0;
  int swipe_gesture_range = 0; 
  float x_range = 0;
  float y_range = 0;
  float z_range = 0;


  for (int i = 0; i < 15; i++)
    for (int j = 0; j < 3; j++)
      dataPoint[i][j] = 0.0;

  //generating 15 data points for 1 second gesture timeframe
  while (count < 15){
    mpu.getEvent(&accel, &gyro, &temp);
    dataPoint[count][0] = gyro.gyro.x;
    dataPoint[count][1] = gyro.gyro.y;
    dataPoint[count][2] = gyro.gyro.z;
    count = count + 1;
    delay(50);
  } // end while loop
  count = 0; //reset count for next loop

  //updating minimum and maximums in the data set
  for(int i=0; i<15; i++){
    if (i==0){
      min_x = dataPoint[0][0];
      min_y = dataPoint[0][1];
      min_z = dataPoint[0][2];
      max_x = dataPoint[0][0];
      max_y = dataPoint[0][1];
      max_z = dataPoint[0][2];
    }
    if (dataPoint[i][0] < min_x) { min_x = dataPoint[i][0]; min_x_idx = i; }
    if (dataPoint[i][1] < min_y) { min_y = dataPoint[i][1]; min_y_idx = i; }
    if (dataPoint[i][2] < min_z) { min_z = dataPoint[i][2]; min_z_idx = i; }
    if (dataPoint[i][0] > max_x) { max_x = dataPoint[i][0]; max_x_idx = i; }
    if (dataPoint[i][1] > max_y) { max_y = dataPoint[i][1]; max_y_idx = i; }
    if (dataPoint[i][2] > max_z) { max_z = dataPoint[i][2]; max_z_idx = i; }
  } //end for loop

  //Serial.printf("Min_x: %f, Min_y: %f, Min_z: %f\n", min_x, min_y, min_z);
  //Serial.printf("Max_x: %f, Max_y: %f, Max_z: %f\n\n\n", max_x, max_y, max_z);
  //Serial.printf("%i, %i\n\n", min_y_idx, max_y_idx);

  x_range = abs(min_x - max_x);
  y_range = abs(min_y - max_y);
  z_range = abs(min_z - max_z);
  swipe_gesture_range = min_y_idx - max_y_idx;

  //Serial.printf("Going Again! \n");
  Serial.read();

  if (x_range <= 5.087169) {
        if (y_range <= 2.490891) {
            if (x_range <= 1.232522) {
                if (y_range <= 0.434467) {
                    ; 
                } else {
                    KeyboardBT.consumerPress(KEY_PLAY_PAUSE);
                    Serial.printf("Key pressed");
                    KeyboardBT.consumerRelease(); 
                }
            } else {
                if (swipe_gesture_range <= -11) {
                    KeyboardBT.consumerPress(KEY_SCAN_PREVIOUS);
                    Serial.printf("Key pressed");
                    KeyboardBT.consumerRelease(); 
                    if (x_range <= 1.332046) {
                      KeyboardBT.consumerPress(KEY_VOLUME_DECREMENT);
                      Serial.printf("Key pressed");
                      KeyboardBT.consumerRelease(); 
                    } else {
                        ; 
                    }
                }
            }
        } else {
            if (swipe_gesture_range <= 4) {
              KeyboardBT.consumerPress(KEY_VOLUME_DECREMENT);
              Serial.printf("Key pressed");
              KeyboardBT.consumerRelease(); 
            } 
            else {
              KeyboardBT.consumerPress(KEY_VOLUME_INCREMENT);
              Serial.printf("Key pressed");
              KeyboardBT.consumerRelease(); 
            }
        }
    } else { // x_range > 5.087169
        if (swipe_gesture_range <= -2) {
          KeyboardBT.consumerPress(KEY_SCAN_PREVIOUS);
          Serial.printf("Key pressed");
          KeyboardBT.consumerRelease(); 
        } 
        else {
            if (swipe_gesture_range <= 6) {
                ;  // (16.0)
            } else {
                KeyboardBT.consumerPress(KEY_SCAN_NEXT);
                  Serial.printf("Key pressed");
                  KeyboardBT.consumerRelease(); 
            }
        }
    }
  
  //Serial.printf("%f,%f,%i,NoGesture\n", x_range, y_range, swipe_gesture_range);
   
  //Serial.printf("%f, %f, %i, Left Swipe\n", x_range, y_range, swipe_gesture_range);
  
  //Serial.printf("%f, %f, %i, No Gesture\n", x_range, y_range, swipe_gesture_range);
  

  for (int i = 0; i < 15; i++)
    for (int j = 0; j < 3; j++)
      dataPoint[i][j] = 0.0;

}
