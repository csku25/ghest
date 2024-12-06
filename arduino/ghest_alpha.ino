/*
  Project:    Ghest
  Author(s):  K'drian Vega, Zachary Andruchowitz
  Start date: Sept. 30, 2024

  External Libraries: Adafruit_MPU6050, Adafruit_Sensor, KeyboardBT, LittleFS
  Credit to: Earle Philhower for Raspberry Pi Pico Arduino Core API - https://github.com/earlephilhower/arduino-pico
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <KeyboardBT.h>
#include <LittleFS.h>

Adafruit_MPU6050 mpu;
bool is_paused = false;
uint8_t button = 15; //button pin
uint8_t debounce = 0;
uint8_t debounce_delay = 50;
uint8_t button_state = HIGH;
bool serial_on = true;  //toggle true/false to output serial

/**
 * handle_file_stuff
 *
 * Opens file system allocated in flash memory and zeroes out
 * file buffer, finds json file used for storing gesture mappings,
 * reads data into buffer
 *
 * @param none
 * @return none
 */
void handle_file_stuff() {
  File f = LittleFS.open("defaults.json", "r");
  if (f) {
    size_t file_size = f.size();
    char f_buff[file_size + 1];  //size + null terminator
    int count = 1;
    bzero(f_buff, file_size + 1);  //size + null terminator
    if (f.read((uint8_t *)f_buff, file_size))
      sscanf(f_buff, "%d", &count);
    f.close();

    // for (int idx = 0; idx < file_size; idx++) {
    //   if (serial_on) Serial.print(f_buff[idx]);
    // }
    // if (serial_on) Serial.print("\n");
  }
}

/**
 * classify_gest
 *
 * classifier model built using Weka, takes an x_range,
 * a y_range, and a swipe_gesture_range to compute from
 * the sensor data which type of gesture was given
 *
 * @param x: float, y: float, swipe: int
 * @return string literal: name of classified gesture
 */
string classify_gest(float x, float y, int swipe) {
  if (x <= 5.087169) {
    if (y <= 2.490891) {
      if (x <= 1.232522) {
        if (y <= 0.434467) return "NO GESTURE";
        else return "PLAY/PAUSE";
      } 
      else {
        if (swipe <= -11) {
          if (x <= 1.332046) return "VOLUME DOWN";
          else return "SKIP BACK";
        }
      }
    }
    else {
      if (swipe <= 4) return "VOLUME DOWN";
      else return "VOLUME UP";
    }
  }
  else {  // x_range > 5.087169
    if (swipe <= -2) return "SKIP BACK";
    else if (swipe <= 6) return "NO GESTURE";
    else return "SKIP FORWARD";
  }
}

void setup() {
  if (serial_on) {
    //start serial communication, baud rate 115,200
    Serial.begin(115200);
    while (!Serial);
  }

  //set physical pico pins and initialize mpu
  pinMode(button, INPUT_PULLUP);
  KeyboardBT.begin("Ghest Bracelet");
  LittleFS.begin();
  handle_file_stuff();

  Wire.setSDA(16); //SDA pin from MPU6050
  Wire.setSCL(17); //SCL pin from MPU6050
  if (!mpu.begin()) {
    if (serial_on) Serial.println("Error: Failed to find MPU6050 sensor!");
    while (true);
  }
  if (serial_on) Serial.println("MPU6050 initialized...\n");

  //configure settings
  mpu.setAccelerometerRange(mpu.getAccelerometerRange());  // Set accelerometer range to +/- 2g
  mpu.setGyroRange(mpu.getGyroRange());                    // Set gyroscope range to +/- 250 deg/s
  mpu.setFilterBandwidth(mpu.getFilterBandwidth());        // Set filter bandwidth to 21 Hz

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

  delay(100);
  if (serial_on) Serial.println("ax,ay,az,gx,gy,gz");
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t discard;
}

