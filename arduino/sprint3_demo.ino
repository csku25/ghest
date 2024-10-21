/*
  Project:    Ghest
  Author(s):  K'drian Vega, Zachary Andruchowitz
  Start date: Sept. 30, 2024

  Used Libraries: Adafruit_MPU6050, Adafruit_Sensor
  Credit to: Earle Philhower for Raspberry Pi Pico Arduino Core API - https://github.com/earlephilhower/arduino-pico
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <KeyboardBT.h>

Adafruit_MPU6050 mpu;
bool is_paused = false;
uint8_t button = 28;
uint8_t debounce = 0;
uint8_t debounce_delay = 50;
uint8_t button_state = HIGH;

void setup(void) {
  //start serial communication, baud rate 115,200
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  //set physical pico pins and initialize mpu
  pinMode(button, INPUT_PULLUP);
  KeyboardBT.begin("Ghest Bracelet");

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
  uint8_t reading = digitalRead(button);
  if (reading != button_state) {
    debounce = millis();
  }

  if ((millis() - debounce) > debounce_delay) {
    // Only toggle if the reading is LOW (button pressed)
    if (reading == LOW && button_state == HIGH) {
      is_paused = !is_paused; // Toggle the paused state

      if (is_paused) {
        Serial.println("Pausing data collection...");
      } else {
        Serial.println("Starting data collection...");
        delay(1000);
      }
    }
  }
  button_state = reading;

  if (!is_paused) {
    //create sensor event objects for accelerometer, gyro, and temperature (discard it)
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t discard;
    //get the current sensor readings
    mpu.getEvent(&accel, &gyro, &discard);
    //discard temp readings
    //delay before the next reading
    Serial.print("AccelX:");
    Serial.print(accel.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(accel.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(accel.acceleration.z);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(gyro.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(gyro.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(gyro.gyro.z);
    Serial.println("");
    delay(50);  //in milliseconds

    if (accel.acceleration.z >= 8 && abs(accel.acceleration.x) >= 15) {
      KeyboardBT.consumerPress(KEY_PLAY_PAUSE);
      KeyboardBT.consumerRelease();
      delay(1000); //1-second buffer bewteen gestures
    }
    else if (accel.acceleration.x >= 9 && abs(accel.acceleration.z) >= 15) {
      KeyboardBT.consumerPress(KEY_SCAN_PREVIOUS);
      KeyboardBT.consumerRelease();
      delay(1000); //1-second buffer bewteen gestures
    }
    else if (accel.acceleration.x <= -9 && abs(accel.acceleration.z) >= 15) {
      KeyboardBT.consumerPress(KEY_SCAN_NEXT);
      KeyboardBT.consumerRelease();
      delay(1000); //1-second buffer bewteen gestures
    }
    else if (accel.acceleration.z >= 15 && abs(accel.acceleration.x) >= 5) {
      KeyboardBT.consumerPress(KEY_VOLUME_INCREMENT);
      KeyboardBT.consumerRelease();
      delay(1000); //1-second buffer bewteen gestures
    }
    else if (accel.acceleration.z <= -15 && abs(accel.acceleration.x) >= 5) {
      KeyboardBT.consumerPress(KEY_VOLUME_DECREMENT);
      KeyboardBT.consumerRelease();
      delay(1000); //1-second buffer bewteen gestures
    }

    Serial.printf("Key pressed");
  }
}