void loop() {
  uint8_t reading = digitalRead(button);
  if (reading != button_state) {
    debounce = millis();
  }

  if ((millis() - debounce) > debounce_delay) {
    // Only toggle if the reading is LOW (button pressed)
    if (reading == LOW && button_state == HIGH) {
      is_paused = !is_paused;  // Toggle the paused state

      if (is_paused) {
        if (serial_on) Serial.println("Pausing data collection...");
      } else {
        if (serial_on) Serial.println("Starting data collection...");
        delay(1000);
      }
    }
  }
  button_state = reading;

  if (!is_paused) {
    while (count < 15) {
      mpu.getEvent(&accel, &gyro, &discard);
      dataPoint[count][0] = gyro.gyro.x;
      dataPoint[count][1] = gyro.gyro.y;
      dataPoint[count][2] = gyro.gyro.z;
      count = count + 1;
      delay(50);
    }           // end while loop
    count = 0;  //reset count for next loop

    //updating minimum and maximums in the data set
    for (int i = 0; i < 15; i++) {
      if (i == 0) {
        min_x = dataPoint[0][0];
        min_y = dataPoint[0][1];
        min_z = dataPoint[0][2];
        max_x = dataPoint[0][0];
        max_y = dataPoint[0][1];
        max_z = dataPoint[0][2];
      }
      if (dataPoint[i][0] < min_x) {
        min_x = dataPoint[i][0];
        min_x_idx = i;
      }
      if (dataPoint[i][1] < min_y) {
        min_y = dataPoint[i][1];
        min_y_idx = i;
      }
      if (dataPoint[i][2] < min_z) {
        min_z = dataPoint[i][2];
        min_z_idx = i;
      }
      if (dataPoint[i][0] > max_x) {
        max_x = dataPoint[i][0];
        max_x_idx = i;
      }
      if (dataPoint[i][1] > max_y) {
        max_y = dataPoint[i][1];
        max_y_idx = i;
      }
      if (dataPoint[i][2] > max_z) {
        max_z = dataPoint[i][2];
        max_z_idx = i;
      }
    }

    x_range = abs(min_x - max_x);
    y_range = abs(min_y - max_y);
    z_range = abs(min_z - max_z);
    swipe_gesture_range = min_y_idx - max_y_idx;
    //create sensor event objects for accelerometer, gyro, and temperature (discard it)
    //get the current sensor readings
    //discard temp readings
    //delay before the next reading
    if (serial_on) {
      Serial.print("aX:");
      Serial.print(accel.acceleration.x);
      Serial.print(",");
      Serial.print("aY:");
      Serial.print(accel.acceleration.y);
      Serial.print(",");
      Serial.print("aZ:");
      Serial.print(accel.acceleration.z);
      Serial.print(",");
      Serial.print("gX:");
      Serial.print(gyro.gyro.x);
      Serial.print(",");
      Serial.print("gY:");
      Serial.print(gyro.gyro.y);
      Serial.print(",");
      Serial.print("gZ:");
      Serial.print(gyro.gyro.z);
      Serial.println("");
    }
    delay(50);  //in milliseconds

    switch (classify_gest(x_range, y_range, swipe_gesture_range)) {
      case "PLAY/PAUSE":
        KeyboardBT.consumerPress(KEY_PLAY_PAUSE);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: PLAY/PAUSE");
        delay(1500);  //1.5-second buffer bewteen taps
        break;
      case "SKIP BACK":
        KeyboardBT.consumerPress(KEY_SCAN_PREVIOUS);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: SKIP BACK");
        delay(1000);  //1-second buffer bewteen gestures
        break;
      case "SKIP FORWARD":
        KeyboardBT.consumerPress(KEY_SCAN_NEXT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: SKIP FORWARD");
        delay(1000);  //1-second buffer bewteen gestures
        break;
      case "VOLUME UP":
        KeyboardBT.consumerPress(KEY_VOLUME_INCREMENT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: VOLUME UP");
        delay(1000);  //1-second buffer bewteen gestures
        break;
      case "VOLUME DOWN":
        KeyboardBT.consumerPress(KEY_VOLUME_DECREMENT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: VOLUME DOWN");
        delay(1000);  //1-second buffer bewteen gestures
        break;
      case "NO GESTURE":
        break;
      default:
        break;
    }
    
    for (int i = 0; i < 15; i++)
    for (int j = 0; j < 3; j++)
      dataPoint[i][j] = 0.0;
  }
}
